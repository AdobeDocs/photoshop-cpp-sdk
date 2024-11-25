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
//		SimpleFormatScripting.cpp
//
//	Description:
//		This file contains the source and functions for the
//		UI for the File Format module SimpleFormat, 
//		which writes a flat file with merged document pixels.
//
//	Use:
//		Format modules are called from the Save, Save as,
//		and Save a copy dialogs.  If you have extra
//		options (such as quality settings) query for
//		them here.
//
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
//	Includes
//-------------------------------------------------------------------------------

#include "SimpleFormat.h"

//-------------------------------------------------------------------------------
//
//	ReadScriptParamsOnRead
//
//	Checks the parameters against scripting-returned parameters, if any, and
//	updates the globals to match ones provided by the scripting system.
//
//	Inputs:
//		void		Pointer to global structure.
//
//	Outputs:
//		returns TRUE		If you should pop your dialog.
//		returns FALSE		If you should not pop your dialog.
//
//		gResult				Will return any fatal error.
//
//-------------------------------------------------------------------------------


bool ReadScriptParamsOnRead (void)
{
	DescriptorKeyID				key = 0;
	DescriptorTypeID			type = 0;
	DescriptorKeyIDArray		array = { NULLID };
	int32						flags = 0;
	OSErr						stickyError = noErr;
	bool						returnValue = true;
	
	PIDescriptorParameters * descParams = gFormatRecord->descriptorParameters;
	if (descParams == NULL) return returnValue; 

	ReadDescriptorProcs * readProcs = descParams->readDescriptorProcs;
	if (readProcs == NULL) return returnValue;

	if (descParams->descriptor == NULL) return returnValue;

	PIReadDescriptor token = readProcs->openReadDescriptorProc(descParams->descriptor, array);
	if (token == NULL) return returnValue;

    while (readProcs->getKeyProc(token, &key, &type, &flags))
	{
		switch (key)
		{
			case keyUsePOSIX:
            {
                Boolean readParam = false;
				readProcs->getBooleanProc(token, &readParam);
                if (gFormatRecord->hostSupportsPOSIXIO && readParam)
                    gData->usePOSIX = readParam;
				break;
            }
			case keyOpenAsSmart:
            {
                Boolean readParam = false;
                readProcs->getBooleanProc(token, &readParam);
                gData->openAsSmartObject = readParam;
                break;
            }
		}
	}

	stickyError = readProcs->closeReadDescriptorProc(token); // closes & disposes.
	// Dispose the parameter block descriptor:
	sPSHandle->Dispose(descParams->descriptor);
	descParams->descriptor = NULL;
	
	if (stickyError)
	{
		if (stickyError == errMissingParameter) // missedParamErr == -1715
			;
			/* (descriptorKeyIDArray != NULL)
			   missing parameter somewhere.  Walk IDarray to find which one. */
		else
			*gResult = stickyError;
	}
		
	returnValue = descParams->playInfo == plugInDialogDisplay;
	/* return TRUE if want to show our Dialog */		
	
	return returnValue;
}
		
//-------------------------------------------------------------------------------
//
//	ReadScriptParamsOnWrite
//
//	Checks the parameters against scripting-returned parameters, if any, and
//	updates the globals to match ones provided by the scripting system.
//
//	Inputs:
//		void		Pointer to global structure.
//
//	Outputs:
//		returns TRUE		If you should pop your dialog.
//		returns FALSE		If you should not pop your dialog.
//
//		gResult				Will return any fatal error.
//
//-------------------------------------------------------------------------------


