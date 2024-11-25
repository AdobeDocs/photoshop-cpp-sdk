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

// External APIs for encryption/decryption of client/server traffic
#include <stddef.h>

typedef enum
{
	kCryptorSuccess = 0,
	kCryptorBufferTooSmall = 1,
	kCryptorAlignmentError = 2,
	kCryptorMemFail = 3,
	kCryptorDecodeErr = 4,
	kCryptorEncodeErr = 5,
	kCryptorParamErr = 6,
	kCryptorUnknownErr = 999
} PSCryptorStatus;

typedef void * PSCryptorRef;

#ifdef __cplusplus
extern "C" {
#endif
PSCryptorRef CreatePSCryptor (const char *password);
void DestroyPSCryptor (PSCryptorRef cryptor);
PSCryptorStatus EncryptDecrypt (PSCryptorRef cryptor, bool encrypt, const void* dataIn, size_t dataLen,
					void *dataOut, size_t dataOutAvailable, size_t *dataOutMoved);
size_t CryptorGetEncryptedLength (size_t plainDataLen);
#ifdef __cplusplus
} // extern "C"
#endif
