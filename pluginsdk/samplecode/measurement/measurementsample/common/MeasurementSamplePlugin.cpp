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

#include "PIDefines.h"

#include <map>
#include <string>
#include <vector>

#include "DialogUtilities.h"

#include "PIAbout.h"
#include "PIMeasurement.h"
#include "PIStringTerminology.h"
#include "PIUtilities.h"

#include "PIUActionControl.h"
#include "PIUActionDescriptor.h"
#include "PIUActionList.h"
#include "PIUASZString.h"
#include "PIUExceptions.h"
#include "PIUMeasurementUtilities.h"

#if __PIWin__
typedef unsigned long long uint64;
#define snprintf sprintf_s 
#endif

#define kTileHeight_Default			256
#define kTileWidth_Default			256

#define kMSP_DataPointDataType_PlanePixelsNonZero_Identifier	"ComAdobeMeasurementSamplePlanePixelsNonZero"

#define kMSP_DataPoint_EdgeTouching_Identifier					"ComAdobeMeasurementSampleEdgeTouching"
#define kMSP_DataPoint_Foreground_Identifier					"ComAdobeMeasurementSampleForeground"
#define kMSP_DataPoint_CenterX_Identifier						"ComAdobeMeasurementSampleCenterX"
#define kMSP_DataPoint_CenterY_Identifier						"ComAdobeMeasurementSampleCenterY"
#define kMSP_DataPoint_ImagePixelsNonZero_Identifier			"ComAdobeMeasurementSampleImagePixelsNonZero"
#define kMSP_DataPoint_GrayPixelsNonZero_Identifier				"ComAdobeMeasurementSampleGrayPixelsNonZero"

typedef enum MeasurementSampleDescriptorKey
	{
	kMSDK_EdgeTouching,
	kMSDK_Foreground,
	kMSDK_CenterX,
	kMSDK_CenterY,
	kMSDK_ImagePixelsNonZero,
	kMSDK_GrayPixelsNonZero,
	kMSDK_Last						// Determines size of key list
	}
	MeasurementSampleDescriptorKey;
	
typedef std::map<std::string, MeasurementSampleDescriptorKey>	MeasurementSampleDescriptorMap;

template<class T, class U> class MeasurementSampleData;

template<class T, class U> void RecordMeasurements (
		MeasurementRecordRecord *inRecord);

template<class T, class U> void MeasureTile (
		const MeasurementRecordRecord *inRecord, 
		MeasurementSampleData<T, U> &ioSummaryData, 
		std::vector< MeasurementSampleData<T, U> > &ioFeatureData);

SPBasicSuite *sSPBasic = NULL;

////////////////////////////////////////////////////////////////////////////////

/**
 * Template class the represents captured data for a particular feature (and
 * for the summary data).  The template-ness allows for the class to be used
 * for 8, 16 or 32-bit pixel value data.  The 'T' template class is the pixel
 * value type.  The 'U' template class is the type for summing pixel values.
 */
template<class T, class U> class MeasurementSampleData
	{
	public:
	
		static uint32 GetDataSize (uint16 inImagePlanes);
		
	public:

		MeasurementSampleData (uint16 inImagePlanes);
		virtual ~MeasurementSampleData ();
		
		void Record (int32 inRow, int32 inColumn, uint16 inFeature,
				T *inImageData, T *inGrayData);

		void Add (const MeasurementSampleData &inData);
		
		void Write (const DescriptorKeyID inDescriptorKeys[],
				PIActionDescriptor &outDescriptor);
		
	private:

		void WriteError (PIUActionDescriptor &inDescriptor,
				DescriptorKeyID inKey, const PIUASZString &inMessage);
		
		void WriteImagePixelsNonZero (PIUActionDescriptor &inDescriptor, 
				DescriptorKeyID inKey);
		
	private:

		MeasurementSampleData ();

	private:

		uint16					fImagePlanes;
		bool					fHasData;
		bool					fEdgeTouching;
		bool					fForeground;
		int32					fRowMinimum;
		int32					fRowMaximum;
		int32					fColumnMinimum;
		int32					fColumnMaximum;
		bool					fHasImageData;
		uint64					*fImagePixelsNonZero;
		bool					fHasGrayData;
		uint64					fGrayPixelsNonZero;
		
	};

//////////////////////////////////////////////////////////////////////////////////

template <class T, class U> uint32 MeasurementSampleData<T, U>::GetDataSize (
		uint16 inImagePlanes)
	{
	return sizeof (MeasurementSampleData<T, U>) + sizeof (uint64) * inImagePlanes;
	}

/******************************************************************************/

/**
 * Construct the MeasurementSampleData object for a particular feature.  The only
 * necessary argument is the number of planes in the image.  This is used for
 * the image pixels non zero data.
 *
 * Initialize the row and column to opposite limits.
 *
 * \param inImagePlanes the number of planes in the original image
 */
template <class T, class U> MeasurementSampleData<T, U>::MeasurementSampleData (uint16 inImagePlanes)
	:	fImagePlanes (inImagePlanes),
		fHasData (false),
		fEdgeTouching (false),
		fForeground (false),
		fRowMinimum (INT_MAX),
		fRowMaximum (INT_MIN),
		fColumnMinimum (INT_MAX),
		fColumnMaximum (INT_MIN),
		fHasImageData (false),
		fImagePixelsNonZero (NULL),
		fHasGrayData (false),
		fGrayPixelsNonZero (0)
	{
	}
	
/******************************************************************************/

/**
 * Destruct the object.
 */
template <class T, class U> MeasurementSampleData<T, U>::~MeasurementSampleData ()
	{
	if (fImagePixelsNonZero != NULL)
		{
		delete [] fImagePixelsNonZero;
		fImagePixelsNonZero = NULL;
		}
	}

/******************************************************************************/

/**
 * Record information about a specific pixels known to be in the feature associated
 * with this data object.
 *
 * The pointers to the image and gray pixel data are template based and the types
 * depend upon the depth of the original image.
 *
 * \param inRow the y coordinate of the pixel in the image
 * \param inColumn the x coordinate of the pixel in the image
 * \param inFeature the feature itself (id + background flag + edge touching flag)
 * \param inImageData a pointer to the image pixel data ('fImagePlanes' planes)
 * \param inGrayData a pointer to grayscale pixel data (1 plane)
 */
