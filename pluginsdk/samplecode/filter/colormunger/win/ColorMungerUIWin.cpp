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
	File: ColorMungerUIWin.cpp

	C source file for Windows specific code for ColorMunger example.
*/

#include "ColorMunger.h"

/****************************************************************************/

const uint16 kMin = 0;
const uint16 kMax = 32767;

const uint8 kPrecision = 6;

enum
{
	dSourceRGB = 3,
	dSourceHSB,
	dSourceCMYK,
	dSourceLab,
	dSourceGray,
	dSourceHSL,
	dSourceXYZ,
	dSourcePicker
};

enum
{
	dSource1 = dSourceRGB,
	dSourceEnd = dSourceXYZ
};

enum
{
	dESource1 = 11,
	dESource2,
	dESource3,
	dESource4
};

enum
{
	dEditSource1 = dESource1,
	dEditSourceEnd = dESource4
};


const uint8	dSourceGroupItem = 15;

enum
{
	dTargetRGB = 17,
	dTargetHSB,
	dTargetCMYK,
	dTargetLab,
	dTargetGray,
	dTargetHSL,
	dTargetXYZ,
	dTargetPicker
};

enum
{
	dTarget1 = dTargetRGB,
	dTargetEnd = dTargetXYZ
};

enum
{
	dETarget1 = 25,
	dETarget2,
	dETarget3,
	dETarget4
};

enum
{
	dEditTarget1 = dETarget1,
	dEditTargetEnd = dETarget4
};

const uint8 dTargetGroupItem = 29;

short Validate (HWND dp, short *item, double *dx, Boolean useAlert);
void UpdateDialogInfo (GPtr globals, HWND dp);
DLLExport BOOL WINAPI FilterProc(HWND hDlg, UINT wMsg, WPARAM wParam, LPARAM lParam);
OSErr DoConversion (GPtr globals, HWND dp, ColorServicesInfo *csinfo);

/*****************************************************************************/

void DoAbout (AboutRecordPtr about)
{
	ShowAbout(about);
}

/*****************************************************************************/
// Loops through values and automatically grabs and validates item.  Returns
// noErr if okay, otherwise focus has been forced back to invalid item.

short Validate(HWND dp, short *item, double *dx, Boolean useAlert)
{
	short 	numberErr = noErr;
	int32 	x = 0; // returned long
	short 	loop = 1;
	
	const double propDoubles[] = {
		0, 0, 0
		/* iPropBigNudgeH, kNudgeMin, kNudgeMax,
		iPropBigNudgeV, kNudgeMin, kNudgeMax,
		iPropRulerOriginH, kOriginMin, kOriginMax,
		iPropRulerOriginV, kOriginMin, kOriginMax,
		iPropGridMajor, kGridMin, kGridMax */
		};

	const short	numPropDoubles = 
		(propDoubles[0] > 0) ? ((sizeof(propDoubles) / sizeof(double)) / 3) : 0;

	const long	propNumbers[] = {
		dESource1, kMin, kMax,
		dESource2, kMin, kMax,
		dESource3, kMin, kMax,
		dESource4, kMin, kMax,
		};

	const short	numPropNumbers = 
		(propNumbers[0] > 0) ? ((sizeof(propNumbers) / sizeof(long)) / 3) : 0;
	
	const short foundIt = -1; // flag that we found it

	
	// first loop through doubles, looking for match
	while (loop <= numPropDoubles && loop != foundIt)
	{
		if (propDoubles[loop*3-3] == *item)
		{
			numberErr = FetchDouble (dp,
									 *item,
									 propDoubles[loop*3-2], // max
									 propDoubles[loop*3-1], // min
									 dx); // returned double
			if (numberErr != noErr && useAlert)
			{ // out of bounds problem.  Report and change focus back.
				AlertDouble (dp,
							 *item,
							 propDoubles[loop*3-2], // max
							 propDoubles[loop*3-1], // min
							 dx, // value
							 AlertID, // global alert dialog
							 numberErr); // error we found
				*item = 0;
			}
			loop = foundIt; // found it!
		} else loop++; // next
	}

	if (loop != foundIt) loop = 1; // reset
	// now loop through longs, looking for match
	while (loop <= numPropNumbers && loop != foundIt)
	{
		if (propNumbers[loop*3-3] == *item)
		{
			numberErr = FetchNumber (dp,
									 *item,
									 propNumbers[loop*3-2], // max
									 propNumbers[loop*3-1], // min
									 &x); // returned long
			if (numberErr != noErr && useAlert)
			{ // out of bounds problem.  Report and change focus back.
				AlertNumber (dp,
							 *item,
							 propNumbers[loop*3-2], // max
							 propNumbers[loop*3-1], // min
							 &x, // value
							 AlertID, // global alert dialog
							 numberErr); // error we found
				*item = 0;
			}
			*dx = (double)x; // coerce long as returned double
			loop = foundIt; // found it!
		} else loop++; // next
	}
	return numberErr;
}

