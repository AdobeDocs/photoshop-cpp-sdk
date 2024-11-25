// ADOBE SYSTEMS INCORPORATED
// Copyright  1993 - 2002 Adobe Systems Incorporated
// All Rights Reserved
//
// NOTICE:  Adobe permits you to use, modify, and distribute this
// file in accordance with the terms of the Adobe license agreement
// accompanying it.  If you have received this file from a source
// other than Adobe, then your use, modification, or distribution
// of it requires the prior written permission of Adobe.
// MyMFCDialog.cpp : implementation file
//

#include "stdafx.h"
//#include "MFCPlugIn.h"
//#include "MyMFCDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// MyMFCDialog dialog

MyMFCDialog::MyMFCDialog(CWnd* pParent /*=NULL*/)
	: CDialog(MyMFCDialog::IDD, pParent), m_pFilterRecord(NULL), m_pData(NULL), m_ScaleFactor(0)

{
	//{{AFX_DATA_INIT(MyMFCDialog)
	m_Value = 0;
	//}}AFX_DATA_INIT
}


void MyMFCDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(MyMFCDialog)
	DDX_Text(pDX, IDC_VALUE, m_Value);
	DDV_MinMaxInt(pDX, m_Value, 10, 20);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(MyMFCDialog, CDialog)
	//{{AFX_MSG_MAP(MyMFCDialog)
	ON_WM_PAINT()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_CHAR()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// MyMFCDialog message handlers

void MyMFCDialog::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	// we need to display our current buffer in the proxy view

	PSPixelMap pixels;
	pixels.version = 1;
	pixels.bounds.top = m_pFilterRecord->outRect.top;
	pixels.bounds.left = m_pFilterRecord->outRect.left;
	pixels.bounds.bottom = m_pFilterRecord->outRect.bottom;
	pixels.bounds.right = m_pFilterRecord->outRect.right;
	pixels.imageMode = m_pFilterRecord->imageMode;
	pixels.rowBytes = m_pFilterRecord->outRowBytes;
	pixels.colBytes = m_pFilterRecord->outHiPlane 
		- m_pFilterRecord->outLoPlane + 1;
	pixels.planeBytes = 1;
	// for now it will be inData, DoFilterRect in Dissolve Sans
	// copies the inData to the outData with the effect
	// we are only showing the original for now, man that is confusing
	pixels.baseAddr = m_pFilterRecord->outData;

	pixels.mat = NULL;
	pixels.masks = NULL;
	pixels.maskPhaseRow = 0;
	pixels.maskPhaseCol = 0;

	PSPixelMask mask;
	if (m_pFilterRecord->isFloating)
	{
		mask.next = NULL;
		mask.maskData = m_pFilterRecord->maskData;
		mask.rowBytes = m_pFilterRecord->maskRowBytes;
		mask.colBytes = 1;
		mask.maskDescription = kSimplePSMask;
	}
	else if ((m_pFilterRecord->inLayerPlanes != 0) 
		&& (m_pFilterRecord->inTransparencyMask != 0))
	{
		mask.next = NULL;
		mask.maskData = m_pFilterRecord->maskData;
		mask.rowBytes = m_pFilterRecord->outRowBytes;
		mask.colBytes = m_pFilterRecord->outHiPlane - 
			m_pFilterRecord->outLoPlane + 1;
		mask.maskDescription = kSimplePSMask;

		pixels.masks = &mask;
	}

	CWnd* pWnd = GetDlgItem(IDC_PROXY);
	if (pWnd == NULL) throw((char*)"No pWnd in MyMFCDialog::OnPaint()");
	PAINTSTRUCT ps;
	pWnd->BeginPaint(&ps);

	if ((m_pFilterRecord->displayPixels)(&pixels, 
									 &pixels.bounds, 
									 m_ProxyRect.top,
									 m_ProxyRect.left,
									 (void*)ps.hdc))
		throw((char*)"DisplayPixels no go");

	pWnd->EndPaint(&ps);

	// Do not call CDialog::OnPaint() for painting messages
}

