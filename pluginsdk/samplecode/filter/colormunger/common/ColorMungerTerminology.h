// ADOBE SYSTEMS INCORPORATED
// Copyright  1993 - 2002 Adobe Systems Incorporated
// All Rights Reserved
//
// NOTICE:  Adobe permits you to use, modify, and distribute this 
// file in accordance with the terms of the Adobe license agreement
// accompanying it.  If you have received this file from a source
// other than Adobe, then your use, modification, or distribution
// of it requires the prior written permission of Adobe.
//-------------------------------------------------------------------
//-------------------------------------------------------------------------------
//
//	File:
//		ColorMungerTerminology.h
//
//	Description:
//		This file contains the resource definitions
//		for the Filter module ColorMunger, a module
//		showing the use of the Color Services suite.
//
//	Use:
//		This module takes a source color of any color space
//		and converts it to a target color in any color
//		space.  It shows how to convert colors as well as
//		pop the color picker.  It appears in
//		Filters>>Utilities>>ColorMunger.
//
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
//	Definitions -- Scripting keys
//-------------------------------------------------------------------------------

#define typeColorName		'tyCN'
#define cnRGB				'cRGB'
#define cnHSB				'cHSB'
#define cnCMYK				'cmyK'
#define cnLab				'cLab'
#define cnGray				'graY'
#define cnHSL				'cHSL'
#define cnXYZ				'cXYZ'

//-------------------------------------------------------------------------------
//	Definitions -- Resources
//-------------------------------------------------------------------------------

#define kPromptSource		ResourceID+10
#define kPromptTarget		kPromptSource+1

//-------------------------------------------------------------------------------
// end ColorMungerTerminology.h
