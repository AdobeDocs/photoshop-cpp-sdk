// ADOBE SYSTEMS INCORPORATED
// Copyright  1993 - 2002 Adobe Systems Incorporated
// All Rights Reserved
//
// NOTICE:  Adobe permits you to use, modify, and distribute this 
// file in accordance with the terms of the Adobe license agreement
// accompanying it.  If you have received this file from a source
// other than Adobe, then your use, modification, or distribution
// of it requires the prior written permission of Adobe.
//-------------------------------------------------------------------
/*
	File: OutboundUIWin.cpp

	Copyright 1990-1991, Thomas Knoll.
	All rights reserved.

	Windows platform dependent C source file for Export example.
*/

#include "Outbound.h"
#include "DialogUtilities.h"

/*****************************************************************************/

const uint8 kDEditText = 4;

/*****************************************************************************/

DLLExport BOOL WINAPI ExportProc(HWND hDlg, UINT wMsg, WPARAM wParam, LPARAM lParam);

/*****************************************************************************/

void DoAbout (AboutRecordPtr about)
{
	ShowAbout(about);
}

/*****************************************************************************/

Boolean DoUI (GPtr globals)
{
	char *my_lpstrFile = (char *)gStuff->filename;
	char my_filename[256];
	char my_lpstrFileTitle[256];
	char my_extension[_MAX_EXT] = ".exp";
	char my_lpstrInitialDir[256];
	char my_dir[256];
	const char *my_lpstrDefExt = "exp";
	const char *my_lpstrFilter = "Outbound export (*.exp)\0*.exp\0\0\0";
	const char *my_lpstrTitle = "Outbound export";
	PlatformData *platform;
	int nResult;
	int len = gStuff->filename[0];
	short hlen;
	OPENFILENAME lpofn;

	gStuff->filename[len+1] = 0; // make sure null terminated

	if (gAliasHandle != NULL)
	{
		/* we've been handed an aliasHandle. Use that instead of querying */
		PIHandle2PString(gAliasHandle, (unsigned char *)&my_lpstrInitialDir);
		hlen = my_lpstrInitialDir[0];

		if (my_lpstrInitialDir [hlen] == '/' || my_lpstrInitialDir [hlen] == '\\')
		{
			_splitpath((const char *)&gStuff->filename[1],
					   NULL, /* drive */
					   NULL, /* dir */
					   (Ptr)gStuff->filename,
					   NULL); /* ext */
			AppendString((unsigned char *)&my_lpstrInitialDir, gStuff->filename, 0, (short)strlen((const char *)gStuff->filename));
			AppendCharToString((unsigned char *)&my_lpstrInitialDir, '.');
			AppendString((unsigned char *)&my_lpstrInitialDir, (const unsigned char *)my_lpstrDefExt, 0, (short)strlen(my_lpstrDefExt));
			hlen = my_lpstrInitialDir[0];
		}
		else my_lpstrInitialDir [hlen+1] = 0;
		
		gStuff->filename[ (gStuff->filename[0] = 0)+1 ] = 0;
		AppendString(gStuff->filename, 
					 (const unsigned char *)&my_lpstrInitialDir, 1,
					 hlen);
		gStuff->filename[ gStuff->filename[0]+1 ] = 0;
		/* makes string pascal and null terminated */

		/* This has to be a Photoshop handle, since it was handed
		   to us by the host, so dispose it (it'll be recreated
		   later.) */
		PIDisposeHandle((Handle)gAliasHandle);
		gAliasHandle = NULL;
	}
	else
		PICopy (my_lpstrInitialDir, &gStuff->filename[1], (short)gStuff->filename[0]);
	
	_splitpath((const char *)&gStuff->filename[1],
			   my_lpstrInitialDir, /* drive */
			   my_dir, /* dir */
			   (Ptr)gStuff->filename,
			   my_extension); /* ext */
	PIstrlcat (my_lpstrInitialDir, my_dir, 255); 
	my_lpstrInitialDir[254]= '\0';
	
	platform = (PlatformData *)((ExportRecordPtr) gStuff)->platformData;

	if (!gQueryForParameters)
	{
		PIstrlcat(my_lpstrInitialDir, (const char *)gStuff->filename, 255);
		PIstrlcat(my_lpstrInitialDir, ".", 255);
		PIstrlcat(my_lpstrInitialDir, my_lpstrDefExt, 255);
		my_lpstrInitialDir[254]= '\0';
		strncpy((Ptr)gStuff->filename, my_lpstrInitialDir, 254);
		gStuff->filename[254] = '\0';
		return TRUE;
	}
	else // gQueryForParameters
	{
		/* initialize OPENFILENAME structure */

		strncpy(my_filename, (const char *)gStuff->filename, 254);
		PIstrlcat(my_filename, ".", 255);
		PIstrlcat(my_filename, my_lpstrDefExt, 255);
		my_filename[254] = '\0';

		lpofn.lStructSize = sizeof(lpofn);
		lpofn.hwndOwner = (HWND)platform->hwnd;
		lpofn.hInstance = GetDLLInstance();
		lpofn.lpstrFilter = my_lpstrFilter;
		lpofn.lpstrCustomFilter = NULL;
		lpofn.nMaxCustFilter = 0;
		lpofn.nFilterIndex = 0;
		lpofn.lpstrFile = my_lpstrFile;
		lpofn.nMaxFile = 255;
		lpofn.lpstrFileTitle = my_lpstrFileTitle;
		lpofn.nMaxFileTitle = 255;
		lpofn.lpstrInitialDir = my_lpstrInitialDir;
		lpofn.lpstrTitle = my_lpstrTitle;
		lpofn.Flags = OFN_LONGNAMES |
					  OFN_HIDEREADONLY | 
					  OFN_PATHMUSTEXIST |
					  OFN_OVERWRITEPROMPT;
		lpofn.nFileOffset = 0;
		lpofn.nFileExtension = 0;
		lpofn.lpstrDefExt = my_lpstrDefExt;
		lpofn.lCustData = 0;
		lpofn.lpfnHook = NULL;
		lpofn.lpTemplateName = NULL;
		lpofn.lStructSize = sizeof(lpofn);

		if (!(nResult = GetSaveFileName(&lpofn)))
			gResult = userCanceledErr;
		else
			gSameNames = (strncmp(my_filename, my_lpstrFileTitle, 255) == noErr);

		return (nResult);
	}
}


