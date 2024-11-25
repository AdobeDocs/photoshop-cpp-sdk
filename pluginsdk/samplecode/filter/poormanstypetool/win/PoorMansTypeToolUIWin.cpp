// ADOBE SYSTEMS INCORPORATED
// Copyright  1993 - 2002 Adobe Systems Incorporated
// All Rights Reserved
//
// NOTICE:  Adobe permits you to use, modify, and distribute this 
// file in accordance with the terms of the Adobe license agreement
// accompanying it.  If you have received this file from a source
// other than Adobe, then your use, modification, or distribution
// of it requires the prior written permission of Adobe.
//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
//
//	File:
//		PoorMansTypeToolUIWin.cpp
//
//	Description:
//		This file contains the source and routines for the
//		Filter module Poor Man's Type Tool, a module that 
//		uses the Channel Ports Suite for pixel munging.
//
//	Use:
//		This is a basic module to exemplify all the typical
//		functions a filter module will do: Read scripting
//		parameters, show a user interface, display a proxy,
//		write changed pixel information, and then write
//		scripting parameters.
//
//-------------------------------------------------------------------------------

namespace std {
	using ::size_t;
};


#include "PoorMansTypeTool.h"
#include "DialogUtilities.h"

#include <sstream>
#include <iomanip>

/****************************************************************************/
void ShowOutputBuffer(GPtr globals, HWND hPrev);
void PaintProxyView(GPtr globals, HWND hDlg);
Boolean Validate (GPtr globals, const HWND hDlg, int *item);
void InitializeMenu(GPtr globals, HWND hDlg, int item);
void SetSizePopUpMenu(HWND hDlg, int32 itemAsNumber);
int32 GetSizePopUpMenu(HWND hDlg);

/*****************************************************************************/

/* Calculates the rectangle we will want to use for the proxy for an image. */

void GetProxyItemRect (HWND	hDlg, Rect *proxyItemRect)
{

	RECT	wRect;
	
	GetWindowRect(GetDlgItem(hDlg, kDProxyItem), &wRect);	
	ScreenToClient (hDlg, (LPPOINT)&wRect);
	ScreenToClient (hDlg, (LPPOINT)&(wRect.right));

	proxyItemRect->left = (short)wRect.left;
	proxyItemRect->top  = (short)wRect.top;
	proxyItemRect->right = (short)wRect.right;
	proxyItemRect->bottom = (short)wRect.bottom;
	
}

/*****************************************************************************/

void DoAbout (AboutRecordPtr about)
{
	ShowAbout(about);
}

/*****************************************************************************/

