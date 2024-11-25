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
//		LayerFormat.cpp
//
//	Description:
//		This file contains the header prototypes and macros for the
//		File Format module LayerFormat, 
//		which writes a flat file with merged document pixels.
//
//	Use:
//		Format modules are called from the Save, Save as,
//		and Save a copy dialogs.
//
//-------------------------------------------------------------------------------
#define _CRT_NONSTDC_NO_DEPRECATE  1
#define _CRT_SECURE_NO_DEPRECATE 1
#define _CRT_SECURE_NO_WARNINGS 1

#pragma warning( disable : 4996 )

//-------------------------------------------------------------------------------
//	Includes
//-------------------------------------------------------------------------------
#include <vector>
#include <sstream>
#include <time.h>
#include "LayerFormat.h"
#include "PIUI.h"

using namespace std;
//-------------------------------------------------------------------------------
//	Prototypes
//-------------------------------------------------------------------------------
static void DoReadPrepare (void);
static void DoReadStart (void);
static void DoReadContinue (void);
static void DoReadFinish (void);
static void DoOptionsPrepare (void);
static void DoOptionsStart (void);
static void DoOptionsContinue (void);
static void DoOptionsFinish (void);
static void DoEstimatePrepare (void);
static void DoEstimateStart (void);
static void DoEstimateContinue (void);
static void DoEstimateFinish (void);
static void DoWritePrepare (void);
static void DoWriteStart (void);
static void DoWriteContinue (void);
static void DoWriteFinish (void);
static void DoFilterFile (void);
static void DoWriteLayerStart (void);
static void DoWriteLayerContinue (void);
static void DoWriteLayerFinish (void);
static void DoReadLayerStart (void);
static void DoReadLayerContinue (void);
static void DoReadLayerFinish (void);

static void AddComment (void);

const int32 HEADER_CANT_READ = 0;
const int32 HEADER_VER1 = 1;
const int32 HEADER_VER2 = 2;

// let us use the TIFF spec. to do cross platform files
const int16 BIGENDIAN = 0x4d4d;
const int16 LITTLEENDIAN = 0x4949;
const int16 TESTENDIAN = 0x002a;

const int32 DESIREDMATTING = 0;

static int CheckIdentifier (char identifier []);
static void SetIdentifier (char identifier []);
static int32 RowBytes (void);

static void ReadSome (int32 count, void * buffer);
static void WriteSome (int32 count, void * buffer);
static void ReadRow (Ptr pixelData, bool needsSwap);
static void WriteRow (Ptr pixelData);
static void DisposeImageResources (void);
static void SwapRow(int32 rowBytes, Ptr pixelData);

static void DoReadICCProfile(void);
static void DoWriteICCProfile(void);

static void InitData(void);
static void CreateDataPtr(void);

static VPoint GetFormatImageSize(void);
static void SetFormatImageSize(VPoint inPoint);
static void SetFormatTheRect(VRect inRect);


template <typename Result>
inline Result GetBigEndian(uint8 *& source)
{
	Result result (0);
	
	for (int bytes (0); bytes < sizeof(Result); ++bytes) 
	{
		result = result << 8;
		result |= *source++;
	}
	
	return result;
}

static void CreateResourceInfoVector(int32 length, 
									 uint8 * resources, 
									 vector<ResourceInfo *> & rInfos);
static void DeleteResourceInfoVector(vector<ResourceInfo *> & rInfos);
static int32 RemoveResources(int32 resourceLength, 
							 const vector<ResourceInfo *> & rInfos, 
							 uint8 * resourcePtr);
static int32 RemoveResource(uint32 type, 
							uint16 id, 
							uint8 * imageResource, 
							int32 size, 
							uint8 * dest);
//-------------------------------------------------------------------------------
//	Globals -- Define global variables for plug-in scope.
//-------------------------------------------------------------------------------

SPBasicSuite * sSPBasic = NULL;
SPPluginRef gPluginRef = NULL;

FormatRecord * gFormatRecord = NULL;
Data * gData = NULL;
int16 * gResult = NULL;
FileHeader gHeader;
// Photoshop UI limits this to 255 chars plus the end
const uint16 MAXLAYERNAMELENGTH = 256;
uint16  gLayerName[MAXLAYERNAMELENGTH];

#define gCountResources gFormatRecord->resourceProcs->countProc
#define gGetResources   gFormatRecord->resourceProcs->getProc
#define gAddResource	gFormatRecord->resourceProcs->addProc

