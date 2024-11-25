// ADOBE SYSTEMS INCORPORATED
// Copyright  1993 - 2002 Adobe Systems Incorporated
// All Rights Reserved
//
// NOTICE:  Adobe permits you to use, modify, and distribute this 
// file in accordance with the terms of the Adobe license agreement
// accompanying it.  If you have received this file from a source
// other than Adobe, then your use, modification, or distribution
// of it requires the prior written permission of Adobe.
//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
//
//	File:
//		PoorMansTypeTool.h
//
//	Description:
//		This file contains the source and routines for the
//		Filter module Poor Man's Type Tool, a module that 
//		uses the Channel Ports Suite for pixel munging.
//
//	Use:
//		This is a basic module to exemplify all the typical
//		functions a filter module will do: Read scripting
//		parameters, show a user interface, display a proxy,
//		write changed pixel information, and then write
//		scripting parameters.
//
//-------------------------------------------------------------------------------

#ifndef __PoorMansTypeTool_H__		// Has this not been defined yet?
#define __PoorMansTypeTool_H__		// Only include this once by predefining it.

#include "PIFilter.h"						// Filter Photoshop header file.
#include "PIUtilities.h"					// SDK Utility library.
#include "PIChannelPortsSuite.h"			// Channel Ports Suite Photoshop header file.
#include "PIChannelPortOperations.h"		// Supported imaging operations on channel ports
namespace std {
	using ::size_t;
}
#include "PIBufferSuite.h"					// Buffer Suite Photoshop header file.
#include "PoorMansTypeToolTerminology.h"	// Terminology for this plug-in.
#if Macintosh
	#include <string.h>
#endif

//-------------------------------------------------------------------------------
//	Definitions -- Dialog items
//-------------------------------------------------------------------------------

enum
{
	kDOK = 1,
	kDCancel = 2,
	kDVerticalText = 3,
	kDVertical = 4,
	kDHorizontalText = 5,
	kDHorizontal = 6,
	kDSizeText = 7,
	kDSize = 8,
	kDGaussianBlurData = 9,
	kDAllLayers = 10,
	kDProxyItem = 11,
	kDWarningText = 12,
	kDDocPopUp = 13,
	kDLayersPopUp = 14
};

//-------------------------------------------------------------------------------
//	Definitions -- Constants
//-------------------------------------------------------------------------------

const int kGutter = 2;		// 2 pixel gutter
const int VSIZE = 8;
const int HSIZE = VSIZE * 11;
const char HelloWorld[VSIZE][HSIZE] = {	"X     X XXXXXXX X       X       XXXXXXX         X     X XXXXXXX XXXXXXX X       XXXXXX ",
										"X     X X       X       X       X     X         X  X  X X     X X     X X       X     X",
										"X     X X       X       X       X     X         X  X  X X     X X     X X       X     X",
										"XXXXXXX XXXXX   X       X       X     X         X  X  X X     X XXXXXXX X       X     X",
										"X     X X       X       X       X     X         X  X  X X     X X   X   X       X     X",
										"X     X X       X       X       X     X         X  X  X X     X X    X  X       X     X",
										"X     X X       X       X       X     X         X  X  X X     X X     X X       X     X",
										"X     X XXXXXXX XXXXXXX XXXXXXX XXXXXXX         XXXXXXX XXXXXXX X     X XXXXXXX XXXXXX " };

//-------------------------------------------------------------------------------
//	Globals -- structures
//-------------------------------------------------------------------------------

typedef struct TParameters
{ // This is our parameter structure that has an entry in the parameter block.

	Point					clickPoint;
	int32					xFactor;
	Boolean					gaussianBlurData;
	Boolean					queryForParameters;

} TParameters, *PParameters, **HParameters;

typedef struct Globals
{ // This is our structure that we use to pass globals between routines:

	short					*result;				// Must always be first in Globals.
	FilterRecord			*filterParamBlock;		// Must always be second in Globals.

	Rect					proxyRect;
	bool					viewAllLayerData;
	int16					viewLayerIndex;
	int16					viewDocIndex;
	ReadImageDocumentDesc	*docDesc;
	float					scaleFactor;
	PSChannelPortsSuite1	*sPSChannelPortsSuite;
	PSBufferSuite2			*sPSBufferSuite64;
	char					*channelData;
	char					*maskData;
	char					*selectionData;
	char					*overlayData;

} Globals, *GPtr;
	
//-------------------------------------------------------------------------------
//	Globals -- definitions and macros
//-------------------------------------------------------------------------------

#define gResult 				(*(globals->result))
#define gStuff  				(globals->filterParamBlock)
#define gProxyRect				(globals->proxyRect)
#define gViewAllLayerData 		(globals->viewAllLayerData)
#define gViewLayerIndex			(globals->viewLayerIndex)
#define gDocDesc				(globals->docDesc)
#define gScaleFactor			(globals->scaleFactor)
#define gPSChannelPortsSuite	(globals->sPSChannelPortsSuite)
#define gPSBufferSuite64		(globals->sPSBufferSuite64)
#define gChannelData			(globals->channelData)
#define gMaskData				(globals->maskData)
#define gSelectionData			(globals->selectionData)
#define gOverlayData			(globals->overlayData)

#define gParams 				((PParameters) *gStuff->parameters)
#define gPoint					(gParams->clickPoint)
#define gPointV					(gParams->clickPoint.v)
#define gPointH					(gParams->clickPoint.h)
#define gXFactor				(gParams->xFactor)
#define gGaussianBlurData		(gParams->gaussianBlurData)
#define gQueryForParameters		(gParams->queryForParameters)

#define gDocInfo				(gStuff->documentInfo)


//-------------------------------------------------------------------------------
//	Prototypes
//-------------------------------------------------------------------------------

OSErr ReadScriptParams (GPtr globals);		// Read any scripting params.
OSErr WriteScriptParams (GPtr globals);		// Write any scripting params.

void DoAbout (AboutRecordPtr about); 	    // Pop about box.
Boolean DoUI (GPtr globals);				// Show the UI.

void UpdateProxyView(GPtr globals);			// Read image data into local buffers for proxy view

// Rescall the proxy rectangle and calculate the scale factor to the original image
void CalcProxyScaleFactor (const VRect *filterRect, Rect *proxyRect, float *scaleFactor);

// release the memory held for the proxy item
void ReleaseProxyMemory(GPtr globals);

// only display the check box if we have more than one layer
short NumberOfLayers(GPtr globals);
//-------------------------------------------------------------------------------

#endif // __PoorMansTypeTool_H__

// end PoorMansTypeTool.h
