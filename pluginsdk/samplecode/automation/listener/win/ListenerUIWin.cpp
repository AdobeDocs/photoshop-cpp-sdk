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
//		ListenerUIWin.cpp
//
//	Description:
//		This file contains the source code and routines for the
//		Automation module Listener, an example of a module
//		that uses the Actions suite in a persistent
//		Automation plug-in.
//
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
//	Includes
//-------------------------------------------------------------------------------

#include "Listener.h"
#include "ListenerUIWin.h"
#include "PITerminology.h"

//-------------------------------------------------------------------------------
//	File globals.
//-------------------------------------------------------------------------------
DLLExport BOOL WINAPI ListenerProc(HWND hDlg, 
								   UINT wMsg, 
								   WPARAM wParam, 
								   LPARAM lParam);

								   
// Some local variables to get from dialog displays to event IDs
struct ListenerEventArray
{
	int32        longVal;
	const char * strStr;
};

// I put these strings into the menu list at runtime
const struct ListenerEventArray useful[] = {
	eventSave,         "eventSave",
	eventGaussianBlur, "eventGaussianBlur",
	eventCopy,         "eventCopy",
	eventCrop,         "eventCrop",
	0,                 NULL
};


// Some local routine definitions
void EventToText(uint32 inputLong, string & returnString);
uint32 TextToEvent(const string & inputStr);


//-------------------------------------------------------------------------------
//
//	DoUI
//
//	Display the Windows user interface.
//
//-------------------------------------------------------------------------------
SPErr DoUI (void)
{
	SPErr error = noErr;
	
	INT_PTR item = kDNoUI;	// Error value.
	
	item = DialogBoxParam(GetDLLInstance(gPlugInRef),
		                  MAKEINTRESOURCE(kListenerDialogID),
						  GetActiveWindow(),
						  (DLGPROC)ListenerProc,
						  NULL);

	if (item != kDOk_button)
	{
		error = 'STOP';
	}
	return error;
}

//-------------------------------------------------------------------------------
//
//	ListenerProc
//
//	Handle dialog interaction
//
//-------------------------------------------------------------------------------
DLLExport BOOL WINAPI ListenerProc(HWND hDlg, 
								   UINT wMsg, 
								   WPARAM wParam, 
								   LPARAM /*lParam*/)
{
	static PIWatchComboBox watchForCombo;
	static PISetComboBox playSetCombo;
	static PIActionComboBox playActionCombo;
	static PIInstalledStaticText staticText;

	switch (wMsg)
	{
		case WM_INITDIALOG:
			CenterDialog(hDlg);
			DoUIInit(hDlg, watchForCombo, playSetCombo, playActionCombo, staticText);
			return TRUE;

		case WM_COMMAND:
		{
			int item = LOWORD (wParam);
			int cmd = HIWORD (wParam);
			switch (item)
			{
				case kDOk_button:
				case kDCancel_button:
					if (cmd == BN_CLICKED)
					{
						EndDialog(hDlg, item);
						return TRUE;
					}
					break;
				case kDWatchFor_editTextPopUp:
					if (cmd == CBN_SELENDOK)
					{
						watchForCombo.Update();
					}
					break;
				case kDPlaySet_editTextPopUp:
					if (cmd == CBN_SELENDOK)
					{
						playSetCombo.Update();
						playActionCombo.Init();
					}
					break;
				case kDPlayAction_editTextPopUp:
					if (cmd == CBN_SELENDOK)
					{
						playActionCombo.Update();
					}
					break;
				default:
					break;
			}
		
			return TRUE;
			break;
		}

		default:
			return FALSE;
			break;
	}
	return TRUE;
}


//-------------------------------------------------------------------------------
//
//	DoUIInit
//
//	Initialize the UI elemets
//
//-------------------------------------------------------------------------------
SPErr DoUIInit(PIDialogPtr dialog,
	PIWatchComboBox & watchFor,
	PISetComboBox & actionSet,
	PIActionComboBox & actionAction,
	PIInstalledStaticText & installedText)
{
	SPErr error = kSPNoError;

	PIItem item = PIGetDialogItem(dialog, kDWatchFor_editTextPopUp);
	watchFor.Init(item);

	item = PIGetDialogItem(dialog, kDPlaySet_editTextPopUp);
	actionSet.Init(item);

	item = PIGetDialogItem(dialog, kDPlayAction_editTextPopUp);
	actionAction.Init(item);

	item = PIGetDialogItem(dialog, kDList_staticText);
	installedText.Init(item);

	return error;
}

//-------------------------------------------------------------------------------
//
//	PIWatchComboBox::Init
//
//	Initialize the watch for combo box.
//
//-------------------------------------------------------------------------------
void PIWatchComboBox::Init(PIItem in)
{
	SetItem(in);

	Clear();

	unsigned short iter = 0;
	while (useful[iter].strStr != NULL)
	{
		AppendItem(useful[iter].strStr);

		if (gEventID == 0)
			gEventID = useful[0].longVal;
		iter++;
	}

	SetCurrentSelection(0);
}

//-------------------------------------------------------------------------------
//
//	PIWatchComboBox::Update
//
//	When the user changes the watch for combo box this routine will get called.
//	Update the global event ID.
//
//-------------------------------------------------------------------------------
void PIWatchComboBox::Update(void)
{
	string text;
	GetCurrentSelection(text);
	DescriptorEventID result = TextToEvent(text);
	if (result != 0)
		gEventID = result;
}