//-------------------------------------------------------------------------------
//
//	PluginMain / main
//
//	All calls to the plug-in module come through this routine.
//	It must be placed first in the resource.  To achieve this,
//	most development systems require this be the first routine
//	in the source.
//
//	The entry point will be "pascal void" for Macintosh,
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
//		int32 * data								Use this to store a handle to our
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
						             FormatRecordPtr formatRecordPtr,
						             intptr_t * data,
						             int16 * result)
{

  try {

	//---------------------------------------------------------------------------
	//	(1) Update our global parameters from the passed in values.
	// 
	//	We removed that nasty passing around of globals. It's global right! So
	//	why pass it around. This also removes the use of some those nasty #defines.
	//---------------------------------------------------------------------------
	gFormatRecord = formatRecordPtr;
	gPluginRef = reinterpret_cast<SPPluginRef>(gFormatRecord->plugInRef);
	gResult = result;
	gData = (Data*)data;

	//---------------------------------------------------------------------------
	//	(2) Check for about box request.
	//
	// 	The about box is a special request; the parameter block is not filled
	// 	out, none of the callbacks or standard data is available.  Instead,
	// 	the parameter block points to an AboutRecord, which is used
	// 	on Windows.
	//---------------------------------------------------------------------------
	if (selector == formatSelectorAbout)
	{
		AboutRecordPtr aboutRecordPtr = reinterpret_cast<AboutRecordPtr>(formatRecordPtr);
		sSPBasic = aboutRecordPtr->sSPBasic;
		gPluginRef = reinterpret_cast<SPPluginRef>(aboutRecordPtr->plugInRef);
		//DoAbout(gPluginRef, AboutID);
	}
	else
	{ // do the rest of the process as normal:

		sSPBasic = formatRecordPtr->sSPBasic;

		if (gCountResources == NULL ||
            gGetResources == NULL ||
            gAddResource == NULL ||
			gFormatRecord->advanceState == NULL)
		{
			*gResult = errPlugInHostInsufficient;
			return;
		}

		// new for Photoshop 8, big documents, rows and columns are now > 30000 pixels
		if (gFormatRecord->HostSupports32BitCoordinates != 0)
			gFormatRecord->PluginUsing32BitCoordinates = true;

		//-----------------------------------------------------------------------
		//	(3) Allocate and initialize globals.
		//
		//-----------------------------------------------------------------------

 		if (gData == NULL)
		{
			CreateDataPtr();
			if (*gResult != noErr)
				return;
			*data = (intptr_t)gData;
			InitData();
		}

		//-----------------------------------------------------------------------
		//	(4) Dispatch selector.
		//-----------------------------------------------------------------------
		switch (selector)
		{
			case formatSelectorReadPrepare:
				DoReadPrepare();
				break;
			case formatSelectorReadStart:
				DoReadStart();
				break;
			case formatSelectorReadContinue:
				DoReadContinue();
				break;
			case formatSelectorReadFinish:
				DoReadFinish();
				break;

			case formatSelectorOptionsPrepare:
				DoOptionsPrepare();
				break;
			case formatSelectorOptionsStart:
				DoOptionsStart();
				break;
			case formatSelectorOptionsContinue:
				DoOptionsContinue();
				break;
			case formatSelectorOptionsFinish:
				DoOptionsFinish();
				break;

			case formatSelectorEstimatePrepare:
				DoEstimatePrepare();
				break;
			case formatSelectorEstimateStart:
				DoEstimateStart();
				break;
			case formatSelectorEstimateContinue:
				DoEstimateContinue();
				break;
			case formatSelectorEstimateFinish:
				DoEstimateFinish();
				break;

			case formatSelectorWritePrepare:
				DoWritePrepare();
				break;
			case formatSelectorWriteStart:
				DoWriteStart();
				break;
			case formatSelectorWriteContinue:
				DoWriteContinue();
				break;
			case formatSelectorWriteFinish:
				DoWriteFinish();
				break;

			case formatSelectorFilterFile:
				DoFilterFile();
				break;
			
			
			case formatSelectorReadLayerStart:
				DoReadLayerStart();
				break;
			case formatSelectorReadLayerContinue:
				DoReadLayerContinue();
				break;
			case formatSelectorReadLayerFinish:
				DoReadLayerFinish();
				break;
			
			case formatSelectorWriteLayerStart:
				DoWriteLayerStart();
				break;
			case formatSelectorWriteLayerContinue:
				DoWriteLayerContinue();
				break;
			case formatSelectorWriteLayerFinish:
				DoWriteLayerFinish();
				break;
		}
			
	} // about selector special

	// release any suites that we may have acquired
	if (selector == formatSelectorAbout ||
		selector == formatSelectorWriteFinish ||
		selector == formatSelectorReadFinish ||
		selector == formatSelectorOptionsFinish ||
		selector == formatSelectorEstimateFinish ||
		selector == formatSelectorFilterFile ||
		*gResult != noErr)
	{
#if __PIMac__
		UnLoadRuntimeFunctions();
#endif
		PIUSuitesRelease();
	}

  } // end try

  catch (...)
  {
#if __PIMac__
  	UnLoadRuntimeFunctions();
#endif
  	if (NULL != result)
		*result = -1;
  }

} // end PluginMain


//-------------------------------------------------------------------------------
//
//	InitData
//	
//	Initialize any global values here.  Called only once when global
//	space is reserved for the first time.
//
//	Outputs:
//		Initializes any global values with their defaults.
//
//-------------------------------------------------------------------------------

static void InitData (void)
{	
	gData->needsSwap = false;
} // end InitData


//-------------------------------------------------------------------------------
//
//	CheckIdentifier
//	
//	Check the passed in character array for our identifier.
//
//  Inputs:
//		array of characters representing the identifier
//	Outputs:
//		HEADER_CANT_READ		= I have no idea what this file is
//		HEADER_VER1	= This is my old header, it has 16 bit rows and columns
//		HEADER_VER2	= This is my NEW header, it has 32 bit rows and columns
//
//-------------------------------------------------------------------------------

static int CheckIdentifier (char identifier [])
{
	if (identifier[3] != 'b' ||
		identifier[4] != 'r' ||
		identifier[5] != 'a' ||
		identifier[6] != 'i' ||
		identifier[7] != 'n')
		return HEADER_CANT_READ;

	if (identifier[0] == 'o' && 
		identifier[1] == 'n' && 
		identifier[2] == 'e')
		return HEADER_VER1;

	if (identifier[0] == 'b' && 
		identifier[1] == 'i' && 
		identifier[2] == 'g')
		return HEADER_VER2;

	return HEADER_CANT_READ;
}

//-------------------------------------------------------------------------------
//
//	SetIdentifier
//	
//	Set the passed in character array to our identifier.
//
//  Inputs:
//		array of characters representing the identifier
//	Outputs:
//		array of characters = "bigbrain"
//
//-------------------------------------------------------------------------------

static void SetIdentifier (char identifier [])
{
	
	identifier[0] = 'b';
    identifier[1] = 'i';
    identifier[2] = 'g';
    identifier[3] = 'b';
    identifier[4] = 'r';
    identifier[5] = 'a';
    identifier[6] = 'i';
    identifier[7] = 'n';
	
}

/*****************************************************************************/

static int32 RowBytes (void)
{
	VPoint imageSize = GetFormatImageSize();
	return (imageSize.h * gFormatRecord->depth + 7) >> 3;
	
}

