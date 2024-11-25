// ADOBE SYSTEMS INCORPORATED
// Copyright  1993 - 2005 Adobe Systems Incorporated
// All Rights Reserved
//
// NOTICE:  Adobe permits you to use, modify, and distribute this 
// file in accordance with the terms of the Adobe license agreement
// accompanying it.  If you have received this file from a source
// other than Adobe, then your use, modification, or distribution
// of it requires the prior written permission of Adobe.
//-------------------------------------------------------------------
/**
* \file PITerminology.h
*
* \brief This file defines constants for the keys used to access descriptor
* events, keys, classes, enum types, and enum values. 
* 
* \details
* Copyright 2000 Adobe Systems Incorporated.
* All Rights Reserved.
* 
* Distribution:
* PUBLIC
*/
//*****************************************************************************
//		WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING
//
//		WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING
//
// This file should NO LONGER BE USED to define OSTypes for keys, classes,
// enumeration types, and enumeration values. It exists only for backward
// compatibility. 
//
// Add names of new events, keys, classes, enum types and enum values to
// PIStringTerminology.h
//
// The aete resource, which referenced these keys, IS BEING
// DELETED. A Pate resource is being used instead (see UActionsTerminology.r
// for the format). This resource identifies items with strings instead
// of OSTypes. New APIs support construction and
// access of descriptors with strings instead of OSTypes. If you need to
// find the string equivalent of an OSType from this table, you can look
// in UActions.cpp. If you need to look it up the string corresponding to
// an ID at runtime inside the app, you can use:
// charPtr = GetStringIDMap ()->Find (DescType)
// From a plugin, use StringIDToTypeID in the ActionControlSuite or the
// BasicActionControlSuite
//
// The symbols in this file are still needed for forms, types, and units,
// and so that existing code which references them will still work. But 
// NEW CODE SHOULD USE THE STRING-BASED APIs and define the string constants
// in PIStringTerminology.h

#ifndef __PITerminology_h__ // Already defined?
#define __PITerminology_h__

#if PRAGMA_ONCE
#pragma once
#endif

#include "PIActions.h"

//*****************************************************************************
//
//
//	Use:
//		Use any of these predefined keys, types, and events for
//		mapping to predefined action terms.
//
//	Version history:
//		Version 1.0.0	1/21/1998	Ace		Initial compilation.
//			Compiled from AdobePITerminology.h, PIActions.h,
//			UActionsTerminology.h, and UBatchTerminology.h.
//		Version 2.0.0	10/20/1999	RW		Frozen -- no new OSType symbols
//											should be added.
//		Version 3.0.0	6/1/2019	JF		Change to enums for faster build
//											times on windows.
//
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
//	Classes.
//-------------------------------------------------------------------------------

enum PIClasses
{
	classAction = 'Actn',	// Action palette keyName, keyItemIndex, keyNumberOfSiblings, keyNumberOfChildren, keyParentName, keyParentIndex.
	classActionSet = 'ASet',	// Action palette keyName, keyItemIndex, keyNumberOfSiblings, keyNumberOfChildren, keyParentName, keyParentIndex.
	classAdjustment = 'Adjs',	// classLevels, classCurves, classBrightnessContrast, classColorBalance, classHueSaturation, classSelectiveColor, classThreshold, classPosterize, classInvert, classChannelMixer.
	classAdjustmentLayer = 'AdjL',	// See also: classLayer, classBackgroundLayer, classTextLayer, classObsoleteTextLayer.
	classAirbrushTool = 'AbTl',
	classAlphaChannelOptions = 'AChl',	// Breaks hash.
	classAntiAliasedPICTAcquire = 'AntA',
	classApplication = 'capp',	// cApplication
	classArrowhead = 'cArw',	// break rule because of keyArrowhead
	classAssert = 'Asrt',
	classAssumedProfile = 'AssP',
	classBMPFormat = 'BMPF',
	classBackgroundLayer = 'BckL',	// See also: classLayer, classAdjustmentLayer, classTextLayer.
	classBevelEmboss = 'ebbl',
	/////// DO NOT ADD ANYTHING HERE - SEE WARNING AT TOP OF FILE ///////////
	classBitmapMode = 'BtmM',	// classMode, keyMode.
	classBlendRange = 'Blnd',	// Breaks hash for bilinear.
	classBlurTool = 'BlTl',
	classBookColor = 'BkCl',
	classBrightnessContrast = 'BrgC',	// classAdjustment.
	classBrush = 'Brsh',
	classBurnInTool = 'BrTl',
	classCachePrefs = 'CchP',
	classCMYKColor = 'CMYC',	// keyCyan, keyMagenta, keyYellow, keyBlack.
	classCMYKColorMode = 'CMYM',	// classMode, keyMode.
	classCMYKSetup = 'CMYS',
	classCalculation = 'Clcl',
	classChannel = 'Chnl',
	classChannelMatrix = 'ChMx',
	classChannelMixer = 'ChnM',	// classAdjustment.
	classCineonFormat = 'SDPX',
	classClippingInfo = 'Clpo',
	classClippingPath = 'ClpP',	// There is also a keyClippingPath, enumClippingPath.
	classCloneStampTool = 'ClTl',
	/////// DO NOT ADD ANYTHING HERE - SEE WARNING AT TOP OF FILE ///////////
	classColor = 'Clr ',
	classColorBalance = 'ClrB',	// classAdjustment.
	classColorCorrection = 'ClrC',
	classColorPickerPrefs = 'Clrk',	// Contains: keyPickerKind, keyPickerID.  There is also a keyColorPickerPrefs.
	classColorSampler = 'ClSm',
	classColorStop = 'Clrt',
	classCommand = 'Cmnd',
	classCurves = 'Crvs',	// classAdjustment.
	classCurvePoint = 'CrPt',
	classCustomPalette = 'Cstl',	// Breaks hash for classCustomPhosphors
	classCurvesAdjustment = 'CrvA',
	classCustomPhosphors = 'CstP',
	classCustomWhitePoint = 'CstW',
	classDicomFormat = 'Dicm',
	classDisplayPrefs = 'DspP',
	classDocument = 'Dcmn',
	/////// DO NOT ADD ANYTHING HERE - SEE WARNING AT TOP OF FILE ///////////
	classDodgeTool = 'DdTl',
	classDropShadow = 'DrSh',
	classDuotoneInk = 'DtnI',
	classDuotoneMode = 'DtnM',	// classMode, keyMode.
	classEPSGenericFormat = 'EPSG',	// Used by Parser plug-ins.
	classEPSPICTPreview = 'EPSC',	// Breaks hash for typeEPSPreview.
	classEPSTIFFPreview = 'EPST',
	classElement = 'Elmn',
	classEllipse = 'Elps',
	classEraserTool = 'ErTl',
	classExport = 'Expr',	// There is also a keyExport.
	classFileInfo = 'FlIn',
	classFileSavePrefs = 'FlSv',
	classFlashPixFormat = 'FlsP',
	classFontDesignAxes = 'FntD',	// There is also a keyFontDesignAxes
	classFormat = 'Fmt ',	// There is also a keyFormat.
	classFrameFX = 'FrFX',
	classContour = 'FxSc',
	/////// DO NOT ADD ANYTHING HERE - SEE WARNING AT TOP OF FILE ///////////
	classGeneralPrefs = 'GnrP',
	classGIF89aExport = 'GF89',
	classGIFFormat = 'GFFr',
	classGlobalAngle = 'gblA',
	classGradient = 'Grdn',
	classGradientFill = 'Grdf',
	classGradientMap = 'GdMp',
	classGradientTool = 'GrTl',
	classGraySetup = 'GrSt',
	classGrayscale = 'Grsc',	// keyGray.
	classGrayscaleMode = 'Grys',	// classMode, keyMode.
	classGuide = 'Gd  ',
	classGuidesPrefs = 'GdPr',
	classHalftoneScreen = 'HlfS', // There is also enumHalftoneScreen, keyHalftoneScreen, eventHalftoneScreen.
	classHalftoneSpec = 'Hlfp', // Collides with classHalftoneScreen
	classHSBColor = 'HSBC',
	/////// DO NOT ADD ANYTHING HERE - SEE WARNING AT TOP OF FILE ///////////
	classHSBColorMode = 'HSBM',
	classHistoryBrushTool = 'HBTl',
	classHistoryPrefs = 'CHsP',	// Breaks hash.
	classHistoryState = 'HstS',
	classHueSatAdjustment = 'HStA',
	classHueSatAdjustmentV2 = 'Hst2',	// Breaks has for classHueSatAdjustment.
	classHueSaturation = 'HStr',	// classAdjustment.
	classIFFFormat = 'IFFF',
	classIllustratorPathsExport = 'IlsP',
	classImagePoint = 'ImgP',
	classImport = 'Impr',	// There is also a keyImport.
	classIndexedColorMode = 'IndC',	// classMode, keyMode.
	classInkTransfer = 'InkT',
	classInnerGlow = 'IrGl',
	classInnerShadow = 'IrSh',
	classInterfaceColor = 'IClr',
	classInvert = 'Invr',	// classAdjustment.
	classJPEGFormat = 'JPEG',	// There is also an enumJPEG.
	classLabColor = 'LbCl',
	/////// DO NOT ADD ANYTHING HERE - SEE WARNING AT TOP OF FILE ///////////
	classLabColorMode = 'LbCM',	// classMode, keyMode. Breaks hash for Lab Color.
	classLayer = 'Lyr ',	// See also: classAdjustmentLayer, classBackgroundLayer, classTextLayer, classObsoleteTextLayer.
	classLayerEffects = 'Lefx',
	classLayerFXVisible = 'lfxv',
	classLevels = 'Lvls',	// classAdjustment.
	classLevelsAdjustment = 'LvlA',
	classLightSource = 'LghS',	// There is also a keyLightSource.
	classLine = 'Ln  ',
	classMacPaintFormat = 'McPn',
	classMagicEraserTool = 'MgEr',
	classMagicPoint = 'Mgcp',	// keyHorizontal, keyVertical.
	classMask = 'Msk ',
	classMenuItem = 'Mn  ',	// Breaks hash. It was classMenu.
	classMode = 'Md  ',	// There is also a keyMode, typeMode.
	classMultichannelMode = 'MltC',	// classMode, keyMode.
	classObsoleteTextLayer = 'TxLy',	// See also: classLayer, classAdjustmentLayer, classBackgroundLayer, classTextLayer.
	classNull = 'null',
	/////// DO NOT ADD ANYTHING HERE - SEE WARNING AT TOP OF FILE ///////////
	classOffset = 'Ofst',	// There is also keyOffset, eventOffset.
	classOpacity = 'Opac',
	classOuterGlow = 'OrGl',
	classPDFGenericFormat = 'PDFG',	// Used by Parser plug-ins.
	classPICTFileFormat = 'PICF',
	classPICTResourceFormat = 'PICR',
	classPNGFormat = 'PNGF',
	classPageSetup = 'PgSt',
	classPaintbrushTool = 'PbTl',
	//		classPaintStroke		= 'PntS',	// Paint stroke recording deleted for 6.0
	classPath = 'Path',
	classPathComponent = 'PaCm',
	classPathPoint = 'Pthp',
	classPattern = 'PttR',
	classPatternStampTool = 'PaTl',
	classPencilTool = 'PcTl',
	classPhotoshop20Format = 'Pht2',
	/////// DO NOT ADD ANYTHING HERE - SEE WARNING AT TOP OF FILE ///////////
	classPhotoshop35Format = 'Pht3',
	classPhotoshopDCS2Format = 'PhD2',
	classPhotoshopDCSFormat = 'PhD1',
	classPhotoshopEPSFormat = 'PhtE',
	classPhotoshopPDFFormat = 'PhtP',
	classPixel = 'Pxel',
	classPixelPaintFormat = 'PxlP',
	classPluginPrefs = 'PlgP',
	classPoint = 'Pnt ',	// keyHorizontal, keyVertical.
	classPointUI = 'PtUI',
	classPoint16 = 'Pnt1',
	classPolygon = 'Plgn',	// keyPoints.
	classPosterize = 'Pstr',	// classAdjustment.
	classPreferences = 'GnrP',
	classProfileSetup = 'PrfS',
	/////// DO NOT ADD ANYTHING HERE - SEE WARNING AT TOP OF FILE ///////////
	classProperty = 'Prpr',
	classRange = 'Rang',
	classRect16 = 'Rct1',
	classRGBColor = 'RGBC',	// keyRed, keyGreen, keyBlue.
	classRGBColorMode = 'RGBM',	// classMode, keyMode.
	classRGBSetup = 'RGBt',	// Breaks hash. with RGBSetupSource
	classRawFormat = 'Rw  ',
	classRectangle = 'Rctn',	// keyTop, keyLeft, keyBottom, keyRight.
	classRectUI = 'RtUI',
	classSaturationTool = 'SrTl',
	classScitexCTFormat = 'Sctx',
	classSelection = 'csel',	// cSelection
	classSelectiveColor = 'SlcC',	// classAdjustment.
	classShapingCurve = 'ShpC',
	classSharpenTool = 'ShTl',
	/////// DO NOT ADD ANYTHING HERE - SEE WARNING AT TOP OF FILE ///////////
	classSingleColumn = 'Sngc',	// keyLeft.
	classSingleRow = 'Sngr',	// keyTop.
	classBackgroundEraserTool = 'SETl',
	classSolidFill = 'SoFi',
	classArtHistoryBrushTool = 'ABTl',
	classSmudgeTool = 'SmTl',
	classSnapshot = 'SnpS',
	classSpotColorChannel = 'SCch',
	classStyle = 'StyC',
	classSubPath = 'Sbpl',
	classTIFFFormat = 'TIFF',
	classTargaFormat = 'TrgF',
	classTextLayer = 'TxLr',	// See also: classLayer, classAdjustmentLayer, classBackgroundLayer, classObsoleteTextLayer.
	classTextStyle = 'TxtS',
	/////// DO NOT ADD ANYTHING HERE - SEE WARNING AT TOP OF FILE ///////////
	classTextStyleRange = 'Txtt',	// Collides with classTextStyle
	classThreshold = 'Thrs',	// classAdjustment.
	classTool = 'Tool',
	classTriangle = 'TAng',
	classTransferSpec = 'Trfp',	// Collides with classTransparencyStop
	classTransferPoint = 'DtnP',	// Used to be DuotonePoint
	classTransparencyPrefs = 'TrnP',
	classTransparencyStop = 'TrnS',
	classUnitsPrefs = 'UntP',
	classUnspecifiedColor = 'UnsC',
	classVersion = 'Vrsn',	// keyVersionMajor, keyVersionMinor, keyVersionFix.
	classWebdavPrefs = 'Wdbv',
	classXYYColor = 'XYYC',
	/////// DO NOT ADD ANYTHING HERE - SEE WARNING AT TOP OF FILE ///////////
	classChromeFX = 'ChFX',

	classBackLight = 'BakL',	// classBackLight. rkulkarn
	classFillFlash = 'FilF',	// classFillFlash. rkulkarn  //11/15/2000
	classColorCast = 'ColC',	// classColorCast. rkulkarn
}; // enum PIClasses

//-------------------------------------------------------------------------------
//	Enumerations.
//-------------------------------------------------------------------------------

