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

#include "DissolveRegistry.h"

//-------------------------------------------------------------------------------
//
// ReadRegistryParameters
//
// Read our parameters out of the Photoshop Registry. This saves us from writing
// to a preferences file and the pains of managing that on each OS. You should 
// probably ignore errors from this routine as other hosts have not 
// implemented a registry.
//
//-------------------------------------------------------------------------------
SPErr ReadRegistryParameters(void)
{
	SPErr err = noErr;
	SPBasicSuite* basicSuite = gFilterRecord->sSPBasic;
	PSDescriptorRegistryProcs* registryProcs = NULL;
	PSActionDescriptorProcs* descriptorProcs = NULL;
	PIActionDescriptor descriptor = NULL;
	DescriptorUnitID unit;
	double percent;
	DescriptorEnumTypeID type;
	DescriptorEnumID disposition;
	Boolean ignoreSelection;

	if (basicSuite == NULL)
		return errPlugInHostInsufficient;

	err = basicSuite->AcquireSuite(kPSDescriptorRegistrySuite, 
		                           kPSDescriptorRegistrySuiteVersion, 
								   (const void **)&registryProcs);
	if (err) goto returnError;

	err = basicSuite->AcquireSuite(kPSActionDescriptorSuite, 
		                           kPSActionDescriptorSuiteVersion, 
								   (const void **)&descriptorProcs);
	if (err) goto returnError;

	err = registryProcs->Get(plugInUniqueID, &descriptor);
	if (err) goto returnError;
	if (descriptor == NULL) goto returnError;

	err = descriptorProcs->GetUnitFloat(descriptor, 
		                                keyAmount, 
										&unit, 
										&percent);
	if (err) goto returnError;
	gParams->percent = (int16)percent;
	
	err = descriptorProcs->GetEnumerated(descriptor, 
		                                 keyDisposition, 
										 &type, 
										 &disposition);
	if (err) goto returnError;
	gParams->disposition = ScriptToDialog(disposition);
	CopyColor(gData->color, gData->colorArray[gParams->disposition]);
	
	err = descriptorProcs->GetBoolean(descriptor, 
		                              keyIgnoreSelection, 
									  &ignoreSelection);
	if (err) goto returnError;
	gParams->ignoreSelection = ignoreSelection;

returnError:
	if (descriptor != NULL)
		descriptorProcs->Free(descriptor);
	if (registryProcs != NULL)
		basicSuite->ReleaseSuite(kPSDescriptorRegistrySuite, 
		                         kPSDescriptorRegistrySuiteVersion);
	if (descriptorProcs != NULL)
		basicSuite->ReleaseSuite(kPSActionDescriptorSuite, 
		                         kPSActionDescriptorSuiteVersion);
	return err;
}



//-------------------------------------------------------------------------------
//
// WriteRegistryParameters
//
// Write our parameters out to the Photoshop Registry. This saves us from writing
// to a preferences file and the pains of managing that on each OS. You should 
// probably ignore errors from this routine as other hosts have not 
// implemented a registry.
// 
//-------------------------------------------------------------------------------
SPErr WriteRegistryParameters(void)
{
	SPErr err = noErr;
	SPBasicSuite* basicSuite = gFilterRecord->sSPBasic;
	PSDescriptorRegistryProcs* registryProcs = NULL;
	PSActionDescriptorProcs* descriptorProcs = NULL;
	PIActionDescriptor descriptor = NULL;


	if (basicSuite == NULL)
		return errPlugInHostInsufficient;

	err = basicSuite->AcquireSuite(kPSDescriptorRegistrySuite, 
		                           kPSDescriptorRegistrySuiteVersion, 
								   (const void **)&registryProcs);
	if (err) goto returnError;

	err = basicSuite->AcquireSuite(kPSActionDescriptorSuite, 
		                           kPSActionDescriptorSuiteVersion, 
								   (const void **)&descriptorProcs);
	if (err) goto returnError;

	err = descriptorProcs->Make(&descriptor);
	if (err) goto returnError;

	err = descriptorProcs->PutUnitFloat(descriptor, 
		                                keyAmount, 
										unitPercent, 
										(int32)gParams->percent);
	if (err) goto returnError;
	
	err = descriptorProcs->PutEnumerated(descriptor, 
		                                 keyDisposition, 
										 typeMood, 
										 DialogToScript(gParams->disposition));
	if (err) goto returnError;
	
	err = descriptorProcs->PutBoolean(descriptor, 
		                              keyIgnoreSelection, 
									  gParams->ignoreSelection);
	if (err) goto returnError;

	err = registryProcs->Register(plugInUniqueID, descriptor, true);
	if (err) goto returnError;

returnError:
	if (descriptor != NULL)
		descriptorProcs->Free(descriptor);
	if (registryProcs != NULL)
		basicSuite->ReleaseSuite(kPSDescriptorRegistrySuite, 
		                         kPSDescriptorRegistrySuiteVersion);
	if (descriptorProcs != NULL)
		basicSuite->ReleaseSuite(kPSActionDescriptorSuite, 
		                         kPSActionDescriptorSuiteVersion);
	return err;
}

// end DissolveRegistry.cpp