/*****************************************************************************/

static void DoReadPrepare (void)
{
	gFormatRecord->maxData = 0;
	
#if __PIMac__
	if (gFormatRecord->hostSupportsPOSIXIO)
		gFormatRecord->pluginUsingPOSIXIO = true;
#endif

}

/*****************************************************************************/

static void ReadSome (int32 count, void * buffer)
{
	
	int32 readCount = count;
	
	if (*gResult != noErr)
		return;
	
	*gResult = PSSDKRead ((int32)gFormatRecord->dataFork,
                          gFormatRecord->posixFileDescriptor,
                          gFormatRecord->pluginUsingPOSIXIO,
                          &readCount,
                          buffer);
	
	if (*gResult == noErr && readCount != count)
		*gResult = eofErr;
	
}

/*****************************************************************************/

static void WriteSome (int32 count, void * buffer)
{
	
	int32 writeCount = count;
	
	if (*gResult != noErr)
		return;
	
	*gResult = PSSDKWrite ((int32)gFormatRecord->dataFork,
                           gFormatRecord->posixFileDescriptor,
                           gFormatRecord->pluginUsingPOSIXIO,
                           &writeCount,
                           buffer);
	
	if (*gResult == noErr && writeCount != count)
		*gResult = dskFulErr;
	
}

/*****************************************************************************/

static void ReadRow (Ptr pixelData, bool needsSwap)
{
	ReadSome (RowBytes(), pixelData);
	if (gFormatRecord->depth == 16 && needsSwap)
		SwapRow(RowBytes(), pixelData);
}

static void SwapRow(int32 rowBytes, Ptr pixelData)
{
	uint16 * bigPixels = reinterpret_cast<uint16 *>(pixelData);
	for (int32 a = 0; a < rowBytes; a+=2, bigPixels++)
		Swap(*bigPixels);
}

/*****************************************************************************/

static void WriteRow (Ptr pixelData)
{
	WriteSome (RowBytes(), pixelData);
}

/*****************************************************************************/

static void DisposeImageResources (void)
{
	
	if (gFormatRecord->imageRsrcData)
	{
		
		sPSHandle->Dispose(gFormatRecord->imageRsrcData);
		
		gFormatRecord->imageRsrcData = NULL;
		
		gFormatRecord->imageRsrcSize = 0;
		
	}
	
}

/*****************************************************************************/

static void DoReadStart (void)
{
	
	// If you add fmtCanCreateThumbnail to the FormatFlags PiPL property
	// you will get called to create a thumbnail. The only way to tell
	// that you are processing a thumbnail is to check openForPreview in the
	// FormatRecord. You do not need to parse the entire file. You need to
	// process enough for a thumbnail view and you need to do it quickly.
	
	ReadScriptParamsOnRead (); // override params here
	
	/* Exit if we have already encountered an error. */

    if (*gResult != noErr)
		return;
		
	/* If we have not encountered an error, then we want to read
	   the file header. */
	   
	*gResult = PSSDKSetFPos ((int32)gFormatRecord->dataFork,
                            gFormatRecord->posixFileDescriptor,
                            gFormatRecord->pluginUsingPOSIXIO,
                            fsFromStart,
                            0);
	if (*gResult != noErr)
		return;
	
	ReadSome (sizeof (gHeader.identifier), &gHeader.identifier);
	if (*gResult != noErr)
		return;

	gData->needsSwap = false;
	
	int headerID = CheckIdentifier (gHeader.identifier);
	
	if (headerID == HEADER_CANT_READ)
	{
		*gResult = formatCannotRead;
	}
	else if (headerID == HEADER_VER1)
	{
		HeaderVer1 headerVer1;
		
		ReadSome(sizeof(HeaderVer1), &headerVer1);
		
		gHeader.mode = headerVer1.mode;
		gHeader.depth = headerVer1.depth;
		gHeader.rows = headerVer1.rows;
		gHeader.cols = headerVer1.cols;
		gHeader.planes = headerVer1.planes;
		gHeader.transparencyPlane = 0;
		gHeader.resourceLength = headerVer1.resourceLength;
	}
	else if (headerID == HEADER_VER2)
	{
		ReadSome(sizeof(HeaderVer2) - sizeof(gHeader.identifier), &gHeader.endian);
		// determine machine endian-ness
		uint32 tempLong = 0x11223344;
		uint8 tempChar = *(reinterpret_cast<uint8 *>(&tempLong) + 3);
		bool isLittleEndian = (tempChar == 0x11);
		if ((gHeader.endian == LITTLEENDIAN) != isLittleEndian)
		{
			gData->needsSwap = true;
			Swap(gHeader.testendian);
			Swap(gHeader.mode);
			Swap(gHeader.depth);
			Swap(gHeader.rows);
			Swap(gHeader.cols);
			Swap(gHeader.planes);
			Swap(gHeader.transparencyPlane);
			Swap(gHeader.resourceLength);
			Swap(gHeader.numLayers);
		}
		
		if (gHeader.testendian != TESTENDIAN)
		{
			*gResult = formatCannotRead;
			return;
		}
	}
	else
	{
		*gResult = formatCannotRead;
	}

	if (*gResult != noErr)
		return;

	gFormatRecord->imageMode = gHeader.mode;
	VPoint imageSize;
	imageSize.v = gHeader.rows;
	imageSize.h = gHeader.cols;
	SetFormatImageSize(imageSize);
	gFormatRecord->depth = gHeader.depth;
	gFormatRecord->planes = gHeader.planes;
	gFormatRecord->transparencyPlane = gHeader.transparencyPlane;
	// Tell the host how many layers there are in the document
	gFormatRecord->layerData=gHeader.numLayers;
	gFormatRecord->transparencyMatting = DESIREDMATTING;
	
	if (sPSHandle->New != NULL)
		gFormatRecord->imageRsrcData = sPSHandle->New(0);
	gFormatRecord->imageRsrcSize = 0;
		
	/* Next, we will will read the lookup tables if in indexed color mode. */
	
	if (gHeader.mode == plugInModeIndexedColor)
	{
		
		ReadSome (3 * sizeof (LookUpTable), &gFormatRecord->redLUT);
		
		if (*gResult != noErr)
			goto CleanUp;
		
	}
	
	return;
		
	/* The following code does any clean-up work in the event of an error. */
	
	CleanUp:
	
	DisposeImageResources ();
        
}

