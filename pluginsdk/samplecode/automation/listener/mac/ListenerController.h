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

#ifndef __ListenerController_H__
#define __ListenerController_H__


#import <Cocoa/Cocoa.h>
#include "PIUtilities.h"
#include "PIUI.h"
#include "Listener.h"
#include "ListenerUIMac.h"


OSStatus orderWindowFrontListener(void);
OSStatus initializeCocoaListener();

/* Make sure this is unique to you and everyone you might encounter, search for
 "Preventing Name Conflicts" or use this link
 http://developer.apple.com/mac/library/documentation/UserExperience/Conceptual/PreferencePanes/Tasks/Conflicts.html
 */


// sub class the dialog so all things work
@interface ListenerController : NSObject
{
    IBOutlet NSWindow *listenerWindow;
    
    IBOutlet NSPopUpButton *watchID;
    IBOutlet NSPopUpButton *setID;
    IBOutlet NSPopUpButton *actionID;
    
    IBOutlet NSTextField *notifiersID;
    
    Listener_t *listenerData;
    //NSString *actionName;
    //NSString *actionSet;
}

- (SPErr) initPopups;
- (int) showWindow;
+ (ListenerController *) listenerController;
@end


#endif //__ListenerController_H__
