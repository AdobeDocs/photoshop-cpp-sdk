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
//-------------------------------------------------------------------------------
//
//	File:
//		Propetizer.h
//
//	Description:
//		This file contains the header prototypes and macros
//		for the Filter module Propetizer, a module that
//		examines and displays grids and guide settings,
//		then makes pseudo-resource history entries when
//		you change them.
//
//	Use:
//		This is a basic module to exemplify all the typical
//		functions a filter module will do with a special
//		emphasis on assessing properties and writing
//		pseudo-resources.
//
//-------------------------------------------------------------------------------

#ifndef __Propetizer_H__			// Has this been defined yet?
#define __Propetizer_H__			// Only include once by predefining it.

#include "PIFilter.h"				// Filter Photoshop header file.
#include "PIUtilities.h"			// SDK Utility Library.
#include "PIProperties.h"			// Properties Photoshop suite.
#include "PropetizerTerminology.h"	// Terminology for this plug-in.
#include "PropertyUtils.h"
#include <vector>
#if Macintosh
#include <string>
#endif

using namespace std;

enum {
	iNudgeH = 0,
	iNudgeV,
	iHorizontal,
	iVertical,
	iGridMajor,
	iGridMinor,
	iWatch,
	iCopyright,
	iURL,
	iWater,
	iSliceID,
	iSliceIndex,
	iXMP,
	kLastItem
};

class PropetizerData {
private:

	double fBigNudgeV;
	double fBigNudgeH;

	double fRulerOriginV;
	double fRulerOriginH;

	double fGridMajor;
	int32 fGridMinor;

	int32 fWatchSuspension;

	bool fCopyright;

	string fURL;

	bool fWatermark;

	int32 fSliceID;
	int32 fSliceIndex;

	string fXMP;

	vector<char> fChanged;


public:
	PropetizerData();
	virtual ~PropetizerData();

	int32 DoUI(void);

	OSErr Start(void);

	bool ReadScriptParameters(void);
	void WriteScriptParameters(void);

	OSErr GetProperties(void);
	OSErr SetProperties(void);

	bool OthersChanged(void);

	void SetBigNudgeH(double in) { fBigNudgeH = in; }
	double GetBigNudgeH(void) { return fBigNudgeH; }
	void SetBigNudgeV(double in) { fBigNudgeV = in; }
	double GetBigNudgeV(void) { return fBigNudgeV; }
	void SetRulerOriginH(double in) { fRulerOriginH = in; }
	double GetRulerOriginH(void) { return fRulerOriginH; }
	void SetRulerOriginV(double in) { fRulerOriginV = in; }
	double GetRulerOriginV(void) { return fRulerOriginV; }
	void SetGridMajor(double in) { fGridMajor = in; }
	double GetGridMajor(void) { return fGridMajor; }
	void SetGridMinor(int32 in) { fGridMinor = in; }
	int32 GetGridMinor(void) { return fGridMinor; }
	void SetWatchSuspension(int32 in) { fWatchSuspension = in; }
	int32 GetWatchSuspension(void) { return fWatchSuspension; }
	void SetCopyright(bool in) { fCopyright = in; }
	bool GetCopyright(void) { return fCopyright; }
	void SetURL(string in) { fURL = in; }
	string GetURL(void) { return fURL; }
	void SetWatermark(bool in) { fWatermark = in; }
	bool GetWatermark(void) { return fWatermark; }
	void SetSliceID(int32 in) { fSliceID = in; }
	int32 GetSliceID(void) { return fSliceID; }
	void SetSliceIndex(int32 in) { fSliceIndex = in; }
	int32 GetSliceIndex(void) { return fSliceIndex; }
	void SetXMP(string inXMP) { fXMP = inXMP; }
	string GetXMP(void) { return fXMP; }
};


extern FilterRecordPtr gFilterRecord;

void DoAbout(void);

string GetDocumentNameString(void);
string GetModeString(void);
string GetLayerLockingString(void);
string GetWorkPathIndexString(void);
string GetClippingPathIndexString(void);
string GetTargetPathIndexString(void);
string InterpolationModeString(void);
string RulerUnitsString(void);
string SerialString(void);
string OldSerialString(void);
string HeightWidthString(void);
string ToolTipsString(void);
string PaintCursorKindString(void);
string PaintCursorShapeString(void);
string PlayInProgressString(void);
string RecordInProgressString(void);
string LayerIsSmartString(void);
string HostVersionString(void);
string ShowSliceNumbers(void);
string SliceLineColor(void);
string GetGuideCount(void);
string ModeToString(int32 mode);
string InterpolationToString(InterpolationMethod m);
string RulerUnitsToString(RulerUnits r);
string CursorKindToString(PaintCursorKind c);
string CursorShapeToString(PaintCursorShape c);
string InterfaceColorToString(int8 color);
string RGBColorToString(RGBColor & color);
string CloudDocumentString(void);
string AccessLevelString(void);

OSErr GetLayerNames(vector<string> & vs);
OSErr GetLayerNames(vector<ps_wstring> & vws);
OSErr GetChannelNames(vector<string> & vs);
OSErr GetChannelNames(vector<ps_wstring> & vws);
OSErr GetPathNames(vector<string> & vs);
OSErr GetPathNames(vector<ps_wstring> & vws);
OSErr GetInterfaceColors(vector<string> & vs);
OSErr GetGuideInfo(vector<string> & vs);

//-------------------------------------------------------------------------------

#endif // __Propetizer_H__
