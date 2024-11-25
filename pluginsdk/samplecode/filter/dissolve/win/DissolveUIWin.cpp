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
//	Includes
//-------------------------------------------------------------------------------

#include "Dissolve.h"
#include "DissolveUI.h"
#include "FilterBigDocument.h"

//-------------------------------------------------------------------------------
// local routines
//-------------------------------------------------------------------------------
void GetProxyItemRect(HWND hDlg);
void PaintProxy(HWND hDlg);
void UpdateProxyItem(HWND hDlg);
void UpdateProxyItemAndRecord(HWND hDlg);
void HandleDpiChanged(HWND hDlg, WPARAM wParam, LPARAM lParam);
bool IsPerMonitorDPIEnabled();



//-------------------------------------------------------------------------------
//
// DissolveProc
//
// The Windows callback to manage our dialog box. This is a very simple
// implementation. I didn't add much error checking. There is no zooming in or
// out on the Proxy. Just a simple dialog to get some simple parameters.
// 
// NOTE:
// You must use the DLLExport macro so Windows can find this procedures address.
//
//-------------------------------------------------------------------------------
DLLExport BOOL WINAPI DissolveProc(HWND hDlg, 
								   UINT wMsg, 
								   WPARAM wParam, 
								   LPARAM lParam)
{
	BOOL returnValue = TRUE;
	int	item, cmd;
	switch (wMsg)
	{
		case WM_INITDIALOG:
			CenterDialog(hDlg);
			// recalculate the proxy and update the pixel data in inData
			UpdateProxyItemAndRecord(hDlg);

			// set the dialog items based on the last values used
			SetDlgItemInt(hDlg, kDEditText, gParams->percent, TRUE);
			CheckRadioButton(hDlg, 
				             kDFirstRadio, 
							 kDLastRadio, 
				             kDFirstRadio + gParams->disposition);
			CheckDlgButton(hDlg, kDEntireImage, gParams->ignoreSelection);
			ShowWindow(GetDlgItem(hDlg, kDEntireImage), 
				       gFilterRecord->haveMask ? SW_SHOW : SW_HIDE );
			SendDlgItemMessage(hDlg, kDEditText, EM_SETSEL, 0, (LPARAM)-1);

			SetFocus(GetDlgItem(hDlg, kDEditText));

			// intentional fall through to the WM_PAINT message
		case WM_PAINT:
			if (*gResult == noErr)
				PaintProxy(hDlg);
			returnValue = FALSE;
			break;

		case WM_COMMAND:
			item = LOWORD (wParam);
			cmd = HIWORD (wParam);
			switch (item)
			{
				case kDOK:
				case kDCancel:
					if (cmd == BN_CLICKED)
					{
						DeleteProxyBuffer();
						DeleteDissolveBuffer();
						EndDialog(hDlg, item);
						returnValue = TRUE;
					}
					break;
				case kDEditText:
					if (cmd == EN_CHANGE)
					{
						gParams->percent = (short)GetDlgItemInt(hDlg, 
							                                    kDEditText, 
																NULL, 
																TRUE);
						UpdateProxyItem(hDlg);
					}
					break;
				case kDEntireImage:
					if (cmd == BN_CLICKED)
					{
						gParams->ignoreSelection = !gParams->ignoreSelection;
						CheckDlgButton(hDlg, 
							           kDEntireImage, 
									   gParams->ignoreSelection);
						UpdateProxyItem(hDlg);
					}
					break;
				default:
					if (item >= kDFirstRadio && item <= kDLastRadio)
					{
						if (cmd == BN_CLICKED)
						{
							CheckRadioButton(hDlg, 
								             kDFirstRadio, 
											 kDLastRadio, 
											 COMMANDID(wParam)); 
							CopyColor(gData->color, 
								      gData->colorArray[item - kDFirstRadio]);
							gParams->disposition = (short)(wParam - kDFirstRadio);
							UpdateProxyItem(hDlg);
						}
					}
					break;
			}
			returnValue = TRUE;
			break;

		case WM_DPICHANGED:
			HandleDpiChanged(hDlg, wParam, lParam);
			break;
		default:
			returnValue = FALSE;
			break;
	}
	return returnValue;
}