/*****************************************************************************/

static void DoReadContinue (void)
{
	// Need to fill for backward compatibility with older hosts
	int32 done;
	int32 total;
	int16 plane;
	int32 row;
	
	/* Dispose of the image resource data if it exists. */
	
	//DisposeImageResources ();
	
	/* Set up the progress variables. */
	
	done = 0;
	VPoint imageSize = GetFormatImageSize();
	total = imageSize.v * gFormatRecord->planes;
		
	/* Next, we will allocate the pixel buffer. */

	uint32 bufferSize = RowBytes();
	Ptr pixelData = sPSBuffer->New( &bufferSize, bufferSize );
	if (pixelData == NULL)
	{
		*gResult = memFullErr;
		return;
	}
	
	uint32 layerNameLength;
	ReadSome(sizeof(uint32), &layerNameLength);
	if (layerNameLength >= MAXLAYERNAMELENGTH)
	{
		*gResult = formatCannotRead;
		return;
	}
	
	if(layerNameLength)
	{
		if (gData->needsSwap)
			Swap(layerNameLength);
		
		ReadSome(layerNameLength*sizeof(uint16), gLayerName);
		
		if (gData->needsSwap)
			for(uint32 index=0;index<layerNameLength;index++)
				Swap(gLayerName[index]);
	}

	/* Set up to start returning chunks of data. */
	
	VRect theRect;

	theRect.left = 0;
	theRect.right = imageSize.h;
	gFormatRecord->colBytes = (gFormatRecord->depth + 7) >> 3;
	gFormatRecord->rowBytes = RowBytes();
	gFormatRecord->planeBytes = 0;
	gFormatRecord->data = pixelData;
	if (gFormatRecord->depth == 16)
		gFormatRecord->maxValue = 0x8000; // I read them like Photoshop writes them

	for (plane = 0; *gResult == noErr && plane < gFormatRecord->planes; ++plane)
	{
		
		gFormatRecord->loPlane = gFormatRecord->hiPlane = plane;
		
		for (row = 0; *gResult == noErr && row < imageSize.v; ++row)
		{
			
			theRect.top = row;
			theRect.bottom = row + 1;

			SetFormatTheRect(theRect);
			
			ReadRow (pixelData, gData->needsSwap);
			
			if (*gResult == noErr)
				*gResult = gFormatRecord->advanceState();
				
			gFormatRecord->progressProc(++done, total);
			
		}
		
	}
	
	// Read through the rest of the layers and do nothing with it
	for (int32 layer = 0; *gResult == noErr && layer < gHeader.numLayers-1; ++layer)
	{
		ReadSome(sizeof(uint32), &layerNameLength);
		if (layerNameLength >= MAXLAYERNAMELENGTH)
		{
			*gResult = formatCannotRead;
			return;
		}

		if(layerNameLength)
		{
			uint16 dummy[MAXLAYERNAMELENGTH];
			if (gData->needsSwap)
				Swap(layerNameLength);
			
			ReadSome(layerNameLength*sizeof(uint16), dummy);
		}
		for (plane = 0; *gResult == noErr && plane < gFormatRecord->planes; ++plane)
			for (row = 0; *gResult == noErr && row < imageSize.v; ++row)
				ReadRow (pixelData, gData->needsSwap);
	}	
	gFormatRecord->data = NULL;
	
	sPSBuffer->Dispose(&pixelData);
}

/*****************************************************************************/

static void DoReadFinish (void)
{
	
	/* Dispose of the image resource data if it exists. */
	DoReadICCProfile ();
	// DisposeImageResources ();
	WriteScriptParamsOnRead (); // should be different for read/write
	AddComment (); // write a history comment

}

/*****************************************************************************/

static void DoOptionsPrepare (void)
{
	gFormatRecord->maxData = 0;
}

/*****************************************************************************/

static void DoOptionsStart (void)
{
	gFormatRecord->data = NULL;
}

/*****************************************************************************/

static void DoOptionsContinue (void)
{
}

/*****************************************************************************/

static void DoOptionsFinish (void)
{
}

/*****************************************************************************/

static void DoEstimatePrepare (void)
{
	gFormatRecord->maxData = 0;
}

/*****************************************************************************/

static void DoEstimateStart (void)
{
	
	int32 dataBytes;
	
	VPoint imageSize = GetFormatImageSize();
	
	int32 numLayers=1;
	if(gFormatRecord->layerData)
		numLayers=gFormatRecord->layerData;
		
	dataBytes = sizeof (FileHeader) +
				gFormatRecord->imageRsrcSize +
				RowBytes () * gFormatRecord->planes * imageSize.v * numLayers;
					  
	if (gFormatRecord->imageMode == plugInModeIndexedColor)
		dataBytes += 3 * sizeof (LookUpTable);
		
	gFormatRecord->minDataBytes = dataBytes;
	gFormatRecord->maxDataBytes = dataBytes;
	
	gFormatRecord->data = NULL;

}

/*****************************************************************************/

static void DoEstimateContinue (void)
{
}

/*****************************************************************************/

static void DoEstimateFinish (void)
{
}

/*****************************************************************************/

static void DoWritePrepare (void)
{
	gFormatRecord->maxData = 0;	

#if __PIMac__
	if (gFormatRecord->hostSupportsPOSIXIO)
		gFormatRecord->pluginUsingPOSIXIO = true;
#endif

}

/*****************************************************************************/

