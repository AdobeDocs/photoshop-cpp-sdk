/**************************************************************************
 ADOBE SYSTEMS INCORPORATED
 Copyright 2010 Adobe Systems Incorporated
 All Rights Reserved.
 
 NOTICE:  Adobe permits you to use, modify, and distribute this file
 in accordance with the terms of the Adobe license agreement accompanying
 it.  If you have received this file from a source other than Adobe, then
 your use, modification, or distribution of it requires the prior written
 permission of Adobe.
 **************************************************************************/


#if _WIN32
#include <winsock2.h>
#include <WinCrypt.h>
#endif

#include "PSCryptor.h"

#include <string.h>

#if _WIN32

PSCryptorStatus MapPlatformStatusToPSCryptorStatus (DWORD status)
{
	switch (status)
	{
		case NTE_BAD_LEN: return kCryptorBufferTooSmall;
		case NTE_NO_MEMORY: return kCryptorMemFail;
		case ERROR_INVALID_PARAMETER: return kCryptorParamErr;

		default:
			return kCryptorUnknownErr;
	}
}
#else
#include <CommonCrypto/CommonCryptor.h>
PSCryptorStatus MapPlatformStatusToPSCryptorStatus (CCCryptorStatus status)
{
	switch (status)
	{
		case kCCParamError: return kCryptorParamErr;
		case kCCBufferTooSmall: return kCryptorBufferTooSmall;
		case kCCMemoryFailure: return kCryptorMemFail;
		case kCCAlignmentError: return kCryptorAlignmentError;

		default: 
			return kCryptorUnknownErr;
	}
}
#endif

#if _WIN32
// BrSulliv 12/21/2010: Begin code copied from Microsoft Support Article http://support.microsoft.com/kb/q228786/ 
// This is Microsoft-specific code to work around the limitation that you can't directly access key buffers in MS Crypto API
// ***************************************************************************************************

BOOL CreatePrivateExponentOneKey(LPTSTR szProvider, 
                                 DWORD dwProvType,
                                 LPTSTR szContainer,
                                 DWORD dwKeySpec,
                                 HCRYPTPROV *hProv, 
                                 HCRYPTKEY *hPrivateKey)
{
   BOOL fReturn = FALSE;
   BOOL fResult;
   int n;
   LPBYTE keyblob = NULL;
   DWORD dwkeyblob;
   DWORD dwBitLen;
   BYTE *ptr;

   __try
   {
      *hProv = 0;
      *hPrivateKey = 0;

      if ((dwKeySpec != AT_KEYEXCHANGE) && (dwKeySpec != AT_SIGNATURE))  __leave;

      // Try to create new container
      fResult = CryptAcquireContext(hProv, szContainer, szProvider, 
                                    dwProvType, CRYPT_NEWKEYSET);
      if (!fResult)
      {
         // If the container exists, open it
         if (GetLastError() == NTE_EXISTS)
         {
            fResult = CryptAcquireContext(hProv, szContainer, szProvider, dwProvType, 0);
            if (!fResult)
            {
               // No good, leave
               __leave;
            }
         }
         else
         {
            // No good, leave
            __leave;
         }
      }

      // Generate the private key
      fResult = CryptGenKey(*hProv, dwKeySpec, CRYPT_EXPORTABLE, hPrivateKey);
      if (!fResult) __leave;

      // Export the private key, we'll convert it to a private
      // exponent of one key
      fResult = CryptExportKey(*hPrivateKey, 0, PRIVATEKEYBLOB, 0, NULL, &dwkeyblob);
      if (!fResult) __leave;      

      keyblob = (LPBYTE)LocalAlloc(LPTR, dwkeyblob);
      if (!keyblob) __leave;

      fResult = CryptExportKey(*hPrivateKey, 0, PRIVATEKEYBLOB, 0, keyblob, &dwkeyblob);
      if (!fResult) __leave;


      CryptDestroyKey(*hPrivateKey);
      *hPrivateKey = 0;

      // Get the bit length of the key
      memcpy(&dwBitLen, &keyblob[12], 4);      

      // Modify the Exponent in Key BLOB format
      // Key BLOB format is documented in SDK

      // Convert pubexp in rsapubkey to 1
      ptr = &keyblob[16];
      for (n = 0; n < 4; n++)
      {
         if (n == 0) ptr[n] = 1;
         else ptr[n] = 0;
      }

      // Skip pubexp
      ptr += 4;
      // Skip modulus, prime1, prime2
      ptr += (dwBitLen/8);
      ptr += (dwBitLen/16);
      ptr += (dwBitLen/16);

      // Convert exponent1 to 1
      for (n = 0; n < (dwBitLen/16); n++)
      {
         if (n == 0) ptr[n] = 1;
         else ptr[n] = 0;
      }

      // Skip exponent1
      ptr += (dwBitLen/16);

      // Convert exponent2 to 1
      for (n = 0; n < (dwBitLen/16); n++)
      {
         if (n == 0) ptr[n] = 1;
         else ptr[n] = 0;
      }

      // Skip exponent2, coefficient
      ptr += (dwBitLen/16);
      ptr += (dwBitLen/16);

      // Convert privateExponent to 1
      for (n = 0; n < (dwBitLen/8); n++)
      {
         if (n == 0) ptr[n] = 1;
         else ptr[n] = 0;
      }
      
      // Import the exponent-of-one private key.      
      if (!CryptImportKey(*hProv, keyblob, dwkeyblob, 0, 0, hPrivateKey))
      {                 
         __leave;
      }

      fReturn = TRUE;
   }
   __finally
   {
      if (keyblob) LocalFree(keyblob);

      if (!fReturn)
      {
         if (*hPrivateKey) CryptDestroyKey(*hPrivateKey);
         if (*hProv) CryptReleaseContext(*hProv, 0);
      }
   }

   return fReturn;
}