void ShowOutputBuffer(GPtr globals, HWND hDlg)
{
	PSPixelMap pixels;
	PSPixelMask mask;
	PSPixelOverlay overlay;
	PAINTSTRUCT  ps;
	Rect  itemBounds;
	RECT  wRect, entireRect;
	POINT	mapOrigin; 
	HDC		hDC;

	GetWindowRect(GetDlgItem(hDlg, kDProxyItem), &wRect);
	mapOrigin.x = 0;
	mapOrigin.y = 0;
	ClientToScreen(hDlg, &mapOrigin);

	itemBounds.left = (short)(((wRect.right + wRect.left - gProxyRect.right + gProxyRect.left)
		/ 2) - mapOrigin.x);
	itemBounds.top = (short)(((wRect.bottom + wRect.top - gProxyRect.bottom + gProxyRect.top) 
		/ 2) - mapOrigin.y);
	itemBounds.right = (short)(itemBounds.left + (gProxyRect.right - gProxyRect.left));
	itemBounds.bottom = (short)(itemBounds.top + (gProxyRect.bottom - gProxyRect.top));
	
	hDC = BeginPaint(hDlg, &ps);	

	SetBkColor(hDC, GetSysColor(COLOR_BTNFACE));
	GetClientRect(GetDlgItem(hDlg, kDProxyItem), &entireRect);
	ExtTextOut(hDC, 0, 0, ETO_OPAQUE, &entireRect, NULL, 0, NULL);

	wRect.left = itemBounds.left;
	wRect.top = itemBounds.top;
	wRect.right = itemBounds.right;
	wRect.bottom = itemBounds.bottom;

	InflateRect(&wRect, kGutter, kGutter);
	FrameRect(hDC, &wRect, (HBRUSH)GetStockObject(BLACK_BRUSH));	
	InflateRect(&wRect, -kGutter, -kGutter);
	
	pixels.version       = 2;
	pixels.bounds.top    = gProxyRect.top;
	pixels.bounds.left   = gProxyRect.left;
	pixels.bounds.bottom = gProxyRect.bottom;
	pixels.bounds.right  = gProxyRect.right;
	pixels.imageMode     = gDocDesc->imageMode;
	pixels.rowBytes      = (gProxyRect.right - gProxyRect.left) 
							* gDocDesc->depth 
							/ 8;
	pixels.colBytes		 = 1;
	pixels.planeBytes    = (gProxyRect.bottom - gProxyRect.top) 
		             		* (gProxyRect.right - gProxyRect.left);
	pixels.baseAddr		 = gChannelData;
	
	pixels.mat			= NULL;
	pixels.masks		= NULL;
	pixels.maskPhaseRow = 0;
	pixels.maskPhaseCol = 0;
	
	// new for version 2 of PSPixelMap
	overlay.next = NULL;
	overlay.data = gOverlayData;
	overlay.rowBytes = gProxyRect.right - gProxyRect.left;
	overlay.colBytes = 1;
	overlay.r = 0;
	overlay.g = 200;
	overlay.b = 0;
	overlay.opacity = 50;
	overlay.overlayAlgorithm = kInvertedAlphaOverlay;
	
	pixels.pixelOverlays = &overlay;
	pixels.colorManagementOptions = kViewAsStandardRGB;

	if (gMaskData) 
	{
		mask.next = NULL;
		mask.maskData = gMaskData;
		mask.rowBytes = gProxyRect.right - gProxyRect.left;
		mask.colBytes = 1;
		mask.maskDescription = kSimplePSMask;
	
		pixels.masks = &mask;
	}
	
	(gStuff->displayPixels)(&pixels, &pixels.bounds, itemBounds.top, itemBounds.left, (void *)hDC);

	EndPaint(hDlg, (LPPAINTSTRUCT) &ps);
}

/*****************************************************************************/
// Because we want to wait until the user changes the focus
// from an EditText field to something else, and because we
// want to ignore validation if the user Cancels, this routine
// does round of validating fields if it's been flagged.

Boolean Validate (GPtr globals, const HWND hDlg, int *item)
{
	int32	x = 0;
	int32	min,max;
	Boolean retn = true;
	Boolean validItem = false;
	short	numberErr = noErr;
	RECT	imageRect;

	if (*item == kDVertical)
	{
		validItem = true;
		min = gDocDesc->bounds.top;
		max = gDocDesc->bounds.bottom;
	}
	else if (*item == kDHorizontal)
	{
		validItem = true;
		min = gDocDesc->bounds.left;
		max = gDocDesc->bounds.right;
	}
	if (validItem)
	{
		numberErr = FetchNumber(hDlg, 
							   *item, 
							   min, 
							   max, 
							   &x);

		if (numberErr != noErr)
		{ // shows alert if there's an error
			AlertNumber(hDlg,
						*item,
						min,
						max,
						&x,
						AlertID,
						numberErr);
			retn = false; // had to pop an error, stay here
		}

		if (*item == kDVertical)
		{
			gPointV = (short)x;
		}
		else if (*item == kDHorizontal)
		{
			gPointH = (short)x;
		}

		// Do Filtering operation
		UpdateProxyView(globals);

		// Invalidate Proxy Item
		GetWindowRect(GetDlgItem(hDlg, kDProxyItem), &imageRect);
		ScreenToClient (hDlg, (LPPOINT)&imageRect);
		ScreenToClient (hDlg, (LPPOINT)&(imageRect.right));
		InvalidateRect (hDlg, &imageRect, FALSE);
	}
	*item = 0;
	return retn;
}

