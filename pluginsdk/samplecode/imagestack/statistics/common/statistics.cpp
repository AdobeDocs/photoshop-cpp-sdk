/*
	File: statistics.cpp

	Copyright (c) 2006, Adobe Systems Incorporated.
	All rights reserved.

	C source file for statistics example.
*/

#include "statistics.h"
#include "PIAbout.h"
#include <cmath>
#include <vector>
#include <algorithm>

#if __PIWin__
#include "PIUtilities.h"
#endif

#if defined(__APPLE_CC__)

namespace std
{
	using ::log2;
}

#endif

/*****************************************************************************/

typedef enum 
	{
	kVariance,
	kStdDeviation,
	kSkewness,
	kKurtosis,
	kEntropy
	} stat_type;

typedef enum
	{
	kMinimum,
	kMaximum,
	kRange,
	kMedian
	} minmax_type;
	
/*****************************************************************************/

void InitGlobals (GPtr globals);
void ValidateParameters (GPtr globals);

void DoPrepare (GPtr globals);
void DoFinalize (GPtr globals);

void DoStartMean ( GPtr globals, bool isSummation );
void DoContinueMean ( GPtr globals, bool isSummation );
void DoFinishMean (GPtr globals);

void DoStartMinMax (GPtr globals);
void DoContinueMinMax (GPtr globals, minmax_type type);
void DoFinishMinMax (GPtr globals);

void DoStartVariance ( GPtr globals, stat_type type );
void DoContinueVariance ( GPtr globals, stat_type type );
void DoFinishVariance (GPtr globals);

/*****************************************************************************/
	
// the most memory we should use
// for 1000 layers, 64Meg results in 181x181 pixel squares
const int32 maxMemory = 64L * 1024L * 1024L;

// set some sanity limits on the tile size
const int32 minTileSize	= 16;
const int32 maxTileSize = 1024;

SPBasicSuite * sSPBasic = NULL;


/*****************************************************************************/

DLLExport MACPASCAL
void MEAN_ENTRY (const short selector,
				 AcquireRecord *acquireParamBlock,
				 intptr_t *data,
				 int16 *result)
	{
	
	try {

	if (selector == acquireSelectorAbout)
		{
		sSPBasic = ( ( AboutRecord *) acquireParamBlock)->sSPBasic;
		DoAbout(( AboutRecord *) acquireParamBlock);
		return;
		}
	
	sSPBasic = acquireParamBlock->sSPBasic;

	if (!*data)
		{
		
		*data = (intptr_t)malloc (sizeof (Globals));
		
		if (!*data)
			{
			*result = memFullErr;
			return;
			}

		InitGlobals ((GPtr)*data);

		}
		
	GPtr globals = (GPtr)*data;
		
	gStuff = acquireParamBlock;
	gResult = noErr;
	
	switch (selector)
		{
			
		case acquireSelectorPrepare:
			DoPrepare (globals);
			break;
		
		case acquireSelectorStart:
			DoStartMean (globals, false);
			break;
		
		case acquireSelectorContinue:
			DoContinueMean (globals, false);
			break;
		
		case acquireSelectorFinish:
			DoFinishMean (globals);
			break;
			
		case acquireSelectorFinalize:
			DoFinalize (globals);
			break;
		
		default:
			gResult = acquireBadParameters;
		
		}
		
	*result = gResult;	
		

	} // end try

	catch (...)
	{
		if (NULL != result)
		{
			*result = -1;
		}
	}

	}

/*****************************************************************************/

DLLExport MACPASCAL
void SUM_ENTRY (const short selector,
				AcquireRecord *acquireParamBlock,
				intptr_t *data,
				int16 *result)
	{
	
	try {

	if (selector == acquireSelectorAbout)
		{
		sSPBasic = ( ( AboutRecord *) acquireParamBlock)->sSPBasic;
		DoAbout(( AboutRecord *) acquireParamBlock);
		return;
		}
	
	sSPBasic = acquireParamBlock->sSPBasic;

	if (!*data)
		{
		
		*data = (intptr_t)malloc (sizeof (Globals));
		
		if (!*data)
			{
			*result = memFullErr;
			return;
			}
			
		InitGlobals ((GPtr)*data);
		
		}
		
	GPtr globals = (GPtr)*data;
	
	gStuff = acquireParamBlock;
	gResult = noErr;
	
	switch (selector)
		{
		
		case acquireSelectorPrepare:
			DoPrepare (globals);
			break;
		
		case acquireSelectorStart:
			DoStartMean (globals, true);
			break;
		
		case acquireSelectorContinue:
			DoContinueMean (globals, true);
			break;
		
		case acquireSelectorFinish:
			DoFinishMean (globals);
			break;
			
		case acquireSelectorFinalize:
			DoFinalize (globals);
			break;
			
		default:
			gResult = acquireBadParameters;
		
		}
		
	*result = gResult;	

	} // end try

	catch (...)
		{
		if (NULL != result)
			{
			*result = -1;
			}
		}

	}

/*****************************************************************************/

DLLExport MACPASCAL
void MIN_ENTRY (const short selector,
				AcquireRecord *acquireParamBlock,
				intptr_t *data,
				int16 *result)
	{
	
	try {

	if (selector == acquireSelectorAbout)
		{
		sSPBasic = ( ( AboutRecord *) acquireParamBlock)->sSPBasic;
		DoAbout(( AboutRecord *) acquireParamBlock);
		return;
		}
	
	sSPBasic = acquireParamBlock->sSPBasic;

	if (!*data)
		{
		
		*data = (intptr_t)malloc (sizeof (Globals));
		
		if (!*data)
			{
			*result = memFullErr;
			return;
			}
			
		InitGlobals ((GPtr)*data);

		}
		
	GPtr globals = (GPtr) *data;
		
	gStuff = acquireParamBlock;
	gResult = noErr;
	
	switch (selector)
		{
		
		case acquireSelectorPrepare:
			DoPrepare (globals);
			break;
		
		case acquireSelectorStart:
			DoStartMinMax (globals);
			break;
		
		case acquireSelectorContinue:
			DoContinueMinMax (globals, kMinimum);
			break;
		
		case acquireSelectorFinish:
			DoFinishMinMax (globals);
			break;
			
		case acquireSelectorFinalize:
			DoFinalize (globals);
			break;
			
		default:
			gResult = acquireBadParameters;
		
		}
		
	*result = gResult;	
		
	} // end try

	catch (...)
	{
		if (NULL != result)
		{
			*result = -1;
		}
	}

	}

/*****************************************************************************/

DLLExport MACPASCAL
void MAX_ENTRY (const short selector,
				AcquireRecord *acquireParamBlock,
				intptr_t *data,
				int16 *result)
	{
	
	try {

	if (selector == acquireSelectorAbout)
		{
		sSPBasic = ( ( AboutRecord *) acquireParamBlock)->sSPBasic;
		DoAbout(( AboutRecord *) acquireParamBlock);
		return;
		}
	
	sSPBasic = acquireParamBlock->sSPBasic;

	if (!*data)
		{
		
		*data = (intptr_t)malloc (sizeof (Globals));
		
		if (!*data)
			{
			*result = memFullErr;
			return;
			}
			
		InitGlobals((GPtr)*data);
		
		}
		
	GPtr globals = (GPtr) *data;
		
	gStuff = acquireParamBlock;
	gResult = noErr;
	
	switch (selector)
		{
		
		case acquireSelectorPrepare:
			DoPrepare (globals);
			break;
		
		case acquireSelectorStart:
			DoStartMinMax (globals);
			break;
		
		case acquireSelectorContinue:
			DoContinueMinMax (globals, kMaximum);
			break;
		
		case acquireSelectorFinish:
			DoFinishMinMax (globals);
			break;
			
		case acquireSelectorFinalize:
			DoFinalize (globals);
			break;
			
		default:
			gResult = acquireBadParameters;
		
		}
		
	*result = gResult;	
		
	} // end try

	catch (...)
	{
		if (NULL != result)
		{
			*result = -1;
		}
	}

	}

/*****************************************************************************/

DLLExport MACPASCAL
void MEDIAN_ENTRY (const short selector,
				   AcquireRecord *acquireParamBlock,
				   intptr_t *data,
				   int16 *result)
	{

	try {

	if (selector == acquireSelectorAbout)
		{
		sSPBasic = ( ( AboutRecord *) acquireParamBlock)->sSPBasic;
		DoAbout(( AboutRecord *) acquireParamBlock);
		return;
		}
	
	sSPBasic = acquireParamBlock->sSPBasic;

	if (!*data)
		{
		
		*data = (intptr_t)malloc (sizeof (Globals));
		
		if (!*data)
			{
			*result = memFullErr;
			return;
			}
			
		InitGlobals ((GPtr)*data);

		}
		
	GPtr globals = (GPtr) *data;
	
	gStuff = acquireParamBlock;
	gResult = noErr;
	
	switch (selector)
		{
		
		case acquireSelectorPrepare:
			DoPrepare (globals);
			break;
		
		case acquireSelectorStart:
			DoStartMinMax (globals);
			break;
		
		case acquireSelectorContinue:
			DoContinueMinMax (globals, kMedian);
			break;
		
		case acquireSelectorFinish:
			DoFinishMinMax (globals);
			break;
			
		case acquireSelectorFinalize:
			DoFinalize (globals);
			break;
			
		default:
			gResult = acquireBadParameters;
		
		}
		
	*result = gResult;	
		
	} // end try

	catch (...)
	{
		if (NULL != result)
		{
			*result = -1;
		}
	}

	}

/*****************************************************************************/

DLLExport MACPASCAL
void VAR_ENTRY (const short selector,
				AcquireRecord *acquireParamBlock,
				intptr_t *data,
				int16 *result)
	{

	try {
		
	if (selector == acquireSelectorAbout)
		{
		sSPBasic = ( ( AboutRecord *) acquireParamBlock)->sSPBasic;
		DoAbout(( AboutRecord *) acquireParamBlock);
		return;
		}
	
	sSPBasic = acquireParamBlock->sSPBasic;

	if (!*data)
		{
		
		*data = (intptr_t)malloc (sizeof (Globals));
		
		if (!*data)
			{
			*result = memFullErr;
			return;
			}
			
		InitGlobals ((GPtr)*data);

		}
		
	GPtr globals = (GPtr) *data;
	
	gStuff = acquireParamBlock;
	gResult = noErr;
	
	switch (selector)
		{
		
		case acquireSelectorPrepare:
			DoPrepare (globals);
			break;
		
		case acquireSelectorStart:
			DoStartVariance (globals, kVariance);
			break;
		
		case acquireSelectorContinue:
			DoContinueVariance (globals, kVariance);
			break;
		
		case acquireSelectorFinish:
			DoFinishVariance (globals);
			break;
			
		case acquireSelectorFinalize:
			DoFinalize (globals);
			break;
		
		default:
			gResult = acquireBadParameters;
		
		}
		
	*result = gResult;	
		
	} // end try

	catch (...)
	{
		if (NULL != result)
		{
			*result = -1;
		}
	}

	}

/*****************************************************************************/

DLLExport MACPASCAL
void STDDEV_ENTRY (const short selector,
				   AcquireRecord *acquireParamBlock,
				   intptr_t *data,
				   int16 *result)
	{
	
	try {
		
	if (selector == acquireSelectorAbout)
		{
		sSPBasic = ( ( AboutRecord *) acquireParamBlock)->sSPBasic;
		DoAbout(( AboutRecord *) acquireParamBlock);
		return;
		}
	
	sSPBasic = acquireParamBlock->sSPBasic;

	if (!*data)
		{
		
		*data = (intptr_t)malloc (sizeof (Globals));
		
		if (!*data)
			{
			*result = memFullErr;
			return;
			}
			
		InitGlobals ((GPtr)*data);

		}
		
	GPtr globals = (GPtr)*data;
	
	gStuff = acquireParamBlock;
	gResult = noErr;
	
	switch (selector)
		{
		
		case acquireSelectorPrepare:
			DoPrepare (globals);
			break;
		
		case acquireSelectorStart:
			DoStartVariance (globals, kStdDeviation);
			break;
		
		case acquireSelectorContinue:
			DoContinueVariance (globals, kStdDeviation);
			break;
		
		case acquireSelectorFinish:
			DoFinishVariance (globals);
			break;
			
		case acquireSelectorFinalize:
			DoFinalize (globals);
			break;
		
		default:
			gResult = acquireBadParameters;
		
		}
		
	*result = gResult;	
		
	} // end try

	catch (...)
	{
		if (NULL != result)
		{
			*result = -1;
		}
	}

	}

/*****************************************************************************/

DLLExport MACPASCAL
void SKEW_ENTRY (const short selector,
				 AcquireRecord *acquireParamBlock,
				 intptr_t *data,
				 int16 *result)
	{

	try {
		
	if (selector == acquireSelectorAbout)
		{
		sSPBasic = ( ( AboutRecord *) acquireParamBlock)->sSPBasic;
		DoAbout(( AboutRecord *) acquireParamBlock);
		return;
		}
	
	sSPBasic = acquireParamBlock->sSPBasic;

	if (!*data)
		{
		
		*data = (intptr_t)malloc (sizeof (Globals));
		
		if (!*data)
			{
			*result = memFullErr;
			return;
			}
			
		InitGlobals ((GPtr)*data);

		}
		
	GPtr globals = (GPtr)*data;
	
	gStuff = acquireParamBlock;
	gResult = noErr;
	
	switch (selector)
		{
		
		case acquireSelectorPrepare:
			DoPrepare (globals);
			break;
		
		case acquireSelectorStart:
			DoStartVariance (globals, kSkewness);
			break;
		
		case acquireSelectorContinue:
			DoContinueVariance (globals, kSkewness);
			break;
		
		case acquireSelectorFinish:
			DoFinishVariance (globals);
			break;
			
		case acquireSelectorFinalize:
			DoFinalize (globals);
			break;
		
		default:
			gResult = acquireBadParameters;
		
		}
		
	*result = gResult;	
		
	} // end try

	catch (...)
	{
		if (NULL != result)
		{
			*result = -1;
		}
	}

	}

/*****************************************************************************/

DLLExport MACPASCAL
void KURT_ENTRY (const short selector,
				 AcquireRecord *acquireParamBlock,
				 intptr_t *data,
				 int16 *result)
	{

	try {
		
	if (selector == acquireSelectorAbout)
		{
		sSPBasic = ( ( AboutRecord *) acquireParamBlock)->sSPBasic;
		DoAbout(( AboutRecord *) acquireParamBlock);
		return;
		}
	
	sSPBasic = acquireParamBlock->sSPBasic;

	if (!*data)
		{
		
		*data = (intptr_t)malloc (sizeof (Globals));
		
		if (!*data)
			{
			*result = memFullErr;
			return;
			}
			
		InitGlobals ((GPtr)*data);

		}
		
	GPtr globals = (GPtr)*data;
	
	gStuff = acquireParamBlock;
	gResult = noErr;
	
	switch (selector)
		{
		
		case acquireSelectorPrepare:
			DoPrepare (globals);
			break;
		
		case acquireSelectorStart:
			DoStartVariance (globals, kKurtosis);
			break;
		
		case acquireSelectorContinue:
			DoContinueVariance (globals, kKurtosis);
			break;
		
		case acquireSelectorFinish:
			DoFinishVariance (globals);
			break;
			
		case acquireSelectorFinalize:
			DoFinalize (globals);
			break;
		
		default:
			gResult = acquireBadParameters;
		
		}
		
	*result = gResult;	
		
	} // end try

	catch (...)
	{
		if (NULL != result)
		{
			*result = -1;
		}
	}

	}

/*****************************************************************************/

DLLExport MACPASCAL
void RANGE_ENTRY (const short selector,
				  AcquireRecord *acquireParamBlock,
				  intptr_t *data,
				  int16 *result)
	{

	try {
		
	if (selector == acquireSelectorAbout)
		{
		sSPBasic = ( ( AboutRecord *) acquireParamBlock)->sSPBasic;
		DoAbout(( AboutRecord *) acquireParamBlock);
		return;
		}
	
	sSPBasic = acquireParamBlock->sSPBasic;

	if (!*data)
		{
		
		*data = (intptr_t)malloc (sizeof (Globals));
		
		if (!*data)
			{
			*result = memFullErr;
			return;
			}
			
		InitGlobals ((GPtr)*data);

		}
		
	GPtr globals = (GPtr)*data;
	
	gStuff = acquireParamBlock;
	gResult = noErr;
	
	switch (selector)
		{
		
		case acquireSelectorPrepare:
			DoPrepare (globals);
			break;
		
		case acquireSelectorStart:
			DoStartMinMax (globals);
			break;
		
		case acquireSelectorContinue:
			DoContinueMinMax (globals, kRange);
			break;
		
		case acquireSelectorFinish:
			DoFinishMinMax (globals);
			break;
			
		case acquireSelectorFinalize:
			DoFinalize (globals);
			break;
		
		default:
			gResult = acquireBadParameters;
		
		}
		
	*result = gResult;	
		
	} // end try

	catch (...)
	{
		if (NULL != result)
		{
			*result = -1;
		}
	}

	}

/*****************************************************************************/

DLLExport MACPASCAL
void ENTROPY_ENTRY (const short selector,
					AcquireRecord *acquireParamBlock,
					intptr_t *data,
					int16 *result)
	{

	try {
		
	if (selector == acquireSelectorAbout)
		{
		sSPBasic = ( ( AboutRecord *) acquireParamBlock)->sSPBasic;
		DoAbout(( AboutRecord *) acquireParamBlock);
		return;
		}
	
	sSPBasic = acquireParamBlock->sSPBasic;

	if (!*data)
		{
		
		*data = (intptr_t)malloc (sizeof (Globals));
		
		if (!*data)
			{
			*result = memFullErr;
			return;
			}
			
		InitGlobals ((GPtr)*data);

		}
		
	GPtr globals = (GPtr)*data;
	
	gStuff = acquireParamBlock;
	gResult = noErr;
	
	switch (selector)
		{
		
		case acquireSelectorPrepare:
			DoPrepare (globals);
			break;
		
		case acquireSelectorStart:
			DoStartVariance (globals, kEntropy);
			break;
		
		case acquireSelectorContinue:
			DoContinueVariance (globals, kEntropy);
			break;
		
		case acquireSelectorFinish:
			DoFinishVariance (globals);
			break;
			
		case acquireSelectorFinalize:
			DoFinalize (globals);
			break;
		
		default:
			gResult = acquireBadParameters;
		
		}
		
	*result = gResult;	
		
	} // end try

	catch (...)
	{
		if (NULL != result)
		{
			*result = -1;
		}
	}

	}

/*****************************************************************************/

/* We can initialize our globals here, but since we don't know if we'll always
   get this call, it's better to flag them and check that they're valid before
   we first use them. */

void InitGlobals (GPtr globals)
{

	gLastRows = 512;
	gLastCols = 512;

	gOutputDepth = 8;
	
	gTileSize = 64;
	
	gInitParameters = false;
	
}

/*****************************************************************************/

/* Check parameters and make sure their valid, or initialize them. */

