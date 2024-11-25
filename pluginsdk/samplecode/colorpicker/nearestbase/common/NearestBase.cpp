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
//		NearestBase.cpp
//
//	Description:
//		This file contains the source and functions
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
//
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
//	Includes
//-------------------------------------------------------------------------------

#include "NearestBase.h"

//-------------------------------------------------------------------------------
//	Globals -- Define global variables for plug-in scope.
//-------------------------------------------------------------------------------
SPBasicSuite * sSPBasic = NULL;

//-------------------------------------------------------------------------------
//
//	PluginMain / main
//
//	All calls to the plug-in module come through this routine.
//	It must be placed first in the resource.  To achieve this,
//	most development systems require this be the first routine
//	in the source.
//
//	The entrypoint will be "pascal void" for Macintosh,
//	"void" for Windows.
//
//	Inputs:
//		const int16 selector				Host provides selector indicating
//											what command to do.
//
//		PIPickerParams *pickerParamBlock	Host provides pointer to parameter
//											block containing pertinent data
//											and callbacks from the host.
//											See PIPicker.h.
//
//	Outputs:
//		PIPickerParams * pickerParamBlock	Host provides pointer to parameter
//											block. Use it to pass pertinent
//											data to and from the host.
//											See PIPicker.h.
//
//		intptr_t *data						Use this to store a pointer to our
//											global parameters structure, which
//											is maintained by the host between
//											calls to the plug-in.
//
//		int16 *result						Return error result or noErr.  Some
//											errors are handled by the host, some
//											are silent, and some you must handle.
//											See PIGeneral.h.
//
//-------------------------------------------------------------------------------
DLLExport MACPASCAL void PluginMain (const int16 selector,
						             PIPickerParams * pickerParamBlock,
						             intptr_t * data,
						             int16 * result)
{

	try { 

	//---------------------------------------------------------------------------
	//	(1) Check for about box request.
	//
	// 	The about box is a special request; the parameter block is not filled
	// 	out, none of the callbacks or standard data is available.  Instead,
	// 	the parameter block points to an AboutRecord.
	//---------------------------------------------------------------------------

	if (selector == pickerSelectorAbout)
	{
		sSPBasic = ((AboutRecord*)pickerParamBlock)->sSPBasic;
		DoAbout((AboutRecordPtr)pickerParamBlock);
	}
	else
	{ // do the rest of the process as normal:

		sSPBasic = ((PIPickerParams*)pickerParamBlock)->sSPBasic;

		Ptr globalPtr = NULL;		// Pointer for global structure
		GPtr globals = NULL; 		// actual globals

		//-----------------------------------------------------------------------
		//	(2) Allocate and initialize globals.
		//
		// 	AllocateGlobals requires the pointer to result, the pointer to the
		// 	parameter block, the size of our local
		// 	"Globals" structure, a pointer to the long *data, a Function
		// 	Proc (FProcP) to the InitGlobals routine.  It automatically sets-up,
		// 	initializes the globals (if necessary), results result to 0, and
		// 	returns with a valid pointer to the locked globals handle or NULL.
		//-----------------------------------------------------------------------
		
		globalPtr = AllocateGlobals (result,
									 pickerParamBlock,
									 sizeof(Globals),
						 			 data,
						 			 InitGlobals);
		
		if (globalPtr == NULL)
		{ // Something bad happened if we could not allocate our pointer.
		  // Fortunately, everything has already been cleaned up,
		  // so all we have to do is report an error.
		  
		  *result = memFullErr;
		  return;
		}
		
		// Get our "globals" variable assigned as a Global Pointer struct with the
		// data we've returned:
		globals = (GPtr)globalPtr;

		//-----------------------------------------------------------------------
		//	(3) Run the plug-in main routine.
		//
		//-----------------------------------------------------------------------	

		DoPick(globals);
					
	} // about selector special		
	
	} // end try

	catch (...)
	{
		if (NULL != result)
			*result = -1;
	}

} // end PluginMain

//-------------------------------------------------------------------------------
//
//	InitGlobals
//	
//	Initialize any global values here.  Called only once when global
//	space is reserved for the first time.
//
//	Inputs:
//		Ptr globalPtr		Standard pointer to a global structure.
//
//	Outputs:
//		Initializes any global values with their defaults.
//
//-------------------------------------------------------------------------------

void InitGlobals (Ptr globalPtr)
{	
	// create "globals" as a our struct global pointer so that any
	// macros work:
	GPtr globals = (GPtr)globalPtr;
	
	// Initialize global variables:
	gCount = 0;
	
} // end InitGlobals

//-------------------------------------------------------------------------------
//
//	DoPick
//
//	Main routine to cycle through colors.  Returns next base color in
//	CMYK (which is automatically converted to the user's chosen color
//  space)
//
//	Inputs:
//		GPtr globals		Pointer to global structure
//
//	Outputs:
//		Changes
//			gStuff->pickParms.colorComponents[0] (C: 0...255 = 100%...0%)
//			gStuff->pickParms.colorComponents[1] (M: 0...255 = 100%...0%)
//			gStuff->pickParms.colorComponents[2] (Y: 0...255 = 100%...0%)
//			gStuff->pickParms.colorComponents[3] (K: 0...255 = 100%...0%)
//		to the next base color in CMYK.
//
//		gResult				Returns noErr if completed without error.
//
//-------------------------------------------------------------------------------

void DoPick (GPtr globals)
{

	// Set result space to CMYK (see PIGeneral.h for valid spaces):
	gStuff->pickParms.resultSpace = plugIncolorServicesCMYKSpace;

	switch (gCount % 4)
	{ // cycle through four cases:
	
		case 0: // red
			gStuff->pickParms.colorComponents[0] = 255 * 0x101;
			gStuff->pickParms.colorComponents[1] = 0 * 0x101;
			gStuff->pickParms.colorComponents[2] = 0 * 0x101;
			gStuff->pickParms.colorComponents[3] = 255 * 0x101;
			break; // 0
		
		case 1: // green
			gStuff->pickParms.colorComponents[0] = 0 * 0x101;
			gStuff->pickParms.colorComponents[1] = 255 * 0x101;
			gStuff->pickParms.colorComponents[2] = 0 * 0x101;
			gStuff->pickParms.colorComponents[3] = 255 * 0x101;
			break; // 1
		
		case 2: // blue
			gStuff->pickParms.colorComponents[0] = 0 * 0x101;
			gStuff->pickParms.colorComponents[1] = 0 * 0x101;
			gStuff->pickParms.colorComponents[2] = 255 * 0x101;
			gStuff->pickParms.colorComponents[3] = 255 * 0x101;
			break; // 2
			
		case 3: // black
			gStuff->pickParms.colorComponents[0] = 0 * 0x101;
			gStuff->pickParms.colorComponents[1] = 0 * 0x101;
			gStuff->pickParms.colorComponents[2] = 0 * 0x101;
			gStuff->pickParms.colorComponents[3] = 0 * 0x101;
			break;
			
		default: // unsupported, "exercise" error message
			gResult = pickerBadParameters;
			break;

	} // switch gCount
	
	gCount++;
	
} // end DoPick

//-------------------------------------------------------------------------------

// end NearestBase.cpp