//-------------------------------------------------------------------------------
//
// DoUI
//
// Pop the UI and return true if the last item was the OK button.
// 
//-------------------------------------------------------------------------------
Boolean DoUI(void)
{
	PlatformData* platform = (PlatformData*)(gFilterRecord->platformData);

	INT_PTR result = DialogBoxParam(GetDLLInstance(),
							(LPSTR)"DISSOLVEDIALOG",
							(HWND)platform->hwnd,
							(DLGPROC)DissolveProc,
							NULL);
	//used by the DissolveProc routine, don't let the error go further
	*gResult = noErr;
	return (result == kDOK);
}



//-------------------------------------------------------------------------------
//
// UpdateProxyItem
//
// Force the WM_PAINT message to the DissolveProc routine.
//
//-------------------------------------------------------------------------------
void UpdateProxyItem(HWND hDlg)
{
	RECT imageRect;

	if (*gResult == noErr)
	{
		ResetProxyBuffer();
		UpdateProxyBuffer();
		GetWindowRect(GetDlgItem(hDlg, kDProxyItem), &imageRect);
		ScreenToClient(hDlg, (LPPOINT)&imageRect);
		ScreenToClient(hDlg, (LPPOINT)&(imageRect.right));
		InvalidateRect(hDlg, &imageRect, FALSE);
	}
}

//-------------------------------------------------------------------------------
//
// UpdateProxyItemAndRecord
//
// Update ProxyItem size and plugin's global record.
//
//-------------------------------------------------------------------------------
void UpdateProxyItemAndRecord(HWND hDlg)
{
	GetProxyItemRect(hDlg);
	SetupFilterRecordForProxy();
	CreateProxyBuffer();
	CreateDissolveBuffer(gData->proxyWidth, gData->proxyHeight);
	UpdateProxyItem(hDlg);
}


//-------------------------------------------------------------------------------
//
// GetProxyItemRect
//
// Get the size of the proxy into the proxyRect variable. In client cordinates.
// 
//-------------------------------------------------------------------------------
void GetProxyItemRect(HWND hDlg)
{
	RECT	wRect;
	
	GetWindowRect(GetDlgItem(hDlg, kDProxyItem), &wRect);	
	ScreenToClient (hDlg, (LPPOINT)&wRect);
	ScreenToClient (hDlg, (LPPOINT)&(wRect.right));

	gData->proxyRect.left = (short)wRect.left;
	gData->proxyRect.top  = (short)wRect.top;
	gData->proxyRect.right = (short)wRect.right;
	gData->proxyRect.bottom = (short)wRect.bottom;
}



