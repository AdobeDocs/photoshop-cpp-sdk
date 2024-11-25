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
//		Selectorama.cpp
//
//	Description:
//		This file contains the source code for the main functions
//		for the Selection module Selectorama, an example module
//		that just returns a pixel selection.
//
//	Use:
//		Use selection modules to return pixel or path selections on a new
//		layer or the current layer.
//
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
//	Includes
//-------------------------------------------------------------------------------

#include "Selectorama.h"

//-------------------------------------------------------------------------------
//	Globals -- Define global variables for plug-in scope.
//-------------------------------------------------------------------------------

SPBasicSuite *  sSPBasic = NULL;

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
//		PISelectionParams *selectionParamBlock		Host provides pointer to parameter
//													block containing pertinent data
//													and callbacks from the host.
//													See PISelection.h.
//
//	Outputs:
//		PISelectionParams *selectionParamBlock		Host provides pointer to parameter
//													block containing pertinent data
//													and callbacks from the host.
//													See PISelection.h.
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
						             SelectionRecordPtr selectionParamBlock,
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

	if (selector == selectionSelectorAbout)
	{
		sSPBasic = ((AboutRecordPtr)selectionParamBlock)->sSPBasic;
		DoAbout((AboutRecordPtr)selectionParamBlock);
	}
	else
	{ // do the rest of the process as normal:

		sSPBasic = selectionParamBlock->sSPBasic;

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
									 selectionParamBlock,
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

		DoExecute(globals);
			
	} // about selector special		
	
	} // end try

	catch(...)
	{
		if (NULL != result)
		{
			*result = -1;
		}
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
	gWhatArea = iSelectMin;
	gWhatChannels = iUseTarget;
	gPercent = 50;
	gCreate = iCreateSelection;
	gQueryForParameters = false;
	
} // end InitGlobals

/*****************************************************************************/

static void TrimVRect (VRect *dst, VRect *src)
	{
	
	if (dst->top    < src->top   ) dst->top    = src->top;
	if (dst->left   < src->left  ) dst->left   = src->left;
	if (dst->bottom > src->bottom) dst->bottom = src->bottom;
	if (dst->right  > src->right ) dst->right  = src->right;
	
	}

/*****************************************************************************/

static Boolean EqualVRects (VRect *a, VRect *b)
	{
	
	return  a->top    == b->top
		 && a->left   == b->left
		 && a->bottom == b->bottom
		 && a->right  == b->right;
	
	}

/*****************************************************************************/

static int32 AccountChannel (ReadChannelDesc *source,
									  ReadChannelDesc *mask,
									  WriteChannelDesc *dest)
	{
	
	VRect limit;
	
	limit = source->bounds;
	TrimVRect (&limit, &dest->bounds);
	if (mask != NULL) TrimVRect (&limit, &mask->bounds);
		
	if (limit.right <= limit.left || limit.bottom <= limit.top) return 0;
	
	return (limit.right - limit.left) * (limit.bottom - limit.top);
	
	}

/*****************************************************************************/

