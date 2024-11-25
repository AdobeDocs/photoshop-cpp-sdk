// ADOBE SYSTEMS INCORPORATED
// Copyright  1993 - 2002 Adobe Systems Incorporated
// All Rights Reserved
//
// NOTICE:  Adobe permits you to use, modify, and distribute this 
// file in accordance with the terms of the Adobe license agreement
// accompanying it.  If you have received this file from a source
// other than Adobe, then your use, modification, or distribution
// of it requires the prior written permission of Adobe.
//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
//
//	File:
//		PoorMansTypeTool.cpp
//
//	Description:
//		This file contains the source and routines for the
//		Filter module Poor Man's Type Tool, a module that 
//		uses the Channel Ports Suite for pixel munging.
//
//	Use:
//		This is a basic module to exemplify all the typical
//		functions a filter module will do: Read scripting
//		parameters, show a user interface, display a proxy,
//		write changed pixel information, and then write
//		scripting parameters.
//
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
//	Includes
//-------------------------------------------------------------------------------

#include "PoorMansTypeTool.h"

//-------------------------------------------------------------------------------
//	Prototypes.
//-------------------------------------------------------------------------------

void InitGlobals (Ptr globalPtr);		  	// Initialize globals.
void ValidateParameters (GPtr globals);		// Validates (inits) parameters.

void DoParameters (GPtr globals);
void DoPrepare (GPtr globals);
void DoStart (GPtr globals);				// Main routine.
void DoFinish (GPtr globals);

void scaleRect(Rect *l, short n, short d);
void shrinkRect(Rect *l, short x, short y);
void copyRect(Rect *l, const VRect *r);

short NumberOfChannels(GPtr globals);

void DoEffect(GPtr globals);
void GaussianBlurEffect(GPtr globals, Rect *inRect, char* data);
void GaussianBlurEffect(GPtr globals, Rect *inRect, Rect *containmentRect, char* data);
void ReadLayerData(GPtr globals, 
				   ReadChannelDesc* pChannel,  
				   char* pLayerData, 
				   Boolean scaled);
void WriteLayerData(GPtr globals, ReadChannelDesc* pChannel, char* pLayerData);
void HelloWorldData(GPtr globals, char *data);
void HelloWorldData(char *data, Rect *outRect, Rect *containmentRect);
void HelloWorldMaskData(GPtr globals, char *data);
void HelloWorldMaskData(char *data, Rect *outRect, Rect * containmentRect, Boolean reset);
void MergeSelectionAndMaskIntoMask(GPtr globals);
				
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
//		FilterRecord *filterParamBlock		Host provides pointer to parameter
//											block containing pertinent data
//											and callbacks from the host.
//											See PIFilter.h.
//
//	Outputs:
//		FilterRecord *filterParamBlock		Host provides pointer to parameter
//											block containing pertinent data
//											and callbacks from the host.
//											See PIFilter.h.
//
//		void *data							Use this to store a pointer to our
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
		
		// See if our suite pointers are initialized. If you run this plug in on 
		// Photoshop 5.0.2 you will get an error the first time you run because
		// InitGlobals() fails. The second time you run InitGlobals() does not get
		// called.
		if (gPSChannelPortsSuite == NULL || gPSBufferSuite64 == NULL)
		{
			*result = errPlugInHostInsufficient;
			return;
		}

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
			case filterSelectorFinish:
				DoFinish(globals);
				break;
		}
				
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

	gProxyRect.top = 0;
	gProxyRect.bottom = 0;
	gProxyRect.left = 0;
	gProxyRect.right = 0;
	gViewAllLayerData = FALSE;
	gViewLayerIndex = 0;
	gDocDesc = gDocInfo;
	gScaleFactor = 1;
	gChannelData = NULL;
	gMaskData = NULL;
	gSelectionData = NULL;
	gOverlayData = NULL;
	if (gStuff->sSPBasic->AcquireSuite( kPSChannelPortsSuite,
					    				kPSChannelPortsSuiteVersion3,
										(const void **)&gPSChannelPortsSuite))
			gResult = errPlugInHostInsufficient;
	if (gStuff->sSPBasic->AcquireSuite(	kPSBufferSuite,
										kPSBufferSuiteVersion2,
										(const void **)&gPSBufferSuite64))
			gResult = errPlugInHostInsufficient;

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
//		gPointH				Default: 0.
//
//		gPointV				Default: 0.
//
//		gXFactor			Default: 4.
//
//		gIgnoreSelection	Default: false.
//
//-------------------------------------------------------------------------------