//-------------------------------------------------------------------------------
//
// PaintProxy
//
// Paint the proxy rectangle using the displayPixels() call back in the
// gFilterRecord.
//
// NOTE:
// 16 bit pixel data does not work for the current version of Photoshop. You
// scale all of the pixel data down to 8 bit values and then display. I din't go
// through that exercise.
// 
//-------------------------------------------------------------------------------
void PaintProxy(HWND hDlg)
{
	PSPixelMap pixels;
	PSPixelMask mask;
	PAINTSTRUCT  ps;
	VRect  itemBounds;
	RECT  wRect;
	POINT	mapOrigin; 
	HDC		hDC;

	// find the proxy in screen cordinates and center the
	// proxy rectangle
	GetWindowRect(GetDlgItem(hDlg, kDProxyItem), &wRect);
	mapOrigin.x = 0;
	mapOrigin.y = 0;
	ClientToScreen(hDlg, &mapOrigin);

	VRect inRect = GetInRect();

	itemBounds.left = (((wRect.right + wRect.left - 
		                        inRect.right + 
								inRect.left) / 
								2) - mapOrigin.x);
	itemBounds.top = (((wRect.bottom + wRect.top - 
		                       inRect.bottom + 
							   inRect.top) / 
							   2) - mapOrigin.y);
	itemBounds.right = (itemBounds.left + 
		                       (inRect.right - 
							   inRect.left));
	itemBounds.bottom = (itemBounds.top + 
		                        (inRect.bottom - 
								inRect.top));
	
	hDC = BeginPaint(hDlg, &ps);	

	wRect.left = itemBounds.left;
	wRect.top = itemBounds.top;
	wRect.right = itemBounds.right;
	wRect.bottom = itemBounds.bottom;

	// paint the black frame with a one pixel space
	// between the image and the frame
	InflateRect(&wRect, 2, 2);
	FrameRect(hDC, &wRect, (HBRUSH)GetStockObject(BLACK_BRUSH));	
	InflateRect(&wRect, -2, -2);
	
	// init the PSPixel map
	pixels.version = 1;
	pixels.bounds.top = inRect.top;
	pixels.bounds.left = inRect.left;
	pixels.bounds.bottom = inRect.bottom;
	pixels.bounds.right = inRect.right;
	pixels.imageMode = DisplayPixelsMode(gFilterRecord->imageMode);
	pixels.rowBytes = gData->proxyWidth;
	pixels.colBytes = 1;
	pixels.planeBytes = gData->proxyPlaneSize;
	pixels.baseAddr = gData->proxyBuffer;

	pixels.mat = NULL;
	pixels.masks = NULL;
	pixels.maskPhaseRow = 0;
	pixels.maskPhaseCol = 0;

	// display the transparency information if it exists
	if (gFilterRecord->isFloating != NULL) 
	{
		mask.next = NULL;
		mask.maskData = gFilterRecord->maskData;
		mask.rowBytes = gFilterRecord->maskRowBytes;
		mask.colBytes = 1;
		mask.maskDescription = kSimplePSMask;
	
		pixels.masks = &mask;
	} 
	else if ((gFilterRecord->inLayerPlanes != 0) && 
		       (gFilterRecord->inTransparencyMask != 0)) 
	{
		mask.next = NULL;
		mask.maskData = ((int8 *) gData->proxyBuffer) + 
			            gData->proxyPlaneSize *
						CSPlanesFromMode(gFilterRecord->imageMode, 0);
		mask.rowBytes = gData->proxyWidth;
		mask.colBytes = 1;
		mask.maskDescription = kSimplePSMask;
	
		pixels.masks = &mask;
	}

	(gFilterRecord->displayPixels)(&pixels, 
		                           &pixels.bounds, 
								   itemBounds.top, 
								   itemBounds.left, 
								   (void*)hDC);

	EndPaint(hDlg, (LPPAINTSTRUCT) &ps);
}



//-------------------------------------------------------------------------------
//
// DoAbout
//
// Pop a simple about box for this plug in.
//
// NOTE:	The global gFilterRecord is NOT a FilterRecord*. You must cast it to
//			an AboutRecord*. See PIAbout.h
//
//-------------------------------------------------------------------------------
void DoAbout(void)
{
	AboutRecord* aboutRecord = (AboutRecord*)gFilterRecord;
	ShowAbout(aboutRecord);
}


//-------------------------------------------------------------------------------
//
// HandleDpiChanged
//
// Do any handling required for the DPI change event like font, resource, windows scaling etc.
// 
//-------------------------------------------------------------------------------
void HandleDpiChanged(HWND hWnd, WPARAM /* wParam */, LPARAM /* lParam */)
{
	//If Per Monitor DPI scaling is not supported do nothing.
	if (IsPerMonitorDPIEnabled() == false)
		return;

	//Keeping this code as a reference in case we want to custom scale any of the controls.
	//float toScale(LOWORD(wParam) / (float)USER_DEFAULT_SCREEN_DPI);
	
	//Resize the window as per new dimensions provided by the OS
	//on the new scaling factor.
	//Note: Since this is a dialog, it automatically gets scaled with V2 awareness as per new DPI
	//RECT* const prcNewWindow = (RECT*)lParam;


	//Update controls and/or any other resources.
	UpdateProxyItemAndRecord(hWnd);
	
}