void ValidateParameters (GPtr globals)
{

	if (gLastRows < kRowsMin)
		gLastRows = kRowsMin;
	else if (gLastRows > kRowsMax)
		gLastRows = kRowsMax;

	if (gLastCols < kColumnsMin)
		gLastCols = kColumnsMin;
	else if (gLastCols > kColumnsMax)
		gLastCols = kColumnsMax;

	if (gOutputDepth != 8 && gOutputDepth != 16 && gOutputDepth != 32)
		gOutputDepth = 8;

}

/*****************************************************************************/

/* Prepare to acquire an image.	If the plug-in module needs a large amount
   of buffer memory, this routine should set the maxData field to the
   number of bytes required.  Since we are going to use the bufferProcs,
   we simply set maxData to zero.  */

void DoPrepare (GPtr globals)
	{
	
	// save this off , so we have some idea of the available RAM
	gMaxData = gStuff->maxData;
	
	// but we don't want Photoshop to free up anything
	// because we'll be using the Photoshop buffer suite
	gStuff->maxData = 0;
	
	if (!WarnAdvanceStateAvailable ())
		gResult = acquireBadParameters;

	if (!WarnBufferProcsAvailable ())
		gResult = acquireBadParameters;

	gStuff->wantFinalize = false;
}

/*****************************************************************************/
/*****************************************************************************/

int32 GetCompositeChannelCount(GPtr globals)
	{

#if 1
	return gStuff->documentInfo->compositeChannelCount;
#else
	int32 count = 0;
	
	ReadChannelDesc *currentChannelDesc = gStuff->documentInfo->mergedCompositeChannels;
	
	while (currentChannelDesc != NULL)
		{
		count++;
		currentChannelDesc = currentChannelDesc->next;
		}
	
	return count;
#endif
	}

/*****************************************************************************/

int32 GetAlphaChannelCount(GPtr globals)
	{
#if 1
	return gStuff->documentInfo->alphaChannelCount;
#else
	// for now, we have to iterate the channels
	
	int32 count = 0;
	
	ReadChannelDesc *currentChannelDesc = gStuff->documentInfo->alphaChannels;
	
	while (currentChannelDesc != NULL)
		{
		count++;
		currentChannelDesc = currentChannelDesc->next;
		}
	
	return count;
#endif
	}

/*****************************************************************************/

bool HasSelectionMask(GPtr globals)
	{
	return gStuff->documentInfo->selection != NULL;
	}

/*****************************************************************************/

bool TargetHasTransparency(GPtr globals)
	{
	return gStuff->documentInfo->targetTransparency != NULL;
	}

/*****************************************************************************/

bool TargetHasLayerMask(GPtr globals)
	{
	return gStuff->documentInfo->targetLayerMask != NULL;
	}

/*****************************************************************************/

bool MergedHasTransparency(GPtr globals)
	{
	return gStuff->documentInfo->mergedTransparency != NULL;
	}

/*****************************************************************************/

// a result of zero means that you only have one layer selected and access it through composite data
int32 GetLayerCount(GPtr globals)
	{
#if 1
	return gStuff->documentInfo->layerCount;
#else
	
	int32 count = 0;
	
	ReadLayerDesc *currentLayerDesc = gStuff->documentInfo->layersDescriptor;
	
	while (currentLayerDesc != NULL)
		{
		count++;
		currentLayerDesc = currentLayerDesc->next;
		}
	
	return count;
#endif
	}

/*****************************************************************************/

// a result of zero means that you only have one layer selected and access it through composite data
// OR there are no visible layers - in either case, just use the composite
int32 GetVisibleLayerCount(GPtr globals)
	{
	
	int32 count = 0;
	
	ReadLayerDesc *currentLayerDesc = gStuff->documentInfo->layersDescriptor;
	
	while (currentLayerDesc != NULL)
		{
		
		if (currentLayerDesc->isVisible
			&& currentLayerDesc->isPixelBased)
			count++;
		
		currentLayerDesc = currentLayerDesc->next;
		}
	
	return count;

	}

/*****************************************************************************/

bool LayerHasLayerMask(ReadLayerDesc *layerDesc)
	{
	return layerDesc->layerMask != NULL;
	}

/*****************************************************************************/

bool LayerHasTransparency(ReadLayerDesc *layerDesc)
	{
	return layerDesc->transparency != NULL;
	}

/*****************************************************************************/

bool LayerIsPixelBased(ReadLayerDesc *layerDesc)
	{
	return layerDesc->isPixelBased != 0;
	}

/*****************************************************************************/

bool LayerIsVisible(ReadLayerDesc *layerDesc)
	{
	return layerDesc->isVisible != 0;
	}

/*****************************************************************************/

unsigned32 GetLayerSheetID(ReadLayerDesc *layerDesc)
	{
	return layerDesc->sheetID;
	}

/*****************************************************************************/

const char * GetLayerName(ReadLayerDesc *layerDesc)
	{
	return layerDesc->name;
	}

/*****************************************************************************/

const uint16 * GetLayerUnicodeName(ReadLayerDesc *layerDesc)
	{
	return layerDesc->unicodeName;
	}

/*****************************************************************************/

OSErr ReadChannelPixelsIntoPlanarBuffer( GPtr globals,
										ReadChannelDesc *currentChannelDesc,
										void *buffer,
										VRect &theRect,
										int32 rowBytes,
										int16 depth )
	{
		
	PSScaling scaling;
	PixelMemoryDesc destination;
	VRect writeRect, wroteRect;
	
	scaling.sourceRect = scaling.destinationRect = theRect;
	
	destination.data = buffer;
	destination.rowBits = rowBytes * 8;
	destination.colBits = depth;
	destination.bitOffset = 0;
	destination.depth = depth;
	
	writeRect = theRect;
	
	OSErr result = gStuff->channelPortProcs->readPixelsProc ( currentChannelDesc->port,
								&scaling,
								&writeRect,
								&destination,
								&wroteRect );
	
	theRect = wroteRect;
	
	return result;
	}

/*****************************************************************************/

OSErr FakeOpaqueIntoPlanarBuffer( GPtr globals,
								ReadChannelDesc * /* currentChannelDesc */,
								void *buffer,
								VRect &theRect,
								int32 rowBytes,
								int16 depth )
	{
	int32 rows = theRect.bottom - theRect.top;
	int32 cols = theRect.right - theRect.left;
	int32 ROW, COL;
	unsigned char *outputData = (unsigned char *)buffer;
		
	for (ROW = 0; ROW < rows; ++ROW)
		{
		
		if (depth == 8)
			{
			unsigned char *bytePtr = (unsigned char *)outputData;
			for (COL = 0; COL < cols; ++COL)
				bytePtr[COL] = 255;
			}
		else if (depth == 16)
			{
			unsigned short *shortPtr = (unsigned short *)outputData;
			for (COL = 0; COL < cols; ++COL)
				shortPtr[COL] = 32768;
			}
		else if (depth == 32)
			{
			float *floatPtr = (float *)outputData;
			for (COL = 0; COL < cols; ++COL)
				floatPtr[COL] = 1.0f;
			}
		
		outputData += rowBytes;
		}
	
	return noErr;
	}

/******************************************************************************/

inline unsigned8 Mul8x8Div255 (unsigned8 a, unsigned8 b)
	{
	unsigned32 temp = (unsigned32)a * (unsigned32)b + 128;
	return static_cast<unsigned8>((temp + (temp >> 8)) >> 8);
	}

/******************************************************************************/

inline unsigned16 Mul16x16Div32768 (unsigned16 a, unsigned16 b)
	{
	unsigned32 temp = (unsigned32)a * (unsigned32)b + 16384;
	return (unsigned16) (temp >> 15);
	}

/******************************************************************************/

inline unsigned8 Screen8x8(unsigned8 a, unsigned8 b)
	{
	return (unsigned8)((int32)a + (int32)b - (int32)Mul8x8Div255(a, b));
	}

/******************************************************************************/

inline unsigned16 Screen16x16( unsigned16 a, unsigned16 b)
	{
	return (unsigned16)((int32)a + (int32)b - (((int32)a * b + 16384) >> 15));
	}

/******************************************************************************/

inline float Mul32x32(float a, float b)
	{
	return a * b;
	}

/******************************************************************************/

inline float Screen32x32( float a, float b)
	{
	//return 1.0 - ((1.0 - a) * (1.0 - b));
	return (a + b) - (a * b);
	}

/*****************************************************************************/

OSErr MergeTransparencyIntoPlanarBuffer( GPtr globals,
								void *dstBuffer,
								void *tempBuffer,
								VRect &theRect,
								int32 rowBytes,
								int16 depth )
	{
	int32 rows = theRect.bottom - theRect.top;
	int32 cols = theRect.right - theRect.left;
	int32 ROW, COL;
	unsigned char *outputData = (unsigned char *)dstBuffer;
	unsigned char *inputData = (unsigned char *)tempBuffer;
	
	for (ROW = 0; ROW < rows; ++ROW)
		{
		
		if (depth == 8)
			{
			unsigned char *outBytePtr = (unsigned char *)outputData;
			unsigned char *inBytePtr = (unsigned char *)inputData;
			for (COL = 0; COL < cols; ++COL)
				outBytePtr[COL] = Mul8x8Div255( outBytePtr[COL], inBytePtr[COL] );
			}
		else if (depth == 16)
			{
			unsigned short *outShortPtr = (unsigned short *)outputData;
			unsigned short *inShortPtr = (unsigned short *)inputData;
			for (COL = 0; COL < cols; ++COL)
				outShortPtr[COL] = Mul16x16Div32768( outShortPtr[COL], inShortPtr[COL] );
			}
		else if (depth == 32)
			{
			float *outFloatPtr = (float *)outputData;
			float *inFloatPtr = (float *)inputData;
			for (COL = 0; COL < cols; ++COL)
				outFloatPtr[COL] = Mul32x32( outFloatPtr[COL], inFloatPtr[COL] );
			}
		
		outputData += rowBytes;
		inputData += rowBytes;
		}
	
	return noErr;
	}

/*****************************************************************************/

OSErr ReadChannelPixelsIntoInterleavedBuffer( GPtr globals,
										ReadChannelDesc *currentChannelDesc,
										void *buffer,
										VRect rect,
										int32 rowBytes,
										int32 colBytes,
										int16 depth,
										int16 channel )
	{
		
	PSScaling scaling;
	PixelMemoryDesc destination;
	VRect writeRect, wroteRect;
	
	scaling.sourceRect = scaling.destinationRect = rect;
	
	destination.data = buffer;
	destination.rowBits = rowBytes * 8;
	destination.colBits = colBytes * 8;
	destination.bitOffset = channel * depth;
	destination.depth = depth;
	
	writeRect = rect;
	
	OSErr result = gStuff->channelPortProcs->readPixelsProc( currentChannelDesc->port,
								&scaling,
								&writeRect,
								&destination,
								&wroteRect );
	
	return result;
	}

/*****************************************************************************/

OSErr StoreData (AcquireRecord *stuff,
				 void *data,
				 AcquireRegion *region,
				 AcquireDataLayout *layout)
	{
	
	OSErr result = noErr;
		
	stuff->data = data;
	
	stuff->theRect.top = (short)region->rect.top;
	stuff->theRect.left = (short)region->rect.left;
	stuff->theRect.bottom = (short)region->rect.bottom;
	stuff->theRect.right = (short)region->rect.right;
	
	if (stuff->HostSupports32BitCoordinates)
		{
		stuff->theRect32 = region->rect;
		}
	
	stuff->loPlane = region->loPlane;
	stuff->hiPlane = region->hiPlane;
	stuff->rowBytes = layout->rowBytes;
	stuff->colBytes = (short)layout->colBytes;
	stuff->planeBytes = layout->planeBytes;
	
	stuff->wantReadBack = false;
	
	result = (*(stuff->advanceState)) ();
	
	stuff->data = NULL;
	
	return result;
	
	}

/*****************************************************************************/

#define RANGE_ITER(lower,upper,first,last,step)								  \
	for (lower = (first);                         							  \
		 (upper = (((lower) + (step) < (last)) ? (lower) + (step) : (last))), \
		 lower < (last);													  \
		 lower = upper)

/*****************************************************************************/


/* replace the system call with a macro */
#undef SetRect
#define SetRect(rect,l,t,r,b)	(rect)->top=(t); (rect)->left=(l); (rect)->bottom=(b); (rect)->right=(r);


/*****************************************************************************/

// NOTE - ccox - this should call the bottleneck
static void DoConvertDepth16to32(Ptr inputData,
					Ptr outputData,
					int32 rows,
					int32 cols,
					int32 inputRowBytes,
					int32 outputRowBytes)
	{
	int32 ROW, COL;
		
	for (ROW = 0; ROW < rows; ++ROW)
		{
		unsigned short *inPtr = (unsigned short *)inputData;
		float *outPtr = (float *)outputData;
		
		for (COL = 0; COL < cols; ++COL)
			{
			outPtr[COL] = ((float) inPtr[COL]) * (1.0f / 32768.0f);
			}
		
		inputData += inputRowBytes;
		outputData += outputRowBytes;
		}
	
	}

/*****************************************************************************/

// NOTE - ccox - this should call the bottleneck
static void DoConvertDepth8to32(Ptr inputData,
					Ptr outputData,
					int32 rows,
					int32 cols,
					int32 inputRowBytes,
					int32 outputRowBytes)
	{
	int32 ROW, COL;
		
	for (ROW = 0; ROW < rows; ++ROW)
		{
		unsigned char *inPtr = (unsigned char *)inputData;
		float *outPtr = (float *)outputData;
		
		for (COL = 0; COL < cols; ++COL)
			{
			outPtr[COL] = ((float) inPtr[COL]) * (1.0f / 255.0f);
			}
		
		inputData += inputRowBytes;
		outputData += outputRowBytes;
		}
	
	}

/*****************************************************************************/

inline unsigned char PinU8( int32 x )
	{
	if (x < 0)
		return 0;
	else if (x > 255)
		return 255;
	else
		return (unsigned char)x;
	}

/*****************************************************************************/

inline unsigned short PinU16( int32 x )
	{
	if (x < 0)
		return 0;
	else if (x > 32768)
		return 32768;
	else
		return (unsigned short)x;
	}

/*****************************************************************************/

// NOTE - ccox - this should call the bottleneck
static void DoConvertDepth32to8(Ptr inputData,
					Ptr outputData,
					int32 rows,
					int32 cols,
					int32 inputRowBytes,
					int32 outputRowBytes)
	{
	int32 ROW, COL;
		
	for (ROW = 0; ROW < rows; ++ROW)
		{
		float *inPtr = (float *)inputData;
		unsigned char *outPtr = (unsigned char *)outputData;
		
		for (COL = 0; COL < cols; ++COL)
			{
			outPtr[COL] = PinU8( (int32)(inPtr[COL] * 255.0f + 0.5) );
			}
		
		inputData += inputRowBytes;
		outputData += outputRowBytes;
		}
	
	}

/*****************************************************************************/

// NOTE - ccox - this should call the bottleneck
static void DoConvertDepth32to16(Ptr inputData,
					Ptr outputData,
					int32 rows,
					int32 cols,
					int32 inputRowBytes,
					int32 outputRowBytes)
	{
	int32 ROW, COL;
	
	for (ROW = 0; ROW < rows; ++ROW)
		{
		float *inPtr = (float *)inputData;
		unsigned short *outPtr = (unsigned short *)outputData;
		
		for (COL = 0; COL < cols; ++COL)
			{
			outPtr[COL] = PinU16( (int32)(inPtr[COL] * 32768.0f + 0.5) );
			}
		
		inputData += inputRowBytes;
		outputData += outputRowBytes;
		}
	
	}

/*****************************************************************************/

// simple, unoptimized copy
// NOTE - ccox - this should call the bottleneck
static void DoMoveRect(Ptr inputData,
					Ptr outputData,
					int32 rows,
					int32 cols,
					int32 inputRowBytes,
					int32 outputRowBytes)
	{
	int32 ROW, COL;
		
	for (ROW = 0; ROW < rows; ++ROW)
		{
		unsigned char *inPtr = (unsigned char *)inputData;
		unsigned char *outPtr = (unsigned char *)outputData;
		
		for (COL = 0; COL < cols; ++COL)
			{
			outPtr[COL] = inPtr[COL];
			}
		
		inputData += inputRowBytes;
		outputData += outputRowBytes;
		}
	
	}

/*****************************************************************************/

OSErr DoConvertDepth( Ptr inputData,
					Ptr outputData,
					VRect writeRect,
					int32 inputRowBytes,
					int32 outputRowBytes,
					int16 inputDepth,
					int16 outputDepth )
	{
	int32 rows = writeRect.bottom - writeRect.top;
	int32 cols = writeRect.right - writeRect.left;
	
	if (inputDepth == outputDepth)
		{
		DoMoveRect( inputData, outputData, rows, cols, inputRowBytes, outputRowBytes );
		return noErr;
		}
	
	
	if (outputDepth == 32)
		{
		
		if (inputDepth == 8)
			{
			DoConvertDepth8to32( inputData, outputData, rows, cols, inputRowBytes, outputRowBytes );
			return noErr;
			}
		else if (inputDepth == 16)
			{
			DoConvertDepth16to32( inputData, outputData, rows, cols, inputRowBytes, outputRowBytes );
			return noErr;
			}
		else
			{
			return 1;
			}
		}
	else
		{
		return 1;
		}
	}

/*****************************************************************************/

static void DoAddBuffer8to32(Ptr inputData,
					Ptr transData,
					Ptr outputData,
					int32 rows,
					int32 cols,
					int32 inputRowBytes,
					int32 outputRowBytes)
	{
	int32 ROW, COL;
		
	for (ROW = 0; ROW < rows; ++ROW)
		{
		unsigned char *inPtr = (unsigned char *)inputData;
		unsigned char *transPtr = (unsigned char *)transData;
		float *outPtr = (float *)outputData;
		
		for (COL = 0; COL < cols; ++COL)
			{
			if (transPtr[COL] >= 128)
				outPtr[COL] += ((float) inPtr[COL]) * (1.0f / 255.0f);
			}
		
		inputData += inputRowBytes;
		transData += inputRowBytes;
		outputData += outputRowBytes;
		}
	
	}

/*****************************************************************************/

static void DoAddBuffer16to32(Ptr inputData,
					Ptr transData,
					Ptr outputData,
					int32 rows,
					int32 cols,
					int32 inputRowBytes,
					int32 outputRowBytes)
	{
	int32 ROW, COL;
		
	for (ROW = 0; ROW < rows; ++ROW)
		{
		unsigned short *inPtr = (unsigned short *)inputData;
		unsigned short *transPtr = (unsigned short *)transData;
		float *outPtr = (float *)outputData;
		
		for (COL = 0; COL < cols; ++COL)
			{
			if (transPtr[COL] >= 16384)
				outPtr[COL] += ((float) inPtr[COL]) * (1.0f / 32768.0f);
			}
		
		inputData += inputRowBytes;
		transData += inputRowBytes;
		outputData += outputRowBytes;
		}
	
	}

/*****************************************************************************/

