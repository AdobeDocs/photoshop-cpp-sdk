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
//		AutomationFilterUI.h
//
//	Description:
//		An automation plug in that uses a hidden filter plug in to
//		do something very boring.
//
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
//	Includes 
//-------------------------------------------------------------------------------
#ifndef __AUTOMATIONFILTERUI_H__
#define __AUTOMATIONFILTERUI_H__

#include "AutomationFilter.h"
#include "PIUI.h"

extern SPBasicSuite* sSPBasic;

const int8 kDDone_Button = 1;
const int8 kDCancel_Button = 2;
const int8 kDLayer_Static = 3;
const int8 kDLayer_List = 4;
const int8 kDChannel_Static = 5;
const int8 kDChannel_List = 6;
const int8 kD0_Button = 7;
const int8 kD50_Button = 8;
const int8 kD100_Button = 9;
const int8 kDRandom_Button = 10;
const int8 kDTileHeight_Static = 11;
const int8 kDTileWidth_Static = 12;

// forward declare, see AutomationFilter.h
class AutomationFilterData;

// class to control our dialog
// we override the Init, Notify, and Destroy routines
class AutomationFilterDialog : public PIDialog {
private:
	AutomationFilterData *_instance;
	PIComboBox layerList;
	PIComboBox channelList;
	PIText tileHeight;
	PIText tileWidth;

	virtual void Init(void);
	virtual void Notify(int32 item);

public:
	AutomationFilterDialog(AutomationFilterData *instance) : PIDialog(),
															   _instance(instance),
															   layerList(), 
															   channelList(),
															   tileHeight(),
															   tileWidth() {}
	~AutomationFilterDialog() {}
};
#endif
// end AutomationFilterUI.h


