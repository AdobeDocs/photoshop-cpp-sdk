// ADOBE SYSTEMS INCORPORATED
// Copyright  1993 - 2003 Adobe Systems Incorporated
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
//		Propetizer.cpp
//
//	Description:
//		This file contains the routines and source
//		for the Filter module Propetizer, a module that
//		examines and displays Photoshop properties.
//
//	Use:
//		This is a basic module to exemplify all the typical
//		functions a filter module will do with a special
//		emphasis on assessing properties.
//
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
//	Includes
//-------------------------------------------------------------------------------


#include "Propetizer.h"
#include "PropertyUtils.h"
#include "Logger.h"
#include "Timer.h"
#include "PIUFile.h"
#include <vector>
#include <fstream>
#include <sstream>

void FileToHandle(const string & fileName, Handle & h);
void HandleToFile(const string & fileName, const Handle h);
void FileToString(const string & fileName, string & inString);
void StringToFile(const string & fileName, const string & outString);

void DumpAllProperties(void);
void DumpIt(ofstream & outStream, vector<string> & vs);

int16 DoStart(void);


FilterRecordPtr gFilterRecord = NULL;
SPBasicSuite * sSPBasic = NULL;
SPPluginRef gPluginRef = NULL;
char gFullPathToDesktop[MAX_PATH] = {0,0};

DLLExport MACPASCAL void PluginMain(const int16 selector,
								    FilterRecordPtr filterRecord,
								    intptr_t * data,
								    int16 * result);

//-------------------------------------------------------------------------------
//
//	PluginMain
//	
//	All calls to the plug in module come through this routine.
//
//	Inputs:
//		const int16 selector		Host provides selector indicating what
//									command to do.
//
//	Inputs and Outputs:
//		FilterRecord *filterRecord	Host provides a pointer to parameter block
//									containing pertinent data and callbacks.
//									See PIFilter.h
//
//		void *data					Use this to store a pointer to our global
//									data structure, which is maintained by the
//									host between calls to the plug in.
//
//	Outputs:
//		int16 *result				Returns error result. Some errors are handled
//									by the host, some are silent, and some you
//									must handle. See PIGeneral.h.
//
//-------------------------------------------------------------------------------
DLLExport MACPASCAL void PluginMain(const int16 selector,
								    FilterRecordPtr filterRecord,
								    intptr_t * data,
								    int16 * result)
{

	try {
		
	Logger logIt("Propetizer");
	Timer timeIt;

	logIt.Write("Selector: ", false);
	logIt.Write(selector, false);
	logIt.Write(" ", false);

	// update our global parameters
	gFilterRecord = filterRecord;
	if (selector == filterSelectorAbout)
	{
		sSPBasic = ((AboutRecord*)gFilterRecord)->sSPBasic;
	}
	else
	{
		sSPBasic = gFilterRecord->sSPBasic;

		if (gFilterRecord->bigDocumentData != NULL)
			gFilterRecord->bigDocumentData->PluginUsing32BitCoordinates = true;
	}

	// do the command according to the selector
	switch (selector)
	{
		case filterSelectorAbout:
			DoAbout();
			break;
		case filterSelectorStart:
			if (NULL != result)
				*result = DoStart();
			break;
		default:
			break;
	}

	logIt.Write( timeIt.GetElapsed(), true );
		
	} // end try

	catch (...)
	{
		if (NULL != result)
			*result = -1;
	}

}

OSErr DoStart(void)
{
	DumpAllProperties();
	PropetizerData propetizerData;
	return propetizerData.Start();
}

PropetizerData::PropetizerData()
	: fBigNudgeV(10.0), fBigNudgeH(10.0), fRulerOriginV(0.0), 
	  fRulerOriginH(0.0), fGridMajor(0.0), fGridMinor(0), 
	  fWatchSuspension(false), fCopyright(false), fURL(""), fXMP(""),
	  fWatermark(false), fSliceID(0), fSliceIndex(0)
{
	fChanged.resize(kLastItem);
	for (int i = 0; i < kLastItem; i++)
		fChanged[i] = false;
}



//-------------------------------------------------------------------------------
//
//	PropetizerData::~PropetizerData
//
//	Get rid of anything we made during this plug in life cycle.
//
//-------------------------------------------------------------------------------
PropetizerData::~PropetizerData()
{
}



//-------------------------------------------------------------------------------
//
//	PropetizerData::DoStart
//
//	This is where all the action is.
//
//-------------------------------------------------------------------------------
OSErr PropetizerData::Start(void)
{
	OSErr error = GetProperties();
	if (error) 
		return error;

	bool playInProgress = false;
	bool popDialog = true;

	if (!PIGetPlayInProgress(playInProgress))
	{
		if (playInProgress)
		{
			popDialog = ReadScriptParameters();
		}
		else
		{
			popDialog = true;
		}
	}
	else
	{
		popDialog = true;
	}
	
	if (popDialog)
	{
		if (DoUI() == 1) // the OK button
		{
			error = SetProperties();
		}
		else
		{
			error = userCanceledErr;
		}
	}
	else 
	{
		error = SetProperties();
	}

	if (!error)
	{
		WriteScriptParameters();
	}

	return error;
}



OSErr PropetizerData::GetProperties(void)
{
	OSErr e = PIGetBigNudge(fBigNudgeH, fBigNudgeV);
	if (!e)
		e = PIGetRulerOrigin(fRulerOriginH, fRulerOriginV);
	if (!e)
		e = PIGetGrid(fGridMajor, fGridMinor);
	if (!e)
		e = PIGetWatchSuspension(fWatchSuspension);
	if (!e)
		e = PIGetCopyright(fCopyright);
	if (!e)
		e = PIGetURL(fURL);
	if (!e)
		e = PIGetWatermark(fWatermark);
	if (!e)
		e = PIGetSelectedSliceID(fSliceID);
	if (!e)
		(void) PIGetSelectedSliceIndex(fSliceIndex);
	if (!e)
		e = PIGetXMP(fXMP);

	return e;
}

