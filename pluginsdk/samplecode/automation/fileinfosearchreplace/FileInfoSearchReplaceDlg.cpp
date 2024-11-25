// FileInfoSearchReplaceDlg.cpp : implementation file
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
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFileInfoSearchReplaceDlg dialog

CFileInfoSearchReplaceDlg::CFileInfoSearchReplaceDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CFileInfoSearchReplaceDlg::IDD, pParent), m_pPhotoshop(NULL), m_pActionControl(NULL)
{
	//{{AFX_DATA_INIT(CFileInfoSearchReplaceDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	// Initialize OLE libraries
	if (!AfxOleInit())
	{
		AfxMessageBox(L"Could not initialize Automation!");
	}
}

void CFileInfoSearchReplaceDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFileInfoSearchReplaceDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CFileInfoSearchReplaceDlg, CDialog)
	//{{AFX_MSG_MAP(CFileInfoSearchReplaceDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTONFIND, OnButtonFind)
	ON_BN_CLICKED(IDC_BUTTONREPLACE, OnButtonReplace)
	ON_BN_CLICKED(IDC_BUTTONHIDESHOW, OnButtonHideShow)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFileInfoSearchReplaceDlg message handlers

BOOL CFileInfoSearchReplaceDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	SetDlgItemText(IDC_STATUS, L"Ready...");
	SetDlgItemText(IDC_VERSION, L"");
	SetDlgItemText(IDC_BUTTONHIDESHOW, L"");
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CFileInfoSearchReplaceDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CFileInfoSearchReplaceDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CFileInfoSearchReplaceDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

// Photoshop accessor routines to make life easier
long CFileInfoSearchReplaceDlg::GetNumberOfOpenDocuments()
{
	long numDocuments = 0;

	if (AttachToPhotoshop())
	{
		IActionReference ref(m_pPhotoshop->MakeReference());
		ref.PutEnumerated(classApplication, typeOrdinal, enumTarget);

		IActionDescriptor resultDesc;
		m_pActionControl->GetActionProperty(ref, (IDispatch**)&resultDesc);

		long hasKey = false;
	
		resultDesc.HasKey(keyNumberOfDocuments, &hasKey);
	
		if (hasKey)
			resultDesc.GetInteger(keyNumberOfDocuments, &numDocuments);
	}
			
	return numDocuments;
}

void CFileInfoSearchReplaceDlg::SwitchToDocument(long docID)
{
	if (AttachToPhotoshop())
	{
		IActionReference docRef(m_pPhotoshop->MakeReference());
		docRef.PutIdentifier(classDocument, docID);

		IActionDescriptor docDesc(m_pPhotoshop->MakeDescriptor());
		docDesc.PutReference('null', docRef);

		m_pActionControl->Play(eventSelect, docDesc, 2);// 2 = dialog silent
	}
}

long CFileInfoSearchReplaceDlg::GetDocumentID(long index)
{
	long docID = -1;
	if (AttachToPhotoshop())
	{
		IActionReference docRef(m_pPhotoshop->MakeReference());
		docRef.PutProperty(classProperty, keyDocumentID);
		if (index == -1)
			docRef.PutEnumerated(classDocument, typeOrdinal, enumTarget);
		else
			docRef.PutIndex(classDocument, index);

		IActionDescriptor docDesc;
		m_pActionControl->GetActionProperty(docRef, (IDispatch**)&docDesc);
					
		long hasKey = false;
		docDesc.HasKey(keyDocumentID, &hasKey);

		if (hasKey)
		{
			docDesc.GetInteger(keyDocumentID, &docID);
		}
	}
	return docID;
}

