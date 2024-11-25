/*
 File: EchoClientAppDelegate.m
 
 Abstract: Implementation of the GUI CocoaEcho client.
 
 Disclaimer: IMPORTANT:  This Apple software is supplied to you by Apple
 Computer, Inc. ("Apple") in consideration of your agreement to the
 following terms, and your use, installation, modification or
 redistribution of this Apple software constitutes acceptance of these
 terms.  If you do not agree with these terms, please do not use,
 install, modify or redistribute this Apple software.
 
 In consideration of your agreement to abide by the following terms, and
 subject to these terms, Apple grants you a personal, non-exclusive
 license, under Apple's copyrights in this original Apple software (the
 "Apple Software"), to use, reproduce, modify and redistribute the Apple
 Software, with or without modifications, in source and/or binary forms;
 provided that if you redistribute the Apple Software in its entirety and
 without modifications, you must retain this notice and the following
 text and disclaimers in all such redistributions of the Apple Software. 
 Neither the name, trademarks, service marks or logos of Apple Computer,
 Inc. may be used to endorse or promote products derived from the Apple
 Software without specific prior written permission from Apple.  Except
 as expressly stated in this notice, no other rights or licenses, express
 or implied, are granted by Apple herein, including but not limited to
 any patent rights that may be infringed by your derivative works or by
 other works in which the Apple Software may be incorporated.
 
 The Apple Software is provided by Apple on an "AS IS" basis.  APPLE
 MAKES NO WARRANTIES, EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION
 THE IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY AND FITNESS
 FOR A PARTICULAR PURPOSE, REGARDING THE APPLE SOFTWARE OR ITS USE AND
 OPERATION ALONE OR IN COMBINATION WITH YOUR PRODUCTS.
 
 IN NO EVENT SHALL APPLE BE LIABLE FOR ANY SPECIAL, INDIRECT, INCIDENTAL
 OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 INTERRUPTION) ARISING IN ANY WAY OUT OF THE USE, REPRODUCTION,
 MODIFICATION AND/OR DISTRIBUTION OF THE APPLE SOFTWARE, HOWEVER CAUSED
 AND WHETHER UNDER THEORY OF CONTRACT, TORT (INCLUDING NEGLIGENCE),
 STRICT LIABILITY OR OTHERWISE, EVEN IF APPLE HAS BEEN ADVISED OF THE
 POSSIBILITY OF SUCH DAMAGE.
 
 Copyright © 2005 Apple Computer, Inc., All Rights Reserved
 */

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

/* UI-portion of Echo Client App GUI */

#import "EchoClientAppDelegate.h"
#import <Foundation/NSJSONSerialization.h>

/**************************************************************************************/

static transaction_id sPreviewTransaction = 0; //for the preview window
static transaction_id sUserJSTransaction = 0; //from send javascript
static transaction_id sImageChangedSubscription =  0;
static transaction_id sMenuChangedSubscription =  0;
static transaction_id sSendInfoTransaction =  0;
static transaction_id sSendLayerInfoTransaction =  0;

static transaction_id sProfileTransaction =  0;
NSURL *  sProfileURL = NULL;

static transaction_id sMenuInfoTransaction =  0;
static transaction_id sToolSubscriptionTransaction =  0;
static transaction_id sEventStatusTransaction =  0;

static transaction_id sCommandTransactionInProgress = 0;

// subscribe to notifications for color changed events
static transaction_id sForegroundColorSubscription = 0;
static transaction_id sBackgroundColorSubscription = 0;
static transaction_id sForegroundColorTransaction = 0;
static transaction_id sBackgroundColorTransaction = 0;
@implementation EchoClientAppDelegate

/**************************************************************************************/

#pragma mark -
#pragma mark random app cruft

/**************************************************************************************/

- (void)awakeFromNib {
    
    [ [EchoClient get] setDelegate: self];
	
	[ foregroundColorWidget setBordered: false ];
	[ backgroundColorWidget setBordered: false ];

	if ([[EchoClient get] password])
		[passwordField setStringValue:[[EchoClient get] password]];
	
	NSColor *theColor = [ NSColor colorWithDeviceRed: 0.5 green:0.5 blue:0.5 alpha: 1.0 ];
	[ connectionStatusWidget setColor: theColor ];

    [self enableDocControls: NO ];

}

/**************************************************************************************/

- (transaction_id)sendJavaScript:(NSString*)stringToSend {
	return [ [EchoClient get] sendJavaScript: stringToSend ];
}

/**************************************************************************************/

- (void)setupSubscription: (const char *)eventStr withTransactionID: (transaction_id*)transaction_id {

	BOOL subscribe = [subscribeChangesMenuItem state] == NSOnState;

	if (*transaction_id)
		{
		[ [EchoClient get] unsubscribeEvent: eventStr ];
		*transaction_id = 0;
		}
	if (subscribe)
		*transaction_id = [ [EchoClient get] subscribeEvent: eventStr ];
}


/**************************************************************************************/

- (transaction_id)sendCancelCommandMessage:(transaction_id)canceltransid {
	return [ [EchoClient get] sendCancelCommandMessage: canceltransid ];
}

/**************************************************************************************/
bool sSkippedScheduledEventStatus = false;

- (transaction_id)getCurrentEventStatus {

    //don't send a new one until the old one is handled,
    //since we only have one sEventStatusTransaction global to track it.

    if (  [pollActiveEventMenuItem state] == NSOnState )
        {
        if (sEventStatusTransaction == 0)
            sEventStatusTransaction = [ [EchoClient get] getCurrentEventStatus ];
        else
            {
            //otherwise just call it once we've handled the old one.
            sSkippedScheduledEventStatus = true;
            }
        }

    return sEventStatusTransaction;
}

/**************************************************************************************/

- (void)postEventStatus:(NSDictionary*)jsonDict {

    double eventTime = [[jsonDict objectForKey:@"eventTime"] doubleValue];
    NSString* eventName = [jsonDict objectForKey:@"eventName"];
    int32_t eventID = [[jsonDict objectForKey:@"eventID"] intValue];
    transaction_id transactionID = [[jsonDict objectForKey:@"eventTransaction"] unsignedIntValue];
   
   if (jsonDict == nil)
       	LogError("postEventStatus: NULL jsonDict");
   if (eventName == nil)
       	LogError("postEventStatus: NULL eventName");

   [curEventID setIntValue: eventID ];
   [curEventName setStringValue: eventName ];
   [curEventTime setDoubleValue: eventTime];

    //allow cancelling of certain commands
    //(doc will have to be really complicated for these to take any significant amount of time)
    //we can only cancel image/javascript commands in progress
    //you need to unsubscribe subscriptions that have completed the javascript subscription command
    sCommandTransactionInProgress = transactionID;

    //if we took longer than the scheduled time to receive an event status, get another one.
    //so we don't have to wait for a whole other period.

    sEventStatusTransaction = 0;
    if (sSkippedScheduledEventStatus)
        {
        sSkippedScheduledEventStatus = false;
        [self getCurrentEventStatus];
        }
 }

/**************************************************************************************/


- (transaction_id)sendDataMessage:(NSData*)dataToSend {
	return [ [EchoClient get] sendDataMessage: dataToSend ];
}

/**************************************************************************************/

- (transaction_id)sendImageMessage:(NSData*)dataToSend {
	return [ [EchoClient get] sendImageMessage: dataToSend];
}

/**************************************************************************************/

- (IBAction)getColorPressed:(id)sender {
    NSString * stringToSend = [NSString stringWithUTF8String:"app.foregroundColor.rgb.hexValue.toString();"];
	[ self sendJavaScript: stringToSend ];
}

/**************************************************************************************/

- (IBAction)invertPressed:(id)sender {
    NSString * stringToSend = [NSString stringWithUTF8String:"var color = app.foregroundColor; color.rgb.red = 255 - color.rgb.red; color.rgb.green = 255 - color.rgb.green; color.rgb.blue = 255 - color.rgb.blue; app.foregroundColor = color;"];
	[ self sendJavaScript: stringToSend ];
}

/**************************************************************************************/

- (IBAction)scriptErrorPressed:(id)sender {
    NSString * stringToSend = [NSString stringWithUTF8String:"alert(\"Barry's Coming"];
	[ self sendJavaScript: stringToSend ];
}

/**************************************************************************************/

- (IBAction)sendBlobPressed:(id)sender {
	NSString *ducky = [[NSBundle mainBundle] pathForResource:@"Ducky" ofType:@"jpg"];
	NSData *imageData = [[[NSData alloc] initWithContentsOfFile:ducky] autorelease];
	[ self sendDataMessage:imageData ];
}

/**************************************************************************************/

- (IBAction)sendHighASCIIPressed:(id)sender {
// Apple says "It is not safe is to include high-bit characters in your source code"
// you have to enter the escaped versions of the values for UTF8
// Since \xe2\x80\x94 is the 3-byte UTF-8 string for 0x2014...
// This is apparently due to GCC mangling high ASCII characters
// ccox - but that does not explain why a UTF16 string pasted into the edit field is also corrupted!

//	char *testString = "alert(\"€£¥ ©®℗℠™" HighASCII Test €£¥\");";

	const char *testString = "alert(\"\xe2\x82\xac\xc2\xa3\xc2\xa5\x20\xc2\xa9\xc2\xae\xe2\x84\x97\xE2\x84\xA0\xE2"
						"\x84\xA2\x20\x48\x69\x67\x68\x41\x53\x43\x49\x49\x20\x54\x65"
						"\x73\x74\x20\xE2\x82\xAC\xC2\xA3\xC2\xA5\");  "
						"\"\xe2\x82\xac\xc2\xa3\xc2\xa5\x20\xc2\xa9\xc2\xae\xe2\x84\x97\xE2\x84\xA0\xE2"
						"\x84\xA2\x20\x48\x69\x67\x68\x41\x53\x43\x49\x49\x20\x54\x65"
						"\x73\x74\x20\xE2\x82\xAC\xC2\xA3\xC2\xA5\"";
    NSString * stringToSend = [NSString stringWithUTF8String:testString];
	
	[ self sendJavaScript: stringToSend ];
}

/**************************************************************************************/

- (IBAction)sendJPEGPressed:(id)sender {
	NSString *ducky = [[NSBundle mainBundle] pathForResource:@"Ducky" ofType:@"jpg"];
	NSData *imageData = [[[NSData alloc] initWithContentsOfFile:ducky] autorelease];
	
	// need one byte before the image data for format type
	NSMutableData *image_message = [[NSMutableData alloc] autorelease];
	
	unsigned char format = jpeg_format;	// JPEG
	[ image_message appendBytes:(const void *)&format length:1 ];
	[ image_message appendData: imageData ];
	
	[ self sendImageMessage:image_message ];
}