/*****************************************************************************/

Boolean CreateExportFile (GPtr globals)
	{
	
	OFSTRUCT ostr;
	OSErr  result;
	HFILE file;
	char *filename;

	/* Delete the file if it exists. */

	gFRefNum = 0;

	filename = (char *) gStuff->filename;
	result = OpenFile(filename,&ostr,(WORD)OF_DELETE);

	if (result == -1)
		{
		if (ostr.nErrCode != ENOENT)
			{
			gResult = openErr;
			return FALSE;
			}
		}


	/* Create a new file */

	file = OpenFile(filename,&ostr,OF_CREATE|OF_WRITE|OF_SHARE_EXCLUSIVE);

	if (file == -1)
		{
		gResult = openErr;
		return FALSE;
		}

	else
		{
		gFRefNum = file;
		return TRUE;
		}

	}
	

/*****************************************************************************/

Boolean CloseExportFile (GPtr globals)
{

	char *filename;
	char my_lpstrInitialDir[256];
	char my_dir[256];
	char my_extension[_MAX_EXT] = ".exp";
	size_t	len = 0;

	OFSTRUCT	ostr;
	
	if (gFRefNum != 0)
		_lclose(gFRefNum);

	/* Delete the file if we had an error. */

	if (gResult != noErr)
		{
		filename = (char *) gStuff->filename;
		OpenFile(filename,&ostr,(WORD)OF_DELETE);
		}
		
	/* Mark the file as clean. */
	
	gStuff->dirty = FALSE;
	
	/* Create alias for scripting system */

	strncpy(my_lpstrInitialDir, (const char *)gStuff->filename, 255);
	my_lpstrInitialDir[255] = '\0';
	
	len = strlen((const char *)gStuff->filename)+1;

	if (gSameNames)
	{
		_splitpath((const char *)gStuff->filename,
				   my_lpstrInitialDir, /* drive */
				   my_dir, /* dir */
				   (Ptr)gStuff->filename,
				   my_extension); /* ext */
		PIstrlcat(my_lpstrInitialDir, my_dir, 255);
		my_lpstrInitialDir[254] = '\0';
		len = strlen(my_lpstrInitialDir)+1;
	}
		
	my_lpstrInitialDir [len] = 0;

	strncpy((Ptr)gStuff->filename, my_lpstrInitialDir, 255); // comment out for pascal
	gStuff->filename[255] = 0; // just in case


	gAliasHandle = PINewHandle(len+1);
	if (gAliasHandle != NULL)
	{
		char * p = *gAliasHandle;
		if (p != NULL)
		{
			strncpy(p, (const char *)gStuff->filename, len);
			p[len] = '\0';
		}
	}

	return TRUE;
	
}

/*****************************************************************************/
/* Dispose alias handle */

void DestroyAliasHandle (GPtr globals)
{
	if (gAliasHandle != NULL)
		PIDisposeHandle((Handle)gAliasHandle);
	gAliasHandle = NULL;
}

/* Dialog box code to ask for export parameters. */

DLLExport BOOL WINAPI ExportProc(HWND hDlg, UINT wMsg, WPARAM wParam, LPARAM lParam)   // Win32 Change
{
	 static char *filename;
	 int idd;	// Win32 Change

    switch  (wMsg)
	{
		case WM_INITDIALOG:
            // Initialize file name with default values
			filename = (char *)lParam;
            StuffText(hDlg, kDEditText, (unsigned char *)&filename);
            CenterDialog(hDlg);

            return  FALSE;

		case WM_COMMAND   :
	  		idd = COMMANDID (wParam);              // WIN32 Change
            switch  (idd)
			{
				case OK:
					FetchText(hDlg, kDEditText, (unsigned char *)&filename);
					/* drop into cancel */
				case CANCEL:
					EndDialog(hDlg, idd);
					break;
				default:
                    return FALSE;
            }
            break;

		default:
			return  FALSE;
	}
    return  TRUE;
}

// end OutboundUIWin.cpp