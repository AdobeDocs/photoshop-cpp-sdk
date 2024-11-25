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
//		GetInfoFromPhotoshop.cpp
//
//	Description:
//		Get all the information you can from the Photoshop actions system. This
//		plug in uses the PIUActionUtils routines to dump this information to a 
//		log file called "Getter.log".
//	
//		NOTE: The "Getter.log" file makes it look like you can set the
//		information. This is not true. I am only using the PIUActionUtils routine
//		so I don't have to rewrite the same code into a different format.
//
//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
//	Includes
//-------------------------------------------------------------------------------

#define _MSL_MACHEADERS_INCLUDES_CMATH 1

#if defined(__cplusplus) && TARGET_API_MAC_CARBON && _MSL_MACHEADERS_INCLUDES_CMATH
	#include <cmath>
#endif

#include "GetInfoFromPhotoshop.h"
#include "PITerminology.h"
#include "PIStringTerminology.h"
#include "PIUSelect.h"
#include <sstream>

//-------------------------------------------------------------------------------
//
//	GetApplicationInfo
//
// This gets all the keys that the classApplication knows about
// check the log file for what key you specifically want
//
//-------------------------------------------------------------------------------
SPErr GetApplicationInfo(char* logfilename)
{
	PIActionDescriptor	result = NULL;
	DescriptorTypeID typeID = 0;
	SPErr error = PIUGetInfo(classApplication, 0, &result, NULL);
	sPSActionControl->StringIDToTypeID("Getting Application Info", &typeID);
	PIUDumpDescriptor(typeID, result, logfilename);
	if (result) sPSActionDescriptor->Free(result);
	return (error);
}



//-------------------------------------------------------------------------------
//
//	GetHistoryInfo
//
// This gets all the keys that the classHistoryState knows about
// check the log file for what key you specifically want
// You can't get the history information from the non-target document
//
//-------------------------------------------------------------------------------
SPErr GetHistoryInfo(char* logfilename)
{
	PIActionDescriptor	result = NULL;
	int32				numHistoryStates, counter;
	SPErr				error = kSPNoError;
	DescriptorTypeID	typeID = 0;
	
	sPSActionControl->StringIDToTypeID("Getting History Info", &typeID);

	error = PIUGetInfo(classHistoryState, keyCount, &numHistoryStates, NULL);

	for (counter = 1; 
	     counter <= numHistoryStates && error == kSPNoError; 
		 counter++)
	{
		error = PIUGetInfoByIndex(counter, classHistoryState, 0, &result, 0);
		PIUDumpDescriptor(typeID, result, logfilename);
		if (result) sPSActionDescriptor->Free(result);
		result = NULL;
	}

	return (error);
}



//-------------------------------------------------------------------------------
//
//	GetLayerInfo
//
// get the layer information from each layer of each document
//
//-------------------------------------------------------------------------------
SPErr GetLayerInfo(char* logfilename)
{
	PIActionDescriptor	result = NULL;
	int32				numDocuments, numLayers;
	SPErr				error = kSPNoError;
	DescriptorTypeID	typeID = 0;

	sPSActionControl->StringIDToTypeID("Getting Layer Info", &typeID);

	error = PIUGetInfo(classApplication, 
		               keyNumberOfDocuments, 
					   &numDocuments, 
					   NULL);
	
	if (numDocuments > 0)	
		for (int32 docCounter = 1; docCounter <= numDocuments && ! error; docCounter++)
		{
			error = PIUGetInfoByIndex(docCounter, 
				                      classDocument, 
									  keyNumberOfLayers, 
									  &numLayers, 
									  NULL);

			if (numLayers > 0)
				for (int32 layCounter = 1; 
				     layCounter <= numLayers && error == kSPNoError; 
					 layCounter++)
				{
					error = PIUGetInfoByIndexIndex(layCounter, 
						                           docCounter, 
												   classLayer, 
												   classDocument, 
												   0, 
												   &result, 
												   NULL);

					PIUDumpDescriptor(typeID, result, logfilename);

					if (result) sPSActionDescriptor->Free(result);
					result = NULL;
				}
		}

	return (error);
}