/**************************************************************************************/

- (IBAction)sendPixmapPressed:(id)sender {
	uint32_t width = 256;
	uint32_t height = 256;
	uint32_t planes = 3;
	uint32_t bufferSize = width * height * planes;
	uint32_t i, j;
	
	NSMutableData *imageData = [[[NSMutableData alloc] initWithLength:bufferSize] autorelease];
	unsigned char *buffer = (unsigned char *) [imageData bytes];
	
	// put some simple gradients in the bits
	for (i = 0; i < height; ++i)
		{
		for (j = 0; j < width; ++j)
			{
			size_t offset = i*width*planes + j*3;
			buffer[ offset + 0 ] = (i) & 0xFF;
			buffer[ offset + 1 ] = (j) & 0xFF;
			buffer[ offset + 2 ] = (i + j) & 0xFF;
			}
		}
	
	
	// construct the header
	NSMutableData *image_message = [[NSMutableData alloc] autorelease];
	
	uint8_t format = pixmap_format;		// Pixmap
	[ image_message appendBytes:(const void *)&format length:1 ];
	
	// 4 bytes uint32 width
	uint32_t temp = htonl( width );
	[ image_message appendBytes:(const void *)&temp length:4 ];
	
	// 4 bytes uint32 height
	temp = htonl( height );
	[ image_message appendBytes:(const void *)&temp length:4 ];
	
	// 4 bytes uint32 rowBytes
	temp = htonl( planes*width );
	[ image_message appendBytes:(const void *)&temp length:4 ];
	
	// 1 byte color mode = 1 = RGB
	uint8_t tempC = rgb_mode;
	[ image_message appendBytes:(const void *)&tempC length:1 ];
	
	// 1 byte channel count
	tempC = planes;
	[ image_message appendBytes:(const void *)&tempC length:1 ];
	
	// 1 byte bits per channel
	tempC = 8;
	[ image_message appendBytes:(const void *)&tempC length:1 ];
	
	// append the bits
	[ image_message appendData: imageData ];
	
	// send the message
	[ self sendImageMessage:image_message ];
}

/**************************************************************************************/

inline NSSize GetMaxThumbnailSize() { return NSMakeSize(2048,2048); }

/**************************************************************************************/

static const char* sFormatVersion =  "1.3.0"; //latest version as of writing Photoshop CC 2015

- (void)setupGeneratorSubscriptions  {

	[ self setupSubscription: "imageChanged"			withTransactionID:&sImageChangedSubscription ];
	[ self setupSubscription: "generatorMenuChanged"	withTransactionID:&sMenuChangedSubscription ];
	[ self setupSubscription: "foregroundColorChanged"	withTransactionID:&sForegroundColorSubscription];
	[ self setupSubscription: "backgroundColorChanged"	withTransactionID:&sBackgroundColorSubscription];
	[ self setupSubscription: "toolChanged"				withTransactionID:&sToolSubscriptionTransaction];
	}

/**************************************************************************************/

static bool sAddedNodeMenus = false;
NSString * const kMenu1Name = @"menu1"; //arbitrary identifying names for menus
NSString * const kMenu2Name = @"menu2";

- (void)sendNodeMenuInitialize {

    if (sAddedNodeMenus)
        return;
    
    sAddedNodeMenus = true;
    
    BOOL menusEnabled = [generatorMenusEnabledMenuItem state] == NSOnState;
    
    //nodeMenuInitialize clears out all the generator items, so you have to specify
    //them all in one list.
 
    //this adds a menu to the menu list
    NSString * menuFormat =
        @"menu1.putString( stringIDToTypeID( \"name\" ), \"%@\" );"
       "menu1.putString( stringIDToTypeID( \"displayName\" ), \"%@\" );"
       "menu1.putBoolean( stringIDToTypeID( \"enabled\" ), %@ );"
       "menu1.putBoolean( stringIDToTypeID( \"checked\" ), %@ );"
        "list1.putObject( stringIDToTypeID( \"nodeMenu\" ), menu1 )";

    NSString * stringToSend = [NSString stringWithFormat:
        @"var nodeMenuInitializeID = stringIDToTypeID( \"nodeMenuInitialize\" );"
       "var desc1 = new ActionDescriptor();"
        "var list1 = new ActionList();"
        "var menu1 = new ActionDescriptor();"
       "%@;"
       "%@;"
       "desc1.putList(nodeMenuInitializeID, list1);"
       "executeAction( nodeMenuInitializeID, desc1, DialogModes.NO );",
       
       [NSString stringWithFormat: menuFormat,
       @"menu1",
       [generatorMenuItem1 title],
        menusEnabled ? @"true" : @"false",
        [generatorMenuItem1 state] == NSOnState ? @"true" : @"false"],
        
      [NSString stringWithFormat: menuFormat,
       @"menu2",
       [generatorMenuItem2 title],
       menusEnabled ? @"true" : @"false",
       [generatorMenuItem2 state] == NSOnState ? @"true" : @"false"]
       ];

	[ self sendJavaScript: stringToSend ];
}


/**************************************************************************************/
//tell photoshop  the state of the node menus
//(we pretend we are the node process for testing)

- (void)setNodeMenuStates {

    if (sAddedNodeMenus)
        {
        BOOL menusEnabled = [generatorMenusEnabledMenuItem state] == NSOnState;
       
       bool menu1Checked = [generatorMenuItem1 state] == NSOnState;
       bool menu2Checked = [generatorMenuItem2 state] == NSOnState;
        
        NSString* formatString = @"var idNS = stringIDToTypeID( \"nodeMenu\" );"
                                   "var desc1 = new ActionDescriptor();"
                                   "desc1.putString( stringIDToTypeID( \"name\" ), \"%@\" );"
                                   "desc1.putString( stringIDToTypeID( \"displayName\" ), \"%@\" );"
                                   "desc1.putBoolean( stringIDToTypeID( \"enabled\" ), %@ );"
                                   "desc1.putBoolean( stringIDToTypeID( \"checked\" ), %@ );"
                                   "executeAction( idNS, desc1, DialogModes.NO );";
        
        //change the display name in generator to test that bit.
        NSString* generatorMenu1DisplayName = [NSString stringWithFormat:@"%@ %@ %@",
                                   [generatorMenuItem1 title],
                                   menusEnabled ? @"enabled" : @"disabled",
                                   menu1Checked ? @"checked" : @"unchecked" ];

        NSString* generatorMenu2DisplayName = [NSString stringWithFormat:@"%@ %@ %@",
                                   [generatorMenuItem2 title],
                                   menusEnabled ? @"enabled" : @"disabled",
                                   menu2Checked ? @"checked" : @"unchecked" ];
        
        NSString * stringToSend1 = [NSString stringWithFormat:formatString,
                                   kMenu1Name,
                                   generatorMenu1DisplayName,
                                   menusEnabled ? @"true" : @"false",
                                   menu1Checked ? @"true" : @"false" ];

        NSString * stringToSend2 = [NSString stringWithFormat:formatString,
                                   kMenu2Name,
                                   generatorMenu2DisplayName,
                                   menusEnabled ? @"true" : @"false",
                                   menu2Checked ? @"true" : @"false" ];
        
        [ self sendJavaScript: stringToSend1 ];
        [ self sendJavaScript: stringToSend2 ];
        }
}

/**************************************************************************************/

const NSTimeInterval kPreviewInterval = .5;
static NSTimeInterval sNextPreviewTime = 0.0;
static bool sScheduledPreview = false;


enum PreviewType {
    eNoPreview = 0, //menu order
    eAllLayers,
    eSelectedLayer,
    eSelectedLayerNoFX, //to test firstLayer, lastLayer form
    eSelectedLayerShape,
    eActiveDocPixMap,
    eActiveDocJPEG,
    eNumPreviewTypes
};


/**************************************************************************************/

- (PreviewType)getPreviewType {

    NSInteger previewIndex = [previewType indexOfSelectedItem];
    
    if (previewIndex < 0)
        previewIndex = 0;
 
    if (previewIndex >= eNumPreviewTypes)
        previewIndex = eNumPreviewTypes - 1;
 
    return PreviewType(previewIndex);

}

/**************************************************************************************/

- (BOOL)canScale {
    PreviewType preview = [self getPreviewType];
    return (preview >= eAllLayers && preview <= eSelectedLayerShape);
}


/**************************************************************************************/

- (BOOL)canComp {
     PreviewType preview = [self getPreviewType];
    return (preview == eAllLayers || preview == eSelectedLayerNoFX )
                && [compsMenu numberOfItems] > 1;
}


/**************************************************************************************/

- (BOOL)canInterpolate {
    PreviewType preview = [self getPreviewType];
    return(preview == eAllLayers || preview == eSelectedLayerNoFX );
}


/**************************************************************************************/