BOOL MyMFCDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// we need to read in the photoshop data for our proxy view
	CWnd* pWnd = GetDlgItem(IDC_PROXY);
	if (pWnd == NULL) throw((char*)"No pWnd in MyMFCDialog::OnInitDialog()");

	pWnd->GetClientRect(&m_ProxyRect);

	SetupFilterRecordForProxy();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void MyMFCDialog::OnMouseMove(UINT nFlags, CPoint point) 
{
	CDialog::OnMouseMove(nFlags, point);
}

void MyMFCDialog::OnLButtonDown(UINT nFlags, CPoint point) 
{
	CDialog::OnLButtonDown(nFlags, point);
}

void MyMFCDialog::OnLButtonUp(UINT nFlags, CPoint point) 
{
	CDialog::OnLButtonUp(nFlags, point);
}

void MyMFCDialog::SetupFilterRecordForProxy ()
{

	CalcProxyScaleFactor();
	
	m_pFilterRecord->inRect.top = (short)m_ProxyRect.top;
	m_pFilterRecord->inRect.left = (short)m_ProxyRect.left;
	m_pFilterRecord->inRect.bottom = (short)m_ProxyRect.bottom;
	m_pFilterRecord->inRect.right = (short)m_ProxyRect.right;
	
	m_pFilterRecord->maskRect = 
		m_pFilterRecord->outRect = m_pFilterRecord->inRect;

	m_pFilterRecord->inLoPlane = m_pFilterRecord->outLoPlane = 0;
	m_pFilterRecord->inHiPlane 
		= m_pFilterRecord->outHiPlane = m_pFilterRecord->planes - 1;

	m_pFilterRecord->inputRate = m_ScaleFactor << 16;
	m_pFilterRecord->maskRate = m_pFilterRecord->inputRate;
 
	m_pFilterRecord->inputPadding = 255;
	m_pFilterRecord->outputPadding = m_pFilterRecord->inputPadding;
	m_pFilterRecord->maskPadding = m_pFilterRecord->inputPadding;
	
	if ((*m_pFilterRecord->advanceState)())
	{
		throw((char*)"advanceState no go!");
	}

	// copy the input buffer to the output buffer
	// we will put our effect into the outData buffer for
	// proxy view updates
	memcpy(m_pFilterRecord->outData, 
		   m_pFilterRecord->inData, 
		   (m_pFilterRecord->planes 
		   * m_pFilterRecord->inRect.bottom 
		   * m_pFilterRecord->inRect.right));
}

/***********************************************************************************/

/* Computes the scaled down rectangle and the scale factor for the proxy */

void MyMFCDialog::CalcProxyScaleFactor ()
{
	// how big is the image
	long imageWidth = m_pFilterRecord->filterRect.right 
		- m_pFilterRecord->filterRect.left;
	long imageHeight = m_pFilterRecord->filterRect.bottom 
		- m_pFilterRecord->filterRect.top;

	// get the ratio of the width to the height
	long widthToHeightRatio = imageWidth / imageHeight;

	// take the longer of the two and scale the proxy
	if ( widthToHeightRatio >= 1 )
	{
		m_ScaleFactor = imageWidth / m_ProxyRect.Width();
	}
	else
	{
		m_ScaleFactor = imageHeight / m_ProxyRect.Height();
	}

	// is our proxy bigger than our image
	if ( m_ScaleFactor < 1 )
	{
		m_ScaleFactor = 1;
		m_ProxyRect.bottom = imageHeight;
		m_ProxyRect.right = imageWidth;
	}
	else
	{
		m_ScaleFactor += 1;
		m_ProxyRect.bottom = imageHeight / m_ScaleFactor;
		m_ProxyRect.right = imageWidth / m_ScaleFactor;
	}

}

void MyMFCDialog::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	CDialog::OnChar(nChar, nRepCnt, nFlags);
}

void MyMFCDialog::OnDestroy() 
{
	CDialog::OnDestroy();
}
