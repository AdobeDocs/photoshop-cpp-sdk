// ADOBE SYSTEMS INCORPORATED
// Copyright  1993 - 2002 Adobe Systems Incorporated
// All Rights Reserved
//
// NOTICE:  Adobe permits you to use, modify, and distribute this 
// file in accordance with the terms of the Adobe license agreement
// accompanying it.  If you have received this file from a source
// other than Adobe, then your use, modification, or distribution
// of it requires the prior written permission of Adobe.
//-------------------------------------------------------------------
//-------------------------------------------------------------------------------
//
//	File:
//		FileUtilitiesMac.cpp
//
//	Description:
//		This library contains sources and functions
//		to simplify the use of extremely common file
//		functions for plug-ins. 
//
//	Use:
//		Macintosh routines with common names. 
//
//-------------------------------------------------------------------------------

#include "PIDefines.h"
#include "FileUtilities.h"
#include <Cocoa/Cocoa.h>

/*****************************************************************************/

// MacOS SDK have marked many File Manager routines deprecated
// Attempt to load them from CoreServices framework and use accordingly

/*****************************************************************************/

typedef OSErr (* P_FSSetForkPosition_proc) (FSIORefNum forkRefNum, UInt16 positionMode, SInt64 positionOffset);
typedef OSErr (* P_FSWriteFork_proc) (FSIORefNum forkRefNum, UInt16 positionMode, SInt64 positionOffset, ByteCount requestCount, const void *buffer, ByteCount *actualCount);
typedef OSErr (* P_FSReadFork_proc) (FSIORefNum forkRefNum, UInt16 positionMode, SInt64 positionOffset, ByteCount requestCount, void *buffer, ByteCount *actualCount);
typedef OSErr (* P_FSResolveAlias_proc) (const FSRef * fromFile, AliasHandle alias, FSRef * target, Boolean * wasChanged);
typedef OSStatus (* P_FSRefMakePath_proc) (const FSRef *ref, UInt8 *path, UInt32 pathBufferSize);
typedef OSStatus (* P_FSCopyAliasInfo_proc) (AliasHandle inAlias, HFSUniStr255 * targetName, HFSUniStr255 * volumeName, CFStringRef * pathString, FSAliasInfoBitmap * whichInfo, FSAliasInfo * info);
typedef OSErr (* P_FSNewAliasMinimal_proc) (const FSRef * target, AliasHandle * inAlias);
typedef Boolean (* P_CFURLGetFSRef_proc) (CFURLRef url, struct FSRef *fsRef);

/*****************************************************************************/

static CFBundleRef		  		sCoreBundleRef = NULL;

/*****************************************************************************/

static P_FSSetForkPosition_proc sFSSetForkPosition_func = NULL;
static P_FSWriteFork_proc 		sFSWriteFork_func = NULL;
static P_FSReadFork_proc 		sFSReadFork_func = NULL;
static P_FSResolveAlias_proc	sFSResolveAlias_func = NULL;
static P_FSRefMakePath_proc		sFSRefMakePath_func = NULL;
static P_FSCopyAliasInfo_proc	sFSCopyAliasInfo_func = NULL;
static P_FSNewAliasMinimal_proc sFSNewAliasMinimal_func = NULL;
static P_CFURLGetFSRef_proc		sCFURLGetFSRef_func = NULL;

/*****************************************************************************/