enum PIEnums
{
	/////// DO NOT ADD ANYTHING HERE - SEE WARNING AT TOP OF FILE ///////////
	enumAdd = 'Add ',	// typeCalculation.
	enumAmountHigh = 'amHi',	// typeAmount.
	enumAmountLow = 'amLo',	// typeAmount.
	enumAmountMedium = 'amMd',	// typeAmount.
	enumAntiAliasNone = 'Anno',	// typeAntiAlias.
	enumAntiAliasLow = 'AnLo',	// typeAntiAlias.
	enumAntiAliasMedium = 'AnMd',	// typeAntiAlias.
	enumAntiAliasHigh = 'AnHi',	// typeAntiAlias.
	enumAntiAliasCrisp = 'AnCr',	// typeAntiAlias.
	enumAntiAliasStrong = 'AnSt',	// typeAntiAlias.
	enumAntiAliasSmooth = 'AnSm',	// typeAntiAlias.
	enumAppleRGB = 'AppR',	// typeBuiltinProfile.
	enumASCII = 'ASCI',	// typeEncoding.
/////// DO NOT ADD ANYTHING HERE - SEE WARNING AT TOP OF FILE ///////////
	enumAskWhenOpening = 'AskW',	// typeProfileMismatch, typeAssumeOptions.
	enumBicubic = 'Bcbc',	// typeInterpolation.
	enumBinary = 'Bnry',	// typeEncoding.
	enumMonitorSetup = 'MntS',	// typeMenuItem. Old file calibration settings menu.
	enum16BitsPerPixel = '16Bt',	// typeDeepDepth.
	enum1BitPerPixel = 'OnBt',	// typeDepth.
	enum2BitsPerPixel = '2Bts',	// typeDeepDepth.
	enum32BitsPerPixel = '32Bt',	// typeDeepDepth.
	enum4BitsPerPixel = '4Bts',	// typeDeepDepth.
	enum5000 = '5000',	// typeKelvin.
	enum5500 = '5500',	// typeKelvin.
	enum6500 = '6500',	// typeKelvin.
	enum72Color = '72Cl',	// typeDCS. Was k72ColorEnum.
	enum72Gray = '72Gr',	// typeDCS. Was k72GrayEnum.
/////// DO NOT ADD ANYTHING HERE - SEE WARNING AT TOP OF FILE ///////////
	enum7500 = '7500',	// typeKelvin.
	enum8BitsPerPixel = 'EghB',	// typeDeepDepth, typeDepth.
	enum9300 = '9300',	// typeKelvin.
	enumA = 'A   ',	// typeChannel.
	enumAbsColorimetric = 'AClr',	// typeIntent.
	enumADSBottoms = 'AdBt',	// typeAlignDistributeSelector.
	enumADSCentersH = 'AdCH',	// typeAlignDistributeSelector.
	enumADSCentersV = 'AdCV',	// typeAlignDistributeSelector.
	enumADSHorizontal = 'AdHr',	// typeAlignDistributeSelector.
	enumADSLefts = 'AdLf',	// typeAlignDistributeSelector.
	enumADSRights = 'AdRg',	// typeAlignDistributeSelector.
	enumADSTops = 'AdTp',	// typeAlignDistributeSelector.
	enumADSVertical = 'AdVr',	// typeAlignDistributeSelector.
	enumADSSpacingH = 'AdSH',  // typeAlignDistributeSelector.
	enumADSSpacingV = 'AdSV',  // typeAlignDistributeSelector.
	enumAboutApp = 'AbAp',	// typeMenuItem. About menu.
/////// DO NOT ADD ANYTHING HERE - SEE WARNING AT TOP OF FILE ///////////
	enumAbsolute = 'Absl',	// typeCorrectionMethod.
	enumActualPixels = 'ActP',	// typeMenuItem. View menu.
	enumAdaptive = 'Adpt',	// typeColorPalette.
	enumAdjustmentOptions = 'AdjO',	// typeMenuItem. Layer menu.
	enumAirbrushEraser = 'Arbs',	// typeEraserKind
	enumAll = 'Al  ',	// typeOrdinal, typePurgeItem.
	enumAmiga = 'Amga',	// typePlatform
	enumAngle = 'Angl',	// typeGradientType.
	enumAny = 'Any ',	// typeOrdinal.
	enumApplyImage = 'AplI',	// typeMenuItem. Image menu.
	enumAroundCenter = 'ArnC',	// typeZigZagType.
	enumArrange = 'Arng',	// typeMenuItem. Window menu.
/////// DO NOT ADD ANYTHING HERE - SEE WARNING AT TOP OF FILE ///////////
	enumAsk = 'Ask ',	// typeYesNo.
	enumB = 'B   ',	// typeChannel.
	enumBack = 'Back',	// typeOrdinal.
	enumBackground = 'Bckg',	// typeFillMode. There is also a keyBackground.
	enumBackgroundColor = 'BckC',	// typeFill, typeFillContents, typeColorStopType, typeMatteColor.
	enumBackward = 'Bckw',	// typeOrdinal.
	enumBehind = 'Bhnd',	// typeBlendMode.
	enumBest = 'Bst ',	// typeBlurQuality.
	enumBetter = 'Dthb',	// typeDitherQuality.
	enumBilinear = 'Blnr',	// typeInterpolation.
	enumBitDepth1 = 'BD1 ',	// typeBitDepth. Breaks hash.
	enumBitDepth16 = 'BD16',	// typeBitDepth. Breaks hash.
	enumBitDepth24 = 'BD24',	// typeBitDepth. Breaks hash.
	enumBitDepth32 = 'BD32',	// typeBitDepth. Breaks hash.
/////// DO NOT ADD ANYTHING HERE - SEE WARNING AT TOP OF FILE ///////////
	enumBitDepth4 = 'BD4 ',	// typeBitDepth. Breaks hash.
	enumBitDepth8 = 'BD8 ',	// typeBitDepth. Breaks hash.
	enumBitDepthA1R5G5B5 = '1565',	// typeBitDepth. Breaks hash.
	enumBitDepthR5G6B5 = 'x565',	// typeBitDepth. Breaks hash.
	enumBitDepthX4R4G4B4 = 'x444',	// typeBitDepth. Breaks hash.
	enumBitDepthA4R4G4B4 = '4444',	// typeBitDepth. Breaks hash.
	enumBitDepthX8R8G8B8 = 'x888',	// typeBitDepth. Breaks hash.
	enumBitmap = 'Btmp',	// typeColorSpace.
	enumBlack = 'Blck',	// typeGrayBehavior, typeFillContents, typeChannel.
	enumBlackAndWhite = 'BanW',	// typeForcedColors.
	enumBlackBody = 'BlcB',	// typeColorPalette.
	enumBlacks = 'Blks',	// typeColors.
	enumBlockEraser = 'Blk ',	// typeEraserKind
	enumBlast = 'Blst',	// typeWindMethod.
	enumBlocks = 'Blks',	// typeExtrudeType. CONFLICT: enumBlacks.
	enumBlue = 'Bl  ',	// typeChannel, typeDither. There is also a keyBlue.
/////// DO NOT ADD ANYTHING HERE - SEE WARNING AT TOP OF FILE ///////////
	enumBlues = 'Bls ',	// typeColors.
	enumBottom = 'Bttm',	// typeVerticalLocation.
	enumBrushDarkRough = 'BrDR',	// typeBrushType. Breaks hash.
	enumBrushesAppend = 'BrsA',	// typeMenuItem. Brushes Palette menu
	enumBrushesDefine = 'BrsD',	// typeMenuItem. Brushes Palette menu
	enumBrushesDelete = 'Brsf',	// typeMenuItem. Brushes Palette menu
	enumBrushesLoad = 'Brsd',	// typeMenuItem. Brushes Palette menu
	enumBrushesNew = 'BrsN',	// typeMenuItem. Brushes Palette menu
	enumBrushesOptions = 'BrsO',	// typeMenuItem. Brushes Palette menu
	enumBrushesReset = 'BrsR',	// typeMenuItem. Brushes Palette menu
	enumBrushesSave = 'Brsv',	// typeMenuItem. Brushes Palette menu
	enumBrushLightRough = 'BrsL',	// typeBrushType.
	enumBrushSimple = 'BrSm',	// typeBrushType. Breaks hash.
	enumBrushSize = 'BrsS',	// typeCursorKind
/////// DO NOT ADD ANYTHING HERE - SEE WARNING AT TOP OF FILE ///////////
	enumBrushSparkle = 'BrSp',	// typeBrushType. Breaks hash.
	enumBrushWideBlurry = 'BrbW',	// typeBrushType.
	enumBrushWideSharp = 'BrsW',	// typeBrushType.
	enumBuiltin = 'Bltn',	// typeRGBSetupSource, typeCompensation, typeCMYKSetupEngine, typeAssumeOptions.
	enumBurnInH = 'BrnH',	// typeBlendMode
	enumBurnInM = 'BrnM',	// typeBlendMode
	enumBurnInS = 'BrnS',	// typeBlendMode
	enumButtonMode = 'BtnM',	// typeMenuItem. Actions palette menu.
	enumCIERGB = 'CRGB',	// typePhosphors, typeBuiltinProfile.
	enumWidePhosphors = 'Wide',	// typePhosphors.
	enumWideGamutRGB = 'WRGB',	// typeBuiltinProfile.
	enumCMYK = 'CMYK',	// typeChannel.
	enumCMYK64 = 'CMSF',	// typeColorSpace. CMYK Sixty-four
	enumCMYKColor = 'ECMY',	// typeColorSpace.
/////// DO NOT ADD ANYTHING HERE - SEE WARNING AT TOP OF FILE ///////////
	enumCalculations = 'Clcl',	// typeMenuItem. Image menu.
	enumCascade = 'Cscd',	// typeMenuItem. Window menu.
	enumCenter = 'Cntr',	// typeAlignment, typeStrokeLocation.
	enumCenterGlow = 'SrcC',	// typeInnerGlowSource.
	enumCenteredFrame = 'CtrF',
	enumChannelOptions = 'ChnO',	// typeMenuItem. Channels palette menu.
	enumChannelsPaletteOptions = 'ChnP',	// typeMenuItem. Channels palette menu.
	enumCheckerboardNone = 'ChcN',	// typeCheckerboardSize
	enumCheckerboardSmall = 'ChcS',	// typeCheckerboardSize
	enumCheckerboardMedium = 'ChcM',	// typeCheckerboardSize
	enumCheckerboardLarge = 'ChcL',	// typeCheckerboardSize
	enumClear = 'Clar',	// typeBlendMode.
	enumClearGuides = 'ClrG',	// typeMenuItem. View menu.
	enumClipboard = 'Clpb',	// typePurgeItem.
/////// DO NOT ADD ANYTHING HERE - SEE WARNING AT TOP OF FILE ///////////
	enumClippingPath = 'ClpP',	// typePathKind. There is also a classClippingPath, keyClippingPath.
	enumCloseAll = 'ClsA',	// typeMenuItem. Window menu.
	enumCoarseDots = 'CrsD',	// typeMezzotintType.
	enumColor = 'Clr ',	// typeBlendMode.
	enumColorBurn = 'CBrn',	// typeBlendMode, typeCalculation.
	enumColorDodge = 'CDdg',	// typeBlendMode, typeCalculation.
	enumColorMatch = 'ClMt',	// typeBuiltinProfile. Breaks hash for foo.
	enumColorNoise = 'ClNs',	// typeGradientForm
	enumColorimetric = 'Clrm',	// typeIntent.
	enumComposite = 'Cmps',	// typeChannel.
	enumConvertToCMYK = 'CnvC',	// typeProfileMismatch. Breaks hash with other "convert to" enums
	enumConvertToGray = 'CnvG',	// typeProfileMismatch. Breaks hash with other "convert to" enums
	enumConvertToLab = 'CnvL',	// typeProfileMismatch. Breaks hash with other "convert to" enums
	enumConvertToRGB = 'CnvR',	// typeProfileMismatch. Breaks hash with other "convert to" enums
	enumCreateDuplicate = 'CrtD',	// typeInterlaceCreateType.
/////// DO NOT ADD ANYTHING HERE - SEE WARNING AT TOP OF FILE ///////////
	enumCreateInterpolation = 'CrtI',	// typeInterlaceCreateType.
	enumCross = 'Crs ',	// typeShape.
	enumCurrentLayer = 'CrrL',	// typeHistoryStateSource.
	enumCustom = 'Cst ',	// typeRGBSetupSource. Breaks hash for enumCustomPattern.
	enumCustomPattern = 'Cstm',	// typeMethod. There is also eventCustom, keyCustom.
	enumCustomStops = 'CstS',	// typeGradientForm
	enumCyan = 'Cyn ',	// typeChannel.
	enumCyans = 'Cyns',	// typeColors.
	enumDark = 'Drk ',	// typePredefinedColors
	enumDarken = 'Drkn',	// typeBlendMode, typeCalculation.
	enumDarkenOnly = 'DrkO',	// typeDiffuseMode.
	enumDashedLines = 'DshL',	// typeGuideGridStyle
	enumDesaturate = 'Dstt',	// typeBlendMode
	enumDiamond = 'Dmnd',	// typeShape, typeGradientType.
	enumDifference = 'Dfrn',	// typeBlendMode, typeCalculation.
	enumDiffusion = 'Dfsn',	// typeDither.
	enumDiffusionDither = 'DfnD',	// typeMethod.
/////// DO NOT ADD ANYTHING HERE - SEE WARNING AT TOP OF FILE ///////////
	enumDisplayCursorsPreferences = 'DspC',	// typeMenuItem. File preferences menu.
	enumDissolve = 'Dslv',	// typeBlendMode.
	enumDistort = 'Dstr',	// typeMenuItem. Edit transform menu.
	enumDodgeH = 'DdgH',	// typeBlendMode
	enumDodgeM = 'DdgM',	// typeBlendMode
	enumDodgeS = 'DdgS',	// typeBlendMode
	enumDots = 'Dts ',	// typeGuideGridStyle
	enumDraft = 'Drft',	// typeBlurQuality.
	enumDuotone = 'Dtn ',	// typeChannel.
	enumEBUITU = 'EBT ',	// typePhosphors.
	enumEdgeGlow = 'SrcE',	// typeInnerGlowSource.
	enumEliminateEvenFields = 'ElmE',	// typeInterlaceEliminateType.
	enumEliminateOddFields = 'ElmO',	// typeInterlaceEliminateType.
/////// DO NOT ADD ANYTHING HERE - SEE WARNING AT TOP OF FILE ///////////
	enumEllipse = 'Elps',	// typeShape.
	enumEmboss = 'Embs',	// typeBevelEmbossStyle. There is also an eventEmboss.
	enumExact = 'Exct',	// typeColorPalette.
	enumExclusion = 'Xclu',	// typeBlendMode, typeCalculation.
	enumFPXCompressLossyJPEG = 'FxJP',	// typeFPXCompress. Breaks hash.
	enumFPXCompressNone = 'FxNo',	// typeFPXCompress. Breaks hash.
	enumFaster = 'Dthf',	// typeDitherQuality.
	enumFile = 'Fle ',	// typeRGBSetupSource. Breaks hash for enumFill.
	enumFileInfo = 'FlIn',	// typeMenuItem. File menu.
	enumFillBack = 'FlBc',	// typeFillColor.
	enumFillFore = 'FlFr',	// typeFillColor.
	enumFillInverse = 'FlIn',	// typeFillColor. CONFLICT: enumFileInfo.
	enumFillSame = 'FlSm',	// typeFillColor.
	enumFineDots = 'FnDt',	// typeMezzotintType.
/////// DO NOT ADD ANYTHING HERE - SEE WARNING AT TOP OF FILE ///////////
	enumFirst = 'Frst',	// typeOrdinal.
	enumFirstIdle = 'FrId',	// PR#17472, typeNotify
	enumFitOnScreen = 'FtOn',	// typeMenuItem. View menu.
	enumForegroundColor = 'FrgC',	// typeFillContents, typeColorStopType, typeMatteColor.
	enumForward = 'Frwr',	// typeOrdinal.
	enumFreeTransform = 'FrTr',	// typeMenuItem. Layer menu.
	enumFront = 'Frnt',	// typeOrdinal.
	enumFullDocument = 'FllD',	// typeHistoryStateSource.
	enumFullSize = 'FlSz',	// typePreview.
	enumGaussianDistribution = 'Gsn ',	// typeDistribution. 
	enumGIFColorFileColorTable = 'GFCT',	// typeGIFColorFileType.
	enumGIFColorFileColors = 'GFCF',	// typeGIFColorFileType.
/////// DO NOT ADD ANYTHING HERE - SEE WARNING AT TOP OF FILE ///////////
	enumGIFColorFileMicrosoftPalette = 'GFMS',	// typeGIFColorFileType.
	enumGIFPaletteAdaptive = 'GFPA',	// typeGIFPaletteType.
	enumGIFPaletteExact = 'GFPE',	// typeGIFPaletteType.
	enumGIFPaletteOther = 'GFPO',	// typeGIFPaletteType.
	enumGIFPaletteSystem = 'GFPS',	// typeGIFPaletteType.
	enumGIFRequiredColorSpaceIndexed = 'GFCI',	// typeGIFRequiredColorSpaceType.
	enumGIFRequiredColorSpaceRGB = 'GFRG',	// typeGIFRequiredColorSpaceType.
	enumGIFRowOrderInterlaced = 'GFIN',	// typeGIFRowOrderType.
	enumGIFRowOrderNormal = 'GFNI',	// typeGIFRowOrderType.
	enumGeneralPreferences = 'GnrP',	// typeMenuItem. File preferences menu.
	enumGood = 'Gd  ',	// typeBlurQuality.
	enumGradientFill = 'GrFl',	// typeFrameFill
	enumGrainClumped = 'GrnC',	// typeGrainType.
	enumGrainContrasty = 'GrCn',	// typeGrainType. Breaks hash.
	enumGrainEnlarged = 'GrnE',	// typeGrainType.
	enumGrainHorizontal = 'GrnH',	// typeGrainType.
	enumGrainRegular = 'GrnR',	// typeGrainType.
	enumGrainSoft = 'GrSf',	// typeGrainType. Breaks hash.
/////// DO NOT ADD ANYTHING HERE - SEE WARNING AT TOP OF FILE ///////////
	enumGrainSpeckle = 'GrSp',	// typeGrainType. Breaks hash.
	enumGrainSprinkles = 'GrSr',	// typeGrainType. Breaks hash.
	enumGrainStippled = 'GrSt',	// typeGrainType. Breaks hash.
	enumGrainVertical = 'GrnV',	// typeGrainType.
	enumGrainyDots = 'GrnD',	// typeMezzotintType.
	enumGraphics = 'Grp ',	// typeIntent.
	enumGray = 'Gry ',	// typeFillContents, typeChannel, typeColor. There is also a keyGray.
	enumGray16 = 'GryX',	// typeColorSpace. GRaY siXteen.
	enumGray18 = 'Gr18',	// typeBuiltinProfile.
	enumGray22 = 'Gr22',	// typeBuiltinProfile.
	enumGray50 = 'Gr50',	// typeMatteColor.
	enumGrayScale = 'Gryc',	// typeColorSpace. Breaks hash for enumGrayscale ('Grys').

// Mac Rez is "case insensitive" w/ respect to enum names. So "enumGrayScale" and "enumGrayscale"
// can not both be enums without choking the Mac Rez tool. To workaround this we change one of
// them to be a preprocessor macro
	//enumGrayscale = 'Grys',	// typeColorSpace, typeColorPalette.
	#define enumGrayscale 'Grys'	// typeColorSpace, typeColorPalette.

