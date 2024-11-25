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
	File: GradientImportUIWin.cpp

	C++ source file for MS-Windows specific code
*/

#include "GradientImport.h"

/*****************************************************************************/

enum
{
	dRowsItem = 3,
	dColsItem,
	dFirstModeItem,
	dLastModeItem = 8,
	dBitmapModeItem = dFirstModeItem,
	dIndexedModeItem = dLastModeItem-1,
	dInvertItem = 9,
	dModeGroupItem = 12,
	dConvertAlpha = 14
};

/*****************************************************************************/

#define STATE_IDLE		0
#define STATE_AGAIN		1
#define STATE_CANCEL	-1

//#define SDK_VALIDATE	WM_USER+100

short	state=STATE_IDLE;
short	lastTitem=dRowsItem;
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
		case dColsItem:
			numberErr = FetchNumber(hDlg, 
							   dColsItem, 
							   kColumnsMin, 
							   kColumnsMax, 
							   &x);
			if (numberErr != noErr)
			{ // shows alert if there's an error
				AlertNumber(hDlg,
							dColsItem,
							kColumnsMin,
							kColumnsMax,
							&x,
							AlertID,
							numberErr);
				retn = false; // had to pop an error, stay here
			}
			break;
		case dRowsItem:
			numberErr = FetchNumber(hDlg, 
							   dRowsItem, 
							   kRowsMin, 
							   kRowsMax, 
							   &x);
			if (numberErr != noErr)
			{ // shows alert if there's an error
				AlertNumber(hDlg,
							dRowsItem,
							kRowsMin,
							kRowsMax,
							&x,
							AlertID,
							numberErr);
				retn = false; // had to pop an error, stay here
			}
			break;
	}
	*item = 0;
	return retn;
}

/*****************************************************************************/

/* Dialog box code to ask for the acquire parameters. */

