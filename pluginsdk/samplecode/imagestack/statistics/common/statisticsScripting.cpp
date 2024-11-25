/*
	File: statisticsScripting.c

	(c) Copyright © 2006 Adobe Systems Incorporated.
	All rights reserved.

	C source file for scripting functions for statistics example.
*/

#include "statistics.h"
#include "PITerminology.h"

/*****************************************************************************/
/* Opens up the scripting parameters to begin reading commands. */

void OpenScriptParams (GPtr globals)
{
	DescriptorKeyID				key = 0;
	DescriptorTypeID			type = 0;
	int32						flags = 0;
	PIDescriptorHandle 			descriptor;
	ReadDescriptorProcs			*reader;
	PIReadDescriptor			token;

#ifndef PIGetKey
#define PIGetKey(token, key, type, flags) \
	reader->getKeyProc(token, key, type, flags)
#endif

#ifndef PIGetPinUnitFloat
#define PIGetPinUnitFloat(token, min, max, unit, value) \
	reader->getPinnedUnitFloatProc(token, min, max, unit, value)
#endif

#ifndef PIGetInt
#define PIGetInt(token, value) \
	reader->getIntegerProc(token, value)
#endif

	if (gStuff->descriptorParameters)
	{
	
		descriptor = gStuff->descriptorParameters->descriptor;
		reader =  gStuff->descriptorParameters->readDescriptorProcs;
		
		token = reader->openReadDescriptorProc(descriptor, NULL);
		
		/* don't know how many we're going to be passed, so don't bother with array */
		if (token)
		{
			while (PIGetKey(token, &key, &type, &flags))
			{
/* nothing to do here
				switch (key)
				{
					case keyVertical:
						PIGetPinUnitFloat(token, &minRows, &maxRows, &pixelsUnitPass, &rows);
						gLastRows = (int32)rows;
						break;
					case keyHorizontal:
						PIGetPinUnitFloat(token, &minColumns, &maxColumns, &pixelsUnitPass, &columns);
						gLastCols = (int32)columns;
						break;
					case keyDepth:
						PIGetInt(token, &depth);
						gOutputDepth = (int16)depth;
						break;
				}
*/
			}
		}
		
		// HostCloseReader
		reader->closeReadDescriptorProc(token);
		PIDisposeHandle( gStuff->descriptorParameters->descriptor );
		gStuff->descriptorParameters->descriptor = NULL;
	}
	/* return true if want to show our Dialog */
}

/*****************************************************************************/

/* Checks for any data then writes our parameters to the scripting system. */

OSErr CheckAndWriteScriptParams (GPtr globals)
{
	OSErr	gotErr;
	
	gotErr = gResult;

	if (gResult != userCanceledErr)
		gotErr = WriteScriptParams(globals);

	return gotErr;
}

/*****************************************************************************/

/* Writes our parameters to the scripting system. */

OSErr WriteScriptParams (GPtr globals)
{
	PIWriteDescriptor			token = NULL;
	OSErr						stickyError = noErr;
	WriteDescriptorProcs		*writer;
	PIDescriptorHandle	h = NULL;

#ifndef PIPutUnitFloat
#define PIPutUnitFloat(token, key, unit, value) \
	writer->putUnitFloatProc(token, key, unit, value)
#endif

#ifndef PIPutInt
#define PIPutInt(token, key, value) \
	writer->putIntegerProc(token, key, value)
#endif

	if (gStuff->descriptorParameters)
	{
		writer = gStuff->descriptorParameters->writeDescriptorProcs;
		token = writer->openWriteDescriptorProc();
		if (token)
		{

			// HostCloseWriter
			gStuff->descriptorParameters->writeDescriptorProcs->closeWriteDescriptorProc(token, &h);
			gStuff->descriptorParameters->descriptor = h;
			gStuff->descriptorParameters->recordInfo = plugInDialogOptional;
		}
		
	}
	return stickyError;
}

/*****************************************************************************/
