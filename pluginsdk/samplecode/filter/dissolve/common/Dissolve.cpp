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

// project header files
#include "Dissolve.h"
#include "DissolveUI.h"
#include "DissolveScripting.h"
#include "DissolveRegistry.h"
#include "FilterBigDocument.h"
#include <time.h>
#include "Logger.h"
#include "Timer.h"

//-------------------------------------------------------------------------------
// global variables
//-------------------------------------------------------------------------------
// parameters passed into PluginMain that need to be global to the project
FilterRecord * gFilterRecord = NULL;
int16 * gResult = NULL;		// all errors go here
SPBasicSuite * sSPBasic = NULL;

// pointers to our data and parameters defined in Dissolve.h
Data * gData = NULL;
Parameters * gParams = NULL;

//-------------------------------------------------------------------------------
// local routines
//-------------------------------------------------------------------------------
// the six main routines of the plug in
void DoAbout(void);
void DoParameters(void);
void DoPrepare(void);
void DoStart(void);
void DoContinue(void);
void DoFinish(void);

void DoFilter(void);
void CalcProxyScaleFactor(void);
void ConvertRGBColorToMode(const int16 imageMode, FilterColor& color);
void ScaleRect(VRect& destination, const int16 num, const int16 den);
void ShrinkRect(VRect& destination, const int16 width, const int16 height);
void CopyRect(VRect& destination, const VRect& source);
void LockHandles(void);
void UnlockHandles(void);
void CreateParametersHandle(void);
void InitParameters(void);
void CreateDataPtr(void);
void InitData(void);
void DissolveRectangle(void* data, 
					   int32 dataRowBytes, 
					   void* mask, 
					   int32 maskRowBytes, 
					   VRect tileRect, 
					   uint8 color,
					   int32 depth);



//-------------------------------------------------------------------------------
//
//	PluginMain
//	
//	All calls to the plug in module come through this routine.
//
//	Inputs:
//		const int16 selector		Host provides selector indicating what
//									command to do.
//
//	Inputs and Outputs:
//		FilterRecord *filterRecord	Host provides a pointer to parameter block
//									containing pertinent data and callbacks.
//									See PIFilter.h
//
//		intptr_t *data				Use this to store a pointer to our global
//									data structure, which is maintained by the
//									host between calls to the plug in.
//
//	Outputs:
//		int16 *result				Returns error result. Some errors are handled
//									by the host, some are silent, and some you
//									must handle. See PIGeneral.h.
//
//-------------------------------------------------------------------------------
DLLExport MACPASCAL void PluginMain(const int16 selector,
								    FilterRecordPtr filterRecord,
								    intptr_t * data,
								    int16 * result)
{
	
	try {


	Logger logIt("Dissolve");
	Timer timeIt;

	logIt.Write( "Selector: ", false );
	logIt.Write( selector, false );
	logIt.Write( " ", false );

	// update our global parameters
	gFilterRecord = filterRecord;
	gResult = result;
	gData = (Data*)*data;

	if (selector == filterSelectorAbout)
	{
		sSPBasic = ((AboutRecord*)gFilterRecord)->sSPBasic;
	}
	else
	{
		sSPBasic = gFilterRecord->sSPBasic;

		if (gFilterRecord->bigDocumentData != NULL)
			gFilterRecord->bigDocumentData->PluginUsing32BitCoordinates = true;
	}

	// do the command according to the selector
	switch (selector)
	{
		case filterSelectorAbout:
			DoAbout();
			break;
		case filterSelectorParameters:
			DoParameters();
			break;
		case filterSelectorPrepare:
			DoPrepare();
			break;
		case filterSelectorStart:
			DoStart();
			break;
		case filterSelectorContinue:
			DoContinue();
			break;
		case filterSelectorFinish:
			DoFinish();
			break;
		default:
			break;
	}
	
	// Unlock our handles used by gParams
	if (selector != filterSelectorAbout)
		UnlockHandles();

	// give the pointer back to Photoshop so it can return the favor in future calls
	*data = (intptr_t)gData;

	logIt.Write( timeIt.GetElapsed(), true );

	} // end try

	catch (...)
	{
		if (NULL != result)
			*result = -1;
	}

}



