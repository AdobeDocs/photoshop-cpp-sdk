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

#ifndef __SelectoramaController_H__
#define __SelectoramaController_H__


#import <Cocoa/Cocoa.h>
#include "Selectorama.h"


OSStatus orderWindowFrontSelectorama(void);
OSStatus initializeCocoaSelectorama(GPtr globals);

/* Make sure this is unique to you and everyone you might encounter, search for
 "Preventing Name Conflicts" or use this link
 http://developer.apple.com/mac/library/documentation/UserExperience/Conceptual/PreferencePanes/Tasks/Conflicts.html
 */


// sub class the dialog so all things work
@interface SelectoramaController : NSObject
{
    IBOutlet NSWindow *selectoramaWindow;
    
    IBOutlet NSButtonCell *areaMinID;
    IBOutlet NSButtonCell *areaMaxID;
    IBOutlet NSButtonCell *areaRandomID;

    IBOutlet NSButtonCell *channelTargetID;
    IBOutlet NSButtonCell *channelMergedID;
    
    IBOutlet NSButtonCell *createSelectionID;
    IBOutlet NSButtonCell *createPathID;
    IBOutlet NSButtonCell *createLayerID;
    
    int areaOption;
    int channelOption;
    int createOption;
    
    GPtr globData;
}

- (int) showWindow;
- (void) updateAreaOption:(int) areaOpt;
- (void) updateChannelOption:(int) channelOpt;
- (void) updateCreateOption:(int) createOpt;
+ (SelectoramaController *) selectoramaController;
@end


#endif //__SelectoramaController_H__
