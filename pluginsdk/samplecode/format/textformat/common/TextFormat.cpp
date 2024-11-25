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
//		TextFormat.h
//
//	Description:
//
//	Use:
//
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
//	Includes
//-------------------------------------------------------------------------------

#include "PIDefines.h"
#include "PIFormat.h"					// Format Photoshop header file.
#include "PIUtilities.h"				// SDK Utility library.
#include "FileUtilities.h"				// File Utility library.
#include "PIUI.h"

//-------------------------------------------------------------------------------
//	Globals
//-------------------------------------------------------------------------------

// everyone needs access to the sPSBasic pointer 
SPBasicSuite * sSPBasic = NULL;

DLLExport MACPASCAL void PluginMain (const int16 selector, FormatRecordPtr formatParamBlock, intptr_t * data, int16 * result);

void DoReadStart(FormatRecordPtr formatRecord);
void DoReadFinish(FormatRecordPtr formatRecord);

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
//		FormatRecord * formatParamBlock				Host provides pointer to parameter
//													block containing pertinent data
//													and callbacks from the host.
//													See PIFormat.h.
//
//	Outputs:
//		FormatRecord * formatParamBlock				Host provides pointer to parameter
//													block containing pertinent data
//													and callbacks from the host.
//													See PIFormat.h.
//
//		void * data									Use this to store a pointer to our
//													global parameters structure, which
//													is maintained by the host between
//													calls to the plug-in.
//
//		int16 * result								Return error result or noErr.  Some
//													errors are handled by the host, some
//													are silent, and some you must handle.
//													See PIGeneral.h.
//
//-------------------------------------------------------------------------------

DLLExport MACPASCAL void PluginMain (const int16 selector,
						             FormatRecordPtr formatParamBlock,
						             intptr_t * data,
						             int16 * result)
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
	if (selector == formatSelectorAbout)
	{
		AboutRecordPtr aboutRecord = reinterpret_cast<AboutRecordPtr>(formatParamBlock);
		sSPBasic = aboutRecord->sSPBasic;
		SPPluginRef sPluginRef = reinterpret_cast<SPPluginRef>(aboutRecord->plugInRef);
		DoAbout(sPluginRef, AboutID);
	}
	else
	{ // do the rest of the process as normal:

		sSPBasic = formatParamBlock->sSPBasic;


		//-----------------------------------------------------------------------
		//	Dispatch selector.
		//-----------------------------------------------------------------------
		switch (selector)
		{
			case formatSelectorReadStart:
				DoReadStart(formatParamBlock);
				break;
			case formatSelectorReadFinish:
				DoReadFinish(formatParamBlock);
				break;
			default:
				break;
		}
			

	} // about selector special		

	// release any suites that we may have acquired
	if (selector == formatSelectorAbout ||
		selector == formatSelectorWriteFinish ||
		selector == formatSelectorReadFinish ||
		selector == formatSelectorOptionsFinish ||
		selector == formatSelectorEstimateFinish ||
		selector == formatSelectorFilterFile)
	{
		PIUSuitesRelease();
	}

	} // end try

	catch(...)
	{
		if (NULL != result)
		{
			*result = -1;
		}
	}

} // end PluginMain


void DoReadStart(FormatRecordPtr formatRecord)
{
	formatRecord->planes = 3;
	formatRecord->imageMode = plugInModeRGBColor;
	formatRecord->depth = 8;

	if (formatRecord->openForPreview)
	{
		formatRecord->imageSize.v = 256;
		formatRecord->imageSize.h = 256;
	}
	else
	{	
		formatRecord->imageSize.v = 10 * 72;
		formatRecord->imageSize.h = 8 * 72;
	}

}

void DoReadFinish(FormatRecordPtr formatRecord)
{
	PIDescriptorHandle h;
			
	PIDescriptorParameters * descParams = formatRecord->descriptorParameters;
	if (descParams == NULL) return;
	
	WriteDescriptorProcs * writeProcs = descParams->writeDescriptorProcs;
	if (writeProcs == NULL) return;

	PIWriteDescriptor token = writeProcs->openWriteDescriptorProc();
	if (token == NULL) return;
	
	sPSHandle->Dispose(descParams->descriptor);
	writeProcs->closeWriteDescriptorProc(token, &h);
	descParams->descriptor = h;
	
}


// end TextFormat.cpp