//-------------------------------------------------------------------------------
//
// DoParameters
//
// Makes sure we have valid Data and Parameters handle(s). Locks and initializes
// these items.
// 
// NOTE:
// This routine is NOT guaranteed to be called by Photoshop. If a user enters the
// CTRL-F keyboard shortcut to invoke the last filter command this routine will
// NOT be called. If the filter is ran by the actions panel or an automation
// plug in this routine will NOT be called.
//
// NOTE:
// The fields in the gFilterRecord are not all valid at this stage.
//-------------------------------------------------------------------------------
void DoParameters(void)
{
	if (gFilterRecord->parameters == NULL)
		CreateParametersHandle();
	if (gData == 0)
		CreateDataPtr();
	if (*gResult == noErr)
	{
		LockHandles();
		InitParameters();
		InitData();
	}
}



//-------------------------------------------------------------------------------
//
// DoPrepare
//
// Almost identical to DoParameters. Make sure we have valid Data and Parameters
// handle(s) and lock and initialize as necessary. Sets the bufferSpace and 
// maxSpace variables in the gFilterRecord so memory is used at an optimum.
//
// NOTE:
// The fields in the gFilterRecord are not all valid at this stage. We will take a
// guess at the actual tile size information.
// 
//-------------------------------------------------------------------------------
void DoPrepare(void)
{
	if (gFilterRecord->parameters != NULL)
		LockHandles();
	else
	{
		if (gFilterRecord->parameters == NULL)
			CreateParametersHandle();
		if (gData == NULL)
			CreateDataPtr();
		if (*gResult == noErr)
		{
			LockHandles();
			InitParameters();
			InitData();
		}
	}
	
	// we don't need any buffer space
	gFilterRecord->bufferSpace = 0; 

	// give as much memory back to Photoshop as you can
	// we only need a tile per plane plus the maskData
	// inTileHeight and inTileWidth are invalid at this
	// point. Assume the tile size is 256 max.
	VRect filterRect = GetFilterRect();
	int32 tileHeight = filterRect.bottom - filterRect.top;
	int32 tileWidth = filterRect.right - filterRect.left;
	if (tileHeight > 256)
		tileHeight = 256;
	if (tileWidth > 256)
		tileWidth = 256;

	int32 tileSize = tileHeight * tileWidth;
	int32 planes = gFilterRecord->planes;
	if (gFilterRecord->maskData != NULL)
		planes++;
	// duplicate because we have two copies, inData and outData
	planes *= 2;

	int32 totalSize = tileSize * planes;
	// this is worst case and can be dropped considerably
	if (gFilterRecord->maxSpace > totalSize)
		gFilterRecord->maxSpace = totalSize;
}



//-------------------------------------------------------------------------------
//
// DoStart
//
// The main filtering routine for this plug in. See if we have any registry
// parameters from the last time we ran. Determine if the UI needs to be
// displayed by reading the script parameters. Save the last dialog parameters
// in case something goes wrong or the user cancels.
//
//-------------------------------------------------------------------------------
void DoStart(void)
{
	LockHandles();

	// see if we have any information in the Photoshop registry
	ReadRegistryParameters();

	// save parameters
	int16 lastDisposition = gParams->disposition;
	int16 lastPercent = gParams->percent;
	Boolean lastIgnoreSelection = gParams->ignoreSelection;

	// does the user want a dialog
	Boolean isOK = true;
	Boolean displayDialog;
	OSErr err = ReadScriptParameters(&displayDialog);

	// run the dialog on the specific OS
	if (!err && displayDialog)
		isOK = DoUI();

	// we know we have enough information to run without next time
	gData->queryForParameters = false;

	if (isOK)
	{
		// the main processing routine
		DoFilter();
	}
	else
	{
		// restore if the user hit cancel
		gParams->disposition = lastDisposition;
		gParams->percent = lastPercent;
		gParams->ignoreSelection = lastIgnoreSelection;
		*gResult = userCanceledErr;
	}
}



