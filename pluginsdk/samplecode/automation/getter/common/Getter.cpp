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
//		Getter.cpp
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
#define OLDP2C 1

#define _MSL_MACHEADERS_INCLUDES_CMATH 1

#if defined(__cplusplus) && TARGET_API_MAC_CARBON && _MSL_MACHEADERS_INCLUDES_CMATH
	#include <cmath>
#endif

#include "PIDefines.h"
#include "SPHost.h"
#include "GetInfoFromPhotoshop.h"
#include "PIActionsPlugIn.h"
#include "PIUSuites.h"
#include "PIUSelect.h"
#include "PIHandleSuite.h"
#include "PIUI.h"

#if __PIMac__
	#include <stdio.h>
	#ifndef MAX_PATH
		#define MAX_PATH	256
	#endif
#endif

#include "PITerminology.h"
#include "PIStringTerminology.h"

bool HostIsElements(void);

/// Definition of the main entrypoint for all automation plugins
DLLExport SPAPI SPErr AutoPluginMain(const char * caller, const char * selector, void * message);

SPErr DoIt(PSActionsPlugInMessage *message);

//-------------------------------------------------------------------------------
//	Globals
//-------------------------------------------------------------------------------
// the basic suite, needed for the new and delete operators
SPBasicSuite *sSPBasic = NULL;

//-------------------------------------------------------------------------------
//
//	AutoPluginMain / main
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
DLLExport SPAPI SPErr AutoPluginMain(const char* caller, const char* selector, void* message)
{

	SPErr error = kSPNoError;

	try {

	SPMessageData * basicMessage = (SPMessageData *) message;

	sSPBasic = basicMessage->basic;
	
	if (sSPBasic->IsEqual(caller, kSPInterfaceCaller))
	{
		if (sSPBasic->IsEqual(selector, kSPInterfaceAboutSelector))
		{
			DoAbout(basicMessage->self, AboutID);
		}
	}

	// Photoshop is calling
	if (sSPBasic->IsEqual((char*)caller, kPSPhotoshopCaller))
	{
		// the one and only message 
		if (sSPBasic->IsEqual((char*)selector, kPSDoIt))
		{
			//pop a dialog to show plug-in works
			PSActionsPlugInMessage * actionsMessage = (PSActionsPlugInMessage *) message;
			DoIt(actionsMessage);
		}
	}

	
	PIUSuitesRelease();

	}

	catch (...)
	{
		if (error == 0)
			error = kSPBadParameterError;
	}

	return error;
}



//-------------------------------------------------------------------------------
//
//	DoIt
//
//	Finally something interesting. You have been selected from your menu entry
//	or via the actions system to do your thing. Dump everything you can about
//	Photoshop by asking Photoshop. The output file is "Getter.log".
//
//-------------------------------------------------------------------------------
SPErr DoIt(PSActionsPlugInMessage * /*message*/)
{
	SPErr	error = 0;

	char logfilename[MAX_PATH];
	char* filename = (char*)"Getter.log";
	
	error = GetFullPathToDesktop(logfilename, MAX_PATH);
	if (error)
	{
		logfilename[0] = '\0';
		error = kSPBadParameterError;
		return error;
	}

	if (PIstrlcat(logfilename, filename, MAX_PATH-1) >= MAX_PATH-1)
	{
		error = kSPBadParameterError;
		return error;
	}

	GetApplicationInfo(logfilename);
	GetLayerInfo(logfilename);
	GetActionInfo(logfilename);
	GetDocumentInfo(logfilename);
	GetChannelInfo(logfilename);
	GetBackgroundInfo(logfilename);

	// the above routines can do their job by indexing into the open objects
	// path info and history info are only available to the target document
	// 1. remember who is the current target
	// 2. target the other documents by indexing
	// 3. switch back to the original target 
	//    (use the ID it is the most reliable way to switch back)
	int32 currentDocumentID = 0;
	int32 numDocuments = 0;
	int32 docCounter = 0;

	// we are ignoring errors so this will not give an invalid error
	// message when there are no documents open
	(void) PIUGetInfo(classDocument, keyDocumentID, &currentDocumentID, NULL);

	(void) PIUGetInfo(classApplication, keyNumberOfDocuments, &numDocuments, NULL);

	if (numDocuments > 0)
	{
		for (docCounter = 1; docCounter <= numDocuments; docCounter++)
		{
			// this routine will error if we select the document
			// that is already selected, we don't care about this
			// as an error, keep chugging
			(void) PIUSelectByIndex(classDocument, docCounter);
			GetPathInfo(logfilename);
			GetHistoryInfo(logfilename);
		}
		// this routine will error if we select the document
		// that is already selected, we don't care about this
		// as an error, keep chugging
		(void) PIUSelectByID(classDocument, currentDocumentID);
	}

	return (error);
}

bool HostIsElements(void)
{
	SPErr error = 0; 
	// assume Photoshop
	bool hostIsElements = false;

	PIActionReference reference = NULL;
	DescriptorTypeID runtimeKeyID = 0;
	PIActionDescriptor result = NULL;
	PIActionDescriptor versionDesc = NULL;
	Boolean limitedValue, hasKey;
	int32 versionNum = 0;
	DescriptorClassID classID;

	// get all the keys for classApplication
	error = sPSActionReference->Make(&reference);
	if (error) goto returnError;

	error = sPSActionReference->PutEnumerated(reference, classApplication, typeOrdinal, enumTarget);
	if (error) goto returnError;

	error = sPSActionControl->Get(&result, reference);
	if (error) goto returnError;

	error = sPSActionControl->StringIDToTypeID(klimitedStr, &runtimeKeyID); // "limited"
	if (error) goto returnError;

	// best case scenario is that the classApplication has this key
	error = sPSActionDescriptor->HasKey(result, runtimeKeyID, &hasKey);
	if (error) goto returnError;

	if (hasKey)
	{
		error = sPSActionDescriptor->GetBoolean(result, runtimeKeyID, &limitedValue);
		if (!error)
		{
			hostIsElements = limitedValue != 0;
		}
	}
	else
	{
		error = sPSActionDescriptor->HasKey(result, keyHostVersion, &hasKey);
        if (error) goto returnError;

		if (hasKey)
		{
			error = sPSActionDescriptor->GetObject(result, keyHostVersion, &classID, &versionDesc);
            if (error) goto returnError;

			error = sPSActionDescriptor->HasKey(versionDesc, keyVersionMajor, &hasKey);
            if (error) goto returnError;

			if (hasKey)
			{
				error = sPSActionDescriptor->GetInteger(versionDesc, keyVersionMajor, &versionNum);
				if (error) goto returnError;
				
				hostIsElements = ((versionNum == 1) || (versionNum == 2));
			}
		}
	}

returnError:
	
     if (result != NULL) sPSActionDescriptor->Free(result);
     if (versionDesc != NULL) sPSActionDescriptor->Free(versionDesc);
     if (reference != NULL) sPSActionReference->Free(reference);

	return hostIsElements;
}
#if Macintosh
void DoAbout(SPPluginRef plugin, int dialogID)
{
}
#endif
// end of Getter.cpp
