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
//		History.cpp
//
//	Copyright 1990-1992, Thomas Knoll.
//	All Rights Reserved.
//
//	Description:
//		This file contains the source routines and functions
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

//-------------------------------------------------------------------------------
//	Includes
//-------------------------------------------------------------------------------

#include "History.h"

//-------------------------------------------------------------------------------
//	Prototypes.
//-------------------------------------------------------------------------------

void InitGlobals (Ptr globalPtr);		  	// Initialize globals.
void ValidateParameters (GPtr globals);		// Validate parameters.
void DoPrepare (GPtr globals);
void DoStart (GPtr globals);				// Main routine.
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
//		ExportRecordPtr exportParamBlock			Host provides pointer to parameter
//													block containing pertinent data
//													and callbacks from the host.
//													See PIExport.h.
//
//	Outputs:
//		ExportRecordPtr exportParamBlock			Host provides pointer to parameter
//													block containing pertinent data
//													and callbacks from the host.
//													See PIExport.h.
//
//		intptr_t *data								Use this to store a pointer to our
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
						             ExportRecordPtr exportParamBlock,
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

	if (selector == exportSelectorAbout)
	{
		sSPBasic = ((AboutRecord*)exportParamBlock)->sSPBasic;
		DoAbout((AboutRecordPtr)exportParamBlock);
	}
	else
	{ // do the rest of the process as normal:

		sSPBasic = ((ExportRecordPtr)exportParamBlock)->sSPBasic;
		
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
									 exportParamBlock,
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
			case exportSelectorPrepare:
				DoPrepare(globals);
				break;
			case exportSelectorStart:
				DoStart(globals);
				break;
			case exportSelectorContinue:
				DoContinue(globals);
				break;
			case exportSelectorFinish:
				DoFinish(globals);
				break;
		}
				
	} // about selector special		

  } // try
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
//		gCurrentHistory		Default: 1.
//
//-------------------------------------------------------------------------------

void ValidateParameters (GPtr globals)
{
	gCurrentHistory = 1;
	gQueryForParameters = true;

} // end ValidateParameters

//-------------------------------------------------------------------------------
//
//	DoPrepare
//
//	Initialize parameters to default values.
//
//	Inputs:
//
//	Outputs:

/*****************************************************************************/

/* Prepare to export an image.	If the plug-in module needs only a limited
   amount of memory, it can lower the value of the 'maxData' field. */

void DoPrepare (GPtr globals)
{
	
	gStuff->maxData = 0;
	
}

/*****************************************************************************/

void GetHistory (GPtr globals, int16 index, Str255 s)
{	
	int16 currentResources = CountPIResources(histResource);
	
	s[ (s[0]=0) + 1] = 0;
	
	if (currentResources >= index)
	{
		Handle h = GetPIResource (histResource, index);	
		if (h != NULL) PIHandle2PString(h, s);
		// just a read-only reference.  Do NOT dispose.
	}
}

/*****************************************************************************/

/* Requests pointer to the first part of the image to be filtered. */

void DoStart (GPtr globals)
{

	if (!WarnResourceProcsAvailable ())
	{
		gResult = errPlugInHostInsufficient;
		return;
	}
		
	gQueryForParameters = ReadScriptParams (globals);
	
	if ( gQueryForParameters )
	{
		Boolean	doThis = DoUI (globals);
	
		if ( !doThis )
		{
			gResult = userCanceledErr;
			return;
		}
		
		// Process anything here.  This plug-in doesn't do anything
		// outside of its UI.  Perhaps later we'll have it save
		// the remaining pseudo-resources to a text file.
	}
}

/*****************************************************************************/

/* Filters the area and requests the next area. */

// Don't need this if using AdvanceState().

void DoContinue (GPtr /*globals*/)
{
}

/*****************************************************************************/

/* This routine will always be called if DoStart does not return an error
   (even if DoContinue returns an error or the user aborts the operation).
   This allows the module to perform any needed cleanup.  None is required
   in this example. */

void DoFinish (GPtr globals)
{
	WriteScriptParams(globals);
}

//-------------------------------------------------------------------------------

// end History.cpp