OSErr PropetizerData::SetProperties(void)
{
	double d1, d2;
	int32 i;
	bool b;
	string s;
	OSErr error = PIGetBigNudge(d1, d2);

	if (!error)
	{
		bool dHoriz = fabs(d1 - fBigNudgeH) > 0.00001;
		bool dVert = fabs(d2 - fBigNudgeV) > 0.00001;
		if (dHoriz && dVert)
		{
			error = PISetBigNudge(fBigNudgeH, fBigNudgeV);
			if (!error)
			{
				fChanged[iNudgeH] = true;
				fChanged[iNudgeV] = true;
			}
		}
		else if (dHoriz)
		{
			error = PISetBigNudge(fBigNudgeH, d2);
			if (!error)
				fChanged[iNudgeH] = true;
		}
		else if (dVert)
		{
			error = PISetBigNudge(d1, fBigNudgeV);
			if (!error)
				fChanged[iNudgeV] = true;
		}
	}

	error  = PIGetRulerOrigin(d1, d2);
	if (!error)
	{
		bool dHoriz = fabs(d1 - fRulerOriginH) > 0.00001;
		bool dVert = fabs(d2 - fRulerOriginV) > 0.00001;
		if (dHoriz && dVert)
		{
			error = PISetRulerOrigin(fRulerOriginH, fRulerOriginV);
			if (!error)
			{
				fChanged[iHorizontal] = true;
				fChanged[iVertical] = true;
			}
		}
		else if (dHoriz)
		{
			error = PISetRulerOrigin(fRulerOriginH, d2);
			if (!error)
				fChanged[iHorizontal] = true;
		}
		else if (dVert)
		{
			error = PISetRulerOrigin(d1, fRulerOriginV);
			if (!error)
				fChanged[iVertical] = true;
		}
	}

	error = PIGetGrid(d1, i);
	if (!error)
	{
		bool dMajor = fabs(d1 - fGridMajor) > 0.00001;
		if (dMajor && i != fGridMinor)
		{
			error = PISetGrid(fGridMajor, fGridMinor);
			if (!error)
			{
				fChanged[iGridMajor] = true;
				fChanged[iGridMinor] = true;
			}
		}
		else if (dMajor)
		{
			error = PISetGrid(fGridMajor, i);
			if (!error)
				fChanged[iGridMajor] = true;
		}
		else if (i != fGridMinor)
		{
			error = PISetGrid(d1, fGridMinor);
			if (!error)
				fChanged[iGridMinor] = true;
		}
	}

	error = PIGetWatchSuspension(i);
	if (!error)
		if (i != fWatchSuspension)
		{
			error = PISetWatchSuspension(fWatchSuspension);
			if (!error)
				fChanged[iWatch] = true;
		}

	error = PIGetCopyright(b);
	if (!error)
		if (b != fCopyright && fCopyright)
		{
			error = PISetCopyright(fCopyright);
			if (!error)
				fChanged[iCopyright] = true;
		}

	error = PIGetURL(s);
	if (!error)
		if (s.compare(fURL) != 0)
		{
			error = PISetURL(fURL);
			if (!error)
				fChanged[iURL] = true;
		}

	error = PIGetWatermark(b);
	if (!error)
		if (b != fWatermark && fWatermark)
		{
			error = PISetWatermark();
			if (!error)
				fChanged[iWater] = true;
		}

	error = PIGetSelectedSliceID(i);
	if (!error)
		if (i != fSliceID)
		{
			error = PISetSelectedSliceID(fSliceID);
			if (!error)
				fChanged[iSliceID] = true;
		}

	error = PIGetSelectedSliceIndex(i);
	if (!error)
		if (i != fSliceIndex)
		{
			error = PISetSelectedSliceIndex(fSliceIndex);
			if (!error)
				fChanged[iSliceIndex] = true;
		}

	// if you set anything above, the xmp is going to be different
	error = PIGetXMP(s);
	if (!error)
		if (s.compare(fXMP) != 0 && ! OthersChanged())
		{
			error = PISetXMP(fXMP);
			if (!error)
				fChanged[iXMP] = true;
		}

	return error;
}

bool PropetizerData::OthersChanged()
{
	for (int i = 0; i < kLastItem; i++)
		if (fChanged[i] && i != iXMP)
			return true;
	return false;
}

void FileToHandle(const string & fileName, Handle & h)
{
	h = NULL;
	vector<char> data;
	#if __PIMac__
		ifstream inFile(fileName.c_str(), ios::in|ios::binary);
	#else
		// j systems have trouble opening the file with Shift-JIS chars
		// use this trick
		ifstream inFile(fopen(fileName.c_str(), "rb"));
	#endif
	do
	{
		char c;
		inFile.read(&c, 1);
		if (inFile.gcount())
		{
			data.push_back(c);
		}
	} while (inFile.gcount());

	size_t s = data.size();
	if (s)
	{
		h = sPSHandle->New((int32)s);
		if (h != NULL)
		{
			Boolean oldLock = FALSE;
			Ptr p = NULL;
			sPSHandle->SetLock(h, true, &p, &oldLock);
			if (p != NULL)
			{
				for(size_t a = 0; a < s; a++, p++)
					*p = data.at(a);
				sPSHandle->SetLock(h, false, &p, &oldLock);
			}
			else
			{
				sPSHandle->Dispose(h);
				h = NULL;
			}
		}
	}
}

