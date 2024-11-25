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
//		ColorMungerUIMac.cpp
//
//	Copyright 1996-1997, Adobe Systems Incorporated.
//	All Rights Reserved.
//
//	Description:
//		This file contains the user interface source
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

#include "PIDefines.h"
#include "ColorMunger.h"

//-------------------------------------------------------------------------------
//	Prototypes.
//-------------------------------------------------------------------------------



//-------------------------------------------------------------------------------
//
//	DoAbout
//
//	Displays the About box.
//
//	Inputs:
//		AboutRecordPtr about	Is actually a pointer to some platform
//								specific data for Windows only.
//		AboutID					#define with ID of String resource to use for text
//
//	Outputs:
//		Displays About box (Dialog).  Press Escape, Enter, or Return to
//		dismiss, or click the mouse.
//
//-------------------------------------------------------------------------------
void DoAbout (AboutRecordPtr /*about*/)
{
	ShowAbout (AboutID);
} // end DoAbout

//-------------------------------------------------------------------------------

Boolean DoUI (GPtr globals)
{
	return FALSE;
}
// end ColorMungerUIMac.cpp
