// ADOBE SYSTEMS INCORPORATED
// Copyright  1993 - 2002 Adobe Systems Incorporated
// All Rights Reserved
//
// NOTICE:  Adobe permits you to use, modify, and distribute this
// file in accordance with the terms of the Adobe license agreement
// accompanying it.  If you have received this file from a source
// other than Adobe, then your use, modification, or distribution
// of it requires the prior written permission of Adobe.
#if !defined(AFX_MYMFCDIALOG_H__57DC22E4_6AEA_11D3_BD7B_0060B0A13DC4__INCLUDED_)
#define AFX_MYMFCDIALOG_H__57DC22E4_6AEA_11D3_BD7B_0060B0A13DC4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MyMFCDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// MyMFCDialog dialog

class MyMFCDialog : public CDialog
{
private:
	void SetupFilterRecordForProxy (void);
	void CalcProxyScaleFactor (void);
	CRect m_ProxyRect;
	long m_ScaleFactor;

// Construction
public:
	MyMFCDialog(CWnd* pParent = NULL);   // standard constructor


	FilterRecord*	m_pFilterRecord;
	intptr_t*			m_pData;

// Dialog Data
	//{{AFX_DATA(MyMFCDialog)
	enum { IDD = IDD_DIALOG1 };
	int		m_Value;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(MyMFCDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(MyMFCDialog)
	afx_msg void OnPaint();
	virtual BOOL OnInitDialog();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MYMFCDIALOG_H__57DC22E4_6AEA_11D3_BD7B_0060B0A13DC4__INCLUDED_)