BOOL ImportPlainSessionBlob(HCRYPTPROV hProv,
                            HCRYPTKEY hPrivateKey,
                            ALG_ID dwAlgId,
                            LPBYTE pbKeyMaterial ,
                            DWORD dwKeyMaterial ,
                            HCRYPTKEY *hSessionKey)
{
   BOOL fResult;   
   BOOL fReturn = FALSE;
   BOOL fFound = FALSE;
   LPBYTE pbSessionBlob = NULL;
   DWORD dwSessionBlob, dwSize, n;
   DWORD dwPublicKeySize;
   DWORD dwProvSessionKeySize;
   ALG_ID dwPrivKeyAlg;
   LPBYTE pbPtr; 
   DWORD dwFlags = CRYPT_FIRST;
   PROV_ENUMALGS_EX ProvEnum;
   HCRYPTKEY hTempKey = 0;

   __try
   {
      // Double check to see if this provider supports this algorithm
      // and key size
      do
      {        
         dwSize = sizeof(ProvEnum);
         fResult = CryptGetProvParam(hProv, PP_ENUMALGS_EX, (LPBYTE)&ProvEnum,
                                     &dwSize, dwFlags);
         if (!fResult) break;

         dwFlags = 0;

         if (ProvEnum.aiAlgid == dwAlgId) fFound = TRUE;
                                     
      } while (!fFound);

      if (!fFound) __leave;

      // We have to get the key size(including padding)
      // from an HCRYPTKEY handle.  PP_ENUMALGS_EX contains
      // the key size without the padding so we can't use it.
      fResult = CryptGenKey(hProv, dwAlgId, 0, &hTempKey);
      if (!fResult) __leave;
      
      dwSize = sizeof(DWORD);
      fResult = CryptGetKeyParam(hTempKey, KP_KEYLEN, (LPBYTE)&dwProvSessionKeySize,
                                 &dwSize, 0);
      if (!fResult) __leave;      
      CryptDestroyKey(hTempKey);
      hTempKey = 0;

      // Our key is too big, leave
      if ((dwKeyMaterial * 8) > dwProvSessionKeySize) __leave;

      // Get private key's algorithm
      dwSize = sizeof(ALG_ID);
      fResult = CryptGetKeyParam(hPrivateKey, KP_ALGID, (LPBYTE)&dwPrivKeyAlg, &dwSize, 0);
      if (!fResult) __leave;

      // Get private key's length in bits
      dwSize = sizeof(DWORD);
      fResult = CryptGetKeyParam(hPrivateKey, KP_KEYLEN, (LPBYTE)&dwPublicKeySize, &dwSize, 0);
      if (!fResult) __leave;

      // calculate Simple blob's length
      dwSessionBlob = (dwPublicKeySize/8) + sizeof(ALG_ID) + sizeof(BLOBHEADER);

      // allocate simple blob buffer
      pbSessionBlob = (LPBYTE)LocalAlloc(LPTR, dwSessionBlob);
      if (!pbSessionBlob) __leave;

      pbPtr = pbSessionBlob;

      // SIMPLEBLOB Format is documented in SDK
      // Copy header to buffer
      ((BLOBHEADER *)pbPtr)->bType = SIMPLEBLOB;
      ((BLOBHEADER *)pbPtr)->bVersion = 2;
      ((BLOBHEADER *)pbPtr)->reserved = 0;
      ((BLOBHEADER *)pbPtr)->aiKeyAlg = dwAlgId;
      pbPtr += sizeof(BLOBHEADER);

      // Copy private key algorithm to buffer
      *((DWORD *)pbPtr) = dwPrivKeyAlg;
      pbPtr += sizeof(ALG_ID);

      // Place the key material in reverse order
      for (n = 0; n < dwKeyMaterial; n++)
      {
         pbPtr[n] = pbKeyMaterial[dwKeyMaterial-n-1];
      }
     
      // 3 is for the first reserved byte after the key material + the 2 reserved bytes at the end.
      dwSize = dwSessionBlob - (sizeof(ALG_ID) + sizeof(BLOBHEADER) + dwKeyMaterial + 3);
      pbPtr += (dwKeyMaterial+1);

      // Generate random data for the rest of the buffer
      // (except that last two bytes)
      fResult = CryptGenRandom(hProv, dwSize, pbPtr);
      if (!fResult) __leave;

      for (n = 0; n < dwSize; n++)
      {
         if (pbPtr[n] == 0) pbPtr[n] = 1;
      }

      pbSessionBlob[dwSessionBlob - 2] = 2;

      fResult = CryptImportKey(hProv, pbSessionBlob , dwSessionBlob, 
                               hPrivateKey, CRYPT_EXPORTABLE, hSessionKey);
      if (!fResult) __leave;

      fReturn = TRUE;           
   }
   __finally
   {
      if (hTempKey) CryptDestroyKey(hTempKey);
      if (pbSessionBlob) LocalFree(pbSessionBlob);
   }
   
   return fReturn;
}

