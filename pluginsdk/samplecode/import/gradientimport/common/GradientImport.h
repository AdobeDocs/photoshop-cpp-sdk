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
//		GradientImport.h
//
//	Copyright 1990, Thomas Knoll.
//	All Rights Reserved.
//
//	Description:
//		This file contains the macros and definitions for the
//		Import module GradientImport, a scriptable multiple-acquire
//		plug-in that creates 4 types of gradients in batches.
//
//	Use:
//		This import module is a great example of scripting
//		Import modules that use the old style Photoshop
//		multiple-acquire mechanism.
//
//-------------------------------------------------------------------------------

#ifndef __GradientImport_H__			// Has this not been defined yet?
#define __GradientImport_H__			// Only include this once by predefining it

#include "PIAcquire.h"					// Import Photoshop header file.
#include "PIUtilities.h"				// SDK Utility library.
#include "DialogUtilities.h"		// SDK Dialog utilities
#include "GradientImportTerminology.h"	// Terminology for plug-in.

//-------------------------------------------------------------------------------
//	Definitions -- Constants
//-------------------------------------------------------------------------------

#define kRowsMin			1
#define kRowsMax			30000
#define kColumnsMin			1
#define kColumnsMax			30000

#define errHowdWeGetHere	16989

#define kMinDescriptors		1
#define kMaxDescriptors		50

//-------------------------------------------------------------------------------
//	Definitions -- Structures
//-------------------------------------------------------------------------------

// The region of the image being acquired (an area and a range of planes):

typedef struct AcquireRegion
{
	Rect rect;
	int16 loPlane;
	int16 hiPlane;

} AcquireRegion;

// The layout of the data:

typedef struct AcquireDataLayout
{
	int32 rowBytes;
	int32 colBytes;
	int32 planeBytes;

} AcquireDataLayout;
	
//-------------------------------------------------------------------------------
//	Globals -- structures
//-------------------------------------------------------------------------------

typedef struct Globals
{ // This is our structure that we use to pass globals between routines:

	short				*result;				// Must always be first in Globals.
	AcquireRecord		*acquireParamBlock;		// Must always be second in Globals.

	#ifdef __PIMac__

		void* dialog;
		void* dialogHdl;

	#elif defined(__PIWin__)

		HWND	dialog;

	#endif
	
	short 		lastImages;
	short 		lastRows;
	short 		lastCols;
	short 		lastMode;
	Boolean 	lastInvert;
	Boolean		convertAlpha;
	Boolean 	queryForParameters;
	Boolean 	initParameters;
	Boolean 	continueImport;
	
	short 		nextImage;
	
	/* scripting info */

	uint32				count;
	
	PIReadDescriptor	token;
	PIDescriptorHandle	array [kMaxDescriptors];
	
} Globals, *GPtr;

//-------------------------------------------------------------------------------
//	Globals -- definitions and macros
//-------------------------------------------------------------------------------

#define gResult (*(globals->result))
#define gStuff  (globals->acquireParamBlock)

#define gDialog (globals->dialog)
#define gDialogHdl (globals->dialogHdl)

#define gLastImages (globals->lastImages)
#define gLastRows (globals->lastRows)
#define gLastCols (globals->lastCols)
#define gLastMode (globals->lastMode)
#define gLastInvert (globals->lastInvert)

#define gConvertAlpha (globals->convertAlpha)

#define gNextImage (globals->nextImage)

#define gQueryForParameters (globals->queryForParameters)
#define gInitParameters (globals->initParameters)
#define gContinueImport (globals->continueImport)

#define gCount (globals->count)
#define gArray (globals->array)
#define gToken (globals->token)

//-------------------------------------------------------------------------------
//	Prototypes
//-------------------------------------------------------------------------------

void InitGlobals (Ptr globalPtr);		  	// Initialize globals.
void DoAbout (AboutRecordPtr about); 	    // Pop about box.

void ValidateParameters (GPtr globals);		// Validates (inits) parameters.

void CreateDescriptor (GPtr globals);
void OpenScriptParams (GPtr globals);
void ReadScriptParams (GPtr globals);
void CloseScriptParams (GPtr globals);
OSErr WriteScriptParams (GPtr globals);		// Write any scripting params.
void SwitchScriptInfo (GPtr globals, PIReadDescriptor readToken);
OSErr CheckAndWriteScriptParams (GPtr globals);

PIType GetGradientMode (short plugInMode);
short GetPlugInMode (PIType gradientMode);

Boolean OpenOurDialog (GPtr globals);
Boolean RunOurDialog (GPtr globals);
void CloseOurDialog (GPtr globals);


OSErr StoreData (AcquireRecord *stuff,
				 void *data,
				 AcquireRegion *region,
				 AcquireDataLayout *layout);

OSErr FetchData (AcquireRecord *stuff,
				 void *data,
				 AcquireRegion *region,
				 AcquireDataLayout *layout);

//-------------------------------------------------------------------------------

#endif // __GradientImport_H__
