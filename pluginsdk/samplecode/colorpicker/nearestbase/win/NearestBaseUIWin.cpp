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
	File: NearestBaseUIWin.c

	C source file for Windows specific code for Color Picker example.
*/

#include "NearestBase.h"
#include "DialogUtilities.h"

/*****************************************************************************/

DLLExport BOOL WINAPI PickerProc(HWND hDlg, UINT wMsg, WPARAM wParam, LPARAM lParam); // Win32 Change

/*****************************************************************************/

void DoAbout (AboutRecordPtr about)
{
	ShowAbout(about);
}

/*****************************************************************************/


DLLExport BOOL WINAPI PickerProc(HWND hDlg, UINT wMsg, WPARAM wParam, LPARAM lParam) // Win32 Change
{
	int				idd;	// WIN32 Change
	static GPtr		globals=NULL;		  /* need to be static */
	long			x = 0;
	
	switch (wMsg)
	{

		case  WM_INITDIALOG:

			/* set up globals	*/
			globals = (GPtr) lParam;
			
			CenterDialog(hDlg);
			
			/* fall into WM_PAINT */
		case WM_PAINT:
			return FALSE;
			break;

		case WM_COMMAND:
	  		idd = COMMANDID (wParam);              // WIN32 Change
			if (idd == x) // any radio groups
				; // handle radios;
			else
			{
				switch (idd)
				{
					case OK:
						// assign to globals
						EndDialog(hDlg, idd);
						break;
					case CANCEL:
						gResult = userCanceledErr;
						EndDialog(hDlg, idd);          // WIN32 change
						break;
					default:
						return FALSE;
						break;
				}
			}
			break;
		default:
			return FALSE;
			break;
	}
	return TRUE;
}

/*****************************************************************************/
Boolean DoParameters (GPtr globals)
{

	INT_PTR			nResult = noErr;
	PlatformData	*platform;
	
	platform = (PlatformData *)((PickerRecordPtr) gStuff)->platformData;

	/* Query the user for parameters. */

	nResult = DialogBoxParam(GetDLLInstance(),
					   (LPSTR)"PICKERPARAM",
					   (HWND)platform->hwnd,
					   (DLGPROC)PickerProc,
						(LPARAM)globals);

	return (nResult == OK); // could be -1
}

// NearestBaseUIWin.cpp