void HandleToFile(const string & fileName, const Handle h)
{
	#if __PIMac__
		ofstream outFile(fileName.c_str(), ios::out|ios::binary);
	#else
		// j systems have trouble opening the file with Shift-JIS chars
		// use this trick
		ofstream outFile(fopen(fileName.c_str(), "wb"));
	#endif
	
	int32 s = sPSHandle->GetSize(h);
	
	if (s > 0)
	{
		Boolean oldLock = FALSE;
		Ptr p = NULL;
		sPSHandle->SetLock(h, true, &p, &oldLock);
		if (p != NULL)
		{
			Ptr o = p;
			for (int32 a = 0; a < s; a++, o++)
				outFile << *o;
			sPSHandle->SetLock(h, false, &p, &oldLock);
		}
	}
}

void FileToString(const string & fileName, string & inString)
{
	#if __PIMac__
		ifstream inFile(fileName.c_str(), ios::in);
	#else
		// j systems have trouble opening the file with Shift-JIS chars
		// use this trick
		ifstream inFile(fopen(fileName.c_str(), "r"));
	#endif
	do
	{
		char c;
		inFile.read(&c, 1);
		if (inFile.gcount())
		{
			inString += c;
		}
	} while (inFile.gcount());
}

void StringToFile(const string & fileName, const string & outString)
{
	#if __PIMac__
		ofstream outFile(fileName.c_str(), ios::out);
	#else
		// j systems have trouble opening the file with Shift-JIS chars
		// use this trick
		ofstream outFile(fopen(fileName.c_str(), "w"));
	#endif

	for (uint32 i =0; i < outString.length(); i++)
		outFile << outString[i];
}

