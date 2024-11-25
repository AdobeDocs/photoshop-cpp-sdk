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
//		HiddenPiPL.r
//
//	Description:
//		Utility plug in for AutomationFilter this guy gets and sets pixels.
//		on command.
//
//-------------------------------------------------------------------------------
#include "PIDefines.h"
#include "PIResourceDefines.h"

#include "HiddenCommands.h"

#define plugInName "Hidden"
#define plugInDescription "A Hidden filter plug in for Adobe Photoshop¨."

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

resource 'PiPL' ( 16000, "Hidden", purgeable )
{
	{
		Kind { Filter },
		Name { "Hidden" },
		Category { "**Hidden**" },
		Version { (latestFilterVersion << 16) | latestFilterSubVersion },

		Component { ComponentNumber, "Hidden" },

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

		EnableInfo { "true" },
		HasTerminology { plugInClassID,	
						 plugInEventID, 
						 16000, 
						 HiddenUniqueString },
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


resource 'aete' ( 16000, " dictionary", purgeable)
{
	1, 0, english, roman,									/* aete version and language specifiers */
	{
		"tommy",											/* vendor suite name */
		"Adobe example plug-ins",							/* optional description */
		plugInSuiteID,										/* suite ID */
		1,													/* suite code, must be 1 */
		1,													/* suite level, must be 1 */
		{													/* structure for filters */
			"Hidden",										/* unique filter name */
			"no commment  ",								/* optional description */
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
				
				"command",								// second parameter
				keyCommand,								// parameter key ID
				typeCommand,									// parameter type ID
				"command string",						// optional description
				flagsEnumeratedParameter,					// parameter flags
				
				"name",
				keyChannelName,
				typeChar,
				"channel name",
				flagsSingleParameter

			}
		},
		{													/* non-filter plug-in class here */
		},
		{													/* comparison ops (not supported) */
		},
		{													/* any enumerations */
			typeCommand,										
			{
				"Info",									
				enumInfo,							
				"",								
				
				"Write",										
				enumWrite,							
				""
			}
				
		}
	}
};

