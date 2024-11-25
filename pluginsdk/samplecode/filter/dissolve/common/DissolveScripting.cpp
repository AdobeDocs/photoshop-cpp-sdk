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

#include "DissolveScripting.h"

//-------------------------------------------------------------------------------
//
// ReadScriptParameters
//
// See if we were called by the Photoshop scripting system and return the value
// in displayDialog if the user wants to see our dialog.
// 
//-------------------------------------------------------------------------------
OSErr ReadScriptParameters(Boolean* displayDialog)
{
	OSErr err = noErr;
	PIReadDescriptor token = NULL;
	DescriptorKeyID key = 0;
	DescriptorTypeID type = 0;
	DescriptorUnitID units;
	int32 flags = 0;
	double percent;
	DescriptorEnumID disposition;
	Boolean ignoreSelection;
	DescriptorKeyIDArray array = { keyAmount, keyDisposition, 0 };

	if (displayDialog != NULL)
		*displayDialog = gData->queryForParameters;
	else
		return errMissingParameter;

	PIDescriptorParameters* descParams = gFilterRecord->descriptorParameters;
	if (descParams == NULL) return err;
	
	ReadDescriptorProcs* readProcs = gFilterRecord->descriptorParameters->readDescriptorProcs;
	if (readProcs == NULL) return err;

	gParams->ignoreSelection = false;

	if (descParams->descriptor != NULL)
	{
		token = readProcs->openReadDescriptorProc(descParams->descriptor, array);
		if (token != NULL)
		{
			while(readProcs->getKeyProc(token, &key, &type, &flags) && !err)
			{
				switch (key)
				{
					case keyAmount:
						err = readProcs->getUnitFloatProc(token, &units, &percent);
						if (!err)
							gParams->percent = (int16)percent;
						break;
					case keyDisposition:
						err = readProcs->getEnumeratedProc(token, &disposition);
						if (!err)
						{
							gParams->disposition = ScriptToDialog(disposition);
							CopyColor(gData->color, 
								      gData->colorArray[gParams->disposition]);
						}
						break;
					case keyIgnoreSelection:
						err = readProcs->getBooleanProc(token, &ignoreSelection);
						if (!err)
							gParams->ignoreSelection = ignoreSelection;
						break;
					default:
						err = readErr;
						break;
				}
			}
			err = readProcs->closeReadDescriptorProc(token);
			gFilterRecord->handleProcs->disposeProc(descParams->descriptor);
			descParams->descriptor = NULL;
		}
		*displayDialog = descParams->playInfo == plugInDialogDisplay;
	}
	return err;
}



//-------------------------------------------------------------------------------
//
// WriteScriptParameters
//
// Write our parameters to the Photoshop scripting system in case we are being
// recorded in the actions pallete.
// 
//-------------------------------------------------------------------------------
OSErr WriteScriptParameters(void)
{
	OSErr err = noErr;
	PIWriteDescriptor token = NULL;
	PIDescriptorHandle h;
	const double percent = gParams->percent;

	PIDescriptorParameters*	descParams = gFilterRecord->descriptorParameters;
	if (descParams == NULL) return err;
	
	WriteDescriptorProcs* writeProcs = gFilterRecord->descriptorParameters->writeDescriptorProcs;
	if (writeProcs == NULL) return err;

	token = writeProcs->openWriteDescriptorProc();
	if (token != NULL)
	{
		writeProcs->putUnitFloatProc(token, 
			                         keyAmount, 
									 unitPercent, 
									 &percent);
		writeProcs->putEnumeratedProc(token, 
			                          keyDisposition, 
									  typeMood, 
									  DialogToScript(gParams->disposition));
		if (gParams->ignoreSelection)
			writeProcs->putBooleanProc(token, 
			                           keyIgnoreSelection, 
									   gParams->ignoreSelection);
		gFilterRecord->handleProcs->disposeProc(descParams->descriptor);
		writeProcs->closeWriteDescriptorProc(token, &h);
		descParams->descriptor = h;
		descParams->recordInfo = plugInDialogOptional;
	}
	else
	{
		return errMissingParameter;
	}
	return err;
}



//-------------------------------------------------------------------------------
//
// DialogToScript
//
// Convert a dialog variable to a scripting variable.
// 
//-------------------------------------------------------------------------------
int32 DialogToScript(int16 dialog)
{
	switch (dialog)
	{
		case 0:
			return dispositionClear;
			break;
		case 1:
			return dispositionCool;
			break;
		case 2:
			return dispositionHot;
			break;
		case 3:
			return dispositionSick;
			break;
	}
	return dispositionCool;
}



//-------------------------------------------------------------------------------
//
// ScriptToDialog
//
// Convert a scripting variable to a dialog variable.
// 
//-------------------------------------------------------------------------------
int16 ScriptToDialog(int32 script)
{
	switch (script)
	{
		case dispositionClear:
			return 0;
			break;
		case dispositionCool:
			return 1;
			break;
		case dispositionHot:
			return 2;
			break;
		case dispositionSick:
			return 3;
			break;
	}
	return 1;
}

// end DissolveScripting.cpp