template <class T, class U> void MeasurementSampleData<T, U>::Record (
		int32 inRow, int32 inColumn, uint16 inFeature,
		T *inImageData, T *inGrayData)
	{
	// We have data
	fHasData = true;
	
	// Is it edge touching?
	fEdgeTouching = ((inFeature & featureMaskEdgeTouchingMask) != 0);
	
	// Is it foreground?
	fForeground = ((inFeature & featureMaskForegroundMask) != 0);
	
	// Account for row and column
	if (fRowMinimum > inRow)
		fRowMinimum = inRow;
	if (fRowMaximum < inRow)
		fRowMaximum = inRow;
	if (fColumnMinimum > inColumn)
		fColumnMinimum = inColumn;
	if (fColumnMaximum < inColumn)
		fColumnMaximum = inColumn;
		
	// If we have any image data
	if (inImageData != NULL)
		{
		// We have image data
		fHasImageData = true;
		
		// Allocate image pixels non zero array, if necessary
		if (fImagePixelsNonZero == NULL)
			{
			fImagePixelsNonZero = new uint64 [fImagePlanes];
			std::memset (fImagePixelsNonZero, 0, sizeof(uint64) * fImagePlanes);
			}
		
		// Account for each plane in the pixel that is non zero	
		for (uint32 index = 0; index < fImagePlanes; index++)
			if (inImageData[index] != 0)
				fImagePixelsNonZero[index]++;
		}

	// If we have any gray data
	if (inGrayData != NULL)
		{
		// We have gray data
		fHasGrayData = true;

		// Account for the pixel that is non zero
		if (*inGrayData != 0)
			fGrayPixelsNonZero++;
		}
	}

/******************************************************************************/

/**
 * Add one MeasurementSampleData object to another.  This is used for calculating
 * the summary MeasurementSampleData object.  Please note that not all fields
 * are truly additive.
 *
 * \param inData the MeasurementSampleData object to add to this object
 */
template <class T, class U> void MeasurementSampleData<T, U>::Add (
		const MeasurementSampleData &inData)
	{
	// If the object doesn't actually have any data, then bail
	if (!inData.fHasData)
		return;
		
	// We have data
	fHasData = true;
	
	// Account for edge touching
	fEdgeTouching |= inData.fEdgeTouching;
	
	// Account for edge touching
	fForeground |= inData.fForeground;

	// Account for row and column, not additive, but expanding
	if (fRowMinimum > inData.fRowMinimum)
		fRowMinimum = inData.fRowMinimum;
	if (fRowMaximum < inData.fRowMaximum)
		fRowMaximum = inData.fRowMaximum;
	if (fColumnMinimum > inData.fColumnMinimum)
		fColumnMinimum = inData.fColumnMinimum;
	if (fColumnMaximum < inData.fColumnMaximum)
		fColumnMaximum = inData.fColumnMaximum;

	// If the object has image data
	if (inData.fHasImageData)
		{
		REQUIRE_NON_NULL (inData.fImagePixelsNonZero);
			
		// We have image data
		fHasImageData = true;
		
		// Allocate image pixels non zero array, if necessary
		if (fImagePixelsNonZero == NULL)
			{
			fImagePixelsNonZero = new uint64 [fImagePlanes];
			std::memset (fImagePixelsNonZero, 0, sizeof(uint64) * fImagePlanes);
			}
		
		// Account for each plane in the pixel that is non zero	
		for (uint32 index = 0; index < fImagePlanes; index++)
			fImagePixelsNonZero[index] += inData.fImagePixelsNonZero[index];
		}
		
	// If the object has gray data
	if (inData.fHasGrayData)
		{
		// We have gray data
		fHasGrayData = true;
		
		// Account for gray pixels non zero
		fGrayPixelsNonZero += inData.fGrayPixelsNonZero;
		}
	}

/******************************************************************************/

/**
 * Write the data captured by this MeasurementSampleData object to a descriptor
 * to be read by the Measurement Log.
 *
 * \param inDescriptorKeys the pre-calculated descriptor keys (rather than repeatedly convert from strings to ID)
 * \param outDescriptor the descriptor the data is added to, make not be allocated if no data stored for this object.
 */
template <class T, class U> void MeasurementSampleData<T, U>::Write (
		const DescriptorKeyID inDescriptorKeys[], 
		PIActionDescriptor &outDescriptor)
	{
	// If we don't have data, then bail
	if (!fHasData)
		return;
	
	// We do NOT own this descriptor (i.e. we shouldn't delete it)
	PIUActionDescriptor		descriptor (outDescriptor, false);
	
	// If there is no descriptor, then make, we release it later if no problems
	if (!descriptor)
		descriptor.Make ();
		
	// Loop through the descriptor keys for the data points
	for (uint32 index = 0; index < kMSDK_Last; index++)
		{
		DescriptorKeyID		descriptorKey;
		
		// Get the descriptor key, if null, then skip
		// typeNull means that we are not to capture this data point
		descriptorKey = inDescriptorKeys[index];
		if (descriptorKey == typeNull)
			continue;
			
		// Switch on the key for the data point
		switch (index)
			{
			case kMSDK_EdgeTouching:
				descriptor.PutBoolean (descriptorKey, fEdgeTouching);
				break;
				
			case kMSDK_Foreground:
				descriptor.PutBoolean (descriptorKey, fForeground);
				break;
				
			case kMSDK_CenterX:
				descriptor.PutFloat (descriptorKey, (fColumnMinimum + fColumnMaximum) / 2);
				break;
				
			case kMSDK_CenterY:
				descriptor.PutFloat (descriptorKey, (fRowMinimum + fRowMaximum) / 2);
				break;
				
			case kMSDK_ImagePixelsNonZero:
				if (!fHasImageData)
					WriteError (descriptor, descriptorKey, "Image mode not supported.");
				else
					WriteImagePixelsNonZero (descriptor, descriptorKey);
				break;
				
			case kMSDK_GrayPixelsNonZero:
				if (!fHasGrayData)
					WriteError (descriptor, descriptorKey, "Image mode not supported.");
				else
					descriptor.PutFloat (descriptorKey, (double)fGrayPixelsNonZero);
				break;

			}
		}
		
	// Release it
	outDescriptor = descriptor.Release ();
	}
	
/******************************************************************************/

/**
 * Write an error to the output descriptor.  An error for a data point is
 * indicated by storing a descriptor of class kmeasurementLogErrorClassStr as
 * the data point.  The error descriptor contains a message ZString for the 
 * kmessageStr key.
 *
 * \param inDescriptor the output descriptor to write the error descriptor to
 * \param inKey the key to use for the data point to write the error descriptor to
 * \param inMessage the message to store in the error descriptor, should be localized since it is display as a tool tip in the Measurement Log
 */