- (void)updatePreview {
	// ask for Pixmap thumbnail of frontmost image sized to fit in our thumbnail space

    PreviewType preview = [self getPreviewType];
	if (preview == eNoPreview)
		return;

    {
        //reduce the frequency of thumnail updates to kThumbInterval

        NSTimeInterval now = [NSDate timeIntervalSinceReferenceDate];
        
        if (now < sNextPreviewTime)
        {
            if (!sScheduledPreview)
                {
                [self performSelector:@selector(updatePreview)
                            withObject:nil  afterDelay:sNextPreviewTime - now];
                sScheduledPreview = true;
                }
            return;
        }
     
        [EchoClientAppDelegate cancelPreviousPerformRequestsWithTarget:self
                                        selector:@selector(updatePreview) object:nil];
        sScheduledPreview = false;
        sNextPreviewTime = now + kPreviewInterval;
    }
    
    NSString * stringToSend = nil;

    if (preview == eActiveDocPixMap || preview == eActiveDocJPEG)
        {
        stringToSend = [NSString stringWithFormat:@"var idNS = stringIDToTypeID( \"sendDocumentThumbnailToNetworkClient\" );"
                                                                "var desc1 = new ActionDescriptor();"
                                                                "desc1.putInteger( stringIDToTypeID( \"width\" ), %d );"
                                                                "desc1.putInteger( stringIDToTypeID( \"height\" ), %d );"
                                                                "desc1.putInteger( stringIDToTypeID( \"format\" ), %d );"
                                                                "executeAction( idNS, desc1, DialogModes.NO );",
                                                                (int)GetMaxThumbnailSize().width,
                                                                (int)GetMaxThumbnailSize().height,
                                                                 preview == eActiveDocJPEG  ? 1 : 2 ];
        }
    else if (preview == eSelectedLayerShape)
        {
        stringToSend = [NSString stringWithFormat:@"var idNS = stringIDToTypeID( \"sendLayerShapeToNetworkClient\" );"
															"var desc1 = new ActionDescriptor();"
                                                            "desc1.putBoolean( stringIDToTypeID( \"selectedLayers\" ), true );"
                                                            "desc1.putString( stringIDToTypeID( \"version\" ), \"1.0.0\" );"
															"executeAction( idNS, desc1, DialogModes.NO );"];
        }
    else
        {
        NSString* interpolationTypes [] = {
            @"nearestNeighbor",
            @"bilinear",
            @"bicubic",
            @"bicubicSmoother",
            @"bicubicSharper",
            @"bicubicAutomatic",
            @"preserveDetailsUpscale",
            @"automaticInterpolation"};
        
        double thumbXScale = [thumbnailXScaleFactorText doubleValue];
        double thumbYScale = ([thumbnailScaleLinked state] == NSOnState) ? thumbXScale : [thumbnailYScaleFactorText doubleValue];
        NSInteger interpolationType = [interpolationMethod indexOfSelectedItem];
     
        const NSInteger kMaxInterpolation = sizeof(interpolationTypes)/sizeof(const char*) - 1;

        if (interpolationType < 0)
            interpolationType = 0;
        else if (interpolationType > kMaxInterpolation)
            interpolationType = kMaxInterpolation;

        NSString* thumbnailInterpolation = interpolationTypes[interpolationType];
        NSString* transformInterpolation = interpolationTypes[interpolationType];
        NSInteger format = 2; //only pixmap supported
  
        bool clipToDocumentBounds = [clipLayerThumbsToDocMenuItem state] == NSOnState;

        bool forceDumbScaling = [forceDumbScalingMenuItem state] == NSOnState;
        bool forceSmartPSDPixelScaling = [forceSmartPSDPixelScalingMenuItem state] == NSOnState;

		//force fx to scale to width
        bool forceFXScaling = [forceFXScalingMenuItem state] == NSOnState;

		//use rect/quadrangle form for transform (otherwise use percentage)
        bool useRectScaling = [useRectScalingMenuItem state] == NSOnState;

        bool includeAdjustors = [includeAdjustorsMenuItem state] == NSOnState;
        bool includeAncestors = [includeAncestorsMenuItem state] == NSOnState;
        bool includeChildren = [includeChildrenMenuItem state] == NSOnState;
        bool includeClipBase = [includeClipBaseMenuItem state] == NSOnState;
        bool includeClipped = [includeClippedMenuItem state] == NSOnState;
 
        NSSize thumbScaleSize = NSMakeSize(thumbXScale,thumbYScale);


        //grab comp from menu
        //index 0 is cur document state (i.e. no comp);
        //index 1 is last document state; (comp id 0)
        //then all the named comps are listed (we compIndex = menu index - 2)

        NSInteger compMenuIndex = [compsMenu indexOfSelectedItem];
        NSString* whichCompStr = @""; //no comp

        if (compMenuIndex == 1)
            whichCompStr =  @"desc1.putInteger( stringIDToTypeID( \"compID\" ), 0 );"; //last doc state "comp"
        else if (compMenuIndex > 1)
            whichCompStr =  [NSString stringWithFormat: @"desc1.putInteger( stringIDToTypeID( \"compIndex\" ), %d );", (int)compMenuIndex - 2];

        //selected layers by default

        NSString* whichLayerStr = @"desc1.putBoolean( stringIDToTypeID( \"selectedLayers\" ), true );";

		if (preview == eAllLayers)
			{
            whichLayerStr = @"";
			}

        else if (preview == eSelectedLayerNoFX)
            {
            //test first/lastLayer form by switching off fx in all layers
            whichLayerStr = @"var numLayers = 0;"
							"if (activeDocument.layers.length) {"
							"numLayers = activeDocument.layers.length;"
                            "desc1.putInteger( stringIDToTypeID( \"firstLayer\" ), 0);"
                            "desc1.putInteger( stringIDToTypeID( \"lastLayer\" ), numLayers - 1 );"
							"desc1.putBoolean( stringIDToTypeID( \"selectedLayers\" ), true );}"
                            "var settingItem = new ActionDescriptor();"
                            "var settingsDesc = new ActionDescriptor();"
                            "var lsID = stringIDToTypeID( \"layerSettings\" );"
                            "var fxID = stringIDToTypeID( \"layerEffects\" );"
                            "var fxDesc = new ActionDescriptor();"
                            "settingsDesc.putObject( fxID, fxID, fxDesc );"
                            "settingItem.putObject( lsID, lsID, settingsDesc );"
                            "var settingsList = new ActionList();"
                            "for (var i = 0; i < numLayers; i++) {settingsList.putObject( lsID, settingItem );}"
                            "desc1.putList( lsID, settingsList );";
           }

        //if a scale factor was specified, add it to the thumbnail action.
        //the layer/layers will be transformed by transform before the thumbnail is built
        //which allows you to do smooth up/downsampling of non-pixel layers (text/smartobj/shapes)

		//scale with width/height?
		bool usePercentScaling = !useRectScaling || forceFXScaling;

		//if non-uniform scale, need both scale and rect/quad
		if ((forceFXScaling && (thumbScaleSize.width != thumbScaleSize.height)))
			useRectScaling = true;

		NSString* transformStr = @"";

		if (usePercentScaling)
			{
			NSString* transformScaleStr = [NSString stringWithFormat:
					@"transform1.putDouble( stringIDToTypeID( \"width\" ), %f );"
					"transform1.putDouble( stringIDToTypeID( \"height\" ), %f );",
							thumbScaleSize.width*100.0,
							(forceFXScaling ? thumbScaleSize.width:thumbScaleSize.height)*100.0];

			transformStr = [transformStr stringByAppendingString: transformScaleStr];
			}

		if (useRectScaling)
			{
			NSString* transformRectStr = [NSString stringWithFormat:
						@"var inputRect = new ActionList();"
							"const unitPixels = app.charIDToTypeID('#Pxl');"
                            "inputRect.putUnitDouble(unitPixels, %f );"
                            "inputRect.putUnitDouble(unitPixels, %f );"
                            "inputRect.putUnitDouble(unitPixels, %f );"
                            "inputRect.putUnitDouble(unitPixels, %f );"
                            "transform1.putList( stringIDToTypeID( \"rectangle\" ), inputRect );"
							"var outputQuad = new ActionList();"
                           "outputQuad.putUnitDouble(unitPixels, %f );"
                           "outputQuad.putUnitDouble(unitPixels, %f );"
                            "outputQuad.putUnitDouble(unitPixels, %f );"
                            "outputQuad.putUnitDouble(unitPixels, %f );"
                            "outputQuad.putUnitDouble(unitPixels, %f );"
                            "outputQuad.putUnitDouble(unitPixels, %f );"
                            "outputQuad.putUnitDouble(unitPixels, %f );"
                            "outputQuad.putUnitDouble(unitPixels, %f );"
                            "transform1.putList( stringIDToTypeID( \"quadrilateral\" ), outputQuad );",
							0., 0., 1., 1.,
							//1., 1., 2., 2., //test offsets

							0., 0.,
							thumbScaleSize.width, 0., 
							thumbScaleSize.width, thumbScaleSize.height,
							0., thumbScaleSize.height];
			transformStr = [transformStr stringByAppendingString: transformRectStr];
			}


        stringToSend = [NSString stringWithFormat:
                        @"var idNS = stringIDToTypeID( \"sendLayerThumbnailToNetworkClient\" );"
                        "var desc1 = new ActionDescriptor();"
                        "desc1.putInteger( stringIDToTypeID( \"width\" ), %d );"
                        "desc1.putInteger( stringIDToTypeID( \"height\" ), %d );"
                        "desc1.putInteger( stringIDToTypeID( \"format\" ), %d );"
                        "%@" //code to specify layer
                        "var transform1 = new ActionDescriptor();"
                        "%@" //code to specify transform
                        "transform1.putBoolean( stringIDToTypeID( \"forceDumbScaling\" ), %s );"
                        "transform1.putBoolean( stringIDToTypeID( \"forceSmartPSDPixelScaling\" ), %s );"
                        "transform1.putEnumerated( stringIDToTypeID( \"interpolation\" ), stringIDToTypeID( \"interpolationType\" ), stringIDToTypeID( \"%@\" ) );"
                        "desc1.putObject( stringIDToTypeID( \"transform\" ), stringIDToTypeID( \"transform\" ), transform1 );"
                        "desc1.putEnumerated( stringIDToTypeID( \"interpolation\" ), stringIDToTypeID( \"interpolationType\" ), stringIDToTypeID( \"%@\" ) );"
                        "%@" //code to specify comp
                        "desc1.putBoolean( stringIDToTypeID( \"clipToDocumentBounds\" ), %s );"
                        "desc1.putEnumerated( stringIDToTypeID( \"includeAdjustors\" ), stringIDToTypeID( \"includeLayers\" ), stringIDToTypeID( \"%s\" ) );"
                        "desc1.putEnumerated( stringIDToTypeID( \"includeAncestors\" ), stringIDToTypeID( \"includeLayers\" ), stringIDToTypeID( \"%s\" ) );"
                        "desc1.putEnumerated( stringIDToTypeID( \"includeChildren\" ), stringIDToTypeID( \"includeLayers\" ), stringIDToTypeID( \"%s\" ) );"
                        "desc1.putEnumerated( stringIDToTypeID( \"includeClipBase\" ), stringIDToTypeID( \"includeLayers\" ), stringIDToTypeID( \"%s\" ) );"
                        "desc1.putEnumerated( stringIDToTypeID( \"includeClipped\" ), stringIDToTypeID( \"includeLayers\" ), stringIDToTypeID( \"%s\" ) );"
                        "desc1.putBoolean( stringIDToTypeID( \"sendThumbnailPixels\" ), true );"
                        "desc1.putBoolean( stringIDToTypeID( \"sendThumbnailBounds\" ), true );"
                        "desc1.putBoolean( stringIDToTypeID( \"sendThumbnailProfile\" ), true );"
                        "desc1.putBoolean( stringIDToTypeID( \"sendThumbnailMissingFonts\" ), true );"
                        "executeAction( idNS, desc1, DialogModes.NO );",
                                (int)GetMaxThumbnailSize().width,
                                (int)GetMaxThumbnailSize().height,
                                (int)format,
                                whichLayerStr,
								transformStr,
                                forceDumbScaling ? "true" : "false",
                                forceSmartPSDPixelScaling ? "true" : "false",
                                transformInterpolation,
                                thumbnailInterpolation,
                                whichCompStr,
                                clipToDocumentBounds ? "true" : "false",
                                includeAdjustors ? "includeVisible" : "includeNone",
                                includeAncestors ? "includeAll" : "includeNone",
                                includeChildren ? "includeVisible" : "includeNone",
                                includeClipBase ? "includeAll" : "includeNone",
                                includeClipped ? "includeVisible" : "includeNone"
                                   ];

        }
  
    sPreviewTransaction = [ self sendJavaScript: stringToSend ];
}

