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
//		PropetizerScripting.cpp
//
//	Description:
//		This file contains the scripting functions
//		for the Filter module Propetizer, a module that
//		examines and displays grids and guide settings,
//		then makes pseudo-resource history entries when
//		you change them.
//
//	Use:
//		This is a basic module to exemplify all the typical
//		functions a filter module will do with a special
//		emphasis on assessing properties and writing
//		pseudo-resources.
//
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
//	Includes
//-------------------------------------------------------------------------------

#include "Propetizer.h"
#include "PropetizerTerminology.h"
#include <vector>
#include "PITerminology.h"

//-------------------------------------------------------------------------------
//
//	PropetizerData::ReadScriptParameters
//
//	Set up the filter based on the descriptor passed in.
//
//-------------------------------------------------------------------------------
bool PropetizerData::ReadScriptParameters(void)
{
	bool popDialog = true;
	SPErr error = 0;

	if (gFilterRecord->descriptorParameters != NULL && 
		gFilterRecord->descriptorParameters->descriptor != NULL)
	{
		vector<char> vc;

		PIDescriptorHandle descHandle = NULL;
		descHandle = gFilterRecord->descriptorParameters->descriptor;
		
		popDialog = gFilterRecord->descriptorParameters->playInfo == plugInDialogDisplay;

		PIActionDescriptor actionDescriptor = NULL;
		sPSActionDescriptor->HandleToDescriptor(descHandle, &actionDescriptor);

		Boolean hasKey;
		uint32 length;
	
		error = sPSActionDescriptor->HasKey(actionDescriptor, keyMyNudgeH, &hasKey);
		if (!error && hasKey)
			sPSActionDescriptor->GetFloat(actionDescriptor, keyMyNudgeH, &fBigNudgeH);

		error = sPSActionDescriptor->HasKey(actionDescriptor, keyMyNudgeV, &hasKey);
		if (!error && hasKey)
			sPSActionDescriptor->GetFloat(actionDescriptor, keyMyNudgeV, &fBigNudgeV);

		error = sPSActionDescriptor->HasKey(actionDescriptor, keyMyHorizontal, &hasKey);
		if (!error && hasKey)
			sPSActionDescriptor->GetFloat(actionDescriptor, keyMyHorizontal, &fRulerOriginH);

		error = sPSActionDescriptor->HasKey(actionDescriptor, keyMyVertical, &hasKey);
		if (!error && hasKey)
			sPSActionDescriptor->GetFloat(actionDescriptor, keyMyVertical, &fRulerOriginV);

		error = sPSActionDescriptor->HasKey(actionDescriptor, keyMyGridMajor, &hasKey);
		if (!error && hasKey)
			sPSActionDescriptor->GetFloat(actionDescriptor, keyMyGridMajor, &fGridMajor);

		error = sPSActionDescriptor->HasKey(actionDescriptor, keyMyGridMinor, &hasKey);
		if (!error && hasKey)
			sPSActionDescriptor->GetInteger(actionDescriptor, keyMyGridMinor, &fGridMinor);

		error = sPSActionDescriptor->HasKey(actionDescriptor, keyMyWatch, &hasKey);
		int32 watch;
		if (!error && hasKey)
		{
			sPSActionDescriptor->GetInteger(actionDescriptor, keyMyWatch, &watch);
			fWatchSuspension = watch;
		}

		error = sPSActionDescriptor->HasKey(actionDescriptor, keyMyCopyright, &hasKey);
		if (!error && hasKey)
			fCopyright = true;

		error = sPSActionDescriptor->HasKey(actionDescriptor, keyMyURL, &hasKey);
		if (!error && hasKey)
		{
			error = sPSActionDescriptor->GetStringLength(actionDescriptor,
				                                         keyMyURL,
														 &length);
			vc.resize(length + 1);

			if (!error && length)
                error = sPSActionDescriptor->GetString(actionDescriptor, keyMyURL, &vc[0], length);
			if (!error)
				fURL.assign(vc.begin(), vc.begin() + length);
		}


		error = sPSActionDescriptor->HasKey(actionDescriptor, keyMyWater, &hasKey);
		if (!error && hasKey)
			fWatermark = true;

		error = sPSActionDescriptor->HasKey(actionDescriptor, keyMySliceID, &hasKey);
		if (!error && hasKey)
			sPSActionDescriptor->GetInteger(actionDescriptor, keyMySliceID, &fSliceID);

		error = sPSActionDescriptor->HasKey(actionDescriptor, keyMySliceIndex, &hasKey);
		if (!error && hasKey)
			sPSActionDescriptor->GetInteger(actionDescriptor, keyMySliceIndex, &fSliceIndex);

		error = sPSActionDescriptor->HasKey(actionDescriptor, keyMyXMP, &hasKey);
		if (!error && hasKey)
		{
			error = sPSActionDescriptor->GetStringLength(actionDescriptor,
				                                         keyMyXMP,
														 &length);
			vc.resize(length + 1);

			if (!error && length)
                error = sPSActionDescriptor->GetString(actionDescriptor, keyMyXMP, &vc[0], length);
			if (!error)
				fXMP.assign(vc.begin(), vc.begin() + length);
		}

		if (actionDescriptor != NULL) sPSActionDescriptor->Free(actionDescriptor);
	}
	
	return popDialog;
}