/*******************************************************************/
void UpdateDialogInfo (GPtr globals, HWND dp)
{
	short loop = 0;
	
	SetRadioGroupState (dp, dSource1, dSourceEnd, dSource1 + gColor);
	SetRadioGroupState (dp, dTarget1, dTargetEnd, dTarget1 + gColorize);

	for (loop = 0; loop < 4; loop++)
		StuffNumber(dp, dEditSource1 + loop, gSourceColor[loop]);
	for (loop = 0; loop < 4; loop++)
		StuffNumber(dp, dEditTarget1 + loop, gTargetColor[loop]);
}

/*******************************************************************/
OSErr DoConversion (GPtr globals, HWND dp, ColorServicesInfo *csinfo)
{
	OSErr	gotErr = noErr;
	short 	loop = 0;
	
	PopulateColorServicesInfo (globals, csinfo);
	gotErr = ColorServices (csinfo);
	gColorize = csinfo->resultSpace; // should be the same
	CSCopyColor (gTargetColor, csinfo->colorComponents);
	for (loop = 0; loop < 4; loop++)
		StuffNumber(dp, dEditTarget1 + loop, gTargetColor[loop]);

	return gotErr;
}
/*****************************************************************************/

DLLExport BOOL WINAPI FilterProc(HWND hDlg, UINT wMsg, WPARAM wParam, LPARAM lParam)       // Win32 Change
{
	long				x = 1;
	double				dx = 0;
	short				numberErr = noErr;
	short				item;
	static short		lastItem = 0, lastTitem = 0;
	static short		cColor;
	static int16		cSourceColor[4];
	static short		cColorize;
	static int16		cTargetColor[4];
	static short		gotErr = 0;
	PIType				key = NULLID;
	static ColorServicesInfo	csinfo;
	int					cmd;
	static GPtr			globals = NULL;		  /* need to be static */
	
	switch  (wMsg)
	{

		case  WM_INITDIALOG:

			/* set up globals	*/

			globals = (GPtr) lParam;

			CenterDialog(hDlg);
			
			// save globals in locals for later if user cancels
			CSCopyColor(cSourceColor, gSourceColor);
			CSCopyColor(cTargetColor, gTargetColor);
			cColor = gColor;
			cColorize = gColorize;
					
			UpdateDialogInfo(globals, hDlg);

			SelectTextItem(hDlg, dEditSource1);

			/* drop into PAINT */
		case WM_PAINT:
			// paint here
			return FALSE;
			break;

		case  WM_COMMAND:
	  		item = COMMANDID (wParam);              // WIN32 Change
			cmd = HIWORD (wParam);

			switch  (item)
			{
				case dSourcePicker:
					if ((numberErr = Validate(hDlg, &lastItem, &dx, true)) == noErr)
					{ // didn't have to change focus.  Continue.
						if (cmd == BN_CLICKED)
						{
							PopulateColorServicesInfo (globals, &csinfo);
							csinfo.selector = plugIncolorServicesChooseColor; // change
							csinfo.resultSpace = plugIncolorServicesChosenSpace; // will return user chosen
							
							if (gotErr = ColorServices (&csinfo) == noErr)
							{ // no problems.  Do this.
								gColor = csinfo.resultSpace;
								CSCopyColor (gSourceColor, csinfo.colorComponents);
								DoConversion(globals, hDlg, &csinfo);
								UpdateDialogInfo (globals, hDlg);
							}
							SelectTextItem(hDlg, lastTitem);
							return FALSE;
						}
					}
					break;
				case dTargetPicker:
					if ((numberErr = Validate(hDlg, &lastItem, &dx, true)) == noErr)
					{ // didn't have to change focus.  Continue.
						if (cmd == BN_CLICKED)
						{
							PopulateColorServicesInfo (globals, &csinfo);
							csinfo.selector = plugIncolorServicesChooseColor; // picker
							if (gotErr = ColorServices (&csinfo) == noErr)
							{ // no problems.  Do this.
								// move old target to source space
								CSCopyColor (gSourceColor, gTargetColor);
								gColor = gColorize;
								// now pull results to target
								gColorize = csinfo.resultSpace; // should be the same
								CSCopyColor (gTargetColor, csinfo.colorComponents);
								UpdateDialogInfo (globals, hDlg);
							}
							SelectTextItem(hDlg, lastTitem);
							return FALSE;
						}
					}
					break;
				case OK:
					if ((numberErr = Validate(hDlg, &lastItem, &dx, true)) == noErr)
					{ // validated fine
						if (cmd == BN_CLICKED)
						{ // wants to dismiss.  End dialog.
							EndDialog(hDlg, item); 
							return TRUE;
						}
					}
					break;
				case CANCEL:
					if (cmd == BN_CLICKED)
					{ // return all globals from saved locals
						CSCopyColor(gSourceColor, cSourceColor);
						CSCopyColor(gTargetColor, cTargetColor);
						gColor = cColor;
						gColorize = cColorize;

						gResult = userCanceledErr;
						EndDialog(hDlg, item);          // WIN32 change
						return TRUE;
					}
					// don't ever validate during cancel
					break;
				default:
					if (item >= dSource1 && item <= dSourceEnd)
					{
						if ((numberErr = Validate(hDlg, &lastItem, &dx, true)) == noErr)
						{ // validated fine
							if (cmd == BN_CLICKED)
							{ // process button
								SetRadioGroupState (hDlg, dSource1, dSourceEnd, item);
								gColor = item - dSource1;
						
								DoConversion (globals, hDlg, &csinfo);
								SelectTextItem(hDlg, lastTitem);
								return FALSE;
							}
						}
					}
					else if (item >= dTarget1 && item <= dTargetEnd)
					{ // set group then do conversion
						if ((numberErr = Validate(hDlg, &lastItem, &dx, true)) == noErr)
						{ // validated fine
							if (cmd == BN_CLICKED)
							{ // process							
								SetRadioGroupState (hDlg, dTarget1, dTargetEnd, item);
								gColorize = item - dTarget1;
								
								DoConversion (globals, hDlg, &csinfo);
								SelectTextItem(hDlg, lastTitem);
								return FALSE;
							}
						}
					}
					else if (item >= dEditSource1 && item <= dEditSourceEnd)
					{
						if ((numberErr = Validate(hDlg, &lastItem, &dx, true)) == noErr)
						{
							if (cmd == EN_KILLFOCUS)
								lastItem = item; // validate will get this
							else if (cmd == EN_CHANGE)
							{ // do me
								numberErr = Validate(hDlg, &item, &dx, false);
								gSourceColor[item - dEditSource1] = (int16)dx;
								lastTitem = item;

								DoConversion (globals, hDlg, &csinfo);
								return FALSE;
							}
						}
					}
					break;
			} // switch (item)
			break; // case WM_COMMAND
		default:
			return FALSE;
			break;
	} // switch
	return  TRUE;
}

/*****************************************************************************/
Boolean DoUI (GPtr globals)
{

	PlatformData *platform;

	platform = (PlatformData*)((FilterRecordPtr) gStuff)->platformData;

	/* Query the user for percent dissolve. */

	INT_PTR    nResult = DialogBoxParam(GetDLLInstance(),
							 (LPSTR)"COLORMUNGERPARAM",
							 (HWND)platform->hwnd,
							 (DLGPROC)FilterProc,
							 (LPARAM)globals);

	return (nResult == OK);

}	
// end ColorMungerUIWin.cpp