static void DoAddBuffer32to32(Ptr inputData,
					Ptr transData,
					Ptr outputData,
					int32 rows,
					int32 cols,
					int32 inputRowBytes,
					int32 outputRowBytes)
	{
	int32 ROW, COL;
		
	for (ROW = 0; ROW < rows; ++ROW)
		{
		float *inPtr = (float *)inputData;
		float *transPtr = (float *)transData;
		float *outPtr = (float *)outputData;
		
		for (COL = 0; COL < cols; ++COL)
			{
			if (transPtr[COL] >= 0.5f)
				outPtr[COL] += inPtr[COL];
			}
		
		inputData += inputRowBytes;
		transData += inputRowBytes;
		outputData += outputRowBytes;
		}
	
	}

/*****************************************************************************/

// this requires that outputDepth is 32
// this is a simple example because we can use an accumulation buffer
OSErr DoAddLayers( Ptr inputData,
					Ptr transData,
					Ptr outputData,
					VRect writeRect,
					int32 inputRowBytes,
					int32 outputRowBytes,
					int16 inputDepth,
					int16 outputDepth,
					int32 layerCount,
					int32 inputLayerBytes )
	{
	int32 rows = writeRect.bottom - writeRect.top;
	int32 cols = writeRect.right - writeRect.left;
	int32 LAYER;
	
	if (outputDepth != 32)
		return 2;
	
	// zero the output buffer
	memset( outputData, 0, rows * outputRowBytes );
	
	// add each layer into the output
	for (LAYER = 0; LAYER < layerCount; ++LAYER)
		{
		
		if (inputDepth == 8)
			{
			DoAddBuffer8to32( inputData, transData, outputData, rows, cols, inputRowBytes, outputRowBytes );
			}
		else if (inputDepth == 16)
			{
			DoAddBuffer16to32( inputData, transData, outputData, rows, cols, inputRowBytes, outputRowBytes );
			}
		else if (inputDepth == 32)
			{
			DoAddBuffer32to32( inputData, transData, outputData, rows, cols, inputRowBytes, outputRowBytes );
			}
		else
			{
			return 1;
			}
		
		// move on to the next layer
		inputData += inputLayerBytes;
		transData += inputLayerBytes;
		
		}
	
	return noErr;
	}

/*****************************************************************************/

static void DoAddAndCountBuffer8to32(Ptr inputData,
					Ptr transData,
					Ptr outputData,
					Ptr countData,
					int32 rows,
					int32 cols,
					int32 inputRowBytes,
					int32 outputRowBytes)
	{
	int32 ROW, COL;
		
	for (ROW = 0; ROW < rows; ++ROW)
		{
		unsigned char *inPtr = (unsigned char *)inputData;
		unsigned char *transPtr = (unsigned char *)transData;
		float *outPtr = (float *)outputData;
		unsigned long *countPtr = (unsigned long *)countData;
		
		for (COL = 0; COL < cols; ++COL)
			{
			if (transPtr[COL] >= 128)
				{
				countPtr[COL] += 1;
				outPtr[COL] += ((float) inPtr[COL]) * (1.0f / 255.0f);
				}
			}
		
		inputData += inputRowBytes;
		transData += inputRowBytes;
		outputData += outputRowBytes;
		countData += outputRowBytes;
		}
	
	}

/*****************************************************************************/

static void DoAddAndCountBuffer16to32(Ptr inputData,
					Ptr transData,
					Ptr outputData,
					Ptr countData,
					int32 rows,
					int32 cols,
					int32 inputRowBytes,
					int32 outputRowBytes)
	{
	int32 ROW, COL;
		
	for (ROW = 0; ROW < rows; ++ROW)
		{
		unsigned short *inPtr = (unsigned short *)inputData;
		unsigned short *transPtr = (unsigned short *)transData;
		float *outPtr = (float *)outputData;
		unsigned long *countPtr = (unsigned long *)countData;
		
		for (COL = 0; COL < cols; ++COL)
			{
			if (transPtr[COL] >= 16384)
				{
				countPtr[COL] += 1;
				outPtr[COL] += ((float) inPtr[COL]) * (1.0f / 32768.0f);
				}
			}
		
		inputData += inputRowBytes;
		transData += inputRowBytes;
		outputData += outputRowBytes;
		countData += outputRowBytes;
		}
	
	}

/*****************************************************************************/

static void DoAddAndCountBuffer32to32(Ptr inputData,
					Ptr transData,
					Ptr outputData,
					Ptr countData,
					int32 rows,
					int32 cols,
					int32 inputRowBytes,
					int32 outputRowBytes)
	{
	int32 ROW, COL;
		
	for (ROW = 0; ROW < rows; ++ROW)
		{
		float *inPtr = (float *)inputData;
		float *transPtr = (float *)transData;
		float *outPtr = (float *)outputData;
		unsigned long *countPtr = (unsigned long *)countData;
		
		for (COL = 0; COL < cols; ++COL)
			{
			if (transPtr[COL] >= 0.5f)
				{
				countPtr[COL] += 1;
				outPtr[COL] += inPtr[COL];
				}
			}
		
		inputData += inputRowBytes;
		transData += inputRowBytes;
		outputData += outputRowBytes;
		countData += outputRowBytes;
		}
	
	}

/*****************************************************************************/

static void DoScaleBuffer32( Ptr outputData,
							Ptr countData,
							int32 rows,
							int32 cols,
							int32 outputRowBytes)
	{
	int32 ROW, COL;
		
	for (ROW = 0; ROW < rows; ++ROW)
		{
		float *outPtr = (float *)outputData;
		unsigned long *countPtr = (unsigned long *)countData;
		
		for (COL = 0; COL < cols; ++COL)
			{
			outPtr[COL] /= (float)countPtr[COL];
			}
		
		outputData += outputRowBytes;
		countData += outputRowBytes;
		}
	
	}

/*****************************************************************************/

// this requires that the output buffer hold 32 bits of data per pixel
// and it requires a count buffer, also 32 bits per pixel

OSErr DoAverageLayers( Ptr inputData,
					Ptr transData,
					Ptr outputData,
					Ptr countData,
					VRect writeRect,
					int32 inputRowBytes,
					int32 outputRowBytes,
					int16 inputDepth,
					int16 outputDepth,
					int32 layerCount,
					int32 inputLayerBytes )
	{
	int32 rows = writeRect.bottom - writeRect.top;
	int32 cols = writeRect.right - writeRect.left;
	int32 LAYER;
	
	
	// zero the output buffer and count buffer (they're the same size)
	memset( outputData, 0, rows * outputRowBytes );
	memset( countData, 0, rows * outputRowBytes );
	
	// add each layer into the output and count
	for (LAYER = 0; LAYER < layerCount; ++LAYER)
		{
		
		if (inputDepth == 8)
			{
			DoAddAndCountBuffer8to32( inputData, transData, outputData, countData, rows, cols, inputRowBytes, outputRowBytes );
			}
		else if (inputDepth == 16)
			{
			DoAddAndCountBuffer16to32( inputData, transData, outputData, countData, rows, cols, inputRowBytes, outputRowBytes );
			}
		else if (inputDepth == 32)
			{
			DoAddAndCountBuffer32to32( inputData, transData, outputData, countData, rows, cols, inputRowBytes, outputRowBytes );
			}
		else
			{
			return 1;
			}
		
		// move on to the next layer
		inputData += inputLayerBytes;
		transData += inputLayerBytes;
		
		}
	
	// now divide by the count
	DoScaleBuffer32( outputData, countData, rows, cols, outputRowBytes );
	
	// inplace, convert depth down as needed for output
	if (outputDepth == 16)
		DoConvertDepth32to16( outputData, outputData, rows, cols, outputRowBytes, outputRowBytes );
	else if (outputDepth == 8)
		DoConvertDepth32to8( outputData, outputData, rows, cols, outputRowBytes, outputRowBytes );
	
	return noErr;
	}

/*****************************************************************************/

static void DoMinMaxBuffer8(Ptr inputData,
					Ptr transData,
					Ptr outputData,
					int32 rows,
					int32 cols,
					int32 inputRowBytes,
					int32 outputRowBytes,
					bool isMaximum)
	{
	int32 ROW, COL;
		
	for (ROW = 0; ROW < rows; ++ROW)
		{
		unsigned char *inPtr = (unsigned char *)inputData;
		unsigned char *transPtr = (unsigned char *)transData;
		unsigned char *outPtr = (unsigned char *)outputData;
		
		if (isMaximum)
			for (COL = 0; COL < cols; ++COL)
				{
				if (transPtr[COL] >= 128)
					if (inPtr[COL] > outPtr[COL])
						outPtr[COL] = inPtr[COL];
				}
		else
			for (COL = 0; COL < cols; ++COL)
				{
				if (transPtr[COL] >= 128)
					if (inPtr[COL] < outPtr[COL])
						outPtr[COL] = inPtr[COL];
				}
		
		inputData += inputRowBytes;
		transData += inputRowBytes;
		outputData += outputRowBytes;
		}
	
	}

/*****************************************************************************/

static void DoMinMaxBuffer16(Ptr inputData,
					Ptr transData,
					Ptr outputData,
					int32 rows,
					int32 cols,
					int32 inputRowBytes,
					int32 outputRowBytes,
					bool isMaximum)
	{
	int32 ROW, COL;
		
	for (ROW = 0; ROW < rows; ++ROW)
		{
		unsigned short *inPtr = (unsigned short *)inputData;
		unsigned short *transPtr = (unsigned short *)transData;
		unsigned short *outPtr = (unsigned short *)outputData;
		
		if (isMaximum)
			for (COL = 0; COL < cols; ++COL)
				{
				if (transPtr[COL] >= 16384)
					if (inPtr[COL] > outPtr[COL])
						outPtr[COL] = inPtr[COL];
				}
		else
			for (COL = 0; COL < cols; ++COL)
				{
				if (transPtr[COL] >= 16384)
					if (inPtr[COL] < outPtr[COL])
						outPtr[COL] = inPtr[COL];
				}
		
		inputData += inputRowBytes;
		transData += inputRowBytes;
		outputData += outputRowBytes;
		}
	
	}

/*****************************************************************************/

static void DoMinMaxBuffer32(Ptr inputData,
					Ptr transData,
					Ptr outputData,
					int32 rows,
					int32 cols,
					int32 inputRowBytes,
					int32 outputRowBytes,
					bool isMaximum)
	{
	int32 ROW, COL;
		
	for (ROW = 0; ROW < rows; ++ROW)
		{
		float *inPtr = (float *)inputData;
		float *transPtr = (float *)transData;
		float *outPtr = (float *)outputData;
		
		if (isMaximum)
			for (COL = 0; COL < cols; ++COL)
				{
				if (transPtr[COL] >= 0.5f)
					if (inPtr[COL] > outPtr[COL])
						outPtr[COL] = inPtr[COL];
				}
		else
			for (COL = 0; COL < cols; ++COL)
				{
				if (transPtr[COL] >= 0.5f)
					if (inPtr[COL] < outPtr[COL])
						outPtr[COL] = inPtr[COL];
				}
		
		inputData += inputRowBytes;
		transData += inputRowBytes;
		outputData += outputRowBytes;
		}
	
	}

/*****************************************************************************/

// this requires that outputDepth is 32
// this is a simple example because we can use an accumulation buffer
OSErr DoMinMaxLayers( Ptr inputData,
					Ptr transData,
					Ptr outputData,
					VRect writeRect,
					int32 inputRowBytes,
					int32 outputRowBytes,
					int16 inputDepth,
					int16 outputDepth,
					int32 layerCount,
					int32 inputLayerBytes,
					bool isMaximum )
	{
	int32 rows = writeRect.bottom - writeRect.top;
	int32 cols = writeRect.right - writeRect.left;
	int32 LAYER;
	
	if (outputDepth != inputDepth)
		return 2;
	
	// set the output data to black or white
	int value = isMaximum ? 0 : 255;
	memset( outputData, value, rows * outputRowBytes );
	
	
	// merge layers into the output
	for (LAYER = 0; LAYER < layerCount; ++LAYER)
		{
		
		if (inputDepth == 8)
			{
			DoMinMaxBuffer8( inputData, transData, outputData, rows, cols, inputRowBytes, outputRowBytes, isMaximum );
			}
		else if (inputDepth == 16)
			{
			DoMinMaxBuffer16( inputData, transData, outputData, rows, cols, inputRowBytes, outputRowBytes, isMaximum );
			}
		else if (inputDepth == 32)
			{
			DoMinMaxBuffer32( inputData, transData, outputData, rows, cols, inputRowBytes, outputRowBytes, isMaximum );
			}
		else
			{
			return 1;
			}
		
		// move on to the next layer
		inputData += inputLayerBytes;
		transData += inputLayerBytes;
		
		}
	
	return noErr;
	}

/*****************************************************************************/

static void DoSquareRootBuffer( Ptr outputData,
							VRect writeRect,
							int32 outputRowBytes,
							int32 outputDepth)
	{
	int32 rows = writeRect.bottom - writeRect.top;
	int32 cols = writeRect.right - writeRect.left;
	int32 ROW, COL;
	
	
	for (ROW = 0; ROW < rows; ++ROW)
		{
		float *outPtr = (float *)outputData;
		
		for (COL = 0; COL < cols; ++COL)
			{
			float oldValue = outPtr[COL];
			if (oldValue >= 0.0f)
				outPtr[COL] = (float)sqrtf(oldValue);
			else
				outPtr[COL] = 0.0f;
			}
		
		outputData += outputRowBytes;
		}
	
	}

/*****************************************************************************/

static float DoPartialVariance8to32( Ptr inputData,
									 Ptr transData,
									int32 layerCount,
									int32 inputLayerBytes,
									int32 col,
									float mean)
	{
	unsigned char *inPtr = (unsigned char *)inputData + col;
	unsigned char *transPtr = (unsigned char *)transData + col;
	float variance = 0.0f;
	int32 LAYER;
	
	for (LAYER = 0; LAYER < layerCount; ++LAYER)
		{
		if (transPtr[LAYER*inputLayerBytes] >= 128)
			{
			float value = ((float)inPtr[LAYER*inputLayerBytes]) * (1.0f / 255.0f);
			
			float diff = value - mean;
			
			variance += (diff * diff);
			}
		}
	
	return variance;
	
	}

/*****************************************************************************/

static float DoPartialVariance16to32( Ptr inputData,
									 Ptr transData,
									int32 layerCount,
									int32 inputLayerBytes,
									int32 col,
									float mean)
	{
	unsigned short *inPtr = (unsigned short *)inputData + col;
	unsigned short *transPtr = (unsigned short *)transData + col;
	float variance = 0.0f;
	int32 LAYER;
	
	for (LAYER = 0; LAYER < layerCount; ++LAYER)
		{
		if (transPtr[LAYER*(inputLayerBytes>>1)] >= 16384)
			{
			float value = ((float)inPtr[LAYER*(inputLayerBytes>>1)]) * (1.0f / 32768.0f);
			
			float diff = value - mean;
			
			variance += (diff * diff);
			}
		}
	
	return variance;
	
	}

/*****************************************************************************/

static float DoPartialVariance32to32( Ptr inputData,
									 Ptr transData,
									int32 layerCount,
									int32 inputLayerBytes,
									int32 col,
									float mean)
	{
	float *inPtr = (float *)inputData + col;
	float *transPtr = (float *)transData + col;
	float variance = 0.0f;
	int32 LAYER;
	
	for (LAYER = 0; LAYER < layerCount; ++LAYER)
		{
		if (transPtr[LAYER*(inputLayerBytes>>2)] >= 0.5f)
			{
			float value = inPtr[LAYER*(inputLayerBytes>>2)];
			
			float diff = value - mean;
			
			variance += (diff * diff);
			}
		}
	
	return variance;
	
	}

/*****************************************************************************/

// NOTE - ccox - there really should be a better way to do this calculation
// but, as far as I can tell, it would need more buffers
static void DoVariance32(Ptr inputData,
					Ptr transData,
					Ptr outputData,
					Ptr countData,
					int32 rows,
					int32 cols,
					int16 inputDepth,
					int32 inputRowBytes,
					int32 outputRowBytes,
					int32 layerCount,
					int32 inputLayerBytes)
	{
	int32 ROW, COL;
		
	for (ROW = 0; ROW < rows; ++ROW)
		{
		float *outPtr = (float *)outputData;
		unsigned long *countPtr = (unsigned long *)countData;
		
		for (COL = 0; COL < cols; ++COL)
			{
			float variance = 0.0f;
			float mean = outPtr[COL];
			unsigned long thisCount = countPtr[COL];
			
			if (inputDepth == 8)
				variance = DoPartialVariance8to32( inputData,
								transData, layerCount, inputLayerBytes, COL, mean);
			else if (inputDepth == 16)
				variance = DoPartialVariance16to32( inputData,
								transData, layerCount, inputLayerBytes, COL, mean);
			else if (inputDepth == 32)
				variance = DoPartialVariance32to32( inputData,
								transData, layerCount, inputLayerBytes, COL, mean);
			
			if (thisCount > 2)
				variance = variance / (float)(thisCount - 1);
			
			outPtr[COL] = variance;
			}
		
		inputData += inputRowBytes;
		transData += inputRowBytes;
		outputData += outputRowBytes;
		countData += outputRowBytes;
		}
	
	}

/*****************************************************************************/

// this requires that the output buffer hold 32 bits of data per pixel
// and it requires a count buffer, also 32 bits per pixel

OSErr DoVarianceLayers( Ptr inputData,
					Ptr transData,
					Ptr outputData,
					Ptr countData,
					VRect writeRect,
					int32 inputRowBytes,
					int32 outputRowBytes,
					int16 inputDepth,
					int16 outputDepth,
					int32 layerCount,
					int32 inputLayerBytes )
	{
	int32 rows = writeRect.bottom - writeRect.top;
	int32 cols = writeRect.right - writeRect.left;
	int32 LAYER;
	
	if (outputDepth != 32)
		return 2;
	
	// zero the output buffer and count buffer (they're the same size)
	memset( outputData, 0, rows * outputRowBytes );
	memset( countData, 0, rows * outputRowBytes );
	
	Ptr tempInputData = inputData;
	Ptr tempTransData = transData;
	
	// add each layer into the output and count
	for (LAYER = 0; LAYER < layerCount; ++LAYER)
		{
		
		if (inputDepth == 8)
			{
			DoAddAndCountBuffer8to32( tempInputData, tempTransData, outputData, countData, rows, cols, inputRowBytes, outputRowBytes );
			}
		else if (inputDepth == 16)
			{
			DoAddAndCountBuffer16to32( tempInputData, tempTransData, outputData, countData, rows, cols, inputRowBytes, outputRowBytes );
			}
		else if (inputDepth == 32)
			{
			DoAddAndCountBuffer32to32( tempInputData, tempTransData, outputData, countData, rows, cols, inputRowBytes, outputRowBytes );
			}
		else
			{
			return 1;
			}
		
		// move on to the next layer
		tempInputData += inputLayerBytes;
		tempTransData += inputLayerBytes;
		
		}
	
	// now divide by the count
	DoScaleBuffer32( outputData, countData, rows, cols, outputRowBytes );
	
	
	// now that we have the mean per pixel, we can calculate the variance (replacing the mean)
	DoVariance32(	inputData,
					transData,
					outputData,
					countData,
					rows,
					cols,
					inputDepth,
					inputRowBytes,
					outputRowBytes,
					layerCount,
					inputLayerBytes );
	
	return noErr;
	}