//-------------------------------------------------------------------------------
//
//	GetPathInfo
//
//	Paths are a different beast because they could have a work path that does not
//	show up as a normal path, I will try to pull that information out from the
//	document, they also don't work for referencing the non active document.
//	
//	NOTE: The keyNumberOfPaths counts the work path but does not report it the 
//	normal way so its
//		sPSActionReference->PutIndex(reference, classPath, pathCounter);
//	for normal paths and
//		sPSActionReference->PutProperty(reference, classPath, keyWorkPath);
//	to get the work path.
//
//-------------------------------------------------------------------------------
SPErr	GetPathInfo(char* logfilename)
{
	SPErr				error = kSPNoError;
	PIActionReference	reference = NULL;
	PIActionDescriptor	result = NULL;
	int32				currentLayerID = 0;
	int32				numLayersAvailable = 0;
 	int32				numPathsAvailable = 0;
 	int32				numPathsCollected = 0;
	int32				normalAndWorkCount = 0;
	DescriptorTypeID	normalTypeID = 0;
	DescriptorTypeID	workTypeID = 0;
	DescriptorTypeID	otherPathID[2];
	DescriptorTypeID	textMaskID = 0;
	DescriptorTypeID	vectorMaskID = 0;
	int32				i = 0;
	int32				layerIndex = 0;
	int32				pathTypeIndex = 0;

	(void) sPSActionControl->StringIDToTypeID("Getting Path Info", &normalTypeID);
	(void) sPSActionControl->StringIDToTypeID("Getting Work Path Info", &workTypeID);

	// look for normal paths and a work path for the document
	// these are the same for the entire document
	error = PIUGetInfo(classDocument, keyNumberOfPaths, &numPathsAvailable, NULL);
	if (error) goto returnError;

	for(i = 0; i < numPathsAvailable && !error; i++)
	{
		error = PIUGetInfoByIndex(i + 1, classPath, 0, &result, NULL);
				
		if (!error) 
		{
			PIUDumpDescriptor(normalTypeID, result, logfilename);
			numPathsCollected++;
			normalAndWorkCount++;
		}
	}

	// see if we didn't get all of them and try the work path
	if (numPathsCollected < numPathsAvailable)
	{
		error = sPSActionReference->Make(&reference);
		if (error) goto returnError;
								
		error = sPSActionReference->PutProperty(reference, 
												classPath, 
												keyWorkPath);
		if (error) goto returnError;
								
		error = sPSActionControl->Get(&result, reference);

		if (!error)
		{
			PIUDumpDescriptor(workTypeID, result, logfilename);
			numPathsCollected++;
			normalAndWorkCount++;
		}

		if (reference != NULL)
		{
			(void) sPSActionReference->Free (reference);
			reference = NULL;
		}
						
		if (result != NULL)
		{
			(void) sPSActionDescriptor->Free(result);
			result = NULL;
		}
	}
	
	(void) sPSActionControl->StringIDToTypeID(kvectorMaskStr, &vectorMaskID);
	(void) sPSActionControl->StringIDToTypeID(ktextShapeStr, &textMaskID);

	otherPathID[0] = vectorMaskID;
	otherPathID[1] = textMaskID;

	error = PIUGetInfo(classDocument, keyNumberOfLayers, &numLayersAvailable, NULL);
	if (error) goto returnError;

	error = PIUGetInfo(classLayer, keyLayerID, &currentLayerID, NULL);
	if (error) goto returnError;

	// now for the rest of them
	// I have to cycle through all the layers because each layer
	// could have a text mask or a vector mask, but not both
	for (layerIndex = 0; layerIndex < numLayersAvailable; layerIndex++)
	{
		// this could error because we are already on that layer, ignore it
		(void) PIUSelectByIndex(classLayer, layerIndex + 1);

		error = PIUGetInfo(classDocument, keyNumberOfPaths, &numPathsAvailable, NULL);
		if (error) goto returnError;

		numPathsCollected = normalAndWorkCount;

		for (pathTypeIndex = 0; (pathTypeIndex < sizeof(otherPathID) / sizeof(otherPathID[0])) &&
			                    (numPathsCollected < numPathsAvailable); pathTypeIndex++)
		{
			error = sPSActionReference->Make(&reference);
			if (error) goto returnError;

			error = sPSActionReference->PutEnumerated(reference, 
													  classPath, 
													  typeOrdinal, 
													  otherPathID[pathTypeIndex]);
			if (error) goto returnError;

			error = sPSActionControl->Get(&result, reference);
					
			if (!error)
			{
				DescriptorTypeID messageID;
				ostringstream messageStream;
				messageStream << "Getting Vector or Text Path Infor for layer " << (layerIndex + 1);
				(void) sPSActionControl->StringIDToTypeID(messageStream.str().c_str(), &messageID);
				PIUDumpDescriptor(messageID, result, logfilename);
				numPathsCollected++;
			} else {
				error = kSPNoError;
			}

			if (reference != NULL)
			{
				(void) sPSActionReference->Free (reference);
				reference = NULL;
			}
						
			if (result != NULL)
			{
				(void) sPSActionDescriptor->Free(result);
				result = NULL;
			}
		}
	}

	(void) PIUSelectByID(classLayer, currentLayerID);

returnError:

	if (reference) sPSActionReference->Free(reference);
	if (result) sPSActionDescriptor->Free(result);

	return (error);
}



