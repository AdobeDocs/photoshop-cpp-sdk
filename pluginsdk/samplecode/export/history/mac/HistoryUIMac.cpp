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
//		HistoryUIMac.cpp
//
//	Copyright 1990-1992, Thomas Knoll.
//
//	Description:
//		This file contains the user interface routines
//		for the Export module History, a module that
//		displays and lets the user manipulate
//		pseudo-resources of type 'hist'.
//
//	Use:
//		This module shows how to examine, display, and work
//		with pseudo-resources.  An additional element might
//		be to have it export those remaining pseudo-resources
//		to a text file.
//
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
//	Includes
//-------------------------------------------------------------------------------

#include <Cocoa/Cocoa.h>
#include "PIDefines.h"
#include "History.h"
#include "HistoryController.h"


//-------------------------------------------------------------------------------
//	Prototypes
//-------------------------------------------------------------------------------

void UpdateHistories (GPtr globals,
					  void* dp,
					  short count,
					  Str255 hS);

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

void DoAbout (AboutRecordPtr /*about*/)
{
    NSLog(@"DoAbout is: %d\n",AboutID);
    
    NSAlert *alert = [[NSAlert alloc] init];
    
    [alert addButtonWithTitle:@"OK"];
    
    [alert addButtonWithTitle:@"http://www.adobe.com"];
    
    [alert addButtonWithTitle:@"http://partners.adobe.com"];
    
    [alert setMessageText:@"About History"];
    
    [alert setInformativeText:@"An example plug-in export module for Adobe Photoshop."];
    
	[alert setAlertStyle:NSAlertStyleWarning];
    
    int buttonPressed = (int)[alert runModal];
    
    if ( buttonPressed == NSAlertFirstButtonReturn) {
        NSLog(@"DoAbout ok clicked.\n");
    } else if (buttonPressed == NSAlertSecondButtonReturn) {
        sPSFileList->BrowseUrl("http://www.adobe.com");
    } else if (buttonPressed == NSAlertThirdButtonReturn) {
        sPSFileList->BrowseUrl("http://partners.adobe.com");
    }
    
    [alert release];
} // end DoAbout

//-------------------------------------------------------------------------------

Boolean DoUI (GPtr globals)
{
    // call function to initialize Cocoa
    OSStatus err = initializeCocoaHistory(globals);
    
    // call function to show window
    // it's not an error 1 = ok, 0 = cancel
    if (!err)
        err = orderWindowFrontHistory();
    else
        err = 0; // simulate cancel
    
    sPSUIHooks.Unload();
    
    return err;
}

//-------------------------------------------------------------------------------

// end HistoryUIMac.cpp
