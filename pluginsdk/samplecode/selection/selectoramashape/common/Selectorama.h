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
//		Selectorama.h
//
//	Description:
//		This file contains the global definitions and headers
//		for the Selection module Selectorama, an example module
//		that just returns a pixel selection.
//
//	Use:
//		Use selection modules to return pixel or path selections on a new
//		layer or the current layer.
//
//-------------------------------------------------------------------------------

#ifndef __Selectorama_H__
#define __Selectorama_H__

#include "PISelection.h"
#include "PIUtilities.h"
#include "SelectoramaTerminology.h"	// Terminology for plug-in.

/*****************************************************************************/
/* Any constants here */

#define kBlockRows			64
#define kBlockCols			64

#define kFirstItem			4
#define kLastItem			kFirstItem+2

#define kPercentStatic		7
#define kPercentEdit		kPercentStatic+1
#define kPercentSymbol		kPercentEdit+1

#define kPercentMin			0
#define kPercentMax			100

#define kUseRadio1			11
#define kUseRadioLast		kUseRadio1 + 1

#define kCreateRadio1		14
#define kCreateRadioLast	kCreateRadio1+2

#define kInitRandom			1
#define kRandomOn			255
#define kRandomOff			0

/*****************************************************************************/

enum
{
	iSelectMin,
	iSelectMax,
	iSelectRandom
};

enum
{
	iUseTarget,
	iUseMerged
};

enum
{
	iCreateSelection,
	iCreateMaskpath,
	iCreateLayer
};

//-------------------------------------------------------------------------------
//	Globals -- structures
//-------------------------------------------------------------------------------

typedef struct Globals
{ // This is our structure that we use to pass globals between routines:

	short				*result;				// Must always be first in Globals.
	PISelectionParams	*selectionParamBlock;	// Must always be second in Globals.

	Boolean	queryForParameters;
	short	whatArea;
	short	whatChannels;
	short	percent;
	short	create;
} Globals, *GPtr;
	
#define gResult 			(*(globals->result))
#define gStuff  			(globals->selectionParamBlock)
#define gQueryForParameters	(globals->queryForParameters)

#define gWhatArea 			(globals->whatArea)
#define gWhatChannels		(globals->whatChannels)
#define	gPercent			(globals->percent)
#define DoTarget			(gWhatChannels == iUseTarget)
#define gCreate				(globals->create)

//-------------------------------------------------------------------------------
//	Prototypes
//-------------------------------------------------------------------------------

void InitGlobals (Ptr globalPtr);		  	// Initialize globals.
void DoAbout (AboutRecordPtr about); 	    // Pop about box.
void DoExecute (GPtr globals);				// Main routine.

Boolean DoUI (GPtr globals);
short KeyToEnum (OSType key, OSType whatType);
OSType EnumToKey (short keyEnum, OSType whatType);
Boolean ReadScriptParams (GPtr globals);
OSErr WriteScriptParams (GPtr globals);

/*****************************************************************************/

#endif /* __Selectorama_H__ */