//-------------------------------------------------------------------------------
//
// DoContinue
//
// If we get here we probably did something wrong. This selector was needed
// before advanceState() was in the FilterRecord*. Now that we use advanceState()
// there is nothing for us to do but set all the rectangles to 0 and return.
//
//-------------------------------------------------------------------------------
void DoContinue(void)
{
	VRect zeroRect = { 0, 0, 0, 0 };

	SetInRect(zeroRect);
	SetOutRect(zeroRect);
	SetMaskRect(zeroRect);
}



//-------------------------------------------------------------------------------
//
// DoFinish
//
// Everything went as planned and the pixels have been modified. Now record
// scripting parameters and put our information in the Photoshop Registry for the
// next time we get called. The Registry saves us from keeping a preferences file.
//
//-------------------------------------------------------------------------------
void DoFinish(void)
{
	LockHandles();
	WriteScriptParameters();
	WriteRegistryParameters();
}



//-------------------------------------------------------------------------------
//
// DoFilter
//
// Randomly change the pixel values based on the parameters the user gave us from
// our dialog box or scripting. We do this a tile at a time making sure the rect.
// we ask for is in the bounds of the filterRect.
//
//-------------------------------------------------------------------------------
void DoFilter(void)
{
	// make the random number generated trully random
	srand((unsigned)time(NULL));

	int32 tileHeight = gFilterRecord->outTileHeight;
	int32 tileWidth = gFilterRecord->outTileWidth;

	if (tileWidth == 0 || tileHeight == 0)
	{
		*gResult = filterBadParameters;
		return;
	}

	VRect filterRect = GetFilterRect();
	int32 rectWidth = filterRect.right - filterRect.left;
	int32 rectHeight = filterRect.bottom - filterRect.top;

	CreateDissolveBuffer(tileWidth, tileHeight);

	// round up to the nearest horizontal and vertical tile count
	int32 tilesVert = (tileHeight - 1 + rectHeight) / tileHeight;
	int32 tilesHoriz = (tileWidth - 1 + rectWidth) / tileWidth;

	// Fixed numbers are 16.16 values 
	// the first 16 bits represent the whole number
	// the last 16 bits represent the fraction
	gFilterRecord->inputRate = (int32)1 << 16;
	gFilterRecord->maskRate = (int32)1 << 16;
 
	// variables for the progress bar, our plug in is so fast
	// we probably don't need these
	int32 progressTotal = tilesVert * tilesHoriz;
	int32 progressDone = 0;

	// loop through each tile makeing sure we don't go over the bounds
	// of the rectHeight or rectWidth
	for (int32 vertTile = 0; vertTile < tilesVert; vertTile++)
	{
		for (int32 horizTile = 0; horizTile < tilesHoriz; horizTile++)
		{
			UpdateDissolveBuffer(tileWidth, tileHeight);

			filterRect = GetFilterRect();
			VRect inRect = GetInRect();

			inRect.top = vertTile * tileHeight + filterRect.top;
			inRect.left = horizTile * tileWidth + filterRect.left;
			inRect.bottom = inRect.top + tileHeight;
			inRect.right = inRect.left + tileWidth;

			if (inRect.bottom > rectHeight)
				inRect.bottom = rectHeight;
			if (inRect.right > rectWidth)
				inRect.right = rectWidth;

			SetInRect(inRect);

			// duplicate what's in the inData with the outData
			SetOutRect(inRect);
			
			// get the maskRect if the user has given us a selection
			if (gFilterRecord->haveMask)
			{
				SetMaskRect(inRect);
			}

			for (int16 plane = 0; plane < gFilterRecord->planes; plane++)
			{
				// we want one plane at a time, small memory foot print is good
				gFilterRecord->outLoPlane = gFilterRecord->inLoPlane = plane;
				gFilterRecord->outHiPlane = gFilterRecord->inHiPlane = plane;
	
				// update the gFilterRecord with our latest request
				*gResult = gFilterRecord->advanceState();
				if (*gResult != noErr) return;

				// muck with the pixels in the outData buffer
				uint8 color = 255;
				int16 expectedPlanes = CSPlanesFromMode(gFilterRecord->imageMode,
					                                    0);

				if (plane < expectedPlanes)
					color = gData->color[plane];

				DissolveRectangle(gFilterRecord->outData,
								  gFilterRecord->outRowBytes,
								  gFilterRecord->maskData,
								  gFilterRecord->maskRowBytes,
								  GetOutRect(), 
								  color,
								  gFilterRecord->depth);
			}

			// uh, update the progress bar
			gFilterRecord->progressProc(++progressDone, progressTotal);
			
			// see if the user is impatient or didn't mean to do that
			if (gFilterRecord->abortProc())
			{
				*gResult = userCanceledErr;
				return;
			}
		}
	}
	DeleteDissolveBuffer();
}