void DumpAllProperties(void)
{
	Logger logAll("PropetizerAll");

	vector<string> vs;
	OSErr e = GetChannelNames(vs);
	if (!e)
	{
		logAll.Write("GetChannelNames: ", Logger::addEndOfLine);
		logAll.Write(vs, Logger::addEndOfLine);
	}

	vector<ps_wstring> vws;
	e = GetChannelNames(vws);
	if (!e)
	{
		logAll.Write("GetChannelNamesUnicode: ", Logger::addEndOfLine);
		logAll.Write(vws, Logger::addEndOfLine);
	}

	string s = GetLayerLockingString();
	logAll.Write("TargetLayerLock: ");
	logAll.Write(s, Logger::addEndOfLine);

	s = GetModeString();
	logAll.Write("ImageMode: ");
	logAll.Write(s, Logger::addEndOfLine);

	e = GetPathNames(vs);
	if (!e)
	{
		logAll.Write("GetPathNames: ", Logger::addEndOfLine);
		logAll.Write(vs, Logger::addEndOfLine);
	}

	e = GetPathNames(vws);
	if (!e)
	{
		logAll.Write("GetPathNamesUnicode: ", Logger::addEndOfLine);
		logAll.Write(vws, Logger::addEndOfLine);
	}

    if (gFullPathToDesktop[0] == 0)
        if (GetFullPathToDesktop(gFullPathToDesktop, MAX_PATH))
            return;

	Handle h = NULL;
	int32 i = 0;
	e = PIGetNumberPaths(i);
	if (!e)
	{
		for (int32 ii = 0; ii < i; ii++)
		{
			e = PIGetPathContents(ii, h);
			if (!e && h)
			{
				ostringstream outFileStream;
				outFileStream << gFullPathToDesktop;
				outFileStream << "PropetizerPath_";
				outFileStream << ii;
				outFileStream << ".log";
				HandleToFile(outFileStream.str(), h);
				sPSHandle->Dispose(h);
			}
		}
	}

	if (!e)
	{
		for (int32 ii = 0; ii < i; ii++)
		{
			e = PIGetPathContentsAI(ii, h);
			if (!e && h)
			{
				ostringstream outFileStream;
				outFileStream << gFullPathToDesktop;
				outFileStream << "PropetizerPath_AI_";
				outFileStream << ii;
				outFileStream << ".log";
				HandleToFile(outFileStream.str(), h);
				sPSHandle->Dispose(h);
			}
		}
	}

	s = GetWorkPathIndexString();
	logAll.Write("GetWorkPathIndexString: ");
	logAll.Write(s, Logger::addEndOfLine);

	s = GetClippingPathIndexString();
	logAll.Write("GetClippingPathIndexString: ");
	logAll.Write(s, Logger::addEndOfLine);

	s = GetTargetPathIndexString();
	logAll.Write("GetTargetPathIndexString: ");
	logAll.Write(s, Logger::addEndOfLine);

	e = PIGetCaption(h);
	if (!e)
	{
		ostringstream outFileStream;
		outFileStream << gFullPathToDesktop;
		outFileStream << "PropetizerCaption";
		outFileStream << ".log";
		HandleToFile(outFileStream.str(), h);
		sPSHandle->Dispose(h);
	}

	e = PIGetXMP(s);
	if (!e)
	{
		Logger xmpLogger("PropetizerXMP");
		xmpLogger.Write(s);
	}

	double d, d2;
	e = PIGetBigNudge(d, d2);
	if (!e)
	{
		ostringstream ss;
		ss << "Horizontal Nudge: ";
		ss << d;
		ss << " Vertical Nudge: ";
		ss << d2;
		logAll.Write(ss.str(), Logger::addEndOfLine);
	}

	s = InterpolationModeString();
	logAll.Write("InterpolationModeString: ");
	logAll.Write(s, Logger::addEndOfLine);

	s = RulerUnitsString();
	logAll.Write("RulerUnitsString: ");
	logAll.Write(s, Logger::addEndOfLine);

	e = PIGetRulerOrigin(d, d2);
	if (!e)
	{
		ostringstream ss;
		ss << "RulerOriginH: ";
		ss << d;
		ss << " RulerOriginV: ";
		ss << d2;
		logAll.Write(ss.str(), Logger::addEndOfLine);
	}

	e = PIGetGrid(d, i);
	if (!e)
	{
		ostringstream ss;
		ss << "GridMajor: ";
		ss << d;
		ss << " GridMinor: ";
		ss << i;
		logAll.Write(ss.str(), Logger::addEndOfLine);
	}

	s = SerialString();
	logAll.Write("SerialString: ");
	logAll.Write(s, Logger::addEndOfLine);

	s = OldSerialString();
	logAll.Write("OldSerialString: ");
	logAll.Write(s, Logger::addEndOfLine);

#if __PIWin__
	e = PIGetHardwareGammaTable(h);
	if (!e)
	{
		ostringstream outFileStream;
		outFileStream << gFullPathToDesktop;
		outFileStream << "PropetizerHardwareGammaTable";
		outFileStream << ".log";
		HandleToFile(outFileStream.str(), h);
		sPSHandle->Dispose(h);
	}
#endif

	e = GetInterfaceColors(vs);
	if (!e)
	{
		logAll.Write("GetInterfaceColors: ");
		logAll.Write(vs, Logger::addEndOfLine);
	}

	e = PIGetWatchSuspension(i);
	if (!e)
	{
		ostringstream ss;
		ss << "PIGetWatchSuspension: ";
		ss << i;
		logAll.Write(ss.str(), Logger::addEndOfLine);
	}

	bool b;
	e = PIGetCopyright(b);
	if (!e)
	{
		logAll.Write("PIGetCopyright: ");
		logAll.Write(b ? "Yes" : "No", Logger::addEndOfLine);
	}

	e = PIGetCopyright2(b);
	if (!e)
	{
		logAll.Write("PIGetCopyright2: ");
		logAll.Write(b ? "Yes" : "No", Logger::addEndOfLine);
	}

	e = PIGetWatermark(b);
	if (!e)
	{
		logAll.Write("PIGetWatermark: ");
		logAll.Write(b ? "Yes" : "No", Logger::addEndOfLine);
	}

	e = PIGetURL(s);
	if (!e)
	{
		logAll.Write("PIGetURL: ");
		logAll.Write(s, Logger::addEndOfLine);
	}

	e = PIGetDocumentName(s);
	if (!e)
	{
		logAll.Write("PIGetDocumentName: ");
		logAll.Write(s, Logger::addEndOfLine);
	}

	s = HeightWidthString();
	logAll.Write("HeightWidthString: ");
	logAll.Write(s, Logger::addEndOfLine);

	e = PIGetSlices(h);
	if (!e)
	{
		ostringstream outFileStream;
		outFileStream << gFullPathToDesktop;
		outFileStream << "PropetizerSlices";
		outFileStream << ".log";
		HandleToFile(outFileStream.str(), h);
		sPSHandle->Dispose(h);
	}

	e = PIGetSelectedSliceID(i);
	if (!e)
	{
		ostringstream ss;
		ss << "PIGetSelectedSliceID: ";
		ss << i;
		logAll.Write(ss.str(), Logger::addEndOfLine);
	}

	e = PIGetSelectedSliceIndex(i);
	if (!e)
	{
		ostringstream ss;
		ss << "PIGetSelectedSliceIndex: ";
		ss << i;
		logAll.Write(ss.str(), Logger::addEndOfLine);
	}

	e = PIGetShowSliceNumbers(b);
	if (!e)
	{
		logAll.Write("PIGetShowSliceNumbers: ");
		logAll.Write(b ? "Yes" : "No", Logger::addEndOfLine);
	}

	s = SliceLineColor();
	logAll.Write("SliceLineColor: ");
	logAll.Write(s, Logger::addEndOfLine);

	e = PIGetToolTips(b);
	if (!e)
	{
		logAll.Write("PIGetToolTips: ");
		logAll.Write(b ? "Yes" : "No", Logger::addEndOfLine);
	}

	s = PaintCursorKindString();
	logAll.Write("PaintCursorKindString: ");
	logAll.Write(s, Logger::addEndOfLine);

	s = PaintCursorShapeString();
	logAll.Write("PaintCursorShapeString: ");
	logAll.Write(s, Logger::addEndOfLine);

	e = PIGetCrosshairVisible(b);
	if (!e)
	{
		logAll.Write("PIGetCrosshairVisible: ");
		logAll.Write(b ? "Yes" : "No", Logger::addEndOfLine);
	}

	e = PIGetZoomWithScrollWheel(b);
	if (!e)
	{
		logAll.Write("PIGetZoomWithScrollWheel: ");
		logAll.Write(b ? "Yes" : "No", Logger::addEndOfLine);
	}

	e = PIGetEXIFData(h);
	if (!e)
	{
		ostringstream outFileStream;
		outFileStream << gFullPathToDesktop;
		outFileStream << "PropetizerEXIFData";
		outFileStream << ".log";
		HandleToFile(outFileStream.str(), h);
		sPSHandle->Dispose(h);
	}

	s = HostVersionString();
	logAll.Write("HostVersionString: ");
	logAll.Write(s, Logger::addEndOfLine);
	
	s = PlayInProgressString();
	logAll.Write("PlayInProgressString: ");
	logAll.Write(s, Logger::addEndOfLine);
	
	e = PIGetRecordInProgress(b);
	if (!e)
	{
		logAll.Write("PIGetRecordInProgress: ");
		logAll.Write(b ? "Yes" : "No", Logger::addEndOfLine);
	}

	ps_wstring ws;
	e = PIGetDocumentName(ws, Logger::addEndOfLine);
	if (!e)
	{
		logAll.Write("PIGetDocumentNameExtension: ");
		logAll.Write(ws, Logger::addEndOfLine);
	}

	e = PIGetDocumentName(ws, false /* withExtension */);
	if (!e)
	{
		logAll.Write("PIGetDocumentName: ");
		logAll.Write(ws, Logger::addEndOfLine);
	}

	e = PIGetDontShowAgainCount(i);
	if (!e)
	{
		ostringstream ss;
		ss << "PIGetDontShowAgainCount: ";
		ss << i;
		logAll.Write(ss.str(), Logger::addEndOfLine);
	}

	e = GetLayerNames(vs);
	if (!e)
	{
		logAll.Write("GetLayerNames: ", Logger::addEndOfLine);
		logAll.Write(vs, Logger::addEndOfLine);
	}

	e = GetLayerNames(vws);
	if (!e)
	{
		logAll.Write("GetLayerNamesUnicode: ", Logger::addEndOfLine);
		logAll.Write(vws, Logger::addEndOfLine);
	}

	e = PIGetTargetLayerIndex(i);
	if (!e)
	{
		ostringstream ss;
		ss << "PIGetTargetLayerIndex: ";
		ss << i;
		logAll.Write(ss.str(), Logger::addEndOfLine);
	}

	e = PIGetScaleFactor(i);
	if (!e)
	{
		ostringstream ss;
		ss << "PIGetScaleFactor: ";
		ss << i;
		logAll.Write(ss.str(), Logger::addEndOfLine);
	}

	e = PIGetDoingPlace(b);
	if (!e)
	{
		logAll.Write("PIGetDoingPlace: ");
		logAll.Write(b ? "Yes" : "No", Logger::addEndOfLine);
	}

	e = PIGetRasterizingSmartObject(b);
	if (!e)
	{
		logAll.Write("PIGetRasterizingSmartObject: ");
		logAll.Write(b ? "Yes" : "No", Logger::addEndOfLine);
	}

	e = PIGetAppIsModal(b);
	if (!e)
	{
		logAll.Write("PIGetAppIsModal: ");
		logAll.Write(b ? "Yes" : "No", Logger::addEndOfLine);
	}

	e = PIGetPixelLength(i);
	if (!e)
	{
		ostringstream ss;
		ss << "PIGetPixelLength: ";
		ss << i;
		logAll.Write(ss.str(), Logger::addEndOfLine);
	}

	e = PIGetLogicalLength(i);
	if (!e)
	{
		ostringstream ss;
		ss << "PIGetLogicalLength: ";
		ss << i;
		logAll.Write(ss.str(), Logger::addEndOfLine);
	}

	e = PIGetMUnits(ws);
	if (!e)
	{
		logAll.Write("PIGetMUnits: ");
		logAll.Write(ws, Logger::addEndOfLine);
	}

	e = PIGetTileSize(i);
	if (!e)
	{
		ostringstream ss;
		ss << "PIGetTileSize: ";
		ss << i;
		logAll.Write(ss.str(), Logger::addEndOfLine);
	}

	e = PIGetScratchVolumeList(h);
	if (!e)
	{
		ostringstream outFileStream;
		outFileStream << gFullPathToDesktop;
		outFileStream << "PropetizerScratchVolumeList";
		outFileStream << ".log";
		HandleToFile(outFileStream.str(), h);
		sPSHandle->Dispose(h);
	}

	e = GetGuideInfo(vs);
	if (!e)
	{
		logAll.Write("GetGuideInfo: ", Logger::addEndOfLine);
		logAll.Write(vs, Logger::addEndOfLine);
	}

	e = PIGetGPUFlags(i);
	if (!e)
	{
		ostringstream ss;
		ss << "PIGetGPUFlags: ";
		ss << i;
		logAll.Write(ss.str(), Logger::addEndOfLine);
	}

	e = PIGetVRAM(i);
	if (!e)
	{
		ostringstream ss;
		ss << "PIGetVRAM: ";
		ss << i;
		logAll.Write(ss.str(), Logger::addEndOfLine);
	}

	s = LayerIsSmartString();
	logAll.Write("LayerIsSmartString: ");
	logAll.Write(s, Logger::addEndOfLine);

	e = PIGetHDPIAware(b);
	if (!e)
	{
		logAll.Write("PIGetHDPIAware: ");
		logAll.Write(b ? "Yes" : "No", Logger::addEndOfLine);
	}

	UIBrightness brightness;
	e = PIGetUIBrightness(brightness);
	if (!e)
	{
		logAll.Write("PIGetUIBrightness: ");
		logAll.Write(brightness, Logger::addEndOfLine);
	}
	
	e = PIGetLargeTabs(b);
	if (!e)
	{
		logAll.Write("PIGetLargeTabs: ");
		logAll.Write(b ? "Yes" : "No", Logger::addEndOfLine);
	}

	CheckerBoardSize  cSize;
	e = PIGetCheckerboardSize(cSize);
	if (!e)
	{
		logAll.Write("PIGetCheckerboardSize: ");
		logAll.Write(cSize, Logger::addEndOfLine);
	}

	e = PIGetCheckerboardOn(b);
	if (!e)
	{
		logAll.Write("PIGetCheckerboardOn: ");
		logAll.Write(b ? "Yes" : "No", Logger::addEndOfLine);
	}

	s = CloudDocumentString();
	logAll.Write("CloudDocumentString: ");
	logAll.Write(s, Logger::addEndOfLine);
	
	s = RecordInProgressString();
	logAll.Write("RecordInProgressString: ");
	logAll.Write(s, Logger::addEndOfLine);

	s = AccessLevelString();
	logAll.Write("AccessLevelString: ");
	logAll.Write(s, Logger::addEndOfLine);
	    
}