static void ApplyChannel (GPtr globals,
						  ReadChannelDesc *source,
						  PixelMemoryDesc *sDesc,
						  ReadChannelDesc *mask, 
						  PixelMemoryDesc *mDesc,
						  WriteChannelDesc *dest, 
						  ChannelReadPort destRead,
						  PixelMemoryDesc *dDesc,
						  PixelMemoryDesc *rDesc,
						  size_t *done,
						  size_t total)
	{
	
	VRect limit;
	int32 row, col, row2, col2;
	unsigned8 *s, *m, *d;
	unsigned8 *r = (unsigned8 *)rDesc->data;
	Boolean initRandom = (*r == kInitRandom);
		
	limit = source->bounds;
	TrimVRect (&limit, &dest->bounds);
	if (mask != NULL) TrimVRect (&limit, &mask->bounds);
		
	if (limit.right <= limit.left || limit.bottom <= limit.top) return;
	
	for (row = limit.top; row < limit.bottom; row += kBlockRows)
		for (col = limit.left; col < limit.right; col += kBlockCols)
			{
			
			VRect area;
			PSScaling scaling;
			VRect wrote;
			
			if (TestAbort ())
				{
				gResult = userCanceledErr;
				return;
				}
			
			area.top = row;  area.bottom = row + kBlockRows;
			area.left = col; area.right  = col + kBlockCols;
			
			if (limit.bottom < area.bottom) area.bottom = limit.bottom;
			if (limit.right  < area.right ) area.right  = limit.right;
			
			scaling.sourceRect = area;
			scaling.destinationRect = area;
			
			gResult = ReadPixels (destRead, &scaling, &area, dDesc, &wrote);
			if (gResult != noErr) return;
			
			if (!EqualVRects (&area, &wrote))
				{
				gResult = -1;
				return;
				}
				
			gResult = ReadPixels (source->port, &scaling, &area, sDesc, &wrote);
			if (gResult != noErr) return;
			
			if (!EqualVRects (&area, &wrote))
				{
				gResult = -1;
				return;
				}
				
			if (mask != NULL)
				{
				
				gResult = ReadPixels (mask->port, &scaling, &area, mDesc, &wrote);
				if (gResult != noErr) return;
				
				if (!EqualVRects (&area, &wrote))
					{
					gResult = -1;
					return;
					}
				}
				m = (unsigned8 *) mDesc->data;
				/* mask all set and ready to go */
									
			/* heart of the routine.  Compares source pixel and destination
			   pixel and, if destination is smaller, replaces it with
			   source */

			s = (unsigned8 *) sDesc->data;
			d = (unsigned8 *) dDesc->data;
			r = (unsigned8 *) rDesc->data;
						
			for (row2 = kBlockRows; row2 > 0; --row2)
				for (col2 = kBlockCols; col2 > 0; --col2)
				{
					
					switch (gWhatArea)
					{
						case iSelectMin:
							if (mask != NULL && *m < *s ) *s = *m;
							if (*s > 127) *d = *s;
							else *d = 0;
							break;
						case iSelectMax:
							if (mask != NULL && *m < *s ) *s = *m;
							if (*s < 128) *d = 255 - *s; // if (*d < *s) *d = *s;
							else *d = 0;
							break;
						case iSelectRandom:
							if (initRandom)
							{
								if ((((unsigned16) rand ()) % 100) < gPercent)
									*r = kRandomOn; // flag as picked
								else *r = kRandomOff; // flag as off
							}
							if (*r == kRandomOn)
								*d = 255;
							else *d = 0;
							break;
					}
					++s;
					++d;
					++r;
					if (mask != NULL) ++m;
				}
					
			gResult = WritePixels (dest->port, &area, dDesc);
			if (gResult != noErr) return;
			
			*done += (area.right - area.left) * (area.bottom - area.top);
			
			PIUpdateProgress ((int32)*done, (int32)total);
				
			}
	
	}

/*****************************************************************************/

