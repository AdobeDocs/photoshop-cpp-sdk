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
/*
	File: SelectoramaUIWin.c

	C source file for Windows specific code for Selection example.
*/

#include "Selectorama.h"
#include "DialogUtilities.h"

/*****************************************************************************/

DLLExport BOOL WINAPI SelectionProc(HWND hDlg, UINT wMsg, WPARAM wParam, LPARAM lParam); // Win32 Change
Boolean Validate (GPtr globals, const HWND hDlg, int *item);

/*****************************************************************************/

void DoAbout (AboutRecordPtr about)
{
	ShowAbout(about);
}

/*****************************************************************************/
// Because we want to wait until the user changes the focus
// from an EditText field to something else, and because we
// want to ignore validation if the user Cancels, this routine
// does round of validating fields if it's been flagged.

Boolean Validate (GPtr globals, const HWND hDlg, int *item)
{
	int32	x = 0;
	Boolean retn = true;
	short	numberErr = noErr;

	switch (*item)
	{
		case  kPercentEdit: // user moving to something not the edit text
			numberErr = FetchNumber(hDlg, 
							   kPercentEdit, 
							   kPercentMin, 
							   kPercentMax, 
							   &x);

			if (numberErr != noErr)
			{ // shows alert if there's an error
				AlertNumber(hDlg,
							kPercentEdit,
							kPercentMin,
							kPercentMax,
							&x,
							AlertID,
							numberErr);
				retn = false; // had to pop an error, stay here
			}

			// gPercent = (short)x;

			break;
	}
	*item = 0;
	return retn;
}

/*****************************************************************************/

DLLExport BOOL WINAPI SelectionProc(HWND hDlg, UINT wMsg, WPARAM wParam, LPARAM lParam) // Win32 Change
{
	int				item = 0;
	int				cmd = 0;
	int32			x = 0;
	short			numberErr = noErr;
	static GPtr		globals=NULL;		  /* need to be static */
	static int		lastItem = 0;
	// static short	lastPercent = gPercent;
	
	switch (wMsg)
	{

		case  WM_INITDIALOG:

			/* set up globals	*/
			globals = (GPtr) lParam;
			
			CenterDialog(hDlg);
			
			SetRadioGroupState(hDlg,
							   kUseRadio1, 
							   kUseRadioLast, 
							   kUseRadio1 + gWhatChannels);
			SetRadioGroupState(hDlg, 
							   kFirstItem, 
							   kLastItem, 
							   kFirstItem + gWhatArea);
			SetRadioGroupState(hDlg,
							   kCreateRadio1,
							   kCreateRadioLast, 
							   kCreateRadio1 + gCreate);
			ShowHideItem(hDlg, kPercentStatic, (gWhatArea == iSelectRandom));
			ShowHideItem(hDlg, kPercentEdit, (gWhatArea == iSelectRandom));
			ShowHideItem(hDlg, kPercentSymbol, (gWhatArea == iSelectRandom));
			StuffNumber(hDlg, kPercentEdit, gPercent);
			if (gWhatArea == iSelectRandom) SelectTextItem(hDlg, kPercentEdit);
			
			/* fall into WM_PAINT */
		case WM_PAINT:
			return FALSE;
			break;

		case WM_COMMAND:
	  		item = COMMANDID (wParam);
			cmd = HIWORD (wParam);

			switch (item)
			{
				case OK:
					if (cmd == BN_CLICKED)
					{
						gWhatChannels = GetRadioGroupState(hDlg, kUseRadio1, kUseRadioLast)
										- kUseRadio1;
						gWhatArea = GetRadioGroupState(hDlg, kFirstItem, kLastItem)
									- kFirstItem;
						gCreate = GetRadioGroupState(hDlg, kCreateRadio1, kCreateRadioLast)
								  - kCreateRadio1;
						if (gWhatArea == iSelectRandom)
						{
							lastItem = kPercentEdit; // force validate
							if (Validate(globals, hDlg, &lastItem))
							{
								FetchNumber(hDlg, kPercentEdit, kPercentMin, kPercentMax, &x);
								gPercent = (short)x;
								EndDialog(hDlg, item); 
								return TRUE;
							}
						}
						else // no iSelectRandom
						{
							EndDialog(hDlg, item);
							return TRUE;
						}
					}
					else 
					{
						(void) Validate(globals, hDlg, &lastItem);
					}
					break;
				case CANCEL:
					if (cmd == BN_CLICKED)
					{
						gResult = userCanceledErr;
						EndDialog(hDlg, item);
					}
					break;
				case kPercentEdit:
					if (Validate(globals, hDlg, &lastItem))
					{
						if (cmd == EN_KILLFOCUS)
							lastItem = kPercentEdit; // validate will get this
						else if (cmd == EN_CHANGE)
						{
							numberErr = FetchNumber(hDlg,
													kPercentEdit,
													kPercentMin,
													kPercentMax,
													&x);
							if (numberErr == noErr && x != gPercent)
							{ // different number, update
								// gPercent = (short)x;
							}
						}
					}
					break;
				default:
					if (item >= kUseRadio1 && item <= kUseRadioLast)
					{
						if (Validate(globals, hDlg, &lastItem))
						{
							if (cmd == BN_CLICKED)
							{
								SetRadioGroupState(hDlg, kUseRadio1, kUseRadioLast, item);
							}
						}
					}
					else if (item >= kCreateRadio1 && item <= kCreateRadioLast)
					{
						if (Validate(globals, hDlg, &lastItem))
						{
							if (cmd == BN_CLICKED)
								SetRadioGroupState(hDlg, kCreateRadio1, kCreateRadioLast, item);
						}
					}
					else if (item >= kFirstItem && item <= kLastItem)
					{
						if (Validate(globals, hDlg, &lastItem))
						{
							if (cmd == BN_CLICKED)
							{
								SetRadioGroupState(hDlg, kFirstItem, kLastItem, item);
								ShowHideItem(hDlg, kPercentStatic, (item == (iSelectRandom + kFirstItem)));
								ShowHideItem(hDlg, kPercentEdit, (item == (iSelectRandom + kFirstItem)));
								ShowHideItem(hDlg, kPercentSymbol, (item == (iSelectRandom + kFirstItem)));
								if (item == (iSelectRandom + kFirstItem)) SelectTextItem(hDlg, kPercentEdit);
							}
						}
					}
					return FALSE;
					break;
			} /* switch (item) */ 
			break;
		default:
			return FALSE;
			break;
	} /* switch (wMsg) */
	return TRUE;
}

/*****************************************************************************/
Boolean DoUI (GPtr globals)
{

	int				nResult = noErr;
	PlatformData	*platform;
	
	platform = (PlatformData *)((SelectionRecordPtr) gStuff)->platformData;

	/* Query the user for parameters. */

	nResult = (int)DialogBoxParam(GetDLLInstance(),
					   (LPSTR)"SELECTIONPARAM",
					   (HWND)platform->hwnd,
					   (DLGPROC)SelectionProc,
						(LPARAM)globals);

	return (nResult == OK); // could be -1
}

// end SelectoramaUIWin.cpp