template <class T, class U> void MeasurementSampleData<T, U>::WriteError (
		PIUActionDescriptor &inDescriptor, DescriptorKeyID inKey, 
		const PIUASZString &inMessage)
	{
	REQUIRE_PARAMETER (inDescriptor);
	REQUIRE_PARAMETER (!inMessage.IsEmpty ());
	
	PIUActionDescriptor		errorDescriptor;
	
	// Create the error descriptor
	errorDescriptor.Make ();
	
	// Add the error message
	errorDescriptor.PutZString (kmessageStr, inMessage);
			
	// Add the descriptor
	inDescriptor.PutObject (inKey, 
			PIUActionControl::GetStringID (kmeasurementLogErrorClassStr), 
			errorDescriptor);
	}

/******************************************************************************/

/**
 * Encapsulate the plane pixels non zero data in a descriptor and write to the output
 * descriptor.  The plane pixels non zero descriptor contains a single list of 
 * float values that contain the count of non zero pixesl for each channel.
 *
 * \param inDescriptor the output descriptor to write the histogram descriptor to
 * \param inKey the key to use for the data point to write the histogram descriptor to
 */
template <class T, class U> void MeasurementSampleData<T, U>::WriteImagePixelsNonZero (
		PIUActionDescriptor &inDescriptor, DescriptorKeyID inKey)
	{
	REQUIRE_PARAMETER (inDescriptor);
	REQUIRE_NON_NULL (fImagePixelsNonZero);
	
	PIUActionList				channelList;
	PIUActionDescriptor			imagePixelsNonZeroDescriptor;
	
	// Create a list of channels
	channelList.Make ();
	
	// Add each channel data to this list
	for (uint32 index = 0; index < fImagePlanes; index++)
		channelList.PutFloat ((double)fImagePixelsNonZero[index]);

	// Create the descriptor
	imagePixelsNonZeroDescriptor.Make ();

	// Add the list to the descriptor
	imagePixelsNonZeroDescriptor.PutList (kchannelsStr, channelList);
	
	// Write it
	inDescriptor.PutObject (inKey, 
			PIUActionControl::GetStringID (kmeasurementLogDataPointDataClassStr), 
			imagePixelsNonZeroDescriptor);
	}
	
////////////////////////////////////////////////////////////////////////////////

class MeasurementSamplePlugin
	{
	public:
		
		MeasurementSamplePlugin ();
		virtual ~MeasurementSamplePlugin ();
		
		void HandleSelector (int16 inSelector, void *inRecord);
		
	private:
	
		void HandleSelectorAbout (AboutRecord *inRecord);
		void HandleSelectorRegisterDataPointDataTypes (MeasurementRegisterRecord *inRecord);
		void HandleSelectorRegisterDataPoints (MeasurementRegisterRecord *inRecord);
		void HandleSelectorPrepareMeasurements (MeasurementPrepareRecord *inRecord);
		void HandleSelectorRecordMeasurements (MeasurementRecordRecord *inRecord);
		void HandleSelectorExportMeasurement (MeasurementExportRecord *inRecord);
		
        void WriteFileX (const std::string &inOutput,
                         const XPlatFileSpec *inDataDirectoryX,
                         PIUASZString &outFilename);

	private:

		MeasurementSamplePlugin (MeasurementSamplePlugin &);
		
	};

////////////////////////////////////////////////////////////////////////////////

MeasurementSamplePlugin::MeasurementSamplePlugin ()
	{
	}

/******************************************************************************/

MeasurementSamplePlugin::~MeasurementSamplePlugin () 
	{
	}
	
/******************************************************************************/

/**
 * Simply switch on the selector and cast the record to the appropriate type.
 *
 * /param inSelector the selector to invoke in the plugin
 * /param inRecord the record associated with the selector
 */
void MeasurementSamplePlugin::HandleSelector (int16 inSelector, void *inRecord)
	{
	switch (inSelector)
		{
		case measurementSelectorAbout:
			sSPBasic = ((AboutRecord*)inRecord)->sSPBasic;
			HandleSelectorAbout ((AboutRecord*)inRecord);
			break;
			
		case measurementSelectorRegisterDataPointDataTypes:
			sSPBasic = AsBase ((MeasurementRegisterRecord*)inRecord)->basicSuite;
			HandleSelectorRegisterDataPointDataTypes ((MeasurementRegisterRecord*)inRecord);
			break;
		
		case measurementSelectorRegisterDataPoints:
			sSPBasic = AsBase ((MeasurementRegisterRecord*)inRecord)->basicSuite;
			HandleSelectorRegisterDataPoints ((MeasurementRegisterRecord*)inRecord);
			break;
		
		case measurementSelectorPrepareMeasurements:
			sSPBasic = AsBase ((MeasurementPrepareRecord*)inRecord)->basicSuite;
			HandleSelectorPrepareMeasurements ((MeasurementPrepareRecord*)inRecord);
			break;
			
		case measurementSelectorRecordMeasurements:
			sSPBasic = AsBase ((MeasurementRecordRecord*)inRecord)->basicSuite;
			HandleSelectorRecordMeasurements ((MeasurementRecordRecord*)inRecord);
			break;
			
		case measurementSelectorExportMeasurement:
			sSPBasic = AsBase ((MeasurementExportRecord*)inRecord)->basicSuite;
			HandleSelectorExportMeasurement ((MeasurementExportRecord*)inRecord);
			break;
			
		default:
			ThrowOSErr (paramErr);
		}
	}

/******************************************************************************/

/**
 * Show the about box for this plugin.
 *
 * \param inRecord the record to show the about box with
 */
void MeasurementSamplePlugin::HandleSelectorAbout (AboutRecord *inRecord)
	{
	REQUIRE_NON_NULL_PARAMETER (inRecord);
	
#if __PIWin__
	ShowAbout (inRecord);
#elif __PIMac__
	ShowAbout (AboutID);
#endif
	}

/******************************************************************************/

/**
 * Register the data point data types for this measurement plugin.  A data point
 * data type is defined by a descriptor that has two properties:
 *
 * kIDStr			String	REQUIRED: A unique ID of the data point data type, 
 *							use reverse domain name for uniqueness, for example
 *							ComAdobeMeasurementSampleMyDataPointDataTypeID
 * kparentIDStr		String	REQUIRED: the unique ID of the parent data point 
 *							data type of this data point data type, the parent 
 *							data point data type determines how any data point 
 *							data will be displayed in the Measurement Log UI, 
 *							typically will be 
 *							kPIM_DataPointDataType_General_Identifier, less
 *							typically will be one of the other predefined
 *							data point data types, rarely will be a user-defined
 *							data point data type
 *
 * \param inRecord the record to register with, the registration field is
 * an Action List, a zero or more descriptors to this list that describe
 * your data point data types
 */
