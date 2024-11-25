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
//		ColorMunger.r
//
//	Description:
//		This file contains the resource information
//		for the Filter module ColorMunger, a module
//		showing the use of the Color Services suite.
//
//	Use:
//		This module takes a source color of any color space
//		and converts it to a target color in any color
//		space.  It shows how to convert colors as well as
//		pop the color picker.  It appears in
//		Filters>>Utilities>>ColorMunger.
//
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
//	Definitions -- Required by include files.
//-------------------------------------------------------------------------------

// The About box and resources are created in PIUtilities.r.
// You can easily override them, if you like.

#define plugInName			"ColorMunger"
#define plugInCopyrightYear	"1996"
#define	plugInDescription \
	"A plug-in filter using Color Services in Adobe Photoshop¨."

//-------------------------------------------------------------------------------
//	Definitions -- Required by other resources in this rez file.
//-------------------------------------------------------------------------------

// Dictionary (aete) resources:

#define vendorName			"AdobeSDK"
#define plugInAETEComment 	"dissolvesans example filter plug-in"

#define plugInSuiteID		'sdKB'
#define plugInClassID		plugInSuiteID
#define plugInEventID		'clrM'

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

#include "ColorMungerTerminology.h"	// Terminology for this plug-in.

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

		HasTerminology
		{
			plugInClassID,					// Class ID
			plugInEventID,					// Event ID
			ResourceID,						// AETE ID
			// Unique string or empty for AppleScript compliant:
			""
		},
		
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
			"DuotoneMode, LabMode)"
		},

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
		},
			
		// PhotoDeluxe PiPL property:
		ButtonIcon
		{
			cicn,				// Macintosh icon type
			ICON, 				// Windows icon type
			ResourceID,			// Icon resource ID
			plugInName	 		// Button text display
		}

	}
};

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
		{													/* structure for filters */
			vendorName " " plugInName,						/* unique filter name */
			plugInAETEComment,								/* optional description */
			plugInClassID,									/* class ID, must be unique or Suite ID */
			plugInEventID,									/* event ID, must be unique */
			
			NO_REPLY,										/* never a reply */
			IMAGE_DIRECT_PARAMETER,							/* direct parameter, used by Photoshop */
			{												/* parameters here, if any */
				"color",									/* parameter name */
				keyColor,									/* parameter key ID */
				typeColorName,								/* parameter type ID */
				"source color",								/* optional description */
				flagsSingleParameter,						/* parameter flags */				

				"colorize",									/* parameter name */
				keyColorize,								/* parameter key ID */
				typeColorName,								/* parameter type ID */
				"target color",								/* optional description */
				flagsSingleParameter,						/* parameter flags */
				
				"foreground color",							// you get the idea
				keyForegroundColor,
				classColor,
				"initial color",
				flagsSingleParameter				
			}
		},
		{},													/* non-filter plug-in class here */
		{},													/* comparison ops (not supported) */
		{													/* any enumerations */
			typeColorName,									// type color name
			{
				"RGB",
				cnRGB,
				"Red-Green-Blue",
				
				"HSB",
				cnHSB,
				"Hue-Saturation-Brightness",
				
				"CMYK",
				cnCMYK,
				"Cyan-Magenta-Yellow-Black",
				
				"Lab",
				cnLab,
				"Luminance-A-B",
				
				"Gray",
				cnGray,
				"Grayscale",
				
				"HSL",
				cnHSL,
				"Hue-Saturation-Luminance",
				
				"XYZ",
				cnXYZ,
				"X-Y-Z"
			}
				
		}

	}
};

//-------------------------------------------------------------------------------
//	Resource text entries
//
//	Entering these as separate resources because then they
//	transfer directly over to windows via CNVTPIPL.
//
//	If I use a Macintosh 'STR#' resource, I could put all these
//	strings into a single resource, but there is no
//	parallel on Windows.  'STR ' resources, which hold
//	one string per ID, exist on Windows and Macintosh.
//-------------------------------------------------------------------------------

resource StringResource (kPromptSource, plugInName " Prompt Source", purgeable)
{
	"Pick a source color:"
};

resource StringResource (kPromptTarget, plugInName " Prompt Target", purgeable)
{
	"Pick a target color:"
};

//-------------------------------------------------------------------------------
//	FltD -- Animation resource for use in Premiere
//-------------------------------------------------------------------------------

resource 'FltD' (ResourceID, plugInName " Premiere FltD", purgeable)
{
	{
		pdShort, 0,		// Interpolate the foreground colors
		pdShort, 0,
		pdShort, 0,
		pdShort, 0,
	};
};

//-------------------------------------------------------------------------------

// end ColorMunger.r
