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
//		OutboundScripting.cpp
//
//	Copyright 1990-1992, Thomas Knoll.
//	All Rights Reserved.
//
//	Description:
//		This file contains the scripting routines
//		for the Export module Outbound, a module that
//		creates a file and stores raw pixel data in it.
//
//	Use:
//		This module shows how to export raw data to a file.
//		It uses a simple "FileUtilities" library that comes
//		with the SDK.  You use it via File>>Export>>Outbound.
//
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
//	Includes
//-------------------------------------------------------------------------------

#include "Outbound.h"
#include "PITerminology.h"

#define keyInBookmark   'bkmk'

//-------------------------------------------------------------------------------
//
//	ReadScriptParams
//
//	Checks the parameters against scripting-returned parameters, if any, and
//	updates the globals to match ones provided by the scripting system.
//
//	Inputs:
//		GPtr globals		Pointer to global structure.
//
//	Outputs:
//		returns TRUE		If you should pop your dialog.
//		returns FALSE		If you should not pop your dialog.
//
//		gResult				Will return any fatal error.
//
//-------------------------------------------------------------------------------

Boolean ReadScriptParams (GPtr globals)
{
    if (gStuff == NULL ||
        gStuff->descriptorParameters == NULL ||
        gStuff->descriptorParameters->descriptor == NULL)
        return TRUE;
    
    OSErr				err = noErr;
    PIActionDescriptor  desc = NULL;

    // Assume we want to pop our dialog unless explicitly told not to:
    Boolean				returnValue = true;

    Boolean hasKey = FALSE;
	Boolean haveBookmark = FALSE;
	
    // use the modern PSActionDescriptorProcs suite instead of the old ReadDescriptorProcs
    err = sPSActionDescriptor2->HandleToDescriptor(gStuff->descriptorParameters->descriptor, &desc);
    if (err)
    {
        gResult = err;
        return TRUE;
    }
    
#if __PIMac__
    hasKey = FALSE;
    err = sPSActionDescriptor2->HasKey(desc, keyInBookmark, &hasKey);
    if ( ! err && hasKey && sPSActionDescriptor.IsAvailable())
	{
		err = sPSActionDescriptor->GetBookmark(desc, keyInBookmark, &gBookMarkData);
		if ( ! err )
		{
			haveBookmark = TRUE;
		}
	}
#endif

    // this key we must have on windows, on mac we prefer bookmark above
    hasKey = FALSE;
    err = sPSActionDescriptor2->HasKey(desc, keyIn, &hasKey);
    if ((err || ! hasKey) && ! haveBookmark)
    {
        sPSActionDescriptor2->Free(desc);
        gResult = err;
        return TRUE;
    }
	
    if (!err && hasKey)
    {
    	err = sPSActionDescriptor2->GetAlias(desc, keyIn, (Handle*)&gAliasHandle);
    	if (err)
    	{
        	sPSActionDescriptor2->Free(desc);
        	gResult = err;
        	return TRUE;
    	}
	}

    sPSActionDescriptor2->Free(desc);
    
    // Whether we had a valid token or not, we were given information
    // as to whether to pop our dialog or not.  PIUtilities has a routine
    // to check that and return TRUE if we should pop it, FALSE if not:
    returnValue = PlayDialog();

    return returnValue;
    
} // end ReadScriptParams

//-------------------------------------------------------------------------------
//
//	WriteScriptParams
//
//	Takes our parameters from our global variables and writes them out to the
//	scripting system, which hands it all back to the host.
//
//	Inputs:
//		GPtr globals				Pointer to global structure.
//
//	Outputs:
//		returns an OSErr		If there was a serious error.
//		returns noErr			If everything worked fine.
//
//-------------------------------------------------------------------------------

OSErr WriteScriptParams (GPtr globals)
{
    if (gStuff == NULL ||
        gStuff->descriptorParameters == NULL)
        return exportBadParameters;

    // we'll return any error with this
    OSErr				err = noErr;
			
    PIActionDescriptor  desc = NULL;
    
    err = sPSActionDescriptor2->Make(&desc);
    if (err)
    	return err;
    
    if (gAliasHandle)
    {
        err = sPSActionDescriptor2->PutAlias(desc, keyIn, (Handle)gAliasHandle);
        PIDisposeHandle((Handle)gAliasHandle);
        gAliasHandle = NULL;

#if __PIMac__
        err = 0; // ignore the error, alias routines might be gone in the future
#endif

	}

#if __PIMac__
	if (gBookMarkData)
	{
		err = sPSActionDescriptor->PutBookmark(desc, keyInBookmark, gBookMarkData);
		CFRelease(gBookMarkData);
		gBookMarkData = NULL;
	}
#endif

    if (err)
    {
        sPSActionDescriptor2->Free(desc);
        return err;
    }

    PIDescriptorHandle handle = NULL;
    err = sPSActionDescriptor2->AsHandle(desc, &handle);
    if (err)
        {
        sPSActionDescriptor2->Free(desc);
        return err;
        }
    
    if (gStuff->descriptorParameters->descriptor != NULL)
        HostDisposeHandle(gStuff->handleProcs, gStuff->descriptorParameters->descriptor);
    
    gStuff->descriptorParameters->descriptor = handle;
    
    // Set recordInfo to default.  Options are: plugInDialogOptional,
    // plugInDialogRequire, plugInDialogNone:
    gStuff->descriptorParameters->recordInfo = plugInDialogOptional;
    
    err = sPSActionDescriptor2->Free(desc);
  
	return err;
	
} // end WriteScriptParams

//-------------------------------------------------------------------------------

// end OutboundScripting.cpp
