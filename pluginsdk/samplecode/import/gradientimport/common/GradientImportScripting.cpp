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
//		GradientImport.cpp
//
//	Copyright 1990, Thomas Knoll.
//	All Rights Reserved.
//
//	Description:
//		This file contains the source and functions for the
//		Import module GradientImport, a scriptable multiple-acquire
//		plug-in that creates 4 types of gradients in batches.
//
//	Use:
//		This import module is a great example of scripting
//		Import modules that use the old style Photoshop
//		multiple-acquire mechanism.
//
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
//	Includes
//-------------------------------------------------------------------------------

#include "GradientImport.h"
#include "PITerminology.h"

/*****************************************************************************/
/* Opens up the scripting parameters to begin reading commands. */

void OpenScriptParams (GPtr globals)
{
	DescriptorKeyID				key = 0;
	DescriptorTypeID			type = 0;
	int32						flags = 0;
	Boolean						leaveEarly = false;
	
	if (DescriptorAvailable(NULL))
	{ 
		gToken = OpenReader(NULL); 
		/* don't know how many we're going to be passed, so don't bother with array */
		if (gToken)
		{
			while (!leaveEarly)
			{
				leaveEarly = PIGetKey(gToken, &key, &type, &flags);
				switch (key)
				{
					case keyMultiImportCount:
						PIGetCount(gToken, &(gCount));
						leaveEarly = true;
						break;
					/* We're going to ignore all other keys.  What we're looking for is our
					  list, which will be preceded by our count key. Once we find it, we'll drop
					   out, to eventually call our Read routine. */
				}
			}
		}
	gQueryForParameters = PlayDialog();
	}
	/* return true if want to show our Dialog */
}
		
/*****************************************************************************/

/* Reads the next batch of values and sets the global parameters. */

void ReadScriptParams (GPtr globals)
{
	DescriptorTypeID			passType = classMultiImportStruct;
	DescriptorKeyIDArray		subKeyIDArray = 
								{ keyRows, keyColumns, 
								  keyOurMode, NULLID }; /* keyInvert */
	PIDescriptorHandle			subHandle = NULL;
	PIReadDescriptor			subToken = NULL;
	OSErr						stickyError = noErr;
	
	if (DescriptorAvailable(NULL))
	{
		if (gToken)
		{
			if (gCount > 0)
			{
				gLastInvert = false; // if we've got a valid descriptor, assume NO invert
				PIGetObj(gToken, &passType, &subHandle);
				subToken = OpenReadDesc(subHandle, subKeyIDArray);
				if (subToken)
				{
					SwitchScriptInfo (globals, subToken);
					stickyError = CloseReadDesc(subToken); // done.
					subToken = NULL; // just in case
					PIDisposeHandle(subHandle); // dispose.
					subHandle = NULL; // just in case.
		
					if (stickyError)
					{
						if (stickyError == errMissingParameter) // -1715
							; /* missing parameter.  Walk keyIDArray to find which one. */
						else
							gResult = stickyError; // real error occured
					}
					gContinueImport = true; // we got something, so keep going!
				}
				gCount--;
			}
			if (gCount < 1)						
				CloseScriptParams(globals); // close em up!
		} /* no readToken */
	} /* not correct descriptorParameters version */
}

/*****************************************************************************/

/* Checks the parameters against scripting-returned parameters, if any, and
   updates our parameters to match ones given to us by the scripting system. */

void CloseScriptParams (GPtr globals)
{
	OSErr			stickyError = noErr;

	if (DescriptorAvailable(NULL))
	{	
		if (gToken)
		{
			stickyError = CloseReader(&gToken); // done.

			if (stickyError)
			{
				if (stickyError == errMissingParameter)
					; /* missing parameter.  Check our parameters and coerce if need be. */
				else
					gResult = stickyError; // real error occured
			}
		}
	}
	gCount = 0; /* reset this puppy */
	gContinueImport = false;
}

		
/*****************************************************************************/
/* Updates globals from scripting parameters                                 */

