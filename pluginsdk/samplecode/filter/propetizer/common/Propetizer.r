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
//		Propetizer.r
//
//	Description:
//		This file contains the resource information
//		for the Filter module Propetizer, a module that
//		examines and displays grids and guide settings,
//		then makes pseudo-resource history entries when
//		you change them.
//
//	Use:
//		This is a basic module to exemplify all the typical
//		functions a filter module will do with a special
//		emphasis on assessing properties and writing
//		pseudo-resources.
//
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
//	Definitions -- Required by include files.
//-------------------------------------------------------------------------------

// The About box and resources are created in PIUtilities.r.
// You can easily override them, if you like.

#define plugInName			"Propetizer"
#define plugInCopyrightYear	"1996"
#define plugInDescription \
	"An example plug-in filter showing properties in Adobe Photoshop¨."

//-------------------------------------------------------------------------------
//	Definitions -- Required by other resources in this rez file.
//-------------------------------------------------------------------------------

// Dictionary (aete) resources:

#define vendorName			"AdobeSDK"
#define plugInAETEComment 	"propetizer example filter plug-in"

#define plugInSuiteID		'sdK9'
#define plugInClassID		plugInSuiteID
#define plugInEventID		'griD'

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

#include "PropetizerTerminology.h"	// Terminology for this plug-in.

//-------------------------------------------------------------------------------
//	PiPL resource
//-------------------------------------------------------------------------------

resource 'PiPL' (ResourceID, plugInName " PiPL", purgeable)
{
	{
		Kind { Filter },
		Name { plugInName "..." },
		Category { vendorName },
		Version { (latestFilterVersion << 16) | latestFilterSubVersion },

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

		HasTerminology { plugInClassID, plugInEventID, ResourceID, "" },
		/* classID, eventID, aete ID, uniqueString */
		
		SupportedModes
		{
			noBitmap, doesSupportGrayScale,
			noIndexedColor, doesSupportRGBColor,
			doesSupportCMYKColor, doesSupportHSLColor,
			doesSupportHSBColor, doesSupportMultichannel,
			doesSupportDuotone, doesSupportLABColor
		},

		EnableInfo
		{
			"in (PSHOP_ImageMode, GrayScaleMode, RGBMode,"
			"CMYKMode, HSLMode, HSBMode, MultichannelMode,"
			"DuotoneMode, LabMode, RGB48Mode, Gray16Mode) ||"
			"PSHOP_ImageDepth == 16 || PSHOP_ImageDepth == 32" 
		},

		PlugInMaxSize { 2000000, 2000000 },
					     
		FilterCaseInfo
		{
			{
				/* Flat data, no selection */
				inWhiteMat, outStraightData,
				doNotWriteOutsideSelection,
				filtersLayerMasks, worksWithBlankData,
				copySourceToDestination,
					
				/* Flat data with selection */
				inWhiteMat, outStraightData,
				doNotWriteOutsideSelection,
				filtersLayerMasks, worksWithBlankData,
				copySourceToDestination,
				
				/* Floating selection */
				inWhiteMat, outStraightData,
				doNotWriteOutsideSelection,
				filtersLayerMasks, worksWithBlankData,
				copySourceToDestination,
					
				/* Editable transparency, no selection */
				inWhiteMat, outStraightData,
				doNotWriteOutsideSelection,
				filtersLayerMasks, worksWithBlankData,
				copySourceToDestination,
					
				/* Editable transparency, with selection */
				inWhiteMat, outStraightData,
				doNotWriteOutsideSelection,
				filtersLayerMasks, worksWithBlankData,
				copySourceToDestination,
					
				/* Preserved transparency, no selection */
				inWhiteMat, outStraightData,
				doNotWriteOutsideSelection,
				filtersLayerMasks, worksWithBlankData,
				copySourceToDestination,
					
				/* Preserved transparency, with selection */
				inWhiteMat, outStraightData,
				doNotWriteOutsideSelection,
				filtersLayerMasks, worksWithBlankData,
				copySourceToDestination
			}
		}
	}
	
}; // end PiPL

//-------------------------------------------------------------------------------
//	PiMI resource (Photoshop 2.5 and other older hosts)
//-------------------------------------------------------------------------------