void ValidateParameters (GPtr globals)
{
	if (gStuff->parameters == NULL)
	{ // We haven't created these yet.

		gStuff->parameters = PINewHandle ((long) sizeof (TParameters));

		if (gStuff->parameters != NULL)
		{ // Got it.  Fill out the fields.
			gPointH = 0;
			gPointV = 0;
			gXFactor = 4;
			gGaussianBlurData = FALSE;
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
   number of bytes required. 

   In this example we are using the Buffer Suite, this allows us to not have
   to deal with the bufferSpace field. We make all our requests for memory
   through the gPSBufferSuite->New() routine.

*/

void DoPrepare (GPtr globals)
{
	ValidateParameters (globals);
}


/*****************************************************************************/

void DoStart (GPtr globals)
{		
	ValidateParameters (globals);
	/* if stuff hasn't been initialized that we need, do it,
		then go check if we've got scripting commands to
		override our settings */
	
	// update our parameters with the scripting parameters, if available
	ReadScriptParams (globals); 

	// always reset the global document descriptor for safety
	gDocDesc = gDocInfo;

	if (gQueryForParameters)
	{
		DoUI (globals);	// Show the UI
		gQueryForParameters = FALSE;
	}

	// Delete the memory we used for the Proxy view
	ReleaseProxyMemory(globals);

	if (gResult != noErr)
		return;
			
	// Do the actual filtering operation on the original image
	DoEffect(globals);
		
}


/*****************************************************************************/

/* Do any necessary clean-up. */

void DoFinish (GPtr globals)
{
	// writes our parameters to the scripting system
	WriteScriptParams (globals); 
}


/******************************************************************************/

/* Do the actual filtering operation on the original image */

void DoEffect(GPtr globals)
{
	// Get a buffer to hold each channel as we process
	char *pLayerData = gPSBufferSuite64->New(NULL, 
										   VSIZE * HSIZE * gXFactor * gXFactor);
	if (pLayerData == NULL)
		return;
		
	// Start with the first target composite channel
	ReadChannelDesc *pChannel = gDocDesc->targetCompositeChannels;

	// we may have a multichannel document
	if (pChannel == NULL)
		pChannel = gDocDesc->alphaChannels;
	
	// Get some information for the progress bar
	int32 done = 0;
	int32 total = NumberOfChannels(globals) + 1;
	
	// Loop through each of the channels
	while (pChannel != NULL && gResult == 0)
	{
		// Update the progress bar
		PIUpdateProgress(done++, total);
		
		// Read in this channel data, we actually don't need this data
	 	// but I will read it in anyway, it is an example you know
		ReadLayerData(globals, pChannel, pLayerData, false);
		
		// Stamp the "Hello World" into the data buffer
		HelloWorldData(globals, pLayerData);
		
		// Decide if we should blur our effect or just write out the
		// data. GaussianBlurEffect() will use a new channel port
		if (gGaussianBlurData)
			{
			Rect write_rect;
			write_rect.top = gPointV;
			write_rect.left = gPointH;
			write_rect.bottom = (short)(gPointV + VSIZE * gXFactor);
			write_rect.right = (short)(gPointH + HSIZE * gXFactor);
			GaussianBlurEffect(globals, &write_rect, pLayerData);
			}
		WriteLayerData(globals, pChannel, pLayerData);
			
		// off to the next channel
		pChannel = pChannel->next;
	}

	// Update the progress bar
	PIUpdateProgress(done++, total);
	
	// now update the transparency information
	// this will be non null for layered data
	pChannel = gDocDesc->targetTransparency;
	if (gResult == 0)
	{
		// Read in this channel data, we actually don't need this data
	 	// but I will read it in anyway, it is an example you know
		ReadLayerData(globals, pChannel, pLayerData, false);

		// Stamp the "Hello World" into the data buffer
		HelloWorldMaskData(globals, pLayerData);

		// write out the channel mask
		WriteLayerData(globals, pChannel, pLayerData);
	}

	// Update the progress bar
	PIUpdateProgress(done++, total);
	
	// dispose of that temp buffer we have been using
	gPSBufferSuite64->Dispose((char**)&pLayerData);
}


/******************************************************************************/

/* Do the gaussian blur effect to the proxy view. We are passed in two
   rectangles: inRect is the rectangle of "Hello World " and containmentRect
   is the rectangle of the complete proxy view. I already have a routine that
   can Gaussian Blur data but it must be contiguous. This routine pulls out the
   rect data into contiguous data, Gaussian Blur's it and then sticks it back
   into the containmentRect. */

void GaussianBlurEffect(GPtr globals, Rect *inRect, Rect *containmentRect, char* data)
{
	// Get the hello world out of the containmentRect
	Rect localRect = *inRect;
	int32 vSize = inRect->bottom - inRect->top;
	int32 hSize = inRect->right - inRect->left;

	int32 hCont = containmentRect->right - containmentRect->left;

	if (inRect->bottom > containmentRect->bottom)
	{
		vSize -= inRect->bottom - containmentRect->bottom;
		localRect.bottom -= inRect->bottom - containmentRect->bottom;
	}

	if (inRect->right > containmentRect->right)
	{
		hSize -= inRect->right - containmentRect->right;
		localRect.right -= inRect->right - containmentRect->right;
	}

	char *localData = gPSBufferSuite64->New(NULL, vSize * hSize);
	if (localData == NULL)
		return;

	// keep track of our starting point
	char *originalData = data;
	char *originalLocal = localData;
	
	if (data != NULL)
		data += hCont * inRect->top + inRect->left;
	else
		return;

	int a = 0;
	int b = 0;
	for (a = 0; a < vSize; a++)
	{
		for (b = 0; b < hSize; b++)
		{
			*localData = *data;
			data++;
			localData++;
		}
		data += hCont - hSize;
	}

	// Do the gaussian blur effect
	GaussianBlurEffect(globals, &localRect, originalLocal);
	
	// Put the hello world back into the containmentRect
	data = originalData;
	localData = originalLocal;
	
	data += hCont * inRect->top + inRect->left;

	for (a = 0; a < vSize; a++)
	{
		for (b = 0; b < hSize; b++)
		{
			*data = *localData;
			data++;
			localData++;
		}
		data += hCont - hSize;
	}
	
	gPSBufferSuite64->Dispose((char**)&originalLocal);
}

/*****************************************************************************/

/* Use the Channel Ports Operation to Gaussian Blur the Effect. We use two ports
   one as the input and one as the output. Push the data into the input port.
   Gaussian Blur from the inport to the outport and then read the data back
   out of the outport. */

void GaussianBlurEffect(GPtr globals, Rect *inRect, char *data)
{
	// Make sure Photoshop supports the Gaussian Blur operation
	Boolean supported;
	if (gPSChannelPortsSuite->SupportsOperation(PSChannelPortGaussianBlurFilter, 
												&supported))
		return;
	if (!supported)
		return;
		
	// Set up a local rect for the size of our port
	VRect write_rect;
	write_rect.top = inRect->top;
	write_rect.left = inRect->left;
	write_rect.bottom = inRect->bottom;
	write_rect.right = inRect->right;

	PIChannelPort inPort, outPort;
	
	// Photoshop will make us a new port and manage the memory for us
	if (gPSChannelPortsSuite->New(&inPort,
								  &write_rect,
								  8,
								  true))
		return;
	if (gPSChannelPortsSuite->New(&outPort,
								  &write_rect,
								  8,
								  true))
		return;
		
	// Set up a PixelMemoryDesc to tell how our channel data is laid out
	PixelMemoryDesc destination;
	destination.data = data;
	destination.depth = 8;
	destination.rowBits = (inRect->right - inRect->left) * 8;
	destination.colBits = 8;
	destination.bitOffset = 0 ;

	// Write the current effect we have into this port
	if (gPSChannelPortsSuite->WritePixelsToBaseLevel(inPort, 
													 &write_rect,
													 &destination))
		return;

	// Set up the parameters for the Gaussian Blur
	PSGaussianBlurParameters gbp;
	int inRadius = 1;
	Fixed what = inRadius << 16;
	gbp.radius = what;
	gbp.padding = -1;

	// Gaussian Blur from our temp port to the writePort of this channel
	/* SPErr error = */ gPSChannelPortsSuite->ApplyOperation(PSChannelPortGaussianBlurFilter, 
										 inPort,
										 outPort,
										 NULL,
										 (void*)&gbp,
										 &write_rect);
	
 	if (gPSChannelPortsSuite->ReadPixelsFromLevel(outPort,
												  0,
												  &write_rect,
												  &destination))
		return;

	// Delete the temp port in use
	gPSChannelPortsSuite->Dispose(&inPort);
	gPSChannelPortsSuite->Dispose(&outPort);
}


/*****************************************************************************/

/* Read in the channel data from the original document. If we are reading for
   the proxy view make sure we scale it. */

void ReadLayerData(GPtr globals, ReadChannelDesc *pChannel, char *pLayerData, Boolean scaled)
	{

	// Make sure there is something for me to read from
	Boolean canRead;
	if (pChannel == NULL)
	{
		canRead = false;
	}
	else if (pChannel->port == NULL)
	{
		canRead = false;
	}
	else if (gPSChannelPortsSuite->CanRead(pChannel->port, &canRead))
	{
		// this function should not error, tell the host accordingly
		gResult = errPlugInHostInsufficient;
		return;
	}

	// if everything is still ok we will continue
	if (!canRead || pLayerData == NULL)
		return;

	// some local variables to play with
	VRect read_rect;
	PixelMemoryDesc destination;
	PSScaling scaling;
	
	// are we doing this for the proxy view or on the original image
	if (scaled)
	{
		// All this work is done for the proxy view
		
		// Get the source document bounds
		scaling.sourceRect.top = pChannel->bounds.top;
		scaling.sourceRect.left = pChannel->bounds.left;
		scaling.sourceRect.bottom = pChannel->bounds.bottom;
		scaling.sourceRect.right = pChannel->bounds.right;

		// Get the bounds of the proxy view in the dialog
		scaling.destinationRect.top = 0;
		scaling.destinationRect.left = 0;
		scaling.destinationRect.bottom = gProxyRect.bottom - gProxyRect.top;
		scaling.destinationRect.right = gProxyRect.right - gProxyRect.left;

		// set up the PixelMemoryDesc
		destination.data = pLayerData;
		destination.depth = pChannel->depth;
		destination.rowBits = (scaling.destinationRect.right - scaling.destinationRect.left) 
							  * pChannel->depth;
		destination.colBits = 8;
		destination.bitOffset = 0;

		read_rect = scaling.destinationRect;

		// scale the document data into our proxy rect for display
		if (globals->sPSChannelPortsSuite->ReadScaledPixels(
			pChannel->port,
			&read_rect, 
			&scaling, 
			&destination))
		{
			return;
		}
	}
	else
	{
		// All this work is done for the DoEffect()
		
		// What area of the document do we want to read from
		read_rect.top = gPointV;
		read_rect.left = gPointH;
		read_rect.bottom = gPointV + VSIZE * gXFactor;
		read_rect.right = gPointH + HSIZE * gXFactor;

		// set up the PixelMemoryDesc
		destination.data = pLayerData;
		destination.depth = pChannel->depth;
		destination.rowBits = HSIZE * pChannel->depth * gXFactor;
		destination.colBits = 8;
		destination.bitOffset = 0 ;

		// Read this data into our buffer, you could check the read_rect to see if
		// you got everything you desired
		if (gPSChannelPortsSuite->ReadPixelsFromLevel(	
			pChannel->port,
			0,
			&read_rect,
			&destination))
		{
			gResult = errPlugInHostInsufficient;
			return;
		}
	}
}

/******************************************************************************/

/* Change the data in this channel to "Hello World " in big ugly block characters
   If you want something smaller than 8x88 then this will need some work */

void HelloWorldData(GPtr globals, char *data)
{
	const char *pHelloWorld = &HelloWorld[0][0];

	for (int a = 0; a < VSIZE; a++)
	{
		for (int b = 0; b < gXFactor; b++)
		{
			for (int c = 0; c < HSIZE; c++)
			{
				for (int d = 0; d < gXFactor; d++)
				{
					if (*pHelloWorld == 'X')
					{
						*data = (char)0;
					}
					else
					{
						*data = (char)255;
					}
					data++;
				}
				pHelloWorld++;
			}
			pHelloWorld -= HSIZE;
		}
		pHelloWorld += HSIZE;
	}
}


/******************************************************************************/

/* This is going the other way. Try to squeeze the "Hello World " characters
   into the proxy view. */

void HelloWorldData(char *data, Rect *outRect, Rect *containmentRect)
{
	int32 vSize = outRect->bottom - outRect->top;
	int32 hSize = outRect->right - outRect->left;

	int32 hCont = containmentRect->right - containmentRect->left;

	float vOffset = (float)VSIZE / (float)vSize;
	float hOffset = (float)HSIZE / (float)hSize;

	if (outRect->bottom > containmentRect->bottom)
		vSize -= outRect->bottom - containmentRect->bottom;

	if (outRect->right > containmentRect->right)
		hSize -= outRect->right - containmentRect->right;

	int c = 0; // index into the HelloWorld array
	int d = 0;

	if (data != NULL)
		data += hCont * outRect->top + outRect->left;
	else
		return;

	for (int a = 0; a < vSize; a++)
	{
		c = (int)((float)a * vOffset);
		for (int b = 0; b < hSize; b++)
		{
			d = (int)((float)b * hOffset);
			if (HelloWorld[c][d] == 'X')
				*data = (char)0;
			else
				*data = (char)255;
			data++;
		}
		data += hCont - hSize;
	}
}

/******************************************************************************/

/* Change the data in this channel's mask so the "Hello World " 
   text can be viewed. */

void HelloWorldMaskData(GPtr globals, char *pLayerData)
{
	if (pLayerData != NULL)
		memset(pLayerData, 255, VSIZE * HSIZE * gXFactor * gXFactor);
}

/******************************************************************************/

/* Change the data in this channel's mask so the "Hello World " text can be 
   viewed in the proxy window. */

void HelloWorldMaskData(char *data, Rect *outRect, Rect * containmentRect, Boolean reset)
{
	if (data == NULL || outRect == NULL || containmentRect == NULL )
		return;

	int32 vSize = outRect->bottom - outRect->top;
	int32 hSize = outRect->right - outRect->left;

	int32 hCont = containmentRect->right - containmentRect->left;

	if (reset)
		memset(data, 0, hCont * (containmentRect->bottom - containmentRect->top));

	if (outRect->bottom > containmentRect->bottom)
		vSize -= outRect->bottom - containmentRect->bottom;

	if (outRect->right > containmentRect->right)
		hSize -= outRect->right - containmentRect->right;

	// don't go into never never land
	if (vSize < 0)
		vSize = 0;

	if (hSize < 0)
		hSize = 0;

	data += hCont * outRect->top + outRect->left;

	for (int a = 0; a < vSize; a++)
	{
		memset(data, 255, hSize);
		data += hCont;
	}
}

/*****************************************************************************/

/* Write out the effect to this channel using the Channel Ports Suite. Make 
   sure we can write to this port and we have a valid data pointer. */

void WriteLayerData(GPtr globals, ReadChannelDesc *pChannel, char *pLayerData)
{
	Boolean canWrite = true;
	
	if (pChannel == NULL || pLayerData == NULL)
	{
		canWrite = false;
	}
	else if (pChannel->writePort == NULL)
	{
		canWrite = false;
	}
	else if (gPSChannelPortsSuite->CanWrite(pChannel->writePort, &canWrite))
	{
		gResult = errPlugInHostInsufficient;
		return;
	}

	if (!canWrite)
		return;

	VRect write_rect;
	write_rect.top = gPointV;
	write_rect.left = gPointH;
	write_rect.bottom = gPointV + VSIZE * gXFactor;
	write_rect.right = gPointH + HSIZE * gXFactor;

	PixelMemoryDesc destination;
	destination.data = pLayerData;
	destination.depth = pChannel->depth;
	destination.rowBits = HSIZE * pChannel->depth * gXFactor;
	destination.colBits = 8;
	destination.bitOffset = 0 ;

	if (gPSChannelPortsSuite->WritePixelsToBaseLevel(	
		pChannel->writePort, 
		&write_rect,
		&destination))
	{
		gResult = errPlugInHostInsufficient;
		return;
	}
}


/*****************************************************************************/

/* Read in the imaged scaled to fit in my proxy window. Use the Channel Ports Suite.
   Apply the effect to each channel as you read it in. This is very similar to 
   DoEffect except on the proxy view data which we keep around. All the proxy data
   buffers need initialization only once, unless you change the proxy view so it can 
   resize. */

void UpdateProxyView(GPtr globals)
{
	// Get some local memory for our display
	// If we have valid pointers then just reuse
	unsigned32 memorySize = 1;
	unsigned32 channelSize = (gProxyRect.bottom - gProxyRect.top)
		             * (gProxyRect.right - gProxyRect.left);
	memorySize *= channelSize;
	memorySize *= NumberOfChannels(globals);

	if (gChannelData == NULL)
	{
		gChannelData = gPSBufferSuite64->New(NULL, memorySize);
		if (gChannelData == NULL)
		{
			gResult = memFullErr;
			return;
		}
	}

	ReadChannelDesc *pChannel = NULL;
	char *pLayerData = NULL;
	short channelCount = 0;

	// figure out what channel(s) we are trying to manipulate
	if (gViewLayerIndex == 0)
	{
		if (gViewAllLayerData)
			pChannel = gDocDesc->mergedCompositeChannels;
		else
			pChannel = gDocDesc->targetCompositeChannels;
	}
	else
	{
		int16 index = gViewLayerIndex - 1;
		ReadLayerDesc * layerDesc = gDocDesc->layersDescriptor;
		while (index && layerDesc != NULL)
		{	
			layerDesc = layerDesc->next;
			index--;
		}

		if (layerDesc != NULL)
			pChannel = layerDesc->compositeChannelsList;
	}
		
	// we may have a multichannel document
	if (pChannel == NULL)
		pChannel = gDocDesc->alphaChannels;

	// figure out the rectangle of "Hello World " in proxy space
	Rect outRect;
	outRect.top = (short)(gPointV / gScaleFactor);
	outRect.left = (short)(gPointH / gScaleFactor);
	outRect.bottom = (short)(outRect.top + VSIZE / gScaleFactor * gXFactor);
	outRect.right = (short)(outRect.left + HSIZE / gScaleFactor * gXFactor);
	
	// gChannelData holds all the channels, so adjust the starting point
	// accordingly. The Gaussian Blur effect is a little off in the proxy view
	// because the data is scaled down for the effect.
	while (pChannel != NULL && gResult == 0)
	{
		pLayerData = gChannelData + channelSize * channelCount++;
		ReadLayerData(globals, pChannel, pLayerData, true);
		HelloWorldData(pLayerData, &outRect, &gProxyRect);
		if (gGaussianBlurData)
			GaussianBlurEffect(globals, &outRect, &gProxyRect, pLayerData);
		pChannel = pChannel->next;
	}

	// Rest of the channels hold less data
	short numChannels = NumberOfChannels(globals);
	if (numChannels > 0)
		memorySize /= numChannels;

	// are we showing another layer, all the merged channels or just the target
	if (gViewLayerIndex == 0)
	{
		if (gViewAllLayerData)
			pChannel = gDocDesc->mergedTransparency;
		else
			pChannel = gDocDesc->targetTransparency;
	}
	else
	{
		int16 index = gViewLayerIndex - 1;
		ReadLayerDesc * layerDesc = gDocDesc->layersDescriptor;
		while (index && layerDesc != NULL)
		{	
			layerDesc = layerDesc->next;
			index--;
		}

		if (layerDesc != NULL)
			pChannel = layerDesc->transparency;
	}

	if (pChannel != NULL)
	{
		if (gMaskData == NULL)
		{
			gMaskData = gPSBufferSuite64->New(NULL, memorySize);
			if (gMaskData == NULL)
			{
				gResult = memFullErr;
				return;
			}
		}

		// Read in the mask and set it so we can see the channels
		ReadLayerData(globals, pChannel, gMaskData, true);
		HelloWorldMaskData(gMaskData, &outRect, &gProxyRect, false);
	}

	// Do we have a selection, I don't do anything with it except in the proxy view
	// I ignore the selection when actually doing the effect. You may want to add
	// additional logic to not do this.
	if (gDocDesc->selection != NULL)
	{
		if (gSelectionData == NULL)
		{
			gSelectionData = gPSBufferSuite64->New(NULL, memorySize);
			if (gSelectionData == NULL)
			{
				gResult = memFullErr;
				return;
			}
		}

		ReadLayerData(globals, gDocDesc->selection, gSelectionData, true);
	}

	// Make the selection look like a mask
	MergeSelectionAndMaskIntoMask(globals);
	
	// This shows how to set up the overlay information in your proxy view.
	// We used this for the painting in Extract. You may want to play with
	// that plug in for a better use of overlays.
	if (gOverlayData == NULL)
	{
		gOverlayData = gPSBufferSuite64->New(NULL, memorySize);
		if (gOverlayData == NULL)
		{
			gResult = memFullErr;
			return;
		}
	}
	HelloWorldMaskData(gOverlayData, &outRect, &gProxyRect, true);
}


/*****************************************************************************/

/* Make the selection look like it is masked out. This plug in ignores the
   currently selected data when writing out the effect. */

void MergeSelectionAndMaskIntoMask(GPtr globals)
{
	if (gSelectionData == NULL || gMaskData == NULL)
		return;

	long bytes = (gProxyRect.bottom - gProxyRect.top) * 
		         (gProxyRect.right - gProxyRect.left);

	char *pSelection = gSelectionData;
	char *pMask = gMaskData;

	for (long a = 0; a < bytes; a++, pSelection++, pMask++)
		if (*pSelection == 0)
			*pMask = 0;
}


/*****************************************************************************/

/* Some rectangle routines for generating the proxy view rectangle. */

void scaleRect(Rect *l, short n, short d) 
{
	l->left = (l->left * n) / d;
	l->top = (l->top * n) / d;
	l->right = (l->right * n) / d;
	l->bottom = (l->bottom * n) / d;
}

void shrinkRect(Rect *l, short x, short y) 
{
	l->left += x;
	l->top += y;
	l->right -= x;
	l->bottom -= y;
}

void copyRect(Rect *l, const VRect *r) 
{
	l->left = (short)r->left;
	l->top = (short)r->top;
	l->right = (short)r->right;
	l->bottom = (short)r->bottom;
}


/***********************************************************************************/

/* Computes the scaled down rectangle and the scale factor for the proxy */

void CalcProxyScaleFactor (const VRect *filterRect, Rect *proxyRect, float *scaleFactor)
{

	if (filterRect == NULL || proxyRect == NULL || scaleFactor == NULL)
		return;

	short filterHeight;
	short filterWidth;
	short itemHeight;
	short itemWidth;
	Point fraction;

	shrinkRect (proxyRect, kGutter, kGutter); 		 // for the border

	filterHeight = (short)(filterRect->bottom - filterRect->top);
	filterWidth  = (short)(filterRect->right  - filterRect->left);
	
	itemHeight = proxyRect->bottom - proxyRect->top;
	itemWidth  = proxyRect->right  - proxyRect->left;
	
	if (itemHeight > filterHeight)
		itemHeight = filterHeight;
		
	if (itemWidth > filterWidth)
		itemWidth = filterWidth;
	
	fraction.h = (short)((filterWidth + itemWidth) / itemWidth);
	fraction.v = (short)((filterHeight + itemHeight) / itemHeight);

	// compute a scale factor now
	if (fraction.h > fraction.v) 
		*scaleFactor = ((float)filterWidth + (float)itemWidth) / (float)itemWidth;
	else
		*scaleFactor = ((float)filterHeight + (float)itemHeight) / (float)itemHeight;

	// resize the proxy rectangle
	copyRect (proxyRect, filterRect);	
	scaleRect (proxyRect, 1, (short)*scaleFactor);

	// now get a more precise scale factor from the resulting rectangle
	if (fraction.h > fraction.v) 
		*scaleFactor = (float)filterWidth / (float)(proxyRect->right  - proxyRect->left);
	else
		*scaleFactor = (float)filterHeight / (float)(proxyRect->bottom - proxyRect->top);

}


/***********************************************************************************/

/* Use the documentInfo found in the FilterRecord to calculate the number of channels
   in this document. */

short NumberOfChannels(GPtr globals)
{
	short numberChannels = 0;

	ReadChannelDesc *pChannel = gDocDesc->targetCompositeChannels;
	// we may have a multichannel document
	if (pChannel == NULL)
		pChannel = gDocDesc->alphaChannels;
	while (pChannel != NULL)
	{
		numberChannels++;
		pChannel = pChannel->next;
	}
	return numberChannels;
}


/***********************************************************************************/

/* Use the documentInfo found in the FilterRecord to calculate the number of layers
   in this document.  */

short NumberOfLayers(GPtr globals)
{
	short numberLayers = 0;

	ReadLayerDesc *pLayerDesc = gDocDesc->layersDescriptor;

	while (pLayerDesc != NULL)
	{
		numberLayers++;
		pLayerDesc = pLayerDesc->next;
	}
	return numberLayers;
}


/***********************************************************************************/

/* Releases the local memory we used to display the proxy image view. */

void ReleaseProxyMemory(GPtr globals)
{
	if (gSelectionData != NULL)
	{
		gPSBufferSuite64->Dispose((char**)&gSelectionData);
		gSelectionData = NULL;
	}
	
	if (gMaskData != NULL)
	{
		gPSBufferSuite64->Dispose((char**)&gMaskData);
		gMaskData = NULL;
	}
	
	if (gChannelData != NULL)
	{
		gPSBufferSuite64->Dispose((char**)&gChannelData);
		gChannelData = NULL;
	}
	
	if (gOverlayData != NULL)
	{
		gPSBufferSuite64->Dispose((char**)&gOverlayData);
		gOverlayData = NULL;
	}
}
/****************************************************************************/
// end PoorMansTypeTool.cpp
