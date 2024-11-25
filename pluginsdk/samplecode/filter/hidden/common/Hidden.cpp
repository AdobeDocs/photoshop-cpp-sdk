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
//		Hidden.cpp
//
//	Description:
//		Utility plug in for AutomationFilter this guy gets and sets pixels.
//		on command.
//
//
//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
//	Includes
//-------------------------------------------------------------------------------

#include "PIDefines.h"
#include "PITypes.h"
#include "PIAbout.h"
#include "PIFilter.h"
#include "PIUtilities.h"
#include "PITerminology.h"
#include "HiddenCommands.h"
#include <vector>
#include <string>
#include <time.h>

using namespace std;

#define HERROR(FUNC) if (true) { error = FUNC; if (error) throw(this); } else

//-------------------------------------------------------------------------------
//	Classes
//-------------------------------------------------------------------------------
class HiddenData {
private:

	DescriptorEnumID			command;
	DescriptorEnumID			enumResult;
	string						channelName;
	int16						channelType;
	uint8						percent;
	int16						error;

	void ReadScriptParameters(void);
	void DoCommand(void);
	void WriteScriptParameters(void);

	ReadChannelDesc * FindChannelDesc(string & name);
	ReadChannelDesc * FindChannelDesc(int16 type);
	void Write(void);
	void GetInfo(PIActionDescriptor & descriptor);

public:
	HiddenData();
	~HiddenData();

	void Start(void);

};


void DoStart(void);

//-------------------------------------------------------------------------------
//	Globals
//-------------------------------------------------------------------------------
SPBasicSuite *				sSPBasic		= NULL;
FilterRecordPtr				gFilterRecord	= NULL;


DLLExport MACPASCAL void PluginMain(const int16 selector,
								    FilterRecordPtr filterRecord,
								    intptr_t * data,
								    int16 * result);

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
//		void *data					Use this to store a pointer to our global
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

	// update our global parameters
	gFilterRecord = filterRecord;
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
		case filterSelectorStart:
			DoStart ();
			break;
		default:
			break;
		}

  }

  catch (...)
  {
  if (NULL != result)
		*result = -1;
  }

}

void DoStart(void)
{
	HiddenData hiddenData;
	hiddenData.Start();
}

//-------------------------------------------------------------------------------
//
//	HiddenData::HiddenData
//
//	Create the plug in.
//
//-------------------------------------------------------------------------------
HiddenData::HiddenData()
	: command(enumUnknownCommand), enumResult(enumOK), channelType(-1), percent(0), error(0)
{
}



//-------------------------------------------------------------------------------
//
//	HiddenData::~HiddenData
//
//	Get rid of anything we made during this plug in life cycle.
//
//-------------------------------------------------------------------------------
HiddenData::~HiddenData()
{
}



//-------------------------------------------------------------------------------
//
//	HiddenData::Start
//
//	This is where all the action is. I can only be called from the scripting
//	system.
//
//-------------------------------------------------------------------------------
void HiddenData::Start(void)
{
	ReadScriptParameters();
	
	DoCommand();
	
	WriteScriptParameters();
}