/*****************************************************************************/

DLLExport BOOL WINAPI PoorMansTypeToolProc(HWND hDlg, UINT wMsg, WPARAM wParam, LPARAM lParam)
{
	// set up some local variables, use these until OK is pressed and then
	// copy in to global data
	static short	lastV = 0;
	static short	lastH = 0;
	static int32	lastXFactor = 4;
	static BOOL		lastGaussianBlurData = FALSE;
	static bool		lastAllLayerData = FALSE;
	static BOOL		lastDiffLayerData = FALSE;
	static int16	lastLayerIndex = 0;
	RECT			imageRect;
	int				item, cmd;
	short			numberErr = 0;
	int32			x = 0;
	static int		lastItem = 0;
	static GPtr		globals = NULL;		  /* need to be static */

	switch  (wMsg)
	{

		case  WM_INITDIALOG:

			/* set up globals	*/

			globals = (GPtr) lParam;

			CenterDialog(hDlg);
			
			lastV = gPointV;
			lastH = gPointH;
			lastXFactor = gXFactor;
			lastGaussianBlurData = gGaussianBlurData;
			lastAllLayerData = gViewAllLayerData;
			lastLayerIndex = gViewLayerIndex;

			GetProxyItemRect(hDlg, &gProxyRect);
			CalcProxyScaleFactor(&gDocDesc->bounds, &gProxyRect, &gScaleFactor);
			UpdateProxyView(globals);

			StuffNumber(hDlg, kDVertical,(int32)lastV);
			StuffNumber(hDlg, kDHorizontal,(int32)lastH);

			InitializeMenu(globals, hDlg, kDSize);
			SetSizePopUpMenu(hDlg, lastXFactor);

			InitializeMenu(globals, hDlg, kDLayersPopUp);
			SendMessage(GetDlgItem(hDlg, kDLayersPopUp), CB_SETCURSEL, 0, 0);

			InitializeMenu(globals, hDlg, kDDocPopUp);
			SendMessage(GetDlgItem(hDlg, kDDocPopUp), CB_SETCURSEL, 0, 0);

			SetCheckBoxState(hDlg, kDAllLayers, lastAllLayerData);

			SetCheckBoxState(hDlg, kDGaussianBlurData, lastGaussianBlurData);

			ShowHideItem(hDlg, kDWarningText, false);

			SelectTextItem(hDlg, kDVertical);

			/* drop into PAINT missing break is intentional */
		case WM_PAINT:
			ShowOutputBuffer(globals, hDlg);	 // Draw Proxy Item
			return FALSE;
			break;

		case  WM_COMMAND:
	  		item = COMMANDID (wParam);              // WIN32 Change
			cmd = HIWORD (wParam);

			switch  (item)
			{
				case kDVertical:
					if (Validate(globals, hDlg, &lastItem))
					{
						if (cmd == EN_KILLFOCUS)
							lastItem = kDVertical; // validate will get this
						else if (cmd == EN_CHANGE)
						{
							numberErr = FetchNumber(hDlg,
													kDVertical,
													gDocDesc->bounds.top,
													gDocDesc->bounds.bottom,
													&x);
							if (numberErr == noErr && x != gPointV)
							{ // different number, update
								gPointV = (short)x;

								// Do Filtering operation
								UpdateProxyView(globals);

								// Invalidate Proxy Item
								GetWindowRect(GetDlgItem(hDlg, kDProxyItem), &imageRect);
								ScreenToClient (hDlg, (LPPOINT)&imageRect);
								ScreenToClient (hDlg, (LPPOINT)&(imageRect.right));
								InvalidateRect (hDlg, &imageRect, FALSE);
							}
						}
					}
					break;

				case kDHorizontal:
					if (Validate(globals, hDlg, &lastItem))
					{
						if (cmd == EN_KILLFOCUS)
							lastItem = kDHorizontal; // validate will get this
						else if (cmd == EN_CHANGE)
						{
							numberErr = FetchNumber(hDlg,
													kDHorizontal,
													gDocDesc->bounds.left,
													gDocDesc->bounds.right,
													&x);
							if (numberErr == noErr && x != gPointH)
							{ // different number, update
								gPointH = (short)x;

								// Do Filtering operation
								UpdateProxyView(globals);

								// Invalidate Proxy Item
								GetWindowRect(GetDlgItem(hDlg, kDProxyItem), &imageRect);
								ScreenToClient (hDlg, (LPPOINT)&imageRect);
								ScreenToClient (hDlg, (LPPOINT)&(imageRect.right));
								InvalidateRect (hDlg, &imageRect, FALSE);
							}
						}
					}
					break;

				case kDSize:
					if (Validate(globals, hDlg, &lastItem))
					{
						if (cmd == CBN_SELENDOK)
						{
							gXFactor = GetSizePopUpMenu(hDlg);
							// Do Filtering operation
							UpdateProxyView(globals);
			
							// Invalidate Proxy Item
							GetWindowRect(GetDlgItem(hDlg, kDProxyItem), &imageRect);
							ScreenToClient (hDlg, (LPPOINT)&imageRect);
							ScreenToClient (hDlg, (LPPOINT)&(imageRect.right));
							InvalidateRect (hDlg, &imageRect, FALSE);
							return FALSE;
						}
					}
					break;

				case kDGaussianBlurData:
					if (Validate(globals, hDlg, &lastItem))
					{
						if (cmd == BN_CLICKED)
						{	
							gGaussianBlurData = !gGaussianBlurData;
							SetCheckBoxState (hDlg, kDGaussianBlurData, gGaussianBlurData);
							
							// Do Filtering operation
							UpdateProxyView(globals);
			
							// Invalidate Proxy Item
							GetWindowRect(GetDlgItem(hDlg, kDProxyItem), &imageRect);
							ScreenToClient (hDlg, (LPPOINT)&imageRect);
							ScreenToClient (hDlg, (LPPOINT)&(imageRect.right));
							InvalidateRect (hDlg, &imageRect, FALSE);
							return FALSE;
						}
					}
					break;

				case kDAllLayers:
					if (Validate(globals, hDlg, &lastItem))
					{
						if (cmd == BN_CLICKED)
						{	
							gViewAllLayerData = !gViewAllLayerData;
							SetCheckBoxState (hDlg, kDAllLayers, gViewAllLayerData);
							ShowHideItem(hDlg, kDWarningText, gViewAllLayerData);
							
							// Do Filtering operation
							UpdateProxyView(globals);
			
							// Invalidate Proxy Item
							GetWindowRect(GetDlgItem(hDlg, kDProxyItem), &imageRect);
							ScreenToClient (hDlg, (LPPOINT)&imageRect);
							ScreenToClient (hDlg, (LPPOINT)&(imageRect.right));
							InvalidateRect (hDlg, &imageRect, FALSE);
							return FALSE;
						}
					}
					break;
				
				case kDLayersPopUp:
					if (Validate(globals, hDlg, &lastItem))
					{
						if (cmd == CBN_SELENDOK)
						{
							HWND hWnd = GetDlgItem(hDlg, kDLayersPopUp);
							if (hWnd == NULL) break;
							gViewLayerIndex = (int16)SendMessage(hWnd, CB_GETCURSEL, 0, 0);

							if (gViewLayerIndex == 0)
							{
								SetCheckBoxState(hDlg, kDAllLayers, gViewAllLayerData);
								PIEnableControl(hDlg, kDAllLayers);
							}
							else
							{
								gViewAllLayerData = false;
								SetCheckBoxState(hDlg, kDAllLayers, false);
								PIDisableControl(hDlg, kDAllLayers);
							}

							// Do Filtering operation
							UpdateProxyView(globals);
			
							// Invalidate Proxy Item
							GetWindowRect(GetDlgItem(hDlg, kDProxyItem), &imageRect);
							ScreenToClient (hDlg, (LPPOINT)&imageRect);
							ScreenToClient (hDlg, (LPPOINT)&(imageRect.right));
							InvalidateRect (hDlg, &imageRect, FALSE);
							return FALSE;
						}
					}
					break;

				case kDDocPopUp:
					if (Validate(globals, hDlg, &lastItem))
					{
						if (cmd == CBN_SELENDOK)
						{
							HWND hWnd = GetDlgItem(hDlg, kDDocPopUp);
							if (hWnd == NULL) break;

							int16 docIndex = (int16)SendMessage(hWnd, CB_GETCURSEL, 0, 0);
							
							// point the global doc desc at the selected doc desc
							gDocDesc = gDocInfo;
							while (docIndex-- > 0 && gDocDesc != NULL)
								gDocDesc = gDocDesc->next;

							gViewLayerIndex = 0;
							gViewAllLayerData = 0;

							SetCheckBoxState(hDlg, kDAllLayers, gViewAllLayerData);
							PIEnableControl(hDlg, kDAllLayers);

							InitializeMenu(globals, hDlg, kDLayersPopUp);
							SendMessage(GetDlgItem(hDlg, kDLayersPopUp), CB_SETCURSEL, 0, 0);

							// reset the proxy stuff to use the new doc desc
							ReleaseProxyMemory(globals);
							GetProxyItemRect(hDlg, &gProxyRect);
							CalcProxyScaleFactor(&gDocDesc->bounds, &gProxyRect, &gScaleFactor);

							UpdateProxyView(globals);
			
							// Invalidate Proxy Item
							GetWindowRect(GetDlgItem(hDlg, kDProxyItem), &imageRect);
							ScreenToClient (hDlg, (LPPOINT)&imageRect);
							ScreenToClient (hDlg, (LPPOINT)&(imageRect.right));
							InvalidateRect (hDlg, &imageRect, FALSE);
							return FALSE;
						}
					}
					break;

				case OK:
					if (cmd == BN_CLICKED)
					{
						lastItem = kDVertical; // force validate
						if (Validate(globals, hDlg, &lastItem))
						{
							lastItem = kDHorizontal;
							if (Validate(globals, hDlg, &lastItem))
							{
								EndDialog(hDlg, item); 
								return TRUE;
							}
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
						gPointV = lastV;
						gPointH = lastH;
						gXFactor = lastXFactor;
						gGaussianBlurData = lastGaussianBlurData;
						gViewAllLayerData = lastAllLayerData;
						gViewLayerIndex = lastLayerIndex;
						gResult = userCanceledErr;
						EndDialog(hDlg, item);          // WIN32 change
						return TRUE;
					}
					// don't ever validate during cancel
					break;
				default:
					break;
			} // switch (item)
			break; // case WM_COMMAND
		default:
			return FALSE;
			break;
		} // switch

	return  TRUE;
}

/*****************************************************************************/
Boolean DoUI (GPtr globals)
{

	INT_PTR nResult;
	PlatformData *platform;

	platform = (PlatformData*)((FilterRecordPtr) gStuff)->platformData;

	/* Query the user for settings. */

	nResult = DialogBoxParam(GetDLLInstance(),
							 (LPSTR)"POORMANSTYPETOOLPARAM",
							 (HWND)platform->hwnd,
							 (DLGPROC)PoorMansTypeToolProc,
							 (LPARAM)globals);
	
	return (nResult == OK);

}	

void InitializeMenu(GPtr globals, HWND hDlg, int item)
{
	HWND hWnd = GetDlgItem(hDlg, item);
	if (hWnd == NULL) return;

	// clear the pupup menu
	SendMessage(hWnd, CB_RESETCONTENT, (WPARAM)0, (LPARAM)0);

	// the layers are read from the current ReadImageDocumentDesc
	// the first one in the list is the target
	// the actual layers are then added
	if (item == kDLayersPopUp)
	{
		if (gDocDesc == NULL) return;

		SendMessage(hWnd, CB_ADDSTRING, 0, (LPARAM)"Target Layer");

		ReadLayerDesc * layerDesc = gDocDesc->layersDescriptor;

		while (layerDesc != NULL)
		{
			SendMessage(hWnd, CB_ADDSTRING, 0, (LPARAM)layerDesc->name);
			layerDesc = layerDesc->next;
		}
	}

	// For Photoshop 7 you can now get at the scrap/clipboard document
	// you can't write to it and it probably doesn't have other layers
	// at this time.
	else if (item == kDDocPopUp)
	{
		// char madeUpName[256];
		int16 counter = 1;
		
		ReadImageDocumentDesc * localDesc = gDocInfo;

		while (localDesc != NULL && gStuff->hasImageScrap)
		{
			std::ostringstream madeUpName;
			madeUpName << "DocDesc " << counter;
			SendMessage(hWnd, CB_ADDSTRING, 0, (LPARAM)madeUpName.str().c_str());
			localDesc = localDesc->next;
			counter++;
		}
	}

	// The size pop up is a static menu from the resources of this plugin
	else if (item == kDSize)
	{
		char			text[256];
		DWORD			error;
		MENUITEMINFO	mii;

		HMENU hMenu = LoadMenu(GetDLLInstance(), "POORMANSTYPETOOLMENU");
		if (hMenu == NULL)
		{
			error = GetLastError();
			return;
		}

		mii.cbSize = sizeof(MENUITEMINFO);
		mii.fMask = MIIM_SUBMENU;
		if (!GetMenuItemInfo(hMenu, 0, TRUE, &mii))
		{
			error = GetLastError();
			return;
		}

		if (mii.hSubMenu == NULL)
		{
			error = GetLastError();
			return;
		}

		hMenu = mii.hSubMenu;

		mii.fMask = MIIM_TYPE;
		mii.fType = MFT_STRING;
		mii.dwTypeData = text;
		mii.cch = sizeof(text);

		int menuItems = GetMenuItemCount(hMenu);
		if (menuItems == 0)
		{
			error = GetLastError();
			return;
		}

		for(int a = 0; a < menuItems; a++)
		{
			if (!GetMenuItemInfo(hMenu, a, TRUE, &mii))
			{
				error = GetLastError();
				return;
			}
			mii.cch = sizeof(text);
			SendMessage(hWnd, CB_ADDSTRING, 0, (LPARAM)text);
		}
	}
	else
		return; // an unknown menu id
}

// Routines and variables to help with the Size Pop up menu
// This array needs to keep in sync with the resource menu
const int32 popUpItemSize = 7;
int32 popUpItemAsIndex[popUpItemSize] = { 1, 2, 4, 8, 16, 32, 64 };

void SetSizePopUpMenu(HWND hDlg, int32 itemAsNumber)
{
	DWORD error;

	HWND hWnd = GetDlgItem(hDlg, kDSize);
	if (hWnd == NULL)
	{
		error = GetLastError();
		return;
	}

	int32 a = 0;
	for (; a < popUpItemSize; a++)
	{
		if (popUpItemAsIndex[a] == itemAsNumber)
			break;
	}

	if (a > (popUpItemSize-1))
	{
		a = 0;
	}

	SendMessage(hWnd, CB_SETCURSEL, a, 0);
}

int32 GetSizePopUpMenu(HWND hDlg)
{
	DWORD error;

	HWND hWnd = GetDlgItem(hDlg, kDSize);
	if (hWnd == NULL)
	{
		error = GetLastError();
		return 1;
	}

	LRESULT currentIndex = SendMessage(hWnd, CB_GETCURSEL, 0, 0);

	if (currentIndex > (popUpItemSize-1))
	{
		currentIndex = 0;
	}

	return (popUpItemAsIndex[currentIndex]);
}
// end PoorMansTypeToolUIWin.cpp