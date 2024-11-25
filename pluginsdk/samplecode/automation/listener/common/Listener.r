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
//		Listener.r
//
//	Description:
//		Resource information for make new document
//		sample Automation plug-in.
//
//-------------------------------------------------------------------------------


//-------------------------------------------------------------------------------
//	Definitions -- Required by other resources in this rez file.
//-------------------------------------------------------------------------------

// Dictionary (aete) resources:

#define vendorName			"AdobeSDK"
#define plugInAETEComment 	"Listener persistent actions plug-in"

#define plugInSuiteID		'sdKG'
#define plugInClassID		plugInSuiteID
#define plugInEventID		'lstN'

//-------------------------------------------------------------------------------
//	Set up included files for Macintosh and Windows.
//-------------------------------------------------------------------------------

#include "PIDefines.h"
#include "PIResourceDefines.h"

#ifdef __PIMac__
	#include "ListenerTerminology.h"
	#include "PIGeneral.r"
	#include "PIUtilities.r"
#elif defined(__PIWin__)
	#include "ListenerTerminology.h"
	#define Rez
	#include "PIGeneral.h"
	#include "PIUtilities.r"
#endif

#include "PITerminology.h"
#include "PIActions.h"


//-------------------------------------------------------------------------------
//	PiPL resource
//-------------------------------------------------------------------------------

resource 'PiPL' (ListenerResourceID, plugInName " PiPL", purgeable)
	{
		{
		Kind { Actions },
		Name { plugInName "..." },
		Category { vendorName },
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
			plugInClassID, 
			plugInEventID, 
			ListenerResourceID, 
			vendorName " " plugInName	// Unique string.
			},
		
		// Include "Persistent" if you want your plug-in to stay loaded
		// until Photoshop quits:
		Persistent {},
		
		// Messages {} is only honored if your plug-in is Persistent:
		Messages
			{
			startupRequired,
			doesNotPurgeCache,
			shutdownRequired,
			acceptProperty
			},
		
		// If you want this on all the time, remove the EnableInfo property (such
		// as for help menu entries.)  To have this on according to document
		// open, close, and mode guidelines, provide a minimal EnableInfo, such
		// as the one here:
		// I'm going to comment EnableInfo out so that this plug-in
		// is available all the time (absence of EnableInfo = always available).
		// EnableInfo { "true" },
		
		}
	};

//-------------------------------------------------------------------------------
//	Dictionary (scripting) resource
//-------------------------------------------------------------------------------

resource 'aete' (ListenerResourceID, plugInName " dictionary", purgeable)
	{
	1, 0, english, roman,					/* aete version and language specifiers */
		{
		vendorName,							/* vendor suite name */
		"Adobe example plug-ins",			/* optional description */
		plugInSuiteID,						/* suite ID */
		1,									/* suite code, must be 1 */
		1,									/* suite level, must be 1 */
			{								/* structure for automation */
			plugInName,						/* name */
			plugInAETEComment,				/* optional description */
			plugInClassID,					/* class ID, must be unique or Suite ID */
			plugInEventID,					/* event ID, must be unique */

			NO_REPLY,						/* never a reply */
			IMAGE_DIRECT_PARAMETER,			/* direct parameter, used by Photoshop */
				{							// filter or selection class here:
				// name:
				"action set",
				// key ID:
				keyPIActionSet,
				// type ID:
				typeChar,
				// optional description:
				"",
				// flags:
				flagsSingleParameter,

				// name:
				"action",
				// key ID:
				keyPIAction,
				// type ID:
				typeChar,
				// optional description:
				"",
				// flags:
				flagsSingleParameter,

				// id:
				"action ID",
				// key ID:
				keyPIActionID,
				// type ID:
				typeInteger,
				// optional description:
				"",
				// flags:
				flagsSingleParameter

				}
			},
			{},	/* non-filter/automation plug-in class here */
			{}, /* comparison ops (not supported) */
			{ // Enumerations go here:
			}	/* end of any enumerations */
		}
	};

resource 'STR ' (kListenerAboutID, plugInName " About Text", purgeable)
{
	plugInName "\n\n"
	"Version " VersionString " "
	"Release " ReleaseString "\n"
	"\u00A9 " plugInCopyrightYear "-" CurrentYear ", Adobe. All rights reserved.\n"
	"All Rights Reserved.\n\n"
	plugInDescription
};
//-------------------------------------------------------------------------------

// end Listener.r
