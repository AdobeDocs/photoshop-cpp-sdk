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
//		Shape.r
//
//	Description:
//		This file contains the source and functions for the
//		Import module Shape, which returns a path, layer,
//		or selection with an interesting shape.
//
//	Use:
//		This module specifically works the path return
//		functionality of Photoshop.  The Paths are stored
//		by derezing a work path saved in Photoshop then
//		storing them in a "Path" resource, which is a
//		binary data resource of a "Path" as defined in the
//		"Path layout" section of the Photosop File Format.
//
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
//	Definitions -- Required by include files.
//-------------------------------------------------------------------------------

// The About box and resources are created in PIUtilities.r.
// You can easily override them, if you like.

#define plugInName1			"Shape"
#define plugInCopyrightYear	"1996"
#define plugInDescription \
	"An example plug-in selection module for Adobe Photoshop¨."

//-------------------------------------------------------------------------------
//	Definitions -- Required by other resources in this rez file.
//-------------------------------------------------------------------------------

// Dictionary (aete) resources:

#define vendorName			"AdobeSDK"
#define plugInAETEComment1 	"nearestbase example color picker plug-in"

#define plugInEventID1		'shpE'
#define ResourceID1			16482
#define DialogID1			16500
//-------------------------------------------------------------------------------
//	Set up included files for Macintosh and Windows.
//-------------------------------------------------------------------------------

#include "ShapeTerminology.h"	// Terminology for this plug-in.
#include "PIResourceDefines.h"

//-------------------------------------------------------------------------------
//	PiPL resource
//-------------------------------------------------------------------------------

resource 'PiPL' (ResourceID1, plugInName1 " PiPL", purgeable)
{
    {
	    Kind { Selection },
	    Name { plugInName1 "..." },
	    Category { vendorName },
	    Version { (latestSelectionVersion << 16) | latestSelectionSubVersion },
		
		Component { ComponentNumber, plugInName1 },

		// ClassID, eventID, aete ID, uniqueString:
	    HasTerminology { plugInClassID, plugInEventID1, ResourceID1, vendorName " " plugInName1 },
		/* classID, eventID, aete ID, uniqueString (presence of uniqueString makes it
		   non-AppleScript */

		EnableInfo { "true" },

		#if Macintosh
			#if defined(__arm64__)
				CodeMacARM64 { "PluginMain1" },
			#endif
			#if defined(__x86_64__)
				CodeMacIntel64 { "PluginMain1" },
			#endif
		#elif MSWindows
			CodeEntryPointWin64 { "PluginMain1" },
		#endif

	}
};

//-------------------------------------------------------------------------------
//	Dictionary (scripting) resource
//-------------------------------------------------------------------------------

resource 'aete' (ResourceID1, plugInName1 " dictionary", purgeable)
{
	1, 0, english, roman,					/* aete version and language specifiers */
	{
		vendorName,							/* vendor suite name */
		"Adobe example plug-ins",			/* optional description */
		plugInSuiteID,							/* suite ID */
		1,									/* suite code, must be 1 */
		1,									/* suite level, must be 1 */
		{									/* structure for filters */
			vendorName " " plugInName1,		/* unique selection name */
			plugInAETEComment1,				/* optional description */
			plugInClassID,					/* class ID, must be unique or Suite ID */
			plugInEventID1,					/* event ID, must be unique */

			NO_REPLY,						/* never a reply */
			IMAGE_DIRECT_PARAMETER,			/* direct parameter, used by Photoshop */
			{								/* parameters here, if any */
				"shape",					/* parameter name */
				keyMyShape,					/* parameter key ID */
				typeMyShape,				/* parameter type ID */
				"shape type",				/* optional description */
				flagsEnumeratedParameter,	/* parameter flags */
	
				"create",					/* parameter name */
				keyMyCreate,				/* parameter key ID */
				typeMyCreate,				/* parameter type ID */
				"create type",				/* optional description */
				flagsEnumeratedParameter	/* parameter flags */

			}
		},
		{},	/* non-filter plug-in class here */
		{}, /* comparison ops (not supported) */
		{									/* enumerations */
			typeMyShape,					/* type shape 'tshP' */
			{
				"triangle",					/* first value */
				shapeTriangle,				/* 'shP0' */
				"triangle path",			/* optional description */
				
				"square",					/* second value */
				shapeSquare,				/* 'shP1' */
				"square path",				/* optional description */

				"circle",					/* third value */
				shapeCircle,				/* 'shP2' */
				"circle path",				/* optional description */

				"star",						/* fourth value */
				shapeStar,	 				/* 'shP3' */
				"star path",				/* optional description */
				
				"treble",					/* fifth value */
				shapeTreble,				/* 'shP4' */
				"treble path",				/* optional description */

				"ribbon",					/* sixth value */
				shapeRibbon,				/* 'shP5' */
				"ribbon path",				/* optional description */

				"note",						/* seventh value */
				shapeNote,					/* 'shP6' */
				"note path"					/* optional description */
			},

			typeMyCreate,					/* type shape 'tshP' */
			{
				"selection",				/* first value */
				createSelection,			/* 'crE0' */
				"make selection",			/* optional description */
				
				"path",						/* second value */
				createMaskpath,				/* 'crE1' */
				"make mask path",			/* optional description */

				"layer",					/* third value */
				createLayer,				/* 'crE2' */
				"make layer"				/* optional description */
			}
		}	/* any enumerations */
	}
};

// end Shape.r
