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
//  PSConnectionCommands.m
//  pspalette
//
//  Created by Christoph on 1/10/11.
//  Copyright 2011 Adobe Systems Inc. All rights reserved.
//

#import "PSConnection.h"
#import "PSConnectionCommands.h"
#import "UIColor-Expanded.h"

@implementation PSConnectionCommands

+ (NSMutableString*) declareFuncFromScript:(NSString*)named
{
	NSString* scriptPath = [[NSBundle mainBundle] pathForResource:named ofType:@"jsx"];
	NSError* error = nil;
	NSMutableString* script = [NSMutableString stringWithContentsOfFile:scriptPath encoding:NSUTF8StringEncoding error:&error];
	return script;
}

+ (NSString*) declareColorVar:(NSString*)name withColor:(UIColor*)color
{
	// get the color as a hex string
	NSString* hexString = [color hexStringFromColor];

	// declare the script
	NSString* scriptPath = [[NSBundle mainBundle] pathForResource:@"ColorVar" ofType:@"jsx"];
	NSError* error = nil;
	NSString* script = [NSString stringWithContentsOfFile:scriptPath encoding:NSUTF8StringEncoding error:&error];

	// replace the values in the script
	script = [script stringByReplacingOccurrencesOfString:@"name" withString:name];
	script = [script stringByReplacingOccurrencesOfString:@"hexString" withString:hexString];
	
	// done
	return script;
}

+ (NSMutableDictionary*) subscriptions
{
	static NSMutableDictionary* subscriptions;
	
	if (subscriptions == nil)
	{
		subscriptions = [[NSMutableDictionary alloc] initWithCapacity:0];
	}
	
	return subscriptions;
}

+ (NSUInteger) getForegroundColor
{
	// connection needs to be open
	if ([[PSConnection sharedPSConnection] isOpen] == NO)
	{
		return kErrorTransactionID;
	}
	
	// request
	// answer will come back in delegate's PSConnection::receivedJavascript callback
	return [[PSConnection sharedPSConnection] sendJavascript:@"app.foregroundColor.rgb.hexValue.toString()"];
}

+ (NSUInteger) getBackgroundColor
{
	// connection needs to be open
	if ([[PSConnection sharedPSConnection] isOpen] == NO)
	{
		return kErrorTransactionID;
	}
	
	// request
	// answer will come back in delegate's PSConnection::receivedJavascript callback
	return [[PSConnection sharedPSConnection] sendJavascript:@"app.backgroundColor.rgb.hexValue.toString()"];
}

+ (NSUInteger) setForegroundColor:(UIColor*)color
{
	// connection needs to be open
	if ([[PSConnection sharedPSConnection] isOpen] == NO)
	{
		return kErrorTransactionID;
	}

	// get the color as a hex string
	// and set the application foreground color with it
	NSString* hexString = [color hexStringFromColor];
	NSString* script = [PSConnectionCommands declareColorVar:@"color" withColor:color];
	script = [script stringByReplacingOccurrencesOfString:@"hexString" withString:hexString];
	script = [script stringByAppendingString:@"\napp.foregroundColor = color;"];

	// send the javascript
	return [[PSConnection sharedPSConnection] sendJavascript:script];
}

+ (NSUInteger) setBackgroundColor:(UIColor*)color
{
	// connection needs to be open
	if ([[PSConnection sharedPSConnection] isOpen] == NO)
	{
		return kErrorTransactionID;
	}

	// get the color as a hex string
	// and set the application foreground color with it
	NSString* hexString = [color hexStringFromColor];
	NSString* script = [PSConnectionCommands declareColorVar:@"color" withColor:color];
	script = [script stringByReplacingOccurrencesOfString:@"hexString" withString:hexString];
	script = [script stringByAppendingString:@"\napp.backgroundColor = color;"];

	// send the javascript
	return [[PSConnection sharedPSConnection] sendJavascript:script];
}

