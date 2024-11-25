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
//		GradientImportTerminology.h
//
//	Copyright 1990, Thomas Knoll.
//	All Rights Reserved.
//
//	Description:
//		This file contains the resource definitions for the
//		Import module GradientImport, a scriptable multiple-acquire
//		plug-in that creates 4 types of gradients in batches.
//
//	Use:
//		This import module is a great example of scripting
//		Import modules that use the old style Photoshop
//		multiple-acquire mechanism.
//
//-------------------------------------------------------------------------------

#ifndef __GradientImportTerminology_h__	// Has this not been defined yet?
#define __GradientImportTerminology_h__	// Only include this once by predefining it

//-------------------------------------------------------------------------------
//	Definitions -- Scripting keys.
//-------------------------------------------------------------------------------

#define keyMultiImportInfo				'mulK'
#define classMultiImportStruct			'mulS'
#define typeClassMultiImportStruct		'mulT'
#define keyMultiImportCount				keyMultiImportInfo // 'mulC'
#define keyOurMode						keyMode
#define keyRows							keyVertical
#define keyColumns						keyHorizontal
#define keyOurAlphaTransparency			'alpT'
#define typeGradientMode				'grmT'
	#define ourBitmapMode				'bitM'
	#define ourGrayscaleMode			'gryS'
	#define ourIndexedColorMode			'indX'
	#define ourRGBColorMode				'rgbC'
	
//-------------------------------------------------------------------------------
//	Definitions -- Resource IDs.
//-------------------------------------------------------------------------------

#define errHowdWeGetHere				16989

//-------------------------------------------------------------------------------

#endif // __GradientImportTerminology_h__