void MeasurementSamplePlugin::HandleSelectorRegisterDataPointDataTypes (MeasurementRegisterRecord *inRecord)
	{
	REQUIRE_NON_NULL_PARAMETER (inRecord);
	REQUIRE_NON_NULL_PARAMETER (inRecord->registration);
	
	PIUActionList			list (inRecord->registration, false);
	PIUActionDescriptor		descriptor;
	
	// Define a somewhat made up data point data type and will be a descriptor
	// with a list of integer values.  There will be one integer per plane in 
	// the list that contains the number of non-zero pixels in the plane in
	// the feature.  (Okay, so its made up, but it makes for an easy example.)
	descriptor.Make ();
	descriptor.PutString (kIDStr, kMSP_DataPointDataType_PlanePixelsNonZero_Identifier);
	descriptor.PutString (kparentIDStr, kPIM_DataPointDataType_Generic_Identifier);
	list.PutObject (kmeasurementLogDataPointDataTypeClassStr, descriptor);
	}
	
/******************************************************************************/

/**
 * Register the data points for this measurement plugin. A data point
 * is defined by a descriptor that has several properties:
 *
 * kIDStr			String	REQUIRED: A unique ID of the data point, 
 *							use reverse domain name for uniqueness, for example
 *							ComAdobeMeasurementSampleMyDataPointID
 * ktypeIDStr		String	REQUIRED: The unique ID of the data point data type, 
 *							can be one of the predefined data point data types 
 *							or a previously defined user data point data type
 * kfullNameStr		String	REQUIRED: The full name of the data point
 * kdisplayNameStr	String	OPTIONAL: The name of the data point displayed in
 *							the application UI, if not specified, then 
 *							kfullNameStr is used
 * kabbreviatedNameStr	String	OPTIONAL: The name of the data point displayed
 *							in the application UI when the kdisplayNameStr is
 *							too wide to fully display, if not specified, the
 *							kdisplayNameStr is used
 * kdescriptionStr	String	OPTIONAL: A description of the data point, displayed
 *							is some places as a tooltip, if not specified, then
 *							kdisplayNameStr is used
 * kpowerStr		Float	OPTIONAL: For kPIM_DataPointDataType_Scaled_Identifier
 *							type only to indicate how many powers of the 
 *							measurement scale should be applied to the value
 *							of the data point
 *
 * \param inRecord the record to register with, the registration field is
 * an Action List, a zero or more descriptors to this list that describe
 * your data points
 */
void MeasurementSamplePlugin::HandleSelectorRegisterDataPoints (MeasurementRegisterRecord *inRecord)
	{
	REQUIRE_NON_NULL_PARAMETER (inRecord);
	REQUIRE_NON_NULL_PARAMETER (inRecord->registration);
	
	PIUActionList			list (inRecord->registration, false);
	PIUActionDescriptor		descriptor;

	// This data point is a boolean that indicates whether a feature is an 
	// edge touching feature or not.
	descriptor.Make ();
	descriptor.PutString (kIDStr, kMSP_DataPoint_EdgeTouching_Identifier);
	descriptor.PutString (ktypeIDStr, kPIM_DataPointDataType_Boolean_Identifier);
	descriptor.PutZString (kfullNameStr, PIUASZString ("Edge Touching"));
	descriptor.PutZString (kdisplayNameStr, PIUASZString ("Edge Touching"));
	descriptor.PutZString (kabbreviatedNameStr, PIUASZString ("ET"));
	descriptor.PutZString (kdescriptionStr, PIUASZString ("Edge Touching - Does the feature intersect the edge of the image?"));
	list.PutObject (kmeasurementLogDataPointClassStr, descriptor);

	// This data point is a boolean that indicates whether a feature is
	// foreground feature or not.
	descriptor.Make ();
	descriptor.PutString (kIDStr, kMSP_DataPoint_Foreground_Identifier);
	descriptor.PutString (ktypeIDStr, kPIM_DataPointDataType_Boolean_Identifier);
	descriptor.PutZString (kfullNameStr, PIUASZString ("Foreground"));
	descriptor.PutZString (kdisplayNameStr, PIUASZString ("Foreground"));
	descriptor.PutZString (kabbreviatedNameStr, PIUASZString ("FG"));
	descriptor.PutZString (kdescriptionStr, PIUASZString ("Foreground - Is the feature in the foreground?"));
	list.PutObject (kmeasurementLogDataPointClassStr, descriptor);

	// This data point is a scaled floating point (power of 1) that indicates
	// the horizontal center of a feature.  That is, the midpoint between the
	// left-most and right-most pixel of a feature.
	descriptor.Make ();
	descriptor.PutString (kIDStr, kMSP_DataPoint_CenterX_Identifier);
	descriptor.PutString (ktypeIDStr, kPIM_DataPointDataType_Scaled_Identifier);
	descriptor.PutZString (kfullNameStr, PIUASZString ("Center (X)"));
	descriptor.PutZString (kdisplayNameStr, PIUASZString ("Center (X)"));
	descriptor.PutZString (kabbreviatedNameStr, PIUASZString ("CX"));
	descriptor.PutZString (kdescriptionStr, PIUASZString ("Center (X) - The horizontal center of the feature"));
	descriptor.PutFloat (kpowerStr, 1);
	list.PutObject (kmeasurementLogDataPointClassStr, descriptor);

	// This data point is a scaled floating point (power of 1) that indicates
	// the vertical center of a feature.  That is, the midpoint between the
	// top-most and bottom-most pixel of a feature.
	descriptor.Make ();
	descriptor.PutString (kIDStr, kMSP_DataPoint_CenterY_Identifier);
	descriptor.PutString (ktypeIDStr, kPIM_DataPointDataType_Scaled_Identifier);
	descriptor.PutZString (kfullNameStr, PIUASZString ("Center (Y)"));
	descriptor.PutZString (kdisplayNameStr, PIUASZString ("Center (Y)"));
	descriptor.PutZString (kabbreviatedNameStr, PIUASZString ("CY"));
	descriptor.PutZString (kdescriptionStr, PIUASZString ("Center (Y) - The vertical center of the feature"));
	descriptor.PutFloat (kpowerStr, 1);
	list.PutObject (kmeasurementLogDataPointClassStr, descriptor);

	// This data point is of our user-defined data point data type 
	// ("ComAdobeMeasurementSamplePlanePixelsNonZero", see above) that describes 
	// the number of non-zero pixels in the feature in each plane of the 
	// original image.
	//
	// Note that the kIDStr and ktypeIDStr can be the same value, but do not 
	// have to be.  That is, IDs and type IDs do not share the same namespace.  
	descriptor.Make ();
	descriptor.PutString (kIDStr, kMSP_DataPoint_ImagePixelsNonZero_Identifier);
	descriptor.PutString (ktypeIDStr, kMSP_DataPointDataType_PlanePixelsNonZero_Identifier);
	descriptor.PutZString (kfullNameStr, PIUASZString ("Plane Pixels (Non-Zero)"));
	descriptor.PutZString (kdisplayNameStr, PIUASZString ("Plane Pixels (Non-Zero)"));
	descriptor.PutZString (kabbreviatedNameStr, PIUASZString ("PPNZ"));
	descriptor.PutZString (kdescriptionStr, PIUASZString ("Plane Pixels (Non-Zero) - The number of non-zero pixels in the feature in each plane in the original image"));
	list.PutObject (kmeasurementLogDataPointClassStr, descriptor);

	// This data point is a float point type that describes the number of 
	// non-zero pixels in the feature in the gray scale version of the image.
	descriptor.Make ();
	descriptor.PutString (kIDStr, kMSP_DataPoint_GrayPixelsNonZero_Identifier);
	descriptor.PutString (ktypeIDStr, kPIM_DataPointDataType_Float_Identifier);
	descriptor.PutZString (kfullNameStr, PIUASZString ("Gray Pixels (Non-Zero)"));
	descriptor.PutZString (kdisplayNameStr, PIUASZString ("Gray Pixels (Non-Zero)"));
	descriptor.PutZString (kabbreviatedNameStr, PIUASZString ("GPNZ"));
	descriptor.PutZString (kdescriptionStr, PIUASZString ("Gray Pixels (Non-Zero) - The number of non-zero pixels in the feature in the grayscale image"));
	list.PutObject (kmeasurementLogDataPointClassStr, descriptor);
	}
	
