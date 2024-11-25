// ADOBE SYSTEMS INCORPORATED
// Copyright  1993 - 2002 Adobe Systems Incorporated
// All Rights Reserved
//
// NOTICE:  Adobe permits you to use, modify, and distribute this 
// file in accordance with the terms of the Adobe license agreement
// accompanying it.  If you have received this file from a source
// other than Adobe, then your use, modification, or distribution
// of it requires the prior written permission of Adobe.
//-------------------------------------------------------------------------------
//	Includes 
//-------------------------------------------------------------------------------
#ifndef __AUTOMATIONFILTER_H__
#define __AUTOMATIONFILTER_H__

#include "PIDefines.h"
#if __PIMac__
	#include <string.h>
#endif
#include <vector>
#include <time.h>
#include "PIUSelect.h"
#include "SPHost.h"
#include "PIUGet.h"
#include "PIActionsPlugIn.h"
#include "PIUIHooksSuite.h"
#include "PIHandleSuite.h"

#include "HiddenCommands.h"
#include "ASZStringSuite.h"
#include "PIUSuites.h"

#define plugInCopyrightYear	"2000"

using namespace std;
#if !__LP64__
// forward decleration, see AutomationFilterUI.h
class AutomationFilterDialog;
#endif

/** Version 2 of AutomationFilter and Hidden.
    The Hidden plug in now does all of the pixel work.
	This enables us to work on much bigger files. The Hidden plug in still
	passes information back to this plug in, tiling information for example.
	Passing all the pixel data is a bad idea so I removed it.
*/
class AutomationFilterData {

private:
	int32 numChannels;
	vector<string> channelNames;
	int32 numLayers;
	vector<string> layerNames;
	DescriptorEnumID command;
	bool hasBackground;
	int32 tileHeight;
	int32 tileWidth;
	SPErr error;

#if __LP64__
	int * mainDialog;
#else // __LP64__
	/// Dialog for this plug in
	AutomationFilterDialog * mainDialog;
#endif // __LP64__
	/// Functions specific to the AutomationFilter plug in
	SPErr GetDocumentInfo(void);
	SPErr ExecuteHiddenFilter(DescriptorEnumID command, 
		                      const string & nameOfChannel,
							  int16 typeOfChannel,
							  uint8 percent);
	SPErr GetChannelNames(void);
	SPErr GetLayerNames(void);

public:

	AutomationFilterData();
	~AutomationFilterData() {}

	/// Run the plug-in from this routine
	SPErr DoIt(PSActionsPlugInMessage * message);

	SPErr GetError(void) { return error; }

	/// Functions needed by AutomationFilterDialog
	SPErr Execute(void);
	void GetLayerName(int32 index, string & outName);
	void GetChannelName(int32 index, string & outName);
	void ExecuteFilter(const string & layerName, const string & channelName, uint8 percentWhite);
	
	/// Tell Photoshop to redraw everything, this is an expensive call
	void Redraw(void);

	/// Return the number of layers in this document, I count the background as a layer
	int32 NumberOfLayers(void) { return (numLayers + hasBackground); }

	/// Return the number of channels in this document
	int32 NumberOfChannels(void) { 	return numChannels; }

	/// Return the tile height for this document
	int32 GetTileHeight(void) { return tileHeight; }

	/// Return the tile width for this document
	int32 GetTileWidth(void) { return tileWidth; }
};
#endif
// end AutomationFilter.h
