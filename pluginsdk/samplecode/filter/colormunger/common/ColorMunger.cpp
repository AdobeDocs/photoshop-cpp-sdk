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
//		ColorMunger.cpp
//
//	Description:
//		This file contains the functions and source
//		Filter module ColorMunger, a module exemplifying
//		the use of the Color Services suite.
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
//	Includes 
//-------------------------------------------------------------------------------

#include "ColorMunger.h"

//-------------------------------------------------------------------------------
//	Prototypes.
//-------------------------------------------------------------------------------

void InitGlobals (Ptr globalPtr);		  	// Initialize globals.
void DoStart (GPtr globals);				// Main routine.

void ValidateParameters (GPtr globals);		// Validates (inits) parameters.
void DoParameters (GPtr globals);
void DoPrepare (GPtr globals);
void DoContinue (GPtr globals);
void DoFinish (GPtr globals);

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
//		const int16 selector						Host provides selector indicating
//													what command to do.
//
//		FilterRecord *filterParamBlock				Host provides pointer to parameter
//													block containing pertinent data
//													and callbacks from the host.
//													See PIFilter.h.
//
//	Outputs:
//		FilterRecord *filterParamBlock				Host provides pointer to parameter
//													block containing pertinent data
//													and callbacks from the host.
//													See PIFilter.h.
//
//		void *data									Use this to store a pointer to our
//													global parameters structure, which
//													is maintained by the host between
//													calls to the plug-in.
//
//		int16 *result								Return error result or noErr.  Some
//													errors are handled by the host, some
//													are silent, and some you must handle.
//													See PIGeneral.h.
//
//-------------------------------------------------------------------------------

