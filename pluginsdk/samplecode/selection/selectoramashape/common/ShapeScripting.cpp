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
//		ShapeScripting.cpp
//
//	Description:
//		This file contains the source and functions for the
//		scripting routines for the Import module Shape,
//		which returns a path, layer, or selection with an
//		interesting shape.
//
//	Use:
//		This module specifically works the path return
//		functionality of Photoshop.  The Paths are stored
//		by derezing a work path saved in Photoshop then
//		storing them in a "Path" resource, which is a
//		binary data resource of a "Path" as defined in the
//		"Path layout" section of the Photosop File Format.
//
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
//	Includes
//-------------------------------------------------------------------------------

#include "Shape.h"
#include "PITerminology.h"

//-------------------------------------------------------------------------------
//
//	ReadScriptParamsShape
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

Boolean ReadScriptParamsShape (GPtr globals)
{
	PIReadDescriptor	token = NULL;	// token we'll use to read descriptor
	DescriptorKeyID		key = NULLID;	// the next key
	DescriptorTypeID	type = NULLID;	// the type of the key we read
	int32				flags = 0;		// any flags for the key
	
	// Populate this array if we're expecting any keys,
	// must be NULLID terminated:
	DescriptorKeyIDArray	array = { keyMyShape, keyMyCreate, NULLID };
	
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

			DescriptorEnumID currEnum = typeNull;
		
			while (PIGetKey(token, &key, &type, &flags) && err == 0)
			{ // got a valid key.  Figure out where to put it:
			
				switch (key)
				{ // match a key to its expected type, read it,
					case keyMyShape:
						err = PIGetEnum(token, &currEnum);
						gWhatShape = KeyToEnumShape(currEnum, typeMyShape);
						break;
					case keyMyCreate:
						err = PIGetEnum(token, &currEnum);
						gCreate = KeyToEnumShape(currEnum, typeMyCreate);
						break;
				} // key
				
			} // PIGetKey

			// PIUtilities routine that automatically deallocates,
			// closes, and sets token to NULL:
            if (err == 0)
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
	
} // end ReadScriptParamsShape
		
//-------------------------------------------------------------------------------
//
//	WriteScriptParamsShape
//
//	Takes our parameters from our global variables and writes them out to the
//	scripting system, which hands it all back to the host.
//
//	Inputs:
//		GPtr globals			Pointer to global structure.
//
//		double gIdleAmount		Seconds to idle.
//
//		Boolean gUseProgress	TRUE = Use progress indicator (Default).
//								FALSE = Suppress progress.
//
//		Boolean gSendUpdates	TRUE = Send update events to application (Default).
//								FALSE = Just idle, don't send events.
//
//	Outputs:
//		returns an OSErr		If there was a serious error.
//		returns noErr			If everything worked fine.
//
//-------------------------------------------------------------------------------

OSErr WriteScriptParamsShape (GPtr globals)
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

			PIPutEnum(token,
					  keyMyShape, 
					  typeMyShape, 
					  EnumToKeyShape(gWhatShape, typeMyShape));
			PIPutEnum(token,
					  keyMyCreate,
					  typeMyCreate, 
					  EnumToKeyShape(gCreate, typeMyCreate));
			
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
	
} // end WriteScriptParamsShape

/*****************************************************************************/
/* Takes the property units and returns a corresponding enumerated number */

short KeyToEnumShape (OSType key, OSType whatType)

{
	short outValue = 0;
	
	switch (whatType)
	{
		case typeMyShape:
			switch (key)
			{
				case shapeTriangle:
					outValue = iShapeTriangle;
					break;
				case shapeSquare:
					outValue = iShapeSquare;
					break;
				case shapeCircle:
					outValue = iShapeCircle;
					break;
				case shapeStar:
					outValue = iShapeStar;
					break;
				case shapeTreble:
					outValue = iShapeTreble;
					break;
				case shapeRibbon:
					outValue = iShapeRibbon;
					break;
				case shapeNote:
					outValue = iShapeNote;
					break;
			}
			break;
		case typeMyCreate:
			switch (key)
			{
				case createSelection:
					outValue = iCreateSelection;
					break;
				case createMaskpath:
					outValue = iCreateMaskpath;
					break;
				case createLayer:
					outValue = iCreateLayer;
					break;
			}
			break;
		case typeMyPISel:
			switch (key)
			{
				case createSelection:
					outValue = piSelMakeMask;
					break;
				case createMaskpath:
					outValue = piSelMakeWorkPath;
					break;
				case createLayer:
					outValue = piSelMakeLayer;
					break;
			}
			break;
	}
	return outValue;
}

/*****************************************************************************/
/* Takes the property enumeration and returns the corresponding key */

OSType EnumToKeyShape (short keyEnum, OSType whatType)

{
	OSType outValue = typeNull;

	switch (whatType)
	{
		case typeMyShape:
			switch (keyEnum)
			{
				case iShapeTriangle:
					outValue = shapeTriangle;
					break;
				case iShapeSquare:
					outValue = shapeSquare;
					break;
				case iShapeCircle:
					outValue = shapeCircle;
					break;
				case iShapeStar:
					outValue = shapeStar;
					break;
				case iShapeTreble:
					outValue = shapeTreble;
					break;
				case iShapeRibbon:
					outValue = shapeRibbon;
					break;
				case iShapeNote:
					outValue = shapeNote;
					break;
			}
			break;
		case typeMyCreate:
			switch (keyEnum)
			{
				case iCreateSelection:
					outValue = createSelection;
					break;
				case iCreateMaskpath:
					outValue = createMaskpath;
					break;
				case iCreateLayer:
					outValue = createLayer;
					break;
			}
			break;
	}
	return outValue;
}

//-------------------------------------------------------------------------------

// end ShapeScripting.cpp

