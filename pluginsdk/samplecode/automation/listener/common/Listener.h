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
//		Listener.h
//
//	Description:
//		Global function and variable definitions for the Listener plug in.
//
//-------------------------------------------------------------------------------

#ifndef __Listener_H__		// Has this not been defined yet?
#define __Listener_H__		// Only include this once by predefining it

#define _MSL_MACHEADERS_INCLUDES_CMATH 1

#if defined(__cplusplus) && TARGET_API_MAC_CARBON && _MSL_MACHEADERS_INCLUDES_CMATH
	#include <cmath>
#endif

#include "PIDefines.h"
#include "SPTypes.h"
#include "SPAccess.h"
#include "SPInterf.h"
#include "PIUActionUtils.h" // this needs to be first so the AEObjects.h gets precompiled first

#include "ListenerTerminology.h"		// Terminology for this plug-in.
#include "PIActionsPlugIn.h"			// Actions plug-in Photoshop header file

#include "PIActions.h"
#include "PIUSuites.h"
#include "PIUActions.h"
#include "PIUGet.h"
#if __PIMac__
	#include <stdio.h>
#endif

//-------------------------------------------------------------------------------
//	Globals -- structures
//-------------------------------------------------------------------------------
typedef struct Listener_t
{
	string 		actionName;
	string 		actionSet;
	DescriptorEventID 	eventID;
	Listener_t*			next;
} Listener_t;

extern string* gActionName;
extern string* gActionSet;
extern DescriptorEventID gEventID;
extern Listener_t* gListenerList;

extern SPBasicSuite* sSPBasic;
extern SPPluginRef gPlugInRef;

extern vector<std::string> * gSubtractStrings;
extern vector<std::string> * gAddStrings;
extern vector<DescriptorEventID> * gRunIDs;

//-------------------------------------------------------------------------------
//	Prototypes
//-------------------------------------------------------------------------------
 // Main routine.
SPErr Execute( PIActionParameters* actionParams );
 // Read any scripting params.
SPErr ReadScriptParams( PIActionParameters* actionParams );
 // Write any scripting params.
SPErr WriteScriptParams( PIActionParameters* actionParams );
 // Read my settings from Photoshop registry.
void ReadPreferences( void );
 // Write my settings to Photoshop registry.
void WritePreferences( void );

void DoAbout (SPPluginRef plugin, int dialogID);
SPErr DoUI(void);
Listener_t* FindListenerListEnd( void );

void SubtractFromList(const std::string &s);
void SubtractFromList(PIActionList subList);
void AddToList(const std::string &s);
void AddToList(PIActionList subList);


//-------------------------------------------------------------------------------

#endif // __Listener_H__
//end Listener.h