/**************************************************************************************/

- (void)previewTypeChanged:(id)sender {

    [self enableDocControls: YES ]; //make sure the other menus enable states match the preview type

    [self getMenuInfo]; //get more info for menus whenever comps/active doc changes (currently, the list of comps)

    [self updatePreview];
}

/**************************************************************************************/

- (IBAction)togglePSMenuState:(id)sender  {

    [sender setState: [sender state] ? NSOffState : NSOnState];

    [self setNodeMenuStates];
}

/**************************************************************************************/

- (IBAction)toggleChangeSubscription:(id)sender  {

	[sender setState: [sender state] ? NSOffState : NSOnState];

	[ self setupGeneratorSubscriptions ];
}

/**************************************************************************************/

- (IBAction)toggleMenuItem:(id)sender  {

    [sender setState: [sender state] ? NSOffState : NSOnState];
    [ self updatePreview ]; //most of the menu items affect the thumb parameters, so invalidate
}

/**************************************************************************************/

- (IBAction)releaseSmartObjects:(id)sender  {
    
        NSString * stringToSend = [NSString stringWithFormat:
                                   @"var idNS = stringIDToTypeID( \"releaseSmartObjects\" );"
                                   "var desc = new ActionDescriptor();"
                                   "executeAction( idNS, desc, DialogModes.NO );"  ];
        sMenuInfoTransaction = [ self sendJavaScript: stringToSend ];
    }


/**************************************************************************************/

- (IBAction)addGeneratorMenus:(id)sender {

   // pretend we are the node process. it's fun to pretend.
    //this will ensure that generator is quit if it's running, and
    //it isn't launched when our menus are selected.
    //(it takes generator out of the picture entirely)
    
    [generatorMenusAddedMenuItem setState:YES];
    [generatorMenusAddedMenuItem setEnabled:NO]; //we can't turn this off without killing ourselves

    //enable the menu commands, we are going to take over the menus, too.
    [generatorMenuItem1 setEnabled:YES];
    [generatorMenuItem2 setEnabled:YES];
    [generatorMenusEnabledMenuItem setEnabled:YES];

    [self sendNodeMenuInitialize];
    [self setNodeMenuStates];
}

/**************************************************************************************/

- (void)updateTestResults:(BOOL)successful {

	NSError* error = nil;
	NSStringEncoding enc;
	NSString* logContents =  [NSString stringWithContentsOfFile:[[EchoClient get] logFilePath] usedEncoding:&enc error:&error];

	[self setResponse: logContents ? logContents : (successful ? @"TEST SUCCESS" : @"TEST FAILURE") error: !successful];
}

/**************************************************************************************/

- (IBAction)cancelCurrentNetworkCommand:(id)sender {
    [self sendCancelCommandMessage: 0 ];
}

/**************************************************************************************/

struct StTest {
    EchoClientAppDelegate* fApp;
    StTest(EchoClientAppDelegate* inApp) : fApp(inApp){
        StartTestMode();
        [inApp setupGeneratorSubscriptions]; //put subscriptions in test mode (mark them as odd)
    }
    ~StTest() {
        EndTestMode();
        [fApp setupGeneratorSubscriptions];
    }
};

/**************************************************************************************/

- (IBAction)runDefaultTest:(id)sender {

    StTest test(self);

	bool init = true;
	bool success = TestOneFile(NULL, NULL, 30.0,init) == 0;
	init = false;

	[self  updateTestResults:success];
}

/**************************************************************************************/

- (IBAction)runChosenFiles:(id)sender {
 
    StTest test(self);

	bool success = true;
    
	NSOpenPanel* openDlg = [NSOpenPanel openPanel];
    
    [openDlg setCanChooseFiles:YES];
    [openDlg setAllowedFileTypes:nil];
    [openDlg setAllowsMultipleSelection:TRUE];
    [openDlg setMessage:@"Choose one or more files to test:"];
    if ( [openDlg runModal] == NSModalResponseOK )
        {
        NSArray *testFiles = [openDlg URLs];
        bool init = true;
        for( int tt = 0; success && tt < [testFiles count]; ++tt )
            {
            success = TestOneFile([[[testFiles objectAtIndex:tt] path] UTF8String], nil, 30.0, init) == 0;
            init = false;
            }
        [self  updateTestResults:success];
        }
}

/**************************************************************************************/

- (IBAction)openTestFile:(id)sender {

    NSString* kDefaultTestFile = @"Shapes.psd";

    NSString* filePath =  [[NSBundle mainBundle] pathForResource:kDefaultTestFile ofType:nil];

    //wait for the service to connect.
    //We are in batch mode, so this is all we can do to get something to send our Javascript to.

    if (filePath == nil)
        [ self setResponse: [NSString stringWithFormat:@"ERROR: Couldn't load test file at %@", filePath]
                error: TRUE];
    else
        [ self sendJavaScript: [NSString stringWithFormat:@"app.open (new File(\"%@\"));", filePath ] ];
}

/**************************************************************************************/

- (IBAction)openChosenFile:(id)sender {

	NSOpenPanel* openDlg = [NSOpenPanel openPanel];
    
    [openDlg setCanChooseFiles:YES];
    [openDlg setAllowedFileTypes:nil];
    [openDlg setAllowsMultipleSelection:TRUE];
    [openDlg setMessage:@"Choose one or more files to open in Photoshop:"];
    if ( [openDlg runModal] == NSModalResponseOK )
        {
        NSArray *testFiles = [openDlg URLs];
        for( int tt = 0;  tt < [testFiles count]; ++tt )
            {
			NSString* filePath = [[testFiles objectAtIndex:tt] path];

			//wait for the service to connect.
			//We are in batch mode, so this is all we can do to get something to send our Javascript to.

			if (filePath == nil) {
				[ self setResponse: [NSString stringWithFormat:@"ERROR: Couldn't open file at %@", filePath]
						error: TRUE];
				break;
				}
			else
				[ self sendJavaScript: [NSString stringWithFormat:@"app.open (new File(\"%@\"));", filePath ] ];

            }
        }
}

/**************************************************************************************/
//send raw javascript to PS.  We try to intuit the response (thumb/javascript/other), and present
//it in the appropriate way


- (IBAction)sendUserJS:(id)sender {

	NSOpenPanel* openDlg = [NSOpenPanel openPanel];
	NSArray *fileTypesArray = [NSArray arrayWithObjects:@"js", @"jsx", nil];
	[openDlg setCanChooseFiles:YES];
	[openDlg setAllowedFileTypes:fileTypesArray];
	[openDlg setAllowsMultipleSelection:YES];
	[openDlg setMessage: @"Choose Javascript to send to Photoshop:"];

	if ( [openDlg runModal] == NSModalResponseOK && [[openDlg URLs] count] ) {

		NSError* error = nil;
		NSStringEncoding enc;
		NSString* jsPath = [[[openDlg URLs] objectAtIndex:0] path];
		NSString* jsContents = [NSString stringWithContentsOfFile:jsPath usedEncoding: &enc error:&error];

		sUserJSTransaction = -1;
		sUserJSTransaction = [ self sendJavaScript: jsContents];
		[ self setResponse: @"JavaScript File Result:\n" error:NO]; //clear out existing response
	}
}

/**************************************************************************************/

- (IBAction)runChosenTests:(id)sender {

    StTest test(self);

	bool success = true;
	NSOpenPanel* openDlg = [NSOpenPanel openPanel];
	NSArray *fileTypesArray = [NSArray arrayWithObjects:@"js", @"jsx", nil];
	[openDlg setCanChooseFiles:YES];
	[openDlg setAllowedFileTypes:fileTypesArray];
	[openDlg setAllowsMultipleSelection:TRUE];
	[openDlg setMessage:@"Choose one or more Javascript tests to run:"];

	if ( [openDlg runModal] == NSModalResponseOK ) {
		NSArray *jsFiles = [openDlg URLs];
		bool init = true;
		for( int jj = 0; success && jj < [jsFiles count]; ++jj ) {
			success = TestOneFile(NULL, [[[jsFiles objectAtIndex:jj] path] UTF8String], 30.0,init) == 0;
			init = false;
		}
        [self  updateTestResults:success];
	}
}

/**************************************************************************************/

- (IBAction)runChosenTestsAndFiles:(id)sender {

    StTest test(self);
    
	bool success = true;

	NSOpenPanel* openDlg = [NSOpenPanel openPanel];
	NSArray *fileTypesArray = [NSArray arrayWithObjects:@"js", @"jsx", nil];
	[openDlg setCanChooseFiles:YES];
	[openDlg setAllowedFileTypes:fileTypesArray];
	[openDlg setAllowsMultipleSelection:TRUE];
	[openDlg setMessage:@"Choose one or more Javascript tests to run:"];

	if ( [openDlg runModal] == NSModalResponseOK ) {
		NSArray *jsFiles = [openDlg URLs];

		[openDlg setCanChooseFiles:YES];
		[openDlg setAllowedFileTypes:nil];
		[openDlg setAllowsMultipleSelection:TRUE];
		[openDlg setMessage:@"Choose one or more files to test:"];
		if ( [openDlg runModal] == NSModalResponseOK )
			{
			NSArray *testFiles = [openDlg URLs];
			bool init = true;
			for( int jj = 0; success && jj < [jsFiles count]; ++jj )
				for( int tt = 0; success && tt < [testFiles count]; ++tt )
					{
					success = TestOneFile([[[testFiles objectAtIndex:tt] path] UTF8String], [[[jsFiles objectAtIndex:jj] path] UTF8String], 30.0, init) == 0;
					init = false;
					}
            [self  updateTestResults:success];
			}
	}
}

