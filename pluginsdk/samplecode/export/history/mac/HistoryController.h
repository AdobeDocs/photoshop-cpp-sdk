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

#ifndef __HistoryController_H__
#define __HistoryController_H__


#import <Cocoa/Cocoa.h>
#include "PIUtilities.h"
#include "History.h"


OSStatus orderWindowFrontHistory(void);
OSStatus initializeCocoaHistory(GPtr globals);

/* Make sure this is unique to you and everyone you might encounter, search for
 "Preventing Name Conflicts" or use this link
 http://developer.apple.com/mac/library/documentation/UserExperience/Conceptual/PreferencePanes/Tasks/Conflicts.html
 */


// sub class the dialog so all things work
@interface HistoryController : NSObject
{
    IBOutlet NSWindow *historyWindow;

    IBOutlet NSButton *trimFirstID;
    IBOutlet NSButton *trimLastID;
    IBOutlet NSButton *upID;
    IBOutlet NSButton *downID;
    IBOutlet NSTextField *textID;
    
    int currentResources;
    Str255 hS;
    GPtr globData;
}

- (int) showWindow;
- (void) CheckAndEnable;
- (void) UpdateHistories;
+ (HistoryController *) historyController;
@end


#endif //__HistoryController_H__
