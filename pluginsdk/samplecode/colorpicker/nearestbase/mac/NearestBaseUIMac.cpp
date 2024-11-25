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
//		NearestBaseScripting.c
//
//	Copyright 1996-1997, Adobe Systems Incorporated.
//	All Rights Reserved.
//
//	Description:
//		This file contains the Macintosh user interface and dialog
//		source and functions for the Color Picker example module NearestBase.
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

#include "PIDefines.h"
#include "NearestBase.h"

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
	// ShowAbout (AboutID);
}

//-------------------------------------------------------------------------------
//
//	DoUI
//
//	Displays the Parameters dialog
//
//	Inputs:
//		GPtr globals		Pointer to global structure
//
//	Outputs:
//		Displays parameters dialog.
//
//		returns TRUE		If displaying dialog succeeded, user picked an
//							option, and clicked OK.
//		returns FALSE		If displaying dialog failed or user clicked CANCEL
//
//-------------------------------------------------------------------------------

Boolean DoUI (GPtr /*globals*/)
{
	return TRUE; // succeeded
}

//-------------------------------------------------------------------------------

// end NearestBaseUIMac.c