/**************************************************************************************/

- (void)getMenuInfo {
	// get just enough doc info to set up popup menus
	
    NSString * stringToSend = [NSString stringWithFormat: @"var idNS = stringIDToTypeID( \"sendDocumentInfoToNetworkClient\" );"
                               "var desc = new ActionDescriptor();"
                               "desc.putInteger( stringIDToTypeID( \"compInfo\" ), true);"
                               "desc.putInteger( stringIDToTypeID( \"imageInfo\" ), true);"
                               "desc.putString( stringIDToTypeID( \"version\" ), \"%s\");"
                               "executeAction( idNS, desc, DialogModes.NO );", sFormatVersion  ];
	sMenuInfoTransaction = [ self sendJavaScript: stringToSend ];
}

/**************************************************************************************/

- (transaction_id)sendDocInfoRequestWithSelectedLayers:(bool)forSelectedLayers {
	// force events that provide image state

    bool fullTextStyles = [getAllTextStyleInfoMenuItem state] == NSOnState;
    bool defaultFX = [getDefaultLayerFXMenuItem state] == NSOnState;

    NSString * stringToSend = [NSString stringWithFormat:
                                @"var idNS = stringIDToTypeID( \"sendDocumentInfoToNetworkClient\" );"
                                "var desc = new ActionDescriptor();"
                                "desc.putBoolean( stringIDToTypeID( \"expandSmartObjects\" ), true);"
                                "desc.putBoolean( stringIDToTypeID( \"getTextStyles\" ), true);"
                                "desc.putBoolean( stringIDToTypeID( \"getFullTextStyles\" ), %s);"
                                "desc.putBoolean( stringIDToTypeID( \"getDefaultLayerFX\" ), %s);"
                                "desc.putBoolean( stringIDToTypeID( \"getCompLayerSettings\" ), true);"
                                "desc.putBoolean( stringIDToTypeID( \"getPathData\" ), true);"
                                "desc.putBoolean( stringIDToTypeID( \"selectedLayers\" ), %s);"
                                "desc.putString( stringIDToTypeID( \"version\" ), \"%s\");"
                                "executeAction( idNS, desc, DialogModes.NO );",
                                fullTextStyles  ? "true" : "false",
                                defaultFX  ? "true" : "false",
                                forSelectedLayers  ? "true" : "false",
                                sFormatVersion ];
                                
    return [ self sendJavaScript: stringToSend ];
    }

/**************************************************************************************/

- (IBAction)sendInfoPressed:(id)sender {

   sSendInfoTransaction = [self sendDocInfoRequestWithSelectedLayers:NO];
}

/**************************************************************************************/

- (IBAction)sendLayerInfoPressed:(id)sender {
	// force events that provide image state

   sSendLayerInfoTransaction = [self sendDocInfoRequestWithSelectedLayers:YES];
}

/**************************************************************************************/

- (IBAction)saveICCProfile:(id)sender {

    
    NSSavePanel* saveDlg = [NSSavePanel savePanel];
    
    [saveDlg setAllowedFileTypes:nil];
    [saveDlg setMessage:@"Specify where to save profile:"];
    
    saveDlg.nameFieldStringValue = @"profile.icc";
    
    if ( [saveDlg runModal] == NSModalResponseOK )
        {
        sProfileURL = [[saveDlg URL] retain];
 

		//send profile of active document (alternatively, specify "profile" description as string,
		//or another document reference form (similar to other APIs that reference document like sendLayeThumbnailToNetworkClient)
        NSString * stringToSend = [NSString stringWithFormat:
                                   @"var idNS = stringIDToTypeID( \"sendProfileToNetworkClient\" );"
                                   "var desc = new ActionDescriptor();"
                                   
                                   //sendLayerThumbnailToNetworkClient: [bool] send back profile for pixels returned by sendLayerThumbnailToNetworkClient
                                   //optionally specify relevant sendLayerThumbnailToNetworkClient used (convertToWorkingRGBProfile and/or useICCProfile)
                                    "desc.putBoolean( stringIDToTypeID( \"sendLayerThumbnailToNetworkClient\" ), true);"
                                   //   "desc.putBooleang( stringIDToTypeID( \"convertToWorkingRGBProfile\" ), true);"
                                   //   -or-
                                   //   "desc.putString( stringIDToTypeID( \"useICCProfile\" ), \"sRGB IEC61966-2.1\");"
                                   // -or-
                                   //profile: [string] send back profile for profile with description "profile"
                                  // "desc.putString( stringIDToTypeID( \"profile\" ), \"sRGB IEC61966-2.1\");"
                                   "executeAction( idNS, desc, DialogModes.NO );" ];
      
        sProfileTransaction = [ self sendJavaScript: stringToSend ];
        }
}

/**************************************************************************************/
//perform some JS on a property reference of the target layer or doc

- (void)sendRefJS:(NSString*)inJS toClass:(NSString*)inClass andProperty:(NSString*)inProperty {

    [ self sendJavaScript: [NSString stringWithFormat:
        //some useful definitions, first
       @"var classProperty = charIDToTypeID('Prpr');"
        "var propNull = charIDToTypeID( 'null' );"
        "var classNull = charIDToTypeID( 'null' );"
        "var typeOrdinal = charIDToTypeID('Ordn');"
        "var enumTarget = charIDToTypeID('Trgt');"
        "var classApplication = charIDToTypeID('capp');"
        "var classDocument = charIDToTypeID('Dcmn');"
        "var classLayer = charIDToTypeID('Lyr ');"
        "var propGeneratorSettings = stringIDToTypeID(\"generatorSettings\");"
        "var propJSON = stringIDToTypeID(\"json\");"
        "var propBla = stringIDToTypeID(\"bla\");"
        "var propProperty = stringIDToTypeID(\"property\");"
        "var strEchoClient = \"echoClient\";"
        "var keyTo = charIDToTypeID( 'T   ' );"
        "var actionSet = charIDToTypeID( \"setd\" );"
        "var actionGet = charIDToTypeID( \"getd\" );"
        "var actionSendJSON = stringIDToTypeID( \"sendJSONToNetworkClient\" );"
        "var theRef = new ActionReference();"
//NOTE: property needs to be defined first in ref or it won't work.
        "theRef.putProperty(classProperty, %@ );"
        "theRef.putEnumerated(%@, typeOrdinal, enumTarget);"
        "%@",
      inProperty,
      inClass,
      inJS]
    ];
}

/**************************************************************************************/

- (void)getJSONPropertyForClass:(NSString*)inClass   {
	
     [ self sendRefJS:
        [NSString stringWithFormat:
        @"var getDescriptor = new ActionDescriptor();"
        "getDescriptor.putReference(propNull,theRef);"
		//this line here lets us focus on just one key in generator settings (this is how a plugin would get its own settings)
        "getDescriptor.putString(propProperty,strEchoClient);"
        "var desc = executeAction( actionGet, getDescriptor, DialogModes.NO );"
        "executeAction( actionSendJSON, desc, DialogModes.NO );"]
        toClass: inClass
        andProperty:@"propJSON"
        ];
}

/**************************************************************************************/
//General JS getter/setter
//These do all the boiler plate of setting up the reference and executing the JS

- (void)setGeneratorPropertyForClass:(NSString*)inClass usingJS:(NSString*)inJS  {
	
    //add new setting each time the menu is called
    //this should produce an imageChanged event, and persist in the image document after a save
     
    [ self sendRefJS:
    
        [NSString stringWithFormat:
        @"var desc = new ActionDescriptor();"
        "desc.%@;" //the javascript to execute on the desc to set it up.

        "var setDescriptor = new ActionDescriptor();"
        "setDescriptor.putReference(propNull,theRef);"
        "setDescriptor.putObject(keyTo,classNull,desc);"
 		//this line here lets us focus on just one key in generator settings (this is how a plugin would set its own settings)
       "setDescriptor.putString(propProperty,strEchoClient);"
        "executeAction( actionSet, setDescriptor, DialogModes.NO );"
        , inJS]
        toClass: inClass
        andProperty:@"propGeneratorSettings"
    ];
}

/**************************************************************************************/

- (void)getGeneratorPropertyForClass:(NSString*)inClass  {
	
     [ self sendRefJS:
        @"var getDescriptor = new ActionDescriptor();"
        "getDescriptor.putReference(propNull,theRef);"
		//this line here lets us focus on just one key in generator settings (this is how a plugin would get its own settings)
        "getDescriptor.putString(propProperty,strEchoClient);"
        "var desc = executeAction( actionGet, getDescriptor, DialogModes.NO );"
        "executeAction( actionSendJSON, desc, DialogModes.NO );"
        toClass: inClass
        andProperty:@"propGeneratorSettings"
        ];
}

/**************************************************************************************/
//menu items

- (IBAction)getLayerJSON:(id)sender {
    [ self getJSONPropertyForClass: @"classLayer" ];
}


- (IBAction)getDocJSON:(id)sender {
    [ self getJSONPropertyForClass: @"classDocument" ];
}

- (IBAction)getLayerGeneratorSettings:(id)sender {
    [ self getGeneratorPropertyForClass: @"classLayer" ];
}


- (IBAction)getDocGeneratorSettings:(id)sender {
    [ self getGeneratorPropertyForClass: @"classDocument" ];
}

- (IBAction)setLayerGeneratorSettings:(id)sender {
    [ self setGeneratorPropertyForClass: @"classLayer" usingJS: @"putBoolean(propBla,true)"];
}

- (IBAction)setDocGeneratorSettings:(id)sender {
    [ self setGeneratorPropertyForClass: @"classDocument" usingJS:@"putBoolean(propBla,true)"];
}

/**************************************************************************************/

- (void)enableDocControls:(BOOL)enabled {
	// force events that provide image state

    [previewType setEnabled: enabled ];

    BOOL canScale = [self canScale];
    BOOL canInterpolate = [self canInterpolate];
    
    [interpolationMethod setEnabled: enabled && canInterpolate ];

    [thumbnailXScaleFactorText setEnabled: enabled && canScale];
    [thumbnailYScaleFactorText setEnabled: enabled && canScale];
    [thumbnailScaleLinked setEnabled: enabled && canScale];
   
    if (enabled == NO)
        [compsMenu setEnabled: NO ]; //to enable, we need more info about cur doc's comps (updateMenus)


    [cancelNetworkCommand setEnabled: sCommandTransactionInProgress > 0];
}

