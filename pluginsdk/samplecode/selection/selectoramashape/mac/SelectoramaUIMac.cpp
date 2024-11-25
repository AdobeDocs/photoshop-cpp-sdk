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
//		SelectoramaUIMac.cpp
//
//
//	Description:
//		This file contains the dialog and platform-specific functions
//		for the Selection module Selectorama, an example module
//		that just returns a pixel selection.
//
//	Use:
//		Use selection modules to return pixel or path selections on a new
//		layer or the current layer.
//
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
//	Includes
//-------------------------------------------------------------------------------

#include "PIDefines.h"
#include <Cocoa/Cocoa.h>
#include "Selectorama.h"
#include "SelectoramaController.h"

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
    
    [alert setMessageText:@"About Selectorama"];
    
    [alert setInformativeText:@"An example plug-in selection module for Adobe Photoshop."];
    
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

/*****************************************************************************/

/* Setup the parameters dialog.  Returns TRUE if it succeeds.				 */

Boolean DoUI (GPtr globals)
{
    // call function to initialize Cocoa
    OSStatus err = initializeCocoaSelectorama(globals);
    
    // call function to show window
    // it's not an error 1 = ok, 0 = cancel
    if (!err)
        err = orderWindowFrontSelectorama();
    else
        err = 0; // simulate cancel
    
    sPSUIHooks.Unload();
    
    return err;
}

/*********************************************************************/
// end SelectoramaUIMac.cpp
