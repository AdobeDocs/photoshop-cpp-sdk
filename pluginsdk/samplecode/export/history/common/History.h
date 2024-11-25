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
//		History.h
//
//	Copyright 1990-1992, Thomas Knoll.
//	All Rights Reserved.
//
//	Description:
//		This file contains the header prototypes and macros
//		for the Export module History, a module that
//		displays and lets the user manipulate
//		pseudo-resources of type 'hist'.
//
//	Use:
//		This module shows how to examine, display, and work
//		with pseudo-resources.  An additional element might
//		be to have it export those remaining pseudo-resources
//		to a text file.
//
//-------------------------------------------------------------------------------

#ifndef __History_H__				// Has this been defined yet?
#define __History_H__				// Only include once by predefining it.

#include "PIExport.h"				// Export Photoshop header file.
#include "PIUtilities.h"			// SDK Utility library.
#include "DialogUtilities.h"		// SDK Dialog utilities
#include "HistoryTerminology.h"		// Terminology specific to this plug-in.

//-------------------------------------------------------------------------------
//	Globals -- structures
//-------------------------------------------------------------------------------

typedef struct Globals
{ // This is our structure that we use to pass globals between routines:

	short				*result;				// Must always be first in Globals.
	ExportRecord		*exportParamBlock;		// Must always be second in Globals.

	Boolean				queryForParameters;		// Pop UI?
	short 				currentHistory;			// Showing history #

} Globals, *GPtr;
		
//-------------------------------------------------------------------------------
//	Globals -- definitions and macros
//-------------------------------------------------------------------------------

#define gResult 				(*(globals->result))
#define gStuff  				(globals->exportParamBlock)

#define gQueryForParameters		(globals->queryForParameters)
#define gCurrentHistory			(globals->currentHistory)

//-------------------------------------------------------------------------------
//	Prototypes
//-------------------------------------------------------------------------------

Boolean ReadScriptParams (GPtr globals);	// Read any scripting params.
OSErr WriteScriptParams (GPtr globals);		// Write any scripting params.

void DoAbout (AboutRecordPtr about); 	    // Pop about box.
Boolean DoUI (GPtr globals);				// Show the UI.

void GetHistory (GPtr globals, int16 index, Str255 string);

//-------------------------------------------------------------------------------

#endif // __History_H__
