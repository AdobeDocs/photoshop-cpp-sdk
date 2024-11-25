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
//		AutomationFilter.cpp
//
//	Description:
//		An automation plug in that uses a hidden filter plug in to
//		do something very boring.
//
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
//	Includes 
//-------------------------------------------------------------------------------
#include "AutomationFilter.h"
#include "AutomationFilterUI.h"
#include "PITerminology.h"
#include "PIStringTerminology.h"

#define AFERROR(FUNC) if (true) { error = FUNC; if (error) throw(this); } else

//-------------------------------------------------------------------------------
//	Globals
//-------------------------------------------------------------------------------

// everyone needs access to the sPSBasic pointer 
SPBasicSuite * sSPBasic = NULL;
SPPluginRef	gPlugInRef = NULL;

// there is a problem with the SuspendHistory() so we
// need a global error value to solve the problem
SPErr gError = kSPNoError;

//-------------------------------------------------------------------------------
// Funciton Prototypes
//-------------------------------------------------------------------------------

// helper routine for the SuspendHistory()
SPErr Execute(void * parameters);

DLLExport SPAPI SPErr AutoPluginMain(
	const char* caller,	// who is calling
	const char* selector, // what do they want
	void* message	// what is the message
);

//-------------------------------------------------------------------------------
//
//	AutoPluginMain
//
//	All calls to the plug-in module come through this routine.
//	It must be placed first in the resource.  To achieve this,
//	most development systems require this be the first routine
//	in the source.
//
//	The entrypoint will be "pascal void" for Macintosh,
//	"void" for Windows.
//
//-------------------------------------------------------------------------------
DLLExport SPAPI SPErr AutoPluginMain(
	const char* caller,	// who is calling
	const char* selector, // what do they want
	void* message	// what is the message
)
{
	SPErr error = kSPNoError;

	try 
	{
		//all messages contain a SPMessageData*
		SPMessageData* basicMessage;
		basicMessage = (SPMessageData*) message;
		sSPBasic = basicMessage->basic;
		gPlugInRef = basicMessage->self;

		// check for SP interface callers
		if (sSPBasic->IsEqual(caller, kSPInterfaceCaller))
		{
			// pop an about box
			if (sSPBasic->IsEqual(selector, kSPInterfaceAboutSelector))
			{
				DoAbout(gPlugInRef, AboutID);
			}
		}
		
		// Photoshop is calling us
		if (sSPBasic->IsEqual(caller, kPSPhotoshopCaller))
		{
			// the one and only message 
			if (sSPBasic->IsEqual(selector, kPSDoIt))
			{

				AutomationFilterData afData;

				// now that we know more we can cast the message to a PSActionsPlugInMessage*
				PSActionsPlugInMessage* actionsMessage = (PSActionsPlugInMessage*) message;

				error = afData.DoIt(actionsMessage);

			}
		}
	}

	catch(...)
	{
		if (error == 0)
			error = kSPBadParameterError;
	}

	return error;
}

//-------------------------------------------------------------------------------
//
//	AutomationFilterData::AutomationFilterData
//	
//	The constructor which happens in a unique way. Via the AllocatePlugin which
//	gets called from PISPMain.cpp.
//
//-------------------------------------------------------------------------------
AutomationFilterData::AutomationFilterData()
  : numChannels(0), numLayers(0), command(0), 
  hasBackground(false), mainDialog(NULL), tileWidth(0), tileHeight(0), error(0)
{
	channelNames.clear();
	layerNames.clear();
}



//-------------------------------------------------------------------------------
//
//	AutomationFilterData::DoIt
//	
//	Do what we need to do. In a round about way this will get to our Execute()
//	routine. We do it this way so only one entry shows up in the history palette.
//	Pretty cool huh!
//
//-------------------------------------------------------------------------------
SPErr AutomationFilterData::DoIt(PSActionsPlugInMessage * message)
{
	Auto_Ref reference;

	ASZString	nameAsZString = NULL;

	AFERROR(sPSUIHooks->GetPluginName(message->d.self, &nameAsZString));
				
	AFERROR(sPSActionReference->PutEnumerated(reference.get(), 
											  classDocument, 
											  typeOrdinal, 
											  enumTarget));
	
	// If your execute routine error's you will not get the
	// error value from SuspendHistory for version 5.5 of Photoshop.
	// This was fixed for Photoshop 6.0. I am going to use a global error value
	// to work around this problem.
	sPSActionControl->SuspendHistory(reference.get(), 
									::Execute, 
									this,
									nameAsZString);
	
	error = gError;
	gError = kSPNoError;

	sASZString->Release(nameAsZString);

	return (error);
}