//-------------------------------------------------------------------------------
//
//	HiddenData::ReadScriptParameters
//
//	Set up the filter based on the descriptor passed in.
//
//-------------------------------------------------------------------------------
void HiddenData::ReadScriptParameters(void)
{
	uint32 stringLength;

	PIDescriptorHandle descHandle;
	descHandle = gFilterRecord->descriptorParameters->descriptor;
	PIActionDescriptor actionDescriptor = NULL;
	
	HERROR(sPSActionDescriptor->HandleToDescriptor(descHandle, &actionDescriptor));

	Boolean hasKey = false;
	
	HERROR(sPSActionDescriptor->HasKey(actionDescriptor, keyCommand, &hasKey));

	if (!hasKey)
	{
		enumResult = enumNoCommand;
		return;
	}

	DescriptorEnumTypeID theType = keyNull;

	HERROR(sPSActionDescriptor->GetEnumerated(actionDescriptor, 
										 keyCommand, 
										 &theType, 
										 &command));

	switch (command)
	{
		// no information to get for enumInfo
		case enumInfo:
			break;

		// read what channel and what percent
		case enumWrite:
			HERROR(sPSActionDescriptor->HasKey(actionDescriptor,
				                          keyChannelName,
										  &hasKey));
			if (!hasKey)
			{
				HERROR(sPSActionDescriptor->HasKey(actionDescriptor,
					                          keyType,
											  &hasKey));

				if (!hasKey)
				{
					enumResult = enumNoTargetChannel;
					return;
				}
				else
				{
					int32 tempType = 0;
					HERROR(sPSActionDescriptor->GetInteger(actionDescriptor,
						                              keyType,
													  &tempType));
					channelType = (int16)tempType;
				}
			}
			else
			{
			
				HERROR(sPSActionDescriptor->GetStringLength(actionDescriptor,
													   keyChannelName,
													   &stringLength));

				vector<char> vc(stringLength+1);

				HERROR(sPSActionDescriptor->GetString(actionDescriptor,
					                             keyChannelName,
												 &vc[0],
												 (uint32)vc.size()));

				channelName.assign(vc.begin(), vc.begin() + stringLength);
			}

			HERROR(sPSActionDescriptor->HasKey(actionDescriptor, 
				                           keyPercent, 
										   &hasKey));

			if (!hasKey)
			{
				enumResult = enumNoPercent;
				return;
			}

			{
				int32 longP = 0;
				HERROR(sPSActionDescriptor->GetInteger(actionDescriptor,
				                                  keyPercent,
											      &longP));

				percent = (uint8)longP;
			}

			if (actionDescriptor != NULL) sPSActionDescriptor->Free(actionDescriptor);

			break;

		default:
			enumResult = enumUnknownCommand;
			break;
	}
}



//-------------------------------------------------------------------------------
//
//	HiddenData::DoCommand
//
//	Do what I am told.
//
//-------------------------------------------------------------------------------
void HiddenData::DoCommand(void)
{
	switch (command)
	{
		// the ReadScriptParameters() already got the necessary information
		// it's time to write
		case enumWrite:
			Write();
			break;

		// the WriteScriptParameters() will do the proper thing for this command
		case enumInfo:
			break;

		default:
			enumResult = enumUnknownCommand;
	}
}



//-------------------------------------------------------------------------------
//
//	HiddenData::WriteScriptParameters
//
//	Write out what happened.
//
//-------------------------------------------------------------------------------
void HiddenData::WriteScriptParameters(void)
{
	PIActionDescriptor actionDescriptor = NULL;
	
	HERROR(sPSActionDescriptor->Make(&actionDescriptor));

	HERROR(sPSActionDescriptor->PutEnumerated(actionDescriptor, 
										 keyResult, 
										 typeResult, 
										 enumResult));

	if (command == enumInfo)
	{
		GetInfo(actionDescriptor);
	}

	if (gFilterRecord->descriptorParameters != NULL && 
		gFilterRecord->descriptorParameters->descriptor != NULL &&
		gFilterRecord->handleProcs != NULL)
	{
		HandleProcs * handleProcs = gFilterRecord->handleProcs;
		handleProcs->disposeProc(gFilterRecord->descriptorParameters->descriptor);
		gFilterRecord->descriptorParameters->descriptor = NULL;
	}

	HERROR(sPSActionDescriptor->AsHandle(actionDescriptor, 
						&gFilterRecord->descriptorParameters->descriptor));

	if (actionDescriptor != NULL) sPSActionDescriptor->Free(actionDescriptor);
}



//-------------------------------------------------------------------------------
//
//	HiddenData::FindChannelDesc
//
//	Set the readChannelDesc based on the channel type the user wants to 
//	manipulate.
//
//-------------------------------------------------------------------------------
ReadChannelDesc * HiddenData::FindChannelDesc(int16 type)
{
	ReadChannelDesc * readChannelDesc = gFilterRecord->documentInfo->targetCompositeChannels;
	while (readChannelDesc != NULL)
	{
		if (type == readChannelDesc->channelType)
			return readChannelDesc;
		readChannelDesc = readChannelDesc->next;
	}

	readChannelDesc = gFilterRecord->documentInfo->targetLayerMask;
	while (readChannelDesc != NULL)
	{
		if (type == readChannelDesc->channelType)
			return readChannelDesc;
		readChannelDesc = readChannelDesc->next;
	}

	readChannelDesc = gFilterRecord->documentInfo->targetTransparency;
	while (readChannelDesc != NULL)
	{
		if (type == readChannelDesc->channelType)
			return readChannelDesc;
		readChannelDesc = readChannelDesc->next;
	}

	readChannelDesc = gFilterRecord->documentInfo->alphaChannels;
	while (readChannelDesc != NULL)
	{
		if (type == readChannelDesc->channelType)
			return readChannelDesc;
		readChannelDesc = readChannelDesc->next;
	}

	readChannelDesc = gFilterRecord->documentInfo->selection;
	while (readChannelDesc != NULL)
	{
		if (type == readChannelDesc->channelType)
			return readChannelDesc;
		readChannelDesc = readChannelDesc->next;
	}
	
	return NULL;
}