/*****************************************************************************/

static float DoPartialSkewness8to32( Ptr inputData,
									 Ptr transData,
									int32 layerCount,
									int32 inputLayerBytes,
									int32 col,
									float mean,
									float &stddev)
	{
	unsigned char *inPtr = (unsigned char *)inputData + col;
	unsigned char *transPtr = (unsigned char *)transData + col;
	float skewness = 0.0f;
	int32 LAYER;
	
	for (LAYER = 0; LAYER < layerCount; ++LAYER)
		{
		if (transPtr[LAYER*inputLayerBytes] >= 128)
			{
			float value = ((float)inPtr[LAYER*inputLayerBytes]) * (1.0f / 255.0f);
			
			float diff = value - mean;
			
			stddev += (diff * diff);
			skewness += (diff * diff * diff);
			}
		}
	
	return skewness;
	
	}

/*****************************************************************************/

static float DoPartialSkewness16to32( Ptr inputData,
									 Ptr transData,
									int32 layerCount,
									int32 inputLayerBytes,
									int32 col,
									float mean,
									float &stddev)
	{
	unsigned short *inPtr = (unsigned short *)inputData + col;
	unsigned short *transPtr = (unsigned short *)transData + col;
	float skewness = 0.0f;
	int32 LAYER;
	
	for (LAYER = 0; LAYER < layerCount; ++LAYER)
		{
		if (transPtr[LAYER*(inputLayerBytes>>1)] >= 16384)
			{
			float value = ((float)inPtr[LAYER*(inputLayerBytes>>1)]) * (1.0f / 32768.0f);
			
			float diff = value - mean;
			
			stddev += (diff * diff);
			skewness += (diff * diff * diff);
			}
		}
	
	return skewness;
	
	}

/*****************************************************************************/

static float DoPartialSkewness32to32( Ptr inputData,
									 Ptr transData,
									int32 layerCount,
									int32 inputLayerBytes,
									int32 col,
									float mean,
									float &stddev)
	{
	float *inPtr = (float *)inputData + col;
	float *transPtr = (float *)transData + col;
	float skewness = 0.0f;
	int32 LAYER;
	
	for (LAYER = 0; LAYER < layerCount; ++LAYER)
		{
		if (transPtr[LAYER*(inputLayerBytes>>2)] >= 0.5f)
			{
			float value = inPtr[LAYER*(inputLayerBytes>>2)];
			
			float diff = value - mean;
			
			stddev += (diff * diff);
			skewness += (diff * diff * diff);
			}
		}
	
	return skewness;
	
	}

/*****************************************************************************/

// NOTE - ccox - there really should be a better way to do this calculation
// but, as far as I can tell, it would need more buffers
static void DoSkewness32(Ptr inputData,
					Ptr transData,
					Ptr outputData,
					Ptr countData,
					int32 rows,
					int32 cols,
					int16 inputDepth,
					int32 inputRowBytes,
					int32 outputRowBytes,
					int32 layerCount,
					int32 inputLayerBytes)
	{
	int32 ROW, COL;
		
	for (ROW = 0; ROW < rows; ++ROW)
		{
		float *outPtr = (float *)outputData;
		unsigned long *countPtr = (unsigned long *)countData;
		
		for (COL = 0; COL < cols; ++COL)
			{
			float skewness = 0.0f;
			float stddev = 0.0f;
			float mean = outPtr[COL];
			unsigned long thisCount = countPtr[COL];
			
			if (inputDepth == 8)
				skewness = DoPartialSkewness8to32( inputData,
								transData, layerCount, inputLayerBytes, COL, mean, stddev );
			else if (inputDepth == 16)
				skewness = DoPartialSkewness16to32( inputData,
								transData, layerCount, inputLayerBytes, COL, mean, stddev );
			else if (inputDepth == 32)
				skewness = DoPartialSkewness32to32( inputData,
								transData, layerCount, inputLayerBytes, COL, mean, stddev );
			
			if (thisCount > 1)
				{
				stddev = stddev / ((float)(thisCount - 1));
				
				if (stddev > 0.0f)
					stddev = sqrtf(stddev);
				else
					stddev = 0.0f;
				
				if (stddev > 1.0e-8)
					skewness = skewness / ((float)(thisCount - 1) * (stddev * stddev * stddev) );
				else
					skewness = 0.0f;
				}
			else
				skewness = 0.0f;
			
			outPtr[COL] = skewness;
			}
		
		inputData += inputRowBytes;
		transData += inputRowBytes;
		outputData += outputRowBytes;
		countData += outputRowBytes;
		}
	
	}

/*****************************************************************************/

// this requires that the output buffer hold 32 bits of data per pixel
// and it requires a count buffer, also 32 bits per pixel

OSErr DoSkewnessLayers( Ptr inputData,
					Ptr transData,
					Ptr outputData,
					Ptr countData,
					VRect writeRect,
					int32 inputRowBytes,
					int32 outputRowBytes,
					int16 inputDepth,
					int16 outputDepth,
					int32 layerCount,
					int32 inputLayerBytes )
	{
	int32 rows = writeRect.bottom - writeRect.top;
	int32 cols = writeRect.right - writeRect.left;
	int32 LAYER;
	
	if (outputDepth != 32)
		return 2;
	
	// zero the output buffer and count buffer (they're the same size)
	memset( outputData, 0, rows * outputRowBytes );
	memset( countData, 0, rows * outputRowBytes );
	
	Ptr tempInputData = inputData;
	Ptr tempTransData = transData;
	
	// add each layer into the output and count
	for (LAYER = 0; LAYER < layerCount; ++LAYER)
		{
		
		if (inputDepth == 8)
			{
			DoAddAndCountBuffer8to32( tempInputData, tempTransData, outputData, countData, rows, cols, inputRowBytes, outputRowBytes );
			}
		else if (inputDepth == 16)
			{
			DoAddAndCountBuffer16to32( tempInputData, tempTransData, outputData, countData, rows, cols, inputRowBytes, outputRowBytes );
			}
		else if (inputDepth == 32)
			{
			DoAddAndCountBuffer32to32( tempInputData, tempTransData, outputData, countData, rows, cols, inputRowBytes, outputRowBytes );
			}
		else
			{
			return 1;
			}
		
		// move on to the next layer
		tempInputData += inputLayerBytes;
		tempTransData += inputLayerBytes;
		
		}
	
	// now divide by the count
	DoScaleBuffer32( outputData, countData, rows, cols, outputRowBytes );
	
	// now that we have the mean per pixel, we can calculate the skewness (replacing the mean)
	DoSkewness32(	inputData,
					transData,
					outputData,
					countData,
					rows,
					cols,
					inputDepth,
					inputRowBytes,
					outputRowBytes,
					layerCount,
					inputLayerBytes );
	
	return noErr;
	}

/*****************************************************************************/

static float DoPartialKurtosis8to32( Ptr inputData,
									 Ptr transData,
									int32 layerCount,
									int32 inputLayerBytes,
									int32 col,
									float mean,
									float &stddev)
	{
	unsigned char *inPtr = (unsigned char *)inputData + col;
	unsigned char *transPtr = (unsigned char *)transData + col;
	float kurtosis = 0.0f;
	int32 LAYER;
	
	for (LAYER = 0; LAYER < layerCount; ++LAYER)
		{
		if (transPtr[LAYER*inputLayerBytes] >= 128)
			{
			float value = ((float)inPtr[LAYER*inputLayerBytes]) * (1.0f / 255.0f);
			
			float diff = value - mean;
			
			float temp = diff * diff;
			
			stddev += temp;
			kurtosis += temp * temp;
			}
		}
	
	return kurtosis;
	
	}

/*****************************************************************************/

static float DoPartialKurtosis16to32( Ptr inputData,
									 Ptr transData,
									int32 layerCount,
									int32 inputLayerBytes,
									int32 col,
									float mean,
									float &stddev)
	{
	unsigned short *inPtr = (unsigned short *)inputData + col;
	unsigned short *transPtr = (unsigned short *)transData + col;
	float kurtosis = 0.0f;
	int32 LAYER;
	
	for (LAYER = 0; LAYER < layerCount; ++LAYER)
		{
		if (transPtr[LAYER*(inputLayerBytes>>1)] >= 16384)
			{
			float value = ((float)inPtr[LAYER*(inputLayerBytes>>1)]) * (1.0f / 32768.0f);
			
			float diff = value - mean;
			
			float temp = diff * diff;
			
			stddev += temp;
			kurtosis += temp * temp;
			}
		}
	
	return kurtosis;
	
	}

/*****************************************************************************/

static float DoPartialKurtosis32to32( Ptr inputData,
									 Ptr transData,
									int32 layerCount,
									int32 inputLayerBytes,
									int32 col,
									float mean,
									float &stddev)
	{
	float *inPtr = (float *)inputData + col;
	float *transPtr = (float *)transData + col;
	float kurtosis = 0.0f;
	int32 LAYER;
	
	for (LAYER = 0; LAYER < layerCount; ++LAYER)
		{
		if (transPtr[LAYER*(inputLayerBytes>>2)] >= 0.5f)
			{
			float value = inPtr[LAYER*(inputLayerBytes>>2)];
			
			float diff = value - mean;
			
			float temp = diff * diff;
			
			stddev += temp;
			kurtosis += temp * temp;
			}
		}
	
	return kurtosis;
	
	}

/*****************************************************************************/

// NOTE - ccox - there really should be a better way to do this calculation
// but, as far as I can tell, it would need more buffers
static void DoKurtosis32(Ptr inputData,
					Ptr transData,
					Ptr outputData,
					Ptr countData,
					int32 rows,
					int32 cols,
					int16 inputDepth,
					int32 inputRowBytes,
					int32 outputRowBytes,
					int32 layerCount,
					int32 inputLayerBytes)
	{
	int32 ROW, COL;
		
	for (ROW = 0; ROW < rows; ++ROW)
		{
		float *outPtr = (float *)outputData;
		unsigned long *countPtr = (unsigned long *)countData;
		
		for (COL = 0; COL < cols; ++COL)
			{
			float kurtosis = 0.0f;
			float stddev = 0.0f;
			float mean = outPtr[COL];
			unsigned long thisCount = countPtr[COL];
			
			if (inputDepth == 8)
				kurtosis = DoPartialKurtosis8to32( inputData,
								transData, layerCount, inputLayerBytes, COL, mean, stddev );
			else if (inputDepth == 16)
				kurtosis = DoPartialKurtosis16to32( inputData,
								transData, layerCount, inputLayerBytes, COL, mean, stddev );
			else if (inputDepth == 32)
				kurtosis = DoPartialKurtosis32to32( inputData,
								transData, layerCount, inputLayerBytes, COL, mean, stddev );
			
			if (thisCount > 1)
				{
				stddev = stddev / ((float)(thisCount - 1));
				
				// no sqrt, so it's really variance, but I need (stddev^4)
				
				if (stddev > 1.0e-8)
					kurtosis = kurtosis / ((float)(thisCount - 1) * (stddev * stddev) );
				else
					kurtosis = 0.0f;
				}
			else
				kurtosis = 0.0f;
			
			outPtr[COL] = kurtosis;
			}
		
		inputData += inputRowBytes;
		transData += inputRowBytes;
		outputData += outputRowBytes;
		countData += outputRowBytes;
		}
	
	}

/*****************************************************************************/

// this requires that the output buffer hold 32 bits of data per pixel
// and it requires a count buffer, also 32 bits per pixel

OSErr DoKurtosisLayers( Ptr inputData,
					Ptr transData,
					Ptr outputData,
					Ptr countData,
					VRect writeRect,
					int32 inputRowBytes,
					int32 outputRowBytes,
					int16 inputDepth,
					int16 outputDepth,
					int32 layerCount,
					int32 inputLayerBytes )
	{
	int32 rows = writeRect.bottom - writeRect.top;
	int32 cols = writeRect.right - writeRect.left;
	int32 LAYER;
	
	if (outputDepth != 32)
		return 2;
	
	// zero the output buffer and count buffer (they're the same size)
	memset( outputData, 0, rows * outputRowBytes );
	memset( countData, 0, rows * outputRowBytes );
	
	Ptr tempInputData = inputData;
	Ptr tempTransData = transData;
	
	// add each layer into the output and count
	for (LAYER = 0; LAYER < layerCount; ++LAYER)
		{
		
		if (inputDepth == 8)
			{
			DoAddAndCountBuffer8to32( tempInputData, tempTransData, outputData, countData, rows, cols, inputRowBytes, outputRowBytes );
			}
		else if (inputDepth == 16)
			{
			DoAddAndCountBuffer16to32( tempInputData, tempTransData, outputData, countData, rows, cols, inputRowBytes, outputRowBytes );
			}
		else if (inputDepth == 32)
			{
			DoAddAndCountBuffer32to32( tempInputData, tempTransData, outputData, countData, rows, cols, inputRowBytes, outputRowBytes );
			}
		else
			{
			return 1;
			}
		
		// move on to the next layer
		tempInputData += inputLayerBytes;
		tempTransData += inputLayerBytes;
		
		}
	
	// now divide by the count
	DoScaleBuffer32( outputData, countData, rows, cols, outputRowBytes );
	
	// now that we have the mean per pixel, we can calculate the kurtosis (replacing the mean)
	DoKurtosis32(	inputData,
					transData,
					outputData,
					countData,
					rows,
					cols,
					inputDepth,
					inputRowBytes,
					outputRowBytes,
					layerCount,
					inputLayerBytes );
	
	return noErr;
	}

/*****************************************************************************/

static void DoEntropy8to32( Ptr inputData,
						 Ptr transData,
						 Ptr outputData,
						 int32 rows,
						 int32 cols,
						int32 layerCount,
						int32 inputRowBytes,
						int32 outputRowBytes,
						int32 inputLayerBytes)
	{
	int32 LAYER, ROW, COL;
	unsigned32 histogram[256];

#if __PIWin__
	float invLog2 = static_cast<float>(1.0 / std::log(2.0));
#endif
	
	
	for (ROW = 0; ROW < rows; ++ROW)
		{
		unsigned char *inPtr = (unsigned char *)inputData;
		unsigned char *transPtr = (unsigned char *)transData;
		float *outPtr = (float *)outputData;
		
		for (COL = 0; COL < cols; ++COL)
			{
			int32 count = 0;
			
			// zero the histogram
			memset( histogram, 0, sizeof(histogram) );
			
			for (LAYER = 0; LAYER < layerCount; ++LAYER)
				{
				
				if (transPtr[LAYER*inputLayerBytes + COL] >= 128)
					{
					unsigned char value = inPtr[LAYER*inputLayerBytes + COL];
					
					count++;
					histogram[value] += 1;
					}
				}
			
			// for a single count, entropy is zero
			if (count > 1)
				{
				float entropy = 0.0f;
				int32 i;
				
				for ( i = 0; i < 256; ++i )
					{
					if (histogram[i] > 0)
						{
						float probability = (float)histogram[i] / (float)count;
#if __PIWin__
						entropy += probability * std::log(probability) * invLog2;
#else
						entropy += probability * std::log2(probability);
#endif
						}
					}
				
				outPtr[COL] = -entropy;
				
				}
			else
				outPtr[COL] = 0.0f;
			}
		
		inputData += inputRowBytes;
		transData += inputRowBytes;
		outputData += outputRowBytes;
		}
	
	}

/*****************************************************************************/

static void DoEntropy16to32( Ptr inputData,
						 Ptr transData,
						 Ptr outputData,
						 int32 rows,
						 int32 cols,
						int32 layerCount,
						int32 inputRowBytes,
						int32 outputRowBytes,
						int32 inputLayerBytes)
	{
	int32 LAYER, ROW, COL;
	unsigned32 *histogram = new unsigned32[65536];	// only 0..32768 are legal, but we allocate larger for safety
	if (histogram == NULL) return;
	unsigned32 *coarseHist = new unsigned32[256];
	if (coarseHist == NULL)
	{
		delete [] histogram;
		return;
	}
	const int32 coarseShift = 8;
	const int32 coarseStep = (1L << coarseShift);

#if __PIWin__
	float invLog2 = static_cast<float>(1.0 / std::log(2.0));
#endif
	
	
	for (ROW = 0; ROW < rows; ++ROW)
		{
		unsigned short *inPtr = (unsigned short *)inputData;
		unsigned short *transPtr = (unsigned short *)transData;
		float *outPtr = (float *)outputData;
		
		for (COL = 0; COL < cols; ++COL)
			{
			int32 count = 0;
			
			// zero the histograms
			memset( histogram, 0, sizeof(unsigned32) * 65536 );
			memset( coarseHist, 0, sizeof(unsigned32) * 256 );
			
			for (LAYER = 0; LAYER < layerCount; ++LAYER)
				{
				
				if (transPtr[LAYER*(inputLayerBytes>>1) + COL] >= 16384)
					{
					unsigned short value = inPtr[LAYER*(inputLayerBytes>>1) + COL];
					
					count++;
					histogram[value] += 1;
					coarseHist[ (value >> coarseShift) ] += 1;
					}
				}
			
			// for a single count, entropy is zero
			if (count > 1)
				{
				float entropy = 0.0f;
				int32 i;
				
				for ( i = 0; i <= 32768; ++i )
					{
					
					// if this region is empty, skip it
					if (coarseHist[ (i >> coarseShift) ] == 0)
						{
						i += (coarseStep - 1);
						continue;
						}
					
					if (histogram[i] > 0)
						{
						float probability = (float)histogram[i] / (float)count;
#if __PIWin__
						entropy += probability * std::log(probability) * invLog2;
#else
						entropy += probability * std::log2(probability);
#endif
						}
					}
				
				outPtr[COL] = -entropy;
				
				}
			else
				outPtr[COL] = 0.0f;
			}
		
		inputData += inputRowBytes;
		transData += inputRowBytes;
		outputData += outputRowBytes;
		}

	delete [] coarseHist;
	delete [] histogram;

	}

/*****************************************************************************/

// this operates differently because we can't use a histogram