//-------------------------------------------------------------------------------
//
//	GetActionInfo
//
// This gets all the keys that the classAction and classActionSet
//
//-------------------------------------------------------------------------------
SPErr GetActionInfo(char* logfilename)
{
	PIActionDescriptor	result = NULL;
	int32				actionSetCount, actionsInSet;
	SPErr				error = kSPNoError;
	DescriptorTypeID	typeID = 0;
	DescriptorTypeID	actionTypeID = 0;

	sPSActionControl->StringIDToTypeID("Getting Action Set Info", &typeID);
	sPSActionControl->StringIDToTypeID("Getting Action Info", &actionTypeID);

	DescriptorTypeID	numberOfActionSetsID;
	int32				numberOfActionSets = -1;
	sPSActionControl->StringIDToTypeID(knumberOfActionSetsStr, &numberOfActionSetsID);
	(void) PIUGetInfo(classApplication, numberOfActionSetsID, &numberOfActionSets, NULL);

	uint32				setCounter = 1;
	if (numberOfActionSets != -1 && (int32)setCounter > numberOfActionSets)
		setCounter = 0;

	while (setCounter)
	{
		// the action stuff doesn't work so well
		// try to get information out of each set until we fail
		// then exit gracefully
		error = PIUGetInfoByIndex(setCounter, classActionSet, 0, &result, 0);
		if (error)
		{ 
			error = 0;
			break;
		}
	
		PIUDumpDescriptor(typeID, result, logfilename);

		//this should give me the same answer
		error = PIUGetSingleItemFromDescriptor(result, 
			                                   keyCount, 
											   &actionSetCount, 
											   NULL);
        if (error) goto returnError;

		// redudancy check for my sanity, yep it works the same.
		error = sPSActionDescriptor->GetInteger(result, 
			                                    keyCount, 
												&actionSetCount);
		if (error) goto returnError;
		
		// looks like it is not counting itself		
		actionSetCount++;

		//this should give me the same answer
		error = PIUGetSingleItemFromDescriptor(result, 
			                                   keyNumberOfChildren, 
											   &actionsInSet, 
											   NULL);
        if (error) goto returnError;

		// redudancy check for my sanity, yep it works the same.
		error = sPSActionDescriptor->GetInteger(result, 
			                                    keyNumberOfChildren, 
												&actionsInSet);
		if (error) goto returnError;

		if (result) sPSActionDescriptor->Free(result);
		result = NULL;
		
		for (int32 counter = 1; counter <= actionsInSet; counter++)
		{
			error = PIUGetInfoByIndexIndex(counter, 
				                           setCounter, 
										   classAction, 
										   classActionSet, 
										   0, 
										   &result, 
										   NULL);


			PIUDumpDescriptor(actionTypeID, result, logfilename);
	
			if (result) sPSActionDescriptor->Free(result);
			result = NULL;
		}

		setCounter++;

		if (numberOfActionSets != -1 && (int32)setCounter > numberOfActionSets)
			setCounter = 0;

	}

returnError:
	if (result) sPSActionDescriptor->Free(result);

	return (error);
}



