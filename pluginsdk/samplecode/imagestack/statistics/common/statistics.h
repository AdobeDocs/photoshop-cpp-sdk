/*
	File: statistics.h

	Copyright © 1993-6, Adobe Systems Incorporated.
	All rights reserved.

	Main header for statistics example.
*/

#ifndef __statistics_H__
#define __statistics_H__

#include "PITypes.h"
#include "PIGeneral.h"
#include "PIAcquire.h"
#include "PIActions.h"
#include "PIUtilities.h"
#include "statisticsRes.h"

/*****************************************************************************/
/* Dialog constants.														 */

#define dRowsItem		101
#define dColsItem		102
#define d8BitItem		103
#define d16BitItem		104
#define d32BitItem		105

/*****************************************************************************/
/* Value bounds.															 */

#define kRowsMin			1
#define kRowsMax			2000000
#define kColumnsMin			1
#define kColumnsMax			2000000

/*****************************************************************************/
/* The region of the image being acquired (an area and a range of planes).   */

typedef struct AcquireRegion
	{
	VRect rect;
	int16 loPlane;
	int16 hiPlane;
	}
AcquireRegion;

/* The layout of the data.                                                   */

typedef struct AcquireDataLayout
	{
	int32 rowBytes;
	int32 colBytes;
	int32 planeBytes;
	}
AcquireDataLayout;
	
/*****************************************************************************/

OSErr StoreData (AcquireRecord *stuff,
				 void *data,
				 AcquireRegion *region,
				 AcquireDataLayout *layout);

OSErr FetchData (AcquireRecord *stuff,
				 void *data,
				 AcquireRegion *region,
				 AcquireDataLayout *layout);

/*****************************************************************************/

#if __PIWin__
extern HANDLE hDllInstance;
#endif

typedef struct Globals
	{

	short result;
	AcquireRecord *stuff;
	
	int32		tileSize;
	int32 		layerCount;
	int32		visibleLayerCount;
	int32		channelCount;
	unsigned32	maxDataSize;
	
	int32 		lastRows;
	int32 		lastCols;
	int16		outputDepth;
	Boolean 	initParameters;
	
	} Globals, *GPtr;

/*****************************************************************************/

#define gResult (globals->result)
#define gStuff  (globals->stuff)

#define gLastRows (globals->lastRows)
#define gLastCols (globals->lastCols)

#define gOutputDepth (globals->outputDepth)

#define gMaxData (globals->maxDataSize)

#define gTileSize (globals->tileSize)
#define gChannelCount (globals->channelCount)
#define gLayerCount (globals->layerCount)
#define gVisibleLayerCount (globals->visibleLayerCount)


#define gInitParameters (globals->initParameters)

/*****************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

void DoAbout (AboutRecordPtr aboutPtr);
void OpenScriptParams (GPtr globals);
OSErr WriteScriptParams (GPtr globals);
OSErr CheckAndWriteScriptParams (GPtr globals);

#ifdef __cplusplus
}
#endif

/*****************************************************************************/

#endif /* __statistics_H__ */