static void DoWriteStart (void)
{
	ReadScriptParamsOnWrite ();
	
    if (*gResult != noErr)
		return;
		
	/* Write the header. */
	
	*gResult = PSSDKSetFPos ((int32)gFormatRecord->dataFork,
                             gFormatRecord->posixFileDescriptor,
                             gFormatRecord->pluginUsingPOSIXIO,
                             fsFromStart,
                             0);
	if (*gResult != noErr)
		return;
	
	SetIdentifier (gHeader.identifier);
	VPoint imageSize = GetFormatImageSize();

	uint32 tempLong = 0x11223344;
	uint8 tempChar = *(reinterpret_cast<uint8 *>(&tempLong) + 3);
	bool isLittleEndian = (tempChar == 0x11);
	gHeader.endian = isLittleEndian ? LITTLEENDIAN : BIGENDIAN;
	gHeader.testendian = TESTENDIAN;
	gHeader.mode = gFormatRecord->imageMode;
	gHeader.rows = imageSize.v;
	gHeader.cols = imageSize.h;
	gHeader.depth = gFormatRecord->depth;
	gHeader.planes = gFormatRecord->planes;
	gHeader.transparencyPlane = gFormatRecord->transparencyPlane;
	gHeader.numLayers=gFormatRecord->layerData;
	
	gHeader.resourceLength = 0;
	WriteSome (sizeof (FileHeader), &gHeader);
	
	if (*gResult != noErr)
		return;
	
	/* Write the lookup tables if appropriate. */
	
	if (gHeader.mode == plugInModeIndexedColor)
	{
		WriteSome (3 * sizeof (LookUpTable), &gFormatRecord->redLUT);
		if (*gResult != noErr)
			return;
	}
}

/*****************************************************************************/

static void DoWriteContinue (void)
{
	int32 row;
	int16 plane;
	int32 done;
	int32 total;
	
	//Need for backward compatibility with older hosts	
	/* Allocate the pixel buffer. */
	uint32 bufferSize = RowBytes();
	Ptr pixelData = sPSBuffer->New( &bufferSize, bufferSize );
	if (pixelData == NULL)
	{
		*gResult = memFullErr;
		return;
	}
	
	//Write out a layer name length of 0
	uint32 layerNameLength=0;
	WriteSome (sizeof (uint32), &layerNameLength);

	/* Set up to start receiving chunks of data. */
	gFormatRecord->colBytes = (gFormatRecord->depth + 7) >> 3;
	gFormatRecord->rowBytes = RowBytes();
	gFormatRecord->planeBytes = 0;
	gFormatRecord->data = pixelData;
	gFormatRecord->transparencyMatting = DESIREDMATTING;
		
	/* Set up the progress variables. */
	done = 0;
	total = gHeader.rows * gHeader.planes;
	
	VRect theRect;

	theRect.left = 0;
	theRect.right = gHeader.cols;
	
	for (plane = 0; *gResult == noErr && plane < gFormatRecord->planes; ++plane)
	{
		
		gFormatRecord->loPlane = gFormatRecord->hiPlane = plane;
		
		for (row = 0; *gResult == noErr && row < gHeader.rows; row++)
		{
			
			theRect.top = row;
			theRect.bottom = row + 1;

			SetFormatTheRect(theRect);
			
			if (*gResult == noErr)
				*gResult = gFormatRecord->advanceState ();
				
			if (*gResult == noErr)
				WriteRow (pixelData);
			
			gFormatRecord->progressProc (++done, total);
			
		}
		
	}
	
	gFormatRecord->data = NULL;
	
	sPSBuffer->Dispose(&pixelData);
}

/*****************************************************************************/

static void DoWriteFinish (void)
{
	DoWriteICCProfile ();
	WriteScriptParamsOnWrite (); // should be different for read/write
}

/*****************************************************************************/

static void DoFilterFile (void)
{
	
	FileHeader header;
	
	/* Exit if we have already encountered an error. */

    if (*gResult != noErr)
		return;
		
	/* Read the file header. */
	   
	*gResult = PSSDKSetFPos ((int32)gFormatRecord->dataFork,
                             gFormatRecord->posixFileDescriptor,
                             gFormatRecord->pluginUsingPOSIXIO,
                             fsFromStart,
                             0);
	if (*gResult != noErr)
		return;
	
	ReadSome (sizeof (FileHeader), &header);
	
	if (*gResult != noErr)
		return;
	
	/* Check the identifier. */
	
	if (!CheckIdentifier (header.identifier))
	{
		*gResult = formatCannotRead;
		return;
	}
	
}

/*****************************************************************************/

/* This routine adds a history entry with the current system date and time
   to the file when incoming. */

static void AddComment (void)
{
	time_t ltime;
	time( &ltime);
	
	string currentTime = ctime(&ltime);

	size_t length = currentTime.size();

	Handle h = sPSHandle->New((int32)length);
	
	if (h != NULL)
	{
		Ptr p = NULL;
		sPSHandle->SetLock(h, true, &p, NULL);
		if (p != NULL)
		{
			for (size_t a = 0; a < length; a++)
				*p++ = currentTime.at(a);
			gAddResource(histResource, h);
			sPSHandle->SetLock(h, false, &p, NULL);
		}
		sPSHandle->Dispose(h);
	}
} // end AddComment

/**************************************************************************/
static void DoReadICCProfile(void)
{
	if (gFormatRecord->canUseICCProfiles)
	{
		ReadSome(sizeof(gFormatRecord->iCCprofileSize), &gFormatRecord->iCCprofileSize);
		if (*gResult == noErr && gFormatRecord->iCCprofileSize > 127)
		{
			if (gData->needsSwap)
				Swap(gFormatRecord->iCCprofileSize);
			if (*gResult == noErr)
			{
				gFormatRecord->iCCprofileData = sPSHandle->New(gFormatRecord->iCCprofileSize);
				Ptr data = NULL;
				sPSHandle->SetLock(gFormatRecord->iCCprofileData, true, &data, NULL);
				if (gFormatRecord->iCCprofileData != NULL && data != NULL)
				{
					ReadSome(gFormatRecord->iCCprofileSize, data);
					sPSHandle->SetLock(gFormatRecord->iCCprofileData, false, &data, NULL);
				}
			}
		}
		else
		{
			*gResult = noErr; // it's an old file or one without ICC info, no worries
		}
	}
}