//-------------------------------------------------------------------------------
//
//	Execute
//	
//	A helper routine. Get back inside the class and Execute from inside there.
//
//-------------------------------------------------------------------------------
SPErr Execute(void * parameters)
{
	// try/catch this also because there is a layer between you and the 
	// SuspendHistory() call. In Photoshop 5.5 the error return value is not
	// passed back to SuspendHistory() show use a global value instead.
	try
	{
		AutomationFilterData * autoData = static_cast<AutomationFilterData *> (parameters);
		if (autoData == NULL)
		{
			gError = kSPBadParameterError;
			throw(gError);
		}
		gError = autoData->Execute();
	}

	// I could throw the plugin
	catch(AutomationFilterData * p)
	{
		gError = p->GetError();
	}

	// I am throwing SPErr's
	catch(SPErr thrownError)
	{
		gError = thrownError;
	}

	// Somebody could throw anything
	catch(...)
	{
		gError = kSPLogicError;
	}

	return (gError);
}



//-------------------------------------------------------------------------------
//
//	AutomationFilterData::Execute
//	
//	DoIt -> Execute() to this Execute. Now we can get down to business.
//
//-------------------------------------------------------------------------------
SPErr AutomationFilterData::Execute(void)
{
	AFERROR(GetDocumentInfo());

	string t;
	
	AFERROR(ExecuteHiddenFilter(enumInfo, t, 0, 0));
#if !__LP64__
	mainDialog = new AutomationFilterDialog(this);

	int item = mainDialog->Modal(gPlugInRef, NULL, 16001);
	
	if (item == kDCancel_Button)
	{
		// you would think this would be kSPUserCanceledError
		// but no Photoshop doesn't like that
		error = userCanceledErr;
	}

	delete mainDialog;
#else // !__LP64__
	ExecuteFilter("Background", "Red", 50);
#endif // !__LP64__
	return (error);
}



//-------------------------------------------------------------------------------
//
//	AutomationFilterData::GetChannelNames
//	
//	Get all the channel names into a vector. The dialog will use them later.
//
//-------------------------------------------------------------------------------
SPErr AutomationFilterData::GetChannelNames(void)
{
	for (int32 channelIndex = 1; channelIndex <= numChannels; channelIndex++)
	{
		uint32 stringLength = 255;
		char tempName[256];
		
		error = PIUGetInfoByIndex(channelIndex,
			                      classChannel,
								  keyChannelName,
								  tempName,
								  &stringLength);
		if (error) break;

		string stringValue;

		stringValue.assign(tempName, tempName + stringLength);
		
		channelNames.push_back(stringValue);
	}

	return (error);
}



