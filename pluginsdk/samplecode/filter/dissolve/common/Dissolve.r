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
#include "PIDefines.h"
#include "PIResourceDefines.h"

#ifdef __PIMac__
	#include <Carbon.r>
	#include "PIGeneral.r"
	#include "DissolveScripting.h"
	#include "PIUtilities.r"
#elif defined(__PIWin__)
	#define Rez
	#include "DissolveScripting.h"
	#include "PIGeneral.h"
	#include "PIUtilities.r"
#endif

#include "PIActions.h"

resource 'PiPL' ( 16000, "Dissolve", purgeable )
{
	{
		Kind { Filter },
		Name { plugInName "..." },
		Category { vendorName },
		Version { (latestFilterVersion << 16 ) | latestFilterSubVersion },

		Component { ComponentNumber, plugInName },

		#if __PIMac__
			#if defined(__arm64__)
				CodeMacARM64 { "PluginMain" },
			#endif
			#if defined(__x86_64__)
				CodeMacIntel64 { "PluginMain" },
			#endif
		#elif __PIWin__
			CodeEntryPointWin64 { "PluginMain" },
		#endif

		SupportedModes
		{
			noBitmap, doesSupportGrayScale,
			noIndexedColor, doesSupportRGBColor,
			doesSupportCMYKColor, doesSupportHSLColor,
			doesSupportHSBColor, doesSupportMultichannel,
			doesSupportDuotone, doesSupportLABColor
		},

		HasTerminology
		{
			plugInClassID,
			plugInEventID,
			16000,
			plugInUniqueID
		},
		
		EnableInfo { "in (PSHOP_ImageMode, RGBMode, GrayScaleMode,"
		             "CMYKMode, HSLMode, HSBMode, MultichannelMode,"
					 "DuotoneMode, LabMode, RGB48Mode, Gray16Mode) ||"
					 "PSHOP_ImageDepth == 16 ||"
					 "PSHOP_ImageDepth == 32" },

		PlugInMaxSize { 2000000, 2000000 },
		
		MonitorScalingAware {},

		FilterLayerSupport {doesSupportFilterLayers},
		
		FilterCaseInfo
		{
			{
				/* Flat data, no selection */
				inWhiteMat, outWhiteMat,
				doNotWriteOutsideSelection,
				filtersLayerMasks, worksWithBlankData,
				copySourceToDestination,
					
				/* Flat data with selection */
				inWhiteMat, outWhiteMat,
				writeOutsideSelection,
				filtersLayerMasks, worksWithBlankData,
				copySourceToDestination,
				
				/* Floating selection */
				inWhiteMat, outWhiteMat,
				writeOutsideSelection,
				filtersLayerMasks, worksWithBlankData,
				copySourceToDestination,
					
				/* Editable transparency, no selection */
				inWhiteMat, outWhiteMat,
				doNotWriteOutsideSelection,
				filtersLayerMasks, worksWithBlankData,
				copySourceToDestination,
					
				/* Editable transparency, with selection */
				inWhiteMat, outWhiteMat,
				writeOutsideSelection,
				filtersLayerMasks, worksWithBlankData,
				copySourceToDestination,
					
				/* Preserved transparency, no selection */
				inWhiteMat, outWhiteMat,
				doNotWriteOutsideSelection,
				filtersLayerMasks, worksWithBlankData,
				copySourceToDestination,
					
				/* Preserved transparency, with selection */
				inWhiteMat, outWhiteMat,
				writeOutsideSelection,
				filtersLayerMasks, worksWithBlankData,
				copySourceToDestination
			}
		}	
	}
};

resource 'aete' (16000, "Dissolve dictionary", purgeable)
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
				"amount",									/* parameter name */
				keyAmount,									/* parameter key ID */
				typeFloat,									/* parameter type ID */
				"dissolve amount",							/* optional description */
				flagsSingleParameter,						/* parameter flags */
				
				"disposition",								// second parameter
				keyDisposition,								// parameter key ID
				typeMood,									// parameter type ID
				"dissolve disposition",						// optional description
				flagsEnumeratedParameter,					// parameter flags
				
				"ignore selection",							/* optional parameter */
				keyIgnoreSelection,							/* key ID */
				typeBoolean,								/* type */
				"filter entire image",						/* optional desc */
				flagsSingleParameter						/* parameter flags */

			}
		},
		{													/* non-filter plug-in class here */
		},
		{													/* comparison ops (not supported) */
		},
		{													/* any enumerations */
			typeMood,										/* type disposition 'mooD' */
			{
				"clear",									/* first value */
				dispositionClear,							/* 'moD0' */
				"clear headed",								/* optional description */
				
				"cool",										/* second value */
				dispositionCool,							/* 'moo1' */
				"got the blues",							/* optional description */
				
				"hot",										/* third value */
				dispositionHot,								/* 'moo2' */
				"red-faced",								/* optional description */
				
				"sick",										/* fourth value */
				dispositionSick,							/* 'moo3' */
				"green with envy"							/* optional description */
			}
		}
	}
};
