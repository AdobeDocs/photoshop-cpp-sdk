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
//		ColorMunger.h
//
//	Description:
//		This file contains the header prototypes and macros
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

#ifndef __ColorMunger_H__		// Has this been defined yet?
#define __ColorMunger_H__		// Only include this once by predefining it.

#include "PIFilter.h"				// Filter Photoshop header file.
#include "PIUtilities.h"			// SDK Utility 
#include "DialogUtilities.h"		// SDK Dialog utilities
#include "ColorMungerTerminology.h"	// Terminology for this plug-in.

//-------------------------------------------------------------------------------
//	Globals -- structures
//-------------------------------------------------------------------------------

typedef struct TParameters
{ // This is our parameter structure that has an entry in the parameter block.

	int16	sourceColor[4]; 		// source color space
	short	queryForParameters;		// show UI?
	short	color; 					// source color
	short	colorize; 				// target color
	int16	targetColor[4]; 		// target color space

} TParameters, *PParameters, **HParameters;

typedef struct Globals
{ // This is our structure that we use to pass globals between routines:

	short				*result;				// Must always be first in Globals.
	FilterRecord		*filterParamBlock;		// Must always be second in Globals.

} Globals, *GPtr;

//-------------------------------------------------------------------------------
//	Globals -- definitions and macros
//-------------------------------------------------------------------------------

#define gResult 			(*(globals->result))
#define gStuff  			(globals->filterParamBlock)

#define gParams				((PParameters) *gStuff->parameters)
#define gQueryForParameters (gParams->queryForParameters)
#define gColor				(gParams->color)
#define gSourceColor		(gParams->sourceColor)
#define gColorize			(gParams->colorize)
#define gTargetColor		(gParams->targetColor)

//-------------------------------------------------------------------------------
//	Prototypes
//-------------------------------------------------------------------------------

Boolean ReadScriptParams (GPtr globals);	// Read any scripting params.
OSErr WriteScriptParams (GPtr globals);		// Write any scripting params.

void DoAbout (AboutRecordPtr about); 	    // Pop about box.
Boolean DoUI (GPtr globals);				// Show the UI.

void DoFilterRect (GPtr globals);
void ValidateParameters (GPtr globals);
Boolean ReadScriptParams (GPtr globals);
OSErr WriteScriptParams (GPtr globals);
void SetColor (void *color, int16 color1, int16 color2,
			   int16 color3, int16 color4);
void CopyColor (void *color1, void *color2);
void PopulateColorServicesInfo (GPtr globals, ColorServicesInfo *csinfo);
void AssignArray (void *array,
					DescriptorKeyID key1,
					DescriptorKeyID key2, 
					DescriptorKeyID key3, 
					DescriptorKeyID key4 );
DescriptorKeyID Enum2Type (DescriptorClassID thisClass, int16 thisEnum);
int16 Type2Enum (DescriptorClassID thisClass, DescriptorKeyID thisType);

//-------------------------------------------------------------------------------

#endif // __ColorMunger_H__


