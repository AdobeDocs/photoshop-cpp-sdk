// FileInfoSearchReplace.h : main header file for the FILEINFOSEARCHREPLACE application
//

#if !defined(AFX_FILEINFOSEARCHREPLACE_H__9C523F1A_6CD4_11D5_BA97_0000861C9048__INCLUDED_)
#define AFX_FILEINFOSEARCHREPLACE_H__9C523F1A_6CD4_11D5_BA97_0000861C9048__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdisp.h>        // MFC Automation classes
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT


#include "pstypelibrary.h"
#include "PITerminology.h"
#include "PIStringTerminology.h"
// I can't #include "TypeLibrary.h"
// because my C++ wrappers have redefined the same interfaces
// the only thing I need is
EXTERN_C const CLSID CLSID_PhotoshopApplication;

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CFileInfoSearchReplaceApp:
// See FileInfoSearchReplace.cpp for the implementation of this class
//

class CFileInfoSearchReplaceApp : public CWinApp
{
public:
	CFileInfoSearchReplaceApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFileInfoSearchReplaceApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CFileInfoSearchReplaceApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FILEINFOSEARCHREPLACE_H__9C523F1A_6CD4_11D5_BA97_0000861C9048__INCLUDED_)
