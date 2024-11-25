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
//		PoorMansTypeToolScripting.cpp
//
//	Description:
//		This file contains the source and routines for the
//		Filter module Poor Man's Type Tool, a module that 
//		uses the Channel Ports Suite for pixel munging.
//
//	Use:
//		This is a basic module to exemplify all the typical
//		functions a filter module will do: Read scripting
//		parameters, show a user interface, display a proxy,
//		write changed pixel information, and then write
//		scripting parameters.
//
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
//	Includes
//-------------------------------------------------------------------------------

#include "PoorMansTypeTool.h"
#include "PITerminology.h"

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
//		gQueryForParameters = TRUE		If you should pop your dialog.
//		gQueryForParameters = FALSE		If you should not pop your dialog.
//		gQueryForParameters = unchanged	If descriptor suite is unavailable.
//
//		returns gResult = OSErr			Will return any fatal error.
//
//-------------------------------------------------------------------------------

OSErr ReadScriptParams (GPtr globals)
{
	PIReadDescriptor	token = NULL;	// token we'll use to read descriptor
	DescriptorKeyID		key = NULLID;	// the next key
	DescriptorTypeID	type = NULLID;	// the type of the key we read
	int32				flags = 0;		// any flags for the key
	
	// Populate this array if we're expecting any keys,
	// must be NULLID terminated:
	DescriptorKeyIDArray	array = { keyHorizontal, keyVertical, keyXFactor, keyGaussianBlurData, NULLID };
	
	// While we're reading keys, errors will stick around and be reported
	// when we close the token:
	OSErr				err = noErr;
	
	// Assume we want to pop our dialog unless explicitly told not to:
	Boolean				returnValue = gQueryForParameters;
	
	if (DescriptorAvailable(NULL))
	{ // descriptor suite is available, go ahead and open descriptor:
	
		// PIUtilities routine to open descriptor handed to us by host:
		token = OpenReader(array);
		
		if (token)
		{ // token was valid, so read keys from it:
			while (PIGetKey(token, &key, &type, &flags) && err == noErr)
			{ // got a valid key.  Figure out where to put it:
			
				switch (key)
				{ // match a key to its expected type:case keyAmount:
					case keyHorizontal:
						int32 h;
						err = PIGetInt(token, &h);
						gPointH = (short)h;
						break;
					case keyVertical:
						int32 v;
						err = PIGetInt(token, &v);
						gPointV = (short)v;
						break;
					case keyXFactor:
						err = PIGetInt(token, &gXFactor);
						break;
					case keyGaussianBlurData:
						err = PIGetBool(token, &gGaussianBlurData);
						break;
					// ignore all other cases and classes
				} // key
				
			} // PIGetKey

			// PIUtilities routine that automatically deallocates,
			// closes, and sets token to NULL:
            if (err == noErr)
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
	
	gQueryForParameters = returnValue;
	
	return err;
	
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
//		short gPointH				Horizontal position of Hello World
//		short gPointV				Vertical position of Hello World
//		int32 gXFactor				Kinda like the font size
//
//		Boolean gIgnoreSelection	TRUE = Effect entire image.
//									FALSE = Effect selection only.
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
			PIPutInt(token, keyHorizontal, gPointH);
			PIPutInt(token, keyVertical, gPointV);
			PIPutInt(token, keyXFactor, gXFactor);
			PIPutBool(token, keyGaussianBlurData, gGaussianBlurData);

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

// end PoorMansTypeToolScripting.cpp