static void DoWriteICCProfile(void)
{
	if (gFormatRecord->canUseICCProfiles)
	{
		if (gFormatRecord->iCCprofileSize && gFormatRecord->iCCprofileData)
		{
			WriteSome(sizeof(gFormatRecord->iCCprofileSize), &gFormatRecord->iCCprofileSize);
			Ptr data = NULL;
			sPSHandle->SetLock(gFormatRecord->iCCprofileData, true, &data, NULL);
			if (data != NULL)
			{
				WriteSome(gFormatRecord->iCCprofileSize, data);
				sPSHandle->SetLock(gFormatRecord->iCCprofileData, false, &data, NULL);
			}
		}
	}
}

static VPoint GetFormatImageSize(void)
{
	VPoint returnPoint = { 0, 0};
	if (gFormatRecord->HostSupports32BitCoordinates && gFormatRecord->PluginUsing32BitCoordinates)
	{
		returnPoint.v = gFormatRecord->imageSize32.v;
		returnPoint.h = gFormatRecord->imageSize32.h;
	}
	else
	{
		returnPoint.v = gFormatRecord->imageSize.v;
		returnPoint.h = gFormatRecord->imageSize.h;
	}
	return returnPoint;
}

static void SetFormatImageSize(VPoint inPoint)
{
	if (gFormatRecord->HostSupports32BitCoordinates && 
		gFormatRecord->PluginUsing32BitCoordinates)
	{
		gFormatRecord->imageSize32.v = inPoint.v;
		gFormatRecord->imageSize32.h = inPoint.h;
	}
	else
	{
		gFormatRecord->imageSize.v = static_cast<int16>(inPoint.v);
		gFormatRecord->imageSize.h = static_cast<int16>(inPoint.h);
	}
}
static void SetFormatTheRect(VRect inRect)
{
	if (gFormatRecord->HostSupports32BitCoordinates && 
		gFormatRecord->PluginUsing32BitCoordinates)
	{
		gFormatRecord->theRect32.top = inRect.top;
		gFormatRecord->theRect32.left = inRect.left;
		gFormatRecord->theRect32.bottom = inRect.bottom;
		gFormatRecord->theRect32.right = inRect.right;
	}
	else
	{
		gFormatRecord->theRect.top = static_cast<int16>(inRect.top);
		gFormatRecord->theRect.left = static_cast<int16>(inRect.left);
		gFormatRecord->theRect.bottom = static_cast<int16>(inRect.bottom);
		gFormatRecord->theRect.right = static_cast<int16>(inRect.right);
	}
}

//-------------------------------------------------------------------------------
//
// CreateDataPtr
//
// Create a pointer to our Data structure. Photoshop will take ownership of this
// pointer and give it back to us for subsequent runs.
//-------------------------------------------------------------------------------
static void CreateDataPtr(void)
{
	gData = (Data*) malloc(sizeof(Data));
	if (gData == NULL)
	{
		*gResult = memFullErr;
		return;
	}
}

static void CreateResourceInfoVector(int32 length,
									 uint8 * resources, 
									 vector<ResourceInfo *> & rInfos)
{
	rInfos.clear();

	int32 calculatedLength = 0;

	uint8 * resourcesEnd = resources + length;

	while (resources < resourcesEnd)
	{
		ResourceInfo * thisResource = new ResourceInfo;

		thisResource->totalSize = 0;
		
		thisResource->type = GetBigEndian<uint32>(resources);
		thisResource->totalSize += sizeof(thisResource->type);
		
		thisResource->id = GetBigEndian<uint16>(resources);
		thisResource->totalSize += sizeof(thisResource->id);

		uint8 nameLength = *resources++;
		thisResource->totalSize++;

		thisResource->name.reserve(nameLength);
		ostringstream inputName;
		for (int a = 0; a < nameLength; a++)
		{
			inputName << *resources++;
			thisResource->totalSize++;
		}
		thisResource->name = inputName.str();

		resources += nameLength | uint8(1); // Add one if even for alignment;
		thisResource->totalSize += nameLength | uint8(1);

		thisResource->size = GetBigEndian<uint32>(resources);
		thisResource->totalSize += sizeof(thisResource->size);

		resources += (thisResource->size + 1) & ~uint32(1); // Add one if odd for alignment;
		thisResource->totalSize += (thisResource->size + 1) & ~uint32(1); // Add one if odd for alignment;

		thisResource->keep = true;
		
		rInfos.push_back(thisResource);
		
		calculatedLength += thisResource->totalSize;
	} 
}

static void DeleteResourceInfoVector(vector<ResourceInfo *> & rInfos)
{
	size_t count = rInfos.size();
	for (size_t a = 0; a < count; a++)
	{
		ResourceInfo * thisResource = rInfos.at(a);
		delete thisResource;
	}
}

static int32 RemoveResources(int32 resourceLength, 
							 const vector<ResourceInfo *> & rInfos, 
							 uint8 * resourcePtr)
{
	size_t count = rInfos.size();

	for (size_t a = 0; a < count; a++)
	{
		if (! rInfos.at(a)->keep)
		{
			resourceLength = RemoveResource(rInfos.at(a)->type,
				                            rInfos.at(a)->id,
											resourcePtr,
											resourceLength,
											resourcePtr);
		}
	}
		
	return resourceLength;
}