	enumGreen = 'Grn ',	// typeChannel, typeColor. There is also a keyGreen.
	enumGreens = 'Grns',	// typeColors.
/////// DO NOT ADD ANYTHING HERE - SEE WARNING AT TOP OF FILE ///////////
	enumGuidesGridPreferences = 'GudG',	// typeMenuItem. File preferences menu.
	enumHDTV = 'HDTV',	// typePhosphors.
	enumHSBColor = 'HSBl',	// typeColorSpace. Breaks hash.
	enumHSLColor = 'HSLC',	// typeColorSpace.
	enumHalftoneFile = 'HlfF',	// typeMethod.
	enumHalftoneScreen = 'HlfS',	// typeMethod. There is also keyHalftoneScreen, eventHalftoneScreen, classHalftoneScreen
	enumHardLight = 'HrdL',	// typeBlendMode, typeCalculation.
	enumHeavy = 'Hvy ',	// typeBlackGeneration.
	enumHideAll = 'HdAl',	// typeUserMaskOptions.
	enumHideSelection = 'HdSl',	// typeUserMaskOptions.
	enumHigh = 'High',	// typeUrgency.
	enumHighQuality = 'Hgh ', 	// typeQuality.
	enumHighlights = 'Hghl',	// typeColors.
	enumHistogram = 'Hstg',	// typeMenuItem. Image menu.
	enumHistory = 'Hsty',	// typePurgeItem.
/////// DO NOT ADD ANYTHING HERE - SEE WARNING AT TOP OF FILE ///////////
	enumHistoryPaletteOptions = 'HstO',	// typeMenuItem. History palette menu.
	enumHistoryPreferences = 'HstP',	// typeMenuItem. File preferences menu.
	enumHorizontal = 'Hrzn',	// typeOrientation. There is also a keyHorizontal.
	enumHorizontalOnly = 'HrzO',	// typeSpherizeMode.
	enumHue = 'H   ',	// typeBlendMode.
	enumIBMPC = 'IBMP',	// typePlatform.
	enumICC = 'ICC ',	// typeCMYKSetupEngine, typeAssumeOptions.
	enumIcon = 'Icn ',	// typePreview.
	enumIdleVM = 'IdVM',	// typeState
	enumIgnore = 'Ignr',	// typeProfileMismatch.
	enumImage = 'Img ',	// typeIntent, typeAreaSelector.
	enumImageCachePreferences = 'ImgP',	// typeMenuItem. File preferences menu.
	enumIndexedColor = 'Indl',	// typeColorSpace. Breaks hash.
	enumInfoPaletteOptions = 'InfP',	// typeMenuItem. Info palette menu.
	enumInfoPaletteToggleSamplers = 'InfT',	// typeMenuItem. Info palette menu.
	enumInnerBevel = 'InrB',	// typeBevelEmbossStyle.
/////// DO NOT ADD ANYTHING HERE - SEE WARNING AT TOP OF FILE ///////////
	enumInsetFrame = 'InsF',
	enumInside = 'Insd',	// typeStrokeLocation.
	enumJPEG = 'JPEG',	// typeDepth, typeEncoding. There is also a classJPEGFormat.
	enumJustifyAll = 'JstA',	// typeAlignment.
	enumJustifyFull = 'JstF',	// typeAlignment.
	enumKeepProfile = 'KPro',	// typeProfileMismatch.
	enumKeyboardPreferences = 'KybP',	// typeMenuItem. File preferences menu.
	enumLab = 'Lab ',	// typeChannel.
	enumLab48 = 'LbCF',	// typeColorSpace. LaB Color Forty-eight.
	enumLabColor = 'LbCl',	// typeColorSpace.
	enumLarge = 'Lrg ',	// typeRippleSize.
	enumLast = 'Lst ',	// typeOrdinal.
	enumLastFilter = 'LstF',	// typeMenuItem. Filter menu.
	enumLayerOptions = 'LyrO',	// typeMenuItem. Layer menu.
	enumLayersPaletteOptions = 'LyrP',	// typeMenuItem. Layers palette menu.
	enumLeft = 'Left',	// From UActionsTerminology. typeHorizontalLocation. CONFLICT: enumLeft ('Lft ').
	enumLeft_PLUGIN = 'Lft ',	// From AdobePITerminology. typeDirection, typeAlignment. CONFLICT: enumLeft ('Left').
	enumLevelBased = 'LvlB',	// typeExtrudeRandom.
	enumLight = 'Lgt ',	// typeBlackGeneration.
/////// DO NOT ADD ANYTHING HERE - SEE WARNING AT TOP OF FILE ///////////
	enumLightBlue = 'LgtB',	// typeGuideGridColor
	enumLightDirBottom = 'LDBt',	// typeLightDirection. Breaks hash.
	enumLightDirBottomLeft = 'LDBL',	// typeLightDirection. Breaks hash.
	enumLightDirBottomRight = 'LDBR',	// typeLightDirection. Breaks hash.
	enumLightDirLeft = 'LDLf',	// typeLightDirection. Breaks hash.
	enumLightDirRight = 'LDRg',	// typeLightDirection. Breaks hash.
	enumLightDirTop = 'LDTp',	// typeLightDirection. Breaks hash.
	enumLightDirTopLeft = 'LDTL',	// typeLightDirection. Breaks hash.
	enumLightDirTopRight = 'LDTR',	// typeLightDirection. Breaks hash.
	enumLightGray = 'LgtG',	// typeGuideGridColor
	enumLightDirectional = 'LghD',	// typeLightType.
	enumLightenOnly = 'LghO',	// typeDiffuseMode. CONFLICT: enumLightOmni.
	enumLightOmni = 'LghO',	// typeLightType. CONFLICT: enumLightenOnly.
	enumLightPosBottom = 'LPBt',	// typeLightPosition. Breaks hash.
	enumLightPosBottomLeft = 'LPBL',	// typeLightPosition. Breaks hash.
	enumLightPosBottomRight = 'LPBr',	// typeLightPosition. Breaks hash.
	enumLightPosLeft = 'LPLf',	// typeLightPosition. Breaks hash.
	enumLightPosRight = 'LPRg',	// typeLightPosition. Breaks hash.
/////// DO NOT ADD ANYTHING HERE - SEE WARNING AT TOP OF FILE ///////////
	enumLightPosTop = 'LPTp',	// typeLightPosition. Breaks hash.
	enumLightPosTopLeft = 'LPTL',	// typeLightPosition. Breaks hash.
	enumLightPosTopRight = 'LPTR',	// typeLightPosition. Breaks hash.
	enumLightRed = 'LgtR',	// typeGuideGridColor
	enumLightSpot = 'LghS',	// typeLightType.
	enumLighten = 'Lghn',	// typeBlendMode, typeCalculation. Breaks hash for Lightness.
	enumLightness = 'Lght',	// typeChannel.
	enumLine = 'Ln  ',	// typeShape.
	enumLines = 'Lns ',	// typeGuideGridStyle
	enumLinear = 'Lnr ',	// typeGradientType.
	enumLinked = 'Lnkd',	// typeOrdinal.
	enumLongLines = 'LngL',	// typeMezzotintType.
	enumLongStrokes = 'LngS',	// typeMezzotintType.
	enumLow = 'Low ',	// typeUrgency.
	enumLower = 'Lwr ',	// typeContourEdge.
	enumLowQuality = 'Lw  ', 	// typeQuality.
	enumLuminosity = 'Lmns',	// typeBlendMode.
	enumMaya = 'Maya',	// typePlatform
	enumMacThumbnail = 'McTh',	// typePreview.
	enumMacintosh = 'Mcnt',	// typePlatform, typeEPSPreview.
/////// DO NOT ADD ANYTHING HERE - SEE WARNING AT TOP OF FILE ///////////
	enumMacintoshSystem = 'McnS',	// typeColorPalette.
	enumMagenta = 'Mgnt',	// typeChannel.
	enumMagentas = 'Mgnt',	// typeColors. CONFLICT: enumMagenta.
	enumMask = 'Msk ',	// typeChannel.
	enumMaskedAreas = 'MskA',	// typeMaskIndicator.
	enumMasterAdaptive = 'MAdp',	// typeColorPalette.
	enumMasterPerceptual = 'MPer',	// typeColorPalette.
	enumMasterSelective = 'MSel',	// typeColorPalette.
	enumMaximum = 'Mxmm',	// typeBlackGeneration.
	enumMaximumQuality = 'Mxm ', 	// typeQuality. There is also a keyMaximum, eventMaximum.
	enumMedium = 'Mdim',	// typeBlackGeneration
	enumMediumBlue = 'MdmB',	// typeGuideGridColor
	enumMediumQuality = 'Mdm ',	// typeQuality, typeRippleSize
	enumMediumDots = 'MdmD',	// typeMezzotintType.
	enumMediumLines = 'MdmL',	// typeMezzotintType.
	enumMediumStrokes = 'MdmS',	// typeMezzotintType.
	enumMemoryPreferences = 'MmrP',	// typeMenuItem. File preferences menu.
	enumMergeChannels = 'MrgC',	// typeMenuItem. Channels palette menu.
	enumMerged = 'Mrgd',	// typeOrdinal.
/////// DO NOT ADD ANYTHING HERE - SEE WARNING AT TOP OF FILE ///////////
	enumMergedLayers = 'Mrg2',	// typeHistoryStateSource.		new for PSCS2
	enumMergedLayersOld = 'MrgL',	// typeHistoryStateSource.		used for PSCS1 and prior
	enumMiddle = 'Mddl',	// typeOrdinal.
	enumMidtones = 'Mdtn',	// typeColors.
	enumModeGray = 'MdGr',	// typeMode.
	enumModeRGB = 'MdRG',	// typeMode.
	enumMonitor = 'Moni',	// typeRGBSetupSource, typeAssumeOptions.
	enumMonotone = 'Mntn',	// typeChannel.
	enumMulti72Color = '72CM',	// typeDCS. Was kMulti72ColorEnum.
	enumMulti72Gray = '72GM',	// typeDCS. Was kMulti72GrayEnum.
	enumMultichannel = 'Mlth',	// typeColorSpace. Breaks hash.
	enumMultiNoCompositePS = 'NCmM',	// typeDCS. Was kMultiNoCompositePSEnum.
	enumMultiply = 'Mltp',	// typeBlendMode, typeCalculation.
	enumNavigatorPaletteOptions = 'NvgP',	// typeMenuItem. Navigator palette menu.
	enumNearestNeighbor = 'Nrst',	// typeInterpolation.
	enumNetscapeGray = 'NsGr',	// typeMatteColor.
	enumNeutrals = 'Ntrl',	// typeColors.
	enumNewView = 'NwVw',	// typeMenuItem. View menu.
/////// DO NOT ADD ANYTHING HERE - SEE WARNING AT TOP OF FILE ///////////
	enumNext = 'Nxt ',	// typeOrdinal.
	enumNikon = 'Nkn ',	// typeLens.
	enumNikon105 = 'Nkn1',	// typeLens.
	enumNo = 'N   ',	// typeYesNo.
	enumNoCompositePS = 'NCmp',	// typeDCS. Was kNoCompositePSEnum.
	enumNone = 'None',	// typeOrdinal, typePreview, typeCompensation, typeBlackGeneration, typeAssumeOptions, typeForcedColors, typeMatteColor.
	enumNormal = 'Nrml',	// typeBlendMode, typeCalculation, typeSherizeMode, typeDiffuseMode.
	enumNormalPath = 'NrmP',	// typePathKind. Breaks hash.
	enumNTSC = 'NTSC',	// typePhosphors, typeBuiltinProfile.
	enumNull = 'null',
	enumOS2 = 'OS2 ',	// typePlatform. Breaks hash.
	enumOff = 'Off ',	// typeOnOff.
	enumOn = 'On  ',	// typeOnOff.
	enumOpenAs = 'OpAs',	// typeMenuItem. File menu.
	enumOrange = 'Orng', 	// typeColor.
	enumOutFromCenter = 'OtFr',	// typeZigZagType.
	enumOutOfGamut = 'OtOf',	// typeColors.
	enumOuterBevel = 'OtrB',	// typeBevelEmbossStyle.
	enumOutside = 'Otsd',	// typeStrokeLocation.
/////// DO NOT ADD ANYTHING HERE - SEE WARNING AT TOP OF FILE ///////////
	enumOutsetFrame = 'OutF',
	enumOverlay = 'Ovrl',	// typeBlendMode, typeCalculation.
	enumPaintbrushEraser = 'Pntb',	// typeEraserKind
	enumPencilEraser = 'Pncl',	// typeEraserKind
	enumP22EBU = 'P22B',	// typePhosphors.
	enumPNGFilterAdaptive = 'PGAd',	// typePNGFilter. Breaks hash.
	enumPNGFilterAverage = 'PGAv',	// typePNGFilter. Breaks hash.
	enumPNGFilterNone = 'PGNo',	// typePNGFilter. Breaks hash.
	enumPNGFilterPaeth = 'PGPt',	// typePNGFilter. Breaks hash. NOTE: File format named after Alan Paeth. Spelling is correct.
	enumPNGFilterSub = 'PGSb',	// typePNGFilter. Breaks hash.
	enumPNGFilterUp = 'PGUp',	// typePNGFilter. Breaks hash.
	enumPNGInterlaceAdam7 = 'PGIA',	// typePNGInterlaceType. Breaks hash.
	enumPNGInterlaceNone = 'PGIN',	// typePNGInterlaceType. Breaks hash.
	enumPagePosCentered = 'PgPC',	// typePagePosition. Breaks hash.
	enumPagePosTopLeft = 'PgTL',	// typePagePosition. Breaks hash.
	enumPageSetup = 'PgSt',	// typeMenuItem. File menu.
	enumPalSecam = 'PlSc',	// typeBuiltinProfile.
	enumPanaVision = 'PnVs',	// typeLens.
/////// DO NOT ADD ANYTHING HERE - SEE WARNING AT TOP OF FILE ///////////
	enumPathsPaletteOptions = 'PthP',	// typeMenuItem. Paths palette menu.
	enumPattern = 'Ptrn',	// typeDither, typeFillContents, typePurgeItem.
	enumPatternDither = 'PtnD',	// typeMethod.
	enumPerceptual = 'Perc',	// typeColorPalette.
	enumPerspective = 'Prsp',	// typeMenuItem. Edit transform menu.
	enumPhotoshopPicker = 'Phtk',	// typePickerKind.
	enumPickCMYK = 'PckC',	// typeMenuItem. Color Palette menu
	enumPickGray = 'PckG',	// typeMenuItem. Color Palette menu
	enumPickHSB = 'PckH',	// typeMenuItem. Color Palette menu
	enumPickLab = 'PckL',	// typeMenuItem. Color Palette menu
	enumPickOptions = 'PckO',	// typeMenuItem. Color Palette menu
	enumPickRGB = 'PckR',	// typeMenuItem. Color Palette menu
	enumPillowEmboss = 'PlEb',	// typeBevelEmbossStyle.
	enumPixelPaintSize1 = 'PxS1',	// Breaks hash.
	enumPixelPaintSize2 = 'PxS2',	// Breaks hash.
	enumPixelPaintSize3 = 'PxS3',	// Breaks hash.
	enumPixelPaintSize4 = 'PxS4',	// Breaks hash.
	enumPlace = 'Plce',	// typeMenuItem. File menu.
	enumPlaybackOptions = 'PbkO',	// typeMenuItem. Actions palette menu.
	enumPluginPicker = 'PlgP',	// typePickerKind.
/////// DO NOT ADD ANYTHING HERE - SEE WARNING AT TOP OF FILE ///////////
	enumPluginsScratchDiskPreferences = 'PlgS',	// typeMenuItem. File preferences menu.
	enumPolarToRect = 'PlrR',	// typeConvert.
	enumPondRipples = 'PndR',	// typeZigZagType.
	enumPrecise = 'Prc ',	// typeCursorKind
	enumPreciseMatte = 'PrBL',	// typeMatteTechnique
	enumPreviewOff = 'PrvO',  // typePreviewCMYK.
	enumPreviewCMYK = 'PrvC',  // typePreviewCMYK.
	enumPreviewCyan = 'Prvy',  // typePreviewCMYK.
	enumPreviewMagenta = 'PrvM',  // typePreviewCMYK.
	enumPreviewYellow = 'PrvY',  // typePreviewCMYK.
	enumPreviewBlack = 'PrvB',  // typePreviewCMYK.
	enumPreviewCMY = 'PrvN',  // typePreviewCMYK.
	enumPrevious = 'Prvs',	// typeColorPalette, typeOrdinal.
	enumPrimaries = 'Prim',	// typeForcedColors.
	enumPrintSize = 'PrnS',	// typeMenuItem. View menu.
	enumPrintingInksSetup = 'PrnI',	// typeMenuItem. Old file calibration settings menu.
	enumPurple = 'Prp ',	// typePredefinedColors
	enumPyramids = 'Pyrm',	// typeExtrudeType.
	enumQCSAverage = 'Qcsa',	// typeQuadCenterState.
	enumQCSCorner0 = 'Qcs0',	// typeQuadCenterState.
	enumQCSCorner1 = 'Qcs1',	// typeQuadCenterState.
/////// DO NOT ADD ANYTHING HERE - SEE WARNING AT TOP OF FILE ///////////
	enumQCSCorner2 = 'Qcs2',	// typeQuadCenterState.
	enumQCSCorner3 = 'Qcs3',	// typeQuadCenterState.
	enumQCSIndependent = 'Qcsi',	// typeQuadCenterState.
	enumQCSSide0 = 'Qcs4',	// typeQuadCenterState.
	enumQCSSide1 = 'Qcs5',	// typeQuadCenterState.
	enumQCSSide2 = 'Qcs6',	// typeQuadCenterState.
	enumQCSSide3 = 'Qcs7',	// typeQuadCenterState.
	enumQuadtone = 'Qdtn',	// typeChannel.
	enumQueryAlways = 'QurA',	// typeQueryState
	enumQueryAsk = 'Qurl',	// typeQueryState
	enumQueryNever = 'QurN', 	// typeQueryState
	enumRepeat = 'Rpt ',	// typeFillMode.
	enumRGB = 'RGB ',	// typeGrayBehavior, typeChannel.
	enumRGB48 = 'RGBF',	// typeColorSpace. RGB Forty-eight
	enumRGBColor = 'RGBC',	// typeColorSpace.
	enumRadial = 'Rdl ',	// typeGradientType.
	enumRandom = 'Rndm',	// typeExtrudeRandom.
/////// DO NOT ADD ANYTHING HERE - SEE WARNING AT TOP OF FILE ///////////
	enumRectToPolar = 'RctP',	// typeConvert.
	enumRed = 'Rd  ',	// typeChannel, typeColor.
	enumRedrawComplete = 'RdCm',	// typeState.
	enumReds = 'Rds ',	// typeColors.
	enumReflected = 'Rflc',	// typeGradientType.
	enumRelative = 'Rltv',	// typeCorrectionMethod.
	enumRepeatEdgePixels = 'RptE',	// typeUndefinedArea.
	enumRevealAll = 'RvlA',	// typeUserMaskOptions.
	enumRevealSelection = 'RvlS',	// typeUserMaskOptions.
	enumRevert = 'Rvrt',	// typeBlendMode
	enumRight = 'Rght',	// typeDirection, typeAlignment, typeHorizontalLocation.
	enumRotate = 'Rtte',	// typeMenuItem. Edit transform menu.
	enumRotoscopingPreferences = 'RtsP',	// typeMenuItem. File preferences menu.
	enumRound = 'Rnd ',	// typeShape.
	enumRulerCm = 'RrCm',	// typeRulerUnits. Breaks hash.
	enumRulerInches = 'RrIn',	// typeRulerUnits. Breaks hash.
/////// DO NOT ADD ANYTHING HERE - SEE WARNING AT TOP OF FILE ///////////
	enumRulerPercent = 'RrPr',	// typeRulerUnits. Breaks hash.
	enumRulerPicas = 'RrPi',	// typeRulerUnits. Breaks hash.
	enumRulerPixels = 'RrPx',	// typeRulerUnits. Breaks hash.
	enumRulerPoints = 'RrPt',	// typeRulerUnits. Breaks hash.
	enumAdobeRGB1998 = 'SMPT',	// typePhosphors, typeBuiltinProfile.
	enumSMPTEC = 'SMPC',	// typePhosphors. typeBuiltinProfile.
	enumSRGB = 'SRGB',	// typeBuiltinProfile.
	enumSample3x3 = 'Smp3',	// typeEyeDropperSample.
	enumSample5x5 = 'Smp5',	// typeEyeDropperSample.
	enumSamplePoint = 'SmpP',	// typeEyeDropperSample.
	enumSaturate = 'Str ',	// typeBlendMode. Breaks hash with enumSaturation
	enumSaturation = 'Strt',	// typeBlendMode.
	enumSaved = 'Sved',	// typeFillContents.
	enumSaveForWeb = 'Svfw',	// typeMenuItem
	enumSavingFilesPreferences = 'SvnF',	// typeMenuItem. File preferences menu.
	enumScale = 'Scl ',	// typeMenuItem. Edit transform menu. There is also a keyScale.
	enumScreen = 'Scrn',	// typeBlendMode, typeCalculation.
	enumScreenCircle = 'ScrC',	// typeScreenType.
/////// DO NOT ADD ANYTHING HERE - SEE WARNING AT TOP OF FILE ///////////
	enumScreenDot = 'ScrD',	// typeScreenType.
	enumScreenLine = 'ScrL',	// typeScreenType.
	enumSelectedAreas = 'SlcA',	// typeMaskIndicator.
	enumSelection = 'Slct',	// typeAreaSelector.
	enumSelective = 'Sele',	// typeColorPalette.
	enumSeparationSetup = 'SprS',	// typeMenuItem. Old file calibration settings menu.
	enumSeparationTables = 'SprT',	// typeMenuItem. Old file calibration settings menu.
	enumShadows = 'Shdw',	// typeColors.
	enumContourLinear = 'sp01',	// typeBuiltInContour
	enumContourGaussian = 'sp02',
	enumContourSingle = 'sp03',
	enumContourDouble = 'sp04',
	enumContourTriple = 'sp05',
	enumContourCustom = 'sp06',
	enumShortLines = 'ShrL',	// typeMezzotintType.
	enumShortStrokes = 'ShSt',	// typeMezzotintType. Breaks hash.
	enumSingle72Color = '72CS',	// typeDCS. Was kSngl72ColorEnum.
	enumSingle72Gray = '72GS',	// typeDCS. Was kSngl72GrayEnum.
	enumSingleNoCompositePS = 'NCmS',	// typeDCS. Was kSnglNoCompositePSEnum.
	enumSkew = 'Skew',	// typeMenuItem. Edit transform menu.
	enumSlopeLimitMatte = 'Slmt',
/////// DO NOT ADD ANYTHING HERE - SEE WARNING AT TOP OF FILE ///////////
	enumSmall = 'Sml ',	// typeRippleSize.
	enumSmartBlurModeEdgeOnly = 'SBME',	// typeSmartBlurMode.
	enumSmartBlurModeNormal = 'SBMN',	// typeSmartBlurMode.
	enumSmartBlurModeOverlayEdge = 'SBMO',	// typeSmartBlurMode.
	enumSmartBlurQualityHigh = 'SBQH',	// typeSmartBlurQuality.
	enumSmartBlurQualityLow = 'SBQL',	// typeSmartBlurQuality.
	enumSmartBlurQualityMedium = 'SBQM',	// typeSmartBlurQuality.
	enumSnapshot = 'Snps',	// typeFillContents, typePurgeItem.
	enumSolidColor = 'SClr',	// typeFrameFill
	enumSoftLight = 'SftL',	// typeBlendMode, typeCalculation.
	enumSoftMatte = 'SfBL',	// typeMatteTechnique
	enumSpectrum = 'Spct',	// typeColorPalette.
	enumSpin = 'Spn ',	// typeBlurMethod.
	enumSpotColor = 'Spot',	// typeMaskIndicator.
	enumSquare = 'Sqr ',	// typeShape.
	enumStagger = 'Stgr',	// typeWindMethod.
	enumStampIn = 'In  ',	// typeBevelEmbossStampStyle.
	enumStampOut = 'Out ',	// typeBevelEmbossStampStyle.
	enumStandard = 'Std ',	// typeCursorKind
	enumStdA = 'StdA',	// typeKelvin.
	enumStdB = 'StdB',	// typeKelvin.
/////// DO NOT ADD ANYTHING HERE - SEE WARNING AT TOP OF FILE ///////////
	enumStdC = 'StdC',	// typeKelvin.
	enumStdE = 'StdE',	// typeKelvin.
	enumStretchToFit = 'StrF',	// typeDisplacementMap.
	enumStrokeDirHorizontal = 'SDHz',	// typeStrokeDirection. Breaks hash. Was enumStokeDirHorizontal.
	enumStrokeDirLeftDiag = 'SDLD',	// typeStrokeDirection. Breaks hash. Was enumStrokeDirLeftDiag.
	enumStrokeDirRightDiag = 'SDRD',	// typeStrokeDirection. Breaks hash. Was enumStokeDirRightDiag.
	enumStrokeDirVertical = 'SDVt',	// typeStrokeDirection. Breaks hash. Was enumStrokeDirVertical.
	enumStylesAppend = 'SlsA',	// typeMenuItem. Styles Palette menu
	enumStylesDelete = 'Slsf',	// typeMenuItem. Styles Palette menu
	enumStylesLoad = 'Slsd',	// typeMenuItem. Styles Palette menu
	enumStylesNew = 'SlsN',	// typeMenuItem. Styles Palette menu
	enumStylesReset = 'SlsR',	// typeMenuItem. Styles Palette menu
	enumStylesSave = 'Slsv',	// typeMenuItem. Styles Palette menu
	enumSubtract = 'Sbtr',	// typeCalculation.
	enumSwatchesAppend = 'SwtA',	// typeMenuItem. Swatches Palette menu
	enumSwatchesReplace = 'Swtp',	// typeMenuItem. Swatches Palette menu
	enumSwatchesReset = 'SwtR',	// typeMenuItem. Swatches Palette menu
	enumSwatchesSave = 'SwtS',	// typeMenuItem. Swatches Palette menu
	enumSystemPicker = 'SysP',	// typePickerKind.
	enumTables = 'Tbl ',	// typeCMYKSetupEngine.
/////// DO NOT ADD ANYTHING HERE - SEE WARNING AT TOP OF FILE ///////////
	enumTarget = 'Trgt',	// typeOrdinal.
	enumTargetPath = 'Trgp',	// typePathKind. There is also a keyTargetPath. Breaks hash.
	enumTexTypeBlocks = 'TxBl',	// typeTextureType. Breaks hash.
	enumTexTypeBrick = 'TxBr',	// typeTextureType. Breaks hash.
	enumTexTypeBurlap = 'TxBu',	// typeTextureType. Breaks hash.
	enumTexTypeCanvas = 'TxCa',	// typeTextureType. Breaks hash.
	enumTexTypeFrosted = 'TxFr',	// typeTextureType. Breaks hash.
	enumTexTypeSandstone = 'TxSt',	// typeTextureType. Breaks hash.
	enumTexTypeTinyLens = 'TxTL',	// typeTextureType. Breaks hash.
	enumThreshold = 'Thrh',	// typeMethod.
	enumThumbnail = 'Thmb',	// typePreview.
	enumTIFF = 'TIFF', 	// typeEPSPreview.
	enumTile = 'Tile',	// From UActionsTerminology. typeMenuItem. Window menu. CONFLICT: enumTile ('Tl  ').
	enumTile_PLUGIN = 'Tl  ',	// From AdobePITerminology. typeDisplacementMap. CONFLICT: enumTile ('Tile').
	enumToggleActionsPalette = 'TglA',	// typeMenuItem. Window menu.
	enumToggleBlackPreview = 'TgBP',	// typeMenuItem. View menu.
	enumToggleBrushesPalette = 'TglB',	// typeMenuItem. Window menu.
	enumToggleCMYKPreview = 'TglC',	// typeMenuItem. View menu.
	enumToggleCMYPreview = 'TgCM',	// typeMenuItem. View menu.
/////// DO NOT ADD ANYTHING HERE - SEE WARNING AT TOP OF FILE ///////////
	enumToggleChannelsPalette = 'Tglh',	// typeMenuItem. Window menu. Breaks hash for ToggleCMYKPreview.
	enumToggleColorPalette = 'Tglc',	// typeMenuItem. Window menu. Breaks hash for TogglePaths.
	enumToggleCyanPreview = 'TgCP',	// typeMenuItem. View menu.	Breaks hash for ToggleCMYKPreview.
	enumToggleEdges = 'TglE',	// typeMenuItem. View menu.
	enumToggleGamutWarning = 'TglG',	// typeMenuItem. View menu.
	enumToggleGrid = 'TgGr',	// typeMenuItem. View menu.
	enumToggleGuides = 'Tgld',	// typeMenuItem. View menu. Breaks hash for ToggleGamutWarning.
	enumToggleHistoryPalette = 'TglH',	// typeMenuItem. Window menu.
	enumToggleInfoPalette = 'TglI',	// typeMenuItem. Window menu.
	enumToggleLayerMask = 'TglM',	// typeMenuItem. Edit transform menu. Breaks hash for ToggleLockGuides.
	enumToggleLayersPalette = 'Tgly',	// typeMenuItem. Window menu. Breaks hash for ToogleLockGuides.
	enumToggleLockGuides = 'TglL',	// typeMenuItem. View menu.
	enumToggleMagentaPreview = 'TgMP',	// typeMenuItem. View menu.
	enumToggleNavigatorPalette = 'TglN',	// typeMenuItem. Window menu.
	enumToggleOptionsPalette = 'TglO',	// typeMenuItem. Window menu.
/////// DO NOT ADD ANYTHING HERE - SEE WARNING AT TOP OF FILE ///////////
	enumTogglePaths = 'TglP',	// typeMenuItem. View menu.
	enumTogglePathsPalette = 'Tglt',	// typeMenuItem. Window menu. Breaks hash for TogglePaths.
	enumToggleRGBMacPreview = 'TrMp',	// typeMenuItem. Window menu
	enumToggleRGBWindowsPreview = 'TrWp',	// typeMenuItem. Window menu
	enumToggleRGBUncompensatedPreview = 'TrUp',	// typeMenuItem. Window menu
	enumToggleRulers = 'TglR',	// typeMenuItem. View menu.
	enumToggleSnapToGrid = 'TgSn',	// typeMenuItem. View menu.
	enumToggleSnapToGuides = 'TglS',	// typeMenuItem. View menu.
	enumToggleStatusBar = 'Tgls',	// typeMenuItem. Window menu. Breaks hash for ToggleSnapToGuides.
	enumToggleStylesPalette = 'TgSl',	// typeMenuItem. Window menu.
	enumToggleSwatchesPalette = 'Tglw',	// typeMenuItem. Window menu. Breaks hash for ToggleSnapToGuides.
	enumToggleToolsPalette = 'TglT',	// typeMenuItem. Window menu.
	enumToggleYellowPreview = 'TgYP',	// typeMenuItem. View menu.
	enumToggleDocumentPalette = 'TgDc',	// typeMenuItem. Window menu. Breaks hash for ToogleLockGuides.
	enumTop = 'Top ',	// typeVerticalLocation.
	enumTransparency = 'Trsp',	// typeChannel.
	enumTransparencyGamutPreferences = 'TrnG',	// typeMenuItem. File preferences menu.
	enumTransparent = 'Trns',	// typeFill.
	enumTrinitron = 'Trnt',	// typePhosphors.
	enumTritone = 'Trtn',	// typeChannel.
/////// DO NOT ADD ANYTHING HERE - SEE WARNING AT TOP OF FILE ///////////
	enumUIBitmap = 'UBtm',	// type SourceMode, for ModeChange plugin
	enumUICMYK = 'UCMY',	// type SourceMode, for ModeChange plugin
	enumUIDuotone = 'UDtn',	// type SourceMode, for ModeChange plugin
	enumUIGrayscale = 'UGry',	// type SourceMode, for ModeChange plugin
	enumUIIndexed = 'UInd',	// type SourceMode, for ModeChange plugin
	enumUILab = 'ULab',	// type SourceMode, for ModeChange plugin
	enumUIMultichannel = 'UMlt',	// type SourceMode, for ModeChange plugin
	enumUIRGB = 'URGB',	// type SourceMode, for ModeChange plugin
	enumUndo = 'Und ',	// typePurgeItem.
	enumUniform = 'Unfm',	// typeColorPalette.
	enumUniformDistribution = 'Unfr',	// typeDistribution.
	enumUnitsRulersPreferences = 'UntR',	// typeMenuItem. File preferences menu.
	enumUpper = 'Upr ',	// typeContourEdge.
	enumUserStop = 'UsrS',	// typeColorStopType.
	enumVMPreferences = 'VMPr',	// typeMenuItem. File preferences menu.
	enumVertical = 'Vrtc',	// typeOrientation.
	enumVerticalOnly = 'VrtO',	// typeSpherizeMode.
	enumViolet = 'Vlt ', 	// typeColor.
	enumWaveSine = 'WvSn',	// typeWaveType. Breaks hash.
	enumWaveSquare = 'WvSq',	// typeWaveType. Breaks hash.
	enumWaveTriangle = 'WvTr',	// typeWaveType. Breaks hash.
/////// DO NOT ADD ANYTHING HERE - SEE WARNING AT TOP OF FILE ///////////
	enumWeb = 'Web ',	// typeColorPalette, typeForcedColors.
	enumWhite = 'Wht ',	// typeFill, typeFillContents, typeMatteColor, typeDither.
	enumWhites = 'Whts',	// typeColors.
	enumWinThumbnail = 'WnTh',	// typePreview.
	enumWind = 'Wnd ',	// typeWindMethod.
	enumWindows = 'Win ',	// typePlatform. Breaks hash.
	enumWindowsSystem = 'WndS',	// typeColorPalette.
	enumWrap = 'Wrp ',	// typeFillMode.
	enumWrapAround = 'WrpA',	// typeUndefinedArea.
	enumWorkPath = 'WrkP',	// typePathKind. There is also keyWorkPathIndex.
	enumYellow = 'Yllw',	// typeChannel.
	enumYellowColor = 'Ylw ', 	// typeColor. Different from other typeColor enums due to conflict with enumYellow.
	enumYellows = 'Ylws',	// typeColors.
	enumYes = 'Ys  ',	// typeYesNo.
	enumZip = 'ZpEn',	// typeEncoding.
	enumZoom = 'Zm  ',	// typeLens.
	enumZoomIn = 'ZmIn',	// typeMenuItem. View menu.
/////// DO NOT ADD ANYTHING HERE - SEE WARNING AT TOP OF FILE ///////////
	enumZoomOut = 'ZmOt',	// typeMenuItem. View menu.
};  // enum PIEnums