void CFileInfoSearchReplaceDlg::GetDocumentTitle(CString& title, long index)
{
	if (AttachToPhotoshop())
	{
		IActionReference docRef(m_pPhotoshop->MakeReference());
		docRef.PutProperty(classProperty, keyTitle);
		if (index == -1)
			docRef.PutEnumerated(classDocument, typeOrdinal, enumTarget);
		else
			docRef.PutIndex(classDocument, index);

		IActionDescriptor docDesc;
		m_pActionControl->GetActionProperty(docRef, (IDispatch**)&docDesc);
					
		long hasKey = false;
		docDesc.HasKey(keyTitle, &hasKey);

		if (hasKey)
		{
			BSTR docTitleBSTR = NULL;
			docDesc.GetString(keyTitle, &docTitleBSTR);
			title = docTitleBSTR;
			(void) SysFreeString(docTitleBSTR);
		}
	}
}

void  CFileInfoSearchReplaceDlg::GetFileInfo(CString& result, long key, long index)
{
	if (AttachToPhotoshop())
	{
		IActionReference docRef(m_pPhotoshop->MakeReference());
		docRef.PutProperty(classProperty, keyFileInfo);
		if (index == -1)
			docRef.PutEnumerated(classDocument, typeOrdinal, enumTarget);
		else
			docRef.PutIndex(classDocument, index);

		IActionDescriptor docDesc;
		m_pActionControl->GetActionProperty(docRef, (IDispatch**)&docDesc);
					
		long hasKey = false;
		docDesc.HasKey(keyFileInfo, &hasKey);
					
		if (hasKey)
		{
			IActionDescriptor fileInfoDesc;
			long classID = 0;
			docDesc.GetObject(keyFileInfo, &classID, (IDispatch**)&fileInfoDesc);
					
			if (classID != classFileInfo) return;

			hasKey = false;
			fileInfoDesc.HasKey(key, &hasKey);

			if (hasKey)
			{
				BSTR fileInfoBSTR = NULL;
				fileInfoDesc.GetString(key, &fileInfoBSTR);
				result = fileInfoBSTR;
				(void) SysFreeString(fileInfoBSTR);
			}
		}
	}
}

void  CFileInfoSearchReplaceDlg::SetFileInfo(CString& result, long key)
{
	if (AttachToPhotoshop())
	{
		IActionReference docRef(m_pPhotoshop->MakeReference());
		docRef.PutProperty(classProperty, keyFileInfo);
		docRef.PutEnumerated(classDocument, typeOrdinal, enumTarget);

		IActionDescriptor fileInfoDesc(m_pPhotoshop->MakeDescriptor());
		fileInfoDesc.PutString(key, result);

		IActionDescriptor docDesc(m_pPhotoshop->MakeDescriptor());
		docDesc.PutReference('null', docRef);
		docDesc.PutObject(keyTo, classFileInfo, fileInfoDesc);

		IActionDescriptor resultDesc;
		resultDesc = m_pActionControl->Play(eventSet, docDesc, 2);//plugInDialogSilent
	}
}

void CFileInfoSearchReplaceDlg::OnButtonFind() 
{
	if (AttachToPhotoshop())
	{
		CString findString;
		GetDlgItemText(IDC_EDITFIND, findString);
		FindReplaceString(findString);
	}
}

void CFileInfoSearchReplaceDlg::OnButtonReplace() 
{
	if (AttachToPhotoshop())
	{
		CString findString;
		GetDlgItemText(IDC_EDITFIND, findString);
		CString replaceString;
		GetDlgItemText(IDC_EDITREPLACE, replaceString);
		FindReplaceString(findString, &replaceString);
	}
}