//-------------------------------------------------------------------------------
//
// DissolveRectangle
//
// This is our core algorithm for changing pixels randomly to a new color. We
// have to look at the maskPixel and the ignoreSelection parameter to know if we
// should or should not modify the pixel. This routine gets a little messy
// because it handles both 8 bit, 16 bit and 32 bit images.
//
//-------------------------------------------------------------------------------
void DissolveRectangle(void* data, 
					   int32 dataRowBytes, 
					   void* mask, 
					   int32 maskRowBytes, 
					   VRect tileRect, 
					   uint8 color,
					   int32 depth)
{
	uint8* pixel = (uint8*)data;
	uint16* bigPixel = (uint16*)data;
	float* fPixel = (float*)data;
	uint8* maskPixel = (uint8*)mask;
	Ptr dissolve = gData->dissolveBuffer;

	int32 rectHeight = tileRect.bottom - tileRect.top;
	int32 rectWidth = tileRect.right - tileRect.left;

	uint16 bigColor = (uint16)(((uint32)color * 1285 + 5) / 10);
	float  fColor = (float)((float)color / (float)255);

	for(int32 pixelY = 0; pixelY < rectHeight; pixelY++)
	{
		for(int32 pixelX = 0; pixelX < rectWidth; pixelX++)
		{
			
			bool leaveItAlone = false;
			if (maskPixel != NULL && !(*maskPixel) && !gParams->ignoreSelection)
				leaveItAlone = true;
			
			if (*dissolve && !leaveItAlone)
			{
				if (depth == 32)
					*fPixel = fColor;
				else if (depth == 16)
					*bigPixel = bigColor;
				else
					*pixel = color;
			}
			pixel++;
			bigPixel++;
			fPixel++;
			dissolve++;
			if (maskPixel != NULL)
				maskPixel++;
		}
		pixel += (dataRowBytes - rectWidth);
		bigPixel += (dataRowBytes / 2 - rectWidth);
		fPixel += (dataRowBytes / 4 - rectWidth);
		if (maskPixel != NULL)
			maskPixel += (maskRowBytes - rectWidth);
	}
}



//-------------------------------------------------------------------------------
//
// CreateParametersHandle
//
// Create a handle to our Parameters structure. Photoshop will take ownership of
// this handle and delete it when necessary.
//-------------------------------------------------------------------------------
void CreateParametersHandle(void)
{
	gFilterRecord->parameters = gFilterRecord->handleProcs->newProc
											(sizeof(Parameters));
	if (gFilterRecord->parameters == NULL)
		*gResult = memFullErr;
}



//-------------------------------------------------------------------------------
//
// InitParameters
//
// Initialize our UI parameters. gParams is guaranteed to point at something
//-------------------------------------------------------------------------------
void InitParameters(void)
{
	gParams->disposition = 1;
	gParams->ignoreSelection = false;
	gParams->percent = 50;
}



//-------------------------------------------------------------------------------
//
// CreateDataPtr
//
// Create a pointer to our Data structure. Photoshop will take ownership of this
// and give it back to use on any future calls.
//-------------------------------------------------------------------------------
void CreateDataPtr(void)
{
	gData = (Data*) malloc(sizeof(Data));
	if (gData == NULL)
	{
		*gResult = memFullErr;
		return;
	}
}