static void DoEntropy32to32( Ptr inputData,
						 Ptr transData,
						 Ptr outputData,
						 int32 rows,
						 int32 cols,
						int32 layerCount,
						int32 inputRowBytes,
						int32 outputRowBytes,
						int32 inputLayerBytes)
	{
	int32 LAYER, ROW, COL;

#if __PIWin__
	float invLog2 = static_cast<float>(1.0 / std::log(2.0));
#endif
	
	
	// allocate the vector once per tile
	std::vector<float> theList;
	
	theList.reserve(layerCount);
	
	
	for (ROW = 0; ROW < rows; ++ROW)
		{
		float *inPtr = (float *)inputData;
		float *transPtr = (float *)transData;
		float *outPtr = (float *)outputData;
		
		for (COL = 0; COL < cols; ++COL)
			{
			
			// clear the list (set size to zero)
			theList.erase( theList.begin(), theList.end() );
			
			for (LAYER = 0; LAYER < layerCount; ++LAYER)
				{
				
				if (transPtr[LAYER*(inputLayerBytes>>2) + COL] >= 0.5)
					{
					float value = inPtr[LAYER*(inputLayerBytes>>2) + COL];
					
					theList.push_back(value);
					}
				}
			
			
			size_t count = theList.size();
			
			// if all values are unique, the entropy is ( -log2(1/count) = log2(count) )
			// if all values are the same, the entropy is zero (-log2(1.0) = -0.0)
			
			// for a single count, entropy is zero
			if (count > 1)
				{
				float entropy = 0.0f;
				size_t i;
				
				// sort the list
				std::sort( theList.begin(), theList.end() );
				
				// for each entry in the list, find duplicate values
				// then go to next non duplicated entry
				for ( i = 0; i < count; ++i )
					{
					
					float startValue = theList[i];
					size_t duplicates = 1;
					size_t index = i + 1;
					
					while ( (index < count) && (theList[index] == startValue) )
						{
						++duplicates;
						++index;
						}
					
					float probability = (float)duplicates / (float)count;
					
#if __PIWin__
					entropy += probability * std::log(probability) * invLog2;
#else
					entropy += probability * std::log2(probability);
#endif
					
					
					// skip over the duplicate values
					i += (duplicates - 1);
					
					}
				
				outPtr[COL] = -entropy;
				
				}
			else
				outPtr[COL] = 0.0f;
			}
		
		inputData += inputRowBytes;
		transData += inputRowBytes;
		outputData += outputRowBytes;
		}
	
	}

/*****************************************************************************/

// this requires that the output buffer hold 32 bits of data per pixel
// and it requires a count buffer, also 32 bits per pixel

OSErr DoEntropyLayers( Ptr inputData,
					Ptr transData,
					Ptr outputData,
					VRect writeRect,
					int32 inputRowBytes,
					int32 outputRowBytes,
					int16 inputDepth,
					int16 outputDepth,
					int32 layerCount,
					int32 inputLayerBytes )
	{
	int32 rows = writeRect.bottom - writeRect.top;
	int32 cols = writeRect.right - writeRect.left;
	
	if (outputDepth != 32)
		return 2;
	
	if (inputDepth == 8)
		DoEntropy8to32( inputData, transData, outputData, rows, cols, layerCount, inputRowBytes, outputRowBytes, inputLayerBytes );
	else if (inputDepth == 16)
		DoEntropy16to32( inputData, transData, outputData, rows, cols, layerCount, inputRowBytes, outputRowBytes, inputLayerBytes );
	else if (inputDepth == 32)
		DoEntropy32to32( inputData, transData, outputData, rows, cols, layerCount, inputRowBytes, outputRowBytes, inputLayerBytes );
	else
		return 1;
	
	return noErr;
	}

/*****************************************************************************/

static void DoRange8( Ptr inputData,
						 Ptr transData,
						 Ptr outputData,
						int32 layerCount,
						int32 inputLayerBytes,
						int32 col)
	{
	unsigned char *inPtr = (unsigned char *)inputData + col;
	unsigned char *transPtr = (unsigned char *)transData + col;
	unsigned char *outPtr = (unsigned char *)outputData + col;
	unsigned char maxValue = 0;
	unsigned char minValue = 255;
	int32 LAYER;
	
	for (LAYER = 0; LAYER < layerCount; ++LAYER)
		{
		if (transPtr[LAYER*inputLayerBytes] >= 128)
			{
			unsigned char value = inPtr[LAYER*inputLayerBytes];
			
			if (value > maxValue)
				maxValue = value;
			
			if (value < minValue)
				minValue = value;
			
			}
		}
	
	*outPtr = maxValue - minValue;
	
	}

/*****************************************************************************/

static void DoRange16( Ptr inputData,
						 Ptr transData,
						 Ptr outputData,
						int32 layerCount,
						int32 inputLayerBytes,
						int32 col)
{
	unsigned short *inPtr = (unsigned short *)inputData + col;
	unsigned short *transPtr = (unsigned short *)transData + col;
	unsigned short *outPtr = (unsigned short *)outputData + col;
	unsigned short maxValue = 0;
	unsigned short minValue = 65535;
	int32 LAYER;
	
	for (LAYER = 0; LAYER < layerCount; ++LAYER)
		{
		if (transPtr[LAYER*(inputLayerBytes>>1)] >= 16384)
			{
			unsigned short value = inPtr[LAYER*(inputLayerBytes>>1)];
			
			if (value > maxValue)
				maxValue = value;
			
			if (value < minValue)
				minValue = value;
			
			}
		}
	
	*outPtr = maxValue - minValue;
	
	}

/*****************************************************************************/

static void DoRange32( Ptr inputData,
					 Ptr transData,
					 Ptr outputData,
					int32 layerCount,
					int32 inputLayerBytes,
					int32 col)
	{
	float *inPtr = (float *)inputData + col;
	float *transPtr = (float *)transData + col;
	float *outPtr = (float *)outputData + col;
	float maxValue = (float) -1e20;
	float minValue = (float) 1e20;
	int32 LAYER;
	
	for (LAYER = 0; LAYER < layerCount; ++LAYER)
		{
		if (transPtr[LAYER*inputLayerBytes] >= 0.5f)
			{
			float value = inPtr[LAYER*(inputLayerBytes>>2)];
			
			if (value > maxValue)
				maxValue = value;
			
			if (value < minValue)
				minValue = value;
			
			}
		}
	
	*outPtr = maxValue - minValue;
	
	}

/*****************************************************************************/

OSErr DoRangeLayers( Ptr inputData,
					Ptr transData,
					Ptr outputData,
					VRect writeRect,
					int32 inputRowBytes,
					int32 outputRowBytes,
					int16 inputDepth,
					int16 outputDepth,
					int32 layerCount,
					int32 inputLayerBytes )
	{
	int32 rows = writeRect.bottom - writeRect.top;
	int32 cols = writeRect.right - writeRect.left;
	int32 ROW, COL;
	
	if (inputDepth != outputDepth)
		return 2;
	
	// zero the output buffer
	memset( outputData, 0, rows * outputRowBytes );
	
	for (ROW = 0; ROW < rows; ++ROW)
		{
		
		for (COL = 0; COL < cols; ++COL)
			{
			
			if (inputDepth == 8)
				DoRange8( inputData, transData, outputData, layerCount, inputLayerBytes, COL );
			else if (inputDepth == 16)
				DoRange16( inputData, transData, outputData, layerCount, inputLayerBytes, COL );
			else if (inputDepth == 32)
				DoRange32( inputData, transData, outputData, layerCount, inputLayerBytes, COL );
			}
		
		inputData += inputRowBytes;
		transData += inputRowBytes;
		outputData += outputRowBytes;
		}
	
	return noErr;
	}

/*****************************************************************************/

// TODO -- beyond a point, creating a histogram would be faster than sorting
// probably near NLog(N) ~= 256, or N about 64
// so, layerCount >= 75 is probably a safe breaking point

static void DoMedian8( Ptr inputData,
						 Ptr transData,
						 Ptr outputData,
						 int32 rows,
						 int32 cols,
						int32 layerCount,
						int32 inputRowBytes,
						int32 outputRowBytes,
						int32 inputLayerBytes)
	{
	int32 LAYER, ROW, COL;
	
	
	// allocate the vector once per tile
	std::vector<unsigned char> theList;
	
	theList.reserve(layerCount);
	
	
	
	for (ROW = 0; ROW < rows; ++ROW)
		{
		unsigned char *inPtr = (unsigned char *)inputData;
		unsigned char *transPtr = (unsigned char *)transData;
		unsigned char *outPtr = (unsigned char *)outputData;
		
		for (COL = 0; COL < cols; ++COL)
			{
			
			// clear the list (set size to zero)
			theList.erase( theList.begin(), theList.end() );
			
			
			for (LAYER = 0; LAYER < layerCount; ++LAYER)
				{
				
				if (transPtr[LAYER*inputLayerBytes + COL] >= 128)
					{
					unsigned char value = inPtr[LAYER*inputLayerBytes + COL];
					
					theList.push_back(value);
					}
				}
			
			size_t length = theList.size();
			size_t half = length >> 1;
			unsigned char median = 0;
			
			if (length > 1)
				{
				// sort the list
				std::sort( theList.begin(), theList.end() );
				
				// find the median

				if (length & 1)	// if odd, take the middle
					{
					median = theList[half];
					}
				else	// if even, take the average of the middle 2 entries
					{
					median = (unsigned char)(((unsigned short)theList[half-1] + (unsigned short)theList[half] + 1) / 2);
					}
				}
			else if (length == 1)
				median = theList[0];
			
			outPtr[COL] = median;
			}
		
		inputData += inputRowBytes;
		transData += inputRowBytes;
		outputData += outputRowBytes;
		}
	
	}

/*****************************************************************************/

// TODO -- beyond a point, creating a histogram would be faster than sorting
// probably near NLog(N) ~= 32768, or N around 4096 (too large to bother?)

static void DoMedian16( Ptr inputData,
						 Ptr transData,
						 Ptr outputData,
						 int32 rows,
						 int32 cols,
						int32 layerCount,
						int32 inputRowBytes,
						int32 outputRowBytes,
						int32 inputLayerBytes)
	{
	int32 LAYER, ROW, COL;
	
	
	// allocate the vector once per tile
	std::vector<unsigned short> theList;
	
	theList.reserve(layerCount);
	
	
	
	for (ROW = 0; ROW < rows; ++ROW)
		{
		unsigned short *inPtr = (unsigned short *)inputData;
		unsigned short *transPtr = (unsigned short *)transData;
		unsigned short *outPtr = (unsigned short *)outputData;
		
		for (COL = 0; COL < cols; ++COL)
			{
			
			// clear the list (set size to zero)
			theList.erase( theList.begin(), theList.end() );
			
			
			for (LAYER = 0; LAYER < layerCount; ++LAYER)
				{
				
				if (transPtr[LAYER*(inputLayerBytes>>1) + COL] >= 16384)
					{
					unsigned short value = inPtr[LAYER*(inputLayerBytes>>1) + COL];
					
					theList.push_back(value);
					}
				}
			
			size_t length = theList.size();
			size_t half = length >> 1;
			unsigned short median = 0;
			
			if (length > 1)
				{
				// sort the list
				std::sort( theList.begin(), theList.end() );
				
				// find the median

				if (length & 1)	// if odd, take the middle
					{
					median = theList[half];
					}
				else	// if even, take the average of the middle 2 entries
					{
					median = (unsigned short)(((unsigned long)theList[half-1] + (unsigned long)theList[half] + 1) / 2);
					}
				}
			else if (length == 1)
				median = theList[0];
			
			outPtr[COL] = median;
			}
		
		inputData += inputRowBytes;
		transData += inputRowBytes;
		outputData += outputRowBytes;
		}
	
	}

/*****************************************************************************/

static void DoMedian32( Ptr inputData,
						 Ptr transData,
						 Ptr outputData,
						 int32 rows,
						 int32 cols,
						int32 layerCount,
						int32 inputRowBytes,
						int32 outputRowBytes,
						int32 inputLayerBytes)
	{
	int32 LAYER, ROW, COL;
	
	
	// allocate the vector once per tile
	std::vector<float> theList;
	
	theList.reserve(layerCount);
	
	
	
	for (ROW = 0; ROW < rows; ++ROW)
		{
		float *inPtr = (float *)inputData;
		float *transPtr = (float *)transData;
		float *outPtr = (float *)outputData;
		
		for (COL = 0; COL < cols; ++COL)
			{
			
			// clear the list (set size to zero)
			theList.erase( theList.begin(), theList.end() );
			
			
			for (LAYER = 0; LAYER < layerCount; ++LAYER)
				{
				
				if (transPtr[LAYER*(inputLayerBytes>>2) + COL] >= 0.5f)
					{
					float value = inPtr[LAYER*(inputLayerBytes>>2) + COL];
					
					theList.push_back(value);
					
					}
				}
			
			size_t length = theList.size();
			size_t half = length >> 1;
			float median = 0.0f;
			
			if (length > 1)
				{
				// sort the list
				std::sort( theList.begin(), theList.end() );
				
				// find the median

				if (length & 1)	// if odd, take the middle
					{
					median = theList[half];
					}
				else	// if even, take the average of the middle 2 entries
					{
					median = 0.5f * (theList[half-1] + theList[half]);
					}
				}
			else if (length == 1)
				median = theList[0];
			
			outPtr[COL] = median;
			}
		
		inputData += inputRowBytes;
		transData += inputRowBytes;
		outputData += outputRowBytes;
		}
	
	}

/*****************************************************************************/

OSErr DoMedianLayers( Ptr inputData,
					Ptr transData,
					Ptr outputData,
					VRect writeRect,
					int32 inputRowBytes,
					int32 outputRowBytes,
					int16 inputDepth,
					int16 outputDepth,
					int32 layerCount,
					int32 inputLayerBytes )
	{
	int32 rows = writeRect.bottom - writeRect.top;
	int32 cols = writeRect.right - writeRect.left;
	
	if (inputDepth != outputDepth)
		return 2;
	
	// zero the output buffer
	memset( outputData, 0, rows * outputRowBytes );
	
	if (inputDepth == 8)
		DoMedian8( inputData,
						transData, outputData, rows, cols, layerCount, inputRowBytes, outputRowBytes, inputLayerBytes );
	else if (inputDepth == 16)
		DoMedian16( inputData,
						transData, outputData, rows, cols, layerCount, inputRowBytes, outputRowBytes, inputLayerBytes );
	else if (inputDepth == 32)
		DoMedian32( inputData,
						transData, outputData, rows, cols, layerCount, inputRowBytes, outputRowBytes, inputLayerBytes );
	else
		return 1;
	
	return noErr;
	}

/*****************************************************************************/
/*****************************************************************************/

// used by mean and summation
void DoStartMean (GPtr globals, bool isSummation)
{

	/* Insist on having the necessary procs */
	if (!WarnAdvanceStateAvailable ()) {
		gResult = acquireBadParameters;
		return;
	}

	if (!WarnBufferProcsAvailable ()) {
		gResult = acquireBadParameters;
		return;
	}

	/* Assume we won't be coming back around for another pass unless explicitly so. */
	gStuff->acquireAgain = false;


	/* if stuff hasn't been initialized that we need, do it,
		then go check if we've got scripting commands to
		override our settings */
	OpenScriptParams (globals); // update our parameters with the scripting parameters, if available

	/* If we need to use our dialog, or it's already up, do it. */
	ValidateParameters (globals);

	if ( gStuff->channelPortProcs == NULL
		|| gStuff->documentInfo == NULL) {
		gResult = acquireBadParameters;
		return;
	}
	
	// we need the new data
	if ( gStuff->documentInfo->maxVersion < 3) {
		gResult = acquireBadParameters;
		return;
	}
	
	if (gStuff->descriptorParameters->playInfo == plugInDialogDisplay)
		{
		// show our UI, if we had any
		}
	
	
	gStuff->planes = (int16) GetCompositeChannelCount(globals);
	
	gStuff->imageMode = (int16) gStuff->documentInfo->imageMode;
	
	
	// result is always 32 bit for summation
	if (isSummation)
		gOutputDepth = 32;
	else
		{
		// if we're averaging, we can use less precision
		// AND we don't want the gamma shift that occurs with going to 32 bit
		
		switch (gStuff->documentInfo->depth)
			{
			
			case 1: 	// how did we get here?!
			case 8:
				gOutputDepth = 16;
				break;
			
			case 16:
				gOutputDepth = 16;
				break;
				
			case 32:
				gOutputDepth = 32;
				break;
			}
		
		
		}
	
	
	// copy the document profile data
	if (gStuff->canUseICCProfiles)
		{
		gStuff->iCCprofileSize = gStuff->documentInfo->iCCprofileSize;
		
		if (gStuff->documentInfo->iCCprofileSize > 0)
			{
			
			// the handle will be freed by Photoshop
			Handle profileData = gStuff->handleProcs->newProc( gStuff->iCCprofileSize );
			
			if (profileData == NULL)
				{
				gResult = memFullErr;
				return;
				}
			
			Ptr tempData = gStuff->handleProcs->lockProc( profileData, false );
			
			if (tempData == NULL)
				{
				gResult = memFullErr;
				return;
				}
			
			PIBlockMove( (Ptr)gStuff->documentInfo->iCCprofileData, (Ptr)tempData, gStuff->iCCprofileSize );
			
			gStuff->handleProcs->unlockProc( profileData );
			
			gStuff->iCCprofileData = profileData;
			
			}
		
		}

	// if indexed color source and result, we need to copy the LUTs
	if (gStuff->imageMode == plugInModeIndexedColor
		&& gStuff->documentInfo->imageMode == plugInModeIndexedColor)
		{
		PIBlockMove( (Ptr)gStuff->documentInfo->redLUT, (Ptr)gStuff->redLUT, sizeof(LookUpTable) );
		PIBlockMove( (Ptr)gStuff->documentInfo->greenLUT, (Ptr)gStuff->greenLUT, sizeof(LookUpTable) );
		PIBlockMove( (Ptr)gStuff->documentInfo->blueLUT, (Ptr)gStuff->blueLUT, sizeof(LookUpTable) );
		}

	gLastRows = gStuff->documentInfo->bounds.bottom - gStuff->documentInfo->bounds.top;
	gLastCols = gStuff->documentInfo->bounds.right - gStuff->documentInfo->bounds.left;
	
	gStuff->imageSize.v = (short)gLastRows;
	gStuff->imageSize.h = (short)gLastCols;
	
	if (gStuff->HostSupports32BitCoordinates)
		{
		gStuff->PluginUsing32BitCoordinates = true;
		gStuff->imageSize32.v = gLastRows;
		gStuff->imageSize32.h = gLastCols;
		}
	
	gStuff->imageHRes = gStuff->documentInfo->hResolution;
	gStuff->imageVRes = gStuff->documentInfo->vResolution;
	
	gStuff->data = NULL;
	

	// work out our optimal tile size based on the number of layers and buffers
	gLayerCount = GetLayerCount(globals);
	gVisibleLayerCount = GetVisibleLayerCount(globals);
	
	if (gVisibleLayerCount < 2)
		{
		// resort to copying the composite if we have no layers or just a single layer
		
		gVisibleLayerCount = 1;	// so calculations won't fail
		gOutputDepth = static_cast<int16>(gStuff->documentInfo->depth);
		
		if (gStuff->documentInfo->mergedTransparency != 0)
			{
			gStuff->planes += 1;	// for transparency
			gStuff->isFirstAlphaTransparency = 1;
			}
		
		}
	
	// we can't really return a duotone document because the duotoneInfo field is undocumented!
	if (gStuff->imageMode == plugInModeDuotone)
		{
		gStuff->imageMode = plugInModeGrayScale;
		}
	
	// multichannel has no composite channels, just alpha channels
	if (gStuff->imageMode == plugInModeMultichannel)
		{
		gStuff->planes = static_cast<int16>(GetAlphaChannelCount(globals));
		}
	
	
	// make sure the image mode is legal for 32 bit!
	if (gOutputDepth == 32)
		{
		int16 newMode = plugInModeGrayScale;
		
		switch (gStuff->imageMode)
			{
			case plugInModeBitmap:
			case plugInModeGrayScale:
			case plugInModeGray16:
			case plugInModeGray32:
			case plugInModeDuotone16:
			case plugInModeIndexedColor:
			case plugInModeDuotone:
				newMode = plugInModeGrayScale;
				break;
			
			default:
			case plugInModeRGB48:
			case plugInModeLab48:
			case plugInModeCMYK64:
			case plugInModeRGB96:
			case plugInModeRGBColor:
			case plugInModeCMYKColor:
			case plugInModeMultichannel:
			case plugInModeLabColor:
				newMode = plugInModeRGBColor;
				break;
			}
		
		if (newMode != gStuff->imageMode
			&& gStuff->iCCprofileData)
			{
			// if we're changing modes, get rid of the ICC profile - it'll be invalid
			
			gStuff->handleProcs->disposeProc( gStuff->iCCprofileData );
		
			gStuff->iCCprofileData = NULL;
			gStuff->iCCprofileSize = 0;
			
			}
		
		
		gStuff->imageMode = newMode;
		
		}
	
	gStuff->depth = gOutputDepth;
	
	
	
	// make sure we have the right scaling!
	if (gOutputDepth == 16)
		gStuff->maxValue = 32768;
	
	
	unsigned32 maxUsage = maxMemory;
	
	if (maxUsage > gMaxData)
		maxUsage = gMaxData;
	
	// how many buffers per layer are needed for calculation?
	// one channel plus transparency
	int32 bufferCount = 2;
	
	// tile size is square
	int32 tileSize = (int32) sqrt( (double)(maxUsage / ( gVisibleLayerCount * bufferCount + 2 )) );
	
	// and set a lower limit on the tile size for sanity's sake
	if (tileSize < minTileSize)
		tileSize = minTileSize;
	
	// also limit tile size to maximum dimension of the resulting image?
	int32 maxDimension = gLastRows;
	
	if (maxDimension < gLastCols)
		maxDimension = gLastCols;
	
	if (tileSize > maxDimension)
		tileSize = maxDimension;
	
	// should we establish a maximum tile size as well?
	if (tileSize > maxTileSize)
		tileSize = maxTileSize;
	
	gTileSize = tileSize;

}

