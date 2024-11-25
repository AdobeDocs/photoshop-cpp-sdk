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
//  PhotoshopLib.m
//  firetutorial
//
//  @author Allen Jeng, Photoshop QE Developer, Adobe Systems Inc.
//  Copyright 2011 Adobe Systems Inc. All rights reserved.
//

#import "PhotoshopLib.h"


@implementation PhotoshopLib

/*
 * the paint brush size JS according to desired brush size
 */
+(NSString *) paintBrushSize: (float)brushSize
{
	NSString *nsStr1 = [NSString stringWithUTF8String: "	var idsetd = charIDToTypeID( \"setd\" );\r"
						"var desc2 = new ActionDescriptor();\r"
						"var idnull = charIDToTypeID( \"null\" );\r"
						"var ref2 = new ActionReference();\r"
						"var idBrsh = charIDToTypeID( \"Brsh\" );\r"
						"var idOrdn = charIDToTypeID( \"Ordn\" );\r"
						"var idTrgt = charIDToTypeID( \"Trgt\" );\r"
						"ref2.putEnumerated( idBrsh, idOrdn, idTrgt );\r"
						"desc2.putReference( idnull, ref2 );\r"
						"var idT = charIDToTypeID( \"T   \" );\r"
						"var desc3 = new ActionDescriptor();\r"
						"var idmasterDiameter = stringIDToTypeID( \"masterDiameter\" );\r"
						"var idPxl = charIDToTypeID( \"#Pxl\" );\r"
						"desc3.putUnitDouble( idmasterDiameter, idPxl, "];
	
	NSString *nsStr2 = [NSString stringWithUTF8String:" );\r"
						"var idBrsh = charIDToTypeID( \"Brsh\" );\r"
						"desc2.putObject( idT, idBrsh, desc3 );\r"
						"executeAction( idsetd, desc2, DialogModes.NO );"];
	
	NSString *finalJS = [NSString stringWithFormat:@"%@%f%@", nsStr1, brushSize, nsStr2];
	return finalJS;
}

/*
 * JS for creating a color object.
 * remember to use the js variable called "theColor"
 */
+(NSString *) setColorJS:(int)red colorGreen:(int)green colorBlue:(int)blue
{
	NSString *mystr = [NSString stringWithUTF8String:"var theColor = new SolidColor();\r"
					   "theColor.rgb.red = "];
	
	NSString *finalStr = [NSString stringWithFormat:@"%@%@", mystr, [NSString stringWithFormat:@"%d",red]];
	
	mystr = [NSString stringWithUTF8String:";\r theColor.rgb.green = "];
	finalStr = [NSString stringWithFormat:@"%@%@%@", finalStr, mystr, [NSString stringWithFormat:@"%d",green]];
	
	mystr = [NSString stringWithUTF8String:";\r theColor.rgb.blue = "];
	finalStr = [NSString stringWithFormat:@"%@%@%@", finalStr, mystr, [NSString stringWithFormat:@"%d ;\r",blue]];
	
	return finalStr;
}

/*
 * JS for changing foreground color in PS
 */
+(NSString *) setForegroundColorJS:(int)red colorGreen:(int)green colorBlue:(int)blue
{
	NSString *mystr = [self setColorJS:red colorGreen:green colorBlue:blue];
	NSString *jsstr = [NSString stringWithUTF8String:"app.foregroundColor = theColor;"];
	NSString *finalStr = [NSString stringWithFormat:@"%@%@", mystr, jsstr];
	
	return finalStr;
}

/*
 * JS for filling currently selected layer with RGB color
 */
+(NSString *) fillLayerWithColor:(int)red colorGreen:(int)green colorBlue:(int)blue
{
	NSString *mystr = [self setColorJS:red colorGreen:green colorBlue:blue];
	NSString *jsstr = [NSString stringWithUTF8String:"app.activeDocument.selection.fill (theColor);"];
	NSString *finalStr = [NSString stringWithFormat:@"%@%@", mystr, jsstr];
	
	return finalStr;
}


/*
 *  Show / hide a layer
 */
+(NSString *) setActiveLayerVisibleJS:(BOOL)wantVisible layerNumber:(int)layerNo
{
	NSString *mystr = [NSString stringWithUTF8String:"app.activeDocument.artLayers["];
	NSString *finalStr = [NSString stringWithFormat:@"%@%d", mystr, layerNo];
	
	if (wantVisible)
		mystr = [NSString stringWithUTF8String:"].visible = true;"];
	else
		mystr = [NSString stringWithUTF8String:"].visible = false;"];
	finalStr = [NSString stringWithFormat:@"%@%@", finalStr, mystr];
	
	return finalStr;
}


/*
 * JS for setting selection brush size
 */
