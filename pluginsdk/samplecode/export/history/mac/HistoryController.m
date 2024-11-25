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

#import "HistoryController.h"

HistoryController *gHistoryController = NULL;
#define kDHistTotal 7

/* Make sure this is unique to you and everyone you might encounter, search for
 "Preventing Name Conflicts" or use this link
 http://developer.apple.com/mac/library/documentation/UserExperience/Conceptual/PreferencePanes/Tasks/Conflicts.html
 */

// controller for the entire dialog
@implementation HistoryController

+ (HistoryController *) historyController
{
    return gHistoryController;
}


- (id) init: (GPtr) globals
{
    self = [super init];
	
    NSBundle * plugin = [NSBundle bundleForClass:[self class]];
	
    if (![plugin loadNibNamed:@"HistDialog" owner:self topLevelObjects:nil])
    {
        NSLog(@"History failed to load HistoryDialog xib");
    }
    
    gHistoryController = self;
    globData = globals;
    
    Str255 s1 = " ^0 to ^1 of ^2";
    s1[0] = (char)strlen((char*)&s1[1]);
    PICopy(hS, s1, s1[0]+1);

    [self CheckAndEnable];
    
    NSLog(@"History Done with init");

    return self;
}

- (int) showWindow
{
    [historyWindow makeKeyAndOrderFront:nil];
    int b = (int)[[NSApplication sharedApplication] runModalForWindow:historyWindow];
    [historyWindow orderOut:self];
    return b;
}

- (IBAction) okPressed:(id)sender
{
    NSLog(@"History OK pressed");
    [NSApp stopModalWithCode:1];
    NSLog(@"History after nsapp stopmodal");
}

- (IBAction) cancelPressed:(id)sender
{
    NSLog(@"History cancel pressed");
    [NSApp stopModalWithCode:0];
    NSLog(@"History after nsapp abortmodal");
}

- (IBAction) trimFirstPressed: (id) sender
{
    NSLog(@"History trim first pressed");
    if (currentResources >= globData->currentHistory)
    {
        HostDeletePIResource(globData->exportParamBlock->resourceProcs, histResource, globData->currentHistory);
        globData->exportParamBlock->dirty = TRUE;
    }
    [self CheckAndEnable];
}

- (IBAction) trimLastPressed: (id) sender
{
    NSLog(@"History trim last pressed");
    if (currentResources >= globData->currentHistory + kDHistTotal - 1)
    {
        HostDeletePIResource(globData->exportParamBlock->resourceProcs, histResource, globData->currentHistory + kDHistTotal - 1);
        globData->exportParamBlock->dirty = TRUE;
    }
    else if (currentResources > 0)
    {
        HostDeletePIResource(globData->exportParamBlock->resourceProcs, histResource, currentResources);
        globData->exportParamBlock->dirty = TRUE;
    }
    [self CheckAndEnable];
}

- (IBAction) upPressed: (id) sender
{
    NSLog(@"History up pressed");
    (globData->currentHistory)--;
    [self CheckAndEnable];
}

- (IBAction) downPressed: (id) sender
{
    NSLog(@"History down pressed");
    (globData->currentHistory)++;
    [self CheckAndEnable];
}

- (void) CheckAndEnable
{
    currentResources = HostCountPIResources(globData->exportParamBlock->resourceProcs, histResource);
    
    if (globData->currentHistory < 1)
        globData->currentHistory = 1;
    
    if (globData->currentHistory > currentResources)
        globData->currentHistory = currentResources;
    
    if (currentResources <= kDHistTotal || globData->currentHistory + kDHistTotal > currentResources)
    {
        [downID setEnabled: false];
    }
    else
        [downID setEnabled: true];
    
    if (globData->currentHistory < 2)
    {
        [upID setEnabled: false];
    }
    else
        [upID setEnabled: true];
    
    if (currentResources >= globData->currentHistory && currentResources > 0)
    {
        [trimFirstID setEnabled: true];
        [trimLastID setEnabled: true];
    }
    else
    {
        [trimFirstID setEnabled: false];
        [trimLastID setEnabled: false];
    }
    
    [self UpdateHistories];
}

- (void) UpdateHistories
{
    Str255 s;
    Str255 n1;
    Str255 n2;
    Str255 nT;
    Str255 ss;
    
    int x = globData->currentHistory + (kDHistTotal-1);
    short loop;
    
    PICopy(ss, hS, hS[0]+1);
    
    for(loop = globData->currentHistory; loop < globData->currentHistory+kDHistTotal; loop++)
    {
        GetHistory(globData, loop, s);
        if (loop == globData->currentHistory && !s[0])
        {
            Str255 s1 = " No history resources in this document.";
            s1[0] = (char)strlen((char*)&s1[1]);
            PICopy(s, s1, s1[0]+1);
            ss[(ss[0] = 0)+1] = 0;
        }

        if (s[0] > 0)
            [textID setStringValue: [NSString stringWithUTF8String:(char *)(s+1)]];
        if (s[0] < 1 && x > loop-1)
            x = loop - 1;
    }
    
    if (ss[0] > 0)
    {
        NumToString(globData->currentHistory, n1);
        NumToString(x, n2);
        NumToString(currentResources, nT);

        PIParamText(ss, n1, n2, nT);
    }
    if(ss[0] > 0)
        [textID setStringValue: [NSString stringWithUTF8String:(char *)(ss+1)]];
}
@end

/* Carbon entry point and C-callable wrapper functions*/
OSStatus initializeCocoaHistory(GPtr globals)
{
    [[HistoryController alloc] init:globals];
    return noErr;
}

OSStatus orderWindowFrontHistory(void)
{
    int okPressed = [[HistoryController historyController] showWindow];
    return okPressed;
}

// end HistoryController.m
