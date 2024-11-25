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
//		ListenerUIMac.cpp
//
//	Description:
//		This file contains the source code and routines for the
//		Automation module Listener, an example of a module
//		that uses the Actions suite in a persistent
//		Automation plug-in.
//
//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
//	Includes
//-------------------------------------------------------------------------------

#include <Cocoa/Cocoa.h>
#include "PIDefines.h"
#include "Listener.h"
#include "ListenerUIMac.h"
#include "ListenerController.h"


//-------------------------------------------------------------------------------
//
//	DoUI
//
//	Display the Macintosh user interface.
//
//-------------------------------------------------------------------------------
SPErr DoUI (void)
{
    // call function to initialize Cocoa
    OSStatus err = initializeCocoaListener();
    
    // call function to show window
    // it's not an error 1 = ok, 0 = cancel
    if (!err)
    {
        err = orderWindowFrontListener();
        if (err == 1)
            err = kSPNoError;
    }
    else
        err = userCanceledErr; // simulate cancel
    
    sPSUIHooks.Unload();
    
    return err;
}



//-------------------------------------------------------------------------------
//
//	DoAbout
//
//	Displays the About box.
//-------------------------------------------------------------------------------
void DoAbout (SPPluginRef plugin, int dialogID)
{
    NSLog(@"DoAbout is: %d\n",dialogID);
    
    NSAlert *alert = [[NSAlert alloc] init];
    
    [alert addButtonWithTitle:@"OK"];
    
    [alert addButtonWithTitle:@"http://www.adobe.com"];
    
    [alert addButtonWithTitle:@"http://partners.adobe.com"];
    
    [alert setMessageText:@"About Listener"];
    
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


// end ListenerUIMac.cpp