//-------------------------------------------------------------------------------
//
// InitData
//
// Initialize the gData pointer
//-------------------------------------------------------------------------------
void InitData(void)
{
	CopyColor(gData->colorArray[0], gFilterRecord->backColor);
	SetColor(gData->colorArray[1], 0, 0, 255, 0);
	SetColor(gData->colorArray[2], 255, 0, 0, 0);
	SetColor(gData->colorArray[3], 0, 255, 0, 0);
	for(int a = 1; a < 4; a++)
		ConvertRGBColorToMode(gFilterRecord->imageMode, gData->colorArray[a]);
	CopyColor(gData->color, gData->colorArray[gParams->disposition]);
	gData->proxyRect.left = 0;
	gData->proxyRect.right = 0;
	gData->proxyRect.top = 0;
	gData->proxyRect.bottom = 0;
	gData->scaleFactor = 1.0;
	gData->queryForParameters = true;
	gData->dissolveBufferID = NULL;
	gData->dissolveBuffer = NULL;
	gData->proxyBufferID = NULL;
	gData->proxyBuffer = NULL;
	gData->proxyWidth = 0;
	gData->proxyHeight = 0;
	gData->proxyPlaneSize = 0;
}



//-------------------------------------------------------------------------------
//
// CreateDissolveBuffer
//
// Updates the dissolve array variables in the gData block. This allows us to 
// process one plane at a time
//
// Global Inputs and Outputs:
//		BufferID gData->dissolveBufferID	ID number of the current buffer in use
//											if it exists we must delete and get
//											another one.
//
//		Ptr		 gData->dissolveBuffer	Actual array containing the dissolve
//										from the current settings of
//										gParams->percent
//			
//-------------------------------------------------------------------------------
void CreateDissolveBuffer(const int32 width, const int32 height)
{
	BufferProcs *bufferProcs = gFilterRecord->bufferProcs;

	bufferProcs->allocateProc(width * height, &gData->dissolveBufferID);
	gData->dissolveBuffer = bufferProcs->lockProc(gData->dissolveBufferID, true);
}



//-------------------------------------------------------------------------------
//
// UpdateDissolveBuffer
//
// Updates the dissolve buffer array variables in the gData block. This allows us
// to process one plane at a time
//
// Global Inputs and Outputs:
//		Ptr		 gData->dissolveBuffer	Actual array containing the dissolve
//										from the current settings of
//										gParams->percent
//			
//-------------------------------------------------------------------------------
void UpdateDissolveBuffer(const int32 width, const int32 height)
{
	if (gData->dissolveBuffer != NULL)
	{
		Ptr dissolve = gData->dissolveBuffer;
		for (int32 x = 0; x < width; x++)
		{
			for (int32 y = 0; y < height; y++)
			{
				*dissolve = ((unsigned16) rand()) % 100 < gParams->percent;
				dissolve++;
			}
		}
	}
}



//-------------------------------------------------------------------------------
//
// DeleteDissolveBuffer
//
// Deletes the dissolve array variables in the gData block.
//
// Global Inputs and Outputs:
//		BufferID gData->dissolveBufferID	ID number of the current buffer in use
//											if it exists we must delete and get
//											another one.
//
//		Ptr		 gData->dissolveBuffer	Actual array containing the dissolve
//										from the current settings of
//										gParams->percent
//			
//-------------------------------------------------------------------------------
void DeleteDissolveBuffer(void)
{
	if (gData->dissolveBufferID != NULL)
	{
		gFilterRecord->bufferProcs->unlockProc(gData->dissolveBufferID);
		gFilterRecord->bufferProcs->freeProc(gData->dissolveBufferID);
		gData->dissolveBufferID = NULL;
		gData->dissolveBuffer = NULL;
	}
}



