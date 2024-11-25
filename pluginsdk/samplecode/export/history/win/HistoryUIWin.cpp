// ADOBE SYSTEMS INCORPORATED
// Copyright  1993 - 2002 Adobe Systems Incorporated
// All Rights Reserved
//
// NOTICE:  Adobe permits you to use, modify, and distribute this 
// file in accordance with the terms of the Adobe license agreement
// accompanying it.  If you have received this file from a source
// other than Adobe, then your use, modification, or distribution
// of it requires the prior written permission of Adobe.
/*
	File: History.cpp

	C source file for Windows specific code for History example.
*/

#include "History.h"

/****************************************************************************/

enum
{
	kDTrimFirst = 3,
	kDTrimLast,

	kDUpButton,
	kDDownButton,

	kDStatusText,

	kDHistItem1,
	kDHistTotal = 7,
	kDHistItemEnd = kDHistItem1 + kDHistTotal
};



/****************************************************************************/

void CheckAndEnable (GPtr globals, const HWND hDlg, int16 *currentResources, Str255 hS);
void UpdateHistories(GPtr globals, const HWND hDlg, short count, Str255 hS);

/*****************************************************************************/

void DoAbout (AboutRecordPtr about)
{
	ShowAbout(about);
}

/*****************************************************************************/
// This routine checks our values and update them accordingly whenever a
// button gets pressed, so this routine will update the resources and populate
// the dialog.

void CheckAndEnable (GPtr globals, const HWND hDlg, int16 *currentResources, Str255 hS)
{

	*currentResources = CountPIResources(histResource);

	if (gCurrentHistory < 1)
		gCurrentHistory = 1;
	
	if (gCurrentHistory > *currentResources)
		gCurrentHistory = *currentResources;
	
	if (*currentResources <= kDHistTotal ||
		gCurrentHistory+kDHistTotal > *currentResources)
	{
		if (GetFocus() == GetDlgItem(hDlg, kDDownButton))
			SetDialogDefaultItem(hDlg, OK); // set focus to OK since we're disabling
		PIDisableControl (hDlg, kDDownButton); // nothing extra to show
	}
	else
		PIEnableControl (hDlg, kDDownButton);

	
	if (gCurrentHistory < 2)
	{
		if (GetFocus() == GetDlgItem(hDlg, kDUpButton))
			SetDialogDefaultItem(hDlg, OK); // set focus to OK since we're disabling
		PIDisableControl (hDlg, kDUpButton);
	}
	else
		PIEnableControl (hDlg, kDUpButton);

	if (*currentResources >= gCurrentHistory && *currentResources > 0)
	{ // enable trim buttons
		PIEnableControl (hDlg, kDTrimFirst);
		PIEnableControl (hDlg, kDTrimLast);
	}
	else
	{ // disable trim buttons
		if (GetFocus() == GetDlgItem(hDlg, kDTrimFirst) ||
			GetFocus() == GetDlgItem(hDlg, kDTrimLast))
			SetDialogDefaultItem(hDlg, OK); // set focus to OK since we're disabling
		PIDisableControl (hDlg, kDTrimFirst);
		PIDisableControl (hDlg, kDTrimLast);
	}

	UpdateHistories(globals, hDlg, *currentResources, hS);
}

/*****************************************************************************/