/**************************************************************************************/

- (void)updateMenus:(NSDictionary*)docInfoDict {
	// force events that provide image state

    if (docInfoDict == nil)
        return;
 
    //if we get a doc info dict (send doc info), it means we have an active doc.
    [self enableDocControls: YES ];
 
    //the rest is to setup/enable the comps menu
    //assume new format
    
    NSArray* comps = [docInfoDict objectForKey:@"comps"];

    NSInteger oldIndex = [compsMenu indexOfSelectedItem];
    [compsMenu removeAllItems];
    [compsMenu addItemWithTitle:@"Current Document State"];
    
    static NSArray* sLastCompArray = nil;
    
    if (comps != nil)
        {
        NSMutableArray* curCompArray = [NSMutableArray array];
        [compsMenu addItemWithTitle:@"Last Document State"];

        for (NSDictionary* comp in comps)
            {
            NSString* compName = [comp objectForKey:@"name"];
            if (compName)
                [compsMenu addItemWithTitle:compName];
                    
            NSInteger compID = [[comp objectForKey:@"id"] integerValue];
            [curCompArray addObject: [NSNumber numberWithInteger: compID]];
           }
        if (sLastCompArray && curCompArray && [sLastCompArray isEqualToArray:curCompArray])
            [compsMenu selectItemAtIndex:oldIndex]; //if we didn't change the comp set, restore the selection
          
        [sLastCompArray release];
        sLastCompArray = [curCompArray retain];
        }

    [compsMenu setEnabled: [self canComp]];
}

/**************************************************************************************/

- (IBAction)setPassword:(id)sender {

	[[EchoClient get] setPassword: [sender stringValue]];
}

/**************************************************************************************/

#pragma mark -
#pragma mark Service list action method

- (IBAction)serverClicked:(id)sender {
    NSTableView * table = (NSTableView *)sender;
    int selectedRow = (int)[table selectedRow];

	if (-1 != selectedRow)
        [[EchoClient get] connectToNthService: selectedRow];
}

/**************************************************************************************/

- (IBAction)interpolationChanged:(id)sender {
    [self updatePreview]; //update the layer thumb when it changes
}

/**************************************************************************************/

static bool sScaleFactorDebounce = false;

- (IBAction)scaleFactorTextChanged:(id)sender {
    if (sScaleFactorDebounce)
        return;
    sScaleFactorDebounce = true;
 
    if ([thumbnailScaleLinked state] == NSOnState)
        {
        if (sender == thumbnailXScaleFactorText)
            [thumbnailYScaleFactorText setDoubleValue:[thumbnailXScaleFactorText doubleValue]];
        if (sender == thumbnailYScaleFactorText)
            [thumbnailXScaleFactorText setDoubleValue:[thumbnailYScaleFactorText doubleValue]];
        }
    
    [self updatePreview]; //update the layer thumb when it changes
    sScaleFactorDebounce = false;
}

/**************************************************************************************/

#pragma mark -
#pragma mark current color display code

- (void)getGroundColors {

	// get foreground
	{
    NSString * stringToSend = [NSString stringWithUTF8String:"\"ignored\\r\"+app.foregroundColor.rgb.hexValue.toString();"];
	sForegroundColorTransaction = [ self sendJavaScript: stringToSend ];
	}

	// get background
	{
    NSString * stringToSend = [NSString stringWithUTF8String:"\"ignored\\r\"+app.backgroundColor.rgb.hexValue.toString();"];
	sBackgroundColorTransaction = [ self sendJavaScript: stringToSend ];
	}
}

/**************************************************************************************/

static NSColor * ColorFromString( NSString *hexColor )
{
	if ( [hexColor length] < 6)
		return NULL;

	long color_value = strtol( [hexColor UTF8String], (char **)NULL, 16 );
	
	unsigned char red = (color_value >> 16) & 0xFF;
	unsigned char green = (color_value >> 8) & 0xFF;
	unsigned char blue = color_value & 0xFF;

	NSColor *theColor = [ NSColor colorWithDeviceRed: (red/255.0f) green:(green/255.0f) blue:(blue/255.0f) alpha: 1.0 ];

	return theColor;
}

/**************************************************************************************/

static NSColor * ColorFromResponseString( NSString *string )
{
	// parse the string returned, of the style:     "toolChanged\rsomeTool"
	// find first return, take substring from that index
	NSRange range = [string rangeOfString:@"\r" ];
	if (range.length == 0)
		return NULL;
	
	NSString *hexColor = [string substringFromIndex: (range.location + 1) ];
	
	return ColorFromString( hexColor );
}

/**************************************************************************************/

- (void)processForegroundChange: (NSString *)string {

	NSColor *theColor = ColorFromResponseString( string );
	
	if (theColor != NULL)
		[ foregroundColorWidget setColor: theColor ];
}

/**************************************************************************************/

- (void)processBackgroundChange: (NSString *)string {

	NSColor *theColor = ColorFromResponseString( string );
	
	if (theColor != NULL)
		[ backgroundColorWidget setColor: theColor ];
}

/**************************************************************************************/

-(void) clearThumbnailImage
    {
    [imageWidget setImage: nil];
    [thumbnailDimensions setStringValue:@""];
    }

/**************************************************************************************/

-(void) setThumbnailRect:(NSRect)inThumbnailRect
    {
	NSString* dimensionsStr = [NSString stringWithFormat: @"top:%.3f left:%.3f bottom:%.3f right:%.3f\nheight:%.3f width:%.3f ",
							inThumbnailRect.origin.y, inThumbnailRect.origin.x,
							inThumbnailRect.origin.y+inThumbnailRect.size.height, inThumbnailRect.origin.x+inThumbnailRect.size.width,
							inThumbnailRect.size.height,
							inThumbnailRect.size.width];
	[thumbnailDimensions setStringValue:dimensionsStr];
    }

/**************************************************************************************/

-(void) setThumbnailImage:(NSImage*)inThumbnailImage andRect:(NSRect)inThumbnailRect
    {
    [imageWidget setImage: inThumbnailImage];
	[self setThumbnailRect: inThumbnailRect];
    }

/**************************************************************************************/

//message delegates for GUI:

-(void) processErrorMessage:(NSString *)string transaction:(transaction_id) transaction {

	if ([string isEqual: @"Error: No image open"] )
        [self clearThumbnailImage];
	else
		[self setResponse: string error:YES];
}

/**************************************************************************************/

-(void)setResponse:(NSString*)inString error:(BOOL)inError{

    if (inError)
        LogError([inString UTF8String] );

	NSColor* color = [ NSColor colorWithDeviceRed: (inError?1.0:0.0) green:0.0 blue:0.0 alpha: 1.0 ];
	NSFont* font = [NSFont systemFontOfSize: 9.0];

    NSDictionary* attributes =[NSDictionary  dictionaryWithObjectsAndKeys:
                                                color,
                                                NSForegroundColorAttributeName,
                                                font,
                                                NSFontAttributeName,
                                                nil];

	NSAttributedString* string = [[[NSAttributedString alloc]initWithString: inString attributes: attributes] autorelease];

	[[responseField textStorage] setAttributedString: string];

	NSRange range;
    range = NSMakeRange ([[responseField string] length], 0);
    [responseField scrollRangeToVisible: range];
}

/**************************************************************************************/

    //Do some sanity checks on the transaction responses:

-(NSString*) validateMessage:(NSString *)string transaction:(transaction_id) transaction error:(BOOL*)isError jsonDict:(NSDictionary**)jsonDict {

        //We get these as responses when executing the Javascript subscription, ignore them
    *isError = NO;
    *jsonDict = nil;
    NSString* response = string;
    
    if ([string isEqual: @"undefined"] || [string isEqual: @"[ActionDescriptor]"] || IsTestTransaction(transaction))
        response = nil; //ignore
    
        //These should be JSON (after the message identifier line). Verify that is the case
      
    else if (transaction == sImageChangedSubscription
        || transaction == sSendInfoTransaction
        || transaction == sSendLayerInfoTransaction
        || transaction == sMenuChangedSubscription
        || transaction == sMenuInfoTransaction
        || transaction == sEventStatusTransaction
        || transaction == sUserJSTransaction
        || transaction == sPreviewTransaction //this has an optional JSON and an image response
        )
        {
        
        //events have additional identifying strings in front of the JSON
        NSString* headerString = transaction == sImageChangedSubscription ? @"imageChanged\r" :
                                 transaction == sMenuChangedSubscription ? @"generatorMenuChanged\r" :
                                 nil;
        
        NSString* jsonString =  headerString ?
                                    [string stringByReplacingOccurrencesOfString:headerString withString:@""] :
                                    string;
        
        NSError* error = nil;
        
        *jsonDict = [NSJSONSerialization
                          JSONObjectWithData:[jsonString dataUsingEncoding:NSUTF8StringEncoding]
                          options:kNilOptions
                          error:&error];

        if ( (*isError = ((*jsonDict == nil) || error)) && transaction != sUserJSTransaction)
            response = [NSString stringWithFormat: @"Invalid JSON:\r%@", string];
        }
    
        //These should be Colors
    
    else if (transaction == sBackgroundColorSubscription
        || transaction == sForegroundColorSubscription)
        {
        if ( (*isError = (ColorFromResponseString(string) == nil)) )
            response = [NSString stringWithFormat: @"Invalid Color:\r%@", string];
        }

    return response;
}

/**************************************************************************************/

NSPoint GetPathPoint(id point, const NSPoint& defaultPt)
    {
    if (point == nil)
        return defaultPt;
    
    else if ([point isKindOfClass:[NSArray class]])
 
        return NSMakePoint([[point objectAtIndex: 0] doubleValue],
                            [[point objectAtIndex: 1] doubleValue]); //old format
    else
        return  NSMakePoint([[point objectForKey:@"x"] doubleValue],
                              [[point objectForKey:@"y"] doubleValue]); //new format
   }

/**************************************************************************************/

NSRect GetBounds(id bounds)
    {
     if ([bounds isKindOfClass:[NSArray class]])
        {
        double top = [[bounds objectAtIndex: 0] doubleValue];
        double left = [[bounds objectAtIndex: 1] doubleValue];
        double bottom = [[bounds objectAtIndex: 2] doubleValue];
        double right = [[bounds objectAtIndex: 3] doubleValue];
        return NSMakeRect(left,top,(right-left),(bottom-top));
       }
    else
        {
        double top = [[bounds objectForKey:@"top"] doubleValue];
        double left = [[bounds objectForKey:@"left"] doubleValue];
        double bottom = [[bounds objectForKey:@"bottom"] doubleValue];
        double right = [[bounds objectForKey:@"right"] doubleValue];
        return NSMakeRect(left,top,(right-left),(bottom-top));
       }
   }