//-------------------------------------------------------------------------------
//
//	PISetComboBox::Init
//
//	Initialize the combo box with a list of the sets from the actions palette.
//
//-------------------------------------------------------------------------------
void PISetComboBox::Init(PIItem in)
{
	SetItem(in);

	if (gActionSet != NULL)
	{
		delete gActionSet;
		gActionSet = NULL;
	}

	Clear();

	// There's a bug in here that makes the count of the classActionSet wrong
	// so you can't do GetInfoFromPhotoshop(classActionSet, keyCount, &count, NULL);
	// So, loop through until GetInfoFromPhotoshopByIndex errors
	uint32 index = 1;
	int32 error = kSPNoError;
	while (error == kSPNoError)
	{
		char name[BigStrMaxLen];
		uint32 length = BigStrMaxLen - 1;
		error = PIUGetInfoByIndex(index,
			classActionSet,
			keyName,
			name,
			&length);

		if (error == kSPNoError)
		{
			AppendItem(name);

			// Create space and copy the name over
			if (gActionSet == NULL)
			{
				gActionSet = new string;
				if (gActionSet != NULL)
					*gActionSet = name;
			}
		}
		index++;
	}

	SetCurrentSelection(0);
}

//-------------------------------------------------------------------------------
//
//	PISetComboBox::Update
//
//	When the user changes the actions set combo box this gets called.
//  Update the gActionSet from the pop up menu. 
//
//-------------------------------------------------------------------------------
void PISetComboBox::Update(void)
{
	string text;
	GetCurrentSelection(text);
	if (gActionSet != NULL)
		delete gActionSet;
	gActionSet = new string;
	if (gActionSet != NULL)
		*gActionSet = text;
}

//-------------------------------------------------------------------------------
//
//	PIActionComboBox::Init
//
//	Set up the combo box with the list of actions in the current set
//  This is dependant on the gActionSet being valid from above. Ugh!
//  I call this routine during the UI init phase and if the gActionSet
//  has changed. The only difference being a valid PIItem in. 
//
//-------------------------------------------------------------------------------
void PIActionComboBox::Init(PIItem in)
{
	if (in != NULL)
		SetItem(in);

	if (gActionName != NULL)
	{
		delete gActionName;
		gActionName = NULL;
	}

	Clear();

	// There's a bug in here that makes items with single actions inside them
	// not appear correctly.  I'll track it down later:
	uint32 index = 1;
	int32 error = kSPNoError;
	while (error == kSPNoError)
	{
		char name[BigStrMaxLen];
		uint32 length;

		error = PIUActionsGetName((char*)(gActionSet->c_str()),
			index,
			name,
			&length);

		if (error == kSPNoError)
		{
			AppendItem(name);

			// Create space and copy the name over
			if (gActionName == NULL)
			{
				gActionName = new string;
				if (gActionName != NULL)
					*gActionName = name;
			}
		}
		index++;
	} // actionsLoop

	SetCurrentSelection(0);
}

//-------------------------------------------------------------------------------
//
//	PIActionComboBox::Update
//
//	This gets called when the user changes the action combo box.
//  Set the gActionName to the current setting
//
//-------------------------------------------------------------------------------
void PIActionComboBox::Update(void)
{
	string text;
	GetCurrentSelection(text);
	if (gActionName != NULL)
		delete gActionName;
	gActionName = new string;
	if (gActionName != NULL)
		*gActionName = text;
}

//-------------------------------------------------------------------------------
//
//	PIInstalledStaticText::Init
//
//	Set the text of the static item with all of the current installed events.
//
//-------------------------------------------------------------------------------
void PIInstalledStaticText::Init(PIItem in)
{
	SetItem(in);

	const char* const displayString = "%s: \"%s\" of \"%s\".\r";
	size_t displayString_size = strlen(displayString);
	string eventAsString;
	// Figure out the size of all the strings in the listener list:

	size_t size = 0;

	Listener_t* thisList = gListenerList;

	while (thisList != NULL)
	{
		EventToText(thisList->eventID, eventAsString);
		size += eventAsString.length();
		size += thisList->actionName.length();
		size += thisList->actionSet.length();
		size += displayString_size;
		thisList = thisList->next;
	}

	string display;

	if (size == 0)
	{
		SetText("No notifiers installed.");
		return;
	}

	thisList = gListenerList;

	size = 0;

	while (thisList != NULL)
	{
		EventToText(thisList->eventID, eventAsString);
		string tempDisplay;

		tempDisplay = eventAsString;
		tempDisplay += ": \"";
		tempDisplay += thisList->actionName;
		tempDisplay += "\" of \"";
		tempDisplay += thisList->actionSet;
		tempDisplay += "\".\r";

		display += tempDisplay;

		thisList = thisList->next;

		size++;
	}

	// I only have room for 5 messages		
	if (size < 6)
		SetText(display);
	else
		SetText("Too many notifiers to display.");

}

//-------------------------------------------------------------------------------
//
//	EventToText
//
//	Convert the event 'Hash' code to the string representation of it.
//
//-------------------------------------------------------------------------------
void EventToText(uint32 inputLong, string & returnString)
{
	unsigned short index = 0;
	while (useful[index].strStr != NULL)
	{
		if (useful[index].longVal == inputLong)
		{
			returnString = useful[index].strStr;
			break;
		}
		index++;
	}
}

//-------------------------------------------------------------------------------
//
//	TextToEvent
//
//	Convert the string to the event 'Hash'
//
//-------------------------------------------------------------------------------
uint32 TextToEvent(const string & inputStr)
{
	uint32 returnLong = 0;
	if (inputStr.length())
	{
		unsigned short index = 0;
		while (useful[index].strStr != NULL)
		{
			if (inputStr.compare(useful[index].strStr) == 0)
			{
				returnLong = useful[index].longVal;
				break;
			}
			index++;
		}
	}

	return (returnLong);
}
//-------------------------------------------------------------------------------
// end ListenerUIWin.cpp