+(NSString *) setSelectionBrushSizeJS:(float) brushSize
{
	NSString *nsStr1 = [NSString stringWithUTF8String: "	var idsetd = charIDToTypeID( \"setd\" );\r"
						"var desc5 = new ActionDescriptor();\r"
						"var idnull = charIDToTypeID( \"null\" );\r"
						"var ref4 = new ActionReference();\r"
						"var idBrsh = charIDToTypeID( \"Brsh\" );\r"
						"var idOrdn = charIDToTypeID( \"Ordn\" );\r"
						"var idTrgt = charIDToTypeID( \"Trgt\" );\r"
						"ref4.putEnumerated( idBrsh, idOrdn, idTrgt );\r"
						"desc5.putReference( idnull, ref4 );\r"
						"var idT = charIDToTypeID( \"T   \" );\r"
						"var desc6 = new ActionDescriptor();\r"
						"var idDmtr = charIDToTypeID( \"Dmtr\" );\r"
						"var idPxl = charIDToTypeID( \"#Pxl\" );\r"
						"desc6.putUnitDouble( idDmtr, idPxl,"];
	
	NSString *nsStr2 = [NSString stringWithUTF8String:");\r"
						"var idHrdn = charIDToTypeID( \"Hrdn\" );\r"
						"var idPrc = charIDToTypeID( \"#Prc\" );\r"
						"desc6.putUnitDouble( idHrdn, idPrc, 100.0 );\r"
						"var idAngl = charIDToTypeID( \"Angl\" );\r"
						"var idAng = charIDToTypeID( \"#Ang\" );\r"
						"desc6.putUnitDouble( idAngl, idAng, 0 );\r"
						"var idRndn = charIDToTypeID( \"Rndn\" );\r"
						"var idPrc = charIDToTypeID( \"#Prc\" );\r"
						"desc6.putUnitDouble( idRndn, idPrc, 100.0 );\r"
						"var idSpcn = charIDToTypeID( \"Spcn\" );\r"
						"var idPrc = charIDToTypeID( \"#Prc\" );\r"
						"desc6.putUnitDouble( idSpcn, idPrc, 25.0 );\r"
						"var idIntr = charIDToTypeID( \"Intr\" );\r"
						"desc6.putBoolean( idIntr, true );\r"
						"var idflipX = stringIDToTypeID( \"flipX\" );\r"
						"desc6.putBoolean( idflipX, false );\r"
						"var idflipY = stringIDToTypeID( \"flipY\" );\r"
						"desc6.putBoolean( idflipY, false );\r"
						"var idcomputedBrush = stringIDToTypeID( \"computedBrush\" );\r"
						"desc5.putObject( idT, idcomputedBrush, desc6 );\r"
						"executeAction( idsetd, desc5, DialogModes.NO );"];
	
	NSString *finalJS = [NSString stringWithFormat:@"%@%f%@", nsStr1, brushSize, nsStr2];
	return finalJS;
}

/*
 * Action code run
 */
+(NSString *) playStrokeJS:(NSString *) path
{
	NSString *newStr;
	NSString *finalStr;
	
	newStr = [NSString stringWithUTF8String:"var descriptor = new ActionDescriptor();\r"
			  "descriptor.putPath(stringIDToTypeID(\"path\"), new File(\""];
	
	finalStr = [NSString stringWithFormat:@"%@%@", newStr, path];
	
	newStr = [NSString stringWithUTF8String:"\") );\r"
			  "descriptor.putBoolean(stringIDToTypeID(\"tool\"), true);\r"
			  "var commandString = stringIDToTypeID(\"playbackStrokeFromFile\");\r"
			  "executeAction(commandString, descriptor, DialogModes.NO);"];
	finalStr = [NSString stringWithFormat:@"%@%@", finalStr, newStr];
	
	return finalStr;
}

/*
 * choose the tool according to toolType
 */
+(NSString *) switchToolJS:(int) toolType
{
	NSString *toolJS = [NSString stringWithUTF8String:"var idslct = charIDToTypeID( \"slct\" );\r"
							 "var desc2 = new ActionDescriptor();\r"
							 "var idnull = charIDToTypeID( \"null\" );\r"
							 "var ref2 = new ActionReference();\r"];

	NSString *theTool;
	NSString *finalStr;
	
	if (toolType == kToolBrush)
	{
		theTool = [NSString stringWithUTF8String:"var brushTool = charIDToTypeID( \"PbTl\" );\r"];
	}
	else if (toolType == kToolQuickSelect)
	{
		theTool = [NSString stringWithUTF8String:"var brushTool = stringIDToTypeID( \"quickSelectTool\" );\r"];
	}
	else if (toolType == kToolSmudge)
	{
		theTool = [NSString stringWithUTF8String:"var brushTool = charIDToTypeID( \"SmTl\" );\r"];
	}

	finalStr = [NSString stringWithFormat:@"%@%@", toolJS, theTool ];
	
	toolJS = [NSString stringWithUTF8String:"ref2.putClass( brushTool );\r"
							 "desc2.putReference( idnull, ref2 );\r"
							 "executeAction( idslct, desc2, DialogModes.NO );"];
	finalStr = [NSString stringWithFormat:@"%@%@", finalStr, toolJS ];
	
	return finalStr;
}