/*****************************************************************************/

// for cases where there are no layers to blend, just copy the composite as-is
void DoCopyComposite(GPtr globals)
	{
	
	AcquireRegion region;
	AcquireDataLayout layout;
	
	BufferID inputBuffer = 0;
	BufferID inputTransparencyBuffer = 0;
	Ptr inputData;
	Ptr inputTransData;
	
	int32 imageWidth = gStuff->imageSize.h;
	int32 imageHeight = gStuff->imageSize.v;
	
	int16 inputDepth = (int16) gStuff->documentInfo->depth;
	
	if (gStuff->HostSupports32BitCoordinates)
		{
		imageWidth = gStuff->imageSize32.h;
		imageHeight = gStuff->imageSize32.v;
		}


	int32 progressDone = 0;
	int32 tilesHigh = (imageHeight + (gTileSize -1)) / gTileSize;
	int32 tilesWide = (imageWidth + (gTileSize -1)) / gTileSize;
	int32 progressTotal = tilesHigh * tilesWide * gStuff->planes;


	/* Allocate the buffers */

	int32 inputRowBytes = (gTileSize * inputDepth) >> 3;
	int32 inputLayerBytes = gTileSize * inputRowBytes;
	int32 inputBufferSize = gVisibleLayerCount * inputLayerBytes;
	
	gResult = AllocateBuffer (inputBufferSize, &inputBuffer);
	
	if (gResult != noErr)
		goto exit;
	
	gResult = AllocateBuffer (inputBufferSize, &inputTransparencyBuffer);
	
	if (gResult != noErr)
		goto exit;
	
	{
			
	// allocate one tile's worth for output
	int32 outputRowBytes = (gTileSize * gOutputDepth) >> 3;
	
	layout.rowBytes = outputRowBytes;
	layout.colBytes = (gOutputDepth >> 3);
	layout.planeBytes = 0;	/* irrelevant -- we work one plane at a time */

	
	inputData = LockBuffer (inputBuffer, false);
	inputTransData = LockBuffer (inputTransparencyBuffer, false);
	
	
	int32 compositePlanes = GetCompositeChannelCount(globals);
	
	if (gStuff->imageMode == plugInModeMultichannel)
		compositePlanes = GetAlphaChannelCount(globals);
	
	
	RANGE_ITER (region.rect.top, region.rect.bottom,
				0, imageHeight, gTileSize)
		{
		
		RANGE_ITER (region.rect.left, region.rect.right,
					0, imageWidth, gTileSize)
			{
			
			Ptr transPtr = inputTransData;
			
			// read the transparency information, and save it off if needed

			ReadChannelDesc *transparencyDesc = gStuff->documentInfo->mergedTransparency;
			
			if (transparencyDesc != NULL)
				{
				
				VRect writeRect = region.rect;
					
				gResult = ReadChannelPixelsIntoPlanarBuffer(
												globals,
												transparencyDesc,
												(void *)transPtr,
												writeRect,
												inputRowBytes,
												inputDepth );

				if (gResult != noErr)
					goto exit;

				// save the transparency
				region.loPlane = gStuff->planes - 1;
				region.hiPlane = region.loPlane;
				
				// store the result
				gResult = StoreData (gStuff, (void *)transPtr, &region, &layout);
				
				if (gResult != noErr)
					goto exit;
				
				}	// if transparencyDesc
			
			
			
			
			// iterate over the color planes
			
			for (region.loPlane = 0; region.loPlane < compositePlanes; ++region.loPlane)
				{
			
				PIUpdateProgress (progressDone, progressTotal);
				progressDone += 1;
				
				if (TestAbort ())
					{
					gResult = userCanceledErr;
					goto exit;
					}
				
				region.hiPlane = region.loPlane;
				
				Ptr layerPtr = inputData;
				
				// read the composite color data
				
					{
					
					// default to composite, in case there is only one layer
					ReadChannelDesc *currentChannelDesc = gStuff->documentInfo->mergedCompositeChannels;
					
					if (gStuff->imageMode == plugInModeMultichannel)
						currentChannelDesc = gStuff->documentInfo->alphaChannels;
					
					if (currentChannelDesc == NULL)
						{
						gResult = acquireBadParameters;
						goto exit;
						}
					
					// iterate to get to the right channel pointer  (!#$%@#$% linked list)
					int32 CHANNEL;
					for (CHANNEL = 0; CHANNEL < region.loPlane && currentChannelDesc != NULL; ++CHANNEL)
						currentChannelDesc = currentChannelDesc->next;
					
					if (currentChannelDesc == NULL)
						{
						gResult = acquireBadParameters;
						goto exit;
						}
					
					VRect writeRect = region.rect;

					gResult = ReadChannelPixelsIntoPlanarBuffer(
													globals,
													currentChannelDesc,
													(void *)layerPtr,
													writeRect,
													inputRowBytes,
													inputDepth );

					if (gResult != noErr)
						goto exit;
					
					}
				
				
				// store the result
				gResult = StoreData (gStuff, (void *)inputData, &region, &layout);
				
				if (gResult != noErr)
					goto exit;
				
				}	// iterate over channels
			
			}	// iterate horizontally
		
		}	// iterate vertically


	// we are done
	gStuff->data = NULL;
	SetRect (&gStuff->theRect, 0, 0, 0, 0);
	
	if (gStuff->HostSupports32BitCoordinates)
		{
		gStuff->theRect32.top = 0;
		gStuff->theRect32.left = 0;
		gStuff->theRect32.bottom = 0;
		gStuff->theRect32.right = 0;
		}

	}

exit:

	if (inputBuffer != 0)
		FreeBuffer (inputBuffer);

	if (inputTransparencyBuffer != 0)
		FreeBuffer (inputTransparencyBuffer);
	
	}

/*****************************************************************************/

// for cases where there are no layers to blend, and the result would be zero
void DoWriteZero(GPtr globals)
	{
	
	AcquireRegion region;
	AcquireDataLayout layout;
	
	BufferID outputBuffer = 0;
	Ptr outputData;
	
	
	int32 imageWidth = gStuff->imageSize.h;
	int32 imageHeight = gStuff->imageSize.v;
	
	if (gStuff->HostSupports32BitCoordinates)
		{
		imageWidth = gStuff->imageSize32.h;
		imageHeight = gStuff->imageSize32.v;
		}


	int32 progressDone = 0;
	int32 tilesHigh = (imageHeight + (gTileSize -1)) / gTileSize;
	int32 tilesWide = (imageWidth + (gTileSize -1)) / gTileSize;
	int32 progressTotal = tilesHigh * tilesWide * gStuff->planes;
	
	// allocate one tile's worth for output
	// output buffer must be 32 bits, even if final output is 8 or 16
	int32 outputRowBytes = (gTileSize * gOutputDepth) >> 3;
	int32 outputBufferSize = gTileSize * outputRowBytes;
	
	layout.rowBytes = outputRowBytes;
	layout.colBytes = (gOutputDepth >> 3);
	layout.planeBytes = 0;	/* irrelevant -- we work one plane at a time */
	
	gResult = AllocateBuffer (outputBufferSize, &outputBuffer);	
	
	if (gResult != noErr)
		goto exit;
	
	outputData = LockBuffer (outputBuffer, false);
	
	
	// fill output buffer with zeros
	memset( outputData, 0, outputBufferSize );
	
	
	RANGE_ITER (region.rect.top, region.rect.bottom,
				0, imageHeight, gTileSize)
		{
		
		RANGE_ITER (region.rect.left, region.rect.right,
					0, imageWidth, gTileSize)
			{
			
			// iterate over the color planes
			for (region.loPlane = 0; region.loPlane < gStuff->planes; ++region.loPlane)
				{
			
				PIUpdateProgress (progressDone, progressTotal);
				progressDone += 1;
				
				if (TestAbort ())
					{
					gResult = userCanceledErr;
					goto exit;
					}
				
				region.hiPlane = region.loPlane;
				
				// buffer is already filled with zeros
				
				
				// store the result
				gResult = StoreData (gStuff, (void *)outputData, &region, &layout);
				
				if (gResult != noErr)
					goto exit;
				
				}	// iterate over channels
			
			}	// iterate horizontally
		
		}	// iterate vertically
	
	
	// we are done
	gStuff->data = NULL;
	SetRect (&gStuff->theRect, 0, 0, 0, 0);
	
	if (gStuff->HostSupports32BitCoordinates)
		{
		gStuff->theRect32.top = 0;
		gStuff->theRect32.left = 0;
		gStuff->theRect32.bottom = 0;
		gStuff->theRect32.right = 0;
		}
	
	
exit:

	if (outputBuffer != 0)
		FreeBuffer (outputBuffer);
	
	}

/*****************************************************************************/

// TODO break out the transparency reading code

// TODO break out the channel reading code

/*****************************************************************************/

void DoContinueMean (GPtr globals, bool isSummation)
	{
	
	AcquireRegion region;
	AcquireDataLayout layout;
	BufferID inputBuffer = 0;
	BufferID inputTransparencyBuffer = 0;
	BufferID outputBuffer = 0;
	BufferID countBuffer = 0;
	BufferID tempTransBuffer = 0;
	Ptr inputData;
	Ptr inputTransData;
	Ptr outputData;
	Ptr countData;
	Ptr tempTransData;
	
	
	// if there are no layers, just copy the composite
	if (gVisibleLayerCount < 2)
		{
		DoCopyComposite(globals);
		return;
		}
	
	int32 imageWidth = gStuff->imageSize.h;
	int32 imageHeight = gStuff->imageSize.v;
	
	int16 inputDepth = (int16) gStuff->documentInfo->depth;
	
	if (gStuff->HostSupports32BitCoordinates)
		{
		imageWidth = gStuff->imageSize32.h;
		imageHeight = gStuff->imageSize32.v;
		}


	int32 progressDone = 0;
	int32 tilesHigh = (imageHeight + (gTileSize -1)) / gTileSize;
	int32 tilesWide = (imageWidth + (gTileSize -1)) / gTileSize;
	int32 progressTotal = tilesHigh * tilesWide * gStuff->planes;


	/* Allocate the buffers */

	int32 inputRowBytes = (gTileSize * inputDepth) >> 3;
	int32 inputLayerBytes = gTileSize * inputRowBytes;
	int32 inputBufferSize = gVisibleLayerCount * inputLayerBytes;
	
	gResult = AllocateBuffer (inputBufferSize, &inputBuffer);
	
	if (gResult != noErr)
		goto exit;
	
	gResult = AllocateBuffer (inputBufferSize, &inputTransparencyBuffer);
	
	if (gResult != noErr)
		goto exit;
	
	gResult = AllocateBuffer (inputLayerBytes, &tempTransBuffer);
	
	if (gResult != noErr)
		goto exit;

	{
		
	// allocate one tile's worth for output
	// output buffer must be 32 bits, even if final output is 8 or 16
	int32 outputRowBytes = gTileSize * (32 >> 3);
	int32 outputBufferSize = gTileSize * outputRowBytes;
	
	layout.rowBytes = outputRowBytes;
	layout.colBytes = (gOutputDepth >> 3);
	layout.planeBytes = 0;	/* irrelevant -- we work one plane at a time */
	
	gResult = AllocateBuffer (outputBufferSize, &outputBuffer);	
	
	if (gResult != noErr)
		goto exit;
	
	gResult = AllocateBuffer (outputBufferSize, &countBuffer);	
	
	if (gResult != noErr)
		goto exit;

	
	inputData = LockBuffer (inputBuffer, false);
	inputTransData = LockBuffer (inputTransparencyBuffer, false);
	tempTransData = LockBuffer (tempTransBuffer, false);
	
	outputData = LockBuffer (outputBuffer, false);
	countData = LockBuffer (countBuffer, false);
	
	
	
	RANGE_ITER (region.rect.top, region.rect.bottom,
				0, imageHeight, gTileSize)
		{
		
		RANGE_ITER (region.rect.left, region.rect.right,
					0, imageWidth, gTileSize)
			{
	
			ReadLayerDesc *currentLayerDesc = gStuff->documentInfo->layersDescriptor;
			
			Ptr transPtr = inputTransData;
			
			int32 LAYER;
			
			// read the transparency information once

			for (LAYER = 0; LAYER < gLayerCount; ++LAYER)
				{
				
				if (currentLayerDesc == NULL)
					{
					gResult = acquireBadParameters;
					goto exit;
					}
				
				if (currentLayerDesc->isVisible && currentLayerDesc->isPixelBased)
					{
					
					// default to composite, in case there is only one layer
					ReadChannelDesc *transparencyDesc = gStuff->documentInfo->mergedCompositeChannels;
					
					if (gLayerCount > 1)
						transparencyDesc = currentLayerDesc->transparency;
					
					VRect writeRect = region.rect;
					
					if (transparencyDesc == NULL)
						// if there is no transparency, then this is a background layer, and opaque
						// so we need to write opaque values into the buffer
						gResult = FakeOpaqueIntoPlanarBuffer(
														globals,
														transparencyDesc,
														(void *)transPtr,
														writeRect,
														inputRowBytes,
														inputDepth );
					else
						gResult = ReadChannelPixelsIntoPlanarBuffer(
														globals,
														transparencyDesc,
														(void *)transPtr,
														writeRect,
														inputRowBytes,
														inputDepth );

					if (gResult != noErr)
						goto exit;

					
					if (currentLayerDesc && currentLayerDesc->layerMask != NULL)
						{
						// read layer mask into temporary buffer
						gResult = ReadChannelPixelsIntoPlanarBuffer(
														globals,
														currentLayerDesc->layerMask,
														(void *)tempTransData,
														writeRect,
														inputRowBytes,
														inputDepth );

						if (gResult != noErr)
							goto exit;
						
						// merge layer mask into layer transparency
						
						gResult = MergeTransparencyIntoPlanarBuffer(globals,
														(void *)transPtr,
														(void *)tempTransData,
														writeRect,
														inputRowBytes,
														inputDepth);
						
						if (gResult != noErr)
							goto exit;
						}
					
					if (currentLayerDesc && currentLayerDesc->vectorMask != NULL)
						{
						// read vector mask into temporary buffer
						gResult = ReadChannelPixelsIntoPlanarBuffer(
														globals,
														currentLayerDesc->vectorMask,
														(void *)tempTransData,
														writeRect,
														inputRowBytes,
														inputDepth );

						if (gResult != noErr)
							goto exit;
						
						// merge vector mask into layer transparency
						gResult = MergeTransparencyIntoPlanarBuffer(globals,
														(void *)transPtr,
														(void *)tempTransData,
														writeRect,
														inputRowBytes,
														inputDepth);
						
						if (gResult != noErr)
							goto exit;
						}
				
					transPtr += inputLayerBytes;
					
					}	// if isVisible
				
				// move on to the next layer
				if (gLayerCount > 1)
					currentLayerDesc = currentLayerDesc->next;
				
				}	// iterate over layers for transparency data
			
			
			
			// iterate over the color planes
			for (region.loPlane = 0; region.loPlane < gStuff->planes; ++region.loPlane)
				{
			
				PIUpdateProgress (progressDone, progressTotal);
				progressDone += 1;
				
				if (TestAbort ())
					{
					gResult = userCanceledErr;
					goto exit;
					}
				
				region.hiPlane = region.loPlane;
				
				// reset the layer pointer (!#$%@#$% linked list)
				currentLayerDesc = gStuff->documentInfo->layersDescriptor;
				
				Ptr layerPtr = inputData;
				
				// read the transparency information once
				for (LAYER = 0; LAYER < gLayerCount; ++LAYER)
					{
				
					if (currentLayerDesc == NULL)
						{
						gResult = acquireBadParameters;
						goto exit;
						}
				
					if (currentLayerDesc->isVisible && currentLayerDesc->isPixelBased)
						{
						
						// default to composite, in case there is only one layer
						ReadChannelDesc *currentChannelDesc = gStuff->documentInfo->mergedCompositeChannels;
						
						if (gLayerCount > 1)
							currentChannelDesc = currentLayerDesc->compositeChannelsList;
						
						
						// iterate to get to the right channel pointer  (!#$%@#$% linked list)
						int32 CHANNEL;
						for (CHANNEL = 0; CHANNEL < region.loPlane; ++CHANNEL)
							currentChannelDesc = currentChannelDesc->next;
						
						VRect writeRect = region.rect;

						gResult = ReadChannelPixelsIntoPlanarBuffer(
														globals,
														currentChannelDesc,
														(void *)layerPtr,
														writeRect,
														inputRowBytes,
														inputDepth );

						if (gResult != noErr)
							goto exit;
					
						layerPtr += inputLayerBytes;
						
						}	// if isVisible
					
					// move on to the next layer
					if (gLayerCount > 1)
						currentLayerDesc = currentLayerDesc->next;
					
					}	// iterate over layers for color data
				
				// we now have transparency data for all layers, and one color channel for all layers
				// process that into the output

				if (isSummation)
					gResult = DoAddLayers( inputData,
											inputTransData,
											outputData,
											region.rect,
											inputRowBytes,
											outputRowBytes,
											inputDepth,
											gOutputDepth,
											gVisibleLayerCount,
											inputLayerBytes);
				else
					gResult = DoAverageLayers( inputData,
											inputTransData,
											outputData,
											countData,
											region.rect,
											inputRowBytes,
											outputRowBytes,
											inputDepth,
											gOutputDepth,
											gVisibleLayerCount,
											inputLayerBytes);
										
				if (gResult != noErr)
					goto exit;
				
				
				// store the result
				gResult = StoreData (gStuff, (void *)outputData, &region, &layout);
				
				if (gResult != noErr)
					goto exit;
				
				}	// iterate over channels
			
			}	// iterate horizontally
		
		}	// iterate vertically


	// we are done
	gStuff->data = NULL;
	SetRect (&gStuff->theRect, 0, 0, 0, 0);
	
	if (gStuff->HostSupports32BitCoordinates)
		{
		gStuff->theRect32.top = 0;
		gStuff->theRect32.left = 0;
		gStuff->theRect32.bottom = 0;
		gStuff->theRect32.right = 0;
		}
		
	}
	
exit:

	if (inputBuffer != 0)
		FreeBuffer (inputBuffer);

	if (inputTransparencyBuffer != 0)
		FreeBuffer (inputTransparencyBuffer);
	
	if (outputBuffer != 0)
		FreeBuffer (outputBuffer);
	
	if (countBuffer != 0)
		FreeBuffer (countBuffer);
	
	if (tempTransBuffer != 0)
		FreeBuffer (tempTransBuffer);
		
	}