static int32 RemoveResource(uint32 type,
							uint16 id,
							uint8 * imageResource,
							int32 size,
							uint8 * dest)
{
	/*
	[first, last) is the source range. first will close on last as we
	search for the resource.
	*/
	uint8 * first  = imageResource;
	uint8 * last  = first + size;
	
	/*
	[resFirst, resLast) marks the range enclosing the found resource
	if the resource has not been found then the range is empty but within
	[first, last).
	*/
	
	uint8 * resFirst  = first;
	uint8 * resLast = first;
	
	// min bytes is the minimum number of bytes in the resource header.
	
	const ptrdiff_t minBytes(sizeof(uint32) + sizeof(uint16) + sizeof(int8));

	while ((last - first) >= minBytes) 
	{
		resFirst = first;
		resLast = first;
	
		// Read the type
		uint32 readType = GetBigEndian<uint32>(first);
		
		// Read the id
		uint16 readID (GetBigEndian<uint16>(first));
		
		// Skip the name
		uint8 length = *first++;
		first += length | uint8(1); // Add one if even for alignment;
		
		// Make sure we have enough space to read the blockSize
		if ((last - first) < sizeof(uint32))
			break;
		
		// Read the block size
		uint32 blockSize (GetBigEndian<uint32>(first));
		
		// Skip the block
		first += (blockSize + 1) & ~uint32(1); // Add one if odd for alignment;
		
		if ((readType == type) && (readID == id))
		{
			resLast = first;
			break;
		}
	}
	
	/*
	Handle the copy as two pieces. If the resource was found then this will omit it.
	Otherwise it is simply copied as all but the last resoruce, and then the last resource.
	*/
	
	uint8 * destEnd = NULL;

	if (resFirst == resLast)
	{
		if (dest != imageResource)
		{
			// nothing was found and I need to copy everything
			destEnd = copy(imageResource, imageResource + size, dest);
		}
		else
		{
			// nothing was found and I don't need to copy anything
			destEnd = imageResource + size;
		}
	}
	else
	{
		if (dest != imageResource)
		{
			// something was found and I need to copy over the start
			destEnd = copy(imageResource, resFirst, dest);
			// skipping the resource that I found
			destEnd = copy(resLast, last, destEnd);
		}
		else
		{
			// skip over the resource only
			destEnd = copy(resLast, last, resFirst); //destEnd);
		}
	}

	return int32(destEnd - dest);
}

void DoWriteLayerStart (void)
{
	// We don't need this in this example.  But, if you need it, it's there.
	// int32 currentLayer = gFormatRecord->layerData;
	
	/* Next, we will allocate the pixel buffer. */
	uint32 bufferSize = RowBytes();
	Ptr pixelData = sPSBuffer->New( &bufferSize, bufferSize );
	if (pixelData == NULL)
	{
		*gResult = memFullErr;
		return;
	}
		
	/* Set up to start receiving chunks of data. */
	gFormatRecord->colBytes = (gFormatRecord->depth + 7) >> 3;
	gFormatRecord->rowBytes = RowBytes();
	gFormatRecord->planeBytes = 0;
	gFormatRecord->data = pixelData;
	gFormatRecord->transparencyMatting = DESIREDMATTING;
	
	// Write the layer name into the file
	uint32 layerNameLength=0;
	if(gFormatRecord->layerName)
	{
		while(gFormatRecord->layerName[layerNameLength] != 0)
			layerNameLength++;
	}
	// Do not forget the trailing NULL
	layerNameLength++;
	if (layerNameLength >= MAXLAYERNAMELENGTH)
	{
		*gResult = formatCannotRead;
		return;
	}
	
	// Start with the length
	WriteSome (sizeof (uint32), &layerNameLength);
	
	//Write the layer name
	if(layerNameLength)
		WriteSome ((layerNameLength)*sizeof (uint16), gFormatRecord->layerName);
	
	double modTime=-1.0;
	
	if(gFormatRecord->layerMetaData && gFormatRecord->layerMetaData->descriptor)
		{
		PSBasicActionControlProcs *actionControlProcs=NULL;
		PSActionDescriptorProcs *actionDescriptorProcs=NULL;
		
		SPErr error = gFormatRecord->sSPBasic->AcquireSuite(kPSActionDescriptorSuite,
							                                kPSActionDescriptorSuiteVersion,
							                                (const void**)&actionDescriptorProcs);
        if (error || actionDescriptorProcs == NULL)
        {
            *gResult = errPlugInHostInsufficient;
            return;
        }
							
		
		error = gFormatRecord->sSPBasic->AcquireSuite(kPSBasicActionControlSuite,
                                                      kPSBasicActionControlSuiteVersion,
                                                      (const void**)&actionControlProcs);
        if (error || actionControlProcs == NULL)
            {
            *gResult = errPlugInHostInsufficient;
            return;
            }
							
		PIActionDescriptor metadataDescriptor = NULL;
		
		actionDescriptorProcs->HandleToDescriptor(gFormatRecord->layerMetaData->descriptor,&metadataDescriptor);
	
		Boolean hasKey = 0;
        DescriptorTypeID typeID = 0;
		
		actionControlProcs->StringIDToTypeID("layerTime",&typeID);
		actionDescriptorProcs->HasKey(metadataDescriptor,typeID,&hasKey);
		if(hasKey)
			{
			actionDescriptorProcs->GetFloat(metadataDescriptor,typeID,&modTime);
			}
			
		if(metadataDescriptor)
			actionDescriptorProcs->Free(metadataDescriptor);
			
		if(actionDescriptorProcs)
			gFormatRecord->sSPBasic->ReleaseSuite(kPSActionDescriptorSuite, kPSActionDescriptorSuiteVersion);	
		actionDescriptorProcs = NULL;
		
		if(actionControlProcs)
			gFormatRecord->sSPBasic->ReleaseSuite(kPSBasicActionControlSuite, kPSBasicActionControlSuiteVersion);	
		actionControlProcs = NULL;

		}
		
	WriteSome (sizeof (double), &modTime);
}

