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
//-------------------------------------------------------------------------------
//
//	File:
//		PoorMansTypeTool.r
//
//	Description:
//		This file contains the source and routines for the
//		Filter module Poor Man's Type Tool, a module that 
//		uses the Channel Ports Suite for pixel munging.
//
//	Use:
//		This is a basic module to exemplify all the typical
//		functions a filter module will do: Read scripting
//		parameters, show a user interface, display a proxy,
//		write changed pixel information, and then write
//		scripting parameters.
//
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
//	Definitions -- Required by include files.
//-------------------------------------------------------------------------------

// The About box and resources are created in PIUtilities.r.
// You can easily override them, if you like.

#define plugInName			"Poor Man's Type Tool"
#define plugInCopyrightYear	"1999"
#define	plugInDescription \
	"An example plug-in filter module for Adobe Photoshop¨."

//-------------------------------------------------------------------------------
//	Definitions -- Required by other resources in this rez file.
//-------------------------------------------------------------------------------

// Dictionary (aete) resources:

#define vendorName			"AdobeSDK"
#define plugInAETEComment 	"poormanstypetool example filter plug-in"

#define plugInSuiteID		'sdK1'
#define plugInClassID		plugInSuiteID
#define plugInEventID		plugInClassID

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

#include "PoorMansTypeToolTerminology.h"	// Terminology for this plug-in.

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
			"98b5a608-46ce-11d3-bd6b-0060b0a13dc4"
		},
		
		SupportedModes
		{
			noBitmap, doesSupportGrayScale,
			noIndexedColor, doesSupportRGBColor,
			doesSupportCMYKColor, doesSupportHSLColor,
			doesSupportHSBColor, doesSupportMultichannel,
			doesSupportDuotone, doesSupportLABColor
		},
			
		EnableInfo { "in (PSHOP_ImageMode, GrayScaleMode, RGBMode,"
					     "CMYKMode, HSLMode, HSBMode, MultichannelMode,"
					     "DuotoneMode, LabMode)" },

		WantsScrap { },

		FilterCaseInfo
		{
			{
				/* Flat data, no selection */
				inWhiteMat, outWhiteMat,
				doNotWriteOutsideSelection,
				filtersLayerMasks, worksWithBlankData,
				doNotCopySourceToDestination,
					
				/* Flat data with selection */
				inWhiteMat, outWhiteMat,
				writeOutsideSelection,
				filtersLayerMasks, worksWithBlankData,
				doNotCopySourceToDestination,
				
				/* Floating selection */
				inWhiteMat, outWhiteMat,
				writeOutsideSelection,
				filtersLayerMasks, worksWithBlankData,
				doNotCopySourceToDestination,
					
				/* Editable transparency, no selection */
				inWhiteMat, outWhiteMat,
				doNotWriteOutsideSelection,
				filtersLayerMasks, worksWithBlankData,
				doNotCopySourceToDestination,
					
				/* Editable transparency, with selection */
				inWhiteMat, outWhiteMat,
				writeOutsideSelection,
				filtersLayerMasks, worksWithBlankData,
				doNotCopySourceToDestination,
					
				/* Preserved transparency, no selection */
				inWhiteMat, outWhiteMat,
				doNotWriteOutsideSelection,
				filtersLayerMasks, worksWithBlankData,
				doNotCopySourceToDestination,
					
				/* Preserved transparency, with selection */
				inWhiteMat, outWhiteMat,
				writeOutsideSelection,
				filtersLayerMasks, worksWithBlankData,
				doNotCopySourceToDestination
			}
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
			plugInName,										/* unique filter name */
			plugInAETEComment,								/* optional description */
			plugInClassID,									/* class ID, must be unique or Suite ID */
			plugInEventID,									/* event ID, must be unique to class ID */
			
			NO_REPLY,										/* never a reply */
			IMAGE_DIRECT_PARAMETER,							/* direct parameter, used by Photoshop */
			{												/* parameters here, if any */
				"Horizontal",								/* parameter name */
				keyHorizontal,								/* parameter key ID */
				typeInteger,								/* parameter type ID */
				"horizontal position",						/* optional description */
				flagsSingleParameter,						/* parameter flags */
				
				"Vertical",									// second parameter
				keyVertical,								// parameter key ID
				typeInteger,								// parameter type ID
				"vertical position",						// optional description
				flagsSingleParameter,						// parameter flags

				"Size",										// third parameter
				keyXFactor,									// parameter key ID
				typeInteger,								// parameter type ID
				"font size",								// optional description
				flagsSingleParameter,						// parameter flags

				"Gaussian Blur",						// fourth parameter
				keyGaussianBlurData,						// key ID
				typeBoolean,								// type
				"Gaussian Blur",							// optional desc
				flagsSingleParameter						// parameter flags

			}
		},
		{													/* non-filter plug-in class here */
		},
		{													/* comparison ops (not supported) */
		},
		{													/* any enumerations */
		}
	}
};


//-------------------------------------------------------------------------------
//	FltD -- Animation resource for use in Premiere
//-------------------------------------------------------------------------------

resource 'FltD' (ResourceID, plugInName " Premiere FltD", purgeable)
{
	{
		pdShort, 0,		// Interpolate the dissolve amount
	};
};
// end PoorMansTypeTool.r