/*****************************************************************************/

/* This routine will always be called if DoStart does not return an error
   (even if DoContinue returns an error or the user aborts the operation).
   This allows the module to perform any needed cleanup. */

void DoFinishMean (GPtr globals)
	{

	gStuff->acquireAgain = false;

	/* Write our parameters if we can't finalize */
	CheckAndWriteScriptParams(globals); // writes script params
	
	}


/*****************************************************************************/
/*****************************************************************************/

// mostly copied from DoContinueMean
void DoStartMinMax (GPtr globals)
{

	/* Insist on having the necessary procs */
	if (!WarnAdvanceStateAvailable ()) {
		gResult = acquireBadParameters;
		return;
	}

	if (!WarnBufferProcsAvailable ()) {
		gResult = acquireBadParameters;
		return;
	}

	/* Assume we won't be coming back around for another pass unless explicitly so. */
	gStuff->acquireAgain = false;


	/* if stuff hasn't been initialized that we need, do it,
		then go check if we've got scripting commands to
		override our settings */
	OpenScriptParams (globals); // update our parameters with the scripting parameters, if available

	/* If we need to use our dialog, or it's already up, do it. */
	ValidateParameters (globals);

	if (gStuff->channelPortProcs == NULL
		|| gStuff->documentInfo == NULL) {
		gResult = acquireBadParameters;
		return;
	}
	
	// we need the new data
	if ( gStuff->documentInfo->maxVersion < 3) {
		gResult = acquireBadParameters;
		return;
	}
	
	if (gStuff->descriptorParameters->playInfo == plugInDialogDisplay)
		{
		// show our UI, if we had any
		}
	
	
	gStuff->planes = (int16) GetCompositeChannelCount(globals);
	
	gStuff->imageMode = (int16) gStuff->documentInfo->imageMode;
	
	
	// result is always the same as the source document
	gOutputDepth = gStuff->depth = (int16) gStuff->documentInfo->depth;
	
	
	// copy the document profile data
	if (gStuff->canUseICCProfiles)
		{
		gStuff->iCCprofileSize = gStuff->documentInfo->iCCprofileSize;
		
		if (gStuff->documentInfo->iCCprofileSize > 0)
			{
			
			// the handle will be freed by Photoshop
			Handle profileData = gStuff->handleProcs->newProc( gStuff->iCCprofileSize );
			
			if (profileData == NULL)
				{
				gResult = memFullErr;
				return;
				}
			
			Ptr tempData = gStuff->handleProcs->lockProc( profileData, false );
			
			if (tempData == NULL)
				{
				gResult = memFullErr;
				return;
				}
			
			PIBlockMove( (Ptr)gStuff->documentInfo->iCCprofileData, (Ptr)tempData, gStuff->iCCprofileSize );
			
			gStuff->handleProcs->unlockProc( profileData );
			
			gStuff->iCCprofileData = profileData;
			
			}
		
		}

	// if indexed color source and result, we need to copy the LUTs
	if (gStuff->imageMode == plugInModeIndexedColor
		&& gStuff->documentInfo->imageMode == plugInModeIndexedColor)
		{
		PIBlockMove( (Ptr)gStuff->documentInfo->redLUT, (Ptr)gStuff->redLUT, sizeof(LookUpTable) );
		PIBlockMove( (Ptr)gStuff->documentInfo->greenLUT, (Ptr)gStuff->greenLUT, sizeof(LookUpTable) );
		PIBlockMove( (Ptr)gStuff->documentInfo->blueLUT, (Ptr)gStuff->blueLUT, sizeof(LookUpTable) );
		}
	
	
	gLastRows = gStuff->documentInfo->bounds.bottom - gStuff->documentInfo->bounds.top;
	gLastCols = gStuff->documentInfo->bounds.right - gStuff->documentInfo->bounds.left;
	
	gStuff->imageSize.v = (short)gLastRows;
	gStuff->imageSize.h = (short)gLastCols;
	
	if (gStuff->HostSupports32BitCoordinates)
		{
		gStuff->PluginUsing32BitCoordinates = true;
		gStuff->imageSize32.v = gLastRows;
		gStuff->imageSize32.h = gLastCols;
		}
	
	gStuff->imageHRes = gStuff->documentInfo->hResolution;
	gStuff->imageVRes = gStuff->documentInfo->vResolution;
	
	gStuff->data = NULL;
	

	// work out our optimal tile size based on the number of layers and buffers
	gLayerCount = GetLayerCount(globals);
	gVisibleLayerCount = GetVisibleLayerCount(globals);
	
	if (gVisibleLayerCount < 2)
		{
		// resort to copying the composite if we have no layers or just a single layer
		
		gVisibleLayerCount = 1;	// so calculations won't fail
		gOutputDepth = static_cast<int16>(gStuff->documentInfo->depth);
		
		if (gStuff->documentInfo->mergedTransparency != 0)
			{
			gStuff->planes += 1;	// for transparency
			gStuff->isFirstAlphaTransparency = 1;
			}
		
		}
	
	// we can't really return a duotone document because the duotoneInfo field is undocumented!
	if (gStuff->imageMode == plugInModeDuotone)
		{
		gStuff->imageMode = plugInModeGrayScale;
		}
	
	// multichannel has no composite channels, just alpha channels
	if (gStuff->imageMode == plugInModeMultichannel)
		{
		gStuff->planes = static_cast<int16>(GetAlphaChannelCount(globals));
		}
	
	
	// make sure the image mode is legal for 32 bit!
	if (gOutputDepth == 32)
	if (gOutputDepth == 32)
		{
		int16 newMode = plugInModeGrayScale;
		
		switch (gStuff->imageMode)
			{
			case plugInModeBitmap:
			case plugInModeGrayScale:
			case plugInModeGray16:
			case plugInModeGray32:
			case plugInModeDuotone16:
			case plugInModeIndexedColor:
			case plugInModeDuotone:
				newMode = plugInModeGrayScale;
				break;
			
			default:
			case plugInModeRGB48:
			case plugInModeLab48:
			case plugInModeCMYK64:
			case plugInModeRGB96:
			case plugInModeRGBColor:
			case plugInModeCMYKColor:
			case plugInModeMultichannel:
			case plugInModeLabColor:
				newMode = plugInModeRGBColor;
				break;
			}
		
		if (newMode != gStuff->imageMode
			&& gStuff->iCCprofileData)
			{
			// if we're changing modes, get rid of the ICC profile - it'll be invalid
			
			gStuff->handleProcs->disposeProc( gStuff->iCCprofileData );
		
			gStuff->iCCprofileData = NULL;
			gStuff->iCCprofileSize = 0;
			
			}
		
		
		gStuff->imageMode = newMode;
		
		}
	
	gStuff->depth = gOutputDepth;
	
	
	
	// make sure we have the right scaling!
	if (gOutputDepth == 16)
		gStuff->maxValue = 32768;
	
	
	unsigned32 maxUsage = maxMemory;
	
	if (maxUsage > gMaxData)
		maxUsage = gMaxData;
	
	// how many buffers per layer are needed for calculation?
	// one channel plus transparency
	int32 bufferCount = 2;
	
	// tile size is square
	int32 tileSize = (int32) sqrt( (double)(maxUsage / ( gVisibleLayerCount * bufferCount + 1 )) );
	
	// and set a lower limit on the tile size for sanity's sake
	if (tileSize < minTileSize)
		tileSize = minTileSize;
	
	// also limit tile size to maximum dimension of the resulting image?
	int32 maxDimension = gLastRows;
	
	if (maxDimension < gLastCols)
		maxDimension = gLastCols;
	
	if (tileSize > maxDimension)
		tileSize = maxDimension;
	
	// should we establish a maximum tile size as well?
	if (tileSize > maxTileSize)
		tileSize = maxTileSize;
	
	gTileSize = tileSize;

}

/*****************************************************************************/

// mostly copied from DoContinueMean
void DoContinueMinMax (GPtr globals, minmax_type type)
	{
	
	AcquireRegion region;
	AcquireDataLayout layout;
	BufferID inputBuffer = 0;
	BufferID inputTransparencyBuffer = 0;
	BufferID outputBuffer = 0;
	BufferID tempTransBuffer = 0;
	Ptr inputData;
	Ptr inputTransData;
	Ptr outputData;
	Ptr tempTransData;
	
	
	// if there are no layers, just copy the composite
	if (gVisibleLayerCount < 2)
		{
		if (type == kRange)
			DoWriteZero(globals);
		else
			DoCopyComposite(globals);
		return;
		}
	
	int32 imageWidth = gStuff->imageSize.h;
	int32 imageHeight = gStuff->imageSize.v;
	
	int16 inputDepth = (int16) gStuff->documentInfo->depth;
	
	if (gStuff->HostSupports32BitCoordinates)
		{
		imageWidth = gStuff->imageSize32.h;
		imageHeight = gStuff->imageSize32.v;
		}


	int32 progressDone = 0;
	int32 tilesHigh = (imageHeight + (gTileSize -1)) / gTileSize;
	int32 tilesWide = (imageWidth + (gTileSize -1)) / gTileSize;
	int32 progressTotal = tilesHigh * tilesWide * gStuff->planes;


	/* Allocate the buffers */

	int32 inputRowBytes = (gTileSize * inputDepth) >> 3;
	int32 inputLayerBytes = gTileSize * inputRowBytes;
	int32 inputBufferSize = gVisibleLayerCount * inputLayerBytes;
	
	gResult = AllocateBuffer (inputBufferSize, &inputBuffer);
	
	if (gResult != noErr)
		goto exit;
	
	gResult = AllocateBuffer (inputBufferSize, &inputTransparencyBuffer);
	
	if (gResult != noErr)
		goto exit;
	
	gResult = AllocateBuffer (inputLayerBytes, &tempTransBuffer);
	
	if (gResult != noErr)
		goto exit;

	{
	
	// allocate one tile's worth for output
	int32 outputRowBytes = (gTileSize * gOutputDepth) >> 3;
	int32 outputBufferSize = gTileSize * outputRowBytes;
	
	layout.rowBytes = outputRowBytes;
	layout.colBytes = (gOutputDepth >> 3);
	layout.planeBytes = 0;	/* irrelevant -- we work one plane at a time */
	
	gResult = AllocateBuffer (outputBufferSize, &outputBuffer);	
	
	if (gResult != noErr)
		goto exit;

	
	inputData = LockBuffer (inputBuffer, false);
	inputTransData = LockBuffer (inputTransparencyBuffer, false);
	tempTransData = LockBuffer (tempTransBuffer, false);
	
	outputData = LockBuffer (outputBuffer, false);
	
	
	
	RANGE_ITER (region.rect.top, region.rect.bottom,
				0, imageHeight, gTileSize)
		{
		
		RANGE_ITER (region.rect.left, region.rect.right,
					0, imageWidth, gTileSize)
			{
	
			ReadLayerDesc *currentLayerDesc = gStuff->documentInfo->layersDescriptor;
			
			Ptr transPtr = inputTransData;
			
			int32 LAYER;
			
			// read the transparency information once
			for (LAYER = 0; LAYER < gLayerCount; ++LAYER)
				{
				
				if (currentLayerDesc == NULL)
					{
					gResult = acquireBadParameters;
					goto exit;
					}
				
				if (currentLayerDesc->isVisible && currentLayerDesc->isPixelBased)
					{
					
					// default to composite, in case there is only one layer
					ReadChannelDesc *transparencyDesc = gStuff->documentInfo->mergedCompositeChannels;
					
					if (gLayerCount > 1)
						transparencyDesc = currentLayerDesc->transparency;
					
					VRect writeRect = region.rect;

					
					if (transparencyDesc == NULL)
						// if there is no transparency, then this is a background layer, and opaque
						// so we need to write opaque values into the buffer
						gResult = FakeOpaqueIntoPlanarBuffer(
														globals,
														transparencyDesc,
														(void *)transPtr,
														writeRect,
														inputRowBytes,
														inputDepth );
					else
						gResult = ReadChannelPixelsIntoPlanarBuffer(
														globals,
														transparencyDesc,
														(void *)transPtr,
														writeRect,
														inputRowBytes,
														inputDepth );

					if (gResult != noErr)
						goto exit;

					
					if (currentLayerDesc->layerMask != NULL)
						{
						// read layer mask into temporary buffer
						gResult = ReadChannelPixelsIntoPlanarBuffer(
														globals,
														currentLayerDesc->layerMask,
														(void *)tempTransData,
														writeRect,
														inputRowBytes,
														inputDepth );

						if (gResult != noErr)
							goto exit;
						
						// merge layer mask into layer transparency
						
						gResult = MergeTransparencyIntoPlanarBuffer(globals,
														(void *)transPtr,
														(void *)tempTransData,
														writeRect,
														inputRowBytes,
														inputDepth);
						
						if (gResult != noErr)
							goto exit;
						}
					
					if (currentLayerDesc->vectorMask != NULL)
						{
						// read vector mask into temporary buffer
						gResult = ReadChannelPixelsIntoPlanarBuffer(
														globals,
														currentLayerDesc->vectorMask,
														(void *)tempTransData,
														writeRect,
														inputRowBytes,
														inputDepth );

						if (gResult != noErr)
							goto exit;
						
						// merge vector mask into layer transparency
						gResult = MergeTransparencyIntoPlanarBuffer(globals,
														(void *)transPtr,
														(void *)tempTransData,
														writeRect,
														inputRowBytes,
														inputDepth);
						
						if (gResult != noErr)
							goto exit;
						}
						
					transPtr += inputLayerBytes;
				
					}	// if isVisible
				
				// move on to the next layer
				if (gLayerCount > 1)
					currentLayerDesc = currentLayerDesc->next;
				
				}
			
			
			
			// iterate over the color planes
			for (region.loPlane = 0; region.loPlane < gStuff->planes; ++region.loPlane)
				{
			
				PIUpdateProgress (progressDone, progressTotal);
				progressDone += 1;
				
				if (TestAbort ())
					{
					gResult = userCanceledErr;
					goto exit;
					}
				
				region.hiPlane = region.loPlane;
				
				// reset the layer pointer (!#$%@#$% linked list)
				currentLayerDesc = gStuff->documentInfo->layersDescriptor;
				
				Ptr layerPtr = inputData;
				
				// read the transparency information once
				for (LAYER = 0; LAYER < gLayerCount; ++LAYER)
					{
					
					if (currentLayerDesc == NULL)
						{
						gResult = acquireBadParameters;
						goto exit;
						}
				
					if (currentLayerDesc->isVisible && currentLayerDesc->isPixelBased)
						{
						
						// default to composite, in case there is only one layer
						ReadChannelDesc *currentChannelDesc = gStuff->documentInfo->mergedCompositeChannels;
						
						if (gLayerCount > 1)
							currentChannelDesc = currentLayerDesc->compositeChannelsList;
						
						
						// iterate to get to the right channel pointer  (!#$%@#$% linked list)
						int32 CHANNEL;
						for (CHANNEL = 0; CHANNEL < region.loPlane; ++CHANNEL)
							currentChannelDesc = currentChannelDesc->next;
						
						VRect writeRect = region.rect;

						gResult = ReadChannelPixelsIntoPlanarBuffer(
														globals,
														currentChannelDesc,
														(void *)layerPtr,
														writeRect,
														inputRowBytes,
														inputDepth );

						if (gResult != noErr)
							goto exit;
						
						layerPtr += inputLayerBytes;
						}	// if isVisible
					
					// move on to the next layer
					if (gLayerCount > 1)
						currentLayerDesc = currentLayerDesc->next;
					
					}
				
				// we now have transparency data for all layers, and one color channel for all layers
				// process that into the output
				
				if (type == kMinimum || type == kMaximum)
					gResult = DoMinMaxLayers( inputData,
											inputTransData,
											outputData,
											region.rect,
											inputRowBytes,
											outputRowBytes,
											inputDepth,
											gOutputDepth,
											gVisibleLayerCount,
											inputLayerBytes,
											type == kMaximum);
				else if (type == kRange)
					gResult = DoRangeLayers(inputData,
											inputTransData,
											outputData,
											region.rect,
											inputRowBytes,
											outputRowBytes,
											inputDepth,
											gOutputDepth,
											gVisibleLayerCount,
											inputLayerBytes);
				else if (type == kMedian)
					gResult = DoMedianLayers(inputData,
											inputTransData,
											outputData,
											region.rect,
											inputRowBytes,
											outputRowBytes,
											inputDepth,
											gOutputDepth,
											gVisibleLayerCount,
											inputLayerBytes);
										
				if (gResult != noErr)
					goto exit;
				
				
				// store the result
				gResult = StoreData (gStuff, (void *)outputData, &region, &layout);
				
				if (gResult != noErr)
					goto exit;
				
				}	// iterate over channels
			
			}	// iterate horizontally
		
		}	// iterate vertically


	// we are done
	gStuff->data = NULL;
	SetRect (&gStuff->theRect, 0, 0, 0, 0);
	
	if (gStuff->HostSupports32BitCoordinates)
		{
		gStuff->theRect32.top = 0;
		gStuff->theRect32.left = 0;
		gStuff->theRect32.bottom = 0;
		gStuff->theRect32.right = 0;
		}
		
	}
	
exit:

	if (inputBuffer != 0)
		FreeBuffer (inputBuffer);

	if (inputTransparencyBuffer != 0)
		FreeBuffer (inputTransparencyBuffer);
	
	if (outputBuffer != 0)
		FreeBuffer (outputBuffer);
	
	if (tempTransBuffer != 0)
		FreeBuffer (tempTransBuffer);
		
	}

