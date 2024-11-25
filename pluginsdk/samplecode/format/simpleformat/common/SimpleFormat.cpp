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
//		SimpleFormat.h
//
//	Description:
//		This file contains the header prototypes and macros for the
//		File Format module SimpleFormat, 
//		which writes a flat file with merged document pixels.
//
//	Use:
//		Format modules are called from the Save, Save as,
//		and Save a copy dialogs.
//
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
//	Includes
//-------------------------------------------------------------------------------

#include <vector>
#include <sstream>
#include <time.h>
#include "SimpleFormat.h"
#include "PIUI.h"
#include "Logger.h"
#include "Timer.h"

/*****************************************************************************/

DLLExport MACPASCAL void PluginMain (const int16 selector,
						             FormatRecordPtr formatParamBlock,
						             intptr_t* data,
						             int16* result);

/*****************************************************************************/

using namespace std;

/*****************************************************************************/

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

static void AddComment (void);

const int32 HEADER_CANT_READ = 0;
const int32 HEADER_VER1 = 1;
const int32 HEADER_VER2 = 2;

// let us use the TIFF spec. to do cross platform files
const int16 BIGENDIAN = 0x4d4d;
const int16 LITTLEENDIAN = 0x4949;
const int16 TESTENDIAN = 0x002a;

const int32 DESIREDMATTING = 0;

// some error codes
const int16 ERRNOHEADER       = -20001;
const int16 ERRMODEHEADER     = -20002;
const int16 ERRDEPTHHEADER    = -20003;
const int16 ERRROWSHEADER     = -20004;
const int16 ERRCOLSHEADER     = -20005;
const int16 ERRPLANESHEADER   = -20006;
const int16 ERRTRANSHEADER    = -20007;
const int16 ERRRESOURCEHEADER = -20008;

/*****************************************************************************/

static int CheckIdentifier (char identifier []);
static int16 CheckHeader(FileHeader * inHeader);
static void SetIdentifier (char identifier []);
static unsigned32 RowBytes (void);

static void ReadSome (int32 count, void * buffer);
static void WriteSome (int32 count, void * buffer);
static void ReadRow (Ptr pixelData, bool needsSwap);
static void WriteRow (Ptr pixelData);
static void DisposeImageResources (void);
static void SwapRow(int32 rowBytes, Ptr pixelData);

static void DoReadICCProfile(void);
static void DoWriteICCProfile(void);

static VPoint GetFormatImageSize(void);
static void SetFormatImageSize(VPoint inPoint);
static void SetFormatTheRect(VRect inRect);

/*****************************************************************************/

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

/*****************************************************************************/

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

/*****************************************************************************/

//-------------------------------------------------------------------------------
//	Globals -- Define global variables for plug-in scope.
//-------------------------------------------------------------------------------

SPBasicSuite* sSPBasic = NULL;
SPPluginRef gPluginRef = NULL;

FormatRecordPtr gFormatRecord = NULL;
Data* gData = NULL;
int16* gResult = NULL;
Logger* gLogger = NULL;

/*****************************************************************************/

#define gCountResources gFormatRecord->resourceProcs->countProc
#define gGetResources   gFormatRecord->resourceProcs->getProc
#define gAddResource	gFormatRecord->resourceProcs->addProc

/*****************************************************************************/

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
//		FormatRecordPtr formatParamBloc				Host provides pointer to parameter
//													block containing pertinent data
//													and callbacks from the host.
//													See PIFormat.h.
//
//	Outputs:
//		FormatRecordPtr formatParamBlock			Host provides pointer to parameter
//													block containing pertinent data
//													and callbacks from the host.
//													See PIFormat.h.
//
//		intptr_t* data								Use this to store a pointer to our
//													global parameters structure, which
//													is maintained by the host between
//													calls to the plug-in.
//
//		int16* result								Return error result or noErr.  Some
//													errors are handled by the host, some
//													are silent, and some you must handle.
//													See PIGeneral.h.
//
//-------------------------------------------------------------------------------

