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

#import "GradientImportController.h"

GradientImportController *gGradientImportController = NULL;

/* Make sure this is unique to you and everyone you might encounter, search for
 "Preventing Name Conflicts" or use this link
 http://developer.apple.com/mac/library/documentation/UserExperience/Conceptual/PreferencePanes/Tasks/Conflicts.html
 */
// get the current value and force an update
@implementation GImportTextField

- (void)keyUp:(NSEvent *)theEvent
{
    NSLog(@"GradientImport start keyUp, %d", [theEvent keyCode]);
    [gGradientImportController validate];
}

@end


// controller for the entire dialog
@implementation GradientImportController

+ (GradientImportController *) gradientImportController
{
    return gGradientImportController;
}


- (id) init: (GPtr) globals
{
    self = [super init];
	
	NSBundle * plugin = [NSBundle bundleForClass:[self class]];
	
    if (![plugin loadNibNamed:@"GradientImportDialog" owner:self topLevelObjects:nil])
    {
        NSLog(@"GradientImport failed to load GradientImportDialog xib");
    }
    
    gGradientImportController = self;
    globData = globals;
    
    [rowsID setStringValue:[NSString stringWithFormat:@"%d", gLastRows]];
    [colsID setStringValue:[NSString stringWithFormat:@"%d", gLastCols]];
    
    [invertID setState:(gLastInvert && gStuff->canReadBack)];
    [convertID setState: gConvertAlpha];
    
    if (gStuff->canReadBack)
        [invertID setEnabled: true];
    else
        [invertID setEnabled: false];
    
    [bitmapID setState: false];
    [grayscaleID setState: false];
    [indexedID setState: false];
    [rgbID setState: false];
    
    switch (globData->lastMode) {
        case plugInModeBitmap:
            [bitmapID setState: true];
            break;
 
        case plugInModeGrayScale:
            [grayscaleID setState: true];
            break;
            
        case plugInModeIndexedColor:
            [indexedID setState: true];
            break;
            
        default:
        case plugInModeRGBColor:
            [rgbID setState: true];
            break;
    }
    
    [importID setKeyEquivalent:@"\r"];
    [doneID setKeyEquivalent:@"\E"];
    state = STATE_IDLE;
    
    NSLog(@"GradientImport Done with init");

    return self;
}

- (int) showWindow
{
    [gradientImportWindow makeKeyAndOrderFront:nil];
    int b = (int)[[NSApplication sharedApplication] runModalForWindow:gradientImportWindow];
    [gradientImportWindow orderOut:self];
    return b;
}

- (IBAction) bitmapPressed: (id) sender
{
    NSLog(@"GradientImport bitmap pressed");
    mode = plugInModeBitmap;
}

- (IBAction) grayScalePressed: (id) sender
{
    NSLog(@"GradientImport grayscale pressed");
    mode = plugInModeGrayScale;
}

- (IBAction) indexedPressed: (id) sender
{
    NSLog(@"GradientImport indexed pressed");
    mode = plugInModeIndexedColor;
}

- (IBAction) rgbPressed: (id) sender
{
    NSLog(@"GradientImport rgb pressed");
    mode = plugInModeRGBColor;
}

- (IBAction) importPressed:(id)sender
{
    NSLog(@"GradientImport import pressed");
    if ([self validate])
    {
        globData->lastRows = [rowsID intValue];
        globData->lastCols = [colsID intValue];
    
        globData->lastMode = mode;
    
        globData->lastInvert = [invertID state];
        globData->convertAlpha = [convertID state];
    
        state = STATE_AGAIN;
        [NSApp stopModalWithCode:1];
    }
}

- (IBAction) donePressed:(id)sender
{
    NSLog(@"GradientImport done pressed");
    state = STATE_CANCEL;
    [NSApp stopModalWithCode:0];
}

- (bool) validate
{
    int rowsVal = [rowsID intValue];
    int colsVal = [rowsID intValue];
    
    if (rowsVal < kRowsMin || rowsVal > kRowsMax)
    {
        NSAlert *alert = [[NSAlert alloc] init];
        [alert addButtonWithTitle:@"OK"];
        
        NSString *alertStr = [NSString stringWithFormat:@"An integer is required between %d and %d", kRowsMin, kRowsMax];
        [alert setMessageText: alertStr];
		[alert setAlertStyle:NSAlertStyleWarning];
        
        if ([alert runModal] == NSAlertFirstButtonReturn)
        {
            [rowsID setStringValue:[NSString stringWithFormat:@"%d", 1]];
        }
        [alert release];
        return false;
    }
        
    if (colsVal < kColumnsMin || colsVal > kColumnsMax)
    {
        NSAlert *alert = [[NSAlert alloc] init];
        [alert addButtonWithTitle:@"OK"];
        NSString *alertStr = [NSString stringWithFormat:@"An integer is required between %d and %d", kColumnsMin, kColumnsMax];
        [alert setMessageText: alertStr];
		[alert setAlertStyle:NSAlertStyleWarning];
        
        if ([alert runModal] == NSAlertFirstButtonReturn)
        {
            [colsID setStringValue:[NSString stringWithFormat:@"%d", 1]];
        }
        [alert release];
        return false;
    }

    return true;
}

- (int) getState
{
    return state;
}
@end

/* Carbon entry point and C-callable wrapper functions*/
OSStatus initializeCocoaGImport(GPtr globals)
{
    [[GradientImportController alloc] init:globals];
    return noErr;
}

OSStatus orderWindowFrontGImport(void)
{
    int okPressed = [[GradientImportController gradientImportController] showWindow];
    return okPressed;
}

int getGImportState()
{
    return [[GradientImportController gradientImportController] getState];
}
// end GradientImportController.m