OSErr LoadRuntimeFunctions(void)
{
	if (sCoreBundleRef &&
	    sFSSetForkPosition_func &&
	    sFSWriteFork_func &&
	    sFSReadFork_func &&
	    sFSResolveAlias_func &&
	    sFSRefMakePath_func &&
	    sFSCopyAliasInfo_func &&
	    sFSNewAliasMinimal_func &&
	    sCFURLGetFSRef_func)
		return noErr;
	
	OSErr err = noErr;
	CFURLRef bundleURL = NULL;
	if (sCoreBundleRef == NULL)
	{
		CFURLRef baseURL = NULL;
		NSError *urlError = nil;
		NSURL *url = [[NSFileManager defaultManager] URLForDirectory:NSLibraryDirectory
		                                             inDomain:NSSystemDomainMask
		                                             appropriateForURL:NULL
		                                             create:false
		                                             error:&urlError];
		if (urlError == nil)
		{
			url = [NSURL URLWithString:@"Frameworks/" relativeToURL:url];
			baseURL = (__bridge CFURLRef)url;
		}
	
		if (baseURL == nil)
		{
			err = coreFoundationUnknownErr;
		}
		else
		{
			bundleURL = (::CFURLCreateCopyAppendingPathComponent(::kCFAllocatorSystemDefault,
																 baseURL,
																 CFSTR("CoreServices.framework"),
																 false));
			if (bundleURL == nil)
			{
				err = coreFoundationUnknownErr;
			}
			else
			{
				sCoreBundleRef = (::CFBundleCreate(::kCFAllocatorSystemDefault, bundleURL));
			}
		}
	}
	
	if (sCoreBundleRef && sFSWriteFork_func == NULL)
	{
		sFSWriteFork_func = (P_FSWriteFork_proc) CFBundleGetFunctionPointerForName(sCoreBundleRef, CFSTR("FSWriteFork"));
		if (sFSWriteFork_func == NULL)
		{
			err = coreFoundationUnknownErr;
		}
	}
	
	if (sCoreBundleRef && sFSSetForkPosition_func == NULL)
	{
		sFSSetForkPosition_func = (P_FSSetForkPosition_proc) CFBundleGetFunctionPointerForName(sCoreBundleRef, CFSTR("FSSetForkPosition"));
		if (sFSSetForkPosition_func == NULL)
		{
			err = coreFoundationUnknownErr;
		}
	}
	
	if (sCoreBundleRef && sFSReadFork_func == NULL)
	{
		sFSReadFork_func = (P_FSReadFork_proc) CFBundleGetFunctionPointerForName(sCoreBundleRef, CFSTR("FSReadFork"));
		if (sFSReadFork_func == NULL)
		{
			err = coreFoundationUnknownErr;
		}
	}
	
	if (sCoreBundleRef && sFSResolveAlias_func == NULL)
	{
		sFSResolveAlias_func = (P_FSResolveAlias_proc) CFBundleGetFunctionPointerForName(sCoreBundleRef, CFSTR("FSResolveAlias"));
		if (sFSResolveAlias_func == NULL)
		{
			err = coreFoundationUnknownErr;
		}
	}

	if (sCoreBundleRef && sFSRefMakePath_func == NULL)
	{
		sFSRefMakePath_func = (P_FSRefMakePath_proc) CFBundleGetFunctionPointerForName(sCoreBundleRef, CFSTR("FSRefMakePath"));
		if (sFSRefMakePath_func == NULL)
		{
			err = coreFoundationUnknownErr;
		}
	}
	
	if (sCoreBundleRef && sFSCopyAliasInfo_func == NULL)
	{
		sFSCopyAliasInfo_func = (P_FSCopyAliasInfo_proc) CFBundleGetFunctionPointerForName(sCoreBundleRef, CFSTR("FSCopyAliasInfo"));
		if (sFSCopyAliasInfo_func == NULL)
		{
			err = coreFoundationUnknownErr;
		}
	}
	
	if (sCoreBundleRef && sFSNewAliasMinimal_func == NULL)
	{
		sFSNewAliasMinimal_func = (P_FSNewAliasMinimal_proc) CFBundleGetFunctionPointerForName(sCoreBundleRef, CFSTR("FSNewAliasMinimal"));
		if (sFSNewAliasMinimal_func == NULL)
		{
			err = coreFoundationUnknownErr;
		}
	}

	if (sCoreBundleRef && sCFURLGetFSRef_func == NULL)
	{
		sCFURLGetFSRef_func = (P_CFURLGetFSRef_proc) CFBundleGetFunctionPointerForName(sCoreBundleRef, CFSTR("CFURLGetFSRef"));
		if (sCFURLGetFSRef_func == NULL)
		{
			err = coreFoundationUnknownErr;
		}
	}
	
	if (err && sCoreBundleRef != NULL)
	{
		CFRelease(sCoreBundleRef);
		sCoreBundleRef = NULL;
	}
	
	if (bundleURL)
	{
		CFRelease(bundleURL);
		bundleURL = NULL;
	}
	
	return err;
}

/*****************************************************************************/

void UnLoadRuntimeFunctions(void)
{
	sFSSetForkPosition_func = NULL;
	sFSWriteFork_func = NULL;
	sFSReadFork_func = NULL;
	sFSResolveAlias_func = NULL;
	sFSRefMakePath_func = NULL;
	sFSCopyAliasInfo_func = NULL;
	sFSNewAliasMinimal_func = NULL;
	sCFURLGetFSRef_func = NULL;

	
	if (sCoreBundleRef)
	{
		CFRelease(sCoreBundleRef);
		sCoreBundleRef = NULL;
	}
}

/*****************************************************************************/

OSErr PSSDKReadFork (FSIORefNum forkRefNum, UInt16 positionMode, SInt64 positionOffset, ByteCount requestCount, void *buffer, ByteCount *actualCount)
{
	OSErr err = LoadRuntimeFunctions();
	if (err == noErr && sFSReadFork_func)
	{
		err = sFSReadFork_func(forkRefNum, positionMode, positionOffset, requestCount, buffer, actualCount);
	}
	return err;
}

/*****************************************************************************/

OSErr PSSDKWriteFork (FSIORefNum forkRefNum, UInt16 positionMode, SInt64 positionOffset, ByteCount requestCount, const void *buffer, ByteCount *actualCount)
{
	OSErr err = LoadRuntimeFunctions();
	if (err == noErr && sFSWriteFork_func)
	{
		err = sFSWriteFork_func(forkRefNum, positionMode, positionOffset, requestCount, buffer, actualCount);
	}
	return err;
}

/*****************************************************************************/