//-------------------------------------------------------------------------------
//
//	PropetizerData::WriteScriptParameters
//
//	Write out what happened.
//
//-------------------------------------------------------------------------------
void PropetizerData::WriteScriptParameters(void)
{
	if (gFilterRecord->descriptorParameters != NULL)
	{
		Auto_Desc actionDescriptor;

		if (fChanged[iNudgeH])
			sPSActionDescriptor->PutFloat(actionDescriptor.get(), 
			                              keyMyNudgeH, fBigNudgeH);

		if (fChanged[iNudgeV])
            sPSActionDescriptor->PutFloat(actionDescriptor.get(), 
			                              keyMyNudgeV, fBigNudgeV);

		if (fChanged[iHorizontal])
            sPSActionDescriptor->PutFloat(actionDescriptor.get(), 
			                              keyMyHorizontal, fRulerOriginH);

		if (fChanged[iVertical])
            sPSActionDescriptor->PutFloat(actionDescriptor.get(), 
			                              keyMyVertical, fRulerOriginV);

		if (fChanged[iGridMajor])
            sPSActionDescriptor->PutFloat(actionDescriptor.get(), 
			                              keyMyGridMajor, fGridMajor);

		if (fChanged[iGridMinor])
            sPSActionDescriptor->PutFloat(actionDescriptor.get(), 
			                              keyMyGridMinor, fGridMinor);

		if (fChanged[iWatch])
            sPSActionDescriptor->PutInteger(actionDescriptor.get(), 
			                                keyMyWatch, fWatchSuspension);

		if (fChanged[iCopyright])
            sPSActionDescriptor->PutBoolean(actionDescriptor.get(), 
			                                keyMyCopyright, true);

		if (fChanged[iURL])
			sPSActionDescriptor->PutString(actionDescriptor.get(), 
				                           keyMyURL, const_cast<char*>(fURL.c_str()));

		if (fChanged[iWater])
            sPSActionDescriptor->PutBoolean(actionDescriptor.get(), 
			                                keyMyWater, true);

		if (fChanged[iSliceID])
            sPSActionDescriptor->PutInteger(actionDescriptor.get(), 
			                                keyMySliceID, fSliceID);

		if (fChanged[iSliceIndex])
            sPSActionDescriptor->PutInteger(actionDescriptor.get(), 
			                                keyMySliceIndex, fSliceIndex);

		if (fChanged[iXMP])
            sPSActionDescriptor->PutString(actionDescriptor.get(),
				                            keyMyXMP, const_cast<char*>(fXMP.c_str()));

		if (gFilterRecord->descriptorParameters->descriptor != NULL)
		{
			sPSHandle->Dispose(gFilterRecord->descriptorParameters->descriptor);
			gFilterRecord->descriptorParameters->descriptor = NULL;
		}

		sPSActionDescriptor->AsHandle(actionDescriptor.get(), 
						              &gFilterRecord->descriptorParameters->descriptor);
			
		gFilterRecord->descriptorParameters->recordInfo = plugInDialogOptional;
	}
}
// end PropetizerScripting.cpp
