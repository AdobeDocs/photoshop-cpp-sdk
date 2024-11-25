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
#import "SelectoramaController.h"

SelectoramaController *gSelectoramaController = NULL;


/* Make sure this is unique to you and everyone you might encounter, search for
 "Preventing Name Conflicts" or use this link
 http://developer.apple.com/mac/library/documentation/UserExperience/Conceptual/PreferencePanes/Tasks/Conflicts.html
 */

// controller for the entire dialog
@implementation SelectoramaController

+ (SelectoramaController *) selectoramaController
{
    return gSelectoramaController;
}


- (id) init: (GPtr) globals
{
    self = [super init];
	
	NSBundle * plugin = [NSBundle bundleForClass:[self class]];
    
    if (![plugin loadNibNamed:@"SelectoramaDialog" owner:self topLevelObjects:nil])
    {
        NSLog(@"Selectorama failed to load SelectoramaDialog xib");
    }
    
    gSelectoramaController = self;
    globData = globals;
    
    [self updateAreaOption:gWhatArea];
    [self updateChannelOption:gWhatChannels];
    [self updateCreateOption:gCreate];
    
    NSLog(@"Selectorama Done with init");

    return self;
}

- (int) showWindow
{
    [selectoramaWindow makeKeyAndOrderFront:nil];
    int b = (int)[[NSApplication sharedApplication] runModalForWindow:selectoramaWindow];
    [selectoramaWindow orderOut:self];
    return b;
}

- (void) updateAreaOption: (int) areaOpt
{
    [areaMinID setState:false];
    [areaMaxID setState:false];
    [areaRandomID setState:false];
    
    switch (areaOpt)
    {
        case iSelectMin:
        default:
            [areaMinID setState:true];
            break;
        case iSelectMax:
            [areaMaxID setState:true];
            break;
        case iSelectRandom:
            [areaRandomID setState:true];
            break;
    }
    
    areaOption = areaOpt;
}

- (void) updateChannelOption: (int) channelOpt
{
    switch (channelOpt)
    {
        case iUseTarget:
        default:
            [channelTargetID setState:true];
            [channelMergedID setState:false];
            break;
        case iUseMerged:
            [channelTargetID setState:false];
            [channelMergedID setState:true];
            break;
    }
    
    channelOption = channelOpt;
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
    NSLog(@"Selectorama OK pressed");
    globData->whatArea = areaOption;
    globData->whatChannels = channelOption;
    globData->create = createOption;
    [NSApp stopModalWithCode:1];
    NSLog(@"Selectorama after nsapp stopmodal");
}

- (IBAction) cancelPressed:(id)sender
{
    NSLog(@"Selectorama cancel pressed");
    [NSApp stopModalWithCode:0];
    NSLog(@"Selectorama after nsapp abortmodal");
}

- (IBAction) aMinPressed: (id) sender
{
    NSLog(@"Selectorama Minimum pressed");
    areaOption = iSelectMin;
}

- (IBAction) aMaxPressed: (id) sender
{
    NSLog(@"Selectorama Maximum pressed");
    areaOption = iSelectMax;
}

- (IBAction) aRandomPressed: (id) sender
{
    NSLog(@"Selectorama Random pressed");
    areaOption = iSelectRandom;
}

- (IBAction) chTargetPressed: (id) sender
{
    NSLog(@"Selectorama Target pressed");
    channelOption = iUseTarget;
}

- (IBAction) chMergerPressed: (id) sender
{
    NSLog(@"Selectorama Merged pressed");
    channelOption = iUseMerged;
}

- (IBAction) cSelectionPressed: (id) sender
{
    NSLog(@"Selectorama Selection pressed");
    createOption = iCreateSelection;
}

- (IBAction) cPathPressed: (id) sender
{
    NSLog(@"Selectorama Path pressed");
    createOption = iCreateMaskpath;
}

- (IBAction) cLayerPressed: (id) sender
{
    NSLog(@"Selectorama Layer pressed");
    createOption = iCreateLayer;
}
@end

/* Carbon entry point and C-callable wrapper functions*/
OSStatus initializeCocoaSelectorama(GPtr globals)
{
    [[SelectoramaController alloc] init:globals];
    return noErr;
}

OSStatus orderWindowFrontSelectorama(void)
{
    int okPressed = [[SelectoramaController selectoramaController] showWindow];
    return okPressed;
}

// end SelectoramaController.m