void DoWriteLayerContinue (void)
{
	
	int32 row;
	int16 plane;
	int32 done;
	int32 total;
	int32 numLayers = 1;
	if (gHeader.numLayers)
		numLayers = gHeader.numLayers;
		
	// We don't need this in this example.  But, if you need it, it's there.
	// int32 currentLayer = gFormatRecord->layerData;
	
	/* Set up the progress variables. */
	done = 0;
	total = gHeader.rows * gHeader.planes * numLayers;
	
	Ptr pixelData = (Ptr)gFormatRecord->data;
	VRect theRect;

	theRect.left = 0;
	theRect.right = gHeader.cols;
	
	for (plane = 0; *gResult == noErr && plane < gFormatRecord->planes; ++plane)
	{
		
		gFormatRecord->loPlane = gFormatRecord->hiPlane = plane;
		
		for (row = 0; *gResult == noErr && row < gHeader.rows; row++)
		{
			
			theRect.top = row;
			theRect.bottom = row + 1;

			SetFormatTheRect(theRect);
			
			if (*gResult == noErr)
				*gResult = gFormatRecord->advanceState ();
				
			if (*gResult == noErr)
				WriteRow (pixelData);
			
			gFormatRecord->progressProc (++done, total);
			
		}
		
	}
}

void DoWriteLayerFinish (void)
{
	// We don't need this in this example.  But, if you need it, it's there.
	// int32 currentLayer = gFormatRecord->layerData;
	
	Ptr pixelData = (Ptr)gFormatRecord->data;
	
	gFormatRecord->data = NULL;
	
	sPSBuffer->Dispose(&pixelData);
}

void DoReadLayerStart (void)
{
	uint32 layerNameLength = 0;
	ReadSome(sizeof(uint32), &layerNameLength);
	if (layerNameLength >= MAXLAYERNAMELENGTH)
	{
		*gResult = formatCannotRead;
		return;
	}

	if(layerNameLength)
	{
		if (gData->needsSwap)
			Swap(layerNameLength);
		
		ReadSome(layerNameLength*sizeof(uint16), gLayerName);
		
		if (gData->needsSwap)
			for(uint32 index=0;index<layerNameLength;index++)
				Swap(gLayerName[index]);
		
		gFormatRecord->layerName=gLayerName;
	}
	
	double modTime = 0;
	ReadSome(sizeof(double), &modTime);
	if(gFormatRecord->layerMetaData)
		{
		PSBasicActionControlProcs *actionControlProcs=NULL;
		PSActionDescriptorProcs *actionDescriptorProcs=NULL;
		SPErr error = gFormatRecord->sSPBasic->AcquireSuite(kPSActionDescriptorSuite,
							                                kPSActionDescriptorSuiteVersion,
							                                (const void**)&actionDescriptorProcs);
        if (error || actionDescriptorProcs == NULL)
            {
            *gResult = errPlugInHostInsufficient;
            return;
            }

        error = gFormatRecord->sSPBasic->AcquireSuite(kPSBasicActionControlSuite,
							                          kPSBasicActionControlSuiteVersion,
							                          (const void**)&actionControlProcs);
        if (error || actionControlProcs == NULL)
            {
            *gResult = errPlugInHostInsufficient;
            return;
            }
							
		PIActionDescriptor metadataDescriptor = NULL;
		
		actionDescriptorProcs->Make(&metadataDescriptor);
		if(metadataDescriptor)
			{
			DescriptorTypeID typeID = 0;
		
			actionControlProcs->StringIDToTypeID("layerTime",&typeID);
			actionDescriptorProcs->PutFloat(metadataDescriptor, typeID, modTime);
			
			actionDescriptorProcs->AsHandle(metadataDescriptor, &gFormatRecord->layerMetaData->descriptor);
			actionDescriptorProcs->Free(metadataDescriptor);
			}
					
			
		if(actionDescriptorProcs)
			gFormatRecord->sSPBasic->ReleaseSuite(kPSActionDescriptorSuite, kPSActionDescriptorSuiteVersion);	
		actionDescriptorProcs = NULL;
		
		if(actionControlProcs)
			gFormatRecord->sSPBasic->ReleaseSuite(kPSBasicActionControlSuite, kPSBasicActionControlSuiteVersion);	
		actionControlProcs = NULL;

		}
}

void DoReadLayerContinue (void)
{
	int32 done;
	int32 total;
	int16 plane;
	int32 row;
	
	/* Dispose of the image resource data if it exists. */
	
	//DisposeImageResources ();
	
	/* Set up the progress variables. */
	
	done = 0;
	VPoint imageSize = GetFormatImageSize();
	total = imageSize.v * gFormatRecord->planes;
		
	/* Next, we will allocate the pixel buffer. */

	uint32 bufferSize = RowBytes();
	Ptr pixelData = sPSBuffer->New( &bufferSize, bufferSize );
	if (pixelData == NULL)
	{
		*gResult = memFullErr;
		return;
	}
	
	/* Set up to start returning chunks of data. */
	
	VRect theRect;

	theRect.left = 0;
	theRect.right = imageSize.h;
	gFormatRecord->colBytes = (gFormatRecord->depth + 7) >> 3;
	gFormatRecord->rowBytes = RowBytes();
	gFormatRecord->planeBytes = 0;
	gFormatRecord->data = pixelData;
	if (gFormatRecord->depth == 16)
		gFormatRecord->maxValue = 0x8000; // I read them like Photoshop writes them

	for (plane = 0; *gResult == noErr && plane < gFormatRecord->planes; ++plane)
	{
		
		gFormatRecord->loPlane = gFormatRecord->hiPlane = plane;
		
		for (row = 0; *gResult == noErr && row < imageSize.v; ++row)
		{
			
			theRect.top = row;
			theRect.bottom = row + 1;

			SetFormatTheRect(theRect);
			
			ReadRow (pixelData, gData->needsSwap);
			
			if (*gResult == noErr)
				*gResult = gFormatRecord->advanceState();
				
			gFormatRecord->progressProc(++done, total);
			
		}
		
	}
		
	gFormatRecord->data = NULL;
	
	sPSBuffer->Dispose(&pixelData);

	
}

void DoReadLayerFinish (void)
{
}

// end LayerFormat.cpp