//-------------------------------------------------------------------------------
//
//	AutomationFilterData::GetLayerNames
//	
//	Get all the layer names into a vector. The dialog will use them later.
//	This will skip layer group start and layer group end as I don't need them.
//
//-------------------------------------------------------------------------------
SPErr AutomationFilterData::GetLayerNames(void)
{
	DescriptorKeyID layerSectionID = 0;
	DescriptorKeyID layerSectionTypeID = 0;
	DescriptorKeyID layerSectionStartID = 0;
	DescriptorKeyID layerSectionEndID = 0;
	(void)sPSActionControl->StringIDToTypeID(klayerGroupStr, &layerSectionID);
	(void)sPSActionControl->StringIDToTypeID(klayerGroupTypeStr, &layerSectionTypeID);
	(void)sPSActionControl->StringIDToTypeID(klayerGroupStartStr, &layerSectionStartID);
	(void)sPSActionControl->StringIDToTypeID(klayerGroupEndStr, &layerSectionEndID);

	Auto_Desc result(false); // don't create one we pass into Get(...) routine
	Auto_Ref reference;

	uint32 stringLength = 255;
	Boolean hasKey;
	int32 layerIndex;
	char tempName[256];

	AFERROR(sPSActionReference->PutProperty(reference.get(), 
		                                    classBackgroundLayer, 
											keyBackground));

	AFERROR(sPSActionReference->PutEnumerated(reference.get(), 
											  classDocument, 
											  typeOrdinal, 
											  enumTarget));

	// ignore the error on purpose, we may not have a background layer
	(void)sPSActionControl->Get(&result, reference.get());
	if (result.get() != NULL)
	{
		hasKey = false;
		
		AFERROR(sPSActionDescriptor->HasKey(result.get(), keyName, &hasKey));

		if (hasKey)
		{

			AFERROR(sPSActionDescriptor->GetStringLength(result.get(),
				                                         keyName,
														 &stringLength));

			AFERROR(sPSActionDescriptor->GetString(result.get(),
				                                   keyName,
												   tempName,
												   stringLength+1));

			string stringValue;

			stringValue.assign(tempName, tempName + stringLength);
		
			layerNames.push_back(stringValue);

			hasBackground = true;
		}
	}
	// i can't deal with group start or end
	// i need to drop my numLayers accordingly
	int32 numLayersAdjuster = 0;

	for (layerIndex = 1; layerIndex <= numLayers; layerIndex++)
	{
		DescriptorTypeID layerTypeID = 0;
		DescriptorEnumID layerEnumID = 0;

		AFERROR(PIUGetInfoByIndex(layerIndex,
			                      classLayer,
								  layerSectionID,
								  &layerEnumID,
								  &layerTypeID));

		if (layerTypeID == layerSectionTypeID && 
		    (layerEnumID != layerSectionStartID && 
			 layerEnumID != layerSectionEndID))
		{
			
			stringLength = 255;

			AFERROR(PIUGetInfoByIndex(layerIndex,
			                          classLayer,
									  keyName,
									  tempName,
									  &stringLength));

			string stringValue;

			stringValue.assign(tempName, tempName + stringLength);
		
			layerNames.push_back(stringValue);
		
		} else { // not a group start or group end
			numLayersAdjuster++;
		}

	} // for loop

	numLayers -= numLayersAdjuster;

	return (error);
}



//-------------------------------------------------------------------------------
//
//	AutomationFilterData::GetDocumentInfo
//	
//  Find out everything we need to know about this document.
//
//-------------------------------------------------------------------------------
SPErr AutomationFilterData::GetDocumentInfo(void)
{
	Auto_Desc result(false);
	Auto_Ref reference;

	AFERROR(sPSActionReference->PutEnumerated(reference.get(), 
											  classDocument, 
											  typeOrdinal, 
											  enumTarget));
	
	AFERROR(sPSActionControl->Get(&result, reference.get()));

	AFERROR(sPSActionDescriptor->GetInteger(result.get(),
											keyNumberOfChannels,
											&numChannels));

	AFERROR(GetChannelNames());
	
	AFERROR(sPSActionDescriptor->GetInteger(result.get(),
											keyNumberOfLayers,
											&numLayers));

	AFERROR(GetLayerNames());
	
	return error;
}



