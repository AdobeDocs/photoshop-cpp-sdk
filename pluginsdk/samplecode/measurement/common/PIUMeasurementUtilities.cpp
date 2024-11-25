// ADOBE SYSTEMS INCORPORATED
// Copyright 2007 Adobe Systems Incorporated
// All Rights Reserved
//
// NOTICE:  Adobe permits you to use, modify, and distribute this 
// file in accordance with the terms of the Adobe license agreement
// accompanying it.  If you have received this file from a source
// other than Adobe, then your use, modification, or distribution
// of it requires the prior written permission of Adobe.
//-------------------------------------------------------------------------------

#include "PIUMeasurementUtilities.h"

#include "PIUExceptions.h"

/******************************************************************************/

MeasurementBaseRecord *AsBase (MeasurementRegisterRecord *inRecord)
	{
	REQUIRE_NON_NULL_PARAMETER (inRecord);
	REQUIRE_NON_NULL (inRecord->base);
	
	return inRecord->base;
	}
	
/******************************************************************************/

MeasurementBaseRecord *AsBase (MeasurementPrepareRecord *inRecord)
	{
	REQUIRE_NON_NULL_PARAMETER (inRecord);
	REQUIRE_NON_NULL (inRecord->base);
	
	return inRecord->base;
	}
	
/******************************************************************************/

MeasurementBaseRecord *AsBase (MeasurementRecordRecord *inRecord)
	{
	MeasurementPrepareRecord		*prepare;
	
	prepare = AsPrepare (inRecord);
	REQUIRE_NON_NULL (prepare->base);
	
	return prepare->base;
	}
	
/******************************************************************************/

MeasurementBaseRecord *AsBase (MeasurementExportRecord *inRecord)
	{
	REQUIRE_NON_NULL_PARAMETER (inRecord);
	REQUIRE_NON_NULL (inRecord->base);
	
	return inRecord->base;
	}
	
/******************************************************************************/

MeasurementPrepareRecord *AsPrepare (MeasurementRecordRecord *inRecord)
	{
	REQUIRE_NON_NULL_PARAMETER (inRecord);
	REQUIRE_NON_NULL (inRecord->prepare);
	
	return inRecord->prepare;
	}
	
/******************************************************************************/

void GetPixelValueLimits (uint8 &outMinimum, uint8 &outMaximum)
	{
	outMinimum = 0;
	outMaximum = 255; 
	}

/******************************************************************************/

void GetPixelValueLimits (uint16 &outMinimum, uint16 &outMaximum)
	{
	outMinimum = 0;
	outMaximum = 32768; 
	}

/******************************************************************************/

void GetPixelValueLimits (float &outMinimum, float &outMaximum)
	{
	outMinimum = 0.0;
	outMaximum = 1.0; 
	}

/******************************************************************************/

inline uint8 To8Bit (uint8 inValue)
	{
	return inValue;
	}
	
/******************************************************************************/

inline uint8 To8Bit (uint16 inValue)
	{
	return (uint8)(((inValue << 8) - inValue + 16384) >> 15);
	}
	
/******************************************************************************/

inline uint8 To8Bit (float inValue)
	{
	return (uint8)(255.0 * inValue + 0.5);
	}

