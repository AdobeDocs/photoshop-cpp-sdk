// ADOBE SYSTEMS INCORPORATED
// Copyright  2015 Adobe Systems Incorporated
// All Rights Reserved
//
// NOTICE:  Adobe permits you to use, modify, and distribute this
// file in accordance with the terms of the Adobe license agreement
// accompanying it.  If you have received this file from a source
// other than Adobe, then your use, modification, or distribution
// of it requires the prior written permission of Adobe.
//-------------------------------------------------------------------------------
#import "ShapeController.h"

ShapeController *gShapeController = NULL;


/* Make sure this is unique to you and everyone you might encounter, search for
 "Preventing Name Conflicts" or use this link
 http://developer.apple.com/mac/library/documentation/UserExperience/Conceptual/PreferencePanes/Tasks/Conflicts.html
 */

// controller for the entire dialog
@implementation ShapeController

+ (ShapeController *) shapeController
{
    return gShapeController;
}


- (id) init: (GPtr) globals
{
    self = [super init];
	
    NSBundle * plugin = [NSBundle bundleForClass:[self class]];

    if (![plugin loadNibNamed:@"ShapeDialog" owner:self topLevelObjects:nil])
    {
        NSLog(@"Shape failed to load ShapeDialog xib");
    }
    
    gShapeController = self;
    globData = globals;
    
    [self updateShapeOption:gWhatShape];
    [self updateCreateOption:gCreate];
    
    NSLog(@"Shape Done with init");
    
    return self;
}

- (int) showWindow
{
    [shapeWindow makeKeyAndOrderFront:nil];
    int b = (int)[[NSApplication sharedApplication] runModalForWindow:shapeWindow];
    [shapeWindow orderOut:self];
    return b;
}

- (void) updateShapeOption: (int) shapeOpt
{
    [shapeTriangleID setState:false];
    [shapeSquareID setState:false];
    [shapeCircleID setState:false];
    [shapeStarID setState:false];
    [shapeTrebleID setState:false];
    [shapeRibbonID setState:false];
    [shapeNoteID setState:false];
    
    switch (shapeOpt)
    {
        case iShapeTriangle:
        default:
            [shapeTriangleID setState:true];
            break;
        case iShapeSquare:
            [shapeSquareID setState:true];
            break;
        case iShapeCircle:
            [shapeCircleID setState:true];
            break;
        case iShapeStar:
            [shapeStarID setState:true];
            break;
        case iShapeTreble:
            [shapeTrebleID setState:true];
            break;
        case iShapeRibbon:
            [shapeRibbonID setState:true];
            break;
        case iShapeNote:
            [shapeNoteID setState:true];
            break;
    }
    
    shapeOption = shapeOpt;
}

- (void) updateCreateOption: (int) createOpt
{
    [createSelectionID setState:false];
    [createLayerID setState:false];
    [createPathID setState:false];
    
    switch (createOpt)
    {
        case iCreateSelection:
        default:
            [createSelectionID setState:true];
            break;
        case iCreateMaskpath:
            [createPathID setState:true];
            break;
        case iCreateLayer:
            [createLayerID setState:true];
            break;
    }
    
    createOption = createOpt;
}

- (IBAction) okPressed:(id)sender
{
    NSLog(@"Shape OK pressed");
    globData->whatShape = shapeOption;
    globData->create = createOption;
    [NSApp stopModalWithCode:1];
    NSLog(@"Shape after nsapp stopmodal");
}

- (IBAction) cancelPressed:(id)sender
{
    NSLog(@"Shape cancel pressed");
    [NSApp stopModalWithCode:0];
    NSLog(@"Shape after nsapp abortmodal");
}

- (IBAction) sTrianglePressed: (id) sender
{
    NSLog(@"Shape Triangle pressed");
    shapeOption = iShapeTriangle;
}

- (IBAction) sSquarePressed: (id) sender
{
    NSLog(@"Shape Square pressed");
    shapeOption = iShapeSquare;
}

- (IBAction) sCirclePressed: (id) sender
{
    NSLog(@"Shape Circle pressed");
    shapeOption = iShapeCircle;
}

- (IBAction) sStarPressed: (id) sender
{
    NSLog(@"Shape Star pressed");
    shapeOption = iShapeStar;
}

- (IBAction) sTreblePressed: (id) sender
{
    NSLog(@"Shape Treble pressed");
    shapeOption = iShapeTreble;
}

- (IBAction) sRibbonPressed: (id) sender
{
    NSLog(@"Shape Ribbon pressed");
    shapeOption = iShapeRibbon;
}

- (IBAction) sNotePressed: (id) sender
{
    NSLog(@"Shape Note pressed");
    shapeOption = iShapeNote;
}

- (IBAction) cSelectionPressed: (id) sender
{
    NSLog(@"Shape Selection pressed");
    createOption = iCreateSelection;
}

- (IBAction) cPathPressed: (id) sender
{
    NSLog(@"Shape Path pressed");
    createOption = iCreateMaskpath;
}

- (IBAction) cLayerPressed: (id) sender
{
    NSLog(@"Shape Layer pressed");
    createOption = iCreateLayer;
}
@end

/* Carbon entry point and C-callable wrapper functions*/
OSStatus initializeCocoaShape(GPtr globals)
{
    [[ShapeController alloc] init:globals];
    return noErr;
}

OSStatus orderWindowFrontShape(void)
{
    OSStatus okPressed = [[ShapeController shapeController] showWindow];
    return okPressed;
}

// end ShapeController.m
