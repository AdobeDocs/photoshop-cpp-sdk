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
//		OutboundUIMac.cpp
//
//	Copyright 1990-1992, Thomas Knoll.
//
//	Description:
//		This file contains the user interface source
//		for the Export module Outbound, a module that
//		creates a file and stores raw pixel data in it.
//
//	Use:
//		This module shows how to export raw data to a file.
//		It uses a simple "FileUtilities" library that comes
//		with the SDK.  You use it via File>>Export>>Outbound.
//
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
//	Includes
//-------------------------------------------------------------------------------

#include "PIDefines.h"
#include "Outbound.h"
#include "PIUFile.h"
#include "FileUtilities.h"

//-------------------------------------------------------------------------------
//	Definitions -- Dialog items
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
//
//	DoAbout
//
//	Displays the About box.
//
//	Inputs:
//		AboutRecordPtr about	Is actually a pointer to some platform
//								specific data for Windows only.
//		AboutID					#define with ID of String resource to use for text
//
//	Outputs:
//		Displays About box (Dialog).  Press Escape, Enter, or Return to
//		dismiss, or click the mouse.
//
//-------------------------------------------------------------------------------

/*****************************************************************************/

void DoAbout (AboutRecordPtr /*about*/)
{
	ShowAbout (AboutID);
}

/*****************************************************************************/

Boolean DoUI (GPtr globals)
{
    
    if (gQueryForParameters)
    {

        NSLog(@"Outbound DoUI start");

        NSSavePanel *panel = [NSSavePanel savePanel];
    
        [panel setNameFieldStringValue:@"Outbound.exp"];
    
        BOOL result = [panel runModal];
    
		if (result == NSModalResponseOK)
        {
            NSString * panelDirectory = [[panel directoryURL] path];
            NSLog(@"Outbound DoUI directory = %@", panelDirectory);
    
            gFileName = [[panel URL] path];
            NSLog(@"Outbound DoUI filename = %@", gFileName);
    
            NSFileHandle *output = [NSFileHandle fileHandleForWritingAtPath:gFileName];

            if (output == nil)
            {
                BOOL madeIt = [[NSFileManager defaultManager] createFileAtPath:gFileName contents:nil attributes:nil];
                if (madeIt)
                    output = [NSFileHandle fileHandleForWritingAtPath:gFileName];
            }
            else
            {
                [output truncateFileAtOffset:0];
            }
    
            gFRefNum = output;
    
            if (output != nil)
                return TRUE;
        }
    }
    else // if (gQueryForParameters)
    {

		// first try the bookmark
	
		BOOL bookmarkIsStale = false;
		NSError * nsError = NULL;
    	NSURL * url = [NSURL URLByResolvingBookmarkData:(__bridge NSData*)gBookMarkData options:NSURLBookmarkResolutionWithoutUI| NSURLBookmarkResolutionWithoutMounting relativeToURL:nil bookmarkDataIsStale:&bookmarkIsStale error:&nsError];
	
		gFileName = [url path];
		NSLog(@"Outbound else DoUI filename = %@", gFileName);
	
		NSFileHandle *output = [NSFileHandle fileHandleForWritingAtPath:gFileName];

		if (output == nil)
		{
			BOOL madeIt = [[NSFileManager defaultManager] createFileAtPath:gFileName contents:nil attributes:nil];
			if (madeIt)
				output = [NSFileHandle fileHandleForWritingAtPath:gFileName];
		}
		else
		{
			[output truncateFileAtOffset:0];
		}
	
		gFRefNum = output;
	
		if (output != nil)
			return TRUE;
	
		// second try the alias
	
		char fullPath[1024];
		CFStringRef pathString = NULL;
        fullPath[0] = 0;
        FSRef fsRef;
        Boolean changed = false;
        OSErr resolveErr = PSSDKResolveAlias(NULL, gAliasHandle, &fsRef, &changed);
        if (resolveErr == noErr || resolveErr == fnfErr)
        {
            HFSUniStr255		targetName;
            HFSUniStr255		volumeName;
            FSAliasInfoBitmap	whichInfo;
            FSAliasInfo			aliasInfo;
        
            resolveErr = PSSDKCopyAliasInfo(gAliasHandle, &targetName, &volumeName, &pathString, &whichInfo, &aliasInfo);
            if (resolveErr == noErr)
            {
                if (CFStringGetCString(pathString, fullPath, 1024,kCFStringEncodingUTF8))
                {
                    gFileName = [[NSString alloc] initWithUTF8String:(const char *)fullPath];
                    NSFileHandle *output = [NSFileHandle fileHandleForWritingAtPath:gFileName];
                    
                    if (output == nil)
                    {
                        BOOL madeIt = [[NSFileManager defaultManager] createFileAtPath:gFileName contents:nil attributes:nil];
                        if (madeIt)
                            output = [NSFileHandle fileHandleForWritingAtPath:gFileName];
                    }
                    else
                    {
                        [output truncateFileAtOffset:0];
                    }
                    
                    gFRefNum = output;
                    
                    if (pathString != NULL)
                        CFRelease(pathString);
                
                    if (output != nil)
                        return TRUE;
				
                } // if (CFStringGetCString(pathString, fullPath, 1024,kCFStringEncodingUTF8))
			
            } // if (resolveErr == noErr)

        } // if (resolveErr == noErr || resolveErr == fnfErr)
   
    } // if (gQueryForParameters)

    return FALSE;

}

/*****************************************************************************/
// On the mac the savePanel opened it up, just check that we have something
// See DoUI for Outbound example on mac
Boolean CreateExportFile (GPtr globals)
{
    if (gFRefNum)
        return  TRUE;
	return FALSE;
}

/*****************************************************************************/

Boolean CloseExportFile (GPtr globals)
{
    [gFRefNum closeFile];

    gFRefNum = nil;

    gStuff->dirty = FALSE;

	if (gBookMarkData)
	{
		CFRelease(gBookMarkData);
		gBookMarkData = nil;
	}

	gAliasHandle = nil;

    if (gFileName)
    {

        CFURLRef fileURL = (CFURLRef)[[NSURL alloc] initFileURLWithPath:gFileName isDirectory:FALSE];

        gFileName = nil;

        if (fileURL)
        {
		
			// first make a bookmark
		
			CFErrorRef errorRef = 0;
			CFURLBookmarkCreationOptions options = 0;
			gBookMarkData = CFURLCreateBookmarkData(kCFAllocatorDefault, fileURL, options, NULL, NULL, &errorRef);
	
			// second make an alias, might fail in future versions of the OS
	
            FSRef	fsRef;
		
            if (PSSDKCFURLGetFSRef(fileURL, &fsRef))
            {
                AliasHandle	temp;
                OSErr error = PSSDKNewAliasMinimal(&fsRef, &temp);
                if (error == noErr)
                {
                    gAliasHandle = temp;
                }
            }
		
            CFRelease(fileURL);
		
        }
    }

	return gAliasHandle || gBookMarkData ? TRUE : FALSE;
	
}
//-------------------------------------------------------------------------------
// end OutboundUIMac.cpp