/*
 *  create a document of width x height with unit as pixels
 */
+(NSString *) createNewDoc:(int)width docHeight:(int)height docName:(NSString *)name
{
	NSString *newStr;
	NSString *finalStr;
	
	newStr = [NSString stringWithUTF8String:"app.documents.add(UnitValue("];
	finalStr = [NSString stringWithFormat:@"%@%d", newStr, width];
	
	newStr = [NSString stringWithUTF8String: ", \"px\"), UnitValue("];
	finalStr = [NSString stringWithFormat:@"%@%@%d", finalStr, newStr, height];
	
	newStr = [NSString stringWithUTF8String:", \"px\"), 72, "];
	finalStr = [NSString stringWithFormat:@"%@%@ \"%@\");", finalStr, newStr, name];
	
	return finalStr;
}

/*
 * JS for filling currently selected layer with RGB color
 */
+(NSString *) createTextJS:(NSString *)theText fontName:(NSString *)font fontSize:(int)fSize 
				 fontColorRed:(int)red fontColorGreen:(int)green fontColorBlue:(int)blue 
				 positionX:(float)x positionY:(float)y
{
	NSString *mystr = [NSString stringWithUTF8String:"var newtext = app.activeDocument.artLayers.add();\r"
					   "newtext.kind = LayerKind.TEXT;\r"
					   "newtext.textItem.contents = \""];
	
	NSString *finalStr = [NSString stringWithFormat:@"%@%@", mystr, theText];
	
	mystr = [NSString stringWithUTF8String:"\";\r newtext.textItem.font = \""];
	finalStr = [NSString stringWithFormat:@"%@%@%@", finalStr, mystr, font];
	
	mystr = [NSString stringWithUTF8String:"\";\r newtext.textItem.size = "];
	finalStr = [NSString stringWithFormat:@"%@%@ %d ;\r", finalStr, mystr, fSize];
	
	mystr = [self setColorJS:red colorGreen:green colorBlue:blue];
	finalStr = [NSString stringWithFormat:@"%@%@", finalStr, mystr];
	
	mystr = [NSString stringWithUTF8String:"newtext.textItem.color = theColor;\r"
			 "newtext.textItem.position = Array( UnitValue("];
	finalStr = [NSString stringWithFormat:@"%@%@%f", finalStr, mystr, x];
	
	mystr = [NSString stringWithUTF8String:", \"in\"), UnitValue("];
	finalStr = [NSString stringWithFormat:@"%@%@%f", finalStr, mystr, y];
	
	mystr = [NSString stringWithUTF8String:", \"in\"));"];
	finalStr = [NSString stringWithFormat:@"%@%@", finalStr, mystr];

	
	return finalStr;
}

/*
 *  rasterlizes the active layer, assuming that the active layer is a text layer
 */
+(NSString *) rasterlizeActiveLayerJS
{
	NSString *mystr = [NSString stringWithUTF8String:"app.activeDocument.activeLayer.rasterize(RasterizeType.TEXTCONTENTS);"];
	
	return mystr;
}

/*
 *  duplicates the active layer
 */
+(NSString *) duplicateActiveLayerJS
{
	NSString *mystr = [NSString stringWithUTF8String:"app.activeDocument.activeLayer.duplicate();"];
	
	return mystr;
}

/*
 *  duplicate active layer and set the dup active
 */
+(NSString *) duplicateActiveLayerJS:(NSString *)layerName
{
	NSString *mystr = [NSString stringWithUTF8String:"app.activeDocument.activeLayer = app.activeDocument.activeLayer.duplicate();\r"
											"app.activeDocument.activeLayer.name = \""];
	NSString *finalStr = [NSString stringWithFormat:@"%@%@\";", mystr, layerName];
	
	return finalStr;
}


/*
 *  rotate the layer by degree and anchor position
 */
+(NSString *) rotateActiveLayerJS:(int)degree anchorPosition:(int)anchor
{
	NSString *mystr = [NSString stringWithUTF8String:"app.activeDocument.activeLayer.rotate ("];
	NSString *finalStr = [NSString stringWithFormat:@"%@%d", mystr, degree];
	
	if ( anchor == kAnchorPositionTopLeft )
	{
		mystr = [NSString stringWithUTF8String:", AnchorPosition.TOPLEFT);"];
	}
	else if ( anchor == kAnchorPositionBottomLeft )
	{
		mystr = [NSString stringWithUTF8String:", AnchorPosition.BOTTOMLEFT);"];
	}
	else
	{
		mystr = [NSString stringWithUTF8String:");"];
	}

	finalStr = [NSString stringWithFormat:@"%@%@", finalStr, mystr];
	
	return finalStr;
}

/*
 *  apply gaussian blur to the active layer
 */
