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

#import "ListenerController.h"

ListenerController *gListenerController = NULL;


/* Make sure this is unique to you and everyone you might encounter, search for
 "Preventing Name Conflicts" or use this link
 http://developer.apple.com/mac/library/documentation/UserExperience/Conceptual/PreferencePanes/Tasks/Conflicts.html
 */

void EventToText(uint32 inputLong, NSString** returnString)
{
    uint32 index = 0;
    while (useful[index].strStr != NULL)
    {
        if (useful[index].longVal == inputLong)
        {
            *returnString = useful[index].strStr;
            break;
        }
        index++;
    }
}

// controller for the entire dialog
@implementation ListenerController

+ (ListenerController *) listenerController
{
    return gListenerController;
}


- (id) init
{
    self = [super init];
	
	NSBundle * plugin = [NSBundle bundleForClass:[self class]];

    if (![plugin loadNibNamed:@"ListenerDialog" owner:self topLevelObjects:nil])
    {
        NSLog(@"Listener failed to load ListenerDialog xib");
    }
    
    gListenerController = self;
    [self initPopups];
    
    NSLog(@"Listener Done with init");

    return self;
}

- (int) showWindow
{
    [listenerWindow makeKeyAndOrderFront:nil];
    int b = (int)[[NSApplication sharedApplication] runModalForWindow:listenerWindow];
    [listenerWindow orderOut:self];
    return b;
}

- (IBAction) watchPopup:(id)sender
{
    int index = (int)[watchID indexOfSelectedItem];
    DescriptorEventID result = useful[index].longVal;
    if (result != 0)
        gEventID = result;
}


- (IBAction) actionSet:(id)sender
{
    NSMenuItem *item = [setID selectedItem];
    std::string * actionSet = new string;
    actionSet->assign([item.title cStringUsingEncoding:NSUTF8StringEncoding]);
    
    if (gActionSet != NULL)
        delete gActionSet;
    
    gActionSet = actionSet;

    [self initActionPopup];
}


- (IBAction) actionName:(id)sender
{
    NSMenuItem *item = [actionID selectedItem];
    std::string * actionName = new string;
    actionName->assign([item.title cStringUsingEncoding:NSUTF8StringEncoding]);
    
    if (gActionName != NULL)
        delete gActionName;
    
    gActionName = actionName;
}


- (IBAction) okPressed:(id)sender
{
    NSLog(@"Listener OK pressed");
    [NSApp stopModalWithCode:1];
    NSLog(@"Listener after nsapp stopmodal");
}

- (IBAction) cancelPressed:(id)sender
{
    NSLog(@"Listener cancel pressed");
    [NSApp stopModalWithCode:0];
    NSLog(@"Listener after nsapp abortmodal");
}

- (SPErr) initPopups
{
    SPErr error = kSPNoError;
    
    // Initializing watch popup
    [watchID removeAllItems];
    int iter = 0;;
    while (useful[iter].strStr != NULL)
    {
        [watchID addItemWithTitle:useful[iter].strStr];
        
        if (gEventID == 0)
            gEventID = useful[iter].longVal;
        
        iter++;
    }
    [watchID selectItemAtIndex:0];
    
    
    // Initializing action set popup
    [setID removeAllItems];
    if (gActionSet != NULL)
    {
        delete gActionSet;
        gActionSet = NULL;
    }
    
    uint32 index = 1;
    error = kSPNoError;
    while (error == kSPNoError)
    {
        char name[BigStrMaxLen];
        uint32 length = BigStrMaxLen - 1;
        error = PIUGetInfoByIndex(index, classActionSet, keyName, name, &length);
        
        if (error == kSPNoError)
        {
            [setID addItemWithTitle: [NSString stringWithUTF8String:name]];
            
            if (gActionSet == NULL)
            {
                gActionSet = new string;
                if (gActionSet != NULL)
                    *gActionSet = name;
            }
        }
        index++;
    }
    [setID selectItemAtIndex:0];
    
    
    [self initActionPopup];

    // Initialize the notifier text field
    NSMutableString* tempDisplay = [[NSMutableString alloc] init];
    [tempDisplay setString:@""];
    NSString* eventAsString = NULL;
    
    Listener_t* thisList = gListenerList;
    uint32 size = 0;
    
    while (thisList != NULL)
    {
        EventToText(thisList->eventID, &eventAsString);
        
        [tempDisplay appendString:eventAsString];
        [tempDisplay appendString:@": \""];
        if (thisList->actionName.c_str())
            [tempDisplay appendString:[NSString stringWithUTF8String:thisList->actionName.c_str()]];
        [tempDisplay appendString:@"\" of \""];
        if (thisList->actionSet.c_str())
            [tempDisplay appendString:[NSString stringWithUTF8String:thisList->actionSet.c_str()]];
        [tempDisplay appendString:@"\".\r"];
        
        thisList = thisList->next;
        size++;
    }
    
    NSUInteger len = [tempDisplay length];
    if (len == 0)
    {
        [notifiersID setStringValue:@"No notifiers installed"];
    }
    else if (size > 6)
    {
        [notifiersID setStringValue:@"Too many notifiers to display"];
    }
    else
    {
        [notifiersID setStringValue: tempDisplay];
    }
    [tempDisplay release];
    
    return error;
}

- (void) initActionPopup
{
    SPErr error = kSPNoError;

    // Initializing action popup
    [actionID removeAllItems];
    if (gActionName != NULL)
        {
        delete gActionName;
        gActionName = NULL;
        }
    
    uint32 index = 1;

    while (error == kSPNoError)
        {
        char name[BigStrMaxLen];
        uint32 length = BigStrMaxLen - 1;
        error = PIUActionsGetName((char*)(gActionSet->c_str()), index, name, &length);
        
        if (error == kSPNoError)
            {
            [actionID addItemWithTitle: [NSString stringWithUTF8String:name]];
            
            if (gActionName == NULL)
                {
                gActionName = new string;
                if (gActionName != NULL)
                    *gActionName = name;
                }
            }
        index++;
        }
    [actionID selectItemAtIndex:0];
    
}

@end


/* Carbon entry point and C-callable wrapper functions*/
OSStatus initializeCocoaListener()
{
    [[ListenerController alloc] init];
    return noErr;
}

OSStatus orderWindowFrontListener(void)
{
    int okPressed = [[ListenerController listenerController] showWindow];
    return okPressed;
}

// end ListenerController.m