/******************************************************************************/

/**
 * Allow the plugin to prepare for the incoming measurement requests.
 * Typically, the only task done with this selector is to examine the image
 * and measurement settings to determine how much heap memory, if any, will be
 * needed that uses non-Photoshop memory APIs.  The plugin can store this
 * value in the requestSpaceReserve field and Photoshop will attempt to free 
 * this amount of memory prior to invoking the actual record selector.
 *
 * /param inRecord the prepare measurement record
 */
void MeasurementSamplePlugin::HandleSelectorPrepareMeasurements (MeasurementPrepareRecord *inRecord)
	{
	REQUIRE_NON_NULL_PARAMETER (inRecord);

	int16		imageDepth;
	int16		imageModePlanes;
	uint32		sampleDataSize;
	
	// Calculate the size of each sample data object based upon the image depth
	// Since MeasurementSampleData is a template class to deal with different
	// pixels sizes, then size of the data changes based upon the pixel type
	// of the original image
	imageDepth = inRecord->imageDepth;
	imageModePlanes = inRecord->imageModePlanes;
	if (imageDepth == 32)
		sampleDataSize = MeasurementSampleData<float, double>::GetDataSize(imageModePlanes);
	else if (imageDepth == 16)
		sampleDataSize = MeasurementSampleData<uint16, uint64>::GetDataSize(imageModePlanes);
	else
		sampleDataSize = MeasurementSampleData<uint8, uint64>::GetDataSize(imageModePlanes);

	// Reserve how much memory we will need through non-Photoshop memory APIs
	// Basically the size of our data storage for each feature plus one for
	// the summary
	inRecord->requestSpaceReserve = (inRecord->maskFeatureCount + 1) *
			sampleDataSize;
	}

/******************************************************************************/

/**
 * Handle the record measurements selector.  Simply passes control to a 
 * templated function, RecordMeasurements, to allow writing the code once.  The
 * templated function (and further invoked templated functions) accept a type 
 * for the pixel value and a type for the summation of pixel values.
 *
 * \param inRecord the record for the selector
 */
void MeasurementSamplePlugin::HandleSelectorRecordMeasurements (MeasurementRecordRecord *inRecord)
	{
	REQUIRE_NON_NULL_PARAMETER (inRecord);
	
	int16		imageDepth;
	
	// Invoke correct instantiation of template function based upon image depth
	imageDepth = AsPrepare (inRecord)->imageDepth;
	if (imageDepth == 32)
		RecordMeasurements<float, double> (inRecord);
	else if (imageDepth == 16)
		RecordMeasurements<uint16, uint64> (inRecord);
	else
		RecordMeasurements<uint8, uint64> (inRecord);
	}

/******************************************************************************/

/**
 * Actually walk through the feature mask, image, and gray data and calculate
 * and store relevant data for each feature.  In short: determine which data
 * points we are trying to collect; determine what data we need (mask, image, 
 * and/or gray) to collect those data points; break the data up into tiles by
 * column then rows, from upper left to lower right; measure each tile and 
 * perform calculations; and write collected data out to descriptors stored in 
 * the record.
 *
 * \param inRecord the record to use to make calculations
 */
