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
//		SelectoramaScripting.cpp
//
//	Description:
//		This file contains the scripting source code and functions
//		for the Selection module Selectorama, an example module
//		that just returns a pixel selection.
//
//	Use:
//		Use selection modules to return pixel or path selections on a new
//		layer or the current layer.
//
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
//	Includes
//-------------------------------------------------------------------------------

#include "PITerminology.h"
#undef keyLength // mac header collision and we don't need it
#include "Selectorama.h"

/*****************************************************************************/

/* Checks the parameters against scripting-returned parameters, if any, and
   updates our parameters to match ones given to us by the scripting system. */

Boolean ReadScriptParams (GPtr globals)
{
	PIReadDescriptor			token = NULL;
	DescriptorKeyID				key = NULLID;
	DescriptorTypeID			type = NULLID;
	double						percent = 0;
	const double				minValue = kPercentMin, maxValue = kPercentMax;
	DescriptorUnitID			percentUnitPass = unitPercent;
	OSType						x = typeNull;
	DescriptorKeyIDArray		array = { keyMyShape, keyMyChannels, keyMyCreate, NULLID };
	int32						flags = 0;
	OSErr						stickyError = noErr;
	Boolean						returnValue = true;
	
	if (DescriptorAvailable(NULL))
	{ /* playing back.  Do our thing. */
		token = OpenReader(array);
		if (token)
		{
			while (PIGetKey(token, &key, &type, &flags))
			{
				switch (key)
				{
					case keyMyArea:
						PIGetEnum(token, &x);
						gWhatArea = KeyToEnum(x, typeMySelect);
						break;
					case keyMyAmount: // optional
						PIGetPinUnitFloat(token, &minValue, &maxValue, &percentUnitPass, &percent);
						gPercent = (short)percent;
						break;
					case keyMyChannels:
						PIGetEnum(token, &x);
						gWhatChannels = KeyToEnum(x, typeMyComposite);
						break;
					case keyMyCreate:
						PIGetEnum(token, &x);
						gCreate = KeyToEnum(x, typeMyCreate);
						break;
				}
			}

			stickyError = CloseReader(&token); // closes & disposes.
				
			if (stickyError)
			{
				if (stickyError == errMissingParameter) // missedParamErr == -1715
					;
					/* (descriptorKeyIDArray != NULL)
					   missing parameter somewhere.  Walk IDarray to find which one. */
				else
					gResult = stickyError;
			}
		}		
		returnValue = PlayDialog();
		/* return TRUE if want to show our Dialog */
	}
	return returnValue;
}
		
/*****************************************************************************/

/* Writes our parameters to the scripting system. */

OSErr WriteScriptParams (GPtr globals)
{
	PIWriteDescriptor			token = NULL;
	double						percent = gPercent;
	OSErr						gotErr = noErr;
			
	if (DescriptorAvailable(NULL))
	{ /* recording.  Do our thing. */
		token = OpenWriter();
		if (token)
		{
			PIPutEnum(token,
					  keyMyArea,
					  typeMySelect,
					  EnumToKey(gWhatArea, typeMySelect));
			if (gWhatArea == iSelectRandom)
				PIPutUnitFloat(token, keyMyAmount, unitPercent, &percent);
			PIPutEnum(token,
					  keyMyChannels,
					  typeMyComposite,
					  EnumToKey(gWhatChannels, typeMyComposite));
			PIPutEnum(token,
					  keyMyCreate,
					  typeMyCreate,
					  EnumToKey(gCreate, typeMyCreate));
			gotErr = CloseWriter(&token); /* closes and sets to dialog optional */
		/* done.  Now pass handle on to Photoshop */
		}
	}
	return gotErr;
}

/*****************************************************************************/
/* Takes the property units and returns a corresponding enumerated number */

short KeyToEnum (OSType key, OSType whatType)
{
	short retn = 0;
	
	switch (whatType)
	{
		case typeMySelect:
			switch (key)
			{
				case selectMin:
					retn = iSelectMin;
					break;
				case selectMax:
					retn = iSelectMax;
					break;
				case selectRandom:
					retn = iSelectRandom;
					break;
			}
			break;
		case typeMyComposite:
			switch (key)
			{
				case useTarget:
					retn = iUseTarget;
					break;
				case useMerged:
					retn = iUseMerged;
					break;
			}
			break;
		case typeMyCreate:
			switch (key)
			{
				case createSelection:
					retn = iCreateSelection;
					break;
				case createMaskpath:
					retn = iCreateMaskpath;
					break;
				case createLayer:
					retn = iCreateLayer;
					break;
			}
			break;
		case typeMyPISel:
			switch (key)
			{
				case createSelection:
					retn = piSelMakeMask;
					break;
				case createMaskpath:
					retn = piSelMakeWorkPath;
					break;
				case createLayer:
					retn = piSelMakeLayer;
					break;
			}
			break;
	}
	return retn;
}

/*****************************************************************************/
/* Takes the property enumeration and returns the corresponding key */

OSType EnumToKey (short keyEnum, OSType whatType)

{
	OSType retn = 0;
	
	switch (whatType)
	{
		case typeMySelect:
			switch (keyEnum)
			{
				case iSelectMin:
					retn = selectMin;
					break;
				case iSelectMax:
					retn = selectMax;
					break;
				case iSelectRandom:
					retn = selectRandom;
					break;
			}
			break;
		case typeMyComposite:
			switch (keyEnum)
			{
				case iUseTarget:
					retn = useTarget;
					break;
				case iUseMerged:
					retn = useMerged;
					break;
			}
			break;
		case typeMyCreate:
			switch (keyEnum)
			{
				case iCreateSelection:
					retn = createSelection;
					break;
				case iCreateMaskpath:
					retn = createMaskpath;
					break;
				case iCreateLayer:
					retn = createLayer;
					break;
			}
			break;
	}
	
	return retn;
}
// end SelectoramaScripting.cpp