// ADOBE SYSTEMS INCORPORATED
// Copyright  2009 Adobe Systems Incorporated
// All Rights Reserved
//
// NOTICE:  Adobe permits you to use, modify, and distribute this 
// file in accordance with the terms of the Adobe license agreement
// accompanying it.  If you have received this file from a source
// other than Adobe, then your use, modification, or distribution
// of it requires the prior written permission of Adobe.
//-------------------------------------------------------------------------------
//	Includes
//-------------------------------------------------------------------------------

#include "PIDefines.h"
#include "Dissolve.h"
#include "DissolveUI.h"
#include "DialogUtilities.h"
#include "FilterBigDocument.h"

#include <Cocoa/Cocoa.h>
#include "DissolveController.h"


//-------------------------------------------------------------------------------
//
// DoUI
//
// Pop our UI, the UI is using the globals and the person calling this is responsible
// to reset the globals to last used accordingly
//
// Return: 1 for ok (true) and 0 for cancel
//-------------------------------------------------------------------------------
extern DissolveController *gDissolveController;

Boolean DoUI (void) 
{
	// call function to initialize Cocoa
    OSStatus err = initializeCocoaDissolve();

	// call function to show window
	// it's not an error 1 = ok, 0 = cancel
    if (!err)
		err = orderWindowFrontDissolve();
	else
		err = 0; // simulate cancel

	sPSUIHooks.Unload();
	
	return err;
}

//-------------------------------------------------------------------------------
//
// DoAbout
//
// Pop a simple about box for this plug in.
//
// NOTE:	The global gFilterRecord is NOT a FilterRecord*. You must cast it to
//			an AboutRecord*. See PIAbout.h
//
//-------------------------------------------------------------------------------
void DoAbout(void)
{
	NSLog(@"DoAbout is: %d\n",AboutID);

	NSAlert *alert = [[NSAlert alloc] init];
	
	[alert addButtonWithTitle:@"OK"];
	
	[alert addButtonWithTitle:@"http://www.adobe.com"];
	
	[alert addButtonWithTitle:@"http://partners.adobe.com"];

	[alert setMessageText:@"About Dissolve"];
	
	[alert setInformativeText:@"An example plug-in filter module for Adobe Photoshop."];
	
	[alert setAlertStyle:NSAlertStyleWarning];
	
	long buttonPressed = [alert runModal];
	
	if ( buttonPressed == NSAlertFirstButtonReturn) {
		NSLog(@"DoAbout ok clicked.\n");
	} else if (buttonPressed == NSAlertSecondButtonReturn) {
		sPSFileList->BrowseUrl("http://www.adobe.com");
	} else if (buttonPressed == NSAlertThirdButtonReturn) {
		sPSFileList->BrowseUrl("http://partners.adobe.com");
	}

	[alert release];
}

// end DissolveUIMacCocoa.cpp 