//-------------------------------------------------------------------------------
//
//	HiddenData::FindChannelDesc
//
//	Set the readChannelDesc based on the channel name the user wants to 
//	manipulate.
//
//-------------------------------------------------------------------------------
ReadChannelDesc * HiddenData::FindChannelDesc(string & name)
{
	ReadChannelDesc * readChannelDesc = gFilterRecord->documentInfo->targetCompositeChannels;
	while (readChannelDesc != NULL)
	{
		if (name.compare(readChannelDesc->name) == 0)
			return readChannelDesc;
		readChannelDesc = readChannelDesc->next;
	}

	readChannelDesc = gFilterRecord->documentInfo->targetLayerMask;
	while (readChannelDesc != NULL)
	{
		if (name.compare(readChannelDesc->name) == 0)
			return readChannelDesc;
		readChannelDesc = readChannelDesc->next;
	}

	readChannelDesc = gFilterRecord->documentInfo->targetTransparency;
	while (readChannelDesc != NULL)
	{
		if (name.compare(readChannelDesc->name) == 0)
			return readChannelDesc;
		readChannelDesc = readChannelDesc->next;
	}

	readChannelDesc = gFilterRecord->documentInfo->alphaChannels;
	while (readChannelDesc != NULL)
	{
		if (name.compare(readChannelDesc->name) == 0)
			return readChannelDesc;
		readChannelDesc = readChannelDesc->next;
	}

	readChannelDesc = gFilterRecord->documentInfo->selection;
	while (readChannelDesc != NULL)
	{
		if (name.compare(readChannelDesc->name) == 0)
			return readChannelDesc;
		readChannelDesc = readChannelDesc->next;
	}

	return NULL;
}



