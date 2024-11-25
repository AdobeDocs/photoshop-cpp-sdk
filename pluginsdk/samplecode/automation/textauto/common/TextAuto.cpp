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
//		TextAuto.cpp
//
//	Description:
//
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
//	Includes 
//-------------------------------------------------------------------------------

#include <sstream>
#include "PIDefines.h"
#include "SPHost.h"
#include "PIUGet.h"
#include "PIUSelect.h"
#include "PIActionsPlugIn.h"
#include "PIUIHooksSuite.h"
#include "PIHandleSuite.h"
#include "PIUI.h"

#include "ASZStringSuite.h"
#include "PIUSuites.h"
#include "PITerminology.h"

#if __PIMac__
	#include <string.h>
#endif
#include <vector>
#include <time.h>

using namespace std;

bool gNotifierOn = false;

void MakeTextLayer(string fullPath, VRect rect);
void DeleteBackground(void);
ASErr StartupPlugin(void);
ASErr ShutdownPlugin(void);
void OpenEvent(DescriptorEventID event, PIActionDescriptor descriptor, PIDialogRecordOptions options, void * data);


//-------------------------------------------------------------------------------
//	Globals
//-------------------------------------------------------------------------------
const int bufferSize = 4096;
const int filePathSize = 1024;

// everyone needs access to the sPSBasic pointer 
SPBasicSuite * sSPBasic = NULL;
SPPluginRef	gPlugInRef = NULL;

/// Definition of the main entrypoint for all automation plugins
DLLExport SPAPI SPErr AutoPluginMain(const char * caller, const char * selector, void * message);


//-------------------------------------------------------------------------------
//
//	AutoPluginMain / main
//
//	All calls to the plug-in module come through this routine.
//	It must be placed first in the resource.  To achieve this,
//	most development systems require this be the first routine
//	in the source.
//
//	The entrypoint will be "pascal void" for Macintosh,
//	"void" for Windows.
//
//-------------------------------------------------------------------------------
DLLExport SPAPI SPErr AutoPluginMain(const char* caller, const char* selector, void* message)
{

	SPErr error = kSPNoError;

	try {

	SPMessageData * basicMessage = (SPMessageData *) message;

	sSPBasic = basicMessage->basic;
	
	gPlugInRef = basicMessage->self;

	if (sSPBasic->IsEqual(caller, kSPInterfaceCaller))
	{
		if (sSPBasic->IsEqual(selector, kSPInterfaceAboutSelector))
			DoAbout(basicMessage->self, AboutID);
		else if (sSPBasic->IsEqual(selector, kSPInterfaceStartupSelector))
			error = StartupPlugin();
		else if (sSPBasic->IsEqual(selector, kSPInterfaceShutdownSelector))
			error = ShutdownPlugin();

	}
	
	PIUSuitesRelease();

	} // end try

	catch(...)
	{
		if (error == 0)
			error = kSPBadParameterError;
	}

	return error;
}


ASErr StartupPlugin(void)
{
	ASErr error = kSPNoError;
	
	if (!gNotifierOn)
	{
		error = sPSActionControl->AddNotify(gPlugInRef,
											eventOpen,
											OpenEvent,
											NULL);
											
		if (!error)
			gNotifierOn = true;
	}

	return error;
}


ASErr ShutdownPlugin(void)
{
	ASErr error = 0;

	if (gNotifierOn)
		error = sPSActionControl->RemoveNotify(gPlugInRef,
											   eventOpen);
	return error;
}


