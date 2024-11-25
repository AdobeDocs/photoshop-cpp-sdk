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
//  PhotoshopLib.h
//  firetutorial
//
//  @author Allen Jeng, Photoshop QE Developer, Adobe Systems Inc.
//  Copyright 2011 Adobe Systems Inc. All rights reserved.
//

#import <Foundation/Foundation.h>

static int kAnchorPositionBottomLeft = 1;
static int kAnchorPositionTopLeft = 2;

static int kToolBrush = 3;
static int kToolQuickSelect = 4;
static int kToolSmudge = 5;

@interface PhotoshopLib : NSObject {

}

/*
 * the paint brush size JS according to desired brush size
 */
+(NSString *) paintBrushSize: (float)brushSize;

/*
 * JS for creating a color object.
 * remember to use the js variable called "theColor"
 */
+(NSString *) setColorJS:(int)red colorGreen:(int)green colorBlue:(int)blue;


/*
 * JS for changing foreground color in PS
 */
+(NSString *) setForegroundColorJS:(int)red colorGreen:(int)green colorBlue:(int)blue;

/*
 * JS for filling currently selected layer with RGB color
 */
+(NSString *) fillLayerWithColor:(int)red colorGreen:(int)green colorBlue:(int)blue;

/*
 * JS for setting selection brush size
 */
+(NSString *) setSelectionBrushSizeJS:(float) brushSize;

/*
 * Play the painting code given the path of the paint binary
 */
+(NSString *) playStrokeJS:(NSString *) path;

/*
 * choose the tool according to toolType
 */
+(NSString *) switchToolJS:(int) toolType;

/*
 *  create a document of width x height
 */
+(NSString *) createNewDoc:(int)width docHeight:(int)height docName:(NSString *)name;


/*
 * JS for filling currently selected layer with RGB color
 */
+(NSString *) createTextJS:(NSString *)theText fontName:(NSString *)font fontSize:(int)fSize 
			  fontColorRed:(int)red fontColorGreen:(int)green fontColorBlue:(int)blue 
				 positionX:(float)x positionY:(float)y;

/*
 *  rasterlizes the active layer, assuming that the active layer is a text layer
 */
+(NSString *) rasterlizeActiveLayerJS;

/*
 *  duplicates the active layer
 */
+(NSString *) duplicateActiveLayerJS;

/*
 *  duplicate active layer and set the dup active
 */
+(NSString *) duplicateActiveLayerJS:(NSString *)layerName;

/*
 *  rotate the layer by degree and anchor position
 */
+(NSString *) rotateActiveLayerJS:(int)degree anchorPosition:(int)anchor;

/*
 *  Show / hide a layer
 */
+(NSString *) setActiveLayerVisibleJS:(BOOL)wantVisible layerNumber:(int)layerNo;

/*
 *  apply gaussian blur to the active layer
 */
+(NSString *) gaussianBlurActiveLayerJS:(int)radius;

/*
 *  apply wind to the active layer
 */
+(NSString *) applyWindJS;

/*
 *  link active layer with a dup of background layer, merge them and give the layer a new name
 */
+(NSString *) mergeWithCopyOfBGLayerJS:(NSString *)newName;

/*
 *  apply hue saturation
 */
+(NSString *) setHueSaturationJS:(int)hue saturationValue:(int)saturation lightnessValue:(int)lightness colorize:(BOOL)wantColorlize;

/*
 *  change the active layer's blend mode to overlay
 */
+(NSString *) changeActiveLayerBlendModeOverlay;

/*
 *  switch the active layer to layerNumber
 */
+(NSString *) switchActiveLayer:(int)layerNumber;


/*
 *  lock transparent pixel for active layer
 */
+(NSString *) lockActiveLayerTransparentPixel:(BOOL)lockIt;


/*
 *  apply the orange / yellow / orange gradient
 */
+(NSString *) applyOrangeYellowGradient;

/*
 *  apply an orange / red drop shadow and inner shadow to active layer
 */
+(NSString *) setSpecialShadow;

@end