template<class T, class U> void RecordMeasurements (MeasurementRecordRecord *inRecord)
	{
	REQUIRE_NON_NULL_PARAMETER (inRecord);

	MeasurementBaseRecord			*baseRecord;
	MeasurementPrepareRecord		*prepareRecord;
	MeasurementRecordRecord			*recordRecord;
	TestAbortProc					FN_TestAbort;
	ProgressProc					FN_Progress;
	AdvanceStateProc				FN_AdvanceState;
	
	// Get the various records
	baseRecord = AsBase (inRecord);
	prepareRecord = AsPrepare (inRecord);
	recordRecord = inRecord;
	
	// Get the various procs
	FN_TestAbort = baseRecord->testAbortProc;
	FN_Progress = baseRecord->progressProc;
	FN_AdvanceState = recordRecord->advanceStateProc;
	
	MeasurementSampleDescriptorMap	descriptorMap;
	DescriptorTypeID				descriptorKeys[kMSDK_Last];
	
	// Initialize descriptor map, used to map identifier strings to an index in
	// the descriptor keys, makes for quicker and easier checks to see if
	// a particular data point is being measured
	descriptorMap[kMSP_DataPoint_EdgeTouching_Identifier] = kMSDK_EdgeTouching;
	descriptorMap[kMSP_DataPoint_Foreground_Identifier] = kMSDK_Foreground;
	descriptorMap[kMSP_DataPoint_CenterX_Identifier] = kMSDK_CenterX;
	descriptorMap[kMSP_DataPoint_CenterY_Identifier] = kMSDK_CenterY;
	descriptorMap[kMSP_DataPoint_ImagePixelsNonZero_Identifier] = kMSDK_ImagePixelsNonZero;
	descriptorMap[kMSP_DataPoint_GrayPixelsNonZero_Identifier] = kMSDK_GrayPixelsNonZero;

	// Initialize descriptor keys, indicates whether a data point is being
	// recorded if associated descriptor key is not typeNull
	for (uint32 index = 0; index < kMSDK_Last; index++)
		descriptorKeys[index] = typeNull;
	
	// Wrap list, but do not own it (we don't want to delete it)
	PIUActionList					dataPointIdentifiers (prepareRecord->dataPointIdentifiers, false);
	uint32							identifierCount;
			
	// Walk the data point identifier list and see what we should record
	identifierCount = dataPointIdentifiers.GetCount ();
	for (uint32 index = 0; index < identifierCount; index++)
		{
		std::string									identifier;
		MeasurementSampleDescriptorMap::iterator	iter;
		
		// Grab the next data point identifier we should record
		identifier = dataPointIdentifiers.GetString (index);
		
		// Find it in the map, if we don't find it ignore (though, this 
		// shouldn't happen anyhow)
		iter = descriptorMap.find (identifier);
		if (descriptorMap.end () == iter)
			continue;
			
		// If we found it, then override the typeNull key in the descriptor keys
		// array with the actual key for the identifier, this indicates that
		// we do want to record this data point
		descriptorKeys[iter->second] = PIUActionControl::GetStringID (identifier);
		}

	// Are we requesting image or gray data (we don't for bitmap mode)
	if (prepareRecord->imageMode != plugInModeBitmap)
		{
		// Image is required if we are requesting any image calculations
		recordRecord->requestImage =
				(descriptorKeys[kMSDK_ImagePixelsNonZero] != typeNull);
				
		// Gray is required if we are requesting any gray calculations
		recordRecord->requestGray = 
				(descriptorKeys[kMSDK_GrayPixelsNonZero] != typeNull);
		}
		
	// Are we requesting the mask?  We need the mask if we are requesting the
	// image or gray (so we know which pixels are in which features) or
	// any of the mask based calculations
	recordRecord->requestMask = 
				(recordRecord->requestImage) || 
				(recordRecord->requestGray) ||
				(descriptorKeys[kMSDK_EdgeTouching] != typeNull) ||
				(descriptorKeys[kMSDK_Foreground] != typeNull) ||
				(descriptorKeys[kMSDK_CenterX] != typeNull) ||
				(descriptorKeys[kMSDK_CenterY] != typeNull);
	
	// Our data objects for the features we collect information with
	// Plus one to act as a summary
	MeasurementSampleData<T, U>					summaryData (prepareRecord->imageModePlanes);
	std::vector< MeasurementSampleData<T, U> >	featureData;

	// Initialize the feature data vector
	featureData.resize (prepareRecord->maskFeatureCount, 
			MeasurementSampleData<T, U> (prepareRecord->imageModePlanes));
	
	// Only process if we are requesting something, setup and then walk
	// through the mask, image, and gray data in a tile based fashion while
	// measuring each tile
	if (recordRecord->requestMask)
		{
		VRect		maskRect;
		int16		tileHeight;
		int16		tileWidth;
		int32		tileOriginV;
		int32		tileOriginH;
		uint8		progressShift;
		uint64		progressTotal;
		uint64		progressCompleted;
		VRect		workRect;
		
		// If we are requesting the image, then setup image request planes to
		// request all normal image planes (ignore alpha and spot planes as
		// we don't use this for this plugin)
		if (recordRecord->requestImage)
			{
			recordRecord->requestImagePlaneLo = 0;
			recordRecord->requestImagePlaneHi = prepareRecord->imageModePlanes - 1;
			}
		
		// We do our work based upon the mask rect
		maskRect = prepareRecord->maskRect;
		
		// Grab and validate the tile height and vertical origin, invalid
		// tile height shouldn't happen, but deal with it anyhow
		tileHeight = prepareRecord->maskTileHeight;
		if (tileHeight > 0)
			tileOriginV = prepareRecord->maskTileOrigin.v;
		else
			{
			tileHeight = kTileHeight_Default;
			tileOriginV = 0;
			}
			
		// Grab and validate the tile width and horizontal origin, invalid
		// tile width shouldn't happen, but deal with it anyhow
		tileWidth = prepareRecord->maskTileWidth;
		if (tileWidth > 0)
			tileOriginH = prepareRecord->maskTileOrigin.h;
		else
			{
			tileWidth = kTileWidth_Default;
			tileOriginH = 0;
			}

		// Setup for progress, pixels can be greater than LONG_MAX, but
		// progress callback is limited, so shift until it fits (and remember
		// the shift for later)
		progressShift = 0;
		progressTotal = ((uint64)maskRect.bottom - (uint64)maskRect.top) * ((uint64)maskRect.right - (uint64)maskRect.left);
		while (progressTotal > LONG_MAX)
			{
			progressShift++;
			progressTotal >>= 1;
			}
		progressCompleted = 0;
		
		// Walk through the entire mask rows, one chunk at a time
		for (workRect.top = maskRect.top; workRect.top < maskRect.bottom; 
				workRect.top = workRect.bottom)
			{
			// Calculate the bottom, do not go past the real bottom
			workRect.bottom = workRect.top + tileHeight;
			if (workRect.bottom > maskRect.bottom)
				workRect.bottom = maskRect.bottom;
			else if (workRect.top == maskRect.top)
				if (workRect.bottom < maskRect.bottom)
					workRect.bottom -= workRect.top - tileOriginV;

			// Walk through the entire mask columns, one chunk at a time
			for (workRect.left = maskRect.left; workRect.left < maskRect.right; 
					workRect.left = workRect.right)
				{
				// Calculate the bottom, do not go past the real right
				workRect.right = workRect.left + tileWidth;
				if (workRect.right > maskRect.right)
					workRect.right = maskRect.right;
				else if (workRect.left == maskRect.left)
					if (workRect.right < maskRect.right)
						workRect.right -= workRect.left - tileOriginH;
					
				// Setup the request rectangles
				recordRecord->requestImageRect = workRect;
				recordRecord->requestGrayRect = workRect;
				recordRecord->requestMaskRect = workRect;

				// Should we abort?
				if (FN_TestAbort ())
					ThrowOSErr (userCanceledErr);
					
				// Advance state to allow the host to service the requests
				// The host takes our requests and stores the data in the 
				// various response fields of the record
				ThrowIfOSErr (FN_AdvanceState ());
					
				// Should we abort?
				if (FN_TestAbort ())
					ThrowOSErr (userCanceledErr);
					
				// Now that we've gotten all relevant information regarding the
				// tile, measure that tile and perform all calculations
				// A templated function to handle different image pixel depths
				MeasureTile<T, U> (inRecord, summaryData, featureData);
				
				// Progress update, account for shift calculated above
				progressCompleted += (workRect.bottom - workRect.top) * (workRect.right - workRect.left);
				FN_Progress ((int32)(progressCompleted >> progressShift), (int32)progressTotal);
				}
			}
			
		// Add the feature data to the summary data
		for (uint32 index = 0; index < featureData.size (); index++)
			summaryData.Add (featureData[index]);
		}
		
	// Write the results
	summaryData.Write (descriptorKeys, recordRecord->summaryData);
	for (uint32 index = 0; index < featureData.size (); index++)
		featureData[index].Write (descriptorKeys, recordRecord->featureData[index]);
	}
	
