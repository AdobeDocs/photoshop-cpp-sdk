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

#ifndef __PSCryptor_H_
#define __PSCryptor_H_
#include <stddef.h>

#if !_WIN32
#include <stdint.h>
#endif

#include "PSCryptorAPI.h"

#define kPBKDKeyLength 24 // key length for 3DES key
#define k3DESBlockSize	8	// block size in bytes for 3DES algorithm

// basic type definitions
#if _WIN32
typedef unsigned __int8 u_int8_t;
typedef unsigned int u_int;
typedef unsigned __int32 u_int32_t;
typedef unsigned __int64 u_int64_t;
#else
typedef uint8_t u_int8_t;
typedef uint32_t  u_int;
typedef uint32_t u_int32_t;
typedef uint64_t u_int64_t;
#endif


class PSCryptor
{
	public:
		PSCryptor (const char *pass);	// null terminated string, used by apps that have secure storage for password string
		PSCryptor (const u_int8_t *key, bool flag);	// derived key of kPBKDKeyLength bytes, used by apps that store encrypted keys
		
		~PSCryptor ();
		
		static void DeriveKey( const char *pass, u_int8_t *derived_key );	// null terminated string for pass, kPBKDKeyLength bytes buffer for derived
		
		static size_t GetEncryptedLength (size_t plainDataLen);
		
		PSCryptorStatus EncryptDecrypt (bool encypt, const void* dataIn, size_t dataLen,
								void *dataOut, size_t dataOutAvailable, size_t *dataOutMoved);
		
		
	private:
	
#if _WIN32
		ULONG_PTR fSessionKey;
#endif
	
		u_int8_t fDerivedKey [kPBKDKeyLength];
		
};


int pkcs5_pbkdf2(const char *pass, size_t pass_len, const char *salt, size_t salt_len,
    u_int8_t *key, size_t key_len, u_int rounds);

#endif