/*****************************************************************************/

/* This routine will always be called if DoStart does not return an error
   (even if DoContinue returns an error or the user aborts the operation).
   This allows the module to perform any needed cleanup. */

void DoFinishMinMax (GPtr globals)
	{

	gStuff->acquireAgain = false;

	/* Write our parameters if we can't finalize */
	CheckAndWriteScriptParams(globals); // writes script params
	
	}


/*****************************************************************************/
/*****************************************************************************/

void DoStartVariance (GPtr globals, stat_type type)
{

	/* Insist on having the necessary procs */
	if (!WarnAdvanceStateAvailable ()) {
		gResult = acquireBadParameters;
		return;
	}

	if (!WarnBufferProcsAvailable ()) {
		gResult = acquireBadParameters;
		return;
	}

	/* Assume we won't be coming back around for another pass unless explicitly so. */
	gStuff->acquireAgain = false;


	/* if stuff hasn't been initialized that we need, do it,
		then go check if we've got scripting commands to
		override our settings */
	OpenScriptParams (globals); // update our parameters with the scripting parameters, if available

	/* If we need to use our dialog, or it's already up, do it. */
	ValidateParameters (globals);

	if ( gStuff->channelPortProcs == NULL
		|| gStuff->documentInfo == NULL) {
		gResult = acquireBadParameters;
		return;
	}
	
	// we need the new data
	if ( gStuff->documentInfo->maxVersion < 3) {
		gResult = acquireBadParameters;
		return;
	}
	
	if (gStuff->descriptorParameters->playInfo == plugInDialogDisplay)
		{
		// show our UI, if we had any
		}
	
	
	gStuff->planes = (int16) GetCompositeChannelCount(globals);
	
	gStuff->imageMode = (int16) gStuff->documentInfo->imageMode;
	
	
	// result is always 32 bit for variance and stdDeviation
	gOutputDepth = 32;


	// TODO - that may not be a good idea, since it'll force a conversion
	// but I don't know the parent document profile!
	// this is a purely numerical result - no ICC profile!
	
	// copy the document profile data
	if (gStuff->canUseICCProfiles)
		{
		gStuff->iCCprofileSize = gStuff->documentInfo->iCCprofileSize;
		
		if (gStuff->documentInfo->iCCprofileSize > 0)
			{
			
			// the handle will be freed by Photoshop
			Handle profileData = gStuff->handleProcs->newProc( gStuff->iCCprofileSize );
			
			if (profileData == NULL)
				{
				gResult = memFullErr;
				return;
				}
			
			Ptr tempData = gStuff->handleProcs->lockProc( profileData, false );
			
			if (tempData == NULL)
				{
				gResult = memFullErr;
				return;
				}
			
			PIBlockMove( (Ptr)gStuff->documentInfo->iCCprofileData, (Ptr)tempData, gStuff->iCCprofileSize );
			
			gStuff->handleProcs->unlockProc( profileData );
			
			gStuff->iCCprofileData = profileData;
			
			}
		
		}

	// if indexed color source and result, we need to copy the LUTs
	if (gStuff->imageMode == plugInModeIndexedColor
		&& gStuff->documentInfo->imageMode == plugInModeIndexedColor)
		{
		PIBlockMove( (Ptr)gStuff->documentInfo->redLUT, (Ptr)gStuff->redLUT, sizeof(LookUpTable) );
		PIBlockMove( (Ptr)gStuff->documentInfo->greenLUT, (Ptr)gStuff->greenLUT, sizeof(LookUpTable) );
		PIBlockMove( (Ptr)gStuff->documentInfo->blueLUT, (Ptr)gStuff->blueLUT, sizeof(LookUpTable) );
		}

	gLastRows = gStuff->documentInfo->bounds.bottom - gStuff->documentInfo->bounds.top;
	gLastCols = gStuff->documentInfo->bounds.right - gStuff->documentInfo->bounds.left;
	
	gStuff->imageSize.v = (short)gLastRows;
	gStuff->imageSize.h = (short)gLastCols;
	
	if (gStuff->HostSupports32BitCoordinates)
		{
		gStuff->PluginUsing32BitCoordinates = true;
		gStuff->imageSize32.v = gLastRows;
		gStuff->imageSize32.h = gLastCols;
		}
	
	gStuff->imageHRes = gStuff->documentInfo->hResolution;
	gStuff->imageVRes = gStuff->documentInfo->vResolution;
	
	gStuff->data = NULL;
	

	// work out our optimal tile size based on the number of layers and buffers
	gLayerCount = GetLayerCount(globals);
	gVisibleLayerCount = GetVisibleLayerCount(globals);
	
	if (gVisibleLayerCount < 2)
		{
		// resort to copying the composite if we have no layers or just a single layer
		
		gVisibleLayerCount = 1;	// so calculations won't fail
		gOutputDepth = static_cast<int16>(gStuff->documentInfo->depth);
		
		if (gStuff->documentInfo->mergedTransparency != 0)
			{
			gStuff->planes += 1;	// for transparency
			gStuff->isFirstAlphaTransparency = 1;
			}
		
		}
	
	// we can't really return a duotone document because the duotoneInfo field is undocumented!
	if (gStuff->imageMode == plugInModeDuotone)
		{
		gStuff->imageMode = plugInModeGrayScale;
		}
	
	// multichannel has no composite channels, just alpha channels
	if (gStuff->imageMode == plugInModeMultichannel)
		{
		gStuff->planes = static_cast<int16>(GetAlphaChannelCount(globals));
		}
		
	
	// make sure the image mode is legal for 32 bit!
	if (gOutputDepth == 32)
		{
		int16 newMode = plugInModeGrayScale;
		
		switch (gStuff->imageMode)
			{
			case plugInModeBitmap:
			case plugInModeGrayScale:
			case plugInModeGray16:
			case plugInModeGray32:
			case plugInModeDuotone16:
			case plugInModeIndexedColor:
			case plugInModeDuotone:
				newMode = plugInModeGrayScale;
				break;
			
			default:
			case plugInModeRGB48:
			case plugInModeLab48:
			case plugInModeCMYK64:
			case plugInModeRGB96:
			case plugInModeRGBColor:
			case plugInModeCMYKColor:
			case plugInModeMultichannel:
			case plugInModeLabColor:
				newMode = plugInModeRGBColor;
				break;
			}
		
		if (newMode != gStuff->imageMode
			&& gStuff->iCCprofileData)
			{
			// if we're changing modes, get rid of the ICC profile - it'll be invalid
			
			gStuff->handleProcs->disposeProc( gStuff->iCCprofileData );
		
			gStuff->iCCprofileData = NULL;
			gStuff->iCCprofileSize = 0;
			
			}
		
		
		gStuff->imageMode = newMode;
		
		}
	
	gStuff->depth = gOutputDepth;
	
	
	
	// make sure we have the right scaling!
	if (gOutputDepth == 16)
		gStuff->maxValue = 32768;
	
	
	unsigned32 maxUsage = maxMemory;
	
	if (maxUsage > gMaxData)
		maxUsage = gMaxData;
	
	// how many buffers per layer are needed for calculation?
	// one channel plus transparency
	int32 bufferCount = 2;
	
	// tile size is square
	int32 tileSize = (int32) sqrt( (double)(maxUsage / ( gVisibleLayerCount * bufferCount + 1 )) );
	
	// and set a lower limit on the tile size for sanity's sake
	if (tileSize < minTileSize)
		tileSize = minTileSize;
	
	// also limit tile size to maximum dimension of the resulting image?
	int32 maxDimension = gLastRows;
	
	if (maxDimension < gLastCols)
		maxDimension = gLastCols;
	
	if (tileSize > maxDimension)
		tileSize = maxDimension;
	
	// should we establish a maximum tile size as well?
	if (tileSize > maxTileSize)
		tileSize = maxTileSize;
	
	gTileSize = tileSize;

}

/*****************************************************************************/

void DoContinueVariance (GPtr globals, stat_type type)
	{
	
	AcquireRegion region;
	AcquireDataLayout layout;
	BufferID inputBuffer = 0;
	BufferID inputTransparencyBuffer = 0;
	BufferID outputBuffer = 0;
	BufferID countBuffer = 0;
	BufferID tempTransBuffer = 0;
	Ptr inputData;
	Ptr inputTransData;
	Ptr outputData;
	Ptr countData;
	Ptr tempTransData;
	
	
	// if there are no layers, just copy the composite
	if (gVisibleLayerCount < 2)
		{
		DoWriteZero(globals);
		return;
		}
	
	int32 imageWidth = gStuff->imageSize.h;
	int32 imageHeight = gStuff->imageSize.v;
	
	int16 inputDepth = (int16) gStuff->documentInfo->depth;
	
	if (gStuff->HostSupports32BitCoordinates)
		{
		imageWidth = gStuff->imageSize32.h;
		imageHeight = gStuff->imageSize32.v;
		}


	int32 progressDone = 0;
	int32 tilesHigh = (imageHeight + (gTileSize -1)) / gTileSize;
	int32 tilesWide = (imageWidth + (gTileSize -1)) / gTileSize;
	int32 progressTotal = tilesHigh * tilesWide * gStuff->planes;


	/* Allocate the buffers */

	int32 inputRowBytes = (gTileSize * inputDepth) >> 3;
	int32 inputLayerBytes = gTileSize * inputRowBytes;
	int32 inputBufferSize = gVisibleLayerCount * inputLayerBytes;
	
	gResult = AllocateBuffer (inputBufferSize, &inputBuffer);
	
	if (gResult != noErr)
		goto exit;
	
	gResult = AllocateBuffer (inputBufferSize, &inputTransparencyBuffer);
	
	if (gResult != noErr)
		goto exit;
	
	gResult = AllocateBuffer (inputLayerBytes, &tempTransBuffer);
	
	if (gResult != noErr)
		goto exit;

	{
			
	// allocate one tile's worth for output
	// output buffer must be 32 bits, even if final output is 8 or 16
	int32 outputRowBytes = gTileSize * (32 >> 3);
	int32 outputBufferSize = gTileSize * outputRowBytes;
	
	layout.rowBytes = outputRowBytes;
	layout.colBytes = (gOutputDepth >> 3);
	layout.planeBytes = 0;	/* irrelevant -- we work one plane at a time */
	
	gResult = AllocateBuffer (outputBufferSize, &outputBuffer);	
	
	if (gResult != noErr)
		goto exit;
	
	gResult = AllocateBuffer (outputBufferSize, &countBuffer);	
	
	if (gResult != noErr)
		goto exit;

	
	inputData = LockBuffer (inputBuffer, false);
	inputTransData = LockBuffer (inputTransparencyBuffer, false);
	tempTransData = LockBuffer (tempTransBuffer, false);
	
	outputData = LockBuffer (outputBuffer, false);
	countData = LockBuffer (countBuffer, false);
	
	
	
	RANGE_ITER (region.rect.top, region.rect.bottom,
				0, imageHeight, gTileSize)
		{
		
		RANGE_ITER (region.rect.left, region.rect.right,
					0, imageWidth, gTileSize)
			{
	
			ReadLayerDesc *currentLayerDesc = gStuff->documentInfo->layersDescriptor;
			
			Ptr transPtr = inputTransData;
			
			int32 LAYER;
			
			// read the transparency information once

			for (LAYER = 0; LAYER < gLayerCount; ++LAYER)
				{
				
				if (currentLayerDesc == NULL)
					{
					gResult = acquireBadParameters;
					goto exit;
					}
				
				if (currentLayerDesc->isVisible && currentLayerDesc->isPixelBased)
					{
					
					// default to composite, in case there is only one layer
					ReadChannelDesc *transparencyDesc = gStuff->documentInfo->mergedCompositeChannels;
					
					if (gLayerCount > 1)
						transparencyDesc = currentLayerDesc->transparency;
					
					VRect writeRect = region.rect;

					
					if (transparencyDesc == NULL)
						// if there is no transparency, then this is a background layer, and opaque
						// so we need to write opaque values into the buffer
						gResult = FakeOpaqueIntoPlanarBuffer(
														globals,
														transparencyDesc,
														(void *)transPtr,
														writeRect,
														inputRowBytes,
														inputDepth );
					else
						gResult = ReadChannelPixelsIntoPlanarBuffer(
														globals,
														transparencyDesc,
														(void *)transPtr,
														writeRect,
														inputRowBytes,
														inputDepth );

					if (gResult != noErr)
						goto exit;

					
					if (currentLayerDesc && currentLayerDesc->layerMask != NULL)
						{
						// read layer mask into temporary buffer
						gResult = ReadChannelPixelsIntoPlanarBuffer(
														globals,
														currentLayerDesc->layerMask,
														(void *)tempTransData,
														writeRect,
														inputRowBytes,
														inputDepth );

						if (gResult != noErr)
							goto exit;
						
						// merge layer mask into layer transparency
						
						gResult = MergeTransparencyIntoPlanarBuffer(globals,
														(void *)transPtr,
														(void *)tempTransData,
														writeRect,
														inputRowBytes,
														inputDepth);
						
						if (gResult != noErr)
							goto exit;
						}
					
					if (currentLayerDesc && currentLayerDesc->vectorMask != NULL)
						{
						// read vector mask into temporary buffer
						gResult = ReadChannelPixelsIntoPlanarBuffer(
														globals,
														currentLayerDesc->vectorMask,
														(void *)tempTransData,
														writeRect,
														inputRowBytes,
														inputDepth );

						if (gResult != noErr)
							goto exit;
						
						// merge vector mask into layer transparency
						gResult = MergeTransparencyIntoPlanarBuffer(globals,
														(void *)transPtr,
														(void *)tempTransData,
														writeRect,
														inputRowBytes,
														inputDepth);
						
						if (gResult != noErr)
							goto exit;
						}
					
					transPtr += inputLayerBytes;
					
					}	// if isVisible
				
				// move on to the next layer
				if (gLayerCount > 1)
					currentLayerDesc = currentLayerDesc->next;
				
				}	// iterate over layers for transparency data
			
			
			
			// iterate over the color planes
			for (region.loPlane = 0; region.loPlane < gStuff->planes; ++region.loPlane)
				{
			
				PIUpdateProgress (progressDone, progressTotal);
				progressDone += 1;
				
				if (TestAbort ())
					{
					gResult = userCanceledErr;
					goto exit;
					}
				
				region.hiPlane = region.loPlane;
				
				// reset the layer pointer (!#$%@#$% linked list)
				currentLayerDesc = gStuff->documentInfo->layersDescriptor;
				
				Ptr layerPtr = inputData;
				
				// read the transparency information once
				for (LAYER = 0; LAYER < gLayerCount; ++LAYER)
					{
				
					if (currentLayerDesc == NULL)
						{
						gResult = acquireBadParameters;
						goto exit;
						}
				
					if (currentLayerDesc->isVisible && currentLayerDesc->isPixelBased)
						{
						
						// default to composite, in case there is only one layer
						ReadChannelDesc *currentChannelDesc = gStuff->documentInfo->mergedCompositeChannels;
						
						if (gLayerCount > 1)
							currentChannelDesc = currentLayerDesc->compositeChannelsList;
						
						
						// iterate to get to the right channel pointer  (!#$%@#$% linked list)
						int32 CHANNEL;
						for (CHANNEL = 0; CHANNEL < region.loPlane; ++CHANNEL)
							currentChannelDesc = currentChannelDesc->next;
						
						VRect writeRect = region.rect;

						gResult = ReadChannelPixelsIntoPlanarBuffer(
														globals,
														currentChannelDesc,
														(void *)layerPtr,
														writeRect,
														inputRowBytes,
														inputDepth );

						if (gResult != noErr)
							goto exit;
						
						layerPtr += inputLayerBytes;
						
						} // if isVisible
					
					// move on to the next layer
					if (gLayerCount > 1)
						currentLayerDesc = currentLayerDesc->next;
					
					}	// iterate over layers for color data
				
				// we now have transparency data for all layers, and one color channel for all layers
				// process that into the output
				if (type == kVariance || type == kStdDeviation )
					gResult = DoVarianceLayers( inputData,
												inputTransData,
												outputData,
												countData,
												region.rect,
												inputRowBytes,
												outputRowBytes,
												inputDepth,
												gOutputDepth,
												gVisibleLayerCount,
												inputLayerBytes);
				else if (type == kSkewness)
					gResult = DoSkewnessLayers( inputData,
												inputTransData,
												outputData,
												countData,
												region.rect,
												inputRowBytes,
												outputRowBytes,
												inputDepth,
												gOutputDepth,
												gVisibleLayerCount,
												inputLayerBytes);
				else if (type == kKurtosis)
					gResult = DoKurtosisLayers( inputData,
												inputTransData,
												outputData,
												countData,
												region.rect,
												inputRowBytes,
												outputRowBytes,
												inputDepth,
												gOutputDepth,
												gVisibleLayerCount,
												inputLayerBytes);
				else if (type == kEntropy)
					gResult = DoEntropyLayers( inputData,
												inputTransData,
												outputData,
												region.rect,
												inputRowBytes,
												outputRowBytes,
												inputDepth,
												gOutputDepth,
												gVisibleLayerCount,
												inputLayerBytes);
				
				if (gResult != noErr)
					goto exit;
				
				if (type == kStdDeviation)
					DoSquareRootBuffer( outputData,
										region.rect,
										outputRowBytes,
										gOutputDepth);
				
				
				// store the result
				gResult = StoreData (gStuff, (void *)outputData, &region, &layout);
				
				if (gResult != noErr)
					goto exit;
				
				}	// iterate over channels
			
			}	// iterate horizontally
		
		}	// iterate vertically


	// we are done
	gStuff->data = NULL;
	SetRect (&gStuff->theRect, 0, 0, 0, 0);
	
	if (gStuff->HostSupports32BitCoordinates)
		{
		gStuff->theRect32.top = 0;
		gStuff->theRect32.left = 0;
		gStuff->theRect32.bottom = 0;
		gStuff->theRect32.right = 0;
		}
	
	}

exit:

	if (inputBuffer != 0)
		FreeBuffer (inputBuffer);

	if (inputTransparencyBuffer != 0)
		FreeBuffer (inputTransparencyBuffer);
	
	if (outputBuffer != 0)
		FreeBuffer (outputBuffer);
	
	if (countBuffer != 0)
		FreeBuffer (countBuffer);
	
	if (tempTransBuffer != 0)
		FreeBuffer (tempTransBuffer);
		
	}

/*****************************************************************************/

/* This routine will always be called if DoStart does not return an error
   (even if DoContinue returns an error or the user aborts the operation).
   This allows the module to perform any needed cleanup. */

void DoFinishVariance (GPtr globals)
	{

	gStuff->acquireAgain = false;

	/* Write our parameters if we can't finalize */
	CheckAndWriteScriptParams(globals); // writes script params
	
	}

/*****************************************************************************/

void DoFinalize (GPtr globals)
	{
	/* does nothing for now */
	}

/*****************************************************************************/