/******************************************************************************/

/**
 * Templated function to examine each tile and measure each feature in the tile.
 * In short; walk each pixel in the tile by columns, then rows from upper left
 * to lower right; check to see if the pixel is properly identified; and record 
 * all relevant data for the feature associated with the pixel.
 *
 * \param inRecord the record containing relevant tile data
 * \param ioSummaryData the summary data object to store data in
 * \param ioFeatureData the vector of feature data objects to store data in
 */
template<class T, class U> void MeasureTile (
		const MeasurementRecordRecord *inRecord, 
		MeasurementSampleData<T, U> &ioSummaryData, 
		std::vector< MeasurementSampleData<T, U> > &ioFeatureData)
	{
	REQUIRE_NON_NULL_PARAMETER (inRecord);

	const MeasurementRecordRecord	*recordRecord;
	VRect							tileRect;
	int32							tileWidth;
	T								*imageData;
	int32							imageColumnBytes;
	int32							imageRowBytesDelta;
	T								*grayData;
	int32							grayColumnBytes;
	int32							grayRowBytesDelta;
	uint16							*maskData;
	int32							maskColumnBytes;
	int32							maskRowBytes;
	int32							maskRowBytesDelta;
	size_t							featureCount;
	
	// Get the various records
	recordRecord = inRecord;

	// Grab the tile rect
	tileRect = recordRecord->requestImageRect;
	tileWidth = tileRect.right - tileRect.left;
	
	// Get the image data
	imageData = (T*)recordRecord->responseImageData;
	
	// Calculate the image related bytes
	imageColumnBytes = recordRecord->responseImageColumnBytes;
	imageRowBytesDelta = recordRecord->responseImageRowBytes - tileWidth * imageColumnBytes; 
		
	// Get the gray data
	grayData = (T*)recordRecord->responseGrayData;
	
	// Calculate the gray related bytes
	grayColumnBytes = recordRecord->responseGrayColumnBytes;
	grayRowBytesDelta = recordRecord->responseGrayRowBytes - tileWidth * grayColumnBytes; 

	// Get the mask data
	maskData = (uint16*)recordRecord->responseMaskData;
	
	// Calculate the mask related bytes
	maskColumnBytes = recordRecord->responseMaskColumnBytes;
	maskRowBytes = recordRecord->responseMaskRowBytes;
	maskRowBytesDelta = maskRowBytes - tileWidth * maskColumnBytes; 
			
	// Feature count
	featureCount = ioFeatureData.size ();
	
	// Walk the rows of the tile
	for (int32 row = tileRect.top; row < tileRect.bottom; row++)
		{
		// Walk the columns of the tile
		for (int32 column = tileRect.left; column < tileRect.right; column++)
			{
			uint16		feature;
			uint16		identifier;
			
			// Determine feature from the mask pixel
			feature = *maskData;
				
			// Grab the identifier, if a valid identifer the use it,
			// It is possible to get a non-valid identifier (0x7FF) if the
			// image has more features than can be counted, for this plugin
			// we ignore unidentified features
			identifier = feature & featureMaskIdentifierMask;
			if (identifier < featureCount)
				ioFeatureData[identifier].Record (row, column, 
						feature, imageData, grayData);

			// Next column (in bytes, which is why there are the pointer casts)
			imageData = (T*)(((uint8*)imageData) + imageColumnBytes);
			grayData = (T*)(((uint8*)grayData) + grayColumnBytes);
			maskData = (uint16*)(((uint8*)maskData) + maskColumnBytes);
			}
		
		// Next row (in bytes, which is why there are the pointer casts)
		imageData = (T*)(((uint8*)imageData) + imageRowBytesDelta);
		grayData = (T*)(((uint8*)grayData) + grayRowBytesDelta);
		maskData = (uint16*)(((uint8*)maskData) + maskRowBytesDelta);
		}
	}

/******************************************************************************/

/**
 * Handle the export measurement selector.  The only data point data type this
 * plugin currently exports is the plane pixels non-zero data type.  In order
 * to show usage of the data directory, this data type is written to a separate 
 * file for each measurement.  The file is a text file contained comma-delimited 
 * data for each plane of the histogram.  
 *
 * The actual file I/O is performed in a separate OS-specific function.
 *
 * \param inRecord the record for the selector
 */
void MeasurementSamplePlugin::HandleSelectorExportMeasurement (MeasurementExportRecord *inRecord)
	{
	REQUIRE_NON_NULL_PARAMETER (inRecord);
	REQUIRE_NON_NULL_PARAMETER (inRecord->dataPointDataType);
	REQUIRE_NON_NULL_PARAMETER (inRecord->dataPointData);
	
	// Wrap the data point data type, but do not own (don't delete it)
	PIUActionDescriptor		dataPointDataType (inRecord->dataPointDataType, false);
	std::string				identifier;
	
	// Which data point data type is it for, we only handle one right now
	identifier = dataPointDataType.GetString (kIDStr);
	if (identifier == kMSP_DataPointDataType_PlanePixelsNonZero_Identifier)
		{
		// Wrap the data point data, but do not own (don't delete it)
		PIUActionDescriptor			dataPointData (inRecord->dataPointData, false);
		PIUActionList				channelList;
		uint32						channelCount;
		PIUASZString				filename;

		// Get the list of channels from the data
		channelList = dataPointData.GetList (kchannelsStr);
		
		// Walk the channels of the list
		channelCount = channelList.GetCount ();
		if (channelCount > 0)
			{
			std::string		output;
			
			for (uint32 channelIndex = 0; channelIndex < channelCount; channelIndex++)
				{
				double		doubleValue;
				uint64		integerValue;
				char		valueString[31];
				
				// Get the value for the channel
				doubleValue = channelList.GetFloat (channelIndex);
				integerValue = (uint64)doubleValue;
				
				// Translate value to a string
				snprintf (valueString, 31, "%ld", (long)integerValue);

				// Add a comma delimiter
				if (channelIndex > 0)
					output += ", ";

				// Add string to the output
				output += valueString;
				}
			
			// Write the file and get the filename used for output purposes
            if (inRecord->hostSupportsXPlatSpec != 0)
                WriteFileX (output, inRecord->dataDirectoryX, filename);
            else
                ThrowLogicErrorDetailed ("hostSupportsXPlatSpec needed to export data.");

			// Set the string written in the main export file as the filename
			// used by the data file in the data directory
			inRecord->exportString = filename.Release ();
			
			// We exported a file, this tells the main application that the
			// data directory is needed as should not be deleted at completion
			inRecord->exportData = true;
			}
		}

	// Else, what?
	else
		ThrowLogicErrorDetailed ("Unknown data point data type to export.");
	}