//-------------------------------------------------------------------------------
//
// IsSystemVersionSufficient
//
// Check if the OS version of system running the application is sufficient or not.
// 
//-------------------------------------------------------------------------------
bool IsSystemVersionSufficient(::DWORD inMajorVersion,
	::DWORD inMinorVersion,
	::WORD inServicePack)
{
	const ::DWORD versionType = VER_MAJORVERSION | VER_MINORVERSION | VER_SERVICEPACKMAJOR;

	::OSVERSIONINFOEXA versionInfo = {};
	versionInfo.dwOSVersionInfoSize = sizeof(versionInfo);

	versionInfo.dwMajorVersion = inMajorVersion;
	versionInfo.dwMinorVersion = inMinorVersion;
	versionInfo.wServicePackMajor = inServicePack;

	DWORDLONG dwlConditionMask = 0;
	VER_SET_CONDITION(dwlConditionMask, VER_MAJORVERSION, VER_GREATER_EQUAL);
	VER_SET_CONDITION(dwlConditionMask, VER_MINORVERSION, VER_GREATER_EQUAL);
	VER_SET_CONDITION(dwlConditionMask, VER_SERVICEPACKMAJOR, VER_GREATER_EQUAL);

	// Perform the test.

	return (TRUE == ::VerifyVersionInfo(&versionInfo,
		versionType,
		dwlConditionMask));
}


//-------------------------------------------------------------------------------
//
// class LibraryLoader
//
// Base class to load libraries.
// 
//-------------------------------------------------------------------------------
class LibraryLoader

{
public:
	LibraryLoader(LPCSTR lib, DWORD inMajorVersion, DWORD inMinorVersion, ::WORD servicePack)

		: fInstance(NULL)
	{
		if (IsSystemVersionSufficient(inMajorVersion, inMinorVersion, servicePack))
			fInstance = ::LoadLibrary(lib);
	}

	virtual ~LibraryLoader()

	{
		if (fInstance != NULL)
			::FreeLibrary(fInstance);
	}

	HINSTANCE GetInstance()

	{
		return fInstance;
	}

	bool IsLoaded()

	{
		return fInstance != NULL;
	}

private:

	LibraryLoader(const LibraryLoader&);

	HINSTANCE fInstance;
};


//-------------------------------------------------------------------------------
//
// class User10Library
//
// Class providing selective procedures of User32.dll.
// 
// Note :
// This class is used to provide Per monitor DPI awareness related procedures,
// these routines are only available on Windows 10 or above. Windows version
// below this will not have these methods.
//-------------------------------------------------------------------------------
class User10Library : public LibraryLoader

{
public:
	typedef BOOL(WINAPI *MySystemParametersInfoForDpi) (UINT uiAction, UINT uiParam, PVOID pvParam, UINT fWinIni, UINT dpi);
	typedef BOOL(WINAPI*MyAdjustWindowRectForDpi) (LPRECT lpRect, DWORD dwStyle, BOOL bMenu, DWORD dwExStyle, UINT dpi);
	typedef UINT(WINAPI *MyGetDPIForWindow) (HWND hWnd);
	typedef DPI_AWARENESS_CONTEXT(WINAPI *MySetThreadDpiAwarenessContext)(DPI_AWARENESS_CONTEXT  dpiContext);
	typedef DPI_AWARENESS_CONTEXT(WINAPI *MyGetThreadDpiAwarenessContext)();
	typedef BOOL(WINAPI *MyAreDpiAwarenessContextsEqual)(DPI_AWARENESS_CONTEXT a, DPI_AWARENESS_CONTEXT b);

