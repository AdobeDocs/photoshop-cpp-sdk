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

#ifndef __ShapeController_H__
#define __ShapeController_H__


#import <Cocoa/Cocoa.h>
#include "Shape.h"


OSStatus orderWindowFrontShape(void);
OSStatus initializeCocoaShape(GPtr globals);

/* Make sure this is unique to you and everyone you might encounter, search for
 "Preventing Name Conflicts" or use this link
 http://developer.apple.com/mac/library/documentation/UserExperience/Conceptual/PreferencePanes/Tasks/Conflicts.html
 */


// sub class the dialog so all things work
@interface ShapeController : NSObject
{
    IBOutlet NSWindow *shapeWindow;
    
    IBOutlet NSButtonCell *shapeTriangleID;
    IBOutlet NSButtonCell *shapeSquareID;
    IBOutlet NSButtonCell *shapeCircleID;
    IBOutlet NSButtonCell *shapeStarID;
    IBOutlet NSButtonCell *shapeTrebleID;
    IBOutlet NSButtonCell *shapeRibbonID;
    IBOutlet NSButtonCell *shapeNoteID;
    
    IBOutlet NSButtonCell *createSelectionID;
    IBOutlet NSButtonCell *createPathID;
    IBOutlet NSButtonCell *createLayerID;
    
    int shapeOption;
    int createOption;
    
    GPtr globData;
}

- (int) showWindow;
- (void) updateShapeOption:(int) shapeOpt;
- (void) updateCreateOption:(int) createOpt;
+ (ShapeController *) shapeController;
@end


#endif //__ShapeController_H__