OSErr PSSDKSetForkPosition (FSIORefNum forkRefNum, UInt16 positionMode, SInt64 positionOffset)
{
	OSErr err = LoadRuntimeFunctions();
	if (err == noErr && sFSSetForkPosition_func)
	{
		err = sFSSetForkPosition_func(forkRefNum, positionMode, positionOffset);
	}
	return err;
}

/*****************************************************************************/

OSErr PSSDKResolveAlias (const FSRef * fromFile, AliasHandle alias, FSRef * target, Boolean * wasChanged)
{
	OSErr err = LoadRuntimeFunctions();
	if (err == noErr && sFSResolveAlias_func)
	{
		err = sFSResolveAlias_func(fromFile, alias, target, wasChanged);
	}
	return err;
}

/*****************************************************************************/

OSStatus PSSDKRefMakePath (const FSRef *ref, UInt8 *path, UInt32 pathBufferSize)
{
	OSErr err = LoadRuntimeFunctions();
	if (err == noErr && sFSRefMakePath_func)
	{
		err = sFSRefMakePath_func(ref, path, pathBufferSize);
	}
	return err;
}

/*****************************************************************************/

OSStatus PSSDKCopyAliasInfo (AliasHandle inAlias, HFSUniStr255 * targetName, HFSUniStr255 * volumeName, CFStringRef * pathString, FSAliasInfoBitmap * whichInfo, FSAliasInfo * info)
{
	OSErr err = LoadRuntimeFunctions();
	if (err == noErr && sFSCopyAliasInfo_func)
	{
		err = sFSCopyAliasInfo_func(inAlias, targetName, volumeName, pathString, whichInfo, info);
	}
	return err;
}

/*****************************************************************************/

OSErr PSSDKNewAliasMinimal (const FSRef * target, AliasHandle * inAlias)
{
	OSErr err = LoadRuntimeFunctions();
	if (err == noErr && sFSNewAliasMinimal_func)
	{
		err = sFSNewAliasMinimal_func(target, inAlias);
	}
	return err;
}

/*****************************************************************************/

Boolean PSSDKCFURLGetFSRef(CFURLRef url, struct FSRef *fsRef)
{
	OSErr err = LoadRuntimeFunctions();
	if (err || sCFURLGetFSRef_func == NULL)
	{
		return FALSE;
	}
	return sCFURLGetFSRef_func(url, fsRef);
}

/*****************************************************************************/

OSErr PSSDKWrite(FileHandle refNum, int32 * count, void * buffPtr)
{
    if (count == NULL || buffPtr == NULL || *count <= 0)
        return writErr;
	
    unsigned long long cPos = [refNum offsetInFile];
	
    NSData * nsData = [NSData dataWithBytes:buffPtr length:*count];
	
    [refNum writeData:nsData];
	
    unsigned long long nPos = [refNum offsetInFile];
	
    if (*count != (nPos - cPos))
        return writErr;
	
    return 0;
}

/*****************************************************************************/

OSErr PSSDKWrite(int32 refNum, int32 refFD, int16 usePOSIXIO, int32 * count, void * buffPtr)
{
    if (NULL == count || NULL == buffPtr)
        return writErr;
    
    ByteCount bytes = *count;
    
    ByteCount bCount = *count;
    
    OSErr err = noErr;
    
    if (usePOSIXIO)
    {
        bCount = write(refFD, buffPtr, *count);
    }
    else
    {
       	err = PSSDKWriteFork(refNum, fsAtMark | noCacheMask, 0, bytes, buffPtr, &bCount);
	}
	
    if (bytes != bCount)
        return writErr;

    if (noErr != err)
        return err;
    
    *count = (int32)bCount;
    
    return err;
    
}

/*****************************************************************************/

OSErr PSSDKSetFPos(int32 refNum, int32 refFD, int16 usePOSIXIO, short posMode, long posOff)
{
    OSErr err = noErr;
    
    if (usePOSIXIO)
    {
        int whence = SEEK_SET;
        if (posMode == fsFromLEOF)
        {
            whence = SEEK_END;
        }
        off_t result = lseek(refFD, posOff, whence);

        if (result == -1)
        {
            err = controlErr;
        }
    }
    else
    {
    	err = PSSDKSetForkPosition(refNum, posMode, posOff);
    }

	return err;
}

/*****************************************************************************/

OSErr PSSDKRead(int32 refNum, int32 refFD, int16 usePOSIXIO, int32 * count, void * buffPtr)
{
	if (NULL == count || NULL == buffPtr)
		return writErr;

	ByteCount bytes = *count;

	ByteCount bCount = *count;

	OSErr err = noErr;
    
    if (usePOSIXIO)
    {
        bCount = read(refFD, buffPtr, *count);
    }
    else
    {
    	err = PSSDKReadFork(refNum, fsAtMark, 0, bytes, buffPtr, &bCount);
    }

	if (bytes != bCount)
		return readErr;
		
	*count = (int32)bCount;

	return err;
}

// end FileUtilitiesMac.cpp