//-------------------------------------------------------------------------------
//
//	HiddenData::Write
//
//	Given a readChannelDesc and a percent write to all the pixels.
//	If the percent is -1 then write a random number to all the pixels.
//
//-------------------------------------------------------------------------------
void HiddenData::Write(void)
{
	// find out which channel we are writing to
	// this was already read in from the scripting parameters
	ReadChannelDesc * readChannelDesc = NULL;
	if (channelName.empty())
		readChannelDesc = FindChannelDesc(channelType);
	else
		readChannelDesc = FindChannelDesc(channelName);
	if (readChannelDesc == NULL)
	{
		error = filterBadParameters;
		throw(this);
	}

	// check some things to see if we should continue
	if (readChannelDesc->tileSize.v == 0)
	{
		error = filterBadParameters;
		throw(this);
	}

	if (readChannelDesc->tileSize.h == 0)
	{
		error = filterBadParameters;
		throw(this);
	}

	Boolean canWrite = false;
	if (readChannelDesc->writePort == NULL)
	{
		error = filterBadParameters;
		throw(this);
	}
	
	int32 bigError = sPSChannelProcs->CanWrite(readChannelDesc->writePort, &canWrite);
	if (bigError != 0 || !canWrite)	
	{
		error = filterBadParameters;
		throw(this);
	}

	// set up for tiling the document
	int32 docHeight = readChannelDesc->bounds.bottom - readChannelDesc->bounds.top;
	int32 docWidth = readChannelDesc->bounds.right - readChannelDesc->bounds.left;
	int32 tileHeight = readChannelDesc->tileSize.v;
	int32 tileWidth = readChannelDesc->tileSize.h;
	
	uint32 tilesVert = (tileHeight - 1 + docHeight) / tileHeight;
	
	uint32 tilesHoriz = (tileWidth - 1 + docWidth) / tileWidth;

	uint32 channelSize = tileHeight * tileWidth * readChannelDesc->depth / 8;

	// get a buffer that we can read and write into
	Ptr pixelData = sPSBuffer->New(NULL, channelSize);	
	if (pixelData == NULL)
	{
		error = filterBadParameters;
		throw(this);
	}

	// set up the PixelMemoryDesc for the channel ports suite
	PixelMemoryDesc pixelMemoryDesc;
	pixelMemoryDesc.data = pixelData;
	pixelMemoryDesc.colBits = readChannelDesc->depth;
	pixelMemoryDesc.bitOffset = 0;
	pixelMemoryDesc.depth = readChannelDesc->depth;

	VRect currentRect;

	int32 progressTotal = tilesVert * tilesHoriz;
	int32 progressDone = 0;

	// process each tile in order
	for (uint32 vertTile = 0; vertTile < tilesVert && error == 0; vertTile++)
	{
		for (uint32 horizTile = 0; horizTile < tilesHoriz && error == 0; horizTile++)
		{
			currentRect.top = vertTile * tileHeight;
			currentRect.left = horizTile * tileWidth;
			currentRect.bottom = currentRect.top + tileHeight;
			currentRect.right = currentRect.left + tileWidth;

			// don't go past the document bounds
			if (currentRect.bottom > docHeight)
				currentRect.bottom = docHeight;
			if (currentRect.right > docWidth)
				currentRect.right = docWidth;

			pixelMemoryDesc.rowBits = (currentRect.right - currentRect.left) * 
		                              readChannelDesc->depth;

			// read
			bigError = sPSChannelProcs->ReadPixelsFromLevel(
							readChannelDesc->port, 0, &currentRect, &pixelMemoryDesc);

			if (bigError)
			{
				error = filterBadParameters;
				throw(this);
			}

			// muck with
			if (percent == kRandomPercent)
			{
				srand((unsigned int)(time(NULL)));
				Ptr pixel = pixelData;
				uint16 * bigPixel = reinterpret_cast<uint16 *>(pixelData);
				float * fPixel = reinterpret_cast<float *>(pixelData);
				for(uint32 a = 0; a < channelSize; a++, pixel++, bigPixel++, fPixel++)
					if (pixelMemoryDesc.depth == 32)
					{
						*fPixel = static_cast<float>( (float)rand() / (float)RAND_MAX );
						a += 3; // 3 because we get one in the loop for free
					} 
					else if (pixelMemoryDesc.depth == 16)
					{
						*bigPixel = static_cast<uint16>( rand() % 32768 );
						a++; // big pixels are half the channelSize
					}
					else
					{
						*pixel = static_cast<uint8>( rand() % 255 );
					}
			}
			else
			{
				if (pixelMemoryDesc.depth == 32)
				{
					float * fPixel = reinterpret_cast<float *>(pixelData);
					for(uint32 a = 0; a < channelSize; a += 4, fPixel++)
						*fPixel = static_cast<float>( (float)percent / (float)100 );
				}
				else if (pixelMemoryDesc.depth == 16)
				{
					uint16 * bigPixel = reinterpret_cast<uint16 *>(pixelData);
					for(uint32 a = 0; a < channelSize; a += 2, bigPixel++)
						*bigPixel = static_cast<uint16>(percent * 32768 / 100);
				}
				else
					memset(pixelData, percent * 255 / 100, channelSize);
			}

			// write
			bigError = sPSChannelProcs->WritePixelsToBaseLevel(
				readChannelDesc->writePort, &currentRect, &pixelMemoryDesc);

			if (bigError)
			{
				error = filterBadParameters;
				throw(this);
			}

			gFilterRecord->progressProc(++progressDone, progressTotal);
			
			// see if the user is impatient or didn't mean to do that
			if (gFilterRecord->abortProc())
			{
				error = userCanceledErr;
				throw(this);
			}

		}
	}

	sPSBuffer->Dispose(&pixelData);
}



//-------------------------------------------------------------------------------
//
//	HiddenData::GetInfo
//
//	Report back the tiling information and the status of the ports available for
//	writing.
//
//-------------------------------------------------------------------------------
void HiddenData::GetInfo(PIActionDescriptor & descriptor)
{
	HERROR(sPSActionDescriptor->PutInteger(descriptor,
			                          keyHeight,
									  gFilterRecord->outTileHeight));

	HERROR(sPSActionDescriptor->PutInteger(descriptor,
									  keyWidth,
									  gFilterRecord->outTileWidth));
}
// end Hidden.cpp

