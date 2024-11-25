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
//		Outbound.cpp
//
//	Copyright 1990-1992, Thomas Knoll.
//	All Rights Reserved.
//
//	Description:
//		This file contains the functions and source
//		for the Export module Outbound, a module that
//		creates a file and stores raw pixel data in it.
//
//	Use:
//		This module shows how to export raw data to a file.
//		It uses a simple "FileUtilities" library that comes
//		with the SDK.  You use it via File>>Export>>Outbound.
//
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
//	Includes
//-------------------------------------------------------------------------------

#include "Outbound.h"

//-------------------------------------------------------------------------------
//	Prototypes.
//-------------------------------------------------------------------------------

void InitGlobals (Ptr globalPtr);		  	// Initialize globals.
void ValidateParameters (GPtr globals);		// Validate parameters.
void DoPrepare (GPtr globals);
void DoStart (GPtr globals);				// Main routine.
void DoContinue (GPtr globals);
void DoFinish (GPtr globals);
void DoInitialRect (GPtr globals);
Boolean DoNextRect (GPtr globals);
void DoExportRect (GPtr globals);

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
#if __PIMac__
				UnLoadRuntimeFunctions();
#endif
				break;
		}
							
	} // about selector special
	  
  } // end try
  catch(...)
  {
#if __PIMac__
	  UnLoadRuntimeFunctions();
#endif
	  if (result != NULL)
		  *result = 1;
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

	gQueryForParameters = true;
	gAliasHandle = NULL; // no handle, yet
#if __PIMac__
	gBookMarkData = NULL; // no bookmark, yet
#endif

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
	
	if (gStuff->maxData > 0x80000)
		gStuff->maxData = 0x80000;
	
}

/*****************************************************************************/

/* Requests pointer to the first part of the image to be filtered. */

void DoStart (GPtr globals)
{
	
	/* This plug-in does not support bitmap images */

	if (gStuff->imageMode == plugInModeBitmap)
	{
		gResult = exportBadMode;
		return;
	}
	
	ValidateParameters(globals);
	
	/* check with the scripting system whether to pop our dialog */
	
	gQueryForParameters = ReadScriptParams (globals);
	
	if (!DoUI (globals))
		return;
		
	if (!CreateExportFile (globals))
		return;
	
	WriteExportFile (globals);
	
	CloseExportFile (globals);

	MarkExportFinished (gStuff);

}

/*****************************************************************************/

/* Filters the area and requests the next area. */

void DoContinue (GPtr globals)
{
	
	/* We shouldn't get here because we did all of the work during the
	   start phase, but we add some code just in case. */
	
	MarkExportFinished (gStuff);
	
	gResult = userCanceledErr;

}

/*****************************************************************************/

/* This routine will always be called if DoStart does not return an error
   (even if DoContinue returns an error or the user aborts the operation).
   This allows the module to perform any needed cleanup.  None is required
   in this example. */

void DoFinish (GPtr globals)
{
	WriteScriptParams (globals);
}

/*****************************************************************************/

#define RANGE_ITER(lower,upper,first,last,step)								  \
	for (lower = (first);                         							  \
		 (upper = (((lower) + (short)(step) < (last)) ? (lower) + (short)(step) : (last))), \
		 lower < (last);													  \
		 lower = upper)

/*****************************************************************************/

Boolean WriteExportFile (GPtr globals)
{
	/* We write out the file as an interleaved raw file. */ 
	
	/* We need to figure out how many rows to write at one time. */
	
	long chunk = gStuff->maxData / gStuff->imageSize.h / gStuff->planes;
	
	ExportRegion region;
	
	region.rect.left = 0;
	region.rect.right = gStuff->imageSize.h;
	region.loPlane = 0;
	region.hiPlane = gStuff->planes - 1;
	
	RANGE_ITER (region.rect.top, region.rect.bottom,
				0, gStuff->imageSize.v, chunk)
		{
		
		int16 row;
		int32 rowCount = gStuff->imageSize.h * gStuff->planes;
		void *data = 0;
		int32 rowBytes = 0;
		unsigned8 *rowData;
		
		if (!TSC (TestAbort ())) return FALSE;
			
		if (!TSR (FetchData (gStuff, &region, &data, &rowBytes))) return FALSE;
		
		for (row = region.rect.top, rowData = (unsigned8 *) data;
			 row < region.rect.bottom;
			 ++row, rowData += rowBytes)
			{
			
			int32 count = rowCount;
			
			PIUpdateProgress (row, gStuff->imageSize.v);
			
			if (!TSC (TestAbort ())) return FALSE;
#if __PIMac__
			if (!TSR (PSSDKWrite (gFRefNum, &count, rowData))) return FALSE;
#else
			if (!TSR (PSSDKWrite (gFRefNum, 0, 0, &count, rowData))) return FALSE;
#endif			
			}
		
		}
		
	return TRUE;
	
}

/*****************************************************************************/

OSErr FetchData (ExportRecord *stuff,
				 ExportRegion *region,
				 void **data,
				 int32 *rowBytes)
{
	
	OSErr result;
	
	if (!WarnHostAdvanceStateAvailable (stuff->advanceState))
		return userCanceledErr;
		
	stuff->theRect = region->rect;
	stuff->loPlane = region->loPlane;
	stuff->hiPlane = region->hiPlane;
	
	result = (*(stuff->advanceState)) ();
	
	if (result != noErr)
		{
		*data = NULL;
		*rowBytes = 0;
		}
	else
		{
		*data = stuff->data;
		*rowBytes = stuff->rowBytes;
		}
		
	return result;
	
}

/*****************************************************************************/

void MarkExportFinished (ExportRecord *stuff)
	{
	
	PISetRect (&stuff->theRect, 0, 0, 0, 0);
	
	}

//-------------------------------------------------------------------------------

// end Outbound.cpp