+(NSString *) gaussianBlurActiveLayerJS:(int)radius
{
	NSString *mystr = [NSString stringWithUTF8String:"app.activeDocument.activeLayer.applyGaussianBlur("];
	NSString *finalStr = [NSString stringWithFormat:@"%@ %d );", mystr, radius];
	
	return finalStr;
}

/*
 *  apply wind to the active layer
 */
+(NSString *) applyWindJS
{
	NSString *finalStr = [NSString stringWithUTF8String:"var idWnd = charIDToTypeID( \"Wnd \" );\r"
						  "var desc22 = new ActionDescriptor();\r"
						  "var idWndM = charIDToTypeID( \"WndM\" );\r"
						  "var idWndM = charIDToTypeID( \"WndM\" );\r"
						  "var idWnd = charIDToTypeID( \"Wnd \" );\r"
						  "desc22.putEnumerated( idWndM, idWndM, idWnd );\r"
						  "var idDrct = charIDToTypeID( \"Drct\" );\r"
						  "var idDrct = charIDToTypeID( \"Drct\" );\r"
						  "var idRght = charIDToTypeID( \"Rght\" );\r"
						  "desc22.putEnumerated( idDrct, idDrct, idRght );\r"
						  "executeAction( idWnd, desc22, DialogModes.NO );"];
	return finalStr;
}

/*
 *  link active layer with a dup of background layer, merge them and give the layer a new name
 */
+(NSString *) mergeWithCopyOfBGLayerJS:(NSString *)newName
{
	// note that if we're to make this more generic, the following should really be separated into 4 different functions:
	// the duping of layer, linking of layer, merging of linked layer, and renaming a layer
	NSString *mystr = [NSString stringWithUTF8String:"app.activeDocument.activeLayer.link (app.activeDocument.backgroundLayer.duplicate());\r"
					   "app.activeDocument.activeLayer.merge();\r"
					   "app.activeDocument.activeLayer.name = \""];
	NSString *finalStr = [NSString stringWithFormat:@"%@%@\";", mystr, newName];

	return finalStr;
}

/*
 *  apply hue saturation
 */
+(NSString *) setHueSaturationJS:(int)hue saturationValue:(int)saturation lightnessValue:(int)lightness colorize:(BOOL)wantColorlize
{
	NSString *mystr = [NSString stringWithUTF8String:"var idHStr = charIDToTypeID( \"HStr\" );\r"
					   "var desc53 = new ActionDescriptor();\r"
					   "var idpresetKind = stringIDToTypeID( \"presetKind\" );\r"
					   "var idpresetKindType = stringIDToTypeID( \"presetKindType\" );\r"
					   "var idpresetKindCustom = stringIDToTypeID( \"presetKindCustom\" );\r"
					   "desc53.putEnumerated( idpresetKind, idpresetKindType, idpresetKindCustom );\r"
					   "var idClrz = charIDToTypeID( \"Clrz\" );\r"
					   "desc53.putBoolean( idClrz,"];
	NSString *finalStr;
	
	if (wantColorlize)
		finalStr = [NSString stringWithFormat:@"%@ true ",mystr];
	else
		finalStr = [NSString stringWithFormat:@"%@ false ",mystr];

	mystr = [NSString stringWithUTF8String:"); \r var idAdjs = charIDToTypeID( \"Adjs\" );\r"
			 "var list2 = new ActionList();\r"
			 "var desc54 = new ActionDescriptor();\r"
			 "var idH = charIDToTypeID( \"H   \" );\r"
			 "desc54.putInteger( idH,"];	
	finalStr = [NSString stringWithFormat:@"%@%@%d", finalStr, mystr, hue];

	mystr = [NSString stringWithUTF8String:");\r var idStrt = charIDToTypeID( \"Strt\" );\r desc54.putInteger( idStrt,"];
	finalStr = [NSString stringWithFormat:@"%@%@%d", finalStr, mystr, saturation];
	
	mystr = [NSString stringWithUTF8String:");\r var idLght = charIDToTypeID( \"Lght\" );\r desc54.putInteger( idLght,"];
	finalStr = [NSString stringWithFormat:@"%@%@%d", finalStr, mystr, lightness];
	
	mystr = [NSString stringWithUTF8String:");\r var idHsttwo = charIDToTypeID( \"Hst2\" ); \r"
			 "list2.putObject( idHsttwo, desc54 );\r"
			 "desc53.putList( idAdjs, list2 );\r"
			 "executeAction( idHStr, desc53, DialogModes.NO );"];
	finalStr = [NSString stringWithFormat:@"%@%@", finalStr, mystr];
	
	return finalStr;
}

/*
 *  change the active layer's blend mode to overlay
 */
+(NSString *) changeActiveLayerBlendModeOverlay
{
	NSString *mystr = [NSString stringWithUTF8String:"app.activeDocument.activeLayer.blendMode = BlendMode.OVERLAY;"];
	return mystr;
}

/*
 *  switch the active layer to layerNumber
 */