DLLExport MACPASCAL void PluginMain (const int16 selector,
						             FilterRecordPtr filterParamBlock,
						             intptr_t *data,
						             int16 *result)
{
  try {
	//---------------------------------------------------------------------------
	//	(1) Check for about box request.
	//
	// 	The about box is a special request; the parameter block is not filled
	// 	out, none of the callbacks or standard data is available.  Instead,
	// 	the parameter block points to an AboutRecord, which is used
	// 	on Windows.
	//---------------------------------------------------------------------------

	if (selector == filterSelectorAbout)
	{
		sSPBasic = ((AboutRecordPtr)filterParamBlock)->sSPBasic;
		DoAbout((AboutRecordPtr)filterParamBlock);
	}
	else
	{ // do the rest of the process as normal:

		sSPBasic = filterParamBlock->sSPBasic;

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
									 filterParamBlock,
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
		//	(3) Dispatch selector.
		//-----------------------------------------------------------------------

		switch (selector)
		{
			case filterSelectorParameters:
				DoParameters(globals);
				break;
			case filterSelectorPrepare:
				DoPrepare(globals);
				break;
			case filterSelectorStart:
				DoStart(globals);
				break;
			case filterSelectorContinue:
				DoContinue(globals);
				break;
			case filterSelectorFinish:
				DoFinish(globals);
				break;
		}
	
	} // about selector special		
	
  } // end try
  catch(...)
  {
	  if (result != NULL)
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
	ValidateParameters (globals);
	
} // end InitGlobals

//-------------------------------------------------------------------------------
//
//	ValidateParameters
//
//	Initialize parameters to default values.
//
//	Inputs:
//		GPtr globals		Pointer to global structure.
//
//	Outputs:
//		gSourceColor		Default: 0, 0, 0, 255.
//
//		gTargetColor		Default: 0, 0, 0, 255.
//
//		gColor				Default: 0.
//
//		gColorize			Default: 0.
//
//-------------------------------------------------------------------------------

void ValidateParameters (GPtr globals)
{
	if (gStuff->parameters == NULL)
	{ // We haven't created these yet.

		gStuff->parameters = PINewHandle ((long) sizeof (TParameters));

		if (gStuff->parameters != NULL)
		{ // Got it.  Fill out the fields.
			CSSetColor (gSourceColor, 0, 0, 0, 255);
			CSCopyColor (gTargetColor, gSourceColor);

			gColor = gColorize = 0;

		}
		else
		{ // Oops.  Couldn't allocate memory.
				
			gResult = memFullErr;
			return;
		}
	} // parameters
	
} // end ValidateParameters

//-------------------------------------------------------------------------------
//
//	DoParameters
//
//	Initialize parameters to default values.
//
//	Inputs:
//
//	Outputs:

/* Asks the user for the plug-in filter module's parameters. Note that
   the image size information is not yet defined at this point. Also, do
   not assume that the calling program will call this routine every time the
   filter is run (it may save the data held by the parameters handle in
   a macro file). Initialize any single-time info here. */

void DoParameters (GPtr globals)

{
	ValidateParameters (globals);
	
	gQueryForParameters = TRUE;
	/* If we're here, that means we're being called for the first time. */
}

/*****************************************************************************/

/* Prepare to filter an image.	If the plug-in filter needs a large amount
   of buffer memory, this routine should set the bufferSpace field to the
   number of bytes required. Also check for */

void DoPrepare (GPtr globals)
{	
	gStuff->bufferSpace = 0;
	CSSetColor (gSourceColor, 0, 255, 0, 0);
}

/*****************************************************************************/

/* Does all of the filtering. */

void DoStart (GPtr globals)
	{
	
	if (!WarnAdvanceStateAvailable ())
	{
		gResult = userCanceledErr; // exit gracefully
		goto done;
	}

	if (!WarnColorServicesAvailable())
	{
		gResult = userCanceledErr; // exit gracefully
		goto done;
	}
	
	ValidateParameters (globals);
	/* if stuff hasn't been initialized that we need, do it,
		then go check if we've got scripting commands to
		override our settings */
	gQueryForParameters = ReadScriptParams (globals); // update our parameters with the scripting parameters, if available

	if (gQueryForParameters)
	{
		gQueryForParameters = FALSE;
		if (!DoUI (globals))
			goto done; // canceled
	}

	if (gResult != noErr)
		goto done;

	done:
	
	// if one screws with the outData with a proxy, reset that here
		
	PISetRect (&gStuff->inRect, 0, 0, 0, 0);
	PISetRect (&gStuff->outRect, 0, 0, 0, 0);
	PISetRect (&gStuff->maskRect, 0, 0, 0, 0);

	}

/*****************************************************************************/

/* Call routines to filter the area. */

void DoFilterRect (GPtr globals)
{

	// do actual filtering here

	PISetRect (&gStuff->inRect, 0, 0, 0, 0);
	PISetRect (&gStuff->outRect, 0, 0, 0, 0);
	PISetRect (&gStuff->maskRect, 0, 0, 0, 0);
	
}

/*****************************************************************************/

/* Given that we do all of the filtering during the start phase, the continue
   phase is negligible. */

void DoContinue (GPtr globals)
	{
	
	/* Force these to zero.  We never should actually get here. */

	PISetRect (&gStuff->inRect, 0, 0, 0, 0);
	PISetRect (&gStuff->outRect, 0, 0, 0, 0);
	PISetRect (&gStuff->maskRect, 0, 0, 0, 0);
	
	}

/*****************************************************************************/

/* Do any necessary clean-up. */

void DoFinish (GPtr globals)
{	
	WriteScriptParams (globals); // writes our parameters to the scripting system
}

/*******************************************************************/
void PopulateColorServicesInfo (GPtr globals, ColorServicesInfo *csinfo)
{
	CSInitInfo(csinfo);
	// now populate color services info with particulars:
	csinfo->sourceSpace = gColor;
	csinfo->resultSpace = gColorize;
	CSCopyColor(csinfo->colorComponents, gSourceColor);
	// on input, source color; on output, result color
}

//-------------------------------------------------------------------------------

// end ColorMunger.cpp
