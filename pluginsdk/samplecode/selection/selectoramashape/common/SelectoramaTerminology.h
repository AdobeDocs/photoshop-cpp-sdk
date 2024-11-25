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
//		SelectoramaTerminology.h
//
//	Description:
//		This file contains the resource definitions for the
//		for the Selection module Selectorama, an example module
//		that just returns a pixel selection.
//
//	Use:
//		Use selection modules to return pixel or path selections on a new
//		layer or the current layer.
//
//-------------------------------------------------------------------------------

#ifndef __SelectoramaTerminology_h__	// Have we defined this yet?
#define __SelectoramaTerminology_h__	// Only define once.

//-------------------------------------------------------------------------------
//	Definitions -- Scripting keys.
//-------------------------------------------------------------------------------

#define selectMin			'seL0'
#define selectMax			'seL1'
#define selectRandom		'shL2'
#define useTarget			'useT'
#define useMerged			'useM'
#define typeMySelect		'tseL'
#define typeMyComposite		'tcoM' // targetCompositeChannels, mergedCompositeChannels
#define typeMyPISel			'tpiS'
#define keyMyCreate			'kcrE'
#define typeMyCreate		'tcrE'
#define keyMyChannels		keyChannels
#define	createSelection		'crE0'
#define createMaskpath		'crE1'
#define createLayer			'crE2'
#define keyMyArea			keyArea
#define keyMyAmount			keyAmount
#define keyMyShape			keyShape

//-------------------------------------------------------------------------------
//	Definitions -- Resource IDs.
//-------------------------------------------------------------------------------


//-------------------------------------------------------------------------------

#endif // __SelectoramaTerminology_h__