DLLExport BOOL WINAPI UIProc(HWND hDlg, UINT wMsg, WPARAM wParam, LPARAM lParam)     // Win32 Change
{
	static GPtr globals=NULL;		  /* need to be static	*/

	int32 x = 0;
	static int lastItem = 0;
	int item;
	int cmd;
	short numberErr = noErr;
	static HWND h = NULL; // last window with focus
	static PlatformData *platform;
	
	switch  (wMsg)
	{
      case WM_INITDIALOG:
		/* set up globals	*/
		globals = (GPtr) lParam;
		platform = (PlatformData *)((AcquireRecordPtr) gStuff)->platformData;
			
		CenterDialog(hDlg);
				
		// Initialize Images, width and height edit boxes with default values
		StuffNumber(hDlg, dRowsItem, gLastRows);
		StuffNumber(hDlg, dColsItem, gLastCols);
		SetCheckBoxState (hDlg, dInvertItem, (gLastInvert && gStuff->canReadBack));
		SetCheckBoxState (hDlg, dConvertAlpha, gConvertAlpha);

		/* We will only be inverting if we can read the image back. */

		if (gStuff->canReadBack)
			PIEnableControl (hDlg, dInvertItem);
		else
			PIDisableControl (hDlg, dInvertItem);

		PIEnableControl (hDlg, dConvertAlpha);

		// Initialize mode to its default
		SetRadioGroupState(hDlg, 
						   dFirstModeItem, 
						   dLastModeItem, 
						   gLastMode - plugInModeBitmap + dFirstModeItem);
	
		// Rows edit box will get the focus
		SelectTextItem(hDlg, dRowsItem);
		return FALSE;
		break;
	  case WM_PAINT:
		return FALSE; // handled it
		break;
	  case WM_ACTIVATE: 
		if (LOWORD(wParam) == WA_INACTIVE && 
			state == STATE_IDLE && 
			lParam == (LPARAM)platform->hwnd)
		{
			 /* Bug in non-modal lets parent get clicked.
				This will post the reversing deactivate and activate
				messages to fix it. */
			h = GetFocus();
			PostMessage((HWND)lParam, WM_ACTIVATE, (WPARAM)WA_INACTIVE,
						(LPARAM)hDlg); // inactivate host
			PostMessage((HWND)hDlg, WM_ACTIVATE, (WPARAM)WA_ACTIVE,
						lParam); // activate us again
			MessageBeep(MB_OK); 
		}
		else if (h != NULL) // reset the focus on activate
		{
			SetDialogDefaultItem(hDlg, OK); // outline
			SetFocus(h);
			h = NULL;
		}
		return FALSE; // 0=we handled it
		break;
	  case WM_COMMAND:
		item = COMMANDID (wParam);              // WIN32 Change
        cmd = HIWORD(wParam);

		switch (item)
		{
			case OK:
				if (cmd == BN_CLICKED)
				{
					lastItem = dRowsItem; // force validation
					if (Validate(globals, hDlg, &lastItem))
					{
						numberErr = FetchNumber(hDlg, 
								dRowsItem, 
								kRowsMin, 
								kRowsMax, 
								&x);
						
						lastItem = dColsItem; 
						if (Validate(globals, hDlg, &lastItem))		
						{
							gLastRows = (short)x;
						
							numberErr = FetchNumber(hDlg, 
								dColsItem, 
								kColumnsMin, 
								kColumnsMax, 
								&x);
							gLastCols = (short)x;

							gLastMode = GetRadioGroupState(hDlg,
												   dFirstModeItem,
												   dLastModeItem)
												   - dFirstModeItem
												   + plugInModeBitmap;
							gLastInvert = GetCheckBoxState(hDlg, dInvertItem);
							gConvertAlpha = GetCheckBoxState(hDlg, dConvertAlpha);
							state = STATE_AGAIN;
						} // bad cols
					} // bad rows
				} // btn clicked
				else 
				{
					(void) Validate(globals, hDlg, &lastItem);
				}
				// btn not clicked but focus may have changed to it so validate
				break;

			case CANCEL:
				if (cmd == BN_CLICKED)
					state = STATE_CANCEL;
				// don't validate ever when we're dealing with cancel
				break;

			case dFirstModeItem:
			case dFirstModeItem+1:
			case dFirstModeItem+2:
			case dFirstModeItem+3:
				if (Validate(globals, hDlg, &lastItem))
					if (cmd == BN_CLICKED)
						SetRadioGroupState(hDlg, dFirstModeItem, dLastModeItem, (int)wParam);
				break;
			case dInvertItem:
				if (Validate(globals, hDlg, &lastItem))
					if (cmd == BN_CLICKED)
						ToggleCheckBoxState(hDlg, dInvertItem);
				break;
			case dConvertAlpha:
				if (Validate(globals, hDlg, &lastItem))
					if (cmd == BN_CLICKED)
						ToggleCheckBoxState(hDlg, dConvertAlpha);
				break;
			case dRowsItem:
				if (Validate(globals, hDlg, &lastItem))
				{
					if (cmd == EN_KILLFOCUS)
						lastTitem = lastItem = dRowsItem; // for validation
					else if (cmd == EN_CHANGE)
					{
						numberErr = FetchNumber(hDlg,
											dRowsItem,
											kRowsMin,
											kRowsMax,
											&x);
						if (numberErr == noErr && x != gLastRows)
						{ // different number, update
							/* later implementation
								gLastRows = (short)x;

								// Do Filtering operation
								DoFilterRect (globals);

								// Invalidate Proxy Item
								GetWindowRect(GetDlgItem(hDlg, kProxyItem), &imageRect);
								ScreenToClient (hDlg, (LPPOINT)&imageRect);
								ScreenToClient (hDlg, (LPPOINT)&(imageRect.right));
								InvalidateRect (hDlg, &imageRect, FALSE);
							*/ // for later
						}
					}
				}
				break;
			case dColsItem:
				if (Validate(globals, hDlg, &lastItem))
				{
					if (cmd == EN_KILLFOCUS)
						lastTitem = lastItem = dColsItem; // for validate routine
					else if (cmd == EN_CHANGE)
					{
						numberErr = FetchNumber(hDlg,
											dColsItem,
											kColumnsMin,
											kColumnsMax,
											&x);
						if (numberErr == noErr && x != gLastCols)
						{ // different number, update
							/* later implementation
								gLastCols = (short)x;

								// Do Filtering operation
								DoFilterRect (globals);

								// Invalidate Proxy Item
								GetWindowRect(GetDlgItem(hDlg, kProxyItem), &imageRect);
								ScreenToClient (hDlg, (LPPOINT)&imageRect);
								ScreenToClient (hDlg, (LPPOINT)&(imageRect.right));
								InvalidateRect (hDlg, &imageRect, FALSE);
							*/ // for later
						}
					}
				}
				break;
		} /* switch (item) */
		return FALSE;
		break;

	  default:
		return FALSE; // we didn't handle it FALSE;
		break;
    } /* switch */
	return TRUE; // we didn't handle it FALSE;
}

