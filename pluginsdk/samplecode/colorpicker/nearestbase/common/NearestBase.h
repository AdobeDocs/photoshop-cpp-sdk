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
//		NearestBase.h
//
//	Description:
//		This file contains the prototypes and utilities
//		for the Color Picker example module NearestBase.
//
//	Use:
//		NearestBase provides another color picker (available
//		from File: Preferences: General) that cycles through
//		four primary colors.  Once selected, clicking any
//		location that normally pops a picker (such as the
//		foreground or background color) will automatically
//		cycle to the next primary color.
//
//-------------------------------------------------------------------------------

#ifndef __NearestBase_H__	// Has this not been defined yet?
#define __NearestBase_H__	// Only include this once by predefining it

#include "PIPicker.h"		// ColorPicker Photoshop header file
#include "PIUtilities.h"	// SDK Utility library

//-------------------------------------------------------------------------------
//	Globals
//-------------------------------------------------------------------------------

typedef struct Globals
{ // This is our structure that we use to pass globals between routines:

	short			*result;			// Must always be first in Globals.
	PIPickerParams	*pickerParamBlock;	// Must always be second in Globals.

	short			count;				// current color

} Globals, *GPtr;				// *GPtr = global pointer; **GHdl = global handle

// These macros are defined to give easier access to the global variables:
#define gResult	(*(globals->result))
#define gStuff  (globals->pickerParamBlock)
#define gCount	(globals->count)

//-------------------------------------------------------------------------------
//	Prototypes
//-------------------------------------------------------------------------------

void InitGlobals (Ptr globalPtr);		  	  // initialize globals
void DoAbout (AboutRecordPtr about); 	      // pop about box
void DoPick (GPtr globals);					  // Main routine
Boolean DoUI (GPtr globals);		  		  // pop UI
Boolean ReadScriptParams (GPtr globals);	  // read any scripting params
OSErr WriteScriptParams (GPtr globals);		  // write any scripting params

//-------------------------------------------------------------------------------

#endif // __NearestBase_H__
