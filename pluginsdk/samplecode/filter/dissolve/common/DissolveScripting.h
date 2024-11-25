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
#ifndef _DISSOLVESCRIPTING_H
#define _DISSOLVESCRIPTING_H

#include "PIDefines.h"
#include "PIActions.h"
#include "PITerminology.h"

#ifndef Rez
#include "Dissolve.h"
#endif

#define vendorName			"AdobeSDK"
#define plugInName			"Dissolve"
#define plugInAETEComment	"dissolve example filter plug-in"
#define plugInSuiteID		'sdk1'
#define plugInClassID		plugInSuiteID
#define plugInEventID		plugInClassID
#define plugInUniqueID		"d9543b0c-3c91-11d4-97bc-00b0d0204936"

#define plugInCopyrightYear  "2000"
#define plugInDescription \
	"An example plug-in filter module for Adobe Photoshop¨."

#define keyDisposition 		'disP'
#define keyIgnoreSelection	'ignS'
#define typeMood			'mooD'
#define dispositionClear	'moD0'
#define dispositionCool		'moD1'
#define dispositionHot		'moD2'
#define dispositionSick		'moD3'

#ifndef Rez
OSErr ReadScriptParameters(Boolean* displayDialog);
OSErr WriteScriptParameters(void);
int16 ScriptToDialog(int32 script);
int32 DialogToScript(int16 dialog);
#endif
#endif