/******************************************************************************/

/**
 * Write the plane pixels non zero data string to a file.  The file is stored in the
 * specified data directory and has a unique filename.
 *
 * \param inOutput the plane pixels non zero data string
 * \param inDataDirectoryX the directory to store the file in
 * \param outFilename the final filename used by the file
 */
#if __PIMac__

void MeasurementSamplePlugin::WriteFileX (const std::string &inOutput,
                                          const XPlatFileSpec *inDataDirectoryX,
                                          PIUASZString &outFilename)
{
    REQUIRE_NON_NULL_PARAMETER (inDataDirectoryX);
    
    int            fileRef;
    OSErr          err = noErr;
    char           fullFilePath [4096];
    
    if (inDataDirectoryX->mFileReference == NULL)
        ThrowOSErr(dirNFErr);
    
    CFStringRef parentPathRef = CFURLCopyFileSystemPath(inDataDirectoryX->mFileReference, kCFURLPOSIXPathStyle);
    if (parentPathRef == NULL)
        ThrowOSErr(cNoMemErr);
    
    // Loop until we find a unique filename
    for (int32 fileCount = 1; ; fileCount++)
    {
        // Generate a filename
        outFilename = "PlanePixelsNonZero-^0.csv";
        outFilename.Replace (0, PIUASZString::RomanizationOf (fileCount));
        
        //Generate full path
        CFStringRef outFilenameRef = CFStringCreateWithCharacters(kCFAllocatorDefault,
                                                                  outFilename.GetUnicodeString (),
                                                                  outFilename.GetUnicodeStringLength ());
        
        if (outFilenameRef)
        {
            CFMutableStringRef fullPathStringRef = CFStringCreateMutableCopy(kCFAllocatorDefault, 0, parentPathRef);
            
            if (fullPathStringRef != NULL)
            {
                CFStringAppend(fullPathStringRef, CFSTR ("/"));
                CFStringAppend(fullPathStringRef, outFilenameRef);
                CFStringGetCString(fullPathStringRef, fullFilePath, 4096, kCFStringEncodingUTF8);
                CFRelease(outFilenameRef);
                CFRelease(fullPathStringRef);
            }
            else
            {
                CFRelease(outFilenameRef);
                err = cNoMemErr;
                break;
            }
            
        }
        else
        {
            err = cNoMemErr;
            break;
        }
        
        // Attempt to create a new file, if not then deal with it
        fileRef = ::open((char*)fullFilePath, O_CREAT | O_EXCL | O_WRONLY, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
        
        if (fileRef == -1 && EEXIST == errno)
            continue;
        else if (fileRef != -1)
            break;
        
        err = openErr;
        break;
    }
    
    CFRelease(parentPathRef);
    if (err != noErr )
        ThrowOSErr(err);
    
    try
    {
        // Write the output
        ssize_t aCount = ::write(fileRef, inOutput.c_str (), inOutput.size ());
        ThrowIfOSErr (aCount == -1 ? writErr : noErr);
        
        // Close
        int closeRes = ::close (fileRef);
        ThrowIfOSErr (closeRes == -1 ? closErr : noErr);
    }
    catch (...)
    {
        if (fileRef != -1)
        {
            ::close(fileRef);
            fileRef = 0;
        }
        ::remove(fullFilePath);
        throw;
    }
}

#elif __PIWin__

void MeasurementSamplePlugin::WriteFileX (const std::string &inOutput,
                                          const XPlatFileSpec *inDataDirectoryX,
                                          PIUASZString &outFilename)
{
    HANDLE            file;
    
    // Loop until we find a unique filename
    for (int32 fileCount = 1; ; fileCount++)
    {
        PIUASZString        path;
        
        // Generate a filename
        outFilename = "PlanePixelsNonZero-^0.csv";
        outFilename.Replace (0, PIUASZString::RomanizationOf (fileCount));
        
        // Generate a path
        path = "^0^1";
        path.Replace (0, PIUASZString (inDataDirectoryX->mFileReference));
        path.Replace (1, outFilename);
        
        // Attempt to create a new file, if not then deal with it
        file = CreateFileW ((LPCWSTR)path.GetUnicodeString (), GENERIC_WRITE, 0,
                            NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);
        if (file == INVALID_HANDLE_VALUE)
        {
            DWORD    lastError;
            
            // Determine the error
            lastError = GetLastError ();
            if (lastError == ERROR_FILE_EXISTS)
                continue;
            else if (lastError == ERROR_SUCCESS)
                break;
            
            ThrowOSErr (writErr);
        }
        
        break;
    }
    
    try
    {
        DWORD    bytesWritten;
        
        // Write the output
        if (!::WriteFile (file, inOutput.c_str (), (DWORD)inOutput.size (), &bytesWritten, NULL))
            ThrowOSErr (writErr);
        
        // Close
        CloseHandle (file);
    }
    catch (...)
    {
        if (file != NULL)
            CloseHandle (file);
        throw;
    }
}

#endif

////////////////////////////////////////////////////////////////////////////////

#if defined(__APPLE_CC__)
#define	RETURN_TYPE		int
#define RETURN_RESULT	0
#else
#define	RETURN_TYPE		void
#define RETURN_RESULT
#endif

////////////////////////////////////////////////////////////////////////////////

/**
 * Standard plugin entry point.  Create an instance of the measurement sample 
 * plugin object, handle the specified selector, cleanup, and properly handle 
 * any exceptions thrown.
 *
 * \param inSelector selector to handle
 * \param inRecord record associated with selection
 * \param ioInstance not used
 * \param outResult any error that occurs in the plugin
 */
DLLExport MACPASCAL RETURN_TYPE PluginMain (const int16 inSelector,
		void *inRecord, intptr_t *ioInstance, int16 *outResult)
	{
	int16		result = noErr;
	
	try 
		{
		MeasurementSamplePlugin		plugin;
			
		// Handle the selector
		plugin.HandleSelector (inSelector, inRecord);
		}
	catch (PIUException &inException)
		{
		result = inException.GetError ();
		}
	catch (OSType &inSPErr)		// For PIUSuites.h
		{
		result = PIUSPException::TranslateSPErr (inSPErr);
		}
	catch (...)
		{
		result = paramErr;
		}

	// Return erro
	if (outResult != NULL)
		*outResult = result;

	return RETURN_RESULT;
	}