/**************************************************************************************/

inline bool IsLineTo( const NSPoint& anchorPt, const NSPoint& backwardPt, const NSPoint& forwardPt)
    {
    return  NSEqualPoints(anchorPt,backwardPt) && NSEqualPoints(anchorPt, forwardPt);
    }

/**************************************************************************************/

-(NSRect) getPathsBounds:(NSArray *)pathArray {

    NSRect allPathBounds = NSMakeRect(0,0,0,0);
    for (NSDictionary* pathDict in pathArray)
        allPathBounds = NSUnionRect(allPathBounds, GetBounds([pathDict objectForKey:@"bounds"]));
    return allPathBounds;
    }
    
 /**************************************************************************************/

//test the path info we get back from photoshop by trying to reconstruct the bezier path.
-(void)logException:(NSString *)description  {
    LogError("Exception thrown while %s", description);
    @try
        {
        [self setResponse: [NSString stringWithFormat: @"Exception thrown while %@", description] error:YES];
        }
    @catch(...)
        {
        }
    }

-(void) drawJSONShapeToImage:(NSDictionary *)shapeDict {

    // convert path dict to NSBezier
    
    id value = [shapeDict objectForKey:@"layerChangedPath"];

    if (value == nil)
        value = [shapeDict objectForKey:@"path"];

    NSArray* pathArray = nil;
    
    if ([value isKindOfClass:[NSArray class]])
        pathArray = value;
    else
        pathArray = [NSArray arrayWithObject:value];
    
     NSBezierPath* bez = [NSBezierPath bezierPath];

	NSColor* fillColor = nil;
	NSDictionary* fill = [shapeDict objectForKey:@"fill"];
	if ( fill  )
		{
        NSDictionary* color = [fill objectForKey:@"color"];
		if (color && [color objectForKey:@"red"])

			fillColor = [NSColor colorWithCalibratedRed:  [[color objectForKey:@"red"] doubleValue]
							green: [[color objectForKey:@"green"] doubleValue]
							 blue: [[color objectForKey:@"blue"] doubleValue]
							alpha: 1.0];
		}

    for (NSDictionary* pathDict in pathArray)
        {
        //enumerate the paths, drawing the components into the image.
        NSArray* componentArray = [pathDict objectForKey:@"pathComponents"];

        const NSPoint zeroPoint = NSMakePoint(0,0);
        
        for (NSDictionary* componentDict in componentArray)
            {
            // add/subtract/intersect/xor
           // NSString* operation = [componentDict objectForKey:@"shapeOperation"];
            
            BOOL winding = [[componentDict objectForKey:@"windingFill"] boolValue];
            NSArray* subPathArray = [componentDict objectForKey:@"subpathListKey"];
            
            for (NSDictionary* subPathDict in subPathArray)
                {
                NSArray* knotArray = [subPathDict objectForKey:@"points"];
                BOOL closed = [[subPathDict objectForKey:@"closedSubpath"] boolValue];
                NSPoint anchorPt;
                NSPoint forwardPt;
                NSPoint backwardPt;
                NSPoint firstAnchorPt;
                NSPoint firstBackwardPt;
                bool firstKnot = true;
                
                for (NSDictionary* knotDict in knotArray)
                    {
                    anchorPt = GetPathPoint([knotDict objectForKey:@"anchor"], zeroPoint);
                    backwardPt = GetPathPoint([knotDict objectForKey:@"backward"],anchorPt);
 
                    if (firstKnot)
                        {
                        [bez moveToPoint: anchorPt];
                        firstAnchorPt = anchorPt;
                        firstBackwardPt = backwardPt;
                        }
                    else if (IsLineTo(forwardPt,backwardPt,anchorPt))
                        [bez lineToPoint: anchorPt];
                          
                    else
                        [bez curveToPoint: anchorPt controlPoint1: forwardPt controlPoint2: backwardPt];

                    forwardPt = GetPathPoint([knotDict objectForKey:@"forward"],anchorPt);

                    firstKnot = false;
                    }
                    
                if (closed)
                    {
                     if (IsLineTo(forwardPt,firstBackwardPt,firstAnchorPt))
                        [bez lineToPoint: firstAnchorPt];
                    else
                        [bez curveToPoint: firstAnchorPt controlPoint1: forwardPt controlPoint2: firstBackwardPt];
                        
                    [bez closePath];
                    }
                    
                [bez setWindingRule: winding ? NSNonZeroWindingRule : NSEvenOddWindingRule];
                }
            }
        }


    double thumbXScale = [thumbnailXScaleFactorText doubleValue];
    double thumbYScale = [thumbnailYScaleFactorText doubleValue];
    NSRect allPathBounds = [self getPathsBounds:pathArray];

    NSAffineTransform * matrix = [ NSAffineTransform transform];

    //bezier is in PS doc pixel coordinates (top < bottom, left < right)
    //we need to transform it into Quartz (bottom > top, left < right)
    //and put the bottom right of the bounding box at the origin
    //so we fill up the thumbnail with the path.
    
    //translate to origin, flip coordinate system,
    //translate back to positive quadrant, scale by control amount

    [matrix scaleXBy:thumbXScale yBy: thumbYScale];
    [matrix translateXBy:0 yBy:allPathBounds.size.height];
    [matrix scaleXBy:1 yBy: -1];
    [matrix translateXBy:-allPathBounds.origin.x yBy:-allPathBounds.origin.y];

    bez = [matrix transformBezierPath: bez]; 

    NSAffineTransform * boundsMatrix = [ NSAffineTransform transform];
    [boundsMatrix scaleXBy:thumbXScale yBy: thumbYScale];

    NSSize imageSize = [boundsMatrix transformSize: allPathBounds.size];

    NSColor* checkerBoard = GetCheckerBoard();
    
    NSBitmapImageRep *thumbSizeImageRep = [[[NSBitmapImageRep alloc] initWithBitmapDataPlanes:nil
                                                    pixelsWide: imageSize.width
                                                    pixelsHigh: imageSize.height
                                                    bitsPerSample:8
                                                    samplesPerPixel:3
                                                    hasAlpha:NO
                                                    isPlanar:NO
                                                    colorSpaceName: NSDeviceRGBColorSpace
                                                    bitmapFormat:0
                                                    bytesPerRow:(3 * imageSize.width)
                                                    bitsPerPixel:24] autorelease];
    
	if (thumbSizeImageRep == nil)
        {
    	LogError("drawJSONShapeToImage: NULL thumbSizeImageRep");
        return;
        }
    
    NSImage *image = [[NSImage alloc] initWithSize: imageSize ];
    [ image addRepresentation: thumbSizeImageRep ];

    if (checkerBoard && image)
        {
        @try
            {
            [image lockFocus];
            NSRect imageRect;
            imageRect.origin = NSZeroPoint;
            imageRect.size = imageSize;
            
            //fill with checkerboard
            
            NSRectFillListWithColorsUsingOperation(&imageRect, &checkerBoard, 1, NSCompositingOperationCopy);

			if (fillColor) //if it's a fill we can do, do it
				{
				[fillColor set];
				[bez fill];
				}

           //stroke the bezier over that.
            //(just to show that we are using path data)

			[[NSColor blackColor] set];
            [bez stroke];

            [image unlockFocus];
            
            [self setThumbnailImage:image andRect:allPathBounds];
            }
        @catch(...)
            {
            [self logException: @"drawing to NSImage in drawJSONShapeToImage"];
           }
        }
    else
        [self setResponse: @"[Error setting thumbnail image]" error:YES];
}

/**************************************************************************************/

