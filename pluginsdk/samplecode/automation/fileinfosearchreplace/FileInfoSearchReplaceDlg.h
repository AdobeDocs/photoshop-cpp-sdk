// FileInfoSearchReplaceDlg.h : header file
//

#if !defined(AFX_FILEINFOSEARCHREPLACEDLG_H__9C523F1C_6CD4_11D5_BA97_0000861C9048__INCLUDED_)
#define AFX_FILEINFOSEARCHREPLACEDLG_H__9C523F1C_6CD4_11D5_BA97_0000861C9048__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CFileInfoSearchReplaceDlg dialog

class CFileInfoSearchReplaceDlg : public CDialog
{
// Construction
public:
	CFileInfoSearchReplaceDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CFileInfoSearchReplaceDlg)
	enum { IDD = IDD_FILEINFOSEARCHREPLACE_DIALOG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFileInfoSearchReplaceDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CFileInfoSearchReplaceDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnButtonFind();
	afx_msg void OnButtonReplace();
	afx_msg void OnButtonHideShow();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	void FindReplaceString(CString& find, CString* replace = NULL);
	BOOL AttachToPhotoshop(void);

	long GetNumberOfOpenDocuments(void);
	void GetDocumentTitle(CString& title, long index = -1);
	void GetFileInfo(CString& result, long key, long index = -1);
	void SetFileInfo(CString& result, long key);
	long GetDocumentID(long index = -1);
	void SwitchToDocument(long docID);

	IPhotoshopApplication* m_pPhotoshop;
	IActionControl* m_pActionControl;

	CString statusString;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FILEINFOSEARCHREPLACEDLG_H__9C523F1C_6CD4_11D5_BA97_0000861C9048__INCLUDED_)