void SwitchScriptInfo (GPtr globals, PIReadDescriptor token)
{
	double						rows = kRowsMin, columns = kColumnsMin;
	PIType						mode = ourRGBColorMode;
	const double				minRows = kRowsMin, maxRows = kRowsMax,
								minColumns = kColumnsMin, maxColumns = kColumnsMax;
	DescriptorUnitID				pixelsUnitPass = unitPixels;
	DescriptorKeyID				key = 0;
	DescriptorTypeID			type = 0;
	int32						flags = 0;
	Boolean						invert = false;
	Boolean						convert = false;
	
	while (PIGetKey(token, &key, &type, &flags))
	{
		switch (key)
		{
			case keyRows:
				PIGetPinUnitFloat(token, &minRows, &maxRows, &pixelsUnitPass, &rows);
				gLastRows = (short)rows;
				break;
			case keyColumns:
				PIGetPinUnitFloat(token, &minColumns, &maxColumns, &pixelsUnitPass, &columns);
				gLastCols = (short)columns;
				break;
			case keyOurMode:
				PIGetEnum(token, &mode);
				gLastMode = GetPlugInMode(mode);
				break;
			case keyInvert:
				PIGetBool(token, &invert);
				gLastInvert = invert;
				break;
			case keyOurAlphaTransparency:
				PIGetBool(token, &convert);
				gConvertAlpha = convert;
				break;
		}
	}
}

/*****************************************************************************/

/* Checks for any data then writes our parameters to the scripting system. */

OSErr CheckAndWriteScriptParams (GPtr globals)
{
	OSErr	gotErr = noErr;
	
	if (gLastImages) 
		gotErr = WriteScriptParams(globals);
	
	return gotErr;
}

/*****************************************************************************/

/* Writes our parameters to the scripting system. */

OSErr WriteScriptParams (GPtr globals)
{
	unsigned32					count = gLastImages;
	PIWriteDescriptor			token = NULL;
	OSErr						stickyError = noErr;
			
	if (DescriptorAvailable(NULL))
	{
		token = OpenWriter();
		if (token)
		{
			PIPutCount(token, keyMultiImportCount, count);
			for (count = 0; count < (unsigned32)gLastImages; count++)
			{
				PIPutObj(token, keyMultiImportInfo, classMultiImportStruct, &gArray [count]);
				/* routine disposes handle when its done with it */
			}
			
			gLastImages = 0; // reset
			stickyError = CloseWriter(&token); // returns descriptor and sets plugInDialogOptional
		}
		
	}
	return stickyError;
}

/*****************************************************************************/
/* Returns a descriptor from our parameters                                  */

void CreateDescriptor (GPtr globals)
{
	PIType						mode = GetGradientMode(gLastMode);
	const double				rows = gLastRows, columns = gLastCols;
	Boolean						invert = gLastInvert;
	Boolean						convert = gConvertAlpha;
	PIWriteDescriptor			token = NULL;
	PIDescriptorHandle			h;
	OSErr						stickyError = noErr;
			
	if (DescriptorAvailable(NULL))
	{
		token = OpenWriter();
		if (token)
		{
			PIPutUnitFloat(token, keyRows, unitPixels, &rows);
			PIPutUnitFloat(token, keyColumns, unitPixels, &columns);
			PIPutEnum(token, keyOurMode, typeGradientMode, mode);
			if (invert) 
				PIPutBool(token, keyInvert, invert);
			if (convert) 
				PIPutBool(token, keyOurAlphaTransparency, convert);
						
			stickyError = CloseWriteDesc(token, &h);
			token = NULL;
					
			if (!stickyError)
			{
				if (gLastImages >= kMaxDescriptors)
				{
					gLastImages--; // just keep replacing last one
					PIDisposeHandle(gArray[gLastImages]); // dispose last handle
				}
				
				gArray [gLastImages++] = h;
			}
		}
	}
}

/*****************************************************************************/
/* Takes the plug-in mode and returns the GradientMode						 */

PIType GetGradientMode (short plugInMode)

{
	switch (plugInMode)
	{
		case plugInModeBitmap:
			return ourBitmapMode;
			break;
		case plugInModeGrayScale:
			return ourGrayscaleMode;
			break;
		case plugInModeIndexedColor:
			return ourIndexedColorMode;
			break;
		case plugInModeRGBColor:
		default:
			return ourRGBColorMode;
			break;
	}
}

/*****************************************************************************/
/* Takes the GradientMode and returns the plug-in mode		  			     */

short GetPlugInMode (PIType gradientMode)

{
	switch (gradientMode)
	{
		case ourBitmapMode:
			return plugInModeBitmap;
			break;
		case ourGrayscaleMode:
			return plugInModeGrayScale;
			break;
		case ourIndexedColorMode:
			return plugInModeIndexedColor;
			break;
		case ourRGBColorMode:
		default:
			return plugInModeRGBColor;
			break;
	}
}

/*****************************************************************************/
// end GradientImportScripting.cpp