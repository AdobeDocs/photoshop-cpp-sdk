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
	File: ShapeUIWin.c

	C source file for Windows specific code for Selection example.
*/

#include "Shape.h"
#include "DialogUtilities.h"

/*****************************************************************************/

enum
{
	kDFirstItem = 4,
	kDLastItem = kDFirstItem + 6,
	kDCreateRadio1 = 12,
	kDCreateRadioLast = kDCreateRadio1+2
};

/*****************************************************************************/
DLLExport BOOL WINAPI ShapeProc(HWND hDlg, UINT wMsg, WPARAM wParam, LPARAM lParam); // Win32 Change

/*****************************************************************************/

//void DoAbout (AboutRecordPtr about)
//{
//	ShowAbout(about);
//}

/*****************************************************************************/

DLLExport BOOL WINAPI ShapeProc(HWND hDlg, UINT wMsg, WPARAM wParam, LPARAM lParam) // Win32 Change
{
	int		idd;	// WIN32 Change
	static GPtr globals=NULL;		  /* need to be static */	
	
	switch  (wMsg)
	{

		case  WM_INITDIALOG:

			/* set up globals	*/
			globals = (GPtr) lParam;
			
			CenterDialog(hDlg); // CenterWindow(hDlg, CW_BOTH);
			
			SetRadioGroupState(hDlg,
							   kDFirstItem,
							   kDLastItem,
							   kDFirstItem + gWhatShape);
			SetRadioGroupState(hDlg,
							   kDCreateRadio1,
							   kDCreateRadioLast,
							   kDCreateRadio1 + gCreate);
			/* fall into WM_PAINT */
		case WM_PAINT:
			return FALSE;
			break;

		case  WM_COMMAND:
	  		idd = COMMANDID (wParam);              // WIN32 Change
			if (idd >= kDFirstItem && idd <= kDLastItem)
				SetRadioGroupState(hDlg, kDFirstItem, kDLastItem, (int)wParam);
			else if (idd >= kDCreateRadio1 && idd <= kDCreateRadioLast)
				SetRadioGroupState(hDlg, kDCreateRadio1, kDCreateRadioLast, (int)wParam);
			else
			{
				switch  (idd)
				{
					case OK:
						gWhatShape = GetRadioGroupState(hDlg, kDFirstItem, kDLastItem)
									 - kDFirstItem;
						gCreate = GetRadioGroupState(hDlg, kDCreateRadio1, kDCreateRadioLast)
								  - kDCreateRadio1;
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

Boolean DoUIShape (GPtr globals)
{

	INT_PTR			nResult = noErr;
	PlatformData	*platform;
	
	platform = (PlatformData *)((SelectionRecordPtr) gStuff)->platformData;

	/* Query the user for parameters. */

	nResult = DialogBoxParam(GetDLLInstance(),
					   (LPSTR)"SHAPEPARAM",
					   (HWND)platform->hwnd,
					   (DLGPROC)ShapeProc,
						(LPARAM)globals);

	return (nResult != 0); // could be -1
}

// end ShapeUIWin.cpp