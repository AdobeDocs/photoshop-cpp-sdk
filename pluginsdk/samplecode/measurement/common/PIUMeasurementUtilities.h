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

#ifndef __PIMeasurementUtilities__
#define __PIMeasurementUtilities__

#include "PIMeasurement.h"

/******************************************************************************/

/**
 * Safe conversion from MeasurementRegisterRecord to MeasurementBaseRecord.
 *
 * \param inRecord the record to convert from
 * \return the converted record
 */
MeasurementBaseRecord *AsBase (MeasurementRegisterRecord *inRecord);

/**
 * Safe conversion from MeasurementPrepareRecord to MeasurementBaseRecord.
 *
 * \param inRecord the record to convert from
 * \return the converted record
 */
MeasurementBaseRecord *AsBase (MeasurementPrepareRecord *inRecord);

/**
 * Safe conversion from MeasurementRecordRecord to MeasurementBaseRecord.
 *
 * \param inRecord the record to convert from
 * \return the converted record
 */
MeasurementBaseRecord *AsBase (MeasurementRecordRecord *inRecord);

/**
 * Safe conversion from MeasurementExportRecord to MeasurementBaseRecord.
 *
 * \param inRecord the record to convert from
 * \return the converted record
 */
MeasurementBaseRecord *AsBase (MeasurementExportRecord *inRecord);

/**
 * Safe conversion from MeasurementRecordRecord to MeasurementPrepareRecord.
 *
 * \param inRecord the record to convert from
 * \return the converted record
 */
MeasurementPrepareRecord *AsPrepare (MeasurementRecordRecord *inRecord);

/******************************************************************************/

/**
 * Get the minimum and maximum values of an 8-bit pixel.  Used in pixel depth
 * agnostic template functions.
 *
 * \param outMinimum on return, the minimum pixel value
 * \param outMaximum on return, the maximum pixel value
 */
void GetPixelValueLimits (uint8 &outMinimum, uint8 &outMaximum);

/**
 * Get the minimum and maximum values of an 16-bit pixel.  Used in pixel depth
 * agnostic template functions.
 *
 * \param outMinimum on return, the minimum pixel value
 * \param outMaximum on return, the maximum pixel value
 */
void GetPixelValueLimits (uint16 &outMinimum, uint16 &outMaximum);

/**
 * Get the minimum and maximum values of an 32-bit pixel.  Used in pixel depth
 * agnostic template functions.
 *
 * \param outMinimum on return, the minimum pixel value
 * \param outMaximum on return, the maximum pixel value
 */
void GetPixelValueLimits (float &outMinimum, float &outMaximum);

/******************************************************************************/

/**
 * Convert an 8-bit pixel value to an 8-bit pixel value.  Effectively, a nop.
 * Used in pixel depth agnostic template functions.
 *
 * \param inValue the 8-bit pixel value to pass through
 * \return the 8-bit pixel value unchanged
 */
inline uint8 To8Bit (uint8 inValue);

/**
 * Convert a 16-bit pixel value (0 - 32768) to an 8-bit pixel value (0 - 255).
 * Used in pixel depth agnostic template functions.
 *
 * \param inValue the 16-bit pixel value to convert
 * \return the 8-bit pixel value converted
 */
inline uint8 To8Bit (uint16 inValue);

/**
 * Convert a 32-bit pixel value (0.0 - 1.0 floating point) to an 8-bit pixel 
 * value (0 - 255).  Used in pixel depth agnostic template functions.
 *
 * \param inValue the 32-bit pixel value to convert
 * \return the 8-bit pixel value converted
 */
inline uint8 To8Bit (float inValue);

/******************************************************************************/

#endif	/* __PIMeasurementUtilities__ */
