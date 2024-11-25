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

#ifndef __GradientImportController_H__
#define __GradientImportController_H__


#import <Cocoa/Cocoa.h>
#include "PIUtilities.h"
#include "GradientImport.h"


#define STATE_IDLE      0
#define STATE_AGAIN     1
#define STATE_CANCEL    -1

OSStatus orderWindowFrontGImport(void);
OSStatus initializeCocoaGImport(GPtr globals);
void closeGImport(void);
int getGImportState(void);

/* Make sure this is unique to you and everyone you might encounter, search for
 "Preventing Name Conflicts" or use this link
 http://developer.apple.com/mac/library/documentation/UserExperience/Conceptual/PreferencePanes/Tasks/Conflicts.html
 */

// sub class the text field so proxy updates occur on each key
@interface GImportTextField : NSTextField
{
}
- (void) keyUp: (NSEvent *) theEvent;
@end

// sub class the dialog so all things work
@interface GradientImportController : NSObject
{
    IBOutlet NSWindow *gradientImportWindow;

    IBOutlet NSButton *importID;
    IBOutlet NSButton *doneID;
    IBOutlet GImportTextField *rowsID;
    IBOutlet GImportTextField *colsID;
    
    IBOutlet NSButtonCell *bitmapID;
    IBOutlet NSButtonCell *grayscaleID;
    IBOutlet NSButtonCell *indexedID;
    IBOutlet NSButtonCell *rgbID;
    
    IBOutlet NSButton *invertID;
    IBOutlet NSButton *convertID;
    
    NSString* rows;
    NSString* cols;
    int mode;
    
    GPtr globData;
    int state;
}

- (int) getState;
- (int) showWindow;
- (bool) validate;

+ (GradientImportController *) gradientImportController;
@end


#endif //__GradientImportController_H__