/****************************************************************************/

string ModeToString(int32 mode)
{
	string s;
	switch(mode)
	{
		case plugInModeBitmap:
			s = "Bitmap";
			break;
		case plugInModeGrayScale:
			s = "GrayScale";
			break;
		case plugInModeIndexedColor:
			s = "IndexedColor";
			break;
		case plugInModeRGBColor:
			s = "RGB";
			break;
		case plugInModeCMYKColor:
			s = "CMYK";
			break;
		case plugInModeHSLColor:
			s = "HSL";
			break;
		case plugInModeHSBColor:
			s = "HSB";
			break;
		case plugInModeMultichannel:
			s = "Multichannel";
			break;
		case plugInModeDuotone:
			s = "Dutotone";
			break;
		case plugInModeLabColor:
			s = "Lab";
			break;
		case plugInModeGray16:
			s = "Gray16";
			break;
		case plugInModeRGB48:
			s = "RGB48";
			break;
		case plugInModeLab48:
			s = "Lab48";
			break;
		case plugInModeCMYK64:
			s = "CMYK64";
			break;
		case plugInModeDeepMultichannel:
			s = "DeepMultichannel";
			break;
		case plugInModeDuotone16:
			s = "Duotone16";
			break;
		default:
			s = "Unkown";
			break;
	}
	return s;
}