void DoExecute (GPtr globals)
{
	
	size_t total = 0;
	size_t done = 0;
	
	Boolean	doThis = true;
	
	BufferID sBuffer = 0;
	Ptr sData = NULL;
	BufferID dBuffer = 0;
	Ptr dData = NULL;
	BufferID mBuffer = 0;
	Ptr mData = NULL;
	BufferID rBuffer = 0;
	Ptr rData = NULL;
	
	PixelMemoryDesc sDesc, dDesc, mDesc, rDesc;

	ReadImageDocumentDesc *doc = gStuff->documentInfo;
	WriteChannelDesc *selection = gStuff->newSelection;
	ChannelReadPort selectionRead;
	ReadChannelDesc *composite;
	ReadChannelDesc *transparency;
	ReadChannelDesc *curChannel;
	
	/* Access the port procs. */
	
	if (!WarnChannelPortAvailable ())
		{
		gResult = errPlugInHostInsufficient;
		goto CleanUp;
		}
		
	gQueryForParameters = ReadScriptParams (globals);
	
	if ( gQueryForParameters ) doThis = DoUI (globals);
	
	if ( !doThis ) goto CleanUp; // user canceled, etc.
	
	/* look in gStuff->supportedTreatments for support for this next thang */
		
	if (gCreate == iCreateMaskpath)
	{ // can't do that!
		Str255 errString = " paths cannot be made from pixel selections";
        errString[0] = (char)strlen((char*)&errString[1]);
		gResult = PIReportError(errString);
		goto CleanUp; // don't do any of this gunk
	}
	else
		gStuff->treatment = KeyToEnum(EnumToKey(gCreate,typeMyCreate),typeMyPISel);
		// Set the treatment then move on
		
	/* We will need a read port for the selection. */
	
	gResult = ReadFromWritePort(&selectionRead, selection->port);
	if (gResult != noErr) goto CleanUp;
	
	/* Figure out which composite data to use. */
	
	if (DoTarget)
	{
		composite = doc->targetCompositeChannels;
		transparency = doc->targetTransparency;
	
		if (composite == NULL)
		{
			composite = doc->mergedCompositeChannels;
			transparency = doc->mergedTransparency;
		}
	}
	else
	{
		composite = doc->mergedCompositeChannels;
		transparency = doc->mergedTransparency;
	}
			
	/* Allocate the buffers. */
	
	if (!WarnBufferProcsAvailable ())
		{
		gResult = +1;
		goto CleanUp;
		}
		
	#define kBufferSize kBlockRows * (long) kBlockCols
	
	gResult = AllocateBuffer (kBufferSize, &sBuffer);
	if (gResult != noErr) goto CleanUp;
	
	gResult = AllocateBuffer (kBufferSize, &dBuffer);
	if (gResult != noErr) goto CleanUp;
	
	gResult = AllocateBuffer (kBufferSize, &rBuffer);
	if (gResult != noErr) goto CleanUp;
	
	if (transparency != NULL)
		{
		gResult = AllocateBuffer (kBufferSize, &mBuffer);
		if (gResult != noErr) goto CleanUp;
		}
		
	/* Lock the buffers down. */
	
	sData = LockBuffer (sBuffer, false);
	dData = LockBuffer (dBuffer, false);
	rData = LockBuffer (rBuffer, false);
	if (!sData || !dData || !rData) goto CleanUp;
	if (mBuffer != 0)
	{
		mData = LockBuffer(mBuffer, false);
		if (!mData) goto CleanUp;
	}
	
	
	/* Set up the pixel memory descriptors. */
	
	sDesc.data = sData;
	sDesc.rowBits = kBlockCols * 8;
	sDesc.colBits = 8;
	sDesc.bitOffset = 0;
	sDesc.depth = 8;
	
	dDesc = sDesc;
	dDesc.data = dData;
	
	rDesc = sDesc;
	rDesc.data = rData;
	
	mDesc = sDesc;
	mDesc.data = mData;
	
	rData[0] = kInitRandom; // flag for uninitialized random buffer
	
	/* Count the channels to process. */
	
	if (doc->selection != NULL)
		total += AccountChannel (doc->selection, NULL, selection);
	
	curChannel = composite;
	while (curChannel != NULL)
	{
		if (DoTarget ? curChannel->target : curChannel->shown)
			total += AccountChannel (curChannel, transparency, selection);
									  
		curChannel = curChannel->next;
	}
		

	if (doc->targetLayerMask != NULL && 
	   (DoTarget ? doc->targetLayerMask->target : doc->targetLayerMask->shown))
			total += AccountChannel (doc->targetLayerMask, NULL, selection);
			
	curChannel = doc->alphaChannels;
	while (curChannel != NULL)
	{
		if (DoTarget ? curChannel->target : curChannel->shown)
			total += AccountChannel (curChannel, NULL, selection);
							  
		curChannel = curChannel->next;
	}

	
	/* Apply any existing selection. */
	
	if (doc->selection != NULL)
		{
		ApplyChannel (globals, doc->selection, &sDesc,
						  NULL, &mDesc,
						  selection, selectionRead, &dDesc,
						  &rDesc, &done, total);
		if (gResult != noErr) goto CleanUp;
		}
	
	/* Apply each of the channels. This isn't the most efficient
	approach since it reads the data back out of the selection
	repeatedly, but it provides a good work out for the code. */
	
	curChannel = composite;
	while (curChannel != NULL)
	{
		if (DoTarget ? curChannel->target : curChannel->shown)
		{
			ApplyChannel (globals, curChannel, &sDesc,
							  transparency, &mDesc,
							  selection, selectionRead, &dDesc,
							  &rDesc, &done, total);
			if (gResult != noErr) goto CleanUp;
		}
							  
		curChannel = curChannel->next;
	}
		
	/* Apply the layer mask. */
	if (doc->targetLayerMask != NULL &&
	   (DoTarget ? doc->targetLayerMask->target : doc->targetLayerMask->shown))
	{
		ApplyChannel (globals, doc->targetLayerMask, &sDesc,
						  NULL, &mDesc,
						  selection, selectionRead, &dDesc,
						  &rDesc, &done, total);
		if (gResult != noErr) goto CleanUp;
	}
		
	/* Process the alpha channels. */
	curChannel = doc->alphaChannels;
	while (curChannel != NULL)
	{
		if (DoTarget ? curChannel->target : curChannel->shown)
		{
			ApplyChannel (globals, curChannel, &sDesc,
							  NULL, &mDesc,
							  selection, selectionRead, &dDesc,
							  &rDesc, &done, total);
			if (gResult != noErr) goto CleanUp;
		}
							  
		curChannel = curChannel->next;
	}
	
	CleanUp:
	
	if (sData != NULL) UnlockBuffer (sBuffer);
	if (dData != NULL) UnlockBuffer (dBuffer);
	if (mData != NULL) UnlockBuffer (mBuffer);
	if (rData != NULL) UnlockBuffer (rBuffer);
	
	if (sBuffer != 0) FreeBuffer (sBuffer);
	if (dBuffer != 0) FreeBuffer (dBuffer);
	if (mBuffer != 0) FreeBuffer (mBuffer);	
	if (rBuffer != 0) FreeBuffer (rBuffer);
	
	WriteScriptParams(globals);

}
// end Selectorama.cpp
