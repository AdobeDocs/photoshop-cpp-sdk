/**************************************************************************
 ADOBE SYSTEMS INCORPORATED
 Copyright 2010 Adobe Systems Incorporated
 All Rights Reserved.
 
 NOTICE:  Adobe permits you to use, modify, and distribute this file
 in accordance with the terms of the Adobe license agreement accompanying
 it.  If you have received this file from a source other than Adobe, then
 your use, modification, or distribution of it requires the prior written
 permission of Adobe.
 **************************************************************************/

//
//  PSConnectionCommands.h
//  pspalette
//
//  Created by Christoph on 1/10/11.
//  Copyright 2011 Adobe Systems Inc. All rights reserved.
//

#import <Foundation/Foundation.h>

// javascript screen modes
#define kScreenMode_Fullscreen @"screenModeFullScreen"
#define kScreenMode_FullscreenWithMenubar @"screenModeFullScreenWithMenubar"
#define kScreenMode_Standard @"screenModeStandard"

@interface PSConnectionCommands : NSObject
{

}

+ (NSMutableDictionary*) subscriptions;

+ (NSMutableString*) declareFuncFromScript:(NSString*)named;

// requests
+ (NSUInteger) getForegroundColor;
+ (NSUInteger) getBackgroundColor;

// commands
+ (NSUInteger) setForegroundColor:(UIColor*)color;
+ (NSUInteger) setBackgroundColor:(UIColor*)color;
+ (NSUInteger) addColorSwatches:(NSArray*)colors withName:(NSString*)setName;
+ (NSUInteger) setTool:(NSString*)toolScriptID;
+ (NSUInteger) showForegroundColorPicker;
+ (NSUInteger) showBackgroundColorPicker;
+ (NSUInteger) zoomTo100Percent;
+ (NSUInteger) setScreenMode:(NSString*)screenMode;

// events
+ (NSUInteger) subscribeToEvent:(NSString*)event target:(id)target selector:(SEL)selector;
+ (void) processEvent:(NSString*)event;

@end