BOOL CFileInfoSearchReplaceDlg::AttachToPhotoshop()
{
	try 
	{
		if (m_pPhotoshop == NULL)
		{
			m_pPhotoshop = new IPhotoshopApplication;
			COleException oleException;
			if (!m_pPhotoshop->CreateDispatch(CLSID_PhotoshopApplication, &oleException))
			{
				TCHAR szCause[255];
				CString strFormatted;

				int emReturn = oleException.GetErrorMessage(szCause, 255);
				strFormatted = _T("OLE Exception error in AttachToPhotoshop CreateDispatch: ");
				strFormatted += szCause;
				SetDlgItemText(IDC_STATUS, strFormatted);
				delete m_pPhotoshop;
				m_pPhotoshop = NULL;
				return FALSE;
			}

			SetDlgItemText(IDC_VERSION, m_pPhotoshop->GetFullName());
			SetDlgItemText(IDC_BUTTONHIDESHOW, m_pPhotoshop->GetVisible() ? L"&Hide" : L"&Show");
		}

		if (m_pActionControl == NULL)
		{
			m_pActionControl = new IActionControl(m_pPhotoshop->MakeControlObject());
			if (m_pActionControl == NULL)
			{
				SetDlgItemText(IDC_STATUS, L"Could not acquire Action Control!");
				return FALSE;
			}
		}
	}

	catch(COleException* eo)
	{
	    TCHAR   szCause[255];
		CString strFormatted;

		int emReturn = eo->GetErrorMessage(szCause, 255);
		strFormatted = _T("OLE Exception error in AttachToPhotoshop: ");
		strFormatted += szCause;
		SetDlgItemText(IDC_STATUS, strFormatted);
		return FALSE;
	}
	
	catch(...)
	{
		SetDlgItemText(IDC_STATUS, L"Catch ...");
		return FALSE;
	}

	return TRUE;
}

void CFileInfoSearchReplaceDlg::FindReplaceString(CString &find, CString *replace)
{
	try
	{
		CListBox* listBox = (CListBox*)GetDlgItem(IDC_LISTRESULTS);
		listBox->ResetContent();
		listBox->UpdateWindow();

		if (AttachToPhotoshop())
		{
			long numDocuments = GetNumberOfOpenDocuments();

			statusString.Format(L"Number of Documents found %d", numDocuments);

			SetDlgItemText(IDC_STATUS, statusString);
			
			if (numDocuments > 0)
				for (long i = 1; i <= numDocuments; i++)
				{
					CString resultString = "";
					CString captionString = "";

					statusString.Format(L"Searching document %d", i);
					SetDlgItemText(IDC_STATUS, statusString);
		
					GetDocumentTitle(resultString, i);

					GetFileInfo(captionString, keyCaption, i);

					if (!captionString.IsEmpty())
						if (replace == NULL)
						{
							resultString += ":" + captionString;
							int itemIndex = listBox->AddString(resultString);
							if (captionString.Find (find) != -1)
								listBox->SetSel(itemIndex);
						}
						else
						{
							int charsReplaced = captionString.Replace (find.GetBuffer(), replace->GetBuffer());
							resultString += ":"+ captionString;
							int itemIndex = listBox->AddString(resultString);
							if (charsReplaced)
								listBox->SetSel(itemIndex);
							long docID = GetDocumentID();
							long needID = GetDocumentID(i);
							if (docID != needID)
								SwitchToDocument(needID);
							SetFileInfo(captionString, keyCaption);
							if (docID != needID)
								SwitchToDocument(docID);
						}
					else
						listBox->AddString(resultString);
					listBox->UpdateWindow();

				}
		}
	}

	catch(COleException* eo)
	{
	    TCHAR   szCause[255];
		CString strFormatted;

		int emReturn = eo->GetErrorMessage(szCause, 255);
		strFormatted = _T("OLE Exception error in FindReplaceString: ");
		strFormatted += szCause;
		SetDlgItemText(IDC_STATUS, strFormatted);
	}
	
	catch(...)
	{
		SetDlgItemText(IDC_STATUS, L"Catch ...");
	}
}

void CFileInfoSearchReplaceDlg::OnButtonHideShow() 
{
	if (AttachToPhotoshop())
	{
		TCHAR buttonText[8];
		if (m_pPhotoshop->GetVisible())
			wcsncpy(buttonText, L"&Show", 6);
		else
			wcsncpy(buttonText, L"&Hide", 6);

		m_pPhotoshop->SetVisible(!(m_pPhotoshop->GetVisible()));
		
		SetDlgItemText(IDC_BUTTONHIDESHOW, buttonText);
	}

}
