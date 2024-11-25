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
//		GradientImportUIMac.cpp
//
//	Copyright 1996-1997, Adobe Systems Incorporated.
//	All Rights Reserved.
//
//	Description:
//		This file contains the dialog and platform-specific functions
//		for the Import module Idle, a very simple example of a module
//		that just idles and sends update events.
//
//	Use:
//		This import module is good as an example of a module
//		that forces the background application to redraw
//		its windows.
//
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
//	Includes
//-------------------------------------------------------------------------------

#include "PIDefines.h"
#include <Cocoa/Cocoa.h>
#include "GradientImport.h"
#include "GradientImportController.h"

//-------------------------------------------------------------------------------
//	Definitions -- Dialog items
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
//	Prototypes.
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
void DoAbout (AboutRecordPtr /*about*/)
{
    NSLog(@"DoAbout is: %d\n",AboutID);
    
    NSAlert *alert = [[NSAlert alloc] init];
    
    [alert addButtonWithTitle:@"OK"];
    
    [alert addButtonWithTitle:@"http://www.adobe.com"];
    
    [alert addButtonWithTitle:@"http://partners.adobe.com"];
    
    [alert setMessageText:@"About GradientImport"];
    
    [alert setInformativeText:@"An example plug-in import module for Adobe Photoshop."];
    
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


Boolean OpenOurDialog (GPtr globals)
{
    OSStatus err = initializeCocoaGImport(globals);
    
    // call function to show window
    // it's not an error 1 = ok, 0 = cancel
    if (!err)
        err = orderWindowFrontGImport();
    else
        err = 0; // simulate cancel

    sPSUIHooks.Unload();
    
    return true;
}

/*****************************************************************************/

Boolean RunOurDialog (GPtr globals)
{
    int state = getGImportState();
    if (state == STATE_AGAIN)
        return true;
    else
        return false;
}

/*****************************************************************************/

void CloseOurDialog (GPtr globals)
{
}

/*****************************************************************************/

// end GradientImportUIMac.cpp
