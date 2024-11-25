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
#ifndef _DISSOLVEUI_H
#define _DISSOLVEUI_H

#include "PITypes.h"
#include "DialogUtilities.h"

const int16 kDOK = 1;
const int16 kDCancel = 2;
const int16 kDEditText = 3;
const int16 kDProxyItem = 5;
const int16 kDFirstRadio = 7;
const int16 kDLastRadio = 10;
const int16 kDEntireImage = 11;

Boolean DoUI(void);
void DoAbout(void);
#endif
// end DissolveUI.h