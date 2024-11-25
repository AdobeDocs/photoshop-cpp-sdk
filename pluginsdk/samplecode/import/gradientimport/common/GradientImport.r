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
//		GradientImport.r
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

//-------------------------------------------------------------------------------
//	Definitions -- Required by include files.
//-------------------------------------------------------------------------------

// The About box and resources are created in PIUtilities.r.
// You can easily override them, if you like.

#define plugInName				"GradientImport"
#define plugInDescription \
	"An example import plug-in module for Adobe Photoshop¨."
	
//-------------------------------------------------------------------------------
//	Definitions -- Required by other resources in this rez file.
//-------------------------------------------------------------------------------

// Dictionary (aete) resources:

#define vendorName					"AdobeSDK"
#define plugInAETEComment 			"gradientimport example import plug-in"
#define plugInSuiteID				'sdK3'
#define plugInClassID				'graD'
#define plugInEventID				typeNull // must be this

//-------------------------------------------------------------------------------
//	Set up included files for Macintosh and Windows.
//-------------------------------------------------------------------------------

#include "PIDefines.h"
#include "PIResourceDefines.h"

#ifdef __PIMac__
	#include "PIGeneral.r"
	#include "PIUtilities.r"
#elif defined (__PIWin__)
	#define Rez
	#include "PIGeneral.h"
	#include "PIUtilities.r"
#endif

#include "PITerminology.h"
#include "PIActions.h"

#include "GradientImportTerminology.h"	// Terminology for plug-in.

//-------------------------------------------------------------------------------
//	PiPL resource
//-------------------------------------------------------------------------------

resource 'PiPL' (ResourceID, plugInName " PiPL", purgeable)
{
	{
		Kind { Acquire },
		Name { plugInName "..." },
		Version { (latestAcquireVersion << 16) | latestAcquireSubVersion },
				
		Component { ComponentNumber, plugInName },

		#if Macintosh
			#if defined(__arm64__)
				CodeMacARM64 { "PluginMain" },
			#endif
			#if defined(__x86_64__)
				CodeMacIntel64 { "PluginMain" },
			#endif
		#elif MSWindows
			CodeEntryPointWin64 { "PluginMain" },
		#endif

		// Class ID, event ID, aete ID, uniqueString:
		HasTerminology { plugInClassID, plugInEventID, ResourceID, "" },
		
		// __PhotoDeluxe__ Mac icon type, Windows icon type, Resource ID, button text:
		ButtonIcon { cicn, ICON, ResourceID, "GradientImport!" },
		
		// __PhotoDeluxe__ Sub-class: None, scanner, camera, video, floppy, cdrom, internet:
		Class { internet },

		// __PhotoDeluxe__ Actual preview file name:
		PreviewFile { "myfile.bok" },

	}
};

resource 'PiMI' (ResourceID, plugInName " PiMI", purgeable)
{ 
	  latestAcquireVersion,
	  latestAcquireSubVersion,
	  0,
	  supportsBitmap +
	  supportsGrayScale +
	  supportsIndexedColor +
	  supportsRGBColor,
	  '    ', /* No required host */
	  {},
};

//-------------------------------------------------------------------------------
//	Dictionary (scripting) resource
//-------------------------------------------------------------------------------

resource 'aete' (ResourceID, plugInName " dictionary", purgeable)
{
	1, 0, english, roman,									/* aete version and language specifiers */
	{
		vendorName,											/* vendor suite name */
		"Adobe example plug-ins",							/* optional description */
		plugInSuiteID,										/* suite ID */
		1,													/* suite code, must be 1 */
		1,													/* suite level, must be 1 */
		{},													/* structure for filters */
		{													/* non-filter plug-in class here */
			vendorName " " plugInName,						/* unique class name */
			plugInClassID,									/* class ID, must be unique or Suite ID */
			plugInAETEComment,								/* optional description */
			{												/* define inheritance */
				"<Inheritance>",							/* must be exactly this */
				keyInherits,								/* must be keyInherits */
				classImport,								/* parent: Format, Import, Export */
				"parent class import",						/* optional description */
				flagsSingleProperty,						/* if properties, list below */

				"multi-import",								/* property name */
				keyMultiImportInfo,							/* our key */
				classMultiImportStruct,						/* our type */
				"multiple import info structure",			/* optional description */
				flagsListProperty
			},
			{}, /* elements (not supported) */

		/* other classes... */
			"import info",									/* unique class name */
			classMultiImportStruct,							/* class key */
			"class import info",							/* optional description */
			{
				"rows",										/* property name */
				keyRows,									/* rows key */
				typeFloat,									/* float, later pin to unixPixels */
				"number of rows",							/* optional description */
				flagsSingleProperty,						/* properties */
				
				"columns",									/* property name */
				keyColumns,									/* columns key */
				typeFloat,									/* float, later pin to unitPixels */
				"number of columns",						/* optional description */
				flagsSingleProperty,						/* properties */
				
				"mode",										/* property name */
				keyOurMode,									/* our mode key */
				typeGradientMode,							/* our modes */
				"color mode",								/* optional description */
				flagsEnumeratedProperty,					/* enumeration. See class below. */

				"invert",									/* property name */
				keyInvert,									/* our key, 'invR' */
				typeBoolean,								/* our check box */
				"invert image",								/* optional description */
				flagsSingleProperty,						/* properties */

				"alpha2transparency",						/* property name */
				keyOurAlphaTransparency,					/* our key, 'alpT' */
				typeBoolean,								/* our check box */
				"alpha is transparency",					/* optional description */
				flagsSingleProperty,						/* properties */

			},
			{}, /* elements (unsupported) */
		},
		{}, /* comparison ops (not supported) */
		{													/* any enumerations */
			typeGradientMode,								/* enumeration */
			{
				"bitmap",									/* bitmap mode */
				ourBitmapMode,								/* 'bitM' */
				"bitmap mode",								/* optional description */
				
				"grayscale",								/* grayscale mode */
				ourGrayscaleMode,							/* 'gryS' */
				"grayscale mode",							/* optional description */
				
				"indexed color",							/* indexed color mode */
				ourIndexedColorMode,						/* 'indX' */
				"indexed color mode",						/* optional description */
				
				"rgb color",								/* RGB color mode */
				ourRGBColorMode,							/* 'rgbC' */
				"rgb color mode"							/* optional description */

			},				
		}
	}
};

//-------------------------------------------------------------------------------
//	Error strings
//-------------------------------------------------------------------------------

resource StringResource (errHowdWeGetHere, "Howd We Get Here", purgeable)
{
	"-- how'd we get here? Aliens?!"
};

//-------------------------------------------------------------------------------

// end GradientImport.r
