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
	File: PropetizerUI.cpp
*/

#include <vector>
#include <sstream>
#include "PropertyUtils.h"
#include "Propetizer.h"
#include "PIUI.h"
#include "Logger.h"

typedef string (* StringFunc)(void);

typedef struct ItemAndFunc {
    const char * text;
	StringFunc func;
} ItemAndFunc;

typedef OSErr (* VStringFunc)(vector<string> & vs);

typedef struct ComboAndFunc {
	const char * text;
	VStringFunc func;
} ComboAndFunc;


#if !__LP64__

class PropetizerDialog : public PIDialog {
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

	int32 fiLayerNames;
	int32 fiPathNames;
	int32 fiBigNudgeV;
	int32 fiBigNudgeH;
	int32 fiRulerOriginV;
	int32 fiRulerOriginH;
	int32 fiGridMajor;
	int32 fiGridMinor;
	int32 fiWatchSuspension;
	int32 fiCopyright;
	int32 fiURL;
	int32 fiWatermark;
	int32 fiSliceID;
	int32 fiSliceIndex;
	int32 fiXMP;


	virtual void Init(void);
	virtual void Notify(int32 index);
public:
	PropetizerDialog() : PIDialog(), fBigNudgeV(0.0), fBigNudgeH(0.0), 
		fRulerOriginV(0.0), fRulerOriginH(0.0), fGridMajor(0.0), fGridMinor(0),
		fWatchSuspension(0), fCopyright(false), fWatermark(false), fSliceID(0),
		fSliceIndex(0), fiLayerNames(0), fiPathNames(0), fiBigNudgeV(0), 
		fiBigNudgeH(0), fiRulerOriginV(0), fiRulerOriginH(0), fiGridMajor(0), 
		fiGridMinor(0), fiWatchSuspension(0), fiCopyright(0), fiURL(0), 
		fiWatermark(0), fiSliceID(0), fiSliceIndex(0), fiXMP(0)
		{}
	~PropetizerDialog() {}

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

typedef string (* EStringFunc)(PropetizerDialog & pd);

string BigNudgeHString(PropetizerDialog & pd);
string BigNudgeVString(PropetizerDialog & pd);
string RulerOriginHeightString(PropetizerDialog & pd);
string RulerOriginWidthString(PropetizerDialog & pd);
string GridMajorString(PropetizerDialog & pd);
string GridMinorString(PropetizerDialog & pd);
string WatchSuspensionString(PropetizerDialog & pd);
string SliceIDString(PropetizerDialog & pd);
string SliceIndexString(PropetizerDialog & pd);

typedef struct EditAndFunc {
	const char * text;
	EStringFunc func;
} EditAndFunc;

EditAndFunc const editItems[] = {
	{ "BigNudge H:", BigNudgeHString },
	{ "V:", BigNudgeVString },
	{ "Ruler Origin H:", RulerOriginHeightString },
	{ "W:", RulerOriginWidthString },
	{ "Grid Major: ", GridMajorString },
	{ "Minor: ", GridMinorString },
	{ "Watch Suspension: ", WatchSuspensionString },
	{ "Slice ID: ", SliceIDString },
	{ "Index: ", SliceIndexString },
};

typedef bool (* BoolFunc)(PropetizerDialog & pd);

bool CopyrightChecked(PropetizerDialog & pd);
bool WatermarkChecked(PropetizerDialog & pd);

typedef struct CheckAndFunc {
	const char * text;
	BoolFunc func;
} CheckAndFunc;

CheckAndFunc const checkItems[] = {
	{ "Copyright", CopyrightChecked },
	{ "Watermark", WatermarkChecked },
};

/****************************************************************************/

int32 PropetizerData::DoUI(void)
{
	PropetizerDialog dialog;
	
	dialog.SetBigNudgeH(GetBigNudgeH());
	dialog.SetBigNudgeV(GetBigNudgeV());
	dialog.SetRulerOriginH(GetRulerOriginH());
	dialog.SetRulerOriginV(GetRulerOriginV());
	dialog.SetGridMajor(GetGridMajor());
	dialog.SetGridMinor(GetGridMinor());
	dialog.SetWatchSuspension(GetWatchSuspension());
	dialog.SetCopyright(GetCopyright());
	dialog.SetURL(GetURL());
	dialog.SetWatermark(GetWatermark());
	dialog.SetSliceID(GetSliceID());
	dialog.SetSliceIndex(GetSliceIndex());
	dialog.SetXMP(GetXMP());

	int32 id = dialog.Modal(NULL, NULL, 16001);
	
	if (id == 1)
	{
		SetBigNudgeH(dialog.GetBigNudgeH());
		SetBigNudgeV(dialog.GetBigNudgeV());
		SetRulerOriginH(dialog.GetRulerOriginH());
		SetRulerOriginV(dialog.GetRulerOriginV());
		SetGridMajor(dialog.GetGridMajor());
		SetGridMinor(dialog.GetGridMinor());
		SetWatchSuspension(dialog.GetWatchSuspension());
		SetCopyright(dialog.GetCopyright());
		SetURL(dialog.GetURL());
		SetWatermark(dialog.GetWatermark());
		SetSliceID(dialog.GetSliceID());
		SetSliceIndex(dialog.GetSliceIndex());
		SetXMP(dialog.GetXMP());
	}
	return id;
}

/****************************************************************************/

void PropetizerDialog::Init(void)
{
	int16 dIndex = 3;
	int32 i = 0;

	PIText sText;

    ItemAndFunc const staticItems[] = {
        { "Serial: ", SerialString },
        { "Old Serial: ", OldSerialString },
        { "InterpolationMethod: ", InterpolationModeString },
        { "LayerLocking: ", GetLayerLockingString },
        { "Title: ",  GetDocumentNameString },
        { "ImageMode: ", GetModeString },
        { "WorkPathIndex: ", GetWorkPathIndexString },
        { "ClippingPathIndex: ", GetClippingPathIndexString },
        { "TargetPathIndex: ", GetTargetPathIndexString },
        { "RulerUnits: ", RulerUnitsString },
        { "Height, Width: ", HeightWidthString },
        { "Tool tips: ", ToolTipsString },
        { "Cursor Kind: ", PaintCursorKindString },
        { "Play in Progress: ", PlayInProgressString },
        { "Host Version: ", HostVersionString },
        { "Show Slice Numbers: ", ShowSliceNumbers },
        { "Slice Line Color: ", SliceLineColor },
        { "Layer is Smart: ", LayerIsSmartString },
        { "Guide Count: ", GetGuideCount },
        { "Cloud Document: ", CloudDocumentString },
        { "Record in Progress: ", RecordInProgressString },
		{ "Access level: ", AccessLevelString },
    };
    
	for (i = 0; i < sizeof(staticItems)/sizeof(ItemAndFunc); i++)
	{
        string s = staticItems[i].text + staticItems[i].func();
		sText = GetItem(dIndex++);
		sText.SetText(s.c_str());
	}

	// the first item for combo boxes is layer names
	fiLayerNames = dIndex + 1;
	fiPathNames = fiLayerNames + 4;

    ComboAndFunc const comboItems[] = {
        { "Layer Names: ", GetLayerNames },
        { "Channel Names: ", GetChannelNames },
        { "Path Names: ", GetPathNames },
        { "Interface Colors: ", GetInterfaceColors },
        { "Guide Info: ", GetGuideInfo },
    };

	for (i = 0; i < sizeof(comboItems)/sizeof(ComboAndFunc); i++)
	{
		sText = GetItem(dIndex++);
		sText.SetText(comboItems[i].text);
		vector<string> vs;
		(void) comboItems[i].func(vs);
		PIComboBox combo = GetItem(dIndex++);
		combo.Clear();
		if (!vs.empty())
		{
			for (size_t ii = 0; ii < vs.size(); ii++)
				combo.AppendItem(vs.at(ii).c_str());
			combo.SetCurrentSelection(0);
		}
	}

	// now we can set the selected item correctly for layer names and guide
	int32 selectedItem;
	PIComboBox comboSetter = GetItem(fiLayerNames);
	if (!PIGetTargetLayerIndex(selectedItem))
		comboSetter.SetCurrentSelection(selectedItem);

	comboSetter = GetItem(fiPathNames);
	if (!PIGetTargetPathIndex(selectedItem))
		comboSetter.SetCurrentSelection(selectedItem);

	// from here down I need to know about the item id's
	// so the notifier routine can read all the items
	// this isn't the cleanest and probably needs work
	fiBigNudgeH = dIndex + 1;
	fiBigNudgeV = fiBigNudgeH + 2;
	fiRulerOriginH = fiBigNudgeV + 2;
	fiRulerOriginV = fiRulerOriginH + 2;
	fiGridMajor = fiRulerOriginV + 2;
	fiGridMinor = fiGridMajor + 2;
	fiWatchSuspension = fiGridMinor + 2;
	fiSliceID = fiWatchSuspension + 2;
	fiSliceIndex = fiSliceID + 2;

	for (i = 0; i < sizeof(editItems)/sizeof(EditAndFunc); i++)
	{
		sText = GetItem(dIndex++);
		sText.SetText(editItems[i].text);
		sText = GetItem(dIndex++);
		sText.SetText(editItems[i].func(*this).c_str());
	}
	
	fiCopyright = dIndex;
	fiWatermark = fiCopyright + 1;

	for (i = 0; i < sizeof(checkItems)/sizeof(CheckAndFunc); i++)
	{
		PICheckBox check = GetItem(dIndex++);
		check.SetText(checkItems[i].text);
		check.SetChecked(checkItems[i].func(*this));
	}

	fiURL = dIndex;
	sText = GetItem(dIndex++);
	sText.SetText(GetURL().c_str());

	fiXMP = dIndex;
	sText = GetItem(dIndex++);
	sText.SetText(GetXMP().c_str());

}

/****************************************************************************/

void PropetizerDialog::Notify(int32 index)
{
	PIText item;
	PICheckBox check;
	string text;
	
	if (index == 1)
	{
		item = GetItem(fiBigNudgeV);
		item.GetText(text);
		SetBigNudgeV(atof(text.c_str()));

		item = GetItem(fiBigNudgeH);
		item.GetText(text);
		SetBigNudgeH(atof(text.c_str()));

		item = GetItem(fiRulerOriginV);
		item.GetText(text);
		SetRulerOriginV(atof(text.c_str()));

		item = GetItem(fiRulerOriginH);
		item.GetText(text);
		SetRulerOriginH(atof(text.c_str()));

		item = GetItem(fiGridMajor);
		item.GetText(text);
		SetGridMajor(atof(text.c_str()));

		item = GetItem(fiGridMinor);
		item.GetText(text);
		SetGridMinor(atoi(text.c_str()));

		item = GetItem(fiWatchSuspension);
		item.GetText(text);
		SetWatchSuspension(atoi(text.c_str()));

		check = GetItem(fiCopyright);
		SetCopyright(check.GetChecked());

		item = GetItem(fiURL);
		item.GetText(text);
		SetURL(text);

		item = GetItem(fiXMP);
		item.GetText(text);
		SetXMP(text);

		check = GetItem(fiWatermark);
		SetWatermark(check.GetChecked());

		item = GetItem(fiSliceID);
		item.GetText(text);
		SetSliceID(atoi(text.c_str()));

		item = GetItem(fiSliceIndex);
		item.GetText(text);
		SetSliceIndex(atoi(text.c_str()));
	
	} else if (index == fiCopyright)
	{
		check = GetItem(fiCopyright);
		bool copyright = false;
		PIGetCopyright(copyright);
		if (copyright && ! check.GetChecked())
		{
			check.SetChecked(true);
			MessageBox(GetActiveWindow(), "You can't uncheck copyright", "You can't uncheck copyright", 0);
		}

	}
}
/****************************************************************************/

string BigNudgeHString(PropetizerDialog & pd)
{
	ostringstream ss;
	ss << pd.GetBigNudgeH();
	return ss.str();
}

/****************************************************************************/

string BigNudgeVString(PropetizerDialog & pd)
{
	ostringstream ss;
	ss << pd.GetBigNudgeV();
	return ss.str();
}

/****************************************************************************/

string RulerOriginHeightString(PropetizerDialog & pd)
{
    ostringstream ss;
	ss << pd.GetRulerOriginH();
	return ss.str();
}

/****************************************************************************/

string RulerOriginWidthString(PropetizerDialog & pd)
{
	ostringstream ss;
	ss << pd.GetRulerOriginV();
	return ss.str();
}

/****************************************************************************/

string GridMajorString(PropetizerDialog & pd)
{
	ostringstream ss;
    ss << pd.GetGridMajor();
	return ss.str();
}

/****************************************************************************/

string GridMinorString(PropetizerDialog & pd)
{
	ostringstream ss;
    ss << pd.GetGridMinor();
	return ss.str();
}

/****************************************************************************/

string WatchSuspensionString(PropetizerDialog & pd)
{
	ostringstream ss;
    ss << pd.GetWatchSuspension();
	return ss.str();
}

/****************************************************************************/

string SliceIDString(PropetizerDialog & pd)
{
	ostringstream ss;
    ss << pd.GetSliceID();
	return ss.str();
}

/****************************************************************************/

string SliceIndexString(PropetizerDialog & pd)
{
	ostringstream ss;
    ss << pd.GetSliceIndex();
	return ss.str();
}

/****************************************************************************/

bool CopyrightChecked(PropetizerDialog & pd)
{
	return pd.GetCopyright();
}

/****************************************************************************/

bool WatermarkChecked(PropetizerDialog & pd)
{
	return pd.GetWatermark();
}

#endif // #if !__LP64__

#if __LP64__
int32 PropetizerData::DoUI(void)
{
    return 0;
}
#endif

//-------------------------------------------------------------------------------
//
// DoAbout
//
// Pop a simple about box for this plug in.
//
// NOTE:	The global gFilterRecord is NOT a FilterRecord*. You must cast it to
//			an AboutRecord*. See PIAbout.h
//
//-------------------------------------------------------------------------------
void DoAbout(void)
{
#if __PIWin__
	ShowAbout((AboutRecord*)gFilterRecord);
#else // #if __PIWin__
	ShowAbout(AboutID);
#endif // #if __PIWin__
}

// end PropetizerUI.cpp