//-------------------------------------------------------------------------------
//	Events.
//-------------------------------------------------------------------------------

enum PIEvents
{
	event3DTransform = 'TdT ',
	eventAverage = 'Avrg',
	eventApplyStyle = 'ASty',
	eventAssert = 'Asrt',
	eventAccentedEdges = 'AccE',
	eventAdd = 'Add ',
	eventAddNoise = 'AdNs',
	eventAddTo = 'AddT',
	eventAlign = 'Algn',
/////// DO NOT ADD ANYTHING HERE - SEE WARNING AT TOP OF FILE ///////////
	eventAll = 'All ',
	eventAngledStrokes = 'AngS',
	eventApplyImage = 'AppI',
	eventBasRelief = 'BsRl',
	eventBatch = 'Btch',
	eventBatchFromDroplet = 'BtcF',
	eventBlur = 'Blr ',
	eventBlurMore = 'BlrM',	// There is also keyBlurMethod, typeBlurMethod.
	eventBorder = 'Brdr',
	eventBrightness = 'BrgC',
	eventCanvasSize = 'CnvS',
	eventChalkCharcoal = 'ChlC',
	eventChannelMixer = 'ChnM',
	eventCharcoal = 'Chrc',
	eventChrome = 'Chrm',
/////// DO NOT ADD ANYTHING HERE - SEE WARNING AT TOP OF FILE ///////////
	eventClear = 'Cler',
	eventClose = 'Cls ',
	eventClouds = 'Clds',
	eventColorBalance = 'ClrB',
	eventColorHalftone = 'ClrH',
	eventColorRange = 'ClrR',
	eventColoredPencil = 'ClrP',
	eventConteCrayon = 'CntC',
	eventContract = 'Cntc',	// Breaks hash for Center.
	eventConvertMode = 'CnvM',
	eventCopy = 'copy',	// kAECopy
	eventCopyEffects = 'CpFX',
	eventCopyMerged = 'CpyM',
	eventCopyToLayer = 'CpTL',
	eventCraquelure = 'Crql',
	eventCreateDroplet = 'CrtD',	// There is also an enumCreateDuplicate
	eventCrop = 'Crop',
	eventCrosshatch = 'Crsh',
	eventCrystallize = 'Crst',
/////// DO NOT ADD ANYTHING HERE - SEE WARNING AT TOP OF FILE ///////////
	eventCurves = 'Crvs',
	eventCustom = 'Cstm',	// There is also keyCustom, enumCustomPattern.
	eventCut = 'cut ',	// kAECut
	eventCutToLayer = 'CtTL',
	eventCutout = 'Ct  ',
	eventDarkStrokes = 'DrkS',
	eventDeInterlace = 'Dntr',
	eventDefinePattern = 'DfnP',
	eventDefringe = 'Dfrg',	// Breaks hash for Difference.
	eventDelete = 'Dlt ',
	eventDesaturate = 'Dstt',	// Breaks hash for Distribution.
	eventDeselect = 'Dslc',
	eventDespeckle = 'Dspc',
	eventDifferenceClouds = 'DfrC',
	eventDiffuse = 'Dfs ',
/////// DO NOT ADD ANYTHING HERE - SEE WARNING AT TOP OF FILE ///////////
	eventDiffuseGlow = 'DfsG',
	eventDisableLayerFX = 'dlfx',
	eventDisplace = 'Dspl',
	eventDistribute = 'Dstr',
	eventDraw = 'Draw',	// Draw Line
	eventDryBrush = 'DryB',
	eventDuplicate = 'Dplc',	// There is also a keyDuplicate.
	eventDustAndScratches = 'DstS',
	eventEmboss = 'Embs',	// There is also an enumEmboss.
	eventEqualize = 'Eqlz',
	eventExchange = 'Exch',
	eventExpand = 'Expn',
	eventExport = 'Expr',
	eventExtrude = 'Extr',
	eventFacet = 'Fct ',
/////// DO NOT ADD ANYTHING HERE - SEE WARNING AT TOP OF FILE ///////////
	eventFade = 'Fade',
	eventFeather = 'Fthr',
	eventFibers = 'Fbrs',
	eventFill = 'Fl  ',	// There is also keyFill, typeFill.
	eventFilmGrain = 'FlmG',
	eventFilter = 'Fltr',
	eventFindEdges = 'FndE',
	eventFlattenImage = 'FltI',
	eventFlip = 'Flip',
	eventFragment = 'Frgm',
	eventFresco = 'Frsc',
	eventGaussianBlur = 'GsnB',
	eventGet = 'getd',	// kAEGetData
	eventGlass = 'Gls ',
	eventGlowingEdges = 'GlwE',
/////// DO NOT ADD ANYTHING HERE - SEE WARNING AT TOP OF FILE ///////////
	eventGradient = 'Grdn',
	eventGradientMap = 'GrMp',
	eventGrain = 'Grn ',
	eventGraphicPen = 'GraP',	// Breaks Hash for "Group with Prev."
	eventGroup = 'GrpL',
	eventGrow = 'Grow',
	eventHalftoneScreen = 'HlfS',	// There is also keyHalftoneScreen, enumHalftoneScreen, classHalftoneScreen.
	eventHide = 'Hd  ',
	eventHighPass = 'HghP',
	eventHSBHSL = 'HsbP',
	eventHueSaturation = 'HStr',
	eventImageSize = 'ImgS',
	eventImport = 'Impr',
	eventInkOutlines = 'InkO',
	eventIntersect = 'Intr',
/////// DO NOT ADD ANYTHING HERE - SEE WARNING AT TOP OF FILE ///////////
	eventIntersectWith = 'IntW',
	eventInverse = 'Invs',	// Breaks hash for Invert.
	eventInvert = 'Invr',
	eventLensFlare = 'LnsF',
	eventLevels = 'Lvls',
	eventLightingEffects = 'LghE',
	eventLink = 'Lnk ',
	eventMake = 'Mk  ',	// Was kMakeEvent.
	eventMaximum = 'Mxm ',	// There is also a keyMaximum, enumMaximumQuality.
	eventMedian = 'Mdn ',
	eventMergeLayers = 'Mrg2',		// starting in PSCS2, use this constant for merge actions
	eventMergeLayersOld = 'MrgL',		// previous to PSCS2 actions use this for merge
	eventMergeSpotChannel = 'MSpt',
	eventMergeVisible = 'MrgV',
/////// DO NOT ADD ANYTHING HERE - SEE WARNING AT TOP OF FILE ///////////
	eventMezzotint = 'Mztn',
	eventMinimum = 'Mnm ',	// There is also a keyMinimum.
	eventMosaic = 'Msc ',	// From UActionsTerminology. CONFLICT: eventMosaic ('MscT'). Was kMosaicEvent.
	eventMosaic_PLUGIN = 'MscT',	// From AdobePITerminology. CONFLICT: eventMosaic ('Msc ').
	eventMotionBlur = 'MtnB',
	eventMove = 'move',	// kAEMove
	eventNTSCColors = 'NTSC',
	eventNeonGlow = 'NGlw',
	eventNext = 'Nxt ',
	eventNotePaper = 'NtPr',
	eventNotify = 'Ntfy',	// PR#17472
	eventNull = 'null',
/////// DO NOT ADD ANYTHING HERE - SEE WARNING AT TOP OF FILE ///////////
	eventOceanRipple = 'OcnR',
	eventOffset = 'Ofst',	// There is also keyOffset, classOffset.
	eventOpen = 'Opn ',
//	eventPaint		= 'Pnt ',	// Paint stroke recording deleted for 6.0
	eventPaintDaubs = 'PntD',
	eventPaletteKnife = 'PltK',
	eventPaste = 'past',	// kAEPaste
	eventPasteEffects = 'PaFX',
	eventPasteInto = 'PstI',
	eventPasteOutside = 'PstO',
	eventPatchwork = 'Ptch',
	eventPhotocopy = 'Phtc',
	eventPinch = 'Pnch',
	eventPlace = 'Plc ',
	eventPlaster = 'Plst',
	eventPlasticWrap = 'PlsW',
	eventPlay = 'Ply ',
/////// DO NOT ADD ANYTHING HERE - SEE WARNING AT TOP OF FILE ///////////
	eventPointillize = 'Pntl',
	eventPolar = 'Plr ',
	eventPosterEdges = 'PstE',
	eventPosterize = 'Pstr',
	eventPrevious = 'Prvs',
	eventPrint = 'Prnt',
	eventProfileToProfile = 'PrfT',
	eventPurge = 'Prge',
	eventQuit = 'quit',	// kAEQuitApplication
	eventRadialBlur = 'RdlB',
	eventRasterize = 'Rstr',
	eventRasterizeTypeSheet = 'RstT',
	eventRemoveBlackMatte = 'RmvB',
	eventRemoveLayerMask = 'RmvL',
	eventRemoveWhiteMatte = 'RmvW',
	eventRename = 'Rnm ',
	eventReplaceColor = 'RplC',
	eventReset = 'Rset',
/////// DO NOT ADD ANYTHING HERE - SEE WARNING AT TOP OF FILE ///////////
	eventReticulation = 'Rtcl',
	eventRevert = 'Rvrt',
	eventRipple = 'Rple',
	eventRotate = 'Rtte',
	eventRoughPastels = 'RghP',
	eventSave = 'save',	// kAESave
	eventSelect = 'slct',	// kAESelect. Was kSelectEvent.
	eventSelectiveColor = 'SlcC',
	eventSet = 'setd',	// kAESetData
	eventSharpenEdges = 'ShrE',
	eventSharpen = 'Shrp',
	eventSharpenMore = 'ShrM',
	eventShear = 'Shr ',
	eventShow = 'Shw ',
	eventSimilar = 'Smlr',
	eventSmartBlur = 'SmrB',
	eventSmooth = 'Smth',
	eventSmudgeStick = 'SmdS',
/////// DO NOT ADD ANYTHING HERE - SEE WARNING AT TOP OF FILE ///////////
	eventSolarize = 'Slrz',
	eventSpatter = 'Spt ',
	eventSpherize = 'Sphr',
	eventSplitChannels = 'SplC',
	eventSponge = 'Spng',
	eventSprayedStrokes = 'SprS',
	eventStainedGlass = 'StnG',
	eventStamp = 'Stmp',
	eventStop = 'Stop',
	eventStroke = 'Strk',
	eventSubtract = 'Sbtr',
	eventSubtractFrom = 'SbtF',
	eventSumie = 'Smie',	// Breaks hash.
	eventTakeMergedSnapshot = 'TkMr',
	eventTakeSnapshot = 'TkSn',
	eventTextureFill = 'TxtF',
	eventTexturizer = 'Txtz',	// Breaks hash.
	eventThreshold = 'Thrs',
	eventTiles = 'Tls ',
	eventTornEdges = 'TrnE',
	eventTraceContour = 'TrcC',
	eventTransform = 'Trnf',	// Breaks hash for Transparent.
/////// DO NOT ADD ANYTHING HERE - SEE WARNING AT TOP OF FILE ///////////
	eventTrap = 'Trap',
	eventTwirl = 'Twrl',
	eventUnderpainting = 'Undr',
	eventUndo = 'undo',	// kAEUndo
	eventUngroup = 'Ungr',
	eventUnlink = 'Unlk',
	eventUnsharpMask = 'UnsM',
	eventVariations = 'Vrtn',
	eventWait = 'Wait',
	eventWatercolor = 'Wtrc',
	eventWaterPaper = 'WtrP',
	eventWave = 'Wave',	// Breaks hash.
	eventWind = 'Wnd ',
	eventZigZag = 'ZgZg',
/////// DO NOT ADD ANYTHING HERE - SEE WARNING AT TOP OF FILE ///////////