/*****************************************************************************/

/* Setup the parameters dialog.  Returns TRUE if it succeeds.				 */

Boolean OpenOurDialog (GPtr globals)
	{
	
	PlatformData *platform;

	state = STATE_IDLE;
	
	if (gDialog != NULL)
	{
		SetActiveWindow (gDialog);
		// Select first text item again
		if (lastTitem > 0)
		{ // reselect last text item
			SelectTextItem(gDialog, lastTitem);
			lastTitem = 0;
		}
		// set the dialog window the active window
		return TRUE;
	}
	else
	{ // dialog not created yet.  Do it.
		platform = (PlatformData *)((AcquireRecordPtr) gStuff)->platformData;


		gDialog = CreateDialogParam(GetDLLInstance(),
									(LPSTR)"UIPARAM",
									(HWND)platform->hwnd,
									(DLGPROC)UIProc, // FARPROC
									(LPARAM)globals);
    
		if (gDialog)
		{ // got it.  Show it.
			ShowWindow(gDialog, SW_NORMAL);
			UpdateWindow(gDialog);
			return TRUE;
		}
		else // lost it.
			return FALSE;		// Couldnt put up the dialog box		
	}
}

/*****************************************************************************/

/* Run the parameters dialog.  Returns TRUE if it succeeds.					 */

Boolean RunOurDialog (GPtr globals)
	{
	// We are basically waiting for user to press "Import" or "Cancel" from the 
	// dialog

	//HWND pWin = NULL; // parent window
	MSG msg;
	
	// First post a message to the host to update itself
	
	/*
	pWin = GetParent(gDialog); // gets the parent (host)
	RedrawWindow(pWin, NULL, NULL, RDW_INTERNALPAINT | RDW_UPDATENOW | RDW_ALLCHILDREN);		
	*/

	while (GetMessage((LPMSG)&msg, NULL, 0, 0))
	{
		if (!IsDialogMessage((HWND)gDialog, &msg))
		{ // wasn't for our dialog.  Send it on its way.
			TranslateMessage((LPMSG)&msg);
			DispatchMessage ((LPMSG)&msg);
		}
		

		// state is set in UIProc based on the user response
		// The only way this function would return is by the user responding with
		// either pressing the "Import" or the "Cancel" button
		if (state == STATE_AGAIN)
		{ 
			return TRUE;
		}
		else if (state == STATE_CANCEL)
		{
			return FALSE;
		}
	}
	return FALSE; // should never get here but error if it does
}


/*****************************************************************************/

/* Close the parameters dialog.					 							 */

void CloseOurDialog (GPtr globals)
{
	
	if (gDialog != NULL)
	{
		DestroyWindow(gDialog);
		gDialog = NULL;
	}
	
}
// end GradientImportUIWin.cpp