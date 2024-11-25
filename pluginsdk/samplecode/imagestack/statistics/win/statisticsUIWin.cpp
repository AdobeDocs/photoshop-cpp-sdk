#include <stddef.h>
#include <stdlib.h>


#include "PITypes.h"
#include "PIAbout.h"
#include "PIGeneral.h"

#include "statistics.h"

/* Displays the about dialog box for the plug-in module. */
extern BOOL FAR PASCAL AboutDlgProc(HWND hDlg, UINT wMsg, WPARAM wParam, LPARAM lParam);

/********************************************************************/

void DoAbout (AboutRecordPtr aboutPtr)
{
	PlatformData *platform = (PlatformData *)(aboutPtr->platformData);

	DialogBox(GetDLLInstance(), MAKEINTRESOURCE(16000),
		(HWND)platform->hwnd, (DLGPROC)AboutDlgProc);
}

/*****************************************************************************/