+(NSString *) switchActiveLayer:(int)layerNumber
{
	NSString *mystr = [NSString stringWithUTF8String:"app.activeDocument.activeLayer = app.activeDocument.artLayers["];
	NSString *finalStr = [NSString stringWithFormat:@"%@%d ];", mystr, layerNumber];
	
	return finalStr;
}

/*
 *  lock transparent pixel for active layer
 */
+(NSString *) lockActiveLayerTransparentPixel:(BOOL)lockIt
{
	NSString *mystr = [NSString stringWithUTF8String:"app.activeDocument.activeLayer.transparentPixelsLocked = "];	
	NSString *finalStr;
	
	if (lockIt)
		finalStr = [NSString stringWithFormat:@"%@ true;", mystr];
	else
		finalStr = [NSString stringWithFormat:@"%@ false;", mystr];
	
	return finalStr;	
}

/*
 *  apply the orange / yellow / orange gradient
 */
+(NSString *) applyOrangeYellowGradient
{
	NSString *finalStr = [NSString stringWithUTF8String:"var idGrdn = charIDToTypeID( \"Grdn\" );\r"
						  "var desc97 = new ActionDescriptor();\r"
						  "var idFrom = charIDToTypeID( \"From\" );\r"
						  "var desc98 = new ActionDescriptor();\r"
						  "var idHrzn = charIDToTypeID( \"Hrzn\" );\r"
						  "var idRlt = charIDToTypeID( \"#Rlt\" );\r"
						  "desc98.putUnitDouble( idHrzn, idRlt, 223.000000 );\r"
						  "var idVrtc = charIDToTypeID( \"Vrtc\" );\r"
						  "var idRlt = charIDToTypeID( \"#Rlt\" );\r"
						  "desc98.putUnitDouble( idVrtc, idRlt, 25.000000 );\r"
						  "var idPnt = charIDToTypeID( \"Pnt \" );\r"
						  "desc97.putObject( idFrom, idPnt, desc98 );\r"
						  "var idT = charIDToTypeID( \"T   \" );\r"
						  "var desc99 = new ActionDescriptor();\r"
						  "var idHrzn = charIDToTypeID( \"Hrzn\" );\r"
						  "var idRlt = charIDToTypeID( \"#Rlt\" );\r"
						  "desc99.putUnitDouble( idHrzn, idRlt, 229.000000 );\r"
						  "var idVrtc = charIDToTypeID( \"Vrtc\" );\r"
						  "var idRlt = charIDToTypeID( \"#Rlt\" );\r"
						  "desc99.putUnitDouble( idVrtc, idRlt, 468.000000 );\r"
						  "var idPnt = charIDToTypeID( \"Pnt \" );\r"
						  "desc97.putObject( idT, idPnt, desc99 );\r"
						  "var idType = charIDToTypeID( \"Type\" );\r"
						  "var idGrdT = charIDToTypeID( \"GrdT\" );\r"
						  "var idLnr = charIDToTypeID( \"Lnr \" );\r"
						  "desc97.putEnumerated( idType, idGrdT, idLnr );\r"
						  "var idDthr = charIDToTypeID( \"Dthr\" );\r"
						  "desc97.putBoolean( idDthr, true );\r"
						  "var idUsMs = charIDToTypeID( \"UsMs\" );\r"
						  "desc97.putBoolean( idUsMs, true );\r"
						  "var idGrad = charIDToTypeID( \"Grad\" );\r"
						  "var desc100 = new ActionDescriptor();\r"
						  "var idNm = charIDToTypeID( \"Nm  \" );\r"
						  "desc100.putString( idNm, \"$$$/DefaultGradient/OrangeYellowOrange=Orange, Yellow, Orange\" );\r"
						  "var idGrdF = charIDToTypeID( \"GrdF\" );\r"
						  "var idGrdF = charIDToTypeID( \"GrdF\" );\r"
						  "var idCstS = charIDToTypeID( \"CstS\" );\r"
						  "desc100.putEnumerated( idGrdF, idGrdF, idCstS );\r"
						  "var idIntr = charIDToTypeID( \"Intr\" );\r"
						  "desc100.putDouble( idIntr, 4096.000000 );\r"
						  "var idClrs = charIDToTypeID( \"Clrs\" );\r"
						  "var list11 = new ActionList();\r"
						  "var desc101 = new ActionDescriptor();\r"
						  "var idClr = charIDToTypeID( \"Clr \" );\r"
						  "var desc102 = new ActionDescriptor();\r"
						  "var idH = charIDToTypeID( \"H   \" );\r"
						  "var idAng = charIDToTypeID( \"#Ang\" );\r"
						  "desc102.putUnitDouble( idH, idAng, 25.675049 );\r"
						  "var idStrt = charIDToTypeID( \"Strt\" );\r"
						  "desc102.putDouble( idStrt, 99.215686 );\r"
						  "var idBrgh = charIDToTypeID( \"Brgh\" );\r"
						  "desc102.putDouble( idBrgh, 100.000000 );\r"
						  "var idHSBC = charIDToTypeID( \"HSBC\" );\r"
						  "desc101.putObject( idClr, idHSBC, desc102 );\r"
						  "var idType = charIDToTypeID( \"Type\" );\r"
						  "var idClry = charIDToTypeID( \"Clry\" );\r"
						  "var idUsrS = charIDToTypeID( \"UsrS\" );\r"
						  "desc101.putEnumerated( idType, idClry, idUsrS );\r"
						  "var idLctn = charIDToTypeID( \"Lctn\" );\r"
						  "desc101.putInteger( idLctn, 0 );\r"
						  "var idMdpn = charIDToTypeID( \"Mdpn\" );\r"
						  "desc101.putInteger( idMdpn, 50 );\r"
						  "var idClrt = charIDToTypeID( \"Clrt\" );\r"
						  "list11.putObject( idClrt, desc101 );\r"
						  "var desc103 = new ActionDescriptor();\r"
						  "var idClr = charIDToTypeID( \"Clr \" );\r"
						  "var desc104 = new ActionDescriptor();\r"
						  "var idH = charIDToTypeID( \"H   \" );\r"
						  "var idAng = charIDToTypeID( \"#Ang\" );\r"
						  "desc104.putUnitDouble( idH, idAng, 59.996338 );\r"
						  "var idStrt = charIDToTypeID( \"Strt\" );\r"
						  "desc104.putDouble( idStrt, 100.000000 );\r"
						  "var idBrgh = charIDToTypeID( \"Brgh\" );\r"
						  "desc104.putDouble( idBrgh, 100.000000 );\r"
						  "var idHSBC = charIDToTypeID( \"HSBC\" );\r"
						  "desc103.putObject( idClr, idHSBC, desc104 );\r"
						  "var idType = charIDToTypeID( \"Type\" );\r"
						  "var idClry = charIDToTypeID( \"Clry\" );\r"
						  "var idUsrS = charIDToTypeID( \"UsrS\" );\r"
						  "desc103.putEnumerated( idType, idClry, idUsrS );\r"
						  "var idLctn = charIDToTypeID( \"Lctn\" );\r"
						  "desc103.putInteger( idLctn, 2048 );\r"
						  "var idMdpn = charIDToTypeID( \"Mdpn\" );\r"
						  "desc103.putInteger( idMdpn, 50 );\r"
						  "var idClrt = charIDToTypeID( \"Clrt\" );\r"
						  "list11.putObject( idClrt, desc103 );\r"
						  "var desc105 = new ActionDescriptor();\r"
						  "var idClr = charIDToTypeID( \"Clr \" );\r"
						  "var desc106 = new ActionDescriptor();\r"
						  "var idH = charIDToTypeID( \"H   \" );\r"
						  "var idAng = charIDToTypeID( \"#Ang\" );\r"
						  "desc106.putUnitDouble( idH, idAng, 25.675049 );\r"
						  "var idStrt = charIDToTypeID( \"Strt\" );\r"
						  "desc106.putDouble( idStrt, 100.000000 );\r"
						  "var idBrgh = charIDToTypeID( \"Brgh\" );\r"
						  "desc106.putDouble( idBrgh, 100.000000 );\r"
						  "var idHSBC = charIDToTypeID( \"HSBC\" );\r"
						  "desc105.putObject( idClr, idHSBC, desc106 );\r"
						  "var idType = charIDToTypeID( \"Type\" );\r"
						  "var idClry = charIDToTypeID( \"Clry\" );\r"
						  "var idUsrS = charIDToTypeID( \"UsrS\" );\r"
						  "desc105.putEnumerated( idType, idClry, idUsrS );\r"
						  "var idLctn = charIDToTypeID( \"Lctn\" );\r"
						  "desc105.putInteger( idLctn, 4096 );\r"
						  "var idMdpn = charIDToTypeID( \"Mdpn\" );\r"
						  "desc105.putInteger( idMdpn, 47 );\r"
						  "var idClrt = charIDToTypeID( \"Clrt\" );\r"
						  "list11.putObject( idClrt, desc105 );\r"
						  "desc100.putList( idClrs, list11 );\r"
						  "var idTrns = charIDToTypeID( \"Trns\" );\r"
						  "var list12 = new ActionList();\r"
						  "var desc107 = new ActionDescriptor();\r"
						  "var idOpct = charIDToTypeID( \"Opct\" );\r"
						  "var idPrc = charIDToTypeID( \"#Prc\" );\r"
						  "desc107.putUnitDouble( idOpct, idPrc, 100.000000 );\r"
						  "var idLctn = charIDToTypeID( \"Lctn\" );\r"
						  "desc107.putInteger( idLctn, 0 );\r"
						  "var idMdpn = charIDToTypeID( \"Mdpn\" );\r"
						  "desc107.putInteger( idMdpn, 50 );\r"
						  "var idTrnS = charIDToTypeID( \"TrnS\" );\r"
						  "list12.putObject( idTrnS, desc107 );\r"
						  "var desc108 = new ActionDescriptor();\r"
						  "var idOpct = charIDToTypeID( \"Opct\" );\r"
						  "var idPrc = charIDToTypeID( \"#Prc\" );\r"
						  "desc108.putUnitDouble( idOpct, idPrc, 100.000000 );\r"
						  "var idLctn = charIDToTypeID( \"Lctn\" );\r"
						  "desc108.putInteger( idLctn, 4096 );\r"
						  "var idMdpn = charIDToTypeID( \"Mdpn\" );\r"
						  "desc108.putInteger( idMdpn, 50 );\r"
						  "var idTrnS = charIDToTypeID( \"TrnS\" );\r"
						  "list12.putObject( idTrnS, desc108 );\r"
						  "desc100.putList( idTrns, list12 );\r"
						  "var idGrdn = charIDToTypeID( \"Grdn\" );\r"
						  "desc97.putObject( idGrad, idGrdn, desc100 );\r"
						  "executeAction( idGrdn, desc97, DialogModes.NO );"];
	return finalStr;
}