void OpenEvent( DescriptorEventID event,
				PIActionDescriptor descriptor,
				PIDialogRecordOptions options,
				void * data)
{
	Boolean hasKey = false;
	DescriptorTypeID type = NULL;
	SPErr error = 0;
	Auto_Desc object(false);
	DescriptorClassID classID = NULL, runtimeID = NULL;
	Handle aliasValue = NULL;
	
	vector<char> fullPathToFile(filePathSize);

	memset(&fullPathToFile[0], 0, filePathSize);

	if (event != eventOpen) return;

	if (descriptor == NULL) return;

	error = sPSActionDescriptor->HasKey(descriptor, keyAs, &hasKey);

	if (error) return;
	if (!hasKey) return;

	error = sPSActionDescriptor->GetType(descriptor, keyAs, &type);
	
	if (error) return;
	if (type != typeObject) return;

	error = sPSActionDescriptor->HasKey(descriptor, keyNull, &hasKey);

	if (error) return;
	if (!hasKey) return;

	error = sPSActionDescriptor->GetType(descriptor, keyNull, &type);
	
	if (error) return;
	if (type != typeAlias && type != typePath) return;
	
	error = sPSActionDescriptor->GetObject(descriptor, keyAs, &classID, &object);

	if (error) return;
	
	error = sPSActionControl->StringIDToTypeID("9E3AF9BA-CBDD-4b26-920C-5FE8A5C61B59", &runtimeID);

	if (error) return;

	if (runtimeID != classID) return;

	error = sPSActionDescriptor->GetAlias(descriptor, keyNull, &aliasValue);

	if (error) return;

	VRect rect;
	
	rect.top = 0;
	rect.left = 0;

	double d = 0.0;
	
	double resolution = 72.0;

	DescriptorUnitID units = NULL;

	(void)PIUGetInfo(classDocument, keyResolution, &resolution, &units);

	if (PIUGetInfo(classDocument, keyWidth, &d, &units))
	{
		rect.right = 100;
	}
	else
	{
		d = UnitsToPixels(d, units, resolution);
		rect.right = static_cast<int32>(d);
	}

	units = NULL;

	if (PIUGetInfo(classDocument, keyHeight, &d, &units))
	{
		rect.bottom = 100;
	}
	else
	{
		d = UnitsToPixels(d, units, resolution);
		rect.bottom = static_cast<int32>(d);
	}

	AliasToFullPath(aliasValue, &fullPathToFile[0], filePathSize);

	MakeTextLayer(&fullPathToFile[0], rect);

	DeleteBackground();

	if (aliasValue != NULL)
		sPSHandle->DisposeRegularHandle(aliasValue);

}

