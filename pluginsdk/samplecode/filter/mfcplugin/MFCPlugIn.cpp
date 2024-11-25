// ADOBE SYSTEMS INCORPORATED
// Copyright  1993 - 2002 Adobe Systems Incorporated
// All Rights Reserved
//
// NOTICE:  Adobe permits you to use, modify, and distribute this
// file in accordance with the terms of the Adobe license agreement
// accompanying it.  If you have received this file from a source
// other than Adobe, then your use, modification, or distribution
// of it requires the prior written permission of Adobe.
// MFCPlugIn.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//
//	Note!
//
//		If this DLL is dynamically linked against the MFC
//		DLLs, any functions exported from this DLL which
//		call into MFC must have the AFX_MANAGE_STATE macro
//		added at the very beginning of the function.
//
//		For example:
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// normal function body here
//		}
//
//		It is very important that this macro appear in each
//		function, prior to any calls into MFC.  This means that
//		it must appear as the first statement within the 
//		function, even before any object variable declarations
//		as their constructors may generate calls into the MFC
//		DLL.
//
//		Please see MFC Technical Notes 33 and 58 for additional
//		details.
//

/////////////////////////////////////////////////////////////////////////////
// CMFCPlugInApp

BEGIN_MESSAGE_MAP(CMFCPlugInApp, CWinApp)
	//{{AFX_MSG_MAP(CMFCPlugInApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMFCPlugInApp construction

CMFCPlugInApp::CMFCPlugInApp() : m_pFilterRecord(NULL), m_pLongData(NULL), m_pData(NULL), m_Value(12)
{
	// Place all significant initialization in InitInstance
}

// this gets called every time through the main() loop so we stay in sync
void CMFCPlugInApp::Entry(FilterRecord* pFilterRecord, intptr_t* pData)
{
	if ( pFilterRecord == NULL ) throw((char*)"pFilterRecord == NULL in CMFCPlugInApp::Entry");
	m_pFilterRecord = pFilterRecord;
	m_pLongData = pData;
}

// I am not sure if I need this, every time we go back to Photoshop you could
// set some stuff up here????, I was thinking about releasing the suites and
// then reacquiring on the Entry() call, but that means lots of acquire and
// release
void CMFCPlugInApp::Exit()
{
	if ((char**)*m_pLongData != NULL)
	{
		m_pFilterRecord->handleProcs->unlockProc((Handle)*m_pLongData);
	}
}

// you could load a dialog here if you want, I went the easy/lazy route
void CMFCPlugInApp::About(void)
{
	DoMessageBox(L"About!", MB_OK, 0);
}

// this will be called the first time your plug in is ran
// if it's zero create a handle and store our default value in there
void CMFCPlugInApp::Parameters(void)
{
	char** sHandle;
	if ( *m_pLongData == NULL )
	{
		sHandle = m_pFilterRecord->handleProcs->newProc(sizeof(m_Value));
		if( sHandle == NULL ) throw((char*)"No handle created in CMFCPlugInApp::Parameters");
		m_pData = m_pFilterRecord->handleProcs->lockProc(sHandle, true);
		if ( m_pData == NULL) throw((char*)"No pointer to data in CMFCPlugInApp::Parameters");
		*m_pLongData = (intptr_t)sHandle;
		*m_pData = m_Value;
	}
}

// the plug in could start here with the Ctrl-F command, "Run Last Filter"
// you better have a valid handle to get your data out of
void CMFCPlugInApp::Prepare(void)
{
	if ( *m_pLongData == NULL )
		Parameters();
	m_pData = m_pFilterRecord->handleProcs->lockProc((char**)*m_pLongData, true);
	if ( m_pData == NULL ) throw((char*)"No pointer to data in CMFCPlugInApp::Prepare");
	m_Value = (int)*m_pData;
}

void CMFCPlugInApp::Start(void)
{
	DoUI();
}

// I should never get here. Just set the rect(s) to zero and it won't happen again.
void CMFCPlugInApp::Continue(void)
{
	m_pFilterRecord->inRect.top = 0;
	m_pFilterRecord->inRect.bottom = 0;
	m_pFilterRecord->inRect.left = 0;
	m_pFilterRecord->inRect.right = 0;
	m_pFilterRecord->outRect.top = 0;
	m_pFilterRecord->outRect.bottom = 0;
	m_pFilterRecord->outRect.left = 0;
	m_pFilterRecord->outRect.right = 0;
	m_pFilterRecord->maskRect.top = 0;
	m_pFilterRecord->maskRect.bottom = 0;
	m_pFilterRecord->maskRect.left = 0;
	m_pFilterRecord->maskRect.right = 0;
}

void CMFCPlugInApp::Finish(void)
{
}


void CMFCPlugInApp::DoUI(void)
{
	MyMFCDialog			dlg;

	dlg.m_Value = m_Value;
	dlg.m_pFilterRecord = m_pFilterRecord;
	dlg.m_pData = m_pLongData;

	INT_PTR err = dlg.DoModal();
	if (err == IDOK)
	{
		m_Value = dlg.m_Value;
		*m_pData = m_Value;
	}	
	else
	{
		throw((short)userCanceledErr);
	}
}


/////////////////////////////////////////////////////////////////////////////
// The one and only CMFCPlugInApp object
CMFCPlugInApp theApp;
SPBasicSuite * sSPBasic = NULL;
SPPluginRef	gPlugInRef = NULL;


DLLExport SPAPI void PluginMain(const int16 selector,
				FilterRecordPtr filterParamBlock,
				intptr_t* data,
				int16* result);

DLLExport SPAPI void PluginMain(const int16 selector,
				FilterRecordPtr filterParamBlock,
				intptr_t* data,
				int16* result)
{

	sSPBasic = filterParamBlock->sSPBasic;
	gPlugInRef = (SPPluginRef)filterParamBlock->plugInRef;

	try
	{
		theApp.Entry(filterParamBlock, data);

		switch( selector )
		{
			case filterSelectorAbout:
				sSPBasic = ((AboutRecordPtr)(filterParamBlock))->sSPBasic;
				gPlugInRef = (SPPluginRef)((AboutRecordPtr)(filterParamBlock))->plugInRef;
				theApp.About();
				break;
			case filterSelectorParameters:
				theApp.Parameters();
				break;
			case filterSelectorPrepare:
				theApp.Prepare();
				break;
			case filterSelectorStart:
				theApp.Start();
				break;
			case filterSelectorContinue:
				theApp.Continue();
				break;
			case filterSelectorFinish:
				theApp.Finish();
				break;
		}

		if (selector != filterSelectorAbout)
			theApp.Exit();

	}

	catch(char* inErrorString)
	{
		OutputDebugStringA(inErrorString);
		char *pErrorString = (char*)filterParamBlock->errorString;
		if (pErrorString != NULL && (strlen(inErrorString) < 256))
		{
			*pErrorString = (char)strlen(inErrorString);
			memcpy(pErrorString+1, inErrorString, (unsigned char)(*pErrorString));
		}
		*result = errReportString;

	}
	
	catch(short inError)
	{
		*result = inError;
	}
	
	catch(...)
	{
		*result = -1;
	}
}