//-------------------------------------------------------------------------------
//
// SetupFilterRecordForProxy
//
// Called by the UI routine to set up the gFilterRecord with the proxy view
// information. CalcProxyScaleFactor sizes the proxy rectangle and calculates the
// scale factor. Then set up gFilterRecord and call advanceState() to init the
// inData with the pixel data for the display.
//
// Global Inputs and Outputs:
//		FilterRecord *gFilterRecord		inRect, inRowBytes, maskRect, etc. has all
//										the information needed to call 
//										advanceState so inData points to the proxy
//										pixel data and maskRect points to the 
//										selection data
//			
//-------------------------------------------------------------------------------
void SetupFilterRecordForProxy(void)
{
	CalcProxyScaleFactor();

	SetInRect(GetFilterRect()); // gFilterRecord->inRect = gFilterRecord->filterRect;
	
	VRect tempRect = GetInRect();
	
	ScaleRect(tempRect, 1, (int16)gData->scaleFactor);

	SetInRect(tempRect);
	
	SetMaskRect(GetInRect()); // gFilterRecord->maskRect = gFilterRecord->inRect;

	// Fixed numbers are 16.16 values 
	// the first 16 bits represent the whole number
	// the last 16 bits represent the fraction
	gFilterRecord->inputRate = (int32)gData->scaleFactor << 16;
	gFilterRecord->maskRate = (int32)gData->scaleFactor << 16;
 
	gFilterRecord->inputPadding = 255;
	gFilterRecord->maskPadding = gFilterRecord->inputPadding;
	
	gData->proxyWidth = gData->proxyRect.right - gData->proxyRect.left;
	gData->proxyHeight = gData->proxyRect.bottom - gData->proxyRect.top;
	gData->proxyPlaneSize = gData->proxyWidth * gData->proxyHeight;
}



//-------------------------------------------------------------------------------
//
// CalcProxyScaleFactor
//
// Called by the UI routine to change the size of the Proxy rectangle and 
// calculate the scale factor. This information is needed by the displayPixels
// routine and the advanceState routine.
//
// Global Inputs:
//		Rect gFilterRecord->filterRect		Rectangle to filter
//
// Global Inputs and Outputs:
//		Rect gData->proxyRect				Rectangle for the Proxy to display in
//		float gData->scaleFactor			Scale factor of the filterRect 
//											to the ProxyRect
//			
//-------------------------------------------------------------------------------
void CalcProxyScaleFactor(void)
{
	int32 filterHeight, filterWidth, itemHeight, itemWidth;
	VPoint fraction;
	
	VRect filterRect = GetFilterRect();
	
	// we place a black frame around the proxy and leave a black
	// between the frame and the actual pixel data
	ShrinkRect(gData->proxyRect, 2, 2);

	filterHeight = filterRect.bottom - filterRect.top;
	filterWidth  = filterRect.right  - filterRect.left;
	
	itemHeight = (gData->proxyRect.bottom - gData->proxyRect.top);
	itemWidth  = (gData->proxyRect.right  - gData->proxyRect.left);
	
	// make sure the proxy isn't bigger than the image after the calculation
	// this will make the proxy half the size for images smaller than the proxy
	if (itemHeight > filterHeight)
		itemHeight = filterHeight;
		
	if (itemWidth > filterWidth)
		itemWidth = filterWidth;
	
	fraction.h = ((filterWidth + itemWidth) / itemWidth);
	fraction.v = ((filterHeight + itemHeight) / itemHeight);

	// calculate the scale factor based on the smaller of height or width
	if (fraction.h > fraction.v) 
		gData->scaleFactor = ((float)filterWidth + (float)itemWidth) /
		                      (float)itemWidth;
	else
		gData->scaleFactor = ((float)filterHeight + (float)itemHeight) / 
		                      (float)itemHeight;

	CopyRect(gData->proxyRect, filterRect);	
	ScaleRect(gData->proxyRect, 1, (int16)gData->scaleFactor);

	// recalculate the scale factor based on the actual proxy size
	// this will reduce rounding errors in the proxy view
	if (fraction.h > fraction.v) 
		gData->scaleFactor = (float)filterWidth / 
		                      (float)(gData->proxyRect.right  - 
							   gData->proxyRect.left);
	else
		gData->scaleFactor = (float)filterHeight / 
							  (float)(gData->proxyRect.bottom - 
							   gData->proxyRect.top);
}