DLLExport MACPASCAL void PluginMain (const int16 selector,
						             FormatRecordPtr formatParamBlock,
						             intptr_t* data,
						             int16* result)
{

  try 
  {

	if (gLogger == NULL)
		gLogger = new Logger( "SimpleFormatPlugin" );
	  
	Timer timeIt;

	gLogger->Write( "Selector: " );
	gLogger->Write( selector );
    gLogger->Write( " " );

	//---------------------------------------------------------------------------
	//	(1) Update our global parameters from the passed in values.
	// 
	//	We removed that nasty passing around of globals. It's global right! So
	//	why pass it around. This also removes the use of some those nasty #defines.
	//---------------------------------------------------------------------------
	gFormatRecord = formatParamBlock;
	gPluginRef = reinterpret_cast<SPPluginRef>(gFormatRecord->plugInRef);
	gResult = result;
	gData = (Data*)*data;

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
		AboutRecordPtr aboutRecord = reinterpret_cast<AboutRecordPtr>(formatParamBlock);
		sSPBasic = aboutRecord->sSPBasic;
		gPluginRef = reinterpret_cast<SPPluginRef>(aboutRecord->plugInRef);
		DoAbout(gPluginRef, AboutID);
	}
	else
	{ // do the rest of the process as normal:

		sSPBasic = formatParamBlock->sSPBasic;

		if (gCountResources == NULL ||
            gGetResources == NULL ||
            gAddResource == NULL ||
			gFormatRecord->advanceState == NULL)
		{
			*gResult = errPlugInHostInsufficient;
			return;
		}

		// new for Photoshop 8, big documents, rows and columns are now > 30000 pixels
		if (gFormatRecord->HostSupports32BitCoordinates)
			gFormatRecord->PluginUsing32BitCoordinates = true;

		//-----------------------------------------------------------------------
		//	(3) Allocate and initialize globals.
		//
		//-----------------------------------------------------------------------

 		if ( gData == NULL )
		{
			gData = (Data*) malloc(sizeof(Data));
			if (gData == NULL)
			{
				*gResult = memFullErr;
				return;
			}
			*data = (intptr_t)gData;
			memset(gData, 0, sizeof(Data));
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
		}
			
	} // about selector special

	gLogger->Write( timeIt.GetElapsed(), true );
	  
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
		delete gLogger;
		gLogger = NULL;
	}

  } // end try

	catch(...)
	{
#if __PIMac__
		UnLoadRuntimeFunctions();
#endif
		delete gLogger;
		gLogger = NULL;
		if (NULL != result)
			*result = -1;
	}

} // end PluginMain


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
//	CheckHeader
//	
//	Check header for bad options and return error values
//
//  Inputs:
//		FileHeader
//	Outputs:
//		0                 = no error
//		NOHEADER          = null error
//		ERRMODEHEADER     = mode error
//		ERRDEPTHHEADER    = depth error
//		ERRROWSHEADER     = rows error
//		ERRCOLSHEADER     = cols error
//		ERRPLANESHEADER   = planes error
//		ERRTRANSHEADER    = transparency plane error
//		ERRRESOURCEHEADER = resource length error
//
//-------------------------------------------------------------------------------
static int16 CheckHeader(FileHeader * inHeader)
{
	if (NULL == inHeader)
		return ERRNOHEADER;
	if (inHeader->mode < plugInModeBitmap || inHeader->mode > plugInModeGray32)
		return ERRMODEHEADER;
	if (inHeader->depth != 1 && 
		inHeader->depth != 8 && 
		inHeader->depth != 16 &&
		inHeader->depth != 32)
		return ERRDEPTHHEADER;
	if (inHeader->rows < 1 || inHeader->rows > 300000)
		return ERRROWSHEADER;
	if (inHeader->cols < 1 || inHeader->cols > 300000)
		return ERRCOLSHEADER;
	if (inHeader->planes < 0 || inHeader->planes > 50)
		return ERRPLANESHEADER;
	if (inHeader->transparencyPlane > inHeader->planes ||
		inHeader->transparencyPlane < -1)
		return ERRTRANSHEADER;
	if (inHeader->resourceLength < 0)
		return ERRRESOURCEHEADER;
	return 0;
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

static unsigned32 RowBytes (void)
{
	VPoint imageSize = GetFormatImageSize();
	return (imageSize.h * gFormatRecord->depth + 7) >> 3;
	
}

/*****************************************************************************/

static void DoReadPrepare (void)
{
	gFormatRecord->maxData = 0;
    gData->usePOSIX = true;
	
	// script params may change our usePOSIX
   	gData->showDialog = ReadScriptParamsOnRead ();

  #if __PIMac__
    if (gFormatRecord->hostSupportsPOSIXIO && gData->usePOSIX)
    {
        gFormatRecord->pluginUsingPOSIXIO = true;
        gLogger->Write( "Using POSIX " );
    }
    else
    {
        gData->usePOSIX = false;
        gLogger->Write( "Using FS " );
    }
  #endif

}

/*****************************************************************************/

static void ReadSome (int32 count, void * buffer)
{
	
	int32 readCount = count;
	
	if (*gResult != noErr)
		return;

	*gResult = PSSDKRead ((int)gFormatRecord->dataFork,
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
	
	*gResult = PSSDKWrite ((int)gFormatRecord->dataFork,
                           gFormatRecord->posixFileDescriptor,
                           gFormatRecord->pluginUsingPOSIXIO,
                           &writeCount, buffer);
	
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

	FileHeader header;
	
	memset(&header, 0, sizeof(header));
	
	/* If we have not encountered an error, then we want to read
	   the file header. */

	*gResult = PSSDKSetFPos ((int)gFormatRecord->dataFork,
                             gFormatRecord->posixFileDescriptor,
                             gFormatRecord->pluginUsingPOSIXIO,
                             fsFromStart, 0);
	if (*gResult != noErr) return;
	
	ReadSome (sizeof (header.identifier), &header.identifier);
	if (*gResult != noErr) return;

	gData->needsSwap = false;
	
	int headerID = CheckIdentifier (header.identifier);
	
	if (headerID == HEADER_CANT_READ)
	{
		*gResult = formatCannotRead;
	}
	else if (headerID == HEADER_VER1)
	{
		HeaderVer1 headerVer1;
		
		ReadSome(sizeof(HeaderVer1), &headerVer1);
		if (*gResult != noErr) return;

		header.mode = headerVer1.mode;
		header.depth = headerVer1.depth;
		header.rows = headerVer1.rows;
		header.cols = headerVer1.cols;
		header.planes = headerVer1.planes;
		header.transparencyPlane = 0;
		header.resourceLength = headerVer1.resourceLength;
	}
	else if (headerID == HEADER_VER2)
	{
		ReadSome(sizeof(HeaderVer2) - sizeof(header.identifier), &header.endian);
		if (*gResult != noErr) return;

		// determine machine endian-ness
		uint32 tempLong = 0x11223344;
		uint8 tempChar = *(reinterpret_cast<uint8 *>(&tempLong) + 3);
		bool isLittleEndian = (tempChar == 0x11);
		if ((header.endian == LITTLEENDIAN) != isLittleEndian)
		{
			gData->needsSwap = true;
			Swap(header.testendian);
			Swap(header.mode);
			Swap(header.depth);
			Swap(header.rows);
			Swap(header.cols);
			Swap(header.planes);
			Swap(header.transparencyPlane);
			Swap(header.resourceLength);
		}
		
		if (header.testendian != TESTENDIAN)
		{
			*gResult = formatCannotRead;
		}
	}
	else
	{
		*gResult = formatCannotRead;
	}

	if (*gResult != noErr) return;

	int16 checkResult = CheckHeader(&header);
	// I had a version where HeaderVer2 did not have the transparencyPlane
	// You get a large transparencyPlane as it has ready the resourceLength
	// this will try to read it anyway
	if (ERRTRANSHEADER == checkResult)
	{
		header.resourceLength = header.transparencyPlane;
		header.transparencyPlane = 0;
		*gResult = PSSDKSetFPos((int)gFormatRecord->dataFork,
                                gFormatRecord->posixFileDescriptor,
                                gFormatRecord->pluginUsingPOSIXIO,
                                fsFromMark, -4);
		if (*gResult != noErr) return;
	}
	else if (noErr != checkResult)
	{
		*gResult = formatBadParameters;
	}

	if (*gResult != noErr) return;

	gFormatRecord->imageMode = header.mode;
	VPoint imageSize;
	imageSize.v = header.rows;
	imageSize.h = header.cols;
	SetFormatImageSize(imageSize);
	gFormatRecord->depth = header.depth;
	gFormatRecord->planes = header.planes;
	gFormatRecord->transparencyPlane = header.transparencyPlane;
	gFormatRecord->transparencyMatting = DESIREDMATTING;
	
	/* Next, we will try to read the image resources. */
	/* If there is none then create an empty resource. */
	
	if (header.resourceLength > 0)
	{
		gFormatRecord->imageRsrcData = sPSHandle->New(header.resourceLength);
		
		if (gFormatRecord->imageRsrcData == NULL)
		{
			*gResult = memFullErr;
		}

		if (*gResult != noErr) return;

		Boolean oldLock = FALSE;
		Ptr resourcePtr = NULL;
		sPSHandle->SetLock(gFormatRecord->imageRsrcData, true, &resourcePtr, &oldLock);
		
		if (resourcePtr != NULL)
		{
			ReadSome (header.resourceLength, resourcePtr);

			if (!gFormatRecord->openForPreview)
			{
				vector<ResourceInfo *> resources;
				CreateResourceInfoVector(header.resourceLength, 
					                     reinterpret_cast<uint8 *>(resourcePtr), 
										 resources);

				// see if the user wants to remove some resources
				if (gData->showDialog && DoUI(resources))
				{
					header.resourceLength = RemoveResources(header.resourceLength, 
							                                resources, 
															reinterpret_cast<uint8 *>(resourcePtr));
					Handle h = sPSHandle->New(header.resourceLength);
					if (h == NULL)
					{
						*gResult = memFullErr;
						sPSHandle->Dispose(gFormatRecord->imageRsrcData);
						return;
					}
					Boolean newoldLock = FALSE;
					Ptr newResourcePtr = NULL;
					sPSHandle->SetLock(h, true, &newResourcePtr, &newoldLock);
					if (newResourcePtr != NULL)
					{
						copy(resourcePtr, resourcePtr + header.resourceLength, newResourcePtr);
						sPSHandle->Dispose(gFormatRecord->imageRsrcData);
						gFormatRecord->imageRsrcData = h;
					}
				}
				DeleteResourceInfoVector(resources);
			}

			sPSHandle->SetLock(gFormatRecord->imageRsrcData, false, &resourcePtr, &oldLock);
		
			if (*gResult != noErr)
				goto CleanUp;
			
			gFormatRecord->imageRsrcSize = header.resourceLength;
			}
		
	}
	else
	{

		if (sPSHandle->New != NULL)
			gFormatRecord->imageRsrcData = sPSHandle->New(0);
		gFormatRecord->imageRsrcSize = 0;

	}
		
	/* Next, we will will read the lookup tables if in indexed color mode. */
	
	if (header.mode == plugInModeIndexedColor)
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
	
	int32 done;
	int32 total;
	int16 plane;
	int32 row;
	
	/* Dispose of the image resource data if it exists. */
	
	DisposeImageResources ();
	
	/* Set up the progress variables. */
	
	done = 0;
	VPoint imageSize = GetFormatImageSize();
	total = imageSize.v * gFormatRecord->planes;
		
	/* Next, we will allocate the pixel buffer. */

	unsigned32 bufferSize = RowBytes();
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

	DoReadICCProfile ();
	
}

/*****************************************************************************/

static void DoReadFinish (void)
{
	/* Test the ability to create the file inside a smart object */
	/* This flag also tells you which menu item was selected */

	// if openAsSmartObject is already true then you cannot turn it off
	gFormatRecord->openAsSmartObject = gData->openAsSmartObject;
	
	/* Dispose of the image resource data if it exists. */
	DisposeImageResources ();
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

	dataBytes = sizeof (FileHeader) +
				gFormatRecord->imageRsrcSize +
				RowBytes () * gFormatRecord->planes * imageSize.v;
					  
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
    gData->usePOSIX = true;
	gData->saveResources = true;

	// script params may change our usePOSIX and saveResources
    gData->showDialog = ReadScriptParamsOnWrite ();

  #if __PIMac__
    if (gFormatRecord->hostSupportsPOSIXIO && gData->usePOSIX)
    {
        gFormatRecord->pluginUsingPOSIXIO = true;
        gLogger->Write( "Using POSIX " );
    }
    else
    {
        gData->usePOSIX = false;
        gLogger->Write( "Using FS " );
    }
  #endif

}

/*****************************************************************************/

static void DoWriteStart (void)
{

	FileHeader header;
	memset(&header, 0, sizeof(FileHeader));
	int32 done;
	int32 total;
	int16 plane;
	int32 row;
	
	/* See if I have any of my comments in this file */
	/* I don't do anything with them but it was fun. */

	int16 resourceCount = gCountResources(histResource);

	while (resourceCount)
	{
		// a read only handle, no need to dispose
		Handle resourceHandle = gGetResources(histResource,resourceCount--);
		Boolean oldLock = FALSE;
		Ptr resourcePtr = NULL;
		sPSHandle->SetLock(resourceHandle, true, &resourcePtr, &oldLock);
		sPSHandle->SetLock(resourceHandle, false, &resourcePtr, &oldLock);
	}

	/* Write the header. */
	
	*gResult = PSSDKSetFPos ((int)gFormatRecord->dataFork,
                             gFormatRecord->posixFileDescriptor,
                             gFormatRecord->pluginUsingPOSIXIO,
                             fsFromStart,
                             0);
	if (*gResult != noErr) return;
	
	SetIdentifier (header.identifier);
	VPoint imageSize = GetFormatImageSize();

	uint32 tempLong = 0x11223344;
	uint8 tempChar = *(reinterpret_cast<uint8 *>(&tempLong) + 3);
	bool isLittleEndian = (tempChar == 0x11);
	header.endian = isLittleEndian ? LITTLEENDIAN : BIGENDIAN;
	header.testendian = TESTENDIAN;
	header.mode = gFormatRecord->imageMode;
	header.rows = imageSize.v;
	header.cols = imageSize.h;
	header.depth = gFormatRecord->depth;
	header.planes = gFormatRecord->planes;
	header.transparencyPlane = gFormatRecord->transparencyPlane;
	
	if (gData->saveResources && gFormatRecord->imageRsrcData)
	{
		header.resourceLength = gFormatRecord->imageRsrcSize;
	}
	else
	{
		header.resourceLength = 0;
		WriteSome (sizeof (FileHeader), &header);
	}
	
	if (*gResult != noErr) return;
	
	/* Write the image resources if any. */
	
	if (header.resourceLength > 0)
	{
		Boolean oldLock = FALSE;
		Ptr p = NULL;
		sPSHandle->SetLock(gFormatRecord->imageRsrcData, true, &p, &oldLock);
		if (p != NULL)
		{
			vector<ResourceInfo *> resources;
			CreateResourceInfoVector(header.resourceLength, 
				                     reinterpret_cast<uint8 *>(p), 
									 resources);
			// see if the user wants to remove some resources
			if (gData->showDialog && DoUI(resources))
			{
				header.resourceLength = RemoveResources(header.resourceLength, 
					                                    resources, 
														reinterpret_cast<uint8 *>(p));
			}
			DeleteResourceInfoVector(resources);

			WriteSome (sizeof (FileHeader), &header);
			WriteSome (header.resourceLength, p);
			sPSHandle->SetLock(gFormatRecord->imageRsrcData, false, &p, &oldLock);
		}
		
		if (*gResult != noErr) return;
		
	}
		
	/* Write the lookup tables if appropriate. */
	
	if (header.mode == plugInModeIndexedColor)
	{
		
		WriteSome (3 * sizeof (LookUpTable), &gFormatRecord->redLUT);
		
		if (*gResult != noErr) return;
		
	}
		
	/* Set up the progress variables. */
	
	done = 0;
	total = header.rows * header.planes;
		
	/* Next, we will allocate the pixel buffer. */
	unsigned32 bufferSize = RowBytes();
	Ptr pixelData = sPSBuffer->New( &bufferSize, bufferSize );
	if (pixelData == NULL)
	{
		*gResult = memFullErr;
		return;
	}
		
	/* Set up to start receiving chunks of data. */
	
	VRect theRect;

	theRect.left = 0;
	theRect.right = header.cols;
	gFormatRecord->colBytes = (gFormatRecord->depth + 7) >> 3;
	gFormatRecord->rowBytes = RowBytes();
	gFormatRecord->planeBytes = 0;
	gFormatRecord->data = pixelData;
	gFormatRecord->transparencyMatting = DESIREDMATTING;

	for (plane = 0; *gResult == noErr && plane < gFormatRecord->planes; ++plane)
	{
		
		gFormatRecord->loPlane = gFormatRecord->hiPlane = plane;
		
		for (row = 0; *gResult == noErr && row < imageSize.v; ++row)
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

	DoWriteICCProfile ();

}

/*****************************************************************************/

static void DoWriteContinue (void)
{
}

/*****************************************************************************/

static void DoWriteFinish (void)
{
	WriteScriptParamsOnWrite (); // should be different for read/write
}

/*****************************************************************************/

static void DoFilterFile (void)
{
	
	FileHeader header;
	
	/* Exit if we have already encountered an error. */

    if (*gResult != noErr) return;

	/* Read the file header. */

	*gResult = PSSDKSetFPos ((int)gFormatRecord->dataFork,
                             gFormatRecord->posixFileDescriptor,
                             gFormatRecord->pluginUsingPOSIXIO,
                             fsFromStart, 0);
	if (*gResult != noErr) return;
	
	ReadSome (sizeof (FileHeader), &header);
	
	if (*gResult != noErr) return;
	
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
		Boolean oldLock = FALSE;
		Ptr p = NULL;
		sPSHandle->SetLock(h, true, &p, &oldLock);
		if (p != NULL)
		{
			for (size_t a = 0; a < length; a++)
				*p++ = currentTime.at(a);
			gAddResource(histResource, h);
			sPSHandle->SetLock(h, false, &p, &oldLock);
		}
		sPSHandle->Dispose(h);
	}
} // end AddComment

/**************************************************************************/
// The size should be bigger than 127 bytes and the signature acsp should
// be at byte 36 to 39 per this document. The first 4 bytes are the size
// again so double check that.
// http://www.color.org/specification/ICC1v43_2010-12.pdf
/**************************************************************************/
static void DoReadICCProfile(void)
{
	if (gFormatRecord->canUseICCProfiles)
	{
		ReadSome(sizeof(gFormatRecord->iCCprofileSize), &gFormatRecord->iCCprofileSize);
		if (*gResult == noErr && gFormatRecord->iCCprofileSize > 127)
		{
			gFormatRecord->iCCprofileData = sPSHandle->New(gFormatRecord->iCCprofileSize);
			Boolean oldLock = FALSE;
			Ptr data = NULL;
			sPSHandle->SetLock(gFormatRecord->iCCprofileData, true, &data, &oldLock);
			if (gFormatRecord->iCCprofileData != NULL && data != NULL)
			{
				ReadSome(gFormatRecord->iCCprofileSize, data);
				uint32 * l = (uint32*)data;
				Swap(*l);
				if (*l != gFormatRecord->iCCprofileSize)
					*gResult = readErr;
				if (*(data + 36) != 'a' ||
					*(data + 37) != 'c' ||
					*(data + 38) != 's' ||
					*(data + 39) != 'p')
					*gResult = readErr;
				Swap(*l);
				sPSHandle->SetLock(gFormatRecord->iCCprofileData, false, &data, &oldLock);
			}
		}
		else
		{
			*gResult = noErr; // it's an old file or one without ICC info, no worries
		}
	}
}

/*****************************************************************************/

static void DoWriteICCProfile(void)
{
	if (gFormatRecord->canUseICCProfiles)
	{
		if (gFormatRecord->iCCprofileSize && gFormatRecord->iCCprofileData)
		{
			WriteSome(sizeof(gFormatRecord->iCCprofileSize), &gFormatRecord->iCCprofileSize);
			Boolean oldLock = FALSE;
			Ptr data = NULL;
			sPSHandle->SetLock(gFormatRecord->iCCprofileData, true, &data, &oldLock);
			if (data != NULL)
			{
				WriteSome(gFormatRecord->iCCprofileSize, data);
				sPSHandle->SetLock(gFormatRecord->iCCprofileData, false, &data, &oldLock);
			}
		}
	}
}

/*****************************************************************************/

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

/*****************************************************************************/

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

/*****************************************************************************/

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

/*****************************************************************************/

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

/*****************************************************************************/

static void DeleteResourceInfoVector(vector<ResourceInfo *> & rInfos)
{
	size_t count = rInfos.size();
	for (size_t a = 0; a < count; a++)
	{
		ResourceInfo * thisResource = rInfos.at(a);
		delete thisResource;
	}
}

/*****************************************************************************/

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

/*****************************************************************************/

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
		if ((last - first) < sizeof(uint32)) break;
		
		// Read the block size
		uint32 blockSize (GetBigEndian<uint32>(first));
		
		// Skip the block
		first += (blockSize + 1) & ~uint32(1); // Add one if odd for alignment;
		
		if ((readType == type) && (readID == id)) {
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

/*****************************************************************************/

#if __PIMac__

bool DoUI (vector<ResourceInfo *> & rInfos)
{
	return false;
}

/*****************************************************************************/

void DoAbout(SPPluginRef plugin, int dialogID)
{
}

#endif

// end SimpleFormat.cpp