-(void) processJavaScriptMessage:(NSString *)string transaction:(transaction_id) transaction {

    @try
        {

        //don't bother reporting these, they get returned for javascripts that don't really return anything
        
        NSDictionary* jsonDict = nil;
        BOOL isError;
        NSString* responseOrError = [ self validateMessage:string transaction: transaction error: &isError jsonDict:&jsonDict ];

        //sMenuInfoTransaction/ is called alot to enable menus. don't report it, it gets too chatty.
        //we put the thumbnail bounds from sPreviewTransaction in the label by the image, so we don't need to report that either.
        //sEventStatusTransaction is reported in another section of the dialog (and will overwrite imageChanged events as it is written
        //after every action in Photoshop)

		NSDictionary* boundsDict = nil;
		NSDictionary* shapeDict = nil;

		if (jsonDict) {

			//check for transaction that may return preview data
			if (transaction == sPreviewTransaction || transaction == sUserJSTransaction)
				{
				if ([jsonDict objectForKey:@"layerChangedPath"] || [jsonDict objectForKey:@"path"])
					shapeDict = jsonDict;


				//determine if JSON dict is bounds from a thumbnail request
				//(has "bounds" entry, and an optional "missingFonts" entry)
				NSDictionary* maybeBoundsDict = [jsonDict objectForKey:@"bounds"];
				[jsonDict count];

				if ( [maybeBoundsDict isKindOfClass:[NSDictionary class]] &&
					([jsonDict count] == 1 || ([jsonDict count]== 2 && [jsonDict objectForKey:@"missingFonts"])) &&
					 [maybeBoundsDict objectForKey:@"top"] &&
					[maybeBoundsDict objectForKey:@"left"] &&
					[maybeBoundsDict objectForKey:@"bottom"] &&
					[maybeBoundsDict objectForKey:@"right"] )
					boundsDict = maybeBoundsDict;
				}
		}

		//Append in this case, so we can see the full response.
		if (responseOrError && transaction == sUserJSTransaction && [responseField string] )
			responseOrError = [[responseField string] stringByAppendingString:responseOrError];

        if (responseOrError
            &&
            ((transaction != sMenuInfoTransaction
            && boundsDict == nil //we put bounds info in another spot above the image
            && transaction != sBackgroundColorTransaction
            && transaction != sForegroundColorTransaction
            && transaction != sEventStatusTransaction) || isError)
            )
            [self setResponse: responseOrError error: isError]; // slap response into console results

        if (isError || responseOrError == nil)
            {
            //nothing interesting in the response or error, ignore.
            }

       else if (boundsDict) //JSON bounds/missing fonts for a thumb request
            {
			double top = [[boundsDict objectForKey:@"top"] doubleValue];
            double left = [[boundsDict objectForKey:@"left"] doubleValue];
            double bottom = [[boundsDict objectForKey:@"bottom"] doubleValue];
            double right = [[boundsDict objectForKey:@"right"] doubleValue];
       
			[self setThumbnailRect: NSMakeRect(left, top, right-left, bottom-top) ];

            NSArray* missingFonts = [jsonDict objectForKey:@"missingFonts"];
            if (missingFonts != nil)
                {
                //if there's missing fonts, just plop the whole JSON dict in the response area.
                [self setResponse: responseOrError error: isError];
                }
            }

        else if (transaction == sToolSubscriptionTransaction)
            {
            //[self processToolChangeNotification:string];
            }
        else if ( transaction == sImageChangedSubscription)
            {
            [self updatePreview]; //update the preview when doc/layer changes
            
            bool compsChanged = [jsonDict objectForKey:@"comps"];
            bool imageActivated = [jsonDict objectForKey:@"active"];
            bool imageClosed = [jsonDict objectForKey:@"closed"];

            if (imageClosed && !imageActivated)
                [self enableDocControls: NO];

            if (imageActivated)
                [self enableDocControls: YES];

            if (compsChanged || imageActivated || imageClosed)
                [self getMenuInfo]; //get more info for menus whenever comps/active doc changes (currently, the list of comps)
           }
        else if ( transaction == sMenuInfoTransaction)
            {
            //response from getCompInfo, use it to update the comp menu
            if (jsonDict)
                [self updateMenus:jsonDict]; //update the comp menu when doc/layer changes
            }
        else if ( shapeDict )
            {
            [self drawJSONShapeToImage: shapeDict];
            }
        else if (transaction == sForegroundColorSubscription
            || transaction == sForegroundColorTransaction)
            {
            [self processForegroundChange:string];
            }
        else if (transaction == sBackgroundColorSubscription
            || transaction == sBackgroundColorTransaction)
            {
            [self processBackgroundChange:string];
            }
        else if ( transaction == sEventStatusTransaction)
            {
            [self postEventStatus:jsonDict];
            }
        else if ( transaction == sMenuChangedSubscription)
            {
            //watch for the menu changed event and update our silly checkbox controls in response
            //(to verify these events are working properly)

            if (sAddedNodeMenus)
                {
                NSDictionary* menuDict = [jsonDict objectForKey:@"generatorMenuChanged"];
     
                NSMenuItem* menu = nil;
         
                if ([[menuDict objectForKey:@"name"] isEqualToString: kMenu1Name])
                    menu = generatorMenuItem1;
                
                if ([[menuDict objectForKey:@"name"] isEqualToString: kMenu2Name])
                    menu = generatorMenuItem2;
         
                if (menu)
                    {
                    //Photoshop doesn't specify these anymore.
                    //generally, generator controls the enabled/checked state of it's own menus

                    id enabledObj = [menuDict objectForKey:@"enabled"];
                    id checkedObj = [menuDict objectForKey:@"checked"];
      
                    if (enabledObj)
                        {
                        BOOL enabled = [enabledObj boolValue];
                        [menu setEnabled: enabled ];
                        [generatorMenusEnabledMenuItem setState: enabled ? NSOnState : NSOffState ];
                        }
                        
                    if (checkedObj)
                        [menu setState: [checkedObj boolValue] ? NSOnState : NSOffState ];
                    else
                        [self togglePSMenuState: menu]; //toggle the menu, and send PS a node menu message about it
                    }
                }
            }
        }
    @catch(...)
        {
        [self logException: @"handling message in processJavaScriptMessage"];
        }
}

/**************************************************************************************/

static NSColor *GetCheckerBoard()
{
    static NSColor* sCheckerBoardColor = nil;
    
    if (sCheckerBoardColor)
        return sCheckerBoardColor;
    
    //  size of our checkerboard pattern
    unsigned short phase_shift = 3;  //8x8
    const int32_t kCheckerboardWidth = 32;
    const int32_t kCheckerboardHeight = 32;
   
   //colors of checkerboard
    const unsigned char r1 = 191;
    const unsigned char g1 = 191;
    const unsigned char b1 = 191;
    
    const unsigned char r2 = 255;
    const unsigned char g2 = 255;
    const unsigned char b2 = 255;
   
    NSBitmapImageRep * checkerRep = [[NSBitmapImageRep alloc] initWithBitmapDataPlanes:nil
                                                    pixelsWide: kCheckerboardWidth
                                                    pixelsHigh: kCheckerboardHeight
                                                    bitsPerSample:8
                                                    samplesPerPixel:3
                                                    hasAlpha:NO
                                                    isPlanar:NO
                                                    colorSpaceName: NSDeviceRGBColorSpace
                                                    bitmapFormat:0
                                                    bytesPerRow:(3 * kCheckerboardWidth)
                                                    bitsPerPixel:24];

    if (checkerRep == nil)
        {
        LogError("GetCheckerBoard: NULL checkerRep");
        return nil;
        }

    // get the byte pointer for our bitmap
    unsigned char *pixels = [checkerRep bitmapData];

    if (pixels == nil)
        {
        LogError("GetCheckerBoard: NULL pixels");
        return nil;
        }

   int x, y;
       
    for (y = 0; y < kCheckerboardHeight; ++y)
    {
        int32_t y_phase = (y >> phase_shift);
        for (x = 0; x < kCheckerboardWidth; ++x)
        {
            int32_t x_phase = (x >> phase_shift);
            int32_t phase = (y_phase ^ x_phase) & 0x01;
            if (phase)
            {
                pixels[0] = r1;
                pixels[1] = g1;
                pixels[2] = b1;
            }
            else
            {
                pixels[0] = r2;
                pixels[1] = g2;
                pixels[2] = b2;
            }
            pixels += 3;
        }
    }
    
    NSImage *image = [[[NSImage alloc] initWithSize: NSMakeSize( kCheckerboardWidth, kCheckerboardHeight ) ] autorelease];
    [ image addRepresentation: checkerRep ];

    sCheckerBoardColor = [[NSColor colorWithPatternImage: image] retain];
    return sCheckerBoardColor;
}

/**************************************************************************************/

-(void) processImageMessage:(NSImage *)image transaction:(transaction_id) transaction {

    @try
        {
        //composite over a checkerboard
        
        //nil image means just checkerboard (nothing selected, or blank image)
     
        NSColor* checkerBoard = GetCheckerBoard();
     
        if (checkerBoard == nil)
            {
            [self setResponse: @"[Error getting checkerboard pattern for thumbnail]" error:YES];
            return;
            }
        
        if ( image == nil)
            image = [[[NSImage alloc] initWithSize:GetMaxThumbnailSize()] autorelease];

        NSRect imageRect;
        imageRect.origin = NSZeroPoint;
        imageRect.size = [image size];

        @try
            {
            [image lockFocus];
            NSRectFillListWithColorsUsingOperation(&imageRect, &checkerBoard, 1, NSCompositingOperationDestinationOver);
            [image unlockFocus];
            }
        @catch(...)
            {
            [self logException: @"drawing to NSImage in processImageMessage"];
            }
        
        PreviewType preview = [self getPreviewType];
            
        if (preview == eActiveDocPixMap || preview == eActiveDocJPEG)
            [self setThumbnailImage:image andRect:imageRect];
        else
            [imageWidget setImage: image]; //rect is coming in a JSON message
       }
    @catch(...)
        {
        [self logException: @"handling image message in processImageMessage"];
        }

 }

/**************************************************************************************/

-(void) processDataMessage:(NSData *)data transaction:(transaction_id) transaction {

    @try
        {
        if (transaction == sProfileTransaction)  {
            [[NSFileManager defaultManager] createFileAtPath:[sProfileURL path] contents:nil attributes:nil];
            NSFileHandle* fileHandle = [NSFileHandle fileHandleForWritingToURL:sProfileURL error:nil];
            [fileHandle truncateFileAtOffset:0]; //erase
            [fileHandle writeData:data];
            [fileHandle closeFile];
            }
        else if (transaction == sPreviewTransaction)   {
            
            //could use this to actually color manage the preview, but we'll just save it as a file for now.
            NSString* previewICCPath = [[EchoClient get] makeLogFilePath: @"preview.icc"];
            [[NSFileManager defaultManager] createFileAtPath:previewICCPath contents:nil attributes:nil];
            NSFileHandle* fileHandle = [NSFileHandle fileHandleForWritingAtPath:previewICCPath];
            [fileHandle truncateFileAtOffset:0]; //erase
            [fileHandle writeData:data];
            [fileHandle closeFile];
            }
        }
    @catch(...)
        {
        [self logException: @"handling image message in processDataMessage"];
        }
    
    [sProfileURL release];
    sProfileURL = 0;
    sProfileTransaction = 0;

    }

/**************************************************************************************/

-(void) processFileMessage:(NSData *)data fileInfo:(NSString*)fileInfo transaction:(transaction_id) transaction {

}

/**************************************************************************************/

-(BOOL) shouldSavePassword {

	return [savePasswordInKeychainCheckbox state] == NSOnState;
}

/**************************************************************************************/

static NSTimer* sEventStatusTimer = nil;

/**************************************************************************************/

-(void) connectionOpened {

	NSColor *theColor = [ NSColor colorWithDeviceRed: 0.0 green:1.0 blue:0.0 alpha: 1.0 ];
	[ connectionStatusWidget setColor: theColor ];

	if ([[EchoClient get] password])
		[passwordField setStringValue:[[EchoClient get] password]];

    // now that encryption is setup, we can sign up for all the events

	[ self setupGeneratorSubscriptions ];

	// get initial foreground, background
	[ self getGroundColors ];

    [ self getMenuInfo ]; //get any comps for any active doc
    [ self updatePreview ];

    //get status every .3 seconds if event listening is on
    sEventStatusTimer = [NSTimer scheduledTimerWithTimeInterval:.3
                                             target:self
                                           selector:@selector(getCurrentEventStatus)
                                           userInfo:nil
                                            repeats:YES];
    [sEventStatusTimer retain];

}

/**************************************************************************************/

-(void) connectionClosed {
	
	NSColor *theColor = [ NSColor colorWithDeviceRed: 1.0 green:0.0 blue:0.0 alpha: 1.0 ];
	[ connectionStatusWidget setColor: theColor ];
    [self clearThumbnailImage];
 
    [sEventStatusTimer invalidate];
    [self enableDocControls: NO ];

}

/**************************************************************************************/

@end
