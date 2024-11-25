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
//    File:
//        PIUFile.cpp
//
//    Description:
//        File utilities for alias values acquired from the automation system.
//
//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
//    Includes
//-------------------------------------------------------------------------------
#include "PIUFile.h"
#include "FileUtilities.h"
#undef min
#include <algorithm>    // std::min
#if __PIMac__
#include <Cocoa/Cocoa.h>
#endif

// See Also FileUtilitiesMac.cpp
// Flag for turning off deprecated warnings
// When the File Manager routines go away then set this flag to 0
#ifndef PLUGIN_SUPPORTS_FS_ROUTINES
    #define PLUGIN_SUPPORTS_FS_ROUTINES  1
#endif


void AliasToFullPath(Handle aliasValue, char* fullPath, int32 maxPathLength)
{
    if (fullPath != NULL)
    {
        *fullPath = 0;
        SPBoolean isCloud = false;
        SPErr error = sPSAlias->IsCloudFile((AliasHandle)aliasValue, &isCloud);
        if ( ! error )
        {
            ASZString zString = NULL;
            if (isCloud)
            {
                error = sPSAlias->CloudPathFromAlias((AliasHandle)aliasValue, &zString);
            } else { // not a cloud document
                uint32 length = sPSHandle->GetSize(aliasValue);
                uint16 * unicodePath = new uint16[length];
                error = sPSAlias->AliasToUnicodePath((AliasHandle)aliasValue, unicodePath, length);
                if ( ! error )
                {
                    error = sASZString2->MakeFromUnicode(unicodePath, length, &zString);
                }
            }
            if ( ! error ) 
            {
                ASUInt32 utf8Size = sASZString2->LengthAsUTF8String(zString);
                unsigned char * utf8Buffer = new unsigned char[utf8Size];
                if (!sASZString2->AsUTF8String(zString, utf8Buffer, utf8Size, true /* check string size */)) 
                {
                    size_t minSize = std::min(utf8Size, (ASUInt32)maxPathLength);
                    strncpy(fullPath, (const char *)utf8Buffer, minSize);
                }
                sASZString2->Release(zString);
                delete [] utf8Buffer;
            }
        }
    }
}

void FullPathToAlias(char* fullPath, Handle& aliasValue)
{
    SPErr error = 0;
    aliasValue = NULL;

    int result = strncmp(fullPath, "/cloud-content", 14);

    if (result == 0)
    {
        error = sPSAlias->NewAliasFromCloudPath(fullPath, (AliasHandle*)&aliasValue);
    }
    else
    {
        ASZString zString;
        error = sASZString2->MakeFromUTF8String((const unsigned char*)fullPath, &zString);
        if ( ! error )
        {
            ASUInt32 unicodeLength = sASZString2->LengthAsUnicodeCString(zString);
            ASUnicode * unicodeStr = new ASUnicode[unicodeLength + 1];
            sASZString->AsUnicodeCString(zString, unicodeStr, unicodeLength, false);
            error = sPSAlias->UnicodePathToAlias(unicodeStr, (AliasHandle*)&aliasValue);
        }
    }
}


void BookmarkToFullPath(CFDataRef bookmarkValue, char* fullPath, int32 maxPathLength)
{
#ifdef __PIMac__
    if (fullPath != NULL)
        {
        *fullPath = 0;

        CFURLBookmarkResolutionOptions options = kCFBookmarkResolutionWithoutUIMask |
                                                 kCFBookmarkResolutionWithoutMountingMask;
        Boolean       bookmarkIsStale = false;
        CFURLRef      relativeToURL = NULL;    // if security-scoped to a document, this needs to be the doc URL
        CFErrorRef    cfError = 0;
        CFURLRef urlRef = CFURLCreateByResolvingBookmarkData(kCFAllocatorDefault,
                                                             bookmarkValue,
                                                             options,
                                                             relativeToURL,
                                                             NULL,
                                                             &bookmarkIsStale,
                                                             &cfError);
        
        if (urlRef == NULL)
            NSLog(@"Could not resolve bookmark in BookmarkToFullPath");

        bool converted = CFURLGetFileSystemRepresentation(urlRef, true, (UInt8*)fullPath, maxPathLength);
        
        if ( ! converted )
            NSLog(@"Could not convert in BookmarkToFullPath");

        if (urlRef != NULL)
            CFRelease(urlRef);
        }
#else
    UNREFERENCED_PARAMETER(maxPathLength);
    UNREFERENCED_PARAMETER(fullPath);
    UNREFERENCED_PARAMETER(bookmarkValue);
#endif
}

void FullPathToBookmark(char* fullPath, CFDataRef& bookmarkValue)
{
#if __PIMac__

    bookmarkValue = NULL;
    
    CFURLRef urlRef = CFURLCreateFromFileSystemRepresentation(kCFAllocatorDefault,
                                                              (const UInt8*)fullPath,
                                                              strlen(fullPath),
                                                              false); // Boolean isDirectory

    if (urlRef == NULL)
    {
        NSLog(@"Could not create url bookmark in FullPathToBookmark");
        return;
    }
    
    CFURLBookmarkCreationOptions options = 0;
    CFErrorRef    error = 0;
    bookmarkValue = CFURLCreateBookmarkData(kCFAllocatorDefault,
                                            urlRef,
                                            options,
                                            NULL, /* CFArrayRef resourcePropertiesToInclude */
                                            NULL, /* CFURLRef relativeToURL */
                                            &error);
    
    if (urlRef != NULL)
        CFRelease(urlRef);

#else
    UNREFERENCED_PARAMETER(bookmarkValue);
    UNREFERENCED_PARAMETER(fullPath);
#endif
}


int32 GetFullPathToDesktop(char * fullPath, int32 maxPathLength)
{
    int32 error = 0;
    if (fullPath == NULL || maxPathLength < 1) return kSPBadParameterError;
    
    #if __PIMac__

        // Get array with first index being path to desktop
        NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDesktopDirectory, NSUserDomainMask, YES);

        // Get the first element
        NSString *desktopPath = [paths objectAtIndex:0];

        [desktopPath getCString:fullPath maxLength:maxPathLength encoding:NSUTF8StringEncoding];

        if (PIstrlcat(fullPath, "/", maxPathLength) >= maxPathLength)
            error = kSPBadParameterError;
        fullPath[maxPathLength-1]= '\0';
    
    #else
        
        if (MAX_PATH <= maxPathLength)
        {
            HRESULT hr = SHGetFolderPath( NULL, CSIDL_DESKTOPDIRECTORY, NULL, 0, fullPath );
            if (FAILED(hr))
            {
                fullPath[0] = 0;
                error = kSPBadParameterError;
            }
            else
            {
                if (PIstrlcat(fullPath, "\\", maxPathLength) >= (size_t)maxPathLength)
                    error = kSPBadParameterError;
                fullPath[maxPathLength-1]= '\0';
            }
        } else {
            error = kSPBadParameterError;
        }

    #endif
    
    return error;
}



// end PIUFile.cpp