//-------------------------------------------------------------------------------
//
// ConvertRGBColorToMode
//
// Convert the FilterColor from RGB mode to the imageMode using the color
// services call backs.
//
// Inputs:
//		int16 imageMode			Mode to convert the color to
// Inputs and Outputs:
//		FilterColor& color		RGB color to convert
//
//-------------------------------------------------------------------------------
void ConvertRGBColorToMode(const int16 imageMode, FilterColor& color)
{
	if (imageMode != plugInModeRGBColor)
	{
		ColorServicesInfo	csInfo;

		csInfo.selector = plugIncolorServicesConvertColor;
		csInfo.sourceSpace = plugIncolorServicesRGBSpace;
		csInfo.reservedSourceSpaceInfo = NULL;
		csInfo.reservedResultSpaceInfo = NULL;
		csInfo.reserved = NULL;
		csInfo.selectorParameter.pickerPrompt = NULL;
		csInfo.infoSize = sizeof(csInfo);

		csInfo.resultSpace = CSModeToSpace(gFilterRecord->imageMode);
		for (int16 a = 0; a < 4; a++)
			csInfo.colorComponents[a] = color[a];

		if (!(gFilterRecord->colorServices(&csInfo)))
			for (int16 b = 0; b < 4; b++)
				color[b] = (int8)csInfo.colorComponents[b];
	}				   
}



//-------------------------------------------------------------------------------
//
// LockHandles
//
// Lock the handles and get the pointers for gParams
// Set the global error, *gResult, if there is trouble
//
//-------------------------------------------------------------------------------
void LockHandles(void)
{
	if (gFilterRecord->parameters == NULL)
	{
		*gResult = filterBadParameters;
		return;
	}
	gParams = (Parameters*)gFilterRecord->handleProcs->lockProc
				(gFilterRecord->parameters, TRUE);
	if (gParams == NULL)
	{
		*gResult = memFullErr;
		return;
	}
}



//-------------------------------------------------------------------------------
//
// UnlockHandles
//
// Unlock the handles used by the params pointers
//
//-------------------------------------------------------------------------------
void UnlockHandles(void)
{
	if (gFilterRecord->parameters != NULL)
		gFilterRecord->handleProcs->unlockProc(gFilterRecord->parameters);
}



//-------------------------------------------------------------------------------
//
// ScaleRect
//
// Utility routine for scaling a rectangle by a rational
//
//-------------------------------------------------------------------------------
void ScaleRect(VRect& destination, const int16 num, const int16 den)
{
	if (den != 0)
	{
		destination.left = (int16)((destination.left * num) / den);
		destination.top = (int16)((destination.top * num) / den);
		destination.right = (int16)((destination.right * num) / den);
		destination.bottom = (int16)((destination.bottom * num) / den);
	}
}



//-------------------------------------------------------------------------------
//
// ShrinkRect
//
// Utility routine for shrinking a Rect by a width and height
//
//-------------------------------------------------------------------------------
void ShrinkRect(VRect& destination, const int16 width, const int16 height)
{
	destination.left = (int16)(destination.left + width);
	destination.top = (int16)(destination.top + height);
	destination.right = (int16)(destination.right - width);
	destination.bottom = (int16)(destination.bottom - height);
}



//-------------------------------------------------------------------------------
//
// CopyRect
//
// Utility routine for setting a Rect from a VRect
//
//-------------------------------------------------------------------------------
void CopyRect(VRect& destination, const VRect& source)
{
	destination.left = source.left;
	destination.top = source.top;
	destination.right = source.right;
	destination.bottom = source.bottom;
}



//-------------------------------------------------------------------------------
//
// CopyColor
//
// Utility routine for setting a FilterColor array from a FilterColor
//
//-------------------------------------------------------------------------------
void CopyColor(FilterColor& destination, const FilterColor& source)
{
	for (int a = 0; a < sizeof(FilterColor); a++)
		destination[a] = source[a];
}



//-------------------------------------------------------------------------------
//
// SetColor
//
// Utility routine for setting a FilterColor array from 4 color components
//
//-------------------------------------------------------------------------------
void SetColor(FilterColor& destination, 
			  const uint8 a, 
			  const uint8 b, 
			  const uint8 c, 
			  const uint8 d)
{
	destination[0] = a;
	destination[1] = b;
	destination[2] = c;
	destination[3] = d;
}



//-------------------------------------------------------------------------------
//
// CreateProxyBuffer
//
//-------------------------------------------------------------------------------
void CreateProxyBuffer(void)
{
	int32 proxySize = gData->proxyPlaneSize * gFilterRecord->planes;
	OSErr e = gFilterRecord->bufferProcs->allocateProc(proxySize, &gData->proxyBufferID);
	if (!e && gData->proxyBufferID)
		gData->proxyBuffer = gFilterRecord->bufferProcs->lockProc(gData->proxyBufferID, true);
}



