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
//		ColorMungerScripting.c
//
//	Description:
//		This file contains the scripting source
//		for the Filter module ColorMunger, a module
//		showing the use of the Color Services suite.
//
//	Use:
//		This module takes a source color of any color space
//		and converts it to a target color in any color
//		space.  It shows how to convert colors as well as
//		pop the color picker.  It appears in
//		Filters>>Utilities>>ColorMunger.
//
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
//	Includes 
//-------------------------------------------------------------------------------

#include "ColorMunger.h"
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
//		returns TRUE		If you should pop your dialog.
//		returns FALSE		If you should not pop your dialog.
//
//		gResult				Will return any fatal error.
//
//-------------------------------------------------------------------------------

Boolean ReadScriptParams (GPtr globals)
{
	// Assume we want to pop our dialog unless explicitly told not to:
	Boolean returnValue = true;
	
	if (DescriptorAvailable(NULL))
	{ // descriptor suite is available, go ahead and open descriptor:
	
		// Populate this array if we're expecting any keys,
		// must be NULLID terminated:
		DescriptorKeyIDArray array =
		{
			keyColor,
			keyColorize,
			keyForegroundColor,
			NULLID
		};
		
		// PIUtilities routine to open descriptor handed to us by host:
		PIReadDescriptor token = OpenReader(array);
		
		if (token)
		{ // token was valid, so read keys from it:
			DescriptorKeyID		key = NULLID;	// the next key
			DescriptorTypeID	type = NULLID;	// the type of the key we read
			int32				flags = 0;		// any flags for the key

			// While we're reading keys, errors will stick around and be reported
			// when we close the token:
			OSErr				err = noErr;
			
			Handle 				h = NULL; 		// need for PIGetObj
			
			while (PIGetKey(token, &key, &type, &flags) && err == noErr)
			{ // got a valid key.  Figure out where to put it:
			
				switch (key)
				{ // match a key to its expected type:case keyAmount:
					case keyColor:
						err = PIGetEnum(token, &type);
						gColor = Type2Enum(classColor, type);
						break;
					case keyColorize:
						err = PIGetEnum(token, &type);
						gColorize = Type2Enum(classColor, type);
						break;
					case keyForegroundColor:
						err = PIGetObj(token, NULL, &h); // we've already been handed the type @ GetKey
						if (h != NULL)
						{
							DescriptorKeyIDArray subarray =
							{
								keyCyan,
								keyMagenta,
								keyYellow,
								keyBlack,
								NULLID
							};
							
							PIReadDescriptor subtoken = NULL;
							 
							switch (type)
							{
								case classRGBColor:
									AssignArray(&subarray, keyRed, keyGreen, keyBlue, NULLID);
									break;
								case classGrayscale:
									AssignArray(&subarray, keyGray, NULLID, NULLID, NULLID);
									break;
								case classHSBColor:
									AssignArray(&subarray, keyHue, keySaturation, keyBrightness, NULLID);
									break;
								case classLabColor:
									AssignArray(&subarray, keyLuminance, keyA, keyB, NULLID);
									break;
								case classUnspecifiedColor: // use for all other colors
								case classCMYKColor:
								default:
									AssignArray(&subarray, keyCyan, keyMagenta, keyYellow, keyBlack);
									break;
							}
							
							subtoken = OpenReadDesc(h, subarray);
							if (subtoken)
							{ // we're about to get a color.
								
								int32 x = 0;
								
								CSSetColor(gSourceColor, 0, 0, 0, 0);
								while (PIGetKey(subtoken, &key, &type, &flags) && err == noErr)
								{
									switch (key)
									{
										case keyRed:
										case keyCyan:
										case keyGray:
										case keyHue:
										case keyLuminance:
											err = PIGetInt(subtoken, &x);
											gSourceColor[0] = (int16)x;
											break;
										case keyGreen:
										case keyMagenta:
										case keySaturation:
										case keyA:
											err = PIGetInt(subtoken, &x);
											gSourceColor[1] = (int16)x;
											break;
										case keyBlue:
										case keyYellow:
										case keyBrightness:
										case keyB:
											PIGetInt(subtoken, &x);
											gSourceColor[2] = (int16)x;
											break;
										case keyBlack:
											PIGetInt(subtoken, &x);
											gSourceColor[3] = (int16)x;
											break;
									}
								}
								
                                if (err == noErr)
                                    err = CloseReadDesc(subtoken);
								subtoken = NULL; // just in case
								
								PIDisposeHandle(h);
								h = NULL;
							
								if (err)
								{
									if (err == errMissingParameter)
										{ // missing parameter somewhere.  Walk IDarray to find which one.
										}
									else
									{ // serious error.  Return as global result.
										gResult = err; // real error occurred
									}
									
								} // stickyError
							
							} // subtoken
						} // h == NULL
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
	
	return returnValue;
	
} // end ReadScriptParams

//-------------------------------------------------------------------------------
//	Constants for WriteScriptParams
//-------------------------------------------------------------------------------

enum
{
	mRGB = 0,
	mHSB,
	mCMYK,
	mLab,
	mGray,
	mHSL,
	mXYZ
};

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
//		short gColor				Color space for source.
//
//		short gColorize				Color space for target.
//
//		short gSourceColor[4]		Color data for source.
//
//	Outputs:
//		returns an OSErr		If there was a serious error.
//		returns noErr			If everything worked fine.
//
//-------------------------------------------------------------------------------

OSErr WriteScriptParams (GPtr globals)
{
	OSErr						err = noErr;
	
	if (DescriptorAvailable(NULL))
	{ // Descriptor suite is available.  Open a token:
	
		// PIUtilities has a routine that will create a descriptor to write
		// parameters to:
		PIWriteDescriptor token = OpenWriter();

		if (token)
		{ // was able to create the token, write our keys:
			PIWriteDescriptor subtoken = OpenWriter();
			if (subtoken)
			{
				DescriptorKeyIDArray array =
				{
					keyCyan, 
					keyMagenta, 
					keyYellow, 
					keyBlack, 
					NULLID
				};
				
				// Handle will receive new object:
				PIDescriptorHandle	h = NULL;

				DescriptorClassID useClass = classUnspecifiedColor; // default class
				
				uint8 loop = 0;
				
				switch (gColor)
				{
					case mRGB:
						useClass = classRGBColor;
						AssignArray(&array, keyRed, keyGreen, keyBlue, NULLID);
						break;
					case mHSB:
						useClass = classHSBColor;
						AssignArray(&array, keyHue, keySaturation, keyBrightness, NULLID); 
						break;
					case mGray:
						useClass = classGrayscale;
						AssignArray(&array, keyGray, NULLID, NULLID, NULLID);
						break;
					case mLab:
						useClass = classLabColor;
						AssignArray(&array, keyLuminance, keyA, keyB, NULLID);
						break;
					case mCMYK:
						useClass = classCMYKColor;
						AssignArray(&array, keyCyan, keyMagenta, keyYellow, keyBlack);
						break;
					default:
						useClass = classUnspecifiedColor;
						AssignArray(&array, keyCyan, keyMagenta, keyYellow, keyBlack);
						break;
				}
				
				// now actually store values
				for (loop = 0; loop < 4 && !err; loop++)
				{
					if (array[loop] != NULLID)
						err = PIPutInt(subtoken, array[loop], gSourceColor[loop]);
				}
				
                if (!err)
                    err = CloseWriteDesc(subtoken, &h);
				subtoken = NULL;

				if (err == noErr && h != NULL)
				{
					err = PIPutObj(token, keyForegroundColor, useClass, &h);
					// routine disposes handle for us
				}

			} // subtoken

            if (!err)
                err = PIPutEnum(token, keyColor, typeColorName, Enum2Type(classColor, gColor));
            if (!err)
                err = PIPutEnum(token, keyColorize, typeColorName, Enum2Type(classColor, gColorize));
			
			// PIUtilities has a routine that will close the token, collapse
			// it, and store it in the struct that the host will then grab
			// and store for recording and/or subsequent scripting.  It tells
			// the host that the dialog is optional.  You can override this
			// by setting recordInfo to whatever you need.  It then
			// deallocates token and sets it to NULL:
            if (!err)
                err = CloseWriter(&token); // closes and sets to dialogOptional
		
		} // wasn't able to create token

	} // descriptor suite unavailable

	return err;
	
} // end WriteScriptParams


//-------------------------------------------------------------------------------
//	Constants for Enum2Type and Type2Enum
//-------------------------------------------------------------------------------

const DescriptorEnumID cnArray[] =
{
	cnRGB,
	cnHSB,
	cnCMYK,
	cnLab,
	cnGray,
	cnHSL,
	cnXYZ
};

/*****************************************************************************/
/* Takes an enumeration and returns a type									 */

DescriptorKeyID Enum2Type (DescriptorClassID thisClass, short thisEnum)
{
	// const DescriptorEnumID cnArray[] = { cnArrayValues };

	DescriptorKeyID retn = typeNull;
	
	switch (thisClass)
	{
		case classColor:
			retn = cnArray[thisEnum];
			break;

	}

	return retn;

} // end Enum2Type

/*****************************************************************************/
/* Takes the GradientMode and returns the plug-in mode		  			     */

int16 Type2Enum (DescriptorClassID thisClass, DescriptorKeyID thisType)
{
	int16 loop;
	int16 retn = 0;
	
	// const DescriptorEnumID cnArray[] = { cnArrayValues };

	switch (thisClass)
	{
		case classColor:
			for (loop = 0; loop < (sizeof(cnArray) / sizeof(PIType)); loop++)
				if (cnArray[loop] == thisType)
					retn = loop;
			break;
	}

	return retn;
	
} // end Type2Enum

/*****************************************************************************/
// Assigns a set of up to 4 values (and null) to an array

void AssignArray (void *array,
					DescriptorKeyID key1, 
					DescriptorKeyID key2,
					DescriptorKeyID key3,
					DescriptorKeyID key4 )
{
	DescriptorKeyID		*p = (DescriptorKeyID *)array;
	
	p[0] = key1;
	p[1] = key2;
	p[2] = key3;
	p[3] = key4;
	p[4] = NULLID;
}

//-------------------------------------------------------------------------------

// end ColorMungerScripting.cpp