/****************************************************************************/

string InterpolationToString(InterpolationMethod m)
{
	ostringstream ss;
	switch(m)
	{
		case nearestNeighbor:
			ss << "Nearest Neighbor";
			break;
		case bilinear:
			ss << "Bilinear";
			break;
		case  bicubic:
			ss << "Bicubic";
			break;
		case bicubicSmoother:
			ss << "Bicubic Smoother";
			break;
		case bicubicSharper:
			ss << "Bicubic Sharper";
			break;
		case bicubicAutomatic:
			ss << "Bicubic Automatic";
			break;
		default:
			ss << "Unknown " << m;
			break;
	}
	return ss.str();
}

/****************************************************************************/

string RulerUnitsToString(RulerUnits r)
{
	string s;
	switch (r)
	{
		case kRulerPixels:
			s = "Pixels";
			break;
		case kRulerInches:
			s = "Inches";
			break;
		case kRulerCm:
			s = "CM";
			break;
		case kRulerMillimeters:
			s = "MM";
			break;
		case kRulerPoints:
			s = "Points";
			break;
		case kRulerPicas:
			s = "Picas";
			break;
		case kRulerPercent:
			s = "Percent";
			break;
		default:
			s = "Unknown";
			break;
	}
	return s;
}

/****************************************************************************/

string CursorKindToString(PaintCursorKind c)
{
	string s;
	switch (c)
	{
		case standard:
			s = "Standard";
			break;
		case precise:
			s = "Precise";
			break;
		case brushSize:
			s = "Brush Size";
			break;
		default:
			s = "Unknown";
			break;
	}
	return s;
}

/****************************************************************************/

string CursorShapeToString(PaintCursorShape c)
{
	string s;
	switch (c)
	{
		case shapeNormal:
			s = "Normal";
			break;
		case shapeFullSized:
			s = "Full Sized";
			break;
		default:
			s = "Unknown";
			break;
	}
	return s;
}

/****************************************************************************/

string AccessLevelToString(AccessLevel c)
{
	string s;
	switch (c)
	{
		case alundefined:
			s = "undefined";
			break;
		case alrelease:
			s = "Release";
			break;
		case alpublicBeta:
			s = "Beta";
			break;
		case alpreRelease:
			s = "Pre Release";
			break;
		case alinternal:
			s = "Internal";
			break;
		case aloff:
			s = "Off";
			break;
		default:
			s = "Unknown";
			break;
	}
	return s;
}

/****************************************************************************/

string InterfaceColorToString(int8 color)
{
	string s;
	switch (color)
	{
		case kPIInterfaceButtonUpFill:
			s = "buttonUpFill";
			break;
		case kPIInterfaceBevelShadow:
			s = "bevelShadow";
			break;
		case kPIInterfaceIconFillActive:
			s = "iconFillActive";
			break;
		case kPIInterfaceIconFillDimmed:
			s = "iconFillDimmed";
			break;
		case kPIInterfacePaletteFill:
			s = "paletteFill";
			break;
		case kPIInterfaceIconFrameDimmed:
			s = "iconFrameDimmed";
			break;
		case kPIInterfaceIconFrameActive:
			s = "iconFrameActive";
			break;
		case kPIInterfaceBevelHighlight:
			s = "bevelHighlight";
			break;
		case kPIInterfaceButtonDownFill:
			s = "buttonDownFill";
			break;
		case kPIInterfaceIconFillSelected:
			s = "iconFillSelected";
			break;
		case kPIInterfaceBorder:
			s = "border";
			break;
		case kPIInterfaceButtonDarkShadow:
			s = "buttonDarkShadow";
			break;
		case kPIInterfaceIconFrameSelected:
			s = "iconFrameSelected";
			break;
		case kPIInterfaceRed:
			s = "red";
			break;
		default:
			s = "Uknown";
			break;
	}
	return s;
}

/****************************************************************************/

string RGBColorToString(RGBColor & color)
{
	ostringstream ss;
	ios::fmtflags remember = ss.setf(ios::hex, ios::basefield);
	ss << color.red << ", ";
	ss << color.green << ", ";
	ss << color.blue;
	ss.setf(remember);
	return ss.str();
}