//-------------------------------------------------------------------------------
//
//	GetDocumentInfo
//
// This gets all the keys that the classDocument knows about
// check the log file for what key you specifically want
//
//-------------------------------------------------------------------------------
SPErr GetDocumentInfo(char* logfilename)
{
	PIActionDescriptor	result = NULL;
	int32				numDocuments;
	SPErr				error = kSPNoError;
	DescriptorTypeID	typeID = 0;

	sPSActionControl->StringIDToTypeID("Getting Document Info", &typeID);

	error = PIUGetInfo(classApplication, 
		               keyNumberOfDocuments, 
					   &numDocuments, 
					   NULL);
	if (error) goto returnError;
	
	if (numDocuments > 0)	
		for (int32 counter = 1; counter <= numDocuments; counter++)
		{
			error = PIUGetInfoByIndex(counter, classDocument, 0, &result, 0);
			if (error) break;

			PIUDumpDescriptor(typeID, result, logfilename);
	
			if (result) sPSActionDescriptor->Free(result);
			result = NULL;
		}

returnError:
	if (result) sPSActionDescriptor->Free(result);
	return (error);
}



//-------------------------------------------------------------------------------
//
//	GetChannelInfo
//
// This gets all the keys that the classChannel knows about
// check the log file for what key you specifically want
//
//-------------------------------------------------------------------------------
SPErr	GetChannelInfo(char* logfilename)
{
	PIActionDescriptor	result = NULL;
	int32				numDocuments, numChannels;
	SPErr				error = kSPNoError;
	DescriptorTypeID	typeID = 0;

	sPSActionControl->StringIDToTypeID("Getting Channel Info", &typeID);

	error = PIUGetInfo(classApplication, 
		               keyNumberOfDocuments, 
					   &numDocuments, 
					   NULL);
	
	if (numDocuments > 0)	
		for (int32 docCounter = 1; docCounter <= numDocuments && ! error; docCounter++)
		{
			error = PIUGetInfoByIndex(docCounter, 
				                      classDocument, 
									  keyNumberOfChannels, 
									  &numChannels, 
									  NULL);

			if (numChannels > 0)
				for (int32 chanCounter = 1; 
				     chanCounter <= numChannels && ! error; 
					 chanCounter++)
				{
					error = PIUGetInfoByIndexIndex(chanCounter,
						                           docCounter,
												   classChannel,
												   classDocument,
												   0,
												   &result,
												   NULL);

					PIUDumpDescriptor(typeID, result, logfilename);

					if (result) sPSActionDescriptor->Free(result);
					result = NULL;
				}
		}

	return (error);
}



//-------------------------------------------------------------------------------
//
//	GetBackgroundInfo
//
// This is a special call for the background of the document. You would think that
// the background would just be a special case for a layer. But nooooo. You need
// to see if the document has a background layer.
//
//-------------------------------------------------------------------------------
SPErr GetBackgroundInfo(char* logfilename)
{
	PIActionReference	reference = NULL;
	PIActionDescriptor	result = NULL;
	int32				numDocuments;
	SPErr				error = kSPNoError;
	DescriptorTypeID	typeID = 0;

	sPSActionControl->StringIDToTypeID("Getting Backround Layer Info", &typeID);

	error = PIUGetInfo(classApplication, 
		               keyNumberOfDocuments, 
					   &numDocuments, 
					   NULL);
	
	if (numDocuments > 0)	
		for (int32 docCounter = 1; docCounter <= numDocuments && ! error; docCounter++)
		{
			error = sPSActionReference->Make(&reference);
	
			if ( ! error)
				error = sPSActionReference->PutProperty(reference, 
				                                    classBackgroundLayer, 
													keyBackground);
			if ( ! error)
				error = sPSActionReference->PutIndex(reference, 
				                                 classDocument, 
												 docCounter);
			if ( ! error)
				error = sPSActionControl->Get(&result, reference);
			
			// we ignore this error on purpose
			// this document may not have a background layer

			if ( ! error)
				PIUDumpDescriptor(typeID, result, logfilename);

			if (reference) sPSActionReference->Free(reference);
			reference = NULL;
			if (result) sPSActionDescriptor->Free(result);
			result = NULL;
			
		}

	return (error);
}
//end of GetInfoFromPhotoshop.cpp