HCRYPTKEY GetSessionKey(const u_int8_t *key)
{
	// Now that we have key bytes, we have to put them into an HCRYPTKEY session key.

	HCRYPTPROV hProv = 0;
	HCRYPTKEY hPubPrivKey = 0;
	HCRYPTKEY hSessionKey = 0;

	// Create a private "exponent-of-one" key to store our key bytes; this is the hack provided by Microsoft
    if (!CreatePrivateExponentOneKey(MS_ENHANCED_PROV, PROV_RSA_FULL,
                                     L"Adobe Photoshop", AT_KEYEXCHANGE, 
                                     &hProv, &hPubPrivKey))
	{
		return 0;
	}

    // Import this key and get an HCRYPTKEY handle. Note that we have specified a 3DES key type here.
    if (!ImportPlainSessionBlob(hProv, hPubPrivKey, CALG_3DES, (LPBYTE) key, kPBKDKeyLength, &hSessionKey))
    {
		return 0;
    }

	return hSessionKey;
}
#endif

PSCryptor::PSCryptor (const char *password)
{
	memset (fDerivedKey, 0, sizeof (fDerivedKey));

	const char* salt = "Adobe Photoshop";
	u_int iterations = 1000;

	// We use the PKCS#5 PBKDF2 standard to create raw key bytes from a password string.
	// PBKDF2 requires us to specify a salt value and an iteration count. These values must be consistent across
	// all implementations of this code, even when it's ported to other operating systems.
	// We choose "Adobe Photoshop" as the salt and 1000 iterations. 

	// We need to tell PBKDF2 how many bytes of key data to generate. Since we're going to use 3DES as the encryption
	// algorithm, we choose 24 bytes (the size of a 3DES key). If we change the key algorithm in the future, we will
	// also have to change this value as well.

	// Call the pkcs5_pbkdf2 function taken from OpenBSD (public domain license) to perform the key byte generation
	pkcs5_pbkdf2(password, (size_t)strlen(password), salt, (size_t)strlen(salt), fDerivedKey, kPBKDKeyLength, iterations );

#if _WIN32
	fSessionKey = GetSessionKey (fDerivedKey);
#endif
}

/*********************************************************************************************************************/
PSCryptor::~PSCryptor ()
{
#if _WIN32
	if (fSessionKey)
		CryptDestroyKey (fSessionKey);
#endif
}

/*********************************************************************************************************************/
size_t PSCryptor::GetEncryptedLength (size_t plainDataLen)
{
	size_t encryptedDataLen = plainDataLen;
	size_t remainder = plainDataLen % k3DESBlockSize;
	encryptedDataLen += (k3DESBlockSize - remainder);

	return encryptedDataLen;
}

/*********************************************************************************************************************/
PSCryptorStatus PSCryptor::EncryptDecrypt (bool encrypt, const void* dataIn, size_t dataLen,
								void *dataOut, size_t dataOutAvailable, size_t *dataOutMoved)
{

#if _WIN32
	if (fSessionKey == NULL)
		return kCryptorUnknownErr; // for lack of better error

	BOOL result = 0;

	// if encrypting or decrypting not in place
	if (dataIn != dataOut)
		memcpy (dataOut, dataIn, dataLen);

	DWORD pdwDataLen = (DWORD) dataLen;

	if (encrypt)
		{
		result = CryptEncrypt ( fSessionKey,
								NULL,
								TRUE,
								0,
								(LPBYTE) dataOut,
								&pdwDataLen,
								(DWORD) dataOutAvailable);

		}
	else
		{
		result = CryptDecrypt ( fSessionKey,
								NULL,
								TRUE,
								0,
								(LPBYTE) dataOut,
								&pdwDataLen);
		}

	if (!result)
		return MapPlatformStatusToPSCryptorStatus (GetLastError ());

	*dataOutMoved = (size_t) pdwDataLen;
	return kCryptorSuccess;
#else
	CCOperation op = (encrypt ? kCCEncrypt : kCCDecrypt);

	CCCryptorStatus status = CCCrypt ( op,
									   kCCAlgorithm3DES,
									   kCCOptionPKCS7Padding,
									   (const void *) fDerivedKey,
									   kPBKDKeyLength,
									   NULL,
									   dataIn,
									   dataLen,
									   dataOut,
									   dataOutAvailable,
									   dataOutMoved);

	if (status == kCCSuccess)
		return kCryptorSuccess;

	return MapPlatformStatusToPSCryptorStatus (status);
#endif

}
/*********************************************************************************************************************/
extern "C" {
// Wrapper Functions for external consumption
PSCryptorRef CreatePSCryptor (const char *password)
{
	PSCryptor *cryptor = new PSCryptor (password);
	return (PSCryptorRef) cryptor;
}
void DestroyPSCryptor (PSCryptorRef cryptor)
{
	if (cryptor)
	{
		PSCryptor *psCrypt = (PSCryptor *) cryptor;
		delete psCrypt;
	}

}
PSCryptorStatus EncryptDecrypt (PSCryptorRef cryptor, bool encrypt, const void* dataIn, size_t dataLen,
					void *dataOut, size_t dataOutAvailable, size_t *dataOutMoved)
{
	if (cryptor)
	{
		PSCryptor *psCrypt = (PSCryptor *) cryptor;
		return psCrypt->EncryptDecrypt (encrypt, dataIn, dataLen, dataOut, dataOutAvailable, dataOutMoved);
	}

	return kCryptorParamErr;
}

size_t CryptorGetEncryptedLength (size_t plainDataLen)
{
	return PSCryptor::GetEncryptedLength (plainDataLen);
}

}
/*********************************************************************************************************************/

