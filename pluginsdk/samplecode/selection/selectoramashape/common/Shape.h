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
//		Shape.h
//
//	Description:
//		This file contains the header prototypes and macros for the
//		Import module Idle, a very simple example of a module
//		that just idles and sends update events.
//
//	Use:
//		This import module is good as an example of a module
//		that forces the background application to redraw
//		its windows.
//
//-------------------------------------------------------------------------------

#ifndef __Shape_H__				// Has this not been defined yet?
#define __Shape_H__				// Only include this once by predefining it

#include "PISelection.h"		// Selection Photoshop header file
#include "PIUtilities.h"		// SDK Utility library
#include "ShapeTerminology.h"	// Terminology for this plug-in.

//-------------------------------------------------------------------------------
//	Definitions -- Enumerations
//-------------------------------------------------------------------------------

enum
{
	iShapeTriangle,
	iShapeSquare,
	iShapeCircle,
	iShapeStar,
	iShapeTreble,
	iShapeRibbon,
	iShapeNote
};

enum
{
	iCreateSelection,
	iCreateMaskpath,
	iCreateLayer
};

//-------------------------------------------------------------------------------
//	Globals -- structures
//-------------------------------------------------------------------------------

typedef struct Globals
{ // This is our structure that we use to pass globals between routines:

	short				*result;				// Must always be first in Globals.
	PISelectionParams	*selectionParamBlock;	// Must always be second in Globals.

	Boolean	queryForParameters;
	short	whatShape;
	short	create;

} Globals, *GPtr;
	
//-------------------------------------------------------------------------------
//	Globals -- definitions and macros
//-------------------------------------------------------------------------------

#define gResult				(*(globals->result))
#define gStuff  			(globals->selectionParamBlock)
#define gQueryForParameters (globals->queryForParameters)

#define gWhatShape 			(globals->whatShape)
#define gCreate 			(globals->create)

//-------------------------------------------------------------------------------
//	Prototypes
//-------------------------------------------------------------------------------

void InitGlobalsShape (Ptr globalPtr);		  	// Initialize globals.
void DoAbout (AboutRecordPtr about); 	    // Pop about box.
void DoExecuteShape (GPtr globals);				// Main routine.

void ValidateParameters (GPtr globals);		// Validates (inits) parameters.
Boolean DoUIShape (GPtr globals);				// Show the UI.

Boolean ReadScriptParamsShape (GPtr globals);	// Read any scripting params.
OSErr WriteScriptParamsShape (GPtr globals);		// Write any scripting params.

short KeyToEnumShape (OSType key, OSType whatType);
OSType EnumToKeyShape (short keyEnum, OSType whatType);

Handle GetPathHandle(HandleProcs * procs, short inTreatment);

//-------------------------------------------------------------------------------

#endif // __Shape_H__


