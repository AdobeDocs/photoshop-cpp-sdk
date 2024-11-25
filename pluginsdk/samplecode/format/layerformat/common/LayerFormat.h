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
//		LayerFormat.h
//
//	Description:
//		This file contains the header prototypes and macros for the
//		File Format module LayerFormat, 
//		which writes a flat file with merged document pixels.
//
//	Use:
//		Format modules are called from the Save, Save as,
//		and Save a copy dialogs.
//
//-------------------------------------------------------------------------------

#ifndef __LayerFormat_H__	// Has this not been defined yet?
#define __LayerFormat_H__	// Only include this once by predefining it

#include "PIDefines.h"
#include "PIFormat.h"					// Format Photoshop header file.
#include "PIUtilities.h"				// SDK Utility library.
#include "FileUtilities.h"				// File Utility library.
#include "LayerFormatTerminology.h"	// Terminology for plug-in.
#include <string>
#include <vector>

//-------------------------------------------------------------------------------
//	Structure -- FileHeader
//-------------------------------------------------------------------------------

typedef struct HeaderVer2
{
	char identifier [8];
	
	int16 endian;
	int16 testendian;
	
	int16 mode;
	int16 depth;
	int32 rows;
	int32 cols;
	int16 planes;
	
	int32 transparencyPlane;
	
	int32 numLayers;
	
	int32 resourceLength;
} FileHeader;

typedef struct HeaderVer1
{
	int16 mode;
	int16 depth;
	int16 rows;
	int16 cols;
	int16 planes;
	
	int32 resourceLength;
} Header16BitRowsCols;


//-------------------------------------------------------------------------------
//	Data -- structures
//-------------------------------------------------------------------------------

typedef struct Data
{ 
	bool needsSwap;
} Data;
	
typedef struct ResourceInfo {
	uint32 totalSize;
	uint32 type;
	uint16 id;
	std::string name;
	uint32 size;
	bool keep;
} ResourceInfo;

extern SPPluginRef gPluginRef;
extern FormatRecord * gFormatRecord;
extern Data * gData;
extern int16 * gResult;

//-------------------------------------------------------------------------------
//	Prototypes
//-------------------------------------------------------------------------------

void DoAbout (AboutRecordPtr about); 	   		// Pop about box.

bool DoUI (std::vector<ResourceInfo *> & rInfos);

// During read phase:
Boolean ReadScriptParamsOnRead (void);	// Read any scripting params.
OSErr WriteScriptParamsOnRead (void);	// Write any scripting params.

// During write phase:
Boolean ReadScriptParamsOnWrite (void);	// Read any scripting params.
OSErr WriteScriptParamsOnWrite (void);	// Write any scripting params.

//-------------------------------------------------------------------------------

#endif // __LayerFormat_H__