	eventBackLight = 'BacL',
	eventFillFlash = 'FilE', //rkulkarn 11/15/2000
	eventColorCast = 'ColE',
	eventOpenUntitled = 'OpnU',
}; // enum PIEvents

//-------------------------------------------------------------------------------
//	Forms.
//-------------------------------------------------------------------------------

enum PIForms
{
	/////// DO NOT ADD ANYTHING HERE - SEE WARNING AT TOP OF FILE ///////////
	formClass = 'Clss',
	formEnumerated = 'Enmr',
	formIdentifier = 'Idnt',					// Also keyID.
	formIndex = 'indx',					// formAbsolutePosition
	formOffset = 'rele',					// formRelativePosition
	formProperty = 'prop',					// formPropertyID
}; // enum PIForms

//-------------------------------------------------------------------------------
//	Keys.
//-------------------------------------------------------------------------------

enum PIKeys
{
	key3DAntiAlias = 'Alis',	// Is of typeAntiAlias.
	keyA = 'A   ',
/////// DO NOT ADD ANYTHING HERE - SEE WARNING AT TOP OF FILE ///////////
	keyAdjustment = 'Adjs',
	keyAligned = 'Algd',	// Collides with keyAlignment
	keyAlignment = 'Algn',
	keyAllPS = 'All ',
	keyAllExcept = 'AllE',
	keyAllToolOptions = 'AlTl',
	keyAlphaChannelOptions = 'AChn',	// Property. Breaks hash.
	keyAlphaChannels = 'AlpC',
	keyAmbientBrightness = 'AmbB',
	keyAmbientColor = 'AmbC',
	keyAmount = 'Amnt',	// There is also a typeAmount.
	keyAmplitudeMax = 'AmMx',	// Breaks hash.
	keyAmplitudeMin = 'AmMn',	// Breaks hash.
	keyAnchor = 'Anch',
	keyAngle = 'Angl',
	keyAngle1 = 'Ang1',
	keyAngle2 = 'Ang2',
/////// DO NOT ADD ANYTHING HERE - SEE WARNING AT TOP OF FILE ///////////
	keyAngle3 = 'Ang3',
	keyAngle4 = 'Ang4',
	keyAntiAlias = 'AntA',
	keyAppend = 'Appe',
	keyApply = 'Aply',
	keyArea = 'Ar  ',
	keyArrowhead = 'Arrw',
	keyAs = 'As  ',
	keyAssetBin = 'Asst',
	keyAssumedCMYK = 'AssC',
	keyAssumedGray = 'AssG',
	keyAssumedRGB = 'AssR',
	keyAt = 'At  ',
	keyAuto = 'Auto',
	keyAutoContrast = 'AuCo',
	keyAutoErase = 'Atrs',
	keyAutoKern = 'AtKr',
	keyAutoUpdate = 'AtUp',
	keyShowMenuColors = 'SwMC',
	keyAxis = 'Axis',
/////// DO NOT ADD ANYTHING HERE - SEE WARNING AT TOP OF FILE ///////////
	keyB = 'B   ',
	keyBackground = 'Bckg',	// There is also an enumBackground.
	keyBackgroundColor = 'BckC',
	keyBackgroundLevel = 'BckL',
	keyBackward = 'Bwd ',
	keyBalance = 'Blnc',
	keyBaselineShift = 'Bsln',
	keyBeepWhenDone = 'BpWh',
	keyBeginRamp = 'BgnR',
	keyBeginSustain = 'BgnS',
	keyBevelDirection = 'bvlD',
	keyBevelEmboss = 'ebbl',
	keyBevelStyle = 'bvlS',
	keyBevelTechnique = 'bvlT',
	keyBigNudgeH = 'BgNH',	// Property. Breaks hash.
	keyBigNudgeV = 'BgNV',	// Property. Breaks hash.
/////// DO NOT ADD ANYTHING HERE - SEE WARNING AT TOP OF FILE ///////////
	keyBitDepth = 'BtDp',	// There is also a keyBitDepth.
	keyBlack = 'Blck',
	keyBlackClip = 'BlcC',
	keyBlackGeneration = 'Blcn',	// Breaks hash for keyBlackGenerationCurve.
	keyBlackGenerationCurve = 'BlcG',
	keyBlackIntensity = 'BlcI',
	keyBlackLevel = 'BlcL',	// CONFLICT: keyBlackLimit.
	keyBlackLimit = 'BlcL',	// CONFLICT: keyBlackLevel.
	keyBleed = 'Bld ',
	keyBlendRange = 'Blnd',	// Breaks hash for bilinear.
	keyBlue = 'Bl  ',	// typeColor. There is also an enumBlue.
	keyBlueBlackPoint = 'BlBl',
	keyBlueGamma = 'BlGm',
	keyBlueWhitePoint = 'BlWh',
	keyBlueX = 'BlX ',
	keyBlueY = 'BlY ',
/////// DO NOT ADD ANYTHING HERE - SEE WARNING AT TOP OF FILE ///////////
	keyBlur = 'blur',
	keyBlurMethod = 'BlrM',	// There is also a typeBlurMethod.
	keyBlurQuality = 'BlrQ',	// There is also a typeBlurQuality.
	keyBook = 'Bk  ',
	keyBorderThickness = 'BrdT',
	keyBottom = 'Btom',
	keyBrightness = 'Brgh',
	keyBrushDetail = 'BrsD',
	keyBrushes = 'Brsh',
	keyBrushSize = 'BrsS',
	keyBrushType = 'BrsT',
	keyBumpAmplitude = 'BmpA',
	keyBumpChannel = 'BmpC',
	keyBy = 'By  ',
	keyByline = 'Byln',
	keyBylineTitle = 'BylT',
	keyByteOrder = 'BytO',
	keyCachePrefs = 'CchP',
	keyChokeMatte = 'Ckmt',
	keyCloneSource = 'ClnS',
	keyCMYKSetup = 'CMYS',
/////// DO NOT ADD ANYTHING HERE - SEE WARNING AT TOP OF FILE ///////////
	keyCalculation = 'Clcl',
	keyCalibrationBars = 'Clbr',
	keyCaption = 'Cptn',	// Property.
	keyCaptionWriter = 'CptW',
	keyCategory = 'Ctgr',
	keyCellSize = 'ClSz',
	keyCenter = 'Cntr',
	keyCenterCropMarks = 'CntC',
	keyChalkArea = 'ChlA',
	keyChannel = 'Chnl',	// Does not conflict. Last character is l, not captial i.
	keyChannelMatrix = 'ChMx',
	keyChannelName = 'ChnN',	// Property. Breaks hash.
	keyChannels = 'Chns',	// Breaks rule for plural
	keyChannelsInterleaved = 'ChnI',	// Does not conflict.  Last character is capital i, not L.
	keyCharcoalAmount = 'ChAm',	// Breaks hash.
	keyCharcoalArea = 'ChrA',
	keyChromeFX = 'ChFX',
	keyCity = 'City',
	keyClearAmount = 'ClrA',
/////// DO NOT ADD ANYTHING HERE - SEE WARNING AT TOP OF FILE ///////////
	keyClippingPath = 'ClPt',	// CONFLICT: Name keyClippingPathEPS.
	keyClippingPathEPS = 'ClpP',	// There is also a classClippingPath, enumClippingPath. CONFLICT: With name keyClippingPath.
	keyClippingPathFlatness = 'ClpF',	// Property. Breaks hash.
	keyClippingPathIndex = 'ClpI',	// Property. Breaks hash.
	keyClippingPathInfo = 'Clpg',	// Property. Breaks hash.
	keyClosedSubpath = 'Clsp',
	keyColor = 'Clr ',
	keyColorChannels = 'Clrh',	// Conflicts with keyColorCorrection
	keyColorCorrection = 'ClrC',
	keyColorIndicates = 'ClrI',
	keyColorManagement = 'ClMg',
	keyColorPickerPrefs = 'Clrr',	// There is also a classColorPickerPrefs.
	keyColorTable = 'ClrT',
	keyColorize = 'Clrz',
	keyColors = 'Clrs',
	keyColorsList = 'ClrL',
	keyColorSpace = 'ClrS',
	keyColumnWidth = 'ClmW',
/////// DO NOT ADD ANYTHING HERE - SEE WARNING AT TOP OF FILE ///////////
	keyCommandKey = 'CmdK',
	keyCompensation = 'Cmpn',
	keyCompression = 'Cmpr',
	keyConcavity = 'Cncv',
	keyCondition = 'Cndt',	// for ModeChange plugin
	keyConstant = 'Cnst',	// CONFLICT: keyConstrainProportions.
	keyConstrain = 'Cnst',	// CONFLICT: keyConstant.
	keyConstrainProportions = 'CnsP',
	keyConstructionFOV = 'Cfov',
	keyContiguous = 'Cntg',
	keyContinue = 'Cntn',
	keyContinuity = 'Cnty',
	keyContrast = 'Cntr',
	keyConvert = 'Cnvr',	// There is also a typeConvert.
	keyCopy = 'Cpy ',
	keyCopyright = 'Cpyr',	// Property.
	keyCopyrightNotice = 'CprN',
	keyCornerCropMarks = 'CrnC',
	keyCount = 'Cnt ',	// Use for count of objects.
	keyCountryName = 'CntN',
	keyCrackBrightness = 'CrcB',
/////// DO NOT ADD ANYTHING HERE - SEE WARNING AT TOP OF FILE ///////////
	keyCrackDepth = 'CrcD',
	keyCrackSpacing = 'CrcS',
	keyCreateLayersFromLayerFX = 'blfl',
	keyCredit = 'Crdt',
	keyCrossover = 'Crss',
	keyCurrent = 'Crnt',
	keyCurrentHistoryState = 'CrnH',
	keyCurrentLight = 'CrnL',
	keyCurrentToolOptions = 'CrnT',
	keyCurve = 'Crv ',
	keyCurveFile = 'CrvF',
	keyCustom = 'Cstm',	// There is also eventCustom, enumCustomPattern. 
	keyCustomForced = 'CstF',
	keyCustomMatte = 'CstM',
	keyCustomPalette = 'CstP',
	keyCyan = 'Cyn ',
	keyDarkIntensity = 'DrkI',
	keyDarkness = 'Drkn',
	keyDateCreated = 'DtCr',
/////// DO NOT ADD ANYTHING HERE - SEE WARNING AT TOP OF FILE ///////////
	keyDatum = 'Dt  ',	// There is a struct called keyData, so we use this cute name.
	keyDCS = 'DCS ',	// There is also a typeDCS.
	keyDefinition = 'Dfnt',
	keyDensity = 'Dnst',
	keyDepth = 'Dpth',	// There is also a typeDepth.
	keyDestBlackMax = 'Dstl',	// Breaks hash for destBlackMin.
	keyDestBlackMin = 'DstB',
	keyDestinationMode = 'DstM',	// for ModeChange plugin
	keyDestWhiteMax = 'Dstt',	// Breaks hash for destWhiteMin.
	keyDestWhiteMin = 'DstW',
	keyDetail = 'Dtl ',
	keyDiameter = 'Dmtr',
	keyDiffusionDither = 'DffD',
	keyDirection = 'Drct',
	keyDirectionBalance = 'DrcB',
	keyDisplaceFile = 'DspF',
	keyDisplacementMap = 'DspM',
/////// DO NOT ADD ANYTHING HERE - SEE WARNING AT TOP OF FILE ///////////
	keyDisplayPrefs = 'DspP',
	keyDistance = 'Dstn',	// There is also a unitDistance.
	keyDistortion = 'Dstr',	// CONFLICT: keyDistribution.
	keyDistribution = 'Dstr',	// CONFLICT: keyDistortion.
	keyDither = 'Dthr',
	keyDitherAmount = 'DthA',
	keyDitherPreserve = 'Dthp',
	keyDitherQuality = 'Dthq',
	keyDocumentID = 'DocI',
	keyDotGain = 'DtGn',
	keyDotGainCurves = 'DtGC',	// Breaks hash for keyDotGain.
	keyDPXFormat = 'DPXf',
	keyDropShadow = 'DrSh',
	keyDuplicate = 'Dplc',	// There is also an eventDuplicate.
	keyDynamicColorSliders = 'DnmC',
	keyEdge = 'Edg ',
	keyEdgeBrightness = 'EdgB',
	keyEdgeFidelity = 'EdgF',
	keyEdgeIntensity = 'EdgI',
	keyEdgeSimplicity = 'EdgS',
	keyEdgeThickness = 'EdgT',
	keyEdgeWidth = 'EdgW',
	keyEffect = 'Effc',
/////// DO NOT ADD ANYTHING HERE - SEE WARNING AT TOP OF FILE ///////////
	keyEmbedProfiles = 'EmbP',
	keyEmbedCMYK = 'EmbC',
	keyEmbedGray = 'EmbG',
	keyEmbedLab = 'EmbL',
	keyEmbedRGB = 'EmbR',
	keyEmulsionDown = 'EmlD',
	keyEnabled = 'enab',
	keyEnableGestures = 'EGst',
	keyEncoding = 'Encd',
	keyEnd = 'End ',
	keyEndArrowhead = 'EndA',
	keyEndRamp = 'EndR',
	keyEndSustain = 'EndS',
	keyEngine = 'Engn',
	keyEraserKind = 'ErsK',	// There is also a typeEraserKind
	keyEraseToHistory = 'ErsT',
	keyExactPoints = 'ExcP',
	keyExport = 'Expr',	// There is also a classExport.
	keyExportClipboard = 'ExpC',
	keyExposure = 'Exps',
	keyExtend = 'Extd',
	keyExtension = 'Extn',
	keyExtensionsQuery = 'ExtQ',
	keyExtrudeDepth = 'ExtD',
	keyExtrudeMaskIncomplete = 'ExtM',
	keyExtrudeRandom = 'ExtR',	// There is also a typeExtrudeRandom.
	keyExtrudeSize = 'ExtS',
/////// DO NOT ADD ANYTHING HERE - SEE WARNING AT TOP OF FILE ///////////
	keyExtrudeSolidFace = 'ExtF',
	keyExtrudeType = 'ExtT',	// There is also a typeExtrudeType.
	keyEyeDropperSample = 'EyDr',
	keyFadeoutSteps = 'FdtS',
	keyFadeTo = 'FdT ',
	keyFalloff = 'FlOf',
	keyFPXCompress = 'FxCm',	// There is also a typeFPXCompress. Breaks hash.
	keyFPXQuality = 'FxQl',	// Breaks hash.
	keyFPXSize = 'FxSz',	// Breaks hash.
	keyFPXView = 'FxVw',	// Breaks hash.
	keyFeather = 'Fthr',
	keyFiberLength = 'FbrL',
	keyFile = 'File',
	keyFileURL = 'furl',
	keyFileCreator = 'FlCr',
	keyFileInfo = 'FlIn',
	keyFileReference = 'FilR',
	keyFileSavePrefs = 'FlSP',
	keyFilesList = 'flst',
	keyFileType = 'FlTy',
	keyFill = 'Fl  ',	// There is also typeFill, eventFIll.
	keyFillColor = 'FlCl',	// There is also a typeFillColor.
	keyFillNeutral = 'FlNt',
	keyFilterLayerRandomSeed = 'FlRs',
/////// DO NOT ADD ANYTHING HERE - SEE WARNING AT TOP OF FILE ///////////
	keyFilterLayerPersistentData = 'FlPd',
	keyFingerpainting = 'Fngr',
	keyFlareCenter = 'FlrC',
	keyFlatness = 'Fltn',
	keyFlatten = 'Fltt',	// Break hash for Flatness.
	keyFlipVertical = 'FlpV',
	keyFocus = 'Fcs ',
	keyFolders = 'Fldr',
	keyFontDesignAxes = 'FntD',	// There is also a classFontDesignAxes
	keyFontDesignAxesVectors = 'FntV',	// Break hash for FontDesignAxes
	keyFontName = 'FntN',
	keyFontScript = 'Scrp',	// was keyScript; conflicts with <Navigation.h>.
	keyFontStyleName = 'FntS',
	keyFontTechnology = 'FntT',
	keyForcedColors = 'FrcC',
	keyForegroundColor = 'FrgC',
	keyForegroundLevel = 'FrgL',
	keyFormat = 'Fmt ',	// There is also a classFormat.
	keyForward = 'Fwd ',	// Points in a path.
	keyFrameFX = 'FrFX',
	keyFrameWidth = 'FrmW',
/////// DO NOT ADD ANYTHING HERE - SEE WARNING AT TOP OF FILE ///////////
	keyFreeTransformCenterState = 'FTcs',
	keyFrequency = 'Frqn',
	keyFrom = 'From',
	keyFromBuiltin = 'FrmB',
	keyFromMode = 'FrmM',
	keyFunctionKey = 'FncK',
	keyFuzziness = 'Fzns',
	keyGamutWarning = 'GmtW',
	keyGCR = 'GCR ',
	keyGeneralPrefs = 'GnrP',
	keyGIFColorFileType = 'GFPT',	// There is also a typeGIFColorFileType.
	keyGIFColorLimit = 'GFCL',
	keyGIFExportCaption = 'GFEC',
	keyGIFMaskChannelIndex = 'GFMI',
	keyGIFMaskChannelInverted = 'GFMV',
	keyGIFPaletteFile = 'GFPF',
	keyGIFPaletteType = 'GFPL',	// There is also a typeGIFPaletteType.
	keyGIFRequiredColorSpaceType = 'GFCS',	// There is also a typeGIFRequiredColorSpaceType.
	keyGIFRowOrderType = 'GFIT',	// There is also a typeGIFRowOrderType.
	keyGIFTransparentColor = 'GFTC',
	keyGIFTransparentIndexBlue = 'GFTB',
	keyGIFTransparentIndexGreen = 'GFTG',
	keyGIFTransparentIndexRed = 'GFTR',
	keyGIFUseBestMatch = 'GFBM',
/////// DO NOT ADD ANYTHING HERE - SEE WARNING AT TOP OF FILE ///////////
	keyGamma = 'Gmm ',
	keyGlobalAngle = 'gblA',
	keyGlobalLightingAngle = 'gagl',
	keyGloss = 'Glos',	// Breaks hash.
	keyGlowAmount = 'GlwA',
	keyGlowTechnique = 'GlwT',
	keyGPUCompatibility = 'Gcmp',
	keyGradient = 'Grad',
	keyGradientFill = 'Grdf',
	keyGrain = 'Grn ',	// CONFLICT: keyGreen.
	keyGrainType = 'Grnt',
	keyGraininess = 'Grns',
	keyGray = 'Gry ',	// There is also an enumGray.
	keyGrayBehavior = 'GrBh',
	keyGraySetup = 'GrSt',
	keyGreen = 'Grn ',	// There is also an enumGreen. CONFLICT: keyGrain.
	keyGreenBlackPoint = 'GrnB',
	keyGreenGamma = 'GrnG',
	keyGreenWhitePoint = 'GrnW',
	keyGreenX = 'GrnX',
	keyGreenY = 'GrnY',
	keyGridColor = 'GrdC',
	keyGridCustomColor = 'Grds',	// Collides with keyGridColor
	keyGridMajor = 'GrdM',	// Property.
/////// DO NOT ADD ANYTHING HERE - SEE WARNING AT TOP OF FILE ///////////
	keyGridMinor = 'Grdn',	// Property. Breaks hash.
	keyGridStyle = 'GrdS',
	keyGridUnits = 'Grdt',	// Collides with keyGridMinor
	keyGroup = 'Grup',
	keyGroutWidth = 'GrtW',
	keyGrowSelection = 'GrwS',
	keyGuides = 'Gdes',	// Property.
	keyGuidesColor = 'GdsC',
	keyGuidesCustomColor = 'Gdss',	// Collides with keyGuidesColor
	keyGuidesStyle = 'GdsS',
	keyGuidesPrefs = 'GdPr',
	keyGutterWidth = 'GttW',
	keyHalftoneFile = 'HlfF',
	keyHalftoneScreen = 'HlfS',	// There is also eventHalftoneScreen, enumHalftoneScreen, classHalftoneScreen.
	keyHalftoneSpec = 'Hlfp',	// Collides with keyHalftoneScreen
	keyHalftoneSize = 'HlSz',	// Breaks hash.
	keyHardness = 'Hrdn',
	keyHasCmdHPreference = 'HCdH',
	keyHeader = 'Hdr ',
	keyHeadline = 'Hdln',
/////// DO NOT ADD ANYTHING HERE - SEE WARNING AT TOP OF FILE ///////////
	keyHeight = 'Hght',
	keyHostName = 'HstN',
	keyHighlightArea = 'HghA',
	keyHighlightColor = 'hglC',
	keyHighlightLevels = 'HghL',
	keyHighlightMode = 'hglM',
	keyHighlightOpacity = 'hglO',
	keyHighlightStrength = 'HghS',
	keyHistoryBrushSource = 'HstB',
	keyHistoryPrefs = 'HstP',	// History prefs.
	keyHistoryStateSource = 'HsSS',
	keyHistoryStates = 'HsSt',
	keyHorizontal = 'Hrzn', // There is also an enumHorizontal.
	keyHorizontalScale = 'HrzS',
	keyHostVersion = 'HstV',
	keyHue = 'H   ',
	keyICCEngine = 'ICCE',
	keyICCSetupName = 'ICCt',	// Breaks hash for keyICCSetupSource.
	keyID = 'Idnt',	// Breaks hash to match formIndentifier.
	keyIdle = 'Idle',
/////// DO NOT ADD ANYTHING HERE - SEE WARNING AT TOP OF FILE ///////////
	keyImageBalance = 'ImgB',
	keyImport = 'Impr',	// There is also a classImport.
	keyImpressionist = 'Imps',
	keyIn = 'In  ',
	keyInherits = 'c@#^', // Hey, Apple defined it!
	keyInkColors = 'InkC',
	keyInks = 'Inks',
	keyInnerGlow = 'IrGl',
	keyInnerGlowSource = 'glwS',
	keyInnerShadow = 'IrSh',
	keyInput = 'Inpt',
	keyInputBlackPoint = 'kIBP',
	keyInputMapRange = 'Inmr',
	keyInputRange = 'Inpr',
	keyInputWhitePoint = 'kIWP',
	keyIntensity = 'Intn',
	keyIntent = 'Inte',	// Breaks hash for keyIntensity.
	keyInterfaceBevelHighlight = 'IntH',	// classInterfaceColor. Breaks hash.
	keyInterfaceBevelShadow = 'Intv',	// classInterfaceColor. Breaks hash.
/////// DO NOT ADD ANYTHING HERE - SEE WARNING AT TOP OF FILE ///////////
	keyInterfaceBlack = 'IntB',	// classInterfaceColor.
	keyInterfaceBorder = 'Intd',	// classInterfaceColor. Breaks hash.
	keyInterfaceButtonDarkShadow = 'Intk',	// classInterfaceColor. Breaks hash.
	keyInterfaceButtonDownFill = 'Intt',	// classInterfaceColor.
	keyInterfaceButtonUpFill = 'InBF',	// classInterfaceColor. Breaks hash.
	keyInterfaceColorBlue2 = 'ICBL',	// classInterfaceColor. Breaks hash.
	keyInterfaceColorBlue32 = 'ICBH',	// classInterfaceColor. Breaks hash.
	keyInterfaceColorGreen2 = 'ICGL',	// classInterfaceColor. Breaks hash.
	keyInterfaceColorGreen32 = 'ICGH',	// classInterfaceColor. Breaks hash.
	keyInterfaceColorRed2 = 'ICRL',	// classInterfaceColor. Breaks hash for InterfaceColorRedLow.
	keyInterfaceColorRed32 = 'ICRH',	// classInterfaceColor. Breaks hash for InterfaceColorRedHigh.
	keyInterfaceIconFillActive = 'IntI',	// classInterfaceColor.
	keyInterfaceIconFillDimmed = 'IntF',	// classInterfaceColor. Breaks hash.
	keyInterfaceIconFillSelected = 'Intc',	// classInterfaceColor.
	keyInterfaceIconFrameActive = 'Intm',	// classInterfaceColor. Breaks hash.
	keyInterfaceIconFrameDimmed = 'Intr',	// classInterfaceColor. Breaks hash.
	keyInterfaceIconFrameSelected = 'IntS',	// classInterfaceColor. Breaks hash.
	keyInterfacePaletteFill = 'IntP',	// classInterfaceColor.
	keyInterfaceRed = 'IntR',	// classInterfaceColor.
/////// DO NOT ADD ANYTHING HERE - SEE WARNING AT TOP OF FILE ///////////
	keyInterfaceWhite = 'IntW',	// classInterfaceColor.
	keyInterfaceToolTipBackground = 'IntT',	// classInterfaceColor.
	keyInterfaceToolTipText = 'ITTT',	// classInterfaceColor. Breaks hash
	keyInterfaceTransparencyForeground = 'ITFg',	// classInterfaceColor.
	keyInterfaceTransparencyBackground = 'ITBg',	// classInterfaceColor.
	keyInterlace = 'Intr',	// CONFLICT: keyInterpolation.
	keyInterlaceCreateType = 'IntC',	// There is also a typeInterlaceCreateType.
	keyInterlaceEliminateType = 'IntE',	// There is also a typeInterlaceEliminateType.
	keyInterpolation = 'Intr',	// CONFLICT: keyInterlace.
	keyInterpolationMethod = 'IntM',	// Property.
	keyInvert = 'Invr',
	keyInvertMask = 'InvM',
	keyInvertSource2 = 'InvS',
	keyInvertTexture = 'InvT',
	keyIsDirty = 'IsDr',	// Has the document changed since last save (or open)?
	keyItemIndex = 'ItmI',	// Actions palette classAction, classActionSet. Can't use keyIndex, that's reserved.
	keyJPEGQuality = 'JPEQ',
/////// DO NOT ADD ANYTHING HERE - SEE WARNING AT TOP OF FILE ///////////
	keyKerning = 'Krng',
	keyKeywords = 'Kywd',
	keyKind = 'Knd ',
	keyLZWCompression = 'LZWC',
	keyLabels = 'Lbls',
	keyLandscape = 'Lnds',
	keyLastTransform = 'LstT',
	keyLayerEffects = 'Lefx',
	keyLayerFXVisible = 'lfxv',
	keyLayer = 'Lyr ',	// Was kLayerKeyword.
	keyLayerID = 'LyrI',
	keyLayerName = 'LyrN',
	keyLayers = 'Lyrs',
	keyLeading = 'Ldng',
	keyLeft = 'Left',
//	keyLength		= 'Lngt',	// HFS extents also have a keyLength. Don't define this, use keyTermLength instead.
	keyTermLength = 'Lngt',		// NOTE: This used to be keyLength, but was changed to avoid conflicts with PDFL.
	keyLens = 'Lns ',
	keyLevel = 'Lvl ',
/////// DO NOT ADD ANYTHING HERE - SEE WARNING AT TOP OF FILE ///////////
	keyLevels = 'Lvls',
	keyLightDark = 'LgDr',	// Breaks hash.
	keyLightDirection = 'LghD',
	keyLightIntensity = 'LghI',
	keyLightPosition = 'LghP',
	keyLightSource = 'LghS',	// There is also a classLightSource.
	keyLightType = 'LghT',
	keyLightenGrout = 'LghG',
	keyLightness = 'Lght',
	keyLine = 'Line',
	keyLinkedLayerIDs = 'LnkL',
	keyLocalLightingAngle = 'lagl',
	keyLocalLightingAltitude = 'Lald',
	keyLocalRange = 'LclR',
	keyLocation = 'Lctn',
	keyLog = 'Log ',	// From UBatchTerminology.
	keyLogarithmic = 'kLog',
	keyLowerCase = 'LwCs',
	keyLuminance = 'Lmnc',
/////// DO NOT ADD ANYTHING HERE - SEE WARNING AT TOP OF FILE ///////////
	keyLUTAnimation = 'LTnm',
	keyMagenta = 'Mgnt',
	keyMakeVisible = 'MkVs',
	keyManipulationFOV = 'Mfov',
	keyMapBlack = 'MpBl',
	keyMapping = 'Mpng',
	keyMappingShape = 'MpgS',
	keyMaterial = 'Mtrl',
	keyMatrix = 'Mtrx',
	keyMatteColor = 'MttC',
	keyMaximum = 'Mxm ',	// There is also a keyMaximumQuality, eventMaximum.
	keyMaximumStates = 'MxmS',	// History prefs.
	keyMemoryUsagePercent = 'MmrU',
	keyMerge = 'Mrge',
	keyMerged = 'Mrgd',
	keyMessage = 'Msge',
	keyMethod = 'Mthd',
	keyMezzotintType = 'MztT',	// There is also a typeMezzotintType.
	keyMidpoint = 'Mdpn',
	keyMidtoneLevels = 'MdtL',
/////// DO NOT ADD ANYTHING HERE - SEE WARNING AT TOP OF FILE ///////////
	keyMinimum = 'Mnm ',	// There is also an eventMinimum.
	keyMismatchCMYK = 'MsmC',
	keyMismatchGray = 'MsmG',
	keyMismatchRGB = 'MsmR',
	keyMode = 'Md  ',	// There is also a classMode, typeMode.
	keyMonochromatic = 'Mnch',
	keyMoveTo = 'MvT ',
	keyName = 'Nm  ',	// NOTE: This used to be keyName, but was changed to avoid conflicts with PDFL.
	keyNegative = 'Ngtv',
	keyNew = 'Nw  ',	// Was kNewKeyword.
	keyNoise = 'Nose',
	keyNonImageData = 'NnIm',
	keyNonLinear = 'NnLn',	// History prefs.
	keyNull = 'null',
	keyNumLights = 'Nm L',
	keyNumber = 'Nmbr',
	keyNumberOfCacheLevels = 'NCch',
	keyNumberOfCacheLevels64 = 'NC64',
	keyNumberOfChannels = 'NmbO',	// Property.
	keyNumberOfChildren = 'NmbC',	// Actions palette classAction, classActionSet.
	keyNumberOfDocuments = 'NmbD',
/////// DO NOT ADD ANYTHING HERE - SEE WARNING AT TOP OF FILE ///////////
	keyNumberOfGenerators = 'NmbG',
	keyNumberOfLayers = 'NmbL',	// CONFLICT: keyNumberOfLevels.
	keyNumberOfLevels = 'NmbL',	// CONFLICT: keyNumberOfLayers.
	keyNumberOfPaths = 'NmbP',	// Property.
	keyNumberOfRipples = 'NmbR',
	keyNumberOfSiblings = 'NmbS',	// Actions palette classAction, classActionSet.
	keyObjectName = 'ObjN',
	keyOffset = 'Ofst',	// There is also classOffset, eventOffset.
	keyOn = 'On  ',
	keyOpacity = 'Opct',
	keyOptimized = 'Optm',
	keyOrientation = 'Ornt',
	keyOriginalHeader = 'OrgH',
	keyOriginalTransmissionReference = 'OrgT',
	keyOtherCursors = 'OthC',
	keyOuterGlow = 'OrGl',
	keyOutput = 'Otpt',
	keyOutputBlackPoint = 'kOBP',
	keyOutputWhitePoint = 'kOWP',
/////// DO NOT ADD ANYTHING HERE - SEE WARNING AT TOP OF FILE ///////////
	keyOverprintColors = 'OvrC',
	keyOverrideOpen = 'OvrO',
	keyOverridePrinter = 'ObrP',
	keyOverrideSave = 'Ovrd',	// Breaks hash for compatibility (used to be called override).
//	keyPaintStroke		= 'PntS',	// Paint stroke recording deleted for 6.0
	keyPaintCursorKind = 'PnCK',
	keyParentIndex = 'PrIn',	// Actions palette classAction, classActionSet.
	keyParentName = 'PrNm',	// Actions palette classAction, classActionSet.
	keyPNGFilter = 'PNGf',	// There is also a typePNGFilter.
	keyPNGInterlaceType = 'PGIT',	// There is also a typePNGInterlaceType. Breaks hash.
	keyPageFormat = 'PMpf',	// Carbon PrintManager PageFormat.
	keyPageNumber = 'PgNm',
	keyPageSetup = 'PgSt',
	keyPagePosition = 'PgPs',	// There is also a typePagePosition.
	keyPaintingCursors = 'PntC',
	keyPaintType = 'PntT',
	keyPalette = 'Plt ',
	keyPaletteFile = 'PltF',
	keyPaperBrightness = 'PprB',
	PSkeyPath = 'Path',
/////// DO NOT ADD ANYTHING HERE - SEE WARNING AT TOP OF FILE ///////////
	keyPathContents = 'PthC',	// Property.
	keyPathName = 'PthN',	// Property.
	keyPattern = 'Pttn',
	keyPencilWidth = 'Pncl',
	keyPerspectiveIndex = 'Prsp',
	keyPhosphors = 'Phsp',
	keyPickerID = 'PckI',
	keyPickerKind = 'Pckr',	// There is also a typePickerKind.
	keyPixelPaintSize = 'PPSz',	// There is also a typePixelPaintSize. Breaks hash.
	keyPlatform = 'Pltf',	// There is also a typePlatform.
	keyPluginFolder = 'PlgF',
	keyPluginPrefs = 'PlgP',
	keyPoints = 'Pts ',
	keyPosition = 'Pstn',
	keyPosterization = 'Pstr',
	keyPostScriptColor = 'PstS',
	keyPredefinedColors = 'PrdC',
	keyPreferBuiltin = 'PrfB',
/////// DO NOT ADD ANYTHING HERE - SEE WARNING AT TOP OF FILE ///////////
	keyPreserveAdditional = 'PrsA',
	keyPreserveLuminosity = 'PrsL',
	keyPreserveTransparency = 'PrsT',
	keyPressure = 'Prs ',
	keyPreferences = 'Prfr',
	keyPreview = 'Prvw',	// There is also a typePreview.
	keyPreviewCMYK = 'PrvK',
	keyPreviewFullSize = 'PrvF',
	keyPreviewIcon = 'PrvI',
	keyPreviewMacThumbnail = 'PrvM',
	keyPreviewWinThumbnail = 'PrvW',
	keyPreviewsQuery = 'PrvQ',
	keyPrintSettings = 'PMps',
	keyProfileSetup = 'PrfS',
	keyProvinceState = 'PrvS',
	keyQuality = 'Qlty',	// There is also a typeQuality.
	keyExtendedQuality = 'EQlt',	// There is also a typeQuality.
	keyQuickMask = 'QucM',	// Property.
	keyRGBSetup = 'RGBS',
	keyRadius = 'Rds ',
	keyRandomSeed = 'RndS',
	keyRatio = 'Rt  ',
/////// DO NOT ADD ANYTHING HERE - SEE WARNING AT TOP OF FILE ///////////
	keyRecentFiles = 'Rcnf',
	keyRed = 'Rd  ',	// There is also an enumRed.
	keyRedBlackPoint = 'RdBl',
	keyRedGamma = 'RdGm',
	keyRedWhitePoint = 'RdWh',
	keyRedX = 'RdX ',
	keyRedY = 'RdY ',
	keyRegistrationMarks = 'RgsM',
	keyRelative = 'Rltv',
	keyRelief = 'Rlf ',
	keyRenderFidelity = 'Rfid',	// Is of typeAmount.
	keyResample = 'Rsmp',
	keyResizeWindowsOnZoom = 'RWOZ',
	keyResolution = 'Rslt',
	keyResourceID = 'RsrI',
	keyResponse = 'Rspn',
	keyRetainHeader = 'RtnH',
	keyReverse = 'Rvrs',
	keyRight = 'Rght',
	keyRippleMagnitude = 'RplM',
/////// DO NOT ADD ANYTHING HERE - SEE WARNING AT TOP OF FILE ///////////
	keyRippleSize = 'RplS',	// There is also a typeRippleSize.
	keyRotate = 'Rtt ',
	keyRoundness = 'Rndn',
	keyRulerOriginH = 'RlrH',	// Property. Breaks hash.
	keyRulerOriginV = 'RlrV',	// Property. Breaks hash.
	keyRulerUnits = 'RlrU',	// Property.
	keySaturation = 'Strt',
	keySaveAndClose = 'SvAn',
	keySaveComposite = 'SvCm',
	keySavePaletteLocations = 'PltL',
	keySavePaths = 'SvPt',
	keySavePyramids = 'SvPy',
	keySaving = 'Svng',
	keyScale = 'Scl ',	// There is also an enumScale.
	keyScaleHorizontal = 'SclH',
	keyScaleVertical = 'SclV',
	keyScaling = 'Scln',
	keyScans = 'Scns',
	keyScratchDisks = 'ScrD',
	keyScreenFile = 'ScrF',
/////// DO NOT ADD ANYTHING HERE - SEE WARNING AT TOP OF FILE ///////////
	keyScreenType = 'ScrT',
	keyShadingIntensity = 'ShdI',
	keyShadingNoise = 'ShdN',
	keyShadingShape = 'ShdS',
	keyContourType = 'ShpC',
	keySerialString = 'SrlS',	// Property.
	keySeparations = 'Sprt',
	keyShadowColor = 'sdwC',
	keyShadowIntensity = 'ShdI',
	keyShadowLevels = 'ShdL',
	keyShadowMode = 'sdwM',
	keyShadowOpacity = 'sdwO',
	keyShape = 'Shp ',
	keySharpness = 'Shrp',
	keyShearEd = 'ShrE',
	keyShearPoints = 'ShrP',
	keyShearSt = 'ShrS',
	keyShiftKey = 'ShfK',
	keyShiftKeyToolSwitch = 'ShKT',
	keyShortNames = 'ShrN',
	keyShowEnglishFontNames = 'ShwE',
	keyShowToolTips = 'ShwT',
	keyShowTransparency = 'ShTr',	// classGradient
/////// DO NOT ADD ANYTHING HERE - SEE WARNING AT TOP OF FILE ///////////
	keySizeKey = 'Sz  ',
	keySkew = 'Skew',
	keySmartBlurMode = 'SmBM',	// There is also a typeSmartBlurMode.
	keySmartBlurQuality = 'SmBQ',	// There is also a typeSmartBlurQuality.
	keySmooth = 'Smoo',
	keySmoothness = 'Smth',
	keySnapshotInitial = 'SnpI',	// History prefs.
	keySoftClip = 'SfCl',
	keySoftness = 'Sftn',
	keySmallFontType = 'Sfts',
	keyOldSmallFontType = 'Sftt',
	keySolidFill = 'SoFi',
	keySource = 'Srce',
	keySource2 = 'Src2',
	keySourceMode = 'SrcM',	// for ModeChange plugin
	keySpacing = 'Spcn',
	keySpecialInstructions = 'SpcI',
	keySpherizeMode = 'SphM',	// There is also a typeSpherizeMode.
	keySpot = 'Spot',	// Collides with keySpatter
	keySprayRadius = 'SprR',
/////// DO NOT ADD ANYTHING HERE - SEE WARNING AT TOP OF FILE ///////////
	keySquareSize = 'SqrS',
	keySrcBlackMax = 'Srcl',	// Breaks hash for srcBlackMin.
	keySrcBlackMin = 'SrcB',
	keySrcWhiteMax = 'Srcm',	// Breaks hash for srcWhiteMin.
	keySrcWhiteMin = 'SrcW',
	keyStart = 'Strt',
	keyStartArrowhead = 'StrA',
	keyState = 'Stte',
	keyStrength = 'srgh',	// From UActionsTerminology. CONFLICTS: keyStrength ('Strg').
	keyStrengthRatio = 'srgR',
	keyStrength_PLUGIN = 'Strg',	// From AdobePITerminology. CONFLICTS: keyStrength ('srgh').
	keyStrokeDetail = 'StDt',
	keyStrokeDirection = 'SDir',	// Breaks hash.
	keyStrokeLength = 'StrL',
	keyStrokePressure = 'StrP',
	keyStrokeSize = 'StrS',
	keyStrokeWidth = 'StrW',
	keyStyle = 'Styl',
	keyStyles = 'Stys',
	keyStylusIsPressure = 'StlP',
/////// DO NOT ADD ANYTHING HERE - SEE WARNING AT TOP OF FILE ///////////
	keyStylusIsColor = 'StlC',
	keyStylusIsOpacity = 'StlO',
	keyStylusIsSize = 'StlS',
	keySubPathList = 'SbpL',
	keySupplementalCategories = 'SplC',
	keySystemInfo = 'SstI',
	keySystemPalette = 'SstP',
	keyTarget = 'null',
	keyTargetPath = 'Trgp',	// There is also an enumTargetPath.
	keyTargetPathIndex = 'TrgP',	// Property.
	keyText = 'Txt ',
	keyTextClickPoint = 'TxtC',
	keyTextData = 'TxtD',
	keyTextStyle = 'TxtS',
	keyTextStyleRange = 'Txtt',	// Collides with keyTextStyle
	keyTexture = 'Txtr',
	keyTextureCoverage = 'TxtC',	// CONFLICT: keyTextClickPoint.
	keyTextureFile = 'TxtF',
	keyTextureType = 'TxtT',
	keyThreshold = 'Thsh',
/////// DO NOT ADD ANYTHING HERE - SEE WARNING AT TOP OF FILE ///////////
	keyTileNumber = 'TlNm',
	keyTileOffset = 'TlOf',
	keyTileSize = 'TlSz',
	keyTitle = 'Ttl ',	// Property.
	keyTo = 'T   ',
	keyToBuiltin = 'TBl ',
	keyToLinked = 'ToLk',
	keyToMode = 'TMd ',
	keyToggleOthers = 'TglO',
	keyTolerance = 'Tlrn',
	keyTop = 'Top ',
	keyTotalLimit = 'TtlL',
	keyTracking = 'Trck',
	keyTransferSpec = 'TrnS',
	keyTransparencyGrid = 'TrnG',
	keyTransferFunction = 'TrnF',
	keyTransparency = 'Trns',
	keyTransparencyGridColors = 'TrnC',
	keyTransparencyGridSize = 'TrnG',
	keyTransparencyPrefs = 'TrnP',
/////// DO NOT ADD ANYTHING HERE - SEE WARNING AT TOP OF FILE ///////////
	keyTransparencyShape = 'TrnS',
	keyTransparentIndex = 'TrnI',
	keyTransparentWhites = 'TrnW',
	keyTwist = 'Twst',
	keyType = 'Type',
	keyUCA = 'UC  ',
	keyUIScaleType = 'UIsc',
	keyUnitsPrefs = 'UntP',
	keyURL = 'URL ',	// Property.
	keyUndefinedArea = 'UndA',
	keyUnderline = 'Undl',
	keyUntitled = 'Untl',
	keyUpperY = 'UppY',
	keyUrgency = 'Urgn',
	keyUseAccurateScreens = 'AcrS',
	keyUseAdditionalPlugins = 'AdPl',
	keyUseCacheForHistograms = 'UsCc',
	keyUseCurves = 'UsCr',
	keyUseDefault = 'UsDf',
	keyUseGlobalAngle = 'uglg',
	keyUseICCProfile = 'UsIC',
/////// DO NOT ADD ANYTHING HERE - SEE WARNING AT TOP OF FILE ///////////
	keyUseMask = 'UsMs',
	keyUserMaskEnabled = 'UsrM',
	keyUserMaskLinked = 'Usrs',	// Breaks hash for userMaskEnabled.
	keyLinkEnable = 'lnkE',
	keyUsing = 'Usng',
	keyValue = 'Vl  ',
	keyVariance = 'Vrnc',
	keyVector0 = 'Vct0',
	keyVector1 = 'Vct1',
	keyVectorColor = 'VctC',	// classGradient
	keyVersionFix = 'VrsF',	// classVersion.
	keyVersionMajor = 'VrsM',	// classVersion.
	keyVersionMinor = 'VrsN',	// classVersion.
	keyVertical = 'Vrtc',
	keyVerticalScale = 'VrtS',
	keyVideoAlpha = 'Vdlp',
	keyVisible = 'Vsbl',
	keyWatchSuspension = 'WtcS',	// Property.
	keyWatermark = 'watr',	// Property.
	keyWaveType = 'Wvtp',
	keyWavelengthMax = 'WLMx',	// Breaks hash.
	keyWavelengthMin = 'WLMn',	// Breaks hash.
	keyWebdavPrefs = 'WbdP',
/////// DO NOT ADD ANYTHING HERE - SEE WARNING AT TOP OF FILE ///////////
	keyWetEdges = 'Wtdg',
	keyWhat = 'What', // PR#17472
	keyWhiteClip = 'WhtC',
	keyWhiteIntensity = 'WhtI',
	keyWhiteIsHigh = 'WhHi',	// Breaks hash.
	keyWhiteLevel = 'WhtL',
	keyWhitePoint = 'WhtP',
	keyWholePath = 'WhPt',
	keyWidth = 'Wdth',
	keyWindMethod = 'WndM',
	keyWith = 'With',
	keyWorkPath = 'WrPt',
	keyWorkPathIndex = 'WrkP',	// Property. There is also enumWorkPath.
	keyX = 'X   ',
	keyY = 'Y   ',
	keyYellow = 'Ylw ',
/////// DO NOT ADD ANYTHING HERE - SEE WARNING AT TOP OF FILE ///////////
	keyZigZagType = 'ZZTy',	// There is also a typeZigZagType. Breaks hash.

