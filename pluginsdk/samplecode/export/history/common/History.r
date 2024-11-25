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
//		History.r
//
//	Copyright 1990-1992, Thomas Knoll.
//	All Rights Reserved.
//
//	Description:
//		This file contains the resource information
//		for the Export module History, a module that
//		displays and lets the user manipulate
//		pseudo-resources of type 'hist'.
//
//	Use:
//		This module shows how to examine, display, and work
//		with pseudo-resources.  An additional element might
//		be to have it export those remaining pseudo-resources
//		to a text file.
//
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
//	Definitions -- Required by include files.
//-------------------------------------------------------------------------------

// The About box and resources are created in PIUtilities.r.
// You can easily override them, if you like.

#define plugInName			"History"
#define plugInCopyrightYear	"1993"
#define plugInDescription \
	"An Export plug-in module showcasing pseudo-resources in Adobe Photoshop¨."

//-------------------------------------------------------------------------------
//	Definitions -- Required by other resources in this rez file.
//-------------------------------------------------------------------------------

// Dictionary (aete) resources:

#define vendorName			"AdobeSDK"
#define plugInAETEComment	"history export plug-in"

#define plugInSuiteID		'sdKB'
#define plugInClassID		'hisT'
#define plugInEventID		typeNull // must be this

//-------------------------------------------------------------------------------
//	Set up included files for Macintosh and Windows.
//-------------------------------------------------------------------------------

#include "PIDefines.h"
#include "PIResourceDefines.h"

#ifdef __PIMac__
	#include "PIGeneral.r"
	#include "PIUtilities.r"
#elif defined(__PIWin__)
	#define Rez
	#include "PIGeneral.h"
	#include "PIUtilities.r"
#endif

#include "PITerminology.h"
#include "PIActions.h"

#include "HistoryTerminology.h"		// Terminology specific to this plug-in.

//-------------------------------------------------------------------------------
//	PiPL resource
//-------------------------------------------------------------------------------

resource 'PiPL' (ResourceID, plugInName " PiPL", purgeable)
{
	{
		Kind { Export },
		Name { plugInName "..." },
		Version { (latestExportVersion << 16) | latestExportSubVersion },

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

		HasTerminology
		{
			plugInClassID,		// class ID
			plugInEventID,		// event ID
			ResourceID,			// aete ID
			"" 					// uniqueString
		},
		
		SupportedModes
		{
			noBitmap, doesSupportGrayScale,
			doesSupportIndexedColor, doesSupportRGBColor,
			doesSupportCMYKColor, doesSupportHSLColor,
			doesSupportHSBColor, doesSupportMultichannel,
			doesSupportDuotone, doesSupportLABColor
		},
			
		EnableInfo
		{ 
			"true"
		}

	}
	
}; // end PiPL

//-------------------------------------------------------------------------------
//	PiMI resource (Photoshop 2.5 and other older hosts)
//-------------------------------------------------------------------------------

resource 'PiMI' (ResourceID, plugInName " PiMI", purgeable)
{ 
	  latestExportVersion,
	  latestExportSubVersion,
	  0,
	  supportsGrayScale +
	  supportsIndexedColor +
	  supportsRGBColor +
	  supportsCMYKColor +
	  supportsHSLColor +
	  supportsHSBColor +
	  supportsMultichannel +
	  supportsDuotone +
	  supportsLABColor,
	  '    ', /* No required host */
	  {},

}; // end PiMI

//-------------------------------------------------------------------------------
//	Dictionary (scripting) resource
//-------------------------------------------------------------------------------

resource 'aete' (ResourceID, plugInName " dictionary", purgeable)
{
	1, 0, english, roman,									/* aete version and language specifiers */
	{
		vendorName,											/* vendor suite name */
		"Adobe example plug-ins",							/* optional description */
		plugInSuiteID,											/* suite ID */
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
				classExport,								/* parent: Format, Import, Export */
				"parent class export",						/* optional description */
				flagsSingleProperty,						/* if properties, list below */
				
				"in",										/* our path */
				keyIn,										/* common key */
				typePlatformFilePath,						/* correct path for platform */
				"file path",								/* optional description */
				flagsSingleProperty
				
				/* no more properties */
			},
			{}, /* elements (not supported) */
			/* class descriptions */
		},
		{}, /* comparison ops (not supported) */
		{}	/* any enumerations */
	}
	
}; // end AETE



//-------------------------------------------------------------------------------

// end History.r