/****************************************************************************/

string GetDocumentNameString(void)
{
	string s;
	if (PIGetDocumentName(s))
		s = "Unknown";
	return s;
}

/****************************************************************************/

string GetModeString(void)
{
	string s = "Unknown";
	int32 i;
	if (!PIGetImageMode(i) && i)
		s = ModeToString(i);
	return s;
}

/****************************************************************************/

string GetLayerLockingString(void)
{	
	string s = "";
	bool b, b1, b2;

	if (!PIGetTargetLayerLock(b, b1, b2))
	{
		if (b)
			s = "transparency ";
		if (b1)
			s = s + "composite ";
		if (b2)
			s = s + "position";
	}

	return s;
}

/****************************************************************************/

string GetWorkPathIndexString(void)
{
	string s = "Unknown";
	int32 i;
	if (!PIGetWorkPathIndex(i))
	{
		ostringstream ss;
		ss << i;
		s = ss.str();
	}
	return s;
}

/****************************************************************************/

string GetClippingPathIndexString(void)
{
	string s = "Unknown";
	int32 i;
	if (!PIGetClippingPathIndex(i))
	{
		ostringstream ss;
		ss << i;
		s = ss.str();
	}
	return s;
}

/****************************************************************************/

string GetTargetPathIndexString(void)
{
	string s = "Unknown";
	int32 i;
	if (!PIGetTargetPathIndex(i))
	{
		ostringstream ss;
		ss << i;
		s = ss.str();
	}
	return s;
}

/****************************************************************************/

string InterpolationModeString(void)
{
	string s = "Unknown";
	InterpolationMethod m;
	if (!PIGetInterpolationMethod(m))
		s = InterpolationToString(m);
	return s;
}

/****************************************************************************/

string RulerUnitsString(void)
{
	string s = "Unknown";
	RulerUnits r;
	if (!PIGetRulerUnits(r))
		s = RulerUnitsToString(r);
	return s;
}

/****************************************************************************/

string SerialString(void)
{
	string s = "Unknown";
	(void)PIGetSerialString(s);
	return s;
}

/****************************************************************************/

string OldSerialString(void)
{
	string s = "Unknown";
	(void)PIGetSerialStringOld(s);
	return s;
}

/****************************************************************************/

string HeightWidthString(void)
{
	ostringstream ss;
	int32 i;
	if (!PIGetDocumentHeight(i))
		ss << i;
	else
		ss << "Unknown";
	
	ss << ", ";

	if (!PIGetDocumentWidth(i))
		ss << i;
	else
		ss << "Unknown";

	return ss.str();
}

/****************************************************************************/

string ToolTipsString(void)
{
	string s = "Unknown";
    bool b;
	if (!PIGetToolTips(b))
    {
		if (b)
        {
			s = " on";
        } else {
			s = " off";
        }
    }
	return s;
}

/****************************************************************************/

string PaintCursorKindString(void)
{
	string s = "Unknown";
	PaintCursorKind cursorKind;
	if (!PIGetPaintCursorKind(cursorKind))
		s = CursorKindToString(cursorKind);
	return s;
}

/****************************************************************************/

string PaintCursorShapeString(void)
{
	string s = "Unknown";
	PaintCursorShape cursorShape;
	if (!PIGetPaintCursorShape(cursorShape))
		s = CursorShapeToString(cursorShape);
	return s;
}

/****************************************************************************/

string PlayInProgressString(void)
{
	string s = "Unknown";
	bool b;
	if (!PIGetPlayInProgress(b))
    {
		if (b)
        {
			s = "Yes";
        } else {
			s = "No";
        }
    }
	return s;
}

/****************************************************************************/

string RecordInProgressString(void)
{
	string s = "Unknown";
	bool b;
	if (!PIGetRecordInProgress(b))
    {
		if (b)
        {
			s = "Yes";
        } else {
			s = "No";
        }
    }
	return s;
}

/****************************************************************************/

string AccessLevelString(void)
{
	string s = "Unknown";
	AccessLevel accessLevel;
	if (!PIGetAccessLevel(accessLevel))
		s = AccessLevelToString(accessLevel);
	return s;
}

/****************************************************************************/

string CloudDocumentString(void)
{
	string s = "Unknown";
	bool b;
	if (!PIGetCloudDocument(b))
    {
		if (b)
        {
			s = "Yes";
        } else {
			s = "No";
        }
    }
	return s;
}

/****************************************************************************/

string LayerIsSmartString(void)
{
	string s = "Unknown";
	bool b;
	if (!PIGetLayerIsSmart(b))
    {
		if (b)
        {
			s = "Yes";
        } else {
			s = "No";
        }
    }
	return s;
}

/****************************************************************************/

string HostVersionString(void)
{
	ostringstream ss;
	int32 major, minor, fix;
	if (!PIGetHostVersion(major, minor, fix))
		ss << major << '.' << minor << '.' << fix;
	else
		ss << "Unknown";
	return ss.str();
}

/****************************************************************************/

string ShowSliceNumbers(void)
{
	string s;
    bool b;
	if (!PIGetShowSliceNumbers(b))
    {
		if (b)
        {
			s = "Yes";
        } else {
			s = "No";
        }
    } else {
		s = "Unknown";
    }
	return s;
}

/****************************************************************************/

string SliceLineColor(void)
{
	RGBColor lineColor;
	if (!PIGetSliceLineColor(lineColor))
	{
		return RGBColorToString(lineColor);
    }
	return "Unknown";
}

/****************************************************************************/