/*
 *  apply an orange / red drop shadow and inner shadow to active layer
 */
+(NSString *) setSpecialShadow
{
	NSString *finalStr = [NSString stringWithUTF8String:	"var idsetd = charIDToTypeID( \"setd\" );\r"
							"var desc109 = new ActionDescriptor();\r"
							"var idnull = charIDToTypeID( \"null\" );\r"
							"var ref44 = new ActionReference();\r"
							"var idPrpr = charIDToTypeID( \"Prpr\" );\r"
							"var idLefx = charIDToTypeID( \"Lefx\" );\r"
							"ref44.putProperty( idPrpr, idLefx );\r"
							"var idLyr = charIDToTypeID( \"Lyr \" );\r"
							"var idOrdn = charIDToTypeID( \"Ordn\" );\r"
							"var idTrgt = charIDToTypeID( \"Trgt\" );\r"
							"ref44.putEnumerated( idLyr, idOrdn, idTrgt );\r"
							"desc109.putReference( idnull, ref44 );\r"
							"var idT = charIDToTypeID( \"T   \" );\r"
							"var desc110 = new ActionDescriptor();\r"
							"var idScl = charIDToTypeID( \"Scl \" );\r"
							"var idPrc = charIDToTypeID( \"#Prc\" );\r"
							"desc110.putUnitDouble( idScl, idPrc, 100.000000 );\r"
							"var idDrSh = charIDToTypeID( \"DrSh\" );\r"
							"var desc111 = new ActionDescriptor();\r"
							"var idenab = charIDToTypeID( \"enab\" );\r"
							"desc111.putBoolean( idenab, true );\r"
							"var idMd = charIDToTypeID( \"Md  \" );\r"
							"var idBlnM = charIDToTypeID( \"BlnM\" );\r"
							"var idMltp = charIDToTypeID( \"Mltp\" );\r"
							"desc111.putEnumerated( idMd, idBlnM, idMltp );\r"
							"var idClr = charIDToTypeID( \"Clr \" );\r"
							"var desc112 = new ActionDescriptor();\r"
							"var idRd = charIDToTypeID( \"Rd  \" );\r"
							"desc112.putDouble( idRd, 237.996368 );\r"
							"var idGrn = charIDToTypeID( \"Grn \" );\r"
							"desc112.putDouble( idGrn, 105.998383 );\r"
							"var idBl = charIDToTypeID( \"Bl  \" );\r"
							"desc112.putDouble( idBl, 0.000000 );\r"
							"var idRGBC = charIDToTypeID( \"RGBC\" );\r"
							"desc111.putObject( idClr, idRGBC, desc112 );\r"
							"var idOpct = charIDToTypeID( \"Opct\" );\r"
							"var idPrc = charIDToTypeID( \"#Prc\" );\r"
							"desc111.putUnitDouble( idOpct, idPrc, 59.000000 );\r"
							"var iduglg = charIDToTypeID( \"uglg\" );\r"
							"desc111.putBoolean( iduglg, true );\r"
							"var idlagl = charIDToTypeID( \"lagl\" );\r"
							"var idAng = charIDToTypeID( \"#Ang\" );\r"
							"desc111.putUnitDouble( idlagl, idAng, 120.000000 );\r"
							"var idDstn = charIDToTypeID( \"Dstn\" );\r"
							"var idPxl = charIDToTypeID( \"#Pxl\" );\r"
							"desc111.putUnitDouble( idDstn, idPxl, 3.000000 );\r"
							"var idCkmt = charIDToTypeID( \"Ckmt\" );\r"
							"var idPxl = charIDToTypeID( \"#Pxl\" );\r"
							"desc111.putUnitDouble( idCkmt, idPxl, 3.000000 );\r"
							"var idblur = charIDToTypeID( \"blur\" );\r"
							"var idPxl = charIDToTypeID( \"#Pxl\" );\r"
							"desc111.putUnitDouble( idblur, idPxl, 10.000000 );\r"
							"var idNose = charIDToTypeID( \"Nose\" );\r"
							"var idPrc = charIDToTypeID( \"#Prc\" );\r"
							"desc111.putUnitDouble( idNose, idPrc, 0.000000 );\r"
							"var idAntA = charIDToTypeID( \"AntA\" );\r"
							"desc111.putBoolean( idAntA, false );\r"
							"var idTrnS = charIDToTypeID( \"TrnS\" );\r"
							"var desc113 = new ActionDescriptor();\r"
							"var idNm = charIDToTypeID( \"Nm  \" );\r"
							"desc113.putString( idNm, \"Linear\" );\r"
							"var idShpC = charIDToTypeID( \"ShpC\" );\r"
							"desc111.putObject( idTrnS, idShpC, desc113 );\r"
							"var idlayerConceals = stringIDToTypeID( \"layerConceals\" );\r"
							"desc111.putBoolean( idlayerConceals, true );\r"
							"var idDrSh = charIDToTypeID( \"DrSh\" );\r"
							"desc110.putObject( idDrSh, idDrSh, desc111 );\r"
							"var idIrSh = charIDToTypeID( \"IrSh\" );\r"
							"var desc114 = new ActionDescriptor();\r"
							"var idenab = charIDToTypeID( \"enab\" );\r"
							"desc114.putBoolean( idenab, true );\r"
							"var idMd = charIDToTypeID( \"Md  \" );\r"
							"var idBlnM = charIDToTypeID( \"BlnM\" );\r"
							"var idMltp = charIDToTypeID( \"Mltp\" );\r"
							"desc114.putEnumerated( idMd, idBlnM, idMltp );\r"
							"var idClr = charIDToTypeID( \"Clr \" );\r"
							"var desc115 = new ActionDescriptor();\r"
							"var idRd = charIDToTypeID( \"Rd  \" );\r"
							"desc115.putDouble( idRd, 235.996399 );\r"
							"var idGrn = charIDToTypeID( \"Grn \" );\r"
							"desc115.putDouble( idGrn, 5.999908 );\r"
							"var idBl = charIDToTypeID( \"Bl  \" );\r"
							"desc115.putDouble( idBl, 5.999908 );\r"
							"var idRGBC = charIDToTypeID( \"RGBC\" );\r"
							"desc114.putObject( idClr, idRGBC, desc115 );\r"
							"var idOpct = charIDToTypeID( \"Opct\" );\r"
							"var idPrc = charIDToTypeID( \"#Prc\" );\r"
							"desc114.putUnitDouble( idOpct, idPrc, 68.000000 );\r"
							"var iduglg = charIDToTypeID( \"uglg\" );\r"
							"desc114.putBoolean( iduglg, true );\r"
							"var idlagl = charIDToTypeID( \"lagl\" );\r"
							"var idAng = charIDToTypeID( \"#Ang\" );\r"
							"desc114.putUnitDouble( idlagl, idAng, 120.000000 );\r"
							"var idDstn = charIDToTypeID( \"Dstn\" );\r"
							"var idPxl = charIDToTypeID( \"#Pxl\" );\r"
							"desc114.putUnitDouble( idDstn, idPxl, 10.000000 );\r"
							"var idCkmt = charIDToTypeID( \"Ckmt\" );\r"
							"var idPxl = charIDToTypeID( \"#Pxl\" );\r"
							"desc114.putUnitDouble( idCkmt, idPxl, 0.000000 );\r"
							"var idblur = charIDToTypeID( \"blur\" );\r"
							"var idPxl = charIDToTypeID( \"#Pxl\" );\r"
							"desc114.putUnitDouble( idblur, idPxl, 49.000000 );\r"
							"var idNose = charIDToTypeID( \"Nose\" );\r"
							"var idPrc = charIDToTypeID( \"#Prc\" );\r"
							"desc114.putUnitDouble( idNose, idPrc, 0.000000 );\r"
							"var idAntA = charIDToTypeID( \"AntA\" );\r"
							"desc114.putBoolean( idAntA, false );\r"
							"var idTrnS = charIDToTypeID( \"TrnS\" );\r"
							"var desc116 = new ActionDescriptor();\r"
							"var idNm = charIDToTypeID( \"Nm  \" );\r"
							"desc116.putString( idNm, \"Linear\" );\r"
							"var idShpC = charIDToTypeID( \"ShpC\" );\r"
							"desc114.putObject( idTrnS, idShpC, desc116 );\r"
							"var idIrSh = charIDToTypeID( \"IrSh\" );\r"
							"desc110.putObject( idIrSh, idIrSh, desc114 );\r"
							"var idLefx = charIDToTypeID( \"Lefx\" );\r"
							"desc109.putObject( idT, idLefx, desc110 );\r"
							"executeAction( idsetd, desc109, DialogModes.NO );"];
	return finalStr;
}

@end