	keyLighter = 'Ligh',   //rkulkarn
	keyDarker = 'Dark',	//rkulkarn
	keyStartUpInPrefs = 'Stup',

	keyLegacySerialString = 'lSNs',

	key_Source = keyTo,
}; // enum PIKeys

//-------------------------------------------------------------------------------
//	Special inheritance property.
//-------------------------------------------------------------------------------
#ifndef pInherits 
#ifdef WIN32
enum PIInherits
{
	pInherits		= keyInherits,	// This is the inheritance property. It is documented in develop 21.
}; // enum PIInherits
#endif
#endif

//-------------------------------------------------------------------------------
//	Types.	
//-------------------------------------------------------------------------------

enum PITypes
{
/////// DO NOT ADD ANYTHING HERE - SEE WARNING AT TOP OF FILE ///////////
	typeActionReference = '#Act',
	typeActionData = 'ActD',
	typeAlignDistributeSelector = 'ADSt',	// enumADSTops, enumADSCentersV, enumADSBottoms, enumADSVertical, enumADSLefts, enumADSCentersH, enumADSRights, enumADSHorizontal, enumADSSpacingH, enumADSSpacingV. 
	typeAlignment = 'Alg ',	// enumLeft, enumCenter, enumRight, enumJustifyFull, enumJustifyAll.
	typeAmount = 'Amnt',	// There is also a keyAmount. enumAmountLow, enumAmountMedium, enumAmountHigh.
	typeAntiAlias = 'Annt',	// enumAntiAliasNone, enumAntiAliasLow, enumAntiAliasMedium, enumAntiAliasHigh.
	typeAreaSelector = 'ArSl',	// enumSelection, enumImage.
	typeAssumeOptions = 'AssO',	// enumNone, enumAskWhenOpening, enumMonitor, enumBuiltin, enumICC.
	typeBevelEmbossStampStyle = 'BESs',	// enumStampIn, enumStampOut.
	typeBevelEmbossStyle = 'BESl',	// enumOuterBevel, enumInnerBevel, enumEmboss, enumPillowEmboss.
	typeBitDepth = 'BtDp',	// There is also a keyBitDepth. enumBitDepth1, enumBitDepth4, enumBitDepth8, enumBitDepth24.
	typeBlackGeneration = 'BlcG',	// enumNone, enumLight, enumMedium ('Mdim'), enumHeavy, enumMaximum.
	typeBlendMode = 'BlnM',	// enumNormal, enumDissolve, enumBehind, enumClear, enumMultiply, enumScreen, enumOverlay, enumSoftLight, enumHardLight, enumDarken, enumLighten, enumDifference, enumHue, enumSaturation, enumColor, enumLuminosity, enumExclusion, enumColorDodge, enumColorBurn.
	typeBlurMethod = 'BlrM',	// There is also a keyBlurMethod. enumSpin.
	typeBlurQuality = 'BlrQ',	// There is also a keyBlurQuality. enumDraft, enumGood, enumBest.
	typeBrushType = 'BrsT',	// enumBrushSimple, enumBrushLightRough, enumBrushDarkRough, enumBrushWideSharp, enumBrushWideBlurry.
	typeBuiltinProfile = 'BltP',	// enumAppleRGB, enumSRGB, enumCIERGB, enumNTSC, enumPalSecam, enumAdobeRGB1998, enumSMPTEC, enumGray18, enumGray22. 
	typeBuiltInContour = 'BltC',
/////// DO NOT ADD ANYTHING HERE - SEE WARNING AT TOP OF FILE ///////////
	typeCMYKSetupEngine = 'CMYE',	// enumBuiltin, enumICC, enumTables.
	typeCalculation = 'Clcn',	// enumNormal, enumMultiply, enumScreen, enumOverlay, enumSoftLight, enumHardLight, enumDarken, enumLighten, enumDifference, enumExclusion, enumColorDodge, enumColorBurn, enumAdd, enumSubtract.
	typeChannel = 'Chnl',	// enumBlack, enumCMYK, enumRGB, enumLab, enumRed, enumGreen, enumCyan, enumLightness, enumBlue, enumMagenta, enumYellow, enumA, enumB, enumComposite, enumMask, enumMonotone, enumDuotone, enumTritone, enumQuadtone, enumTransparency.
	typeChannelReference = '#ChR',
	typeCheckerboardSize = 'Chck',	// enumCheckerboardNone, enumCheckerboardSmall, enumCheckerboardMedium, enumCheckerboardLarge
	typeClass = typeType,
	typeClassColor = '#Clr',
	typeClassElement = '#ClE',	// Specifies multiple types.  Read Develop 21 for more info.
	typeClassExport = '#Cle',	// CONFLICTS: typeClassElement. Changed '#ClE' -> '#Cle'. Specifies multiple types.  Read Develop 21 for more info. CONFLICT: typeClassElement.
	typeClassFormat = '#ClF',	// Specifies multiple types.  Read Develop 21 for more info.
	typeClassHueSatHueSatV2 = '#HsV',
	typeClassImport = '#ClI',	// Specifies multiple types.  Read Develop 21 for more info.
	typeClassMode = '#ClM',	// Specifies multiple types.  Read Develop 21 for more info.
	typeClassStringFormat = '#ClS',	// Specifies multiple types.  Read Develop 21 for more info.
	typeClassTextExport = '#CTE',	// Breaks hash.
	typeClassTextImport = '#ClT',	// Specifies multiple types.  Read Develop 21 for more info.
	typeColor = 'Clr ', 	// enumRed, enumOrange, enumYellowColor, enumGreen, enumSeafoam, enumBlue, enumIndigo, enumMagentaColor, enumFuchsia, enumViolet, enumGray.
	typeColorChannel = '#ClC', 	// Specifies multiple types.  Read Develop 21 for more info.
	typeColorPalette = 'ClrP',	// enumExact, enumWeb, enumUniform, enumAdaptive, enumPerceptual, enumSelective, enumMasterAdaptive, enumMasterPerceptual, enumMasterSeletive, enumPrevious, enumSpectrum, enumGrayscale, enumBlackBody, enumMacintoshSystem, enumWindowsSystem.
	typeColorSpace = 'ClrS',	// enumGrayscale, enumRGBColor, enumCMYKColor, enumLabColor, enumBitmap, enumGrayScale, enumGray16, enumIndexedColor, enumRGB48, enumCMYK64, enumHSLColor, enumHSBColor, enumMultichannel, enumLab48.
	typeColorStopType = 'Clry',	// enumForegroundColor, enumBackgroundColor, enumUserStop.
/////// DO NOT ADD ANYTHING HERE - SEE WARNING AT TOP OF FILE ///////////
	typeColors = 'Clrs',	// enumReds, enumYellows, enumGreens, enumCyans, enumBlues, enumMagentas, enumWhites, enumNeutrals, enumBlacks, enumHighlights, enumMidtones, enumShadows, enumOutOfGamut.
	typeCompensation = 'Cmpn',	// enumNone, enumBuiltin.
	typeContourEdge = 'CntE',	// enumUpper, enumLower.
	typeConvert = 'Cnvr',	// There is also a keyConvert. enumRectToPolar, enumPolarToRect.
	typeCorrectionMethod = 'CrcM',	// enumRelative, enumAbsolute.
	typeCursorKind = 'CrsK',	// enumStandard, enumPrecise, enumBrushSize
	typeDCS = 'DCS ',	// enumSingleNoCompositePS, enumSingle72Gray, enumSingle72Color, enumMultiNoCompositePS, enumMulti72Gray, enumMulti72Color, enumNoCompositePS, enum72Gray, enum72Color. There is also a keyDCS.
	typeDeepDepth = 'DpDp',	// enum2BitsPerPixel, enum4BitsPerPixel, enum8BitsPerPixel, enum16BitsPerPixel, enum32BitsPerPixel.
	typeDepth = 'Dpth',	// enum1BitPerPixel, enum8BitsPerPixel. There is also a keyDepth.
	typeDiffuseMode = 'DfsM',	// enumNormal, enumLightenOnly, enumDarkenOnly.
	typeDirection = 'Drct',	// enumLeft, enumRight.
	typeDisplacementMap = 'DspM',	// enumStretchToFit, enumTile.
	typeDistribution = 'Dstr',	// enumUniformDistribution, enumGaussianDistribution.
	typeDither = 'Dthr',	// enumPattern, enumDiffusion.
	typeDitherQuality = 'Dthq',	// enumBetter, enumFaster.
	typeDocumentReference = '#DcR',
	typeEPSPreview = 'EPSP',	// enumTIFF, enumMacintosh.
	typeElementReference = '#ElR',
/////// DO NOT ADD ANYTHING HERE - SEE WARNING AT TOP OF FILE ///////////
	typeEncoding = 'Encd',	// enumASCII, enumBinary, enumJPEG, enumZip.
	typeEraserKind = 'ErsK',	// There is also a keyEraserKind
	typeExtrudeRandom = 'ExtR',	// There is also a keyExtrudeRandom. enumRandom, enumLevelBased.
	typeExtrudeType = 'ExtT',	// There is also a keyExtrudeType. enumBlocks, enumPyramids.
	typeEyeDropperSample = 'EyDp',	// enumSamplePoint, enumSample3x3, enumSample5x5.
	typeFPXCompress = 'FxCm',	// There is also a keyFPXCompress. Breaks hash. enumFPXCompressNone, enumFPXCompressLossyJPEG.
	typeFill = 'Fl  ',	// enumWhite, enumBackgroundColor, enumTransparent. There is also keyFill, eventFill.
	typeFillColor = 'FlCl',	// There is also a keyFillColor. enumFillBack, enumFillFore, enumFillInverse, enumFillSame.
	typeFillContents = 'FlCn',	// enumForegroundColor, enumBackgroundColor, enumPattern, enumSaved, enumSnapshot, enumBlack, enumWhite, enumGray.
	typeFillMode = 'FlMd',	// enumBackground, enumRepeat, enumWrap.
	typeForcedColors = 'FrcC',	// enumNone, enumBlackAndWhite, enumPrimaries, enumWeb.
	typeFrameFill = 'FrFl',
	typeFrameStyle = 'FStl',
	typeGIFColorFileType = 'GFPT',	// There is also a keyGIFColorFileType. enumGIFColorFileColors, enumGIFColorFileColorTable, enumGIFColorFileMicrosoftPalette.
	typeGIFPaletteType = 'GFPL',	// There is also a keyGIFPaletteType. enumGIFPaletteExact, enumGIFPaletteAdaptive, enumGIFPaletteSystem, enumGIFPaletteOther.
	typeGIFRequiredColorSpaceType = 'GFCS',	// There is also a keyGIFRequiredColorSpaceType. enumGIFRequiredColorSpaceRGB, enumGIFRequiredColorSpaceIndexed.
	typeGIFRowOrderType = 'GFIT',	// There is also a keyGIFRowOrderType. enumGIFRowOrderNormal, enumGIFRowOrderInterlaced.
	typeGlobalClass = 'GlbC',
	typeGlobalObject = 'GlbO',
	typeGradientType = 'GrdT',	// enumLinear, enumRadial, enumAngle, enumReflected, enumDiamond.
	typeGradientForm = 'GrdF',	// user stops or color noise
/////// DO NOT ADD ANYTHING HERE - SEE WARNING AT TOP OF FILE ///////////
	typeGrainType = 'Grnt',	// enumGrainRegular, enumGrainSoft, enumGrainSprinkles, enumGrainClumped, enumGrainContrasty, enumGrainEnlarged, enumGrainStippled, enumGrainHorizontal, enumGrainVertical, enumGrainSpeckle.
	typeGrayBehavior = 'GrBh',	// enumRGB, enumBlack.
	typeGuideGridColor = 'GdGr',	// enumCustom, enumLightBlue, enumLightRed, enumGreen, enumMediumBlue, enumYellow, enumMagenta, enumCyan, enumLightGray, enumBlack
	typeGuideGridStyle = 'GdGS',	// conflicts with typeGuideGridColor; enumLines, enumDashedLines, enumDots
	typeHistoryStateSource = 'HstS',	// enumFullDocument, enumMergedLayers, enumCurrentLayer.
	typeHorizontalLocation = 'HrzL',	// enumLeft, enumRight. 
	typeImageReference = '#ImR',	// <Enumerated> Used as filter direct paramter.
	typeInnerGlowSource = 'IGSr',	// enumCenterGlow, enumEdgeGlow.
	typeIntegerChannel = '#inC',
	typeIntent = 'Inte',	// enumImage, enumGraphics, enumColorimetric.
	typeInterlaceCreateType = 'IntC',	// There is also a keyInterlaceCreateType. enumCreateDuplicate, enumCreateInterpolation.
	typeInterlaceEliminateType = 'IntE',	// There is also a keyInterlaceEliminateType. enumEliminateOddFields, enumEliminateEvenFields.
	typeInterpolation = 'Intp',	// enumNearestNeighbor, enumBilinear, enumBicubic. Breaks hash for Intersect.
	typeKelvin = 'Klvn',	// enum5000, enum5500, enum6500, enum7500, enum9300, enumStdA, enumStdB, enumStdC, enumStdE.
	typeKelvinCustomWhitePoint = '#Klv',
	typeLens = 'Lns ',	// enumZoom, enumPanaVision, enumNikon, enumNikon105.
	typeLightDirection = 'LghD',	// enumLightDirBottom, enumLightDirBottomLeft, enumLightDirLeft, enumLightDirTopLeft, enumLightDirTop, enumLightDirTopRight, enumLightDirRight, enumLightDirBottomRight.
	typeLightPosition = 'LghP',	// enumLightPosBottom, enumLightPosBottomLeft, enumLightPosBottomRight, enumLightPosLeft, enumLightPosTopLeft, enumLightPosTop, enumLightPosTopRight, enumLightPosRight.
	typeLightType = 'LghT',	// enumLightDirectional, enumLightOmni, enumLightSpot.
	typeLocationReference = '#Lct',
/////// DO NOT ADD ANYTHING HERE - SEE WARNING AT TOP OF FILE ///////////
	typeMaskIndicator = 'MskI',	// enumMaskedAreas, enumSelectedAreas, enumSpotColor.
	typeMatteColor = 'MttC',	// enumNone, enumForegroundColor, enumBackgroundColor, enumWhite, enumBlack, enumGray50, enumNetscapeGray
	typeMatteTechnique = 'BETE',	// enumSoftMatte, enumPreciseBevel.
	typeMenuItem = 'MnIt',	// enumAboutAp, enumPlace, enumOpenAs, enumFileInfo, enumPageSetup, ...
	typeMethod = 'Mthd',	// enumThreshold, enumPatternDither, enumDiffusionDither, enumHalftoneScreen, enumHalftoneFile, enumCustomPattern.
	typeMezzotintType = 'MztT',	// There is also a keyMezzotintType. enumFineDots, enumMediumDots, enumGrainyDots, enumCoarseDots, enumShortLines, enumMediumLines, enumLongLines, enumShortStrokes, enumMediumStrokes, enumLongStrokes.
	typeMode = 'Md  ',	// enumModeGray, enumModeRGB. There is also keyMode, classMode.
	typeNotify = 'Ntfy',	// PR#17472, enumFirstIdle
	typeObject = 'Objc',
	typeObjectReference = 'obj ',
	typeOnOff = 'OnOf',	// enumOn, enumOff.
	typeOrdinal = 'Ordn',	// enumAll, enumFirst, enumLast, enumNext, enumPrevious, enumMiddle, enumAny, enumNone, enumTarget, enumForward, enumBackward, enumFront, enumBack, enumMerged, enumLinked.
	typeOrientation = 'Ornt',	// enumHorizontal, enumVertical.
	typePNGFilter = 'PNGf',	// There is also a keyPNGFilter. enumPNGFilterNone, enumPNGFilterSub, enumPNGFilterUp, enumPNGFilterAverage, enumPNGFilterPaeth, enumPNGFilterAdaptive.
	typePNGInterlaceType = 'PGIT',	// There is also a keyPNGInterlaceType. Breaks hash. enumPNGInterlaceNone, enumPNGInterlaceAdam7.
	typePagePosition = 'PgPs',	// There is also a keyPagePosition. enumPagePosTopLeft, enumPagePosCentered.
	typePathKind = 'PthK',	// enumNormalPath, enumWorkPath, enumClippingPath, enumTargetPath.
	typePathReference = '#PtR',
	typePhosphors = 'Phsp',	// enumCIERGB, enumEBUITU, enumHDTV, enumNTSC, enumP22EBU, enumAdobeRGB1998, enumSMPTEC, enumTrinitron.
	typePhosphorsCustomPhosphors = '#Phs',
/////// DO NOT ADD ANYTHING HERE - SEE WARNING AT TOP OF FILE ///////////
	typePickerKind = 'PckK',	// enumSystemPicker, enumPhotoshopPicker, enumPluginPicker.
	typePixelPaintSize = 'PPSz',	// There is also a keyPixelPaintSize. Breaks hash. enumPixelPaintSize1, enumPixelPaintSize2, enumPixelPaintSize3, enumPixelPaintSize4.
	typePlatform = 'Pltf',	// There is also a keyPlatform. enumOS2, enumWindows, enumMacintosh, enumIBMPC.
	typePreview = 'Prvw',	// enumNone, enumIcon, enumThumbnail, enumMacThumbnail, enumWinThumbnail, enumFullSize. There is also a keyPreview.
	typePreviewCMYK = 'Prvt',	// enumPreviewOff, enumPreviewCMYK, enumPreviewCyan, enumPreviewMagenta, enumPreviewYellow, enumPreviewBlack, enumPreviewCMY
	typeProfileMismatch = 'PrfM',	// enumIgnore, enumAskWhenOpening, enumConvertToCMYK, enumConvertToRGB, enumConvertToLab, enumConvertToGray, enumKeepProfile.
	typePurgeItem = 'PrgI',	// enumClipboard, enumSnapshot, enumUndo, enumPattern, enumHistory, enumAll.
	typeQuadCenterState = 'QCSt',	// enumQCSAverage, enumQCSCorner0, enumQCSCorner1, enumQCSCorner2, enumQCSCorner3, enumQCSSide0, enumQCSSide1, enumQCSSide2, enumQCSSide3, enumQCSIndependent.
	typeQuality = 'Qlty',	// enumLowQuality, enumMediumQuality, enumHighQuality, enumMaximumQuality. There is also a typeQuality.
	typeQueryState = 'QurS',	// enumQueryAlways, enumQueryAsk, enumQueryNever
	typeRGBSetupSource = 'RGBS',	// enumCustom, enumBuiltin, enumMonitor, enumFile.
	typeRawData = 'tdta',	// same as typeData
	typeRippleSize = 'RplS',	// There is also a keyRippleSize. enumSmall, enumMediumQuality, enumLarge.
	typeRulerUnits = 'RlrU',	// enumRulerPixels, enumRulerInches, enumRulerCm, enumRulerPoints, enumRulerPicas, enumRulerPercent.
	typeScreenType = 'ScrT',	// enumScreenCircle, enumScreenDot, enumScreenLine.
	typeShape = 'Shp ',	// enumRound, enumDiamond, enumEllipse, enumLine, enumSquare, enumCross.
	typeSmartBlurMode = 'SmBM',	// There is also a keySmartBlurMode. enumSmartBlurModeNormal, enumSmartBlurModeEdgeOnly, enumSmartBlurModeOverlayEdge.
	typeSmartBlurQuality = 'SmBQ',	// There is also a keySmartBlurQuality. enumSmartBlurQualityLow, enumSmartBlurQualityMedium, enumSmartBlurQualityHigh.
	typeSourceMode = 'Cndn',	// for ModeChange plugin
/////// DO NOT ADD ANYTHING HERE - SEE WARNING AT TOP OF FILE ///////////
	typeSpherizeMode = 'SphM',	// There is also a keySpherizeMode. enumNormal, enumHorizontalOnly, enumVerticalOnly.
	typeState = 'Stte',	// enumRedrawComplete.
	typeStringClassFormat = '#StC',	// Specifies multiple types.  Read Develop 21 for more info.
	typeStringChannel = '#sth',	// Specifies multiple types.  Read Develop 21 for more info.
	typeStringCompensation = '#Stm',	// Breaks hash for typeStringClassFormat.
	typeStringFSS = '#Stf',	// Specifies multiple types.  Read Develop 21 for more info.
	typeStringInteger = '#StI',	// Specifies multiple types.  Read Develop 21 for more info.
	typeStrokeDirection = 'StrD',	// enumStrokeDirRightDiag, enumStrokeDirHorizontal, enumStrokeDirLeftDiag, enumStrokeDirVertical.
	typeStrokeLocation = 'StrL',	// enumInside, enumOutside, enumCenter.
	typeTermText = typeChar,
	typeTextureType = 'TxtT',	// enumTexTypeBrick, enumTextTypeBurlap, enumTextTypeCanvas, enumTexTypeSandstone, enumTexTypeBlocks, enumTexTypeFrosted, enumTexTypeTinyLens.
	typeTransparencyGridColors = 'Trnl',	// enumLight, enumMedium, enumDark, enumRed, enumOrange, enumGreen, enumBlue, enumPurple, enumCustom
	typeTransparencyGridSize = 'TrnG',	// enumNone, enumSmall, enumMedium, enumLarge
	typeTypeClassModeOrClassMode = '#TyM',	// Specifies multiple types.  Read Develop 21 for more info.
	typeUndefinedArea = 'UndA',	// enumWrapAround, enumRepeatEdgePixels.
	typeUnitFloat = 'UntF',	// Special type for unit values.
	typeUrgency = 'Urgn',	// enumLow, enumHigh.
/////// DO NOT ADD ANYTHING HERE - SEE WARNING AT TOP OF FILE ///////////
	typeUserMaskOptions = 'UsrM',	// enumHideAll, enumRevealAll, enumHideSelection, enumRevealSelection.
	typeValueList = 'VlLs',
	typeVerticalLocation = 'VrtL',	// enumTop, enumBottom.
	typeWaveType = 'Wvtp',	// enumWaveSine, enumWaveTriangle, enumWaveSquare.
	typeWindMethod = 'WndM',	// enumWind, enumBlast, enumStagger.
	typeYesNo = 'YsN ',	// enumYes, enumNo, enumAsk.
	typeZigZagType = 'ZZTy',	// There is also a keyZigZagType. Breaks hash.
}; // enum PITypes

//-------------------------------------------------------------------------------
//	Units.
//-------------------------------------------------------------------------------

enum PIUnits
{
	unitAngle = '#Ang',	// Unit value - base degrees
	unitDensity = '#Rsl',	// Unit value - base per inch
	unitDistance = '#Rlt',	// Unit value - base 72ppi
	unitNone = '#Nne',	// Unit value - coerced.
	unitPercent = '#Prc',	// Tagged unit value.
	unitPixels = '#Pxl',	// Tagged unit value.
/////// DO NOT ADD ANYTHING HERE - SEE WARNING AT TOP OF FILE ///////////

	unitMillimeters = '#Mlm',	// Unit value used ONLY by text code
	unitPoints = '#Pnt',	// Unit value used ONLY by text code
}; // enum PIUnits
//-------------------------------------------------------------------------------

/////// DO NOT ADD ANYTHING HERE - SEE WARNING AT TOP OF FILE ///////////

#endif // __PITerminology_h__
