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
//
//	File:
//		HiddenCommands.h
//
//	Description:
//		Utility plug in for AutomationFilter this guy writes pixels on command.
//
//-------------------------------------------------------------------------------
#ifndef __HiddenCommands_H__
#define __HiddenCommands_H__

// WARNING: This makes your plug in unique for scripting.
// WARNING: Please generate a new number using uuidgen.exe or equivalent
// WARNING: Do not copy this value into another plug-in
// WARNING: If you modify the functionality you must also generate a new number
#define HiddenUniqueString "36f7e9a0-1e42-11d6-ba98-0000861c9048"

#define plugInCopyrightYear	"2000"

#define plugInSuiteID		'sdK1'
#define plugInClassID		plugInSuiteID
#define plugInEventID		plugInClassID

#define	keyCommand			'Cmd '
#define keyResult			'Rslt'
#define keyPercent			'Perc'
#define kRandomPercent		150

#define typeCommand			'TCmd'
#define enumWrite			'Writ'
#define enumInfo			'Info'

#define typeResult			'TRsl'
#define enumOK				'OK  '
#define enumBad				'Bad '
#define enumNoCommand		'NoCd'
#define enumNoTo			'NoTo'
#define enumNoTargetChannel	'NoTC'
#define enumUnknownCommand	'Unkn'
#define enumNoPercent		'NoPc'

#endif
