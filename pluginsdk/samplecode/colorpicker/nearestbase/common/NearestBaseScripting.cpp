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
//		NearestBaseScripting.cpp
//
//	Description:
//		This file contains the scripting source and functions
//		for the Color Picker example module NearestBase.
//
//	Use:
//		NearestBase provides another color picker (available
//		from File: Preferences: General) that cycles through
//		four primary colors.  Once selected, clicking any
//		location that normally pops a picker (such as the
//		foreground or background color) will automatically
//		cycle to the next primary color.
//
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
//	Includes
//-------------------------------------------------------------------------------

#include "NearestBase.h"

//-------------------------------------------------------------------------------
//
//	ReadScriptParams
//
//	Checks the parameters against scripting-returned parameters, if any, and
//	updates the globals to match ones provided by the scripting system.
//
//	Inputs:
//		GPtr globals		Pointer to global structure
//
//	Outputs:
//		returns TRUE		If you should pop your dialog
//		returns FALSE		If you should not pop your dialog
//
//		gResult				Will return any fatal error
//
//-------------------------------------------------------------------------------

Boolean ReadScriptParams (GPtr globals)
{
	PIReadDescriptor	token = NULL;	// token we'll use to read descriptor
	DescriptorKeyID		key = NULLID;	// the next key
	DescriptorTypeID	type = NULLID;	// the type of the key we read
	int32				flags = 0;		// any flags for the key
	
	// can populate this array if we're expecting any keys (we're not, so its NULL):
	DescriptorKeyIDArray	array = { NULLID };
	
	// while we're reading keys, errors will stick around and be reported
	// when we close the token:
	OSErr				stickyError = noErr;
	
	// assume we want to pop our dialog unless explicitly told not to:
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
				{ // match a key to its expected type, read it,
					case 0:
						break;
					default:
						break;
				} // key
				
			} // PIGetKey

			// PIUtilities routine that automatically deallocates,
			// closes, and sets token to NULL:
			stickyError = CloseReader(&token);
				
			if (stickyError)
			{ // an error did occur while we were reading keys:
			
				if (stickyError == errMissingParameter) // missedParamErr == -1715
				{ // missing parameter somewhere.  Walk IDarray to find which one.
				
				} else // serious error.  Return it as a global result:
					gResult = stickyError;
					
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
//		GPtr globals		Pointer to global structure
//
//	Outputs:
//		returns an OSErr	If there was a serious error
//		returns noErr		If everything worked fine.
//
//-------------------------------------------------------------------------------

OSErr WriteScriptParams (GPtr globals)
{
	PIWriteDescriptor	token = NULL;	// token to write our parameters to
	OSErr				gotErr = noErr; // we'll return any error with this
			
	if (DescriptorAvailable(NULL))
	{ // Descriptor suite is available.  Open a token:
	
		// PIUtilities has a routine that will create a descriptor to write
		// parameters to:
		token = OpenWriter();
		
		if (token)
		{ // was able to create the token, write our keys:

			// no keys to write
			
			// PIUtilities has a routine that will close the token, collapse
			// it, and store it in the struct that the host will then grab
			// and store for recording and/or subsequent scripting.  It tells
			// the host that the dialog is optional.  You can override this
			// by setting recordInfo to whatever you need.  It then
			// deallocates token and sets it to NULL:
			gotErr = CloseWriter(&token);
			
		} // wasn't able to create token
	
	} // descriptor suite unavailable
	
	return gotErr;
	
} // end WriteScriptParams

//-------------------------------------------------------------------------------

// end NearestBaseScripting.cpp
