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
//		ListenerTerminology.h
//
//	Description:
//		Resource and code definitions for persistent automation plug-in. 
//
//-------------------------------------------------------------------------------

#ifndef __ListenerTerminology_h__
#define __ListenerTerminology_h__



//-------------------------------------------------------------------------------
//	Definitions -- Required by include files.
//-------------------------------------------------------------------------------

// The About box and resources are created in PIUtilities.r.
// You can easily override them, if you like.

#define plugInName			"Listener"
#define plugInCopyrightYear	"1998"
#define plugInDescription \
	"A persistent Actions Module to background listen in Adobe Photoshop¨."

//-------------------------------------------------------------------------------
//	Definitions -- Scripting keys
//-------------------------------------------------------------------------------

#define keyPIAction		'actN'
#define keyPIActionSet	'actS'
#define keyPIActionID	'actI'

//-------------------------------------------------------------------------------
//	Definitions -- Resources
//-------------------------------------------------------------------------------
// Have to do it this way, and not with an enum, because the Windows
// precompiler does not do the right thing when precompiling the .r file:
#define ListenerResourceID		18500
#define kListenerDialogID		ListenerResourceID+1

#define kListenerAboutID		kListenerDialogID+1

//-------------------------------------------------------------------------------

#endif // __ListenerTerminology_h__
// end ListenerTerminology.h
