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
//		ShapeUIMac.cpp
//
//	Copyright 1996-1997, Adobe Systems Incorporated.
//	All Rights Reserved.
//
//	Description:
//		This file contains the source and functions for the
//		UI for the Import module Shape, which returns a path, layer,
//		or selection with an interesting shape.
//
//	Use:
//		This module specifically works the path return
//		functionality of Photoshop.  The Paths are stored
//		by derezing a work path saved in Photoshop then
//		storing them in a "Path" resource, which is a
//		binary data resource of a "Path" as defined in the
//		"Path layout" section of the Photosop File Format.
//
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
//	Includes
//-------------------------------------------------------------------------------

#include "PIDefines.h"
#include <Cocoa/Cocoa.h>
#include "Shape.h"
#include "ShapeController.h"


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

void DoAboutShape(AboutRecordPtr about)
{
    NSLog(@"DoAbout is: %d\n",AboutID);
    
    NSAlert *alert = [[NSAlert alloc] init];
    
    [alert addButtonWithTitle:@"OK"];
    
    [alert addButtonWithTitle:@"http://www.adobe.com"];
    
    [alert addButtonWithTitle:@"http://partners.adobe.com"];
    
    [alert setMessageText:@"About Shape"];
    
    [alert setInformativeText:@"An example plug-in filter module for Adobe Photoshop."];
    
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
}

//-------------------------------------------------------------------------------
//
//	DoUI
//
//	Displays the Parameters dialog
//
//	Inputs:
//		GPtr globals		Pointer to global structure.
//
//	Outputs:
//		Displays parameters dialog.
//
//		returns TRUE		If displaying dialog succeeded, user picked an
//							option, and clicked OK.
//		returns FALSE		If displaying dialog failed or user clicked CANCEL.
//
//-------------------------------------------------------------------------------

Boolean DoUIShape (GPtr globals)
{
    // call function to initialize Cocoa
    OSStatus err = initializeCocoaShape(globals);
    
    // call function to show window
    // it's not an error 1 = ok, 0 = cancel
    if (!err)
        err = orderWindowFrontShape();
    else
        err = 0; // simulate cancel
    
    sPSUIHooks.Unload();
    
    return err;
}
//-------------------------------------------------------------------------------

// end ShapeUIMac.cpp