OSErr GetLayerNames(vector<string> & vs)
{
	int32 i = 0;
	OSErr e = PIGetNumberLayers(i);
	vs.clear();
	if (!e && !i)
		vs.push_back("Background");
	if (!e)
	{
		vs.reserve(i);
		for(int32 a = 0; a < i; a++)
		{
			string s;
			e = PIGetLayerName(a, s);
			if (!e)
				vs.push_back(s);
		}
	}

	return e;
}

/****************************************************************************/

OSErr GetLayerNames(vector<ps_wstring> & vs)
{
	int32 i = 0;
	OSErr e = PIGetNumberLayers(i);
	vs.clear();
	if (!e && !i)
    {
        const ASUnicode s[] = {'B','a','c','k','g','r','o','u','n','d',0};
        vs.push_back(s);
    }
	if (!e)
	{
		vs.reserve(i);
		for(int32 a = 0; a < i; a++)
		{
			ps_wstring s;
			e = PIGetLayerName(a, s);
			if (!e)
				vs.push_back(s);
		}
	}

	return e;
}

/****************************************************************************/

OSErr GetChannelNames(vector<string> & vs)
{
	int32 i = 0;
	OSErr e = PIGetNumberChannels(i);
	vs.clear();
	if (!e && i > 0)
	{
		vs.reserve(i);
		for (int32 a = 0; a < i; a++)
		{
			string s;
			e = PIGetChannelName(a, s);
			if (!e)
				vs.push_back(s);
		}
	}
	return e;
}

/****************************************************************************/

OSErr GetChannelNames(vector<ps_wstring> & vs)
{
	int32 i = 0;
	OSErr e = PIGetNumberChannels(i);
	vs.clear();
	if (!e && i > 0)
	{
		vs.reserve(i);
		for (int32 a = 0; a < i; a++)
		{
			ps_wstring s;
			e = PIGetChannelName(a, s);
			if (!e)
				vs.push_back(s);
		}
	}
	return e;
}

/****************************************************************************/

 OSErr GetPathNames(vector<string> & vs)
{
	int32 i = 0;
	OSErr e = PIGetNumberPaths(i);
	vs.clear();
    if (!e && i > 0)
	{
		vs.reserve(i);
		for (int32 a = 0; a < i; a++)
		{
			string s;
			e = PIGetPathName(a, s);
			if (!e)
				vs.push_back(s);
		}
	}
	return e;
}

/****************************************************************************/

 OSErr GetPathNames(vector<ps_wstring> & vs)
{
	int32 i = 0;
	OSErr e = PIGetNumberPaths(i);
	vs.clear();
    if (!e && i > 0)
	{
		vs.reserve(i);
		for (int32 a = 0; a < i; a++)
		{
			ps_wstring s;
			e = PIGetPathName(a, s);
			if (!e)
				vs.push_back(s);
		}
	}
	return e;
}

/****************************************************************************/

OSErr GetInterfaceColors(vector<string> & vs)
{
	OSErr e = 0;
	PIInterfaceColor iColor;
	vs.clear();
	for (int8 color = kPIInterfaceButtonUpFill; color <= kPIInterfaceRed; color++)
	{
		e = PIGetInterfaceColor(color, iColor);
		if (!e)
		{
			string s = InterfaceColorToString(color);
			ostringstream ss;
			ios::fmtflags remember = ss.setf(ios::hex, ios::basefield);
			ss << static_cast<short>(iColor.color2.alpha) << ", ";
			ss << static_cast<short>(iColor.color2.r) << ", ";
			ss << static_cast<short>(iColor.color2.g) << ", ";
			ss << static_cast<short>(iColor.color2.b);
			ss << "  " << static_cast<short>(iColor.color32.alpha) << ", ";
			ss << static_cast<short>(iColor.color32.r) << ", ";
			ss << static_cast<short>(iColor.color32.g) << ", ";
			ss << static_cast<short>(iColor.color32.b);
			s = s + " " + ss.str();
			vs.push_back(s);
			ss.setf(remember);
		}
	}
	RGBColor rgbColor;
	e = PIGetGuideColor(rgbColor);
	if (!e)
	{
		string s = "guide " + RGBColorToString(rgbColor);
		vs.push_back(s);
	}
	e = PIGetCheckerboardColor(1, rgbColor);
	if (!e)
	{
		string s = "checker 1 " + RGBColorToString(rgbColor);
		vs.push_back(s);
	}
	e = PIGetCheckerboardColor(2, rgbColor);
	if (!e)
	{
		string s = "checker 2 " + RGBColorToString(rgbColor);
		vs.push_back(s);
	}
	return e;
}

/****************************************************************************/

string GetGuideCount(void)
{
	int32 guideCount = 0;
	ostringstream ss;
	if (!PIGetGuideCount(guideCount))
	{
		ios::fmtflags remember = ss.setf(ios::dec, ios::basefield);
		ss << guideCount;
		ss.setf(remember);
	}
	else
		ss << "Unknown";
	return ss.str();
}

/****************************************************************************/

OSErr GetGuideInfo(vector<string> & vs)
{
	int32 guideCount = 0;
	OSErr e = PIGetGuideCount(guideCount);
	vs.clear();
	if (!e && guideCount)
	{
		for (int32 i = 0; i < guideCount; i++)
		{
			bool horizontal;
			e = PIGetGuideHorizontal(i, horizontal);
			if (!e)
			{
				double position;
				e = PIGetGuidePosition(i, position);
				if (!e)
				{
					ostringstream ss;
					ios::fmtflags remember = ss.setf(ios::hex, ios::basefield);
					ss << position;
					string s = ( horizontal ? "Horz " : "Vert " ) + ss.str();
					vs.push_back(s);
					ss.setf(remember);
				}
			}
		}
	}
	return e;
}

// end Propetizer.cpp

