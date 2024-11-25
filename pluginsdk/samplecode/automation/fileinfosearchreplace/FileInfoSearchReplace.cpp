// FileInfoSearchReplace.cpp : Defines the class behaviors for the application.
//

#define USING_MFC 1

#include "FileInfoSearchReplace.h"
#include "FileInfoSearchReplaceDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFileInfoSearchReplaceApp

BEGIN_MESSAGE_MAP(CFileInfoSearchReplaceApp, CWinApp)
	//{{AFX_MSG_MAP(CFileInfoSearchReplaceApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFileInfoSearchReplaceApp construction

CFileInfoSearchReplaceApp::CFileInfoSearchReplaceApp()
{
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CFileInfoSearchReplaceApp object

CFileInfoSearchReplaceApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CFileInfoSearchReplaceApp initialization

BOOL CFileInfoSearchReplaceApp::InitInstance()
{
	AfxEnableControlContainer();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	// MSDEV 2011 told me to comment this out Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

	CFileInfoSearchReplaceDlg dlg;
	m_pMainWnd = &dlg;
	int nResponse = (int)dlg.DoModal();
	if (nResponse == IDOK)
	{
		//  dismissed with OK
	}
	else if (nResponse == IDCANCEL)
	{
		//  dismissed with Cancel
	}

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}
// end FileInfoSearchReplace.cpp