void MakeTextLayer(string fullPath, VRect rect)
{
	#if __PIMac__
		ifstream inFile(fullPath.c_str(), ios_base::binary | ios_base::in);
	#else
		// j systems have trouble opening the file with Shift-JIS chars
		// use this trick
		ifstream inFile(fopen(fullPath.c_str(), "rb"));
	#endif
	
	ostringstream fileContents;

	vector<char> buffer(bufferSize);

	inFile.read(&buffer[0], bufferSize - 1);

	int bytesRead = (int)inFile.gcount();

	buffer[bytesRead] = 0;

	// the \n shows up odd in Photoshop, it likes \r
	int id = 0;
	for (int is = 0; is < bytesRead; is++)
	{
		if (buffer[is] == '\n')
			buffer[id++] = '\r';
		else
			buffer[id++] = buffer[is];
	}

	buffer[id] = 0;

	fileContents << &buffer[0];

	if (fileContents.str().length() == 0) return;

	Auto_Desc result(false);
    
	DescriptorTypeID runtimeKeyID = 0;
    DescriptorTypeID runtimeTypeID = 0;
    DescriptorTypeID runtimeObjID = 0;
    DescriptorTypeID runtimeEnumID = 0;
    DescriptorTypeID runtimeClassID = 0;
    DescriptorTypeID runtimeUnitID = 0;
    
    Auto_Desc desc;

    Auto_Ref ref;

	(void)sPSActionReference->PutClass(ref.get(), classTextLayer);
	(void)sPSActionDescriptor->PutReference(desc.get(), keyNull, ref.get());

	Auto_Desc textObject;

	(void)sPSActionDescriptor->PutString(textObject.get(), 
											keyText, 
											const_cast<char *>(fileContents.str().c_str()));

	Auto_Desc warpObject;

	(void)sPSActionControl->StringIDToTypeID("warpStyle", &runtimeKeyID);
	(void)sPSActionControl->StringIDToTypeID("warpNone", &runtimeEnumID);
	(void)sPSActionControl->StringIDToTypeID("warpStyle", &runtimeTypeID);
	(void)sPSActionDescriptor->PutEnumerated(warpObject.get(), runtimeKeyID, runtimeTypeID, runtimeEnumID);
	(void)sPSActionControl->StringIDToTypeID("warpValue", &runtimeKeyID);
	(void)sPSActionDescriptor->PutFloat(warpObject.get(), runtimeKeyID, 0);
	(void)sPSActionControl->StringIDToTypeID("warpPerspective", &runtimeKeyID);
	(void)sPSActionDescriptor->PutFloat(warpObject.get(), runtimeKeyID, 0);
	(void)sPSActionControl->StringIDToTypeID("warpPerspectiveOther", &runtimeKeyID);
	(void)sPSActionDescriptor->PutFloat(warpObject.get(), runtimeKeyID, 0);
	(void)sPSActionControl->StringIDToTypeID("warpRotate", &runtimeKeyID);
	(void)sPSActionDescriptor->PutEnumerated(warpObject.get(), runtimeKeyID, typeOrientation, enumHorizontal);
	(void)sPSActionControl->StringIDToTypeID("warp", &runtimeObjID);
	(void)sPSActionControl->StringIDToTypeID("warp", &runtimeKeyID);
	(void)sPSActionDescriptor->PutObject(textObject.get(), runtimeKeyID, runtimeObjID, warpObject.get());

	Auto_Desc positionObject;

	(void)sPSActionDescriptor->PutUnitFloat(positionObject.get(), keyHorizontal, unitDistance, rect.left);
	(void)sPSActionDescriptor->PutUnitFloat(positionObject.get(), keyVertical, unitDistance, rect.top);
	(void)sPSActionDescriptor->PutObject(textObject.get(), keyTextClickPoint, classPoint, positionObject.get());
	(void)sPSActionControl->StringIDToTypeID("textGridding", &runtimeKeyID);
	(void)sPSActionControl->StringIDToTypeID("textGridding", &runtimeTypeID);
	(void)sPSActionDescriptor->PutEnumerated(textObject.get(), runtimeKeyID, runtimeTypeID, enumNone);
	(void)sPSActionDescriptor->PutEnumerated(textObject.get(), keyOrientation, typeOrientation, enumHorizontal);
	(void)sPSActionControl->StringIDToTypeID("antiAliasSharp", &runtimeEnumID);
	(void)sPSActionDescriptor->PutEnumerated(textObject.get(), keyAntiAlias, typeAntiAlias, runtimeEnumID);

	Auto_List shapeList;
	Auto_Desc boundsObject;

	(void)sPSActionControl->StringIDToTypeID("box", &runtimeEnumID);
	(void)sPSActionDescriptor->PutEnumerated(boundsObject.get(), 'TEXT', typeChar, runtimeEnumID);

	Auto_Desc boundsRectObject;

	(void)sPSActionDescriptor->PutFloat(boundsRectObject.get(), keyTop, rect.top);
	(void)sPSActionDescriptor->PutFloat(boundsRectObject.get(), keyLeft, rect.left);
	(void)sPSActionDescriptor->PutFloat(boundsRectObject.get(), keyBottom, rect.bottom);
	(void)sPSActionDescriptor->PutFloat(boundsRectObject.get(), keyRight, rect.right);
	(void)sPSActionControl->StringIDToTypeID("bounds", &runtimeKeyID);
	(void)sPSActionDescriptor->PutObject(boundsObject.get(), runtimeKeyID, classRectangle, boundsRectObject.get());
	(void)sPSActionControl->StringIDToTypeID("textShape", &runtimeClassID);
	(void)sPSActionList->PutObject(shapeList.get(), runtimeClassID, boundsObject.get());
	(void)sPSActionControl->StringIDToTypeID("textShape", &runtimeKeyID);
	(void)sPSActionDescriptor->PutList(textObject.get(), runtimeKeyID, shapeList.get());

	Auto_List textStyleList;
	Auto_Desc textStyleRangeObject;

	(void)sPSActionDescriptor->PutInteger(textStyleRangeObject.get(), keyFrom, 0);
	(void)sPSActionDescriptor->PutInteger(textStyleRangeObject.get(), keyTo, (int32)(fileContents.str().length() + 1));

	Auto_Desc textStyleObject;

	(void)sPSActionControl->StringIDToTypeID("fontPostScriptName", &runtimeKeyID);
	(void)sPSActionDescriptor->PutString(textStyleObject.get(), runtimeKeyID, (char*)"Helvetica");
	(void)sPSActionDescriptor->PutString(textStyleObject.get(), keyFontName, (char*)"Helvetica");
	(void)sPSActionDescriptor->PutString(textStyleObject.get(), keyFontStyleName, (char*)"Medium");
	(void)sPSActionDescriptor->PutInteger(textStyleObject.get(), keyFontScript, 0);
	(void)sPSActionDescriptor->PutInteger(textStyleObject.get(), keyFontTechnology, 0);
	(void)sPSActionControl->StringIDToTypeID("pointsUnit", &runtimeUnitID);
	(void)sPSActionDescriptor->PutUnitFloat(textStyleObject.get(), keySizeKey, runtimeUnitID, 24);
	(void)sPSActionControl->StringIDToTypeID("syntheticBold", &runtimeKeyID);
	(void)sPSActionDescriptor->PutBoolean(textStyleObject.get(), runtimeKeyID, false);
	(void)sPSActionControl->StringIDToTypeID("syntheticItalic", &runtimeKeyID);
	(void)sPSActionDescriptor->PutBoolean(textStyleObject.get(), runtimeKeyID, false);
	(void)sPSActionControl->StringIDToTypeID("autoLeading", &runtimeKeyID);
	(void)sPSActionDescriptor->PutBoolean(textStyleObject.get(), runtimeKeyID, true);
	(void)sPSActionDescriptor->PutInteger(textStyleObject.get(), keyHorizontalScale, 100);
	(void)sPSActionDescriptor->PutInteger(textStyleObject.get(), keyVerticalScale, 100);
	(void)sPSActionDescriptor->PutInteger(textStyleObject.get(), keyTracking, 0);
	(void)sPSActionDescriptor->PutBoolean(textStyleObject.get(), keyAutoKern, true);
	(void)sPSActionControl->StringIDToTypeID("pointsUnit", &runtimeUnitID);
	(void)sPSActionDescriptor->PutUnitFloat(textStyleObject.get(), keyBaselineShift, runtimeUnitID, 0);
	(void)sPSActionControl->StringIDToTypeID("fontCaps", &runtimeKeyID);
	(void)sPSActionControl->StringIDToTypeID("fontCaps", &runtimeTypeID);
	(void)sPSActionDescriptor->PutEnumerated(textStyleObject.get(), runtimeKeyID, runtimeTypeID, enumNormal);
	(void)sPSActionControl->StringIDToTypeID("baseline", &runtimeKeyID);
	(void)sPSActionControl->StringIDToTypeID("baseline", &runtimeTypeID);
	(void)sPSActionDescriptor->PutEnumerated(textStyleObject.get(), runtimeKeyID, runtimeTypeID, enumNormal);
	(void)sPSActionDescriptor->PutBoolean(textStyleObject.get(), keyUnderline, false);
	(void)sPSActionControl->StringIDToTypeID("strikethrough", &runtimeKeyID);
	(void)sPSActionDescriptor->PutBoolean(textStyleObject.get(), runtimeKeyID, false);
	(void)sPSActionControl->StringIDToTypeID("ligature", &runtimeKeyID);
	(void)sPSActionDescriptor->PutBoolean(textStyleObject.get(), runtimeKeyID, true);
	(void)sPSActionControl->StringIDToTypeID("altligature", &runtimeKeyID);
	(void)sPSActionDescriptor->PutBoolean(textStyleObject.get(), runtimeKeyID, false);
	(void)sPSActionControl->StringIDToTypeID("oldStyle", &runtimeKeyID);
	(void)sPSActionDescriptor->PutBoolean(textStyleObject.get(), runtimeKeyID, false);
	(void)sPSActionControl->StringIDToTypeID("proportionalNumbers", &runtimeKeyID);
	(void)sPSActionDescriptor->PutBoolean(textStyleObject.get(), runtimeKeyID, true);
	(void)sPSActionDescriptor->PutBoolean(textStyleObject.get(), keyRotate, true);
	(void)sPSActionControl->StringIDToTypeID("baselineDirection", &runtimeKeyID);
	(void)sPSActionControl->StringIDToTypeID("withStream", &runtimeEnumID);
	(void)sPSActionControl->StringIDToTypeID("baselineDirection", &runtimeTypeID);
	(void)sPSActionDescriptor->PutEnumerated(textStyleObject.get(), runtimeKeyID, runtimeTypeID, runtimeEnumID);
	(void)sPSActionControl->StringIDToTypeID("mojiZume", &runtimeKeyID);
	(void)sPSActionDescriptor->PutFloat(textStyleObject.get(), runtimeKeyID, 0);
	(void)sPSActionControl->StringIDToTypeID("gridAlignment", &runtimeKeyID);
	(void)sPSActionControl->StringIDToTypeID("roman", &runtimeEnumID);
	(void)sPSActionControl->StringIDToTypeID("gridAlignment", &runtimeTypeID);
	(void)sPSActionDescriptor->PutEnumerated(textStyleObject.get(), runtimeKeyID, runtimeTypeID, runtimeEnumID);
	(void)sPSActionControl->StringIDToTypeID("textLanguage", &runtimeKeyID);
	(void)sPSActionControl->StringIDToTypeID("englishLanguage", &runtimeEnumID);
	(void)sPSActionControl->StringIDToTypeID("textLanguage", &runtimeTypeID);
	(void)sPSActionDescriptor->PutEnumerated(textStyleObject.get(), runtimeKeyID, runtimeTypeID, runtimeEnumID);
	(void)sPSActionControl->StringIDToTypeID("wariChuCount", &runtimeKeyID);
	(void)sPSActionDescriptor->PutInteger(textStyleObject.get(), runtimeKeyID, 1);
	(void)sPSActionControl->StringIDToTypeID("wariChuScale", &runtimeKeyID);
	(void)sPSActionDescriptor->PutFloat(textStyleObject.get(), runtimeKeyID, 1);
	(void)sPSActionControl->StringIDToTypeID("wariChuWidow", &runtimeKeyID);
	(void)sPSActionDescriptor->PutInteger(textStyleObject.get(), runtimeKeyID, 25);
	(void)sPSActionControl->StringIDToTypeID("wariChuOrphan", &runtimeKeyID);
	(void)sPSActionDescriptor->PutInteger(textStyleObject.get(), runtimeKeyID, 25);
	(void)sPSActionControl->StringIDToTypeID("noBreak", &runtimeKeyID);
	(void)sPSActionDescriptor->PutBoolean(textStyleObject.get(), runtimeKeyID, false);

	Auto_Desc colorObject;

	(void)sPSActionDescriptor->PutFloat(colorObject.get(), keyRed, 0);
	(void)sPSActionDescriptor->PutFloat(colorObject.get(), keyGreen, 0);
	(void)sPSActionDescriptor->PutFloat(colorObject.get(), keyBlue, 0);
	(void)sPSActionDescriptor->PutObject(textStyleObject.get(), keyColor, classRGBColor, colorObject.get());
	
	(void)sPSActionDescriptor->PutBoolean(textStyleObject.get(), keyFill, true);
	(void)sPSActionDescriptor->PutBoolean(textStyleObject.get(), 'Strk', false);
	(void)sPSActionControl->StringIDToTypeID("fillFirst", &runtimeKeyID);
	(void)sPSActionDescriptor->PutBoolean(textStyleObject.get(), runtimeKeyID, false);
	(void)sPSActionControl->StringIDToTypeID("verticalUnderlinePosition", &runtimeKeyID);
	(void)sPSActionControl->StringIDToTypeID("verticalUnderlineLeft", &runtimeEnumID);
	(void)sPSActionControl->StringIDToTypeID("verticalUnderlinePosition", &runtimeTypeID);
	(void)sPSActionDescriptor->PutEnumerated(textStyleObject.get(), runtimeKeyID, runtimeTypeID, runtimeEnumID);
	(void)sPSActionDescriptor->PutObject(textStyleRangeObject.get(), keyTextStyle, classTextStyle, textStyleObject.get());
	(void)sPSActionList->PutObject(textStyleList.get(), classTextStyleRange, textStyleRangeObject.get());
	(void)sPSActionDescriptor->PutList(textObject.get(), keyTextStyleRange, textStyleList.get());

	Auto_List paragraphStyleList;

	Auto_Desc paragraphRangeObject;

	(void)sPSActionDescriptor->PutInteger(paragraphRangeObject.get(), keyFrom, 0);
	(void)sPSActionDescriptor->PutInteger(paragraphRangeObject.get(), keyTo, (int32)(fileContents.str().length() + 1));

	Auto_Desc paragraphStyleObject;

	(void)sPSActionDescriptor->PutEnumerated(paragraphStyleObject.get(), keyAlignment, typeAlignment, enumLeft);
	(void)sPSActionControl->StringIDToTypeID("firstLineIndent", &runtimeKeyID);
	(void)sPSActionControl->StringIDToTypeID("pointsUnit", &runtimeUnitID);
	(void)sPSActionDescriptor->PutUnitFloat(paragraphStyleObject.get(), runtimeKeyID, runtimeUnitID, 0);
	(void)sPSActionControl->StringIDToTypeID("startIndent", &runtimeKeyID);
	(void)sPSActionControl->StringIDToTypeID("pointsUnit", &runtimeUnitID);
	(void)sPSActionDescriptor->PutUnitFloat(paragraphStyleObject.get(), runtimeKeyID, runtimeUnitID, 0);
	(void)sPSActionControl->StringIDToTypeID("endIndent", &runtimeKeyID);
	(void)sPSActionControl->StringIDToTypeID("pointsUnit", &runtimeUnitID);
	(void)sPSActionDescriptor->PutUnitFloat(paragraphStyleObject.get(), runtimeKeyID, runtimeUnitID, 0);
	(void)sPSActionControl->StringIDToTypeID("spaceBefore", &runtimeKeyID);
	(void)sPSActionControl->StringIDToTypeID("pointsUnit", &runtimeUnitID);
	(void)sPSActionDescriptor->PutUnitFloat(paragraphStyleObject.get(), runtimeKeyID, runtimeUnitID, 0);
	(void)sPSActionControl->StringIDToTypeID("spaceAfter", &runtimeKeyID);
	(void)sPSActionControl->StringIDToTypeID("pointsUnit", &runtimeUnitID);
	(void)sPSActionDescriptor->PutUnitFloat(paragraphStyleObject.get(), runtimeKeyID, runtimeUnitID, 0);
	(void)sPSActionControl->StringIDToTypeID("hyphenate", &runtimeKeyID);
	(void)sPSActionDescriptor->PutBoolean(paragraphStyleObject.get(), runtimeKeyID, true);
	(void)sPSActionControl->StringIDToTypeID("hyphenateWordSize", &runtimeKeyID);
	(void)sPSActionDescriptor->PutInteger(paragraphStyleObject.get(), runtimeKeyID, 8);
	(void)sPSActionControl->StringIDToTypeID("hyphenatePreLength", &runtimeKeyID);
	(void)sPSActionDescriptor->PutInteger(paragraphStyleObject.get(), runtimeKeyID, 3);
	(void)sPSActionControl->StringIDToTypeID("hyphenatePostLength", &runtimeKeyID);
	(void)sPSActionDescriptor->PutInteger(paragraphStyleObject.get(), runtimeKeyID, 3);
	(void)sPSActionControl->StringIDToTypeID("hyphenateLimit", &runtimeKeyID);
	(void)sPSActionDescriptor->PutInteger(paragraphStyleObject.get(), runtimeKeyID, 2);
	(void)sPSActionControl->StringIDToTypeID("hyphenationZone", &runtimeKeyID);
	(void)sPSActionDescriptor->PutFloat(paragraphStyleObject.get(), runtimeKeyID, 36);
	(void)sPSActionControl->StringIDToTypeID("hyphenateCapitalized", &runtimeKeyID);
	(void)sPSActionDescriptor->PutBoolean(paragraphStyleObject.get(), runtimeKeyID, true);
	(void)sPSActionControl->StringIDToTypeID("justificationWordMinimum", &runtimeKeyID);
	(void)sPSActionDescriptor->PutFloat(paragraphStyleObject.get(), runtimeKeyID, 0.8);
	(void)sPSActionControl->StringIDToTypeID("justificationWordDesired", &runtimeKeyID);
	(void)sPSActionDescriptor->PutFloat(paragraphStyleObject.get(), runtimeKeyID, 1);
	(void)sPSActionControl->StringIDToTypeID("justificationWordMaximum", &runtimeKeyID);
	(void)sPSActionDescriptor->PutFloat(paragraphStyleObject.get(), runtimeKeyID, 1.33);
	(void)sPSActionControl->StringIDToTypeID("justificationLetterMinimum", &runtimeKeyID);
	(void)sPSActionDescriptor->PutFloat(paragraphStyleObject.get(), runtimeKeyID, 0);
	(void)sPSActionControl->StringIDToTypeID("justificationLetterDesired", &runtimeKeyID);
	(void)sPSActionDescriptor->PutFloat(paragraphStyleObject.get(), runtimeKeyID, 0);
	(void)sPSActionControl->StringIDToTypeID("justificationLetterMaximum", &runtimeKeyID);
	(void)sPSActionDescriptor->PutFloat(paragraphStyleObject.get(), runtimeKeyID, 0);
	(void)sPSActionControl->StringIDToTypeID("justificationGlyphMinimum", &runtimeKeyID);
	(void)sPSActionDescriptor->PutFloat(paragraphStyleObject.get(), runtimeKeyID, 1);
	(void)sPSActionControl->StringIDToTypeID("justificationGlyphDesired", &runtimeKeyID);
	(void)sPSActionDescriptor->PutFloat(paragraphStyleObject.get(), runtimeKeyID, 1);
	(void)sPSActionControl->StringIDToTypeID("justificationGlyphMaximum", &runtimeKeyID);
	(void)sPSActionDescriptor->PutFloat(paragraphStyleObject.get(), runtimeKeyID, 1);
	(void)sPSActionControl->StringIDToTypeID("hangingRoman", &runtimeKeyID);
	(void)sPSActionDescriptor->PutBoolean(paragraphStyleObject.get(), runtimeKeyID, false);
	(void)sPSActionControl->StringIDToTypeID("burasagari", &runtimeKeyID);
	(void)sPSActionDescriptor->PutBoolean(paragraphStyleObject.get(), runtimeKeyID, false);
	(void)sPSActionControl->StringIDToTypeID("preferredKinsokuOrder", &runtimeKeyID);
	(void)sPSActionControl->StringIDToTypeID("pushIn", &runtimeEnumID);
	(void)sPSActionControl->StringIDToTypeID("preferredKinsokuOrder", &runtimeTypeID);
	(void)sPSActionDescriptor->PutEnumerated(paragraphStyleObject.get(), runtimeKeyID, runtimeTypeID, runtimeEnumID);
	(void)sPSActionControl->StringIDToTypeID("mojiKumiName", &runtimeKeyID);
	(void)sPSActionDescriptor->PutString(paragraphStyleObject.get(), runtimeKeyID, (char*)"None");
	(void)sPSActionControl->StringIDToTypeID("textEveryLineComposer", &runtimeKeyID);
	(void)sPSActionDescriptor->PutBoolean(paragraphStyleObject.get(), runtimeKeyID, false);
	(void)sPSActionControl->StringIDToTypeID("autoLeadingPercentage", &runtimeKeyID);
	(void)sPSActionDescriptor->PutFloat(paragraphStyleObject.get(), runtimeKeyID, 1.2);
	(void)sPSActionControl->StringIDToTypeID("leadingType", &runtimeKeyID);
	(void)sPSActionControl->StringIDToTypeID("leadingBelow", &runtimeEnumID);
	(void)sPSActionControl->StringIDToTypeID("leadingType", &runtimeTypeID);
	(void)sPSActionDescriptor->PutEnumerated(paragraphStyleObject.get(), runtimeKeyID, runtimeTypeID, runtimeEnumID);
	(void)sPSActionControl->StringIDToTypeID("paragraphStyle", &runtimeObjID);
	(void)sPSActionControl->StringIDToTypeID("paragraphStyle", &runtimeKeyID);
	(void)sPSActionDescriptor->PutObject(paragraphRangeObject.get(), runtimeKeyID, runtimeObjID, paragraphStyleObject.get());
	(void)sPSActionControl->StringIDToTypeID("paragraphStyleRange", &runtimeClassID);
	(void)sPSActionList->PutObject(paragraphStyleList.get(), runtimeClassID, paragraphRangeObject.get());
	(void)sPSActionControl->StringIDToTypeID("paragraphStyleRange", &runtimeKeyID);
	(void)sPSActionDescriptor->PutList(textObject.get(), runtimeKeyID, paragraphStyleList.get());

	(void)sPSActionDescriptor->PutObject(desc.get(), keyUsing, classTextLayer, textObject.get());
	(void)sPSActionControl->Play(&result, eventMake, desc.get(), plugInDialogSilent);
}


void DeleteBackground(void)
{
	Auto_Desc result(false);
	Auto_Desc desc;
	Auto_Ref ref;

	(void)sPSActionReference->PutProperty(ref.get(), classLayer, keyBackground);
	(void)sPSActionDescriptor->PutReference(desc.get(), keyNull, ref.get());
	(void)sPSActionControl->Play(&result, eventDelete, desc.get(), plugInDialogSilent);
}

// end TextAuto.cpp