	User10Library()
		: LibraryLoader("User32.dll", 10, 0, 0)
		, fSystemInfo(nullptr)
		, fAdjustWindowRect(nullptr)
		, fSetThreadDpiAwarenessContext(nullptr)
		, fGetThreadDpiAwarenessContext(nullptr)
		, fAreDpiAwarenessContextsEqual(nullptr)
		, fDPIForHWND(nullptr)
	{
		if (IsLoaded())
		{
			auto instance = GetInstance();
			fSystemInfo = reinterpret_cast <MySystemParametersInfoForDpi> (GetProcAddress(instance, "SystemParametersInfoForDpi"));
			fAdjustWindowRect = reinterpret_cast <MyAdjustWindowRectForDpi> (GetProcAddress(instance, "AdjustWindowRectExForDpi"));
			fDPIForHWND = reinterpret_cast <MyGetDPIForWindow> (GetProcAddress(GetInstance(), "GetDpiForWindow"));
			fSetThreadDpiAwarenessContext = reinterpret_cast<MySetThreadDpiAwarenessContext>(GetProcAddress(instance, "SetThreadDpiAwarenessContext"));
			fGetThreadDpiAwarenessContext = reinterpret_cast<MyGetThreadDpiAwarenessContext>(GetProcAddress(instance, "GetThreadDpiAwarenessContext"));
			fAreDpiAwarenessContextsEqual = reinterpret_cast<MyAreDpiAwarenessContextsEqual>(GetProcAddress(instance, "AreDpiAwarenessContextsEqual"));
		}
	}

	UINT GetDPIForWindow(HWND hWnd) const

	{
		if (fDPIForHWND == NULL)
			return 96;

		return (*fDPIForHWND) (hWnd);
	}

	bool HasGetDPIForWindow() const

	{
		return fDPIForHWND != nullptr;
	}

	bool AdjustWindowRectExForDpi(LPRECT lpRect, DWORD dwStyle, BOOL bMenu, DWORD dwExStyle, UINT dpi) const
	{
		return (*fAdjustWindowRect)(lpRect, dwStyle, bMenu, dwExStyle, dpi) == TRUE;
	}

	bool HasAdjustWindowRectExForDpi() const
	{
		return fAdjustWindowRect != nullptr;
	}

	DPI_AWARENESS_CONTEXT SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT dpiContext) const
	{
		DPI_AWARENESS_CONTEXT result = fSetThreadDpiAwarenessContext(dpiContext);
		return result;
	}

	bool HasSetThreadDpiAwarenessContext() const
	{
		return fSetThreadDpiAwarenessContext != nullptr;
	}

	DPI_AWARENESS_CONTEXT GetThreadDpiAwarenessContext() const
	{
		DPI_AWARENESS_CONTEXT result = fGetThreadDpiAwarenessContext();
		return result;
	}

	bool HasGetThreadDpiAwarenessContext() const
	{
		return fGetThreadDpiAwarenessContext != nullptr;
	}

	bool AreDpiAwarenessContextsEqual(DPI_AWARENESS_CONTEXT a, DPI_AWARENESS_CONTEXT b) const
	{
		return fAreDpiAwarenessContextsEqual(a, b) != FALSE;
	}

	bool HasAreDpiAwarenessContextsEqual() const
	{
		return fAreDpiAwarenessContextsEqual != nullptr;
	}

private:

	MySystemParametersInfoForDpi fSystemInfo;
	MyAdjustWindowRectForDpi fAdjustWindowRect;
	MyGetDPIForWindow fDPIForHWND;
	MySetThreadDpiAwarenessContext fSetThreadDpiAwarenessContext;
	MyGetThreadDpiAwarenessContext fGetThreadDpiAwarenessContext;
	MyAreDpiAwarenessContextsEqual fAreDpiAwarenessContextsEqual;
};


//-------------------------------------------------------------------------------
//
// IsPerMonitorDPIEnabled
//
// Check if Per monitor DPI is enabled for the thread.
// 
// Note:
// This property is control by the main application. To enable it set 
// MonitorScalingAware property in the resource (.r) file.
//-------------------------------------------------------------------------------
bool IsPerMonitorDPIEnabled()
{
	static bool sChecked = false;
	static bool sIsEnabled = false;

	if (!sChecked)
	{
		sChecked = true;

		User10Library userLib;

		// We won't be loaded unless we are at least Windows 10

		if (userLib.IsLoaded() && userLib.HasSetThreadDpiAwarenessContext())
		{
			DPI_AWARENESS_CONTEXT rv = userLib.GetThreadDpiAwarenessContext();

			if (userLib.AreDpiAwarenessContextsEqual(rv, DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2))
				sIsEnabled = true;

		}
	}

	return sIsEnabled;
}

// end DissolveUIWin.cpp