//-------------------------------------------------------------------------------
//
//	AutomationFilterData::ExecuteHiddenFilter
//	
//  Run the hidden filter given the following command.
//	You have the source to the hidden filter so you can add new commands.
//
//-------------------------------------------------------------------------------
SPErr AutomationFilterData::ExecuteHiddenFilter(DescriptorEnumID command, 
												  const string & nameOfChannel,
												  int16 typeOfChannel,
												  uint8 percent)
{
	Auto_Desc descriptor;
	Auto_Desc result(false);

	DescriptorTypeID hiddenEventID;
	DescriptorEnumID enumID = enumNoCommand;
	DescriptorEnumTypeID typeID;
	
	AFERROR(sPSActionDescriptor->PutEnumerated(descriptor.get(), 
		                                       keyCommand, 
											   typeCommand, 
											   command));

	if (command == enumWrite)
	{
		if (nameOfChannel.length() == 0)
			error = sPSActionDescriptor->PutInteger(descriptor.get(),
													keyType,
													(int32)typeOfChannel);
		else
			error = sPSActionDescriptor->PutString(descriptor.get(),
												   keyChannelName,
												   (char*)(nameOfChannel.c_str()));
		if (error) throw(this);
		
		AFERROR(sPSActionDescriptor->PutInteger(descriptor.get(), keyPercent, percent));
				
	} // writing pass extra parameters
		
	AFERROR(sPSActionControl->StringIDToTypeID(HiddenUniqueString, 
		                                       &hiddenEventID));
	
	error = sPSActionControl->Play(&result, 
		                           hiddenEventID, 
								   descriptor.get(), 
								   plugInDialogSilent);
	
	if (result.get() != NULL && error == noErr)
		error = sPSActionDescriptor->GetEnumerated(result.get(), 
		                                       keyResult, 
											   &typeID, 
											   &enumID);
	
	if (enumID != enumOK || error)
	{
		error = -1;
		uint32 stringLength;
		if (sPSActionDescriptor->GetStringLength(result.get(), 
			                                     keyMessage, 
												 &stringLength) == 0)
		{
			vector<char> message(stringLength+1);
			(void)sPSActionDescriptor->GetString(result.get(),
				 								 keyMessage,
												 &message[0],
												 stringLength+1);
		}
	}

	// pull the tile data out of the return descriptor
	if (command == enumInfo)
	{
		Boolean hasKey;
		hasKey = false;

		(void)sPSActionDescriptor->HasKey(result.get(), keyWidth, &hasKey);
		
		if (hasKey)
		{
			AFERROR(sPSActionDescriptor->GetInteger(result.get(), keyWidth, &tileWidth));
		}

		hasKey = false;
		(void)sPSActionDescriptor->HasKey(result.get(), keyHeight, &hasKey);

		if (hasKey)
		{
			AFERROR(sPSActionDescriptor->GetInteger(result.get(), keyHeight, &tileHeight));
		}
	}

	return error;
}



//-------------------------------------------------------------------------------
//
//	AutomationFilterData::GetLayerName
//	
//	Return the name of the layer at the given index.
//
//-------------------------------------------------------------------------------
void AutomationFilterData::GetLayerName(int32 index, string & outName)
{
	if (index >= 0 && index < numLayers + hasBackground && index < (int32)layerNames.size())
		outName = layerNames[index];
}



//-------------------------------------------------------------------------------
//
//	AutomationFilterData::GetChannelName
//	
//	Return the name of the channel at the given index.
//
//-------------------------------------------------------------------------------
void AutomationFilterData::GetChannelName(int32 index, string & outName)
{
	if (index >= 0 && index < numChannels && index < (int32)channelNames.size())
		outName = channelNames[index];
}



//-------------------------------------------------------------------------------
//
//	AutomationFilterData::ExecuteFilter
//	
//	Set up the document to target the layer and channel and blast some pixels.
//
// NOTE: You have to blast the transparency layer or your pixel data will not
// show.
//
//-------------------------------------------------------------------------------
void AutomationFilterData::ExecuteFilter(const string & layerName, const string & channelName, uint8 percentWhite)
{
	Boolean isBackground = true;

	PIUSelectByName(classLayer, (char*)(layerName.c_str()));
	PIUSelectByName(classChannel, (char*)(channelName.c_str()));

	PIUGetInfo(classLayer, keyBackground, &isBackground, NULL);

	if (!isBackground)
	{
		// you are on a layer so set the transparency layer
		string t;
		ExecuteHiddenFilter(enumWrite, t, ctTransparency, 100);
	}

	ExecuteHiddenFilter(enumWrite, channelName, ctUnspecified, percentWhite);
}



//-------------------------------------------------------------------------------
//
//	AutomationFilterData::Redraw
//	
//	Tell Photoshop to redraw everything.
//
//-------------------------------------------------------------------------------
void AutomationFilterData::Redraw(void)
{
	Auto_Desc descriptor;
	Auto_Desc result(false);
	AFERROR(sPSActionDescriptor->PutEnumerated(descriptor.get(), keyState, typeState, enumRedrawComplete));
	(void)sPSActionControl->Play(&result,
	                             eventWait, 
								 descriptor.get(),
								 plugInDialogSilent);
}

// end AutomationFilter.cpp



