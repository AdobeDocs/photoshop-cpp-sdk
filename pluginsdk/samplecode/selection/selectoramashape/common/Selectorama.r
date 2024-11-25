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
//		Selectorama.r
//
//	Description:
//		This file contains the resource text descriptions for the
//		for the Selection module Selectorama, an example module
//		that just returns a pixel selection.
//
//	Use:
//		Use selection modules to return pixel or path selections on a new
//		layer or the current layer.
//
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
//	Definitions -- Required by include files.
//-------------------------------------------------------------------------------

// The About box and resources are created in PIUtilities.r.
// You can easily override them, if you like.

#define plugInName			"Selectorama"
#define plugInCopyrightYear	"1996"
#define plugInDescription \
	"An example plug-in selection module for Adobe Photoshop¨."

//-------------------------------------------------------------------------------
//	Definitions -- Required by other resources in this rez file.
//-------------------------------------------------------------------------------

// Dictionary (aete) resources:

#define vendorName			"AdobeSDK"
#define plugInAETEComment \
	"selectorama plug-in"

#define plugInSuiteID		'sdk7'
#define plugInClassID		plugInSuiteID
#define plugInEventID		'selM'

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

#include "SelectoramaTerminology.h"	// Terminology for plug-in.

//-------------------------------------------------------------------------------
//	PiPL resource
//-------------------------------------------------------------------------------

resource 'PiPL' (ResourceID, plugInName " PiPL", purgeable)
{
    {
	    Kind { Selection },
	    Name { plugInName "..." },
	    Category { vendorName },
	    Version { (latestSelectionVersion << 16) | latestSelectionSubVersion },

		Component { ComponentNumber, plugInName },

	    HasTerminology { plugInClassID, plugInEventID, ResourceID, vendorName " " plugInName },
		/* classID, eventID, aete ID, uniqueString */

		EnableInfo { "true" },

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

	}
};

//-------------------------------------------------------------------------------
//	Dictionary (scripting) resource
//-------------------------------------------------------------------------------

resource 'aete' (ResourceID, plugInName " dictionary", purgeable)
{
	1, 0, english, roman,					/* aete version and language specifiers */
	{
		vendorName,							/* vendor suite name */
		"Adobe example plug-ins",			/* optional description */
		plugInSuiteID,						/* suite ID */
		1,									/* suite code, must be 1 */
		1,									/* suite level, must be 1 */
		{									/* structure for filters */
			vendorName " selectorama",		/* unique selection name */
			plugInAETEComment,				/* optional description */
			plugInClassID,					/* class ID, must be unique or Suite ID */
			plugInEventID,					/* event ID, must be unique */

			NO_REPLY,						/* never a reply */
			IMAGE_DIRECT_PARAMETER,			/* direct parameter, used by Photoshop */
			{												/* parameters here, if any */
				"select area",								/* parameter name */
				keyMyArea,									/* parameter key ID */
				typeMySelect,									/* parameter type ID */
				"selection area",							/* optional description */
				flagsEnumeratedParameter,					/* parameter flags */
				
				"amount",									/* parameter name */
				keyMyAmount,								/* parameter key ID */
				typeFloat,									/* parameter type ID */
				"random amount",							/* optional description */
				flagsSingleParameter,						/* parameter flags */

				"use channels from",						/* parameter name */
				keyMyChannels,								/* key ID */
				typeMyComposite,							/* type ID */
				"use composite target or merged",			/* optional descript */
				flagsEnumeratedParameter,
				
				"create",									/* parameter name */
				keyMyCreate,								/* parameter key ID */
				typeMyCreate,								/* parameter type ID */
				"create type",								/* optional description */
				flagsEnumeratedParameter					/* parameter flags */


			}
		},
		{},	/* non-filter plug-in class here */
		{}, /* comparison ops (not supported) */
		{													/* enumerations */
			typeMySelect,									/* type select 'tseL' */
			{
				"minimum",									/* first value */
				selectMin,					 				/* 'seL0' */
				"select minimum",							/* optional description */
				
				"maximum",									/* second value */
				selectMax,									/* 'seL1' */
				"select maximum",							/* optional description */

				"random",									/* third value */
				selectRandom,				 				/* 'seL2' */
				"select random"								/* optional description */
			},
			
			typeMyComposite,								/* type 'tcoM' */
			{
				"target",									/* use target composite */
				useTarget,									/* 'useT' */
				"use target composite",						/* optional descript */
				
				"merged",									/* use merged channels */
				useMerged,									/* 'useM' */
				"use merged composite"						/* optional descript */
			},

			typeMyCreate,									/* type shape 'tshP' */
			{
				"selection",								/* first value */
				createSelection,				 			/* 'crE0' */
				"make selection",							/* optional description */
				
				"path",										/* second value */
				createMaskpath,								/* 'crE1' */
				"make path",								/* optional description */

				"layer",									/* third value */
				createLayer,				 				/* 'crE2' */
				"make layer"								/* optional description */
			}			
		}	/* other enumerations */
	}
};


// end Selectorama.r
