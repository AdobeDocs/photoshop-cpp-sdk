// ADOBE SYSTEMS INCORPORATED
// Copyright  1993 - 2015 Adobe Systems Incorporated
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
//		ListenerUIMac.h
//
//	Description:
//		Definitions and routines for Mac
//
//-------------------------------------------------------------------------------

#ifndef __ListenerUIMac_H__		// Has this not been defined yet?
#define __ListenerUIMac_H__

#include "PITerminology.h"
#include <Cocoa/Cocoa.h>


struct ListenerEventArray
{
    int32       longVal;
    NSString*   strStr;
};


const struct ListenerEventArray useful[] = {
    eventSave,  @"eventSave",
    eventGaussianBlur, @"eventGaussianBlur",
    eventCopy, @"eventCopy",
    eventCrop, @"eventCrop",
    0, NULL
};


#endif
