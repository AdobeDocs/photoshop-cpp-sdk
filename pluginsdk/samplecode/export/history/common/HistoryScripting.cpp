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
//		HistoryScripting.cpp
//
//	Copyright 1990-1992, Thomas Knoll.
//	All Rights Reserved.
//
//	Description:
//		This file contains the scripting routines
//		for the Export module History, a module that
//		displays and lets the user manipulate
//		pseudo-resources of type 'hist'.
//
//	Use:
//		This module shows how to examine, display, and work
//		with pseudo-resources.  An additional element might
//		be to have it export those remaining pseudo-resources
//		to a text file.
//
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
//	Includes
//-------------------------------------------------------------------------------

#include "History.h"

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
	PIReadDescriptor	token = NULL;	// token we'll use to read descriptor
	DescriptorKeyID		key = NULLID;	// the next key
	DescriptorTypeID	type = NULLID;	// the type of the key we read
	int32				flags = 0;		// any flags for the key
	
	// Populate this array if we're expecting any keys,
	// must be NULLID terminated:
	DescriptorKeyIDArray array = { NULLID };
	
	// While we're reading keys, errors will stick around and be reported
	// when we close the token:
	OSErr				err = noErr;
	
	// Assume we want to pop our dialog unless explicitly told not to:
	Boolean				returnValue = true;
	
	if (DescriptorAvailable(NULL))
	{ // descriptor suite is available, go ahead and open descriptor:
	
		// PIUtilities routine to open descriptor handed to us by host:
		token = OpenReader(array);
		
		if (token)
		{ // token was valid, so read keys from it:
			
			while (PIGetKey(token, &key, &type, &flags))
			{ // got a valid key.  Figure out where to put it:
			
				switch (key)
				{ // match a key to its expected type:case keyAmount:
					case 0:
						break;

					default:
						break;
					// Read scripting keys and values here.
					// See PIActions.h and PIUtilities.h for
					// routines and macros for scripting functions.
				
				} // key
				
			} // PIGetKey

			// PIUtilities routine that automatically deallocates,
			// closes, and sets token to NULL:
			err = CloseReader(&token);
				
			if (err)
			{ // an error did occur while we were reading keys:
			
				if (err == errMissingParameter) // missedParamErr == -1715
				{ // missing parameter somewhere.  Walk IDarray to find which one.
				}
				else
				{ // serious error.  Return it as a global result:
					gResult = err;
				}
					
			} // stickyError
						
		} // didn't have a valid token
		
		// Whether we had a valid token or not, we were given information
		// as to whether to pop our dialog or not.  PIUtilities has a routine
		// to check that and return TRUE if we should pop it, FALSE if not:	
		returnValue = PlayDialog();
	
	} // descriptor suite unavailable
	
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
	PIWriteDescriptor	token = NULL;	// token to write our parameters to
	OSErr				err = noErr; // we'll return any error with this
			
	if (DescriptorAvailable(NULL))
	{ // Descriptor suite is available.  Open a token:
	
		// PIUtilities has a routine that will create a descriptor to write
		// parameters to:
		token = OpenWriter();
		
		if (token)
		{ // was able to create the token, write our keys:
			
			// Write scripting keys and values here.
			// See PIActions.h and PIUtilities.h for
			// routines and macros for scripting functions.

			// PIUtilities has a routine that will close the token, collapse
			// it, and store it in the struct that the host will then grab
			// and store for recording and/or subsequent scripting.  It tells
			// the host that the dialog is optional.  You can override this
			// by setting recordInfo to whatever you need.  It then
			// deallocates token and sets it to NULL:
			err = CloseWriter(&token);
			
		} // wasn't able to create token
	
	} // descriptor suite unavailable
	
	return err;
	
} // end WriteScriptParams

//-------------------------------------------------------------------------------

// end HistoryScripting.cpp