DLLExport UINT_PTR WINAPI ExportProc(HWND hDlg, UINT wMsg, WPARAM wParam, LPARAM lParam)       // Win32 Change
{
	int				item, cmd;
	short			numberErr = 0;
	int32			x = 0;
	static Str255	hS = " ^0 to ^1 of ^2";
 	static int16	currentResources = 0; // static so retains value on calls
	static int		lastItem = 0;
	static GPtr		globals = NULL;		  /* need to be static */

	switch  (wMsg)
	{

		case  WM_INITDIALOG:

			/* set up globals	*/

			globals = (GPtr) lParam;

			CenterDialog(hDlg);
			
		    hS[0] = (char)strlen((char*)&hS[1]);

			CheckAndEnable (globals, hDlg, &currentResources, hS);
			/* drop into PAINT */
		case WM_PAINT:
			// return FALSE;
			break;

		case  WM_COMMAND:
	  		item = COMMANDID (wParam);              // WIN32 Change
			cmd = HIWORD (wParam);

			switch  (item)
			{
				case kDTrimFirst:
					if (cmd == BN_CLICKED)
					{
						if (currentResources >= gCurrentHistory)
						{
							DeletePIResource (histResource, gCurrentHistory);
							gStuff->dirty = TRUE;
						}
						CheckAndEnable (globals, hDlg, &currentResources, hS);
						// checks buttons and values
						break;
					}
				case kDTrimLast:
					if (cmd == BN_CLICKED)
					{
						if (currentResources >= gCurrentHistory + kDHistTotal-1)
						{
							DeletePIResource (histResource,
								(short)(gCurrentHistory + kDHistTotal-1));
							gStuff->dirty = TRUE;
						}
						else if (currentResources > 0)
						{
							DeletePIResource (histResource,
								(short)(currentResources));
							gStuff->dirty = TRUE;
						}
						CheckAndEnable (globals, hDlg, &currentResources, hS);
						// checks buttons and values
						break;
					}
				case kDUpButton:
					if (cmd == BN_CLICKED)
					{
						gCurrentHistory--;
						CheckAndEnable (globals, hDlg, &currentResources, hS);
						break;
					}
				case kDDownButton:
					if (cmd == BN_CLICKED)
					{
						gCurrentHistory++;
						CheckAndEnable (globals, hDlg, &currentResources, hS);
						break;
					}
				case OK:
					if (cmd == BN_CLICKED)
					{
						EndDialog(hDlg, item); 
						return TRUE;
					}
					break;
				case CANCEL:
					if (cmd == BN_CLICKED)
					{
						gResult = userCanceledErr;
						EndDialog(hDlg, item);          // WIN32 change
						return TRUE;
					}
					// don't ever validate during cancel
					break;
				default:
					return FALSE;
					break;
			} // switch (item)
			break; // case WM_COMMAND
		} // switch

	return  FALSE;
}

/*****************************************************************************/
Boolean DoUI (GPtr globals)
{

	INT_PTR    nResult;
	PlatformData *platform;

	platform = (PlatformData *)((ExportRecordPtr) gStuff)->platformData;

	/* Query the user for percent dissolve. */

	nResult = DialogBoxParam(GetDLLInstance(),
							 (LPSTR)"HISTORYPARAM",
							 (HWND)platform->hwnd,
							 (DLGPROC)ExportProc,
							 (LPARAM)globals);

	return (nResult == OK);

}	

/****************************************************************************/
/* Populate dialog with history strings. */

void UpdateHistories (GPtr globals, const HWND hDlg, short count, Str255 hS)
{
	Str255 s = "";
		
	Str255 n1 = ""; // for "^0 to ^1 of ^2"
	Str255 n2 = ""; //
	Str255 nT = ""; //
	Str255 ss = ""; //

	long	x = gCurrentHistory + (kDHistTotal - 1);
	short	loop;

	PICopy(ss, hS, hS[0]+1); // make a new copy

	for (loop = gCurrentHistory; loop < gCurrentHistory + kDHistTotal; loop++)
	{
		GetHistory (globals, loop, s);
		if (loop == gCurrentHistory && !s[0])
		{
            Str255 s1 = " No history resources in this document.";
            s1[0] = (char)strlen((char*)&s1[1]);
            PICopy(s, s1, s1[0]+1);
			ss[ (ss[0] = 0)+1 ] = 0; // none
		}
		StuffText (hDlg, (short)(kDHistItem1 + (loop - gCurrentHistory)), s);
		if (s[0] < 1 && x > loop-1)
			x = loop-1;
	}

	if (ss[0] > 0)
	{ // got the display string.  Populate it.
		NumToString(gCurrentHistory, n1);
		NumToString(x, n2);
		NumToString(count, nT);
		PIParamText(ss, n1, n2, nT);
	}
	StuffText (hDlg, kDStatusText, ss);
	// even if nothing, stuff empty or string in field
}

/*****************************************************************************/

// end HistoryUIWin.cpp