//-------------------------------------------------------------------------------
//
// ResetProxyBuffer
//
//-------------------------------------------------------------------------------
extern "C" void ResetProxyBuffer(void)
{
	uint8* proxyPixel = (uint8*)gData->proxyBuffer;

	if (proxyPixel != NULL)
	{
		for (int16 plane = 0; plane < gFilterRecord->planes; plane++)
		{
			gFilterRecord->inLoPlane = plane;
			gFilterRecord->inHiPlane = plane;
			
			*gResult = gFilterRecord->advanceState();
			if (*gResult != noErr) return;
			
			uint8* inPixel = (uint8*)gFilterRecord->inData;
			
			for (int32 y = 0; y < gData->proxyHeight; y++)
			{
				uint8* start = inPixel;

				for (int32 x = 0; x < gData->proxyWidth; x++)
				{
					if (gFilterRecord->depth == 32)
					{
						float * reallyBigPixel = (float*)inPixel;
						if ( *reallyBigPixel > 1.0 )
							*reallyBigPixel = 1.0;
						if ( *reallyBigPixel < 0.0 )
							*reallyBigPixel = 0.0;
						*proxyPixel = (uint8)(*reallyBigPixel * 255);
						inPixel+=4;
					} else 	if (gFilterRecord->depth == 16)
					{
						uint16* bigPixel = (uint16*)inPixel;
						*proxyPixel = (uint8)(*bigPixel * 10 / 1285);
						inPixel+=2;
					}
					else
					{
						*proxyPixel = *inPixel;
						inPixel++;
					}
					proxyPixel++;
				}
				inPixel = start + gFilterRecord->inRowBytes;
			}
		}
	}
}



//-------------------------------------------------------------------------------
//
// UpdateProxyBuffer
//
//-------------------------------------------------------------------------------
extern "C" void UpdateProxyBuffer(void)
{
	Ptr localData = gData->proxyBuffer;

	if (localData != NULL)
	{
		UpdateDissolveBuffer(gData->proxyWidth, gData->proxyHeight);
		for (int16 plane = 0; plane < gFilterRecord->planes; plane++)
		{
			uint8 color = 255;
			uint16 expectedPlanes = CSPlanesFromMode(gFilterRecord->imageMode, 0); 
			if (plane < expectedPlanes)
				color = gData->color[plane];
			DissolveRectangle(localData, 
				              gData->proxyWidth, 
							  gFilterRecord->maskData, 
							  gFilterRecord->maskRowBytes, 
							  gData->proxyRect, 
							  color,
							  8);
			localData += (gData->proxyPlaneSize);
		}
	}
}



//-------------------------------------------------------------------------------
//
// DeleteProxyBuffer
//
//-------------------------------------------------------------------------------
void DeleteProxyBuffer(void)
{
	gFilterRecord->bufferProcs->unlockProc(gData->proxyBufferID);
	gFilterRecord->bufferProcs->freeProc(gData->proxyBufferID);
	gData->proxyBufferID = NULL;
	gData->proxyBuffer = NULL;
}



//-------------------------------------------------------------------------------
//
// DisplayPixelsMode
//
// Convert the imageMode into a display mode so we can use displayPixels.
// All of the 16 bit data is converted to 8 bit.
//
//-------------------------------------------------------------------------------
int32 DisplayPixelsMode(int16 mode)
{
	int32 returnMode = mode;
	switch (mode)
	{
		case plugInModeGray16:
		case plugInModeGray32:
			returnMode = plugInModeGrayScale;
			break;
		case plugInModeRGB96:
		case plugInModeRGB48:
			returnMode = plugInModeRGBColor;
			break;
		case plugInModeLab48:
			returnMode = plugInModeLabColor;
			break;
		case plugInModeCMYK64:
			returnMode = plugInModeCMYKColor;
			break;
		case plugInModeDeepMultichannel:
			returnMode = plugInModeMultichannel;
			break;
		case plugInModeDuotone16:
			returnMode = plugInModeDuotone;
			break;
	}
	return (returnMode);
}
// end Dissolve.cpp
