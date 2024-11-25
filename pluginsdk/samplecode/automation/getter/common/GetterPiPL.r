// ADOBE SYSTEMS INCORPORATED
// Copyright  1993 - 2002 Adobe Systems Incorporated
// All Rights Reserved
//
// NOTICE:  Adobe permits you to use, modify, and distribute this
// file in accordance with the terms of the Adobe license agreement
// accompanying it.  If you have received this file from a source
// other than Adobe, then your use, modification, or distribution
// of it requires the prior written permission of Adobe.

#define plugInName "Getter"
#define plugInCopyrightYear	"2000"
#define plugInDescription \
	"An Actions Module to get info from Adobe Photoshop¨."

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

resource 'PiPL' ( 16000, "Getter", purgeable)
	{
		{
		Kind { Actions },
		Name { plugInName },
		Category { "AdobeSDK" },
		Version { (latestActionsPlugInVersion << 16) | latestActionsPlugInSubVersion },
				
		Component { ComponentNumber, plugInName },

		#if Macintosh
			#if defined(__arm64__)
				CodeMacARM64 { "AutoPluginMain" },
			#endif
			#if defined(__x86_64__)
				CodeMacIntel64 { "AutoPluginMain" },
			#endif
		#elif MSWindows
			CodeEntryPointWin64 { "AutoPluginMain" },
		#endif

		HasTerminology
			{ 
			'get ', 
			'getr', 
			16000, 
			"459ac2e6-82d1-11d5-9879-00b0d0204936"
			},
		}
	};

//-------------------------------------------------------------------------------
//	Dictionary (scripting) resource
//-------------------------------------------------------------------------------

resource 'aete' (16000, "Getter dictionary", purgeable)
	{
	1, 0, english, roman,					/* aete version and language specifiers */
		{
		"Testing",							/* vendor suite name */
		"Adobe example plug-ins",			/* optional description */
		'get ',						/* suite ID */
		1,									/* suite code, must be 1 */
		1,									/* suite level, must be 1 */
			{								/* structure for automation */
			plugInName,						/* name */
			"No comment",					/* optional description */
			'get ',					/* class ID, must be unique or Suite ID */
			'getr',					/* event ID, must be unique */

			NO_REPLY,						/* never a reply */
			IMAGE_DIRECT_PARAMETER,			/* direct parameter, used by Photoshop */
				{							// filter or selection class here:
				}
			},
			{},	/* non-filter/automation plug-in class here */
			{}, /* comparison ops (not supported) */
			{ // Enumerations go here:
			}	/* end of any enumerations */
		}
	};
// end GetterPiPL.r
