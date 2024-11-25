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
//		ListenerUIWIN.h
//
//	Description:
//		Definitions and routines that are common to Mac and Win
//
//-------------------------------------------------------------------------------
#ifndef __ListenerUIWIN_H__		// Has this not been defined yet?
#define __ListenerUIWIN_H__		// Only include this once by predefining it

#include "Listener.h"
#include "PIUI.h"
//-------------------------------------------------------------------------------
//	Definitions -- Dialog items
//-------------------------------------------------------------------------------
enum
{
	kDNoUI = -1,		// Error.
	kDOk_button = 1,	// Must be one.
	kDCancel_button,	// Must be two.
	kDWatchFor_staticText,
	kDWatchFor_editTextPopUp,
	kDPlaySet_staticText,
	kDPlaySet_editTextPopUp,
	kDPlayAction_staticText,
	kDPlayAction_editTextPopUp,
	kDInstalled_staticText,
	kDList_staticText
};
	
class PIWatchComboBox : public PIComboBox {
public:
	void Init(PIItem in);
	void Update(void);
};

class PISetComboBox : public PIComboBox {
public:
	void Init(PIItem in);
	void Update(void);
};

class PIActionComboBox : public PIComboBox {
public:
	void Init(PIItem in = NULL);
	void Update(void);
};

class PIInstalledStaticText : public PIText {
public:
	void Init(PIItem in);
	void Update(void);
};

// Show the UI.
SPErr DoUIInit(PIDialogPtr dialog, 
			   PIWatchComboBox & watchFor, 
			   PISetComboBox & actionSet, 
			   PIActionComboBox & actionAction, 
			   PIInstalledStaticText & installedText);
#endif