+ (NSUInteger) addColorSwatches:(NSArray*)colors withName:(NSString*)setName
{
	// connection needs to be open
	if ([[PSConnection sharedPSConnection] isOpen] == NO)
	{
		return kErrorTransactionID;
	}
	
	// make sure the colors are valid
	if (colors == nil || colors.count == 0)
	{
		return kErrorTransactionID;
	}
	
	// start by declaring a MakeSwatch function
	NSMutableString* script = [PSConnectionCommands declareFuncFromScript:@"MakeSwatchFunc"];

	// loop through all the colors
	// and add script to make a swatch for each color
	int i = 0;
	for (UIColor* color in colors)
	{
		// make a name for the variable
		NSString* colorVarName = [NSString stringWithFormat:@"color%d", i++];
		NSString* swatchWord = NSLocalizedString(@"Swatch", @"$$$/PSConnection/Swatch=Swatch");
		NSString* swatchName = [NSString stringWithFormat:@"%@ - %@ %d", swatchWord, setName, i];
		
		// add script to declare that color var
		[script appendFormat:@"\n%@", [PSConnectionCommands declareColorVar:colorVarName withColor:color]];
		
		// add script to MakeSwatch with that color var
		[script appendFormat:@"\nMakeSwatch( %@, '%@' );", colorVarName, swatchName];
	}
	
	// send it
	return [[PSConnection sharedPSConnection] sendJavascript:script];
}

+ (NSUInteger) setTool:(NSString*)toolScriptID;
{
	// connection needs to be open
	if ([[PSConnection sharedPSConnection] isOpen] == NO)
	{
		return kErrorTransactionID;
	}

	// make sure the tool ID is valid
	if (toolScriptID == nil || toolScriptID.length == 0)
	{
		return kNoTransactionID;
	}
	
	// script for setting the active tool
	NSString* script = [NSString stringWithFormat:@"var id93 = charIDToTypeID( 'slct' );"];
	script = [script stringByAppendingFormat:@" var desc32 = new ActionDescriptor();"];
	script = [script stringByAppendingFormat:@" var id94 = charIDToTypeID( 'null' );"];
	script = [script stringByAppendingFormat:@" var ref30 = new ActionReference();"];
	script = [script stringByAppendingFormat:@" ref30.putClass( stringIDToTypeID( '%@' ) );", toolScriptID];
	script = [script stringByAppendingFormat:@" desc32.putReference( id94, ref30 );"];
	script = [script stringByAppendingFormat:@" executeAction( id93, desc32, DialogModes.NO );"];
	
	// send it
	return [[PSConnection sharedPSConnection] sendJavascript:script];
}


+ (NSUInteger) showForegroundColorPicker
{
	// connection needs to be open
	if ([[PSConnection sharedPSConnection] isOpen] == NO)
	{
		return kErrorTransactionID;
	}
	
	// command
	return [[PSConnection sharedPSConnection] sendJavascript:@"app.showColorPicker(true)"];
}

+ (NSUInteger) showBackgroundColorPicker
{
	// connection needs to be open
	if ([[PSConnection sharedPSConnection] isOpen] == NO)
	{
		return kErrorTransactionID;
	}
	
	// command
	return [[PSConnection sharedPSConnection] sendJavascript:@"app.showColorPicker(false)"];
}

+ (NSUInteger) zoomTo100Percent
{
	// connection needs to be open
	if ([[PSConnection sharedPSConnection] isOpen] == NO)
	{
		return kErrorTransactionID;
	}
	
	// script for setting the active tool
	NSString* script = [NSString stringWithFormat:@"var idslct = charIDToTypeID( 'slct' );"];
	script = [script stringByAppendingFormat:@"var desc18 = new ActionDescriptor();"];
	script = [script stringByAppendingFormat:@"var idnull = charIDToTypeID( 'null' );"];
	script = [script stringByAppendingFormat:@"var ref11 = new ActionReference();"];
	script = [script stringByAppendingFormat:@"var idMn = charIDToTypeID( 'Mn  ' );"];
	script = [script stringByAppendingFormat:@"var idMnIt = charIDToTypeID( 'MnIt' );"];
	script = [script stringByAppendingFormat:@"var idActP = charIDToTypeID( 'ActP' );"];
	script = [script stringByAppendingFormat:@"ref11.putEnumerated( idMn, idMnIt, idActP );"];
	script = [script stringByAppendingFormat:@"desc18.putReference( idnull, ref11 );"];
	script = [script stringByAppendingFormat:@"executeAction( idslct, desc18, DialogModes.NO );"];

	// send it
	return [[PSConnection sharedPSConnection] sendJavascript:script];
}

+ (NSUInteger) setScreenMode:(NSString*)screenMode
{
	// connection needs to be open
	if ([[PSConnection sharedPSConnection] isOpen] == NO)
	{
		return kErrorTransactionID;
	}

	// make sure the screen mode is valid
	if ([screenMode compare:kScreenMode_Standard] != NSOrderedSame &&
		[screenMode compare:kScreenMode_FullscreenWithMenubar] != NSOrderedSame &&
		[screenMode compare:kScreenMode_Fullscreen] != NSOrderedSame)
	{
		return kErrorTransactionID;
	}

	// script for setting the screen mode
	NSString* script = [NSString stringWithFormat:@"var idslct = charIDToTypeID( 'slct' );"];
	script = [script stringByAppendingFormat:@"var desc17 = new ActionDescriptor();"];
	script = [script stringByAppendingFormat:@"var idnull = charIDToTypeID( 'null' );"];
	script = [script stringByAppendingFormat:@"var ref10 = new ActionReference();"];
	script = [script stringByAppendingFormat:@"var idMn = charIDToTypeID( 'Mn  ' );"];
	script = [script stringByAppendingFormat:@"var idMnIt = charIDToTypeID( 'MnIt' );"];
	script = [script stringByAppendingFormat:@"var idscreenMode = stringIDToTypeID( '%@' );", screenMode];
	script = [script stringByAppendingFormat:@"ref10.putEnumerated( idMn, idMnIt, idscreenMode );"];
	script = [script stringByAppendingFormat:@"desc17.putReference( idnull, ref10 );"];
	script = [script stringByAppendingFormat:@"executeAction( idslct, desc17, DialogModes.NO );"];

	// send it
	return [[PSConnection sharedPSConnection] sendJavascript:script];
}

+ (NSUInteger) subscribeToEvent:(NSString*)event target:(id)target selector:(SEL)selector
{
	// connection needs to be open
	if ([[PSConnection sharedPSConnection] isOpen] == NO)
	{
		return kErrorTransactionID;
	}
	
	// make sure event is valid
	if (event == nil || event.length == 0)
	{
		return kErrorTransactionID;
	}

	// currently only supports a single subscription
	// will need to support multiple subscriptions in the future
	// adding a subscription that already exists will replace
	// the previous subscription
	NSMutableDictionary* subscription = [NSMutableDictionary dictionaryWithCapacity:2];
	[subscription setObject:target forKey:@"Target"];
	[subscription setObject:NSStringFromSelector(selector) forKey:@"Selector"];
	[[PSConnectionCommands subscriptions] setObject:subscription forKey:event];

	// start by declaring the subcribe to event function
	NSMutableString* script = [PSConnectionCommands declareFuncFromScript:@"SubscribeToEventFunc"];

	// call the func
	[script appendFormat:@"\nSubscribeToEvent( '%@' );", event];
	
	// send it
	return [[PSConnection sharedPSConnection] sendJavascript:script];
}

+ (void) processEvent:(NSString*)event
{
	// make sure there are subscriptions
	NSMutableDictionary* subscriptions = [PSConnectionCommands subscriptions];
	
	// tokenize the event
	NSArray* tokens = [event componentsSeparatedByString:@"\r"];
	
	// find a subscribed event
	// nothing to do if not found
	NSDictionary* subscription = [subscriptions objectForKey:[tokens objectAtIndex:0]];
	if (subscription == nil)
	{
		return;
	}
	
	// get the target and selector
	NSObject* target = [subscription objectForKey:@"Target"];
	SEL selector = NSSelectorFromString([subscription objectForKey:@"Selector"]);
	
	// perform the selector
	// the remaining tokens are ignored
	NSString* argument = (tokens.count > 1 ? [tokens objectAtIndex:1] : nil);
	[target performSelector:selector withObject:argument];
}

@end