resource 'PiMI' (ResourceID, plugInName " PiMI", purgeable)
{ 
	  latestFilterVersion,
	  latestFilterSubVersion,
	  0,
	  supportsGrayScale +
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
		{													/* structure for filters */
			vendorName " " plugInName,						/* unique filter name */
			plugInAETEComment,								/* optional description */
			plugInClassID,									/* class ID, must be unique or Suite ID */
			plugInEventID,									/* event ID, must be unique */
			
			NO_REPLY,										/* never a reply */
			IMAGE_DIRECT_PARAMETER,							/* direct parameter, used by Photoshop */
			{												/* parameters here, if any */
				"nudge H",									/* parameter name */
				keyMyNudgeH,								/* parameter key ID */
				typeFloat,									/* parameter type ID */
				"horizontal big nudge",						/* optional description */
				flagsSingleParameter,						/* parameter flags */
				
				"nudge V",									/* parameter name */
				keyMyNudgeV,								/* parameter key ID */
				typeFloat,									/* parameter type ID */
				"vertical big nudge",						/* optional description */
				flagsSingleParameter,						/* parameter flags */

				"origin H",									/* parameter name */
				keyMyHorizontal,							/* parameter key ID */
				typeFloat,									/* parameter type ID */
				"horizontal origin",						/* optional description */
				flagsSingleParameter,						/* parameter flags */
				
				"origin V",									/* parameter name */
				keyMyVertical,								/* parameter key ID */
				typeFloat,									/* parameter type ID */
				"vertical origin",							/* optional description */
				flagsSingleParameter,						/* parameter flags */
				
				"gridMajor",								/* parameter name */
				keyMyGridMajor,								/* parameter key ID */
				typeFloat,									/* parameter type ID */
				"major grid divisions",						/* optional description */
				flagsSingleParameter,						/* parameter flags */
				
				"gridMinor",								/* parameter name */
				keyMyGridMinor,								/* parameter key ID */
				typeInteger,								/* parameter type ID */
				"minor grid subdivisions",					/* optional description */
				flagsSingleParameter,						/* parameter flags */

				"watch",
				keyMyWatch,
				typeInteger,
				"watch cursor",
				flagsSingleParameter,
				
				"copyright",								/* you get the idea */
				keyMyCopyright,
				typeChar,
				"copyright string",
				flagsSingleParameter,
				
				"url",
				keyMyURL,
				typeChar,
				"URL string",
				flagsSingleParameter,

				"watermark",
				keyMyWater,
				typeBoolean,
				"watermark",
				flagsSingleParameter,
				
				"slice ID",
				keyMySliceID,
				typeInteger,
				"slice ID",
				flagsSingleParameter,
				
				"slice index",
				keyMySliceIndex,
				typeInteger,
				"slice index",
				flagsSingleParameter,
				
				"XMP",
				keyMyXMP,
				typeChar,
				"XMP",
				flagsSingleParameter
	
			}
		},
		{},													/* non-filter plug-in class here */
		{},													/* comparison ops (not supported) */
		{}													/* any enumerations */
	}
	
}; // end AETE

//-------------------------------------------------------------------------------
//	Dialog value text entires
//
//	Entering these as separate resources because then they
//	transfer directly over to windows via CNVTPIPL.
//
//	If I use a Macintosh 'STR#' resource, I could put all these
//	strings into a single resource, but there is no
//	parallel on Windows.  'STR ' resources, which hold
//	one string per ID, exist on Windows and Macintosh.
//-------------------------------------------------------------------------------

resource StringResource(kUnits, purgeable)
{
	"pixels"
};

resource StringResource (kUnits+1, purgeable)
{
	"inches"
};

resource StringResource (kUnits+2, purgeable)
{
	"cm"
};

resource StringResource (kUnits+3, purgeable)
{
	"points"
};

resource StringResource (kUnits+4, purgeable)
{
	"picas"
};

resource StringResource (kUnits+5, purgeable)
{
	"percent"
};

//-------------------------------------------------------------------------------
//	History resource text entry
//-------------------------------------------------------------------------------

resource StringResource (kHistoryEntry, "History", purgeable)
{
	plugInName ": grid=^2, nudge=^1, origin=^0."
};

//-------------------------------------------------------------------------------

// end Propetizer.r
