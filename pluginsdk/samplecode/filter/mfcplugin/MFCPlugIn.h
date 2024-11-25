// ADOBE SYSTEMS INCORPORATED
// Copyright  1993 - 2002 Adobe Systems Incorporated
// All Rights Reserved
//
// NOTICE:  Adobe permits you to use, modify, and distribute this
// file in accordance with the terms of the Adobe license agreement
// accompanying it.  If you have received this file from a source
// other than Adobe, then your use, modification, or distribution
// of it requires the prior written permission of Adobe.
// MFCPlugIn.h : main header file for the MFCPLUGIN DLL
//

#if !defined(AFX_MFCPLUGIN_H__57DC22DD_6AEA_11D3_BD7B_0060B0A13DC4__INCLUDED_)
#define AFX_MFCPLUGIN_H__57DC22DD_6AEA_11D3_BD7B_0060B0A13DC4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CMFCPlugInApp
// See MFCPlugIn.cpp for the implementation of this class
//

class CMFCPlugInApp : public CWinApp
{
private:
	FilterRecord*	m_pFilterRecord;
	intptr_t*		m_pLongData;
	char*			m_pData;
public:
	CMFCPlugInApp();

	void Entry(FilterRecord*,intptr_t*);
	void Exit();
	void About(void);
	void Parameters(void);
	void Prepare(void);
	void Start(void);
	void Continue(void);
	void Finish(void);

	void DoUI(void);

	int m_Value;
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMFCPlugInApp)
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CMFCPlugInApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MFCPLUGIN_H__57DC22DD_6AEA_11D3_BD7B_0060B0A13DC4__INCLUDED_)