bool ReadScriptParamsOnWrite (void)
{
	DescriptorKeyID				key = 0;
	DescriptorTypeID			type = 0;
	DescriptorKeyIDArray		array = { NULLID };
	int32						flags = 0;
	bool						returnValue = true;
    
	PIDescriptorParameters *	descParams = gFormatRecord->descriptorParameters;
	if (descParams == NULL) return returnValue;
	
	ReadDescriptorProcs * readProcs = gFormatRecord->descriptorParameters->readDescriptorProcs;
	if (readProcs == NULL) return returnValue;

	if (descParams->descriptor == NULL) return returnValue;
    
	PIReadDescriptor token = readProcs->openReadDescriptorProc(descParams->descriptor, array);
	if (token == NULL) return returnValue;

    while (readProcs->getKeyProc(token, &key, &type, &flags))
	{
		switch (key)
		{
			case keyUsePOSIX:
            {
                Boolean readParam = false;
                readProcs->getBooleanProc(token, &readParam);
                if (gFormatRecord->hostSupportsPOSIXIO && readParam)
                    gData->usePOSIX = readParam;
                break;
            }
			case keySaveResources:
			{
				Boolean readParam = true;
				readProcs->getBooleanProc(token, &readParam);
				gData->saveResources = readParam;
				break;
			}
		}
	}
	
	readProcs->closeReadDescriptorProc(token); // closes & disposes.
	// Dispose the parameter block descriptor:
	sPSHandle->Dispose(descParams->descriptor);
	descParams->descriptor = NULL;
	
	returnValue = descParams->playInfo == plugInDialogDisplay;
	/* return TRUE if want to show our Dialog */		
		
	return returnValue;
}

//-------------------------------------------------------------------------------
//
//	WriteScriptParamsOnRead
//
//	Takes our parameters from our global variables and writes them out to the
//	scripting system, which hands it all back to the host.
//
//	Inputs:
//		void			Pointer to global structure.
//
//		Boolean gFooRead		Boolean to write to scripting system.
//
//	Outputs:
//		returns an OSErr		If there was a serious error.
//		returns noErr			If everything worked fine.
//
//-------------------------------------------------------------------------------

OSErr WriteScriptParamsOnRead (void)
{
	OSErr						gotErr = noErr;
	PIDescriptorHandle			h;
			
	PIDescriptorParameters * descParams = gFormatRecord->descriptorParameters;
	if (descParams == NULL) return gotErr;
	
	WriteDescriptorProcs * writeProcs = descParams->writeDescriptorProcs;
	if (writeProcs == NULL) return gotErr;

	PIWriteDescriptor token = writeProcs->openWriteDescriptorProc();
	if (token == NULL) return gotErr;
	
	writeProcs->putBooleanProc(token, keyUsePOSIX, gData->usePOSIX);

	writeProcs->putBooleanProc(token, keyOpenAsSmart, gData->openAsSmartObject);

	sPSHandle->Dispose(descParams->descriptor);
	writeProcs->closeWriteDescriptorProc(token, &h);
	descParams->descriptor = h;
	
	return gotErr;
}

		
//-------------------------------------------------------------------------------
//
//	WriteScriptParamsOnWrite
//
//	Takes our parameters from our global variables and writes them out to the
//	scripting system, which hands it all back to the host.
//
//	Inputs:
//		void			Pointer to global structure.
//
//		Boolean gBarWrite		Boolean to write to scripting system.
//
//	Outputs:
//		returns an OSErr		If there was a serious error.
//		returns noErr			If everything worked fine.
//
//-------------------------------------------------------------------------------

OSErr WriteScriptParamsOnWrite (void)
{
	OSErr						gotErr = noErr;
	PIDescriptorHandle			h;
			
	PIDescriptorParameters * descParams = gFormatRecord->descriptorParameters;
	if (descParams == NULL) return gotErr;
	
	WriteDescriptorProcs * writeProcs = descParams->writeDescriptorProcs;
	if (writeProcs == NULL) return gotErr;

	PIWriteDescriptor token = writeProcs->openWriteDescriptorProc();
	if (token == NULL) return gotErr;

    writeProcs->putBooleanProc(token, keyUsePOSIX, gData->usePOSIX);
	
	writeProcs->putBooleanProc(token, keySaveResources, gData->saveResources);

	sPSHandle->Dispose(descParams->descriptor);
	writeProcs->closeWriteDescriptorProc(token, &h);
	descParams->descriptor = h;
	
	return gotErr;
}

//-------------------------------------------------------------------------------
// end SimpleFormatScripting.cpp
