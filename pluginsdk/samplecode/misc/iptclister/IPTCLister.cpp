// ADOBE SYSTEMS INCORPORATED
// Copyright  1993 - 2002 Adobe Systems Incorporated
// All Rights Reserved
//
// NOTICE:  Adobe permits you to use, modify, and distribute this 
// file in accordance with the terms of the Adobe license agreement
// accompanying it.  If you have received this file from a source
// other than Adobe, then your use, modification, or distribution
// of it requires the prior written permission of Adobe.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

/* -------------------------------------------------------------- */

enum 
	{
	tTIFF_IPTC_NAA		= 33723,
	
	tTIFF_AdobeData		= 34377,
	
	tJPEG_SOI			= 0xFFD8,
	tJPEG_EOI			= 0xFFD9,
	tJPEG_APP0			= 0xFFE0,
	tJPEG_APPDMarker	= 0xFFED,
	tJPEG_SOS			= 0xFFDA,
	
	tPSD_IPTC			= 1028,
	tPSD_Copyright		= 1034,
	tPSD_URL			= 1035
	};

enum
	{
	ttByte = 1,
	ttAscii,
	ttShort,
	ttLong,
	ttRational,
	ttSByte,
	ttUndefined,
	ttSShort,
	ttSLong,
	ttSRational,
	ttFloat,
	ttDouble,
	ttIFD,
	ttUnicode,
	ttComplex
	};
	
enum
	{
	// leave zero as an invalid value
	kTIFF = 1,
	kJPEG,
	kPSD
	};

/* -------------------------------------------------------------- */

bool ScanTIFF(FILE *input);
bool ScanJPEG(FILE *input);
bool ScanPSD(FILE *input);
bool ScanPhotoshopResources(FILE *input, long length);
void LoadIPTCData(FILE *input, long length, bool includeAll=false);

/* -------------------------------------------------------------- */
int main(int argc, char * argv[])
{

	printf("\nVersion: 1.1\n");

	if (argc <= 1)
		{
		printf("Usage: IPTCLister filename1.psd filename2.ext ...\n");
		printf("\tOnly tif, jpg and psd are supported currently.\n");
		return 1;
		}
	
	FILE *inputFile = NULL;
	long filetype = -1;
	

	for (int i = 0; i < argc - 1; i++)
 		{
 		
		char *inputFileName = argv[i+1];
		
		try
			{

			printf("Scanning ""%s"" for IPTC tags\n", inputFileName );

			char * extension = inputFileName + strlen(inputFileName) - 3;

			if (extension < inputFileName)
				extension = inputFileName;

			if ( !strncmp(extension,"tif",3)
				|| !strncmp(extension,"TIF",3) )
				filetype = kTIFF;
			else  if ( !strcmp(extension,"jpg")
				|| !strcmp(extension,"JPG")
				|| !strcmp(extension,"jpeg")
				|| !strcmp(extension,"JPEG") )
				filetype = kJPEG;
			else  if ( !strncmp(extension,"psd",3)
				|| !strncmp(extension,"PSD",3) )
				filetype = kPSD;
			else
				filetype = -2;
			
		
			// open file
			inputFile = fopen( inputFileName, "rb" );	// read as binary
			if (inputFile == NULL)
				{
				printf("Could not open file %s\n", inputFileName );
				exit(-1);
				}

			bool foundIPTC = false;

			// find the tags
			switch( filetype )
				{
				case kTIFF:
					foundIPTC = ScanTIFF( inputFile );
					break;
				case kJPEG:
					foundIPTC = ScanJPEG( inputFile );
					break;
				case kPSD:
					foundIPTC = ScanPSD( inputFile );
					break;
				default:
					printf("Unrecognized file type\n");
					foundIPTC = false;
					break;
				}
			
			if (!foundIPTC)
				printf("No IPTC tag found in ""%s""\n\n", inputFileName );
			
			// cleanup
			if (inputFile != NULL)	
				fclose( inputFile );
			
			}
		catch(...)
			{
			printf("Unexpected, unknown error caught while scanning %s!", inputFileName );
			}

		printf("-------------------------------------------\n");

		}

	return 0;
}  /* main */

/* -------------------------------------------------------------- */

static unsigned short GetShort( FILE *input, bool byteSwap )
{
	unsigned short result;
	unsigned char b0, b1;
	
	b0 = (unsigned char)getc(input);
	b1 = (unsigned char)getc(input);

	if (byteSwap)
		{
		result  = (unsigned short)b1 << 8;
		result |= (unsigned short)b0;
		}
	else
		{
		result  = (unsigned short)b0 << 8;
		result |= (unsigned short)b1;
		}
	
	return result;
}

/* -------------------------------------------------------------- */

static unsigned long GetLong( FILE *input, bool byteSwap )
{
	unsigned long result;
	unsigned char b0, b1, b2, b3;
	
	b0 = (unsigned char)getc(input);
	b1 = (unsigned char)getc(input);
	b2 = (unsigned char)getc(input);
	b3 = (unsigned char)getc(input);

	if (byteSwap)
		{
		result  = (unsigned long)b3 << 24;
		result |= (unsigned long)b2 << 16;
		result |= (unsigned long)b1 << 8;
		result |= (unsigned long)b0;
		}
	else
		{
		result  = (unsigned long)b0 << 24;
		result |= (unsigned long)b1 << 16;
		result |= (unsigned long)b2 << 8;
		result |= (unsigned long)b3;
		}
	
	return result;
}

/* -------------------------------------------------------------- */

static int TagTypeSize (short type)
{
	switch (type)
		{

		case ttShort:
		case ttSShort:
			return 2;
			
		case ttLong:
		case ttSLong:
		case ttFloat:
		case ttIFD:
			return 4;
			
		case ttRational:
		case ttSRational:
		case ttDouble:
			return 8;
			
		default:
			return 1;
			
		}
}

/* -------------------------------------------------------------- */

// first search for pure IPTC data, then try Photoshop image resources
bool ScanTIFF(FILE *input)
{
	bool LSBFirst = false;
	bool foundIPTC = false;
	unsigned char tempChar;
	unsigned short tempShort;
	unsigned long tempLong;


	// first, verify that it is a TIFF file
	tempShort = GetShort(input,false);
	
	switch (tempShort)
		{
		
		case 0x4949:
			LSBFirst = true;
			break;
			
		case 0x4D4D:
			LSBFirst = false;
			break;
			
		default:
			printf("This is not a valid TIFF file\n");
			return false;
		}
	
	// determine machine endian-ness
	tempLong = 0x11223344;
	
	tempChar = *((unsigned char *)(&tempLong) + 3);
	
	bool isLittleEndian = (tempChar == 0x11);
	
	// decide whether to byteswap based on platform and LSBFirst flag
	bool needsByteSwap = LSBFirst;
	
	
	// double check the validity...
	tempShort = GetShort(input,needsByteSwap);
	
	if (tempShort != 42)
		{
		printf("This is not a valid TIFF file\n");
		return false;
		}
		
	
	// get the tag directory offset
	long directory = GetLong(input,needsByteSwap);
	
	while (directory != 0)
		{
		
		// go to position of tag directory
		(void)fseek( input, directory, SEEK_SET );
		
		// see how many tags there are in this directory
		long entries = GetShort(input,needsByteSwap);

		// set a limit on the number of entries
		if (entries > 10000)
			entries = 10000;
		
		// now cycle through the TIFF tags, looking for IPTC tag or Photoshop image resources
		for ( long i = 0; i < entries; i++ )
			{
			
			// reseek incase we read something from another location
			// most of the time this will do nothing
			(void)fseek(input, directory + 2 + (12 * i), SEEK_SET );
			
			unsigned short tagCode = GetShort(input,needsByteSwap);
			unsigned short tagType = GetShort(input,needsByteSwap);
			unsigned long tagCount = GetLong(input,needsByteSwap);
			unsigned long tagValue = GetLong(input,needsByteSwap);
			
			switch ( tagCode )
				{
				case tTIFF_IPTC_NAA:
					(void)fseek( input, tagValue, SEEK_SET );
					tempLong =  TagTypeSize(tagType) * tagCount ;
					printf("IPTC length is %ld bytes\n", tempLong );
					LoadIPTCData(input,tempLong,false);
					foundIPTC = true;
					break;
				
				case tTIFF_AdobeData:
					(void)fseek( input, tagValue, SEEK_SET );
					tempLong =  TagTypeSize(tagType) * tagCount ;
					printf("PSD resource length is %ld bytes\n", tempLong );
					foundIPTC = ScanPhotoshopResources(input, tempLong);
					break;
				
				default:
					// skip over it
					break;
				}
			
			}	/* for ( long i = 0; i < entries; i++ ) */
		
		// reset location, in case we were reading from somewhere else
		(void)fseek(input, directory + 2 + (12 * entries), SEEK_SET );
		
		// see if there is another tag directory
		directory = GetLong(input,needsByteSwap);
		
		}	/* while (directory != 0) */

	return foundIPTC;
}

/* -------------------------------------------------------------- */

// first search for pure IPTC data, then try Photoshop image resources
bool ScanJPEG(FILE *input)
{
	unsigned char tempChar;
	unsigned short tempShort;
	unsigned long tempLong;
	bool foundIPTC = false;
	
	
	// determine machine endian-ness
	// JPEG is always BigEndian
	tempLong = 0x11223344;
	tempChar = *((unsigned char *)(&tempLong) + 3);
	bool needsByteSwap = false;
	

	// CodeWarrior 5.x feof() function is broken
	while ( !feof(input) )
		{
		unsigned short marker = GetShort(input,needsByteSwap);
		
		
		switch (marker)
			{
			case tJPEG_SOI:
				break;
			
			case tJPEG_EOI:
				// end of the image, we're done
				return foundIPTC;
			
			case tJPEG_APPDMarker:
				// marker length includes length tag and string!
				tempShort = GetShort(input,needsByteSwap);
				
				// null terminated identifier string - just ignore it
				while ( (tempChar = (unsigned char)getc(input)) != NULL)
					{
					tempShort--;
					}
				
				tempShort--;	// for the NULL
				
				printf("PS resource length is %ld bytes\n", (long)tempShort - 2 );
				foundIPTC = ScanPhotoshopResources( input, tempShort - 2 );
				break;
			
			case tJPEG_SOS:
				// beginning of image data, get out now
				return foundIPTC;
			
			default:
				// marker length includes length tag
				tempShort = GetShort(input,needsByteSwap);
				if (tempShort < 2)
					{
					printf("Bad JPEG marker length!\n");
					return foundIPTC;
					}
				(void)fseek( input, tempShort - 2, SEEK_CUR );
				break;
			}
		
		}

	return foundIPTC;
}

/* -------------------------------------------------------------- */

bool ScanPSD(FILE *input)
{
	bool foundIPTC = false;
	unsigned char tempChar;
	unsigned short tempShort;
	unsigned long tempLong;
	
	
	// determine machine endian-ness
	// PSD is always BigEndian
	tempLong = 0x11223344;
	tempChar = *((unsigned char *)(&tempLong) + 3);
	bool needsByteSwap = false;
	
	
	
	// file header
	tempLong = GetLong(input,needsByteSwap);
	if (tempLong != '8BPS')
		{
		printf("This is not a valid PSD file\n");
		return false;
		}
	
	tempShort = GetShort(input,needsByteSwap);
	if (tempShort != 1)
		{
		printf("Invalid PSD version of %ld\n", tempShort);
		return false;
		}

	// 6 bytes reserved, must be zero
	tempLong = GetLong(input,needsByteSwap);
	tempShort = GetShort(input,needsByteSwap);
	if (tempLong != 0 || tempShort != 0)
		{
		printf("Reserved PSD header fields are not zero\n");
		return false;
		}
	
	unsigned short channels = GetShort(input,needsByteSwap);
	unsigned long rows = GetLong(input,needsByteSwap);
	unsigned long columns = GetLong(input,needsByteSwap);
	unsigned short bitDepth = GetShort(input,needsByteSwap);
	unsigned short colorMode = GetShort(input,needsByteSwap);

	printf("PSD image is %ld pixels wide by %ld pixels high\n", columns, rows );
	printf("PSD image is %ld bits per channel, with %ld channels\n", bitDepth, channels );
	printf("PSD image color mode is ");
	
	switch (colorMode)
		{
		case 0:
			printf("Bitmap\n");
			break;
		case 1:
			printf("Indexed\n");
			break;
		case 2:
			printf("Grayscale\n");
			break;
		case 3:
			printf("RGB\n");
			break;
		case 4:
			printf("CMYK\n");
			break;
		case 7:
			printf("Multichannel\n");
			break;
		case 8:
			printf("Duotone\n");
			break;
		case 9:
			printf("LAB\n");
			break;
		default:
			printf("unknown mode %ld\n", colorMode );
			break;
		}
	
	
	
	
	// color mode data - only useful for indexed color images
	tempLong = GetLong(input,needsByteSwap);
	
	if (tempLong != 0)
		(void)fseek(input,tempLong,SEEK_CUR);
	
	
	
	// image resources
	tempLong = GetLong(input,needsByteSwap);
	
	printf("PS resource length is %ld bytes\n", tempLong );
	foundIPTC = ScanPhotoshopResources(input, tempLong);
	
	
	
	
	// layer info and mask data
	tempLong = GetLong(input,needsByteSwap);	// length of combined layer and mask data
	
	if (tempLong != 0)
		(void)fseek(input,tempLong,SEEK_CUR);
		
	
	// image data
	
	// compression type -- do NOT treat this as a boolean!!
	tempShort = GetShort(input,needsByteSwap);
	
	switch (tempShort)
		{
		case 0:
			printf("PSD image is not compressed\n");
			break;
		case 1:
			printf("PSD image is RLE compressed\n");
			break;
		default:
			printf("unknown PSD compression type %d\n", tempShort);
			break;
		}
	
	// ignore the image data.....
	
	
	return foundIPTC;
}

/* -------------------------------------------------------------- */

static char * GetString(FILE *input, unsigned short length)
{
	if (length == 0)
		return NULL;
	
	if (length > 1024)
		printf("excessive string length of %d!\n", length);

	char *result = (char *)malloc( length+1 );

	size_t readLength = fread( result, 1, length, input );

	if (readLength != length)
		{
		printf("did not read %zu bytes of %d bytes GetString\n", readLength, length);
		free(result);
		return NULL;
		}

	result[length] = '\0';
	
	return result;
}

/* -------------------------------------------------------------- */

// we have Photoshop image resources located
// now find the IPTC/File Info part
bool ScanPhotoshopResources(FILE *input, long length)
{
	bool foundIPTC = false;
	unsigned char tempChar;
	
	// capture the current position
	long start = ftell(input);
	
	long position = ftell(input);
	
	while ( (position-start) < length)
		{
		
		if ( (length - (position - start)) < (4+2+1+4)) {
			printf("Photoshop resource block is too short for another tag\n");
			// skip remaining resource block
			(void)fseek( input, start+length, SEEK_SET );
			break;
		}
		
		unsigned long tagCreator = GetLong(input,false);
		unsigned short tagType = GetShort(input,false);
		
		
		// Prior to Photoshop 6, names were always empty
		// BUT, they were fully spelled out in the SDK since Photoshop 3.0
		// some other programs fail here because they assumed the names would always be NULL
		
		unsigned char nameLength = (unsigned char)getc(input);
		char *tagName = GetString(input,nameLength);
		
		// pad name to even boundary, including length byte
		if (!(nameLength & 0x01))
			tempChar = (unsigned char)getc(input);
		
		
		unsigned long tagLength = GetLong(input,false);
		
		if ( (position + (long)tagLength - start) > length) {
			printf("Bad Photoshop resource block length\n");
			// skip remaining resource block
			(void)fseek( input, start+length, SEEK_SET );
			free( tagName );
			break;
		}
		
		
		switch( tagType )
			{
			
			case tPSD_IPTC:
				printf("IPTC length is %ld bytes\n", tagLength );
				LoadIPTCData(input, tagLength, false);
				foundIPTC = true;
				break;
			
			case tPSD_Copyright:
			case tPSD_URL:
			
			default:
				// skip over unknown tags
				(void)fseek( input, tagLength, SEEK_CUR );
				break;
			}
		
		if (tagLength & 0x01)
			tempChar = (unsigned char)getc(input);
		
		free( tagName );
		
		position = ftell(input);
		}
	
	return foundIPTC;
}

/* -------------------------------------------------------------- */

static unsigned short * GetUnicodeString(FILE *input)
{
	long length = GetLong(input,false);
	
	if (length > 32768)
		printf("excessive string length of %d!\n", length);

	if (length < 0)
		printf("negative string length of %d!\n", length);

	if (length == 0 || length > 32768 || length < 0)
		return NULL;

	unsigned short *result = (unsigned short *)malloc( length*2 );

	size_t readLength = fread( result, 2, length, input );

	if (readLength != length)
		{
		printf("did not read %zu bytes of %d bytes GetUnicodeString\n", readLength, length);
		free(result);
		return NULL;
		}

	return result;
}

/* -------------------------------------------------------------- */

static void DoIPTCString(const char *descString,FILE *input, unsigned short length)
{
	char *tempString = GetString(input,length);
	if (tempString != NULL)
		{
		printf("%s: %s\n", descString, tempString );
		free(tempString);
		}
}

/* -------------------------------------------------------------- */
		
enum DataSet
	{
	kRecordVersionSet					= 0,	// short
	kObjectNameSet						= 5,
	kUrgencySet							= 10,	// single character
	kCategorySet						= 15,
	kSupplementalCategoriesSet			= 20,	// List
	kKeywordsSet						= 25,	// List
	kSpecialInstructionsSet				= 40,
	kDateCreatedSet						= 55,
	kBylineSet							= 80,
	kBylineTitleSet						= 85,
	kCitySet							= 90,
	kProvinceStateSet					= 95,
	kCountryNameSet						= 101,
	kOriginalTransmissionReferenceSet	= 103,
	kHeadlineSet						= 105,
	kCreditSet							= 110,
	kSourceSet							= 115,
	kCopyrightNoticeSet					= 116,
	kCaptionSet							= 120,
	kCaptionWriterSet					= 122,
	
	kCaptionFileFlag					= 0xAA,	// Not a valid data set header, used only by PS
	kCaptionFileVersion					= 0x01
	};

// we've found the IPTC data, read it in
void LoadIPTCData(FILE *input, long length, bool includeAll)
{
	unsigned char tempChar;
	unsigned short tempShort;
	
	
	// capture the current position
	long start = ftell(input);
	
	// this is not valid IPTC data, this is extended stuff from Photoshop
	if (includeAll)
		{
		tempChar = (unsigned char)getc(input);
		if (tempChar != kCaptionFileFlag)
			{
			printf("bad IPTC caption flag\n");
			return;
			}
		
		tempChar = (unsigned char)getc(input);
		if (tempChar != kCaptionFileVersion)
			{
			printf("unknown IPTC caption version\n");
			return;
			}
		
		tempChar = (unsigned char)getc(input);		// copyright boolean
		
		printf("PS CopyrightFlag: %s\n", (tempChar?"true":"false") );
		
		unsigned short *URL = GetUnicodeString(input);	// URL, as unicode string
		
		if (URL != NULL)
			free(URL);
		
		}
		
	
	// now we do the normal IPTC data
	// loop until we've read all the data
	
	long position = ftell(input);
	
	while ( (position-start) < length)
		{
		unsigned short header = GetShort(input,false);
		unsigned char set = (unsigned char)getc(input);
		unsigned short headerLength = GetShort(input,false);
		
		switch( set )
			{
			case kRecordVersionSet:
				if (headerLength != 2)
					{
					printf("bad IPTC version length!\n");
					return;
					}
				tempShort = GetShort(input,false);
				if (tempShort != 2)
					{
					printf("bad IPTC version number %d %d !\n", tempShort, 2);
					return;
					}
				break;
			
			case kObjectNameSet:
				DoIPTCString("Object Name",input,headerLength);
				break;
			
			case kUrgencySet:
				tempChar = (unsigned char)getc(input);
				printf("Urgency: %c\n", tempChar );
				break;
			
			case kCategorySet:
				DoIPTCString("Category",input,headerLength);
				break;
			
			case kSpecialInstructionsSet:
				DoIPTCString("Special Instructions",input,headerLength);
				break;
			
			case kDateCreatedSet:
				DoIPTCString("Date Created",input,headerLength);
				break;
			
			case kBylineSet:
				DoIPTCString("Byline",input,headerLength);
				break;
			
			case kBylineTitleSet:
				DoIPTCString("Byline Title",input,headerLength);
				break;
			
			case kCitySet:
				DoIPTCString("City",input,headerLength);
				break;
			
			case kProvinceStateSet:
				DoIPTCString("State/Province",input,headerLength);
				break;
			
			case kCountryNameSet:
				DoIPTCString("Country",input,headerLength);
				break;
			
			case kOriginalTransmissionReferenceSet:
				DoIPTCString("Original Reference",input,headerLength);
				break;
			
			case kHeadlineSet:
				DoIPTCString("Headline",input,headerLength);
				break;
			
			case kCreditSet:
				DoIPTCString("Credit",input,headerLength);
				break;
			
			case kSourceSet:
				DoIPTCString("Source",input,headerLength);
				break;
			
			case kCopyrightNoticeSet:
				DoIPTCString("Copyright Notice",input,headerLength);
				break;
			
			case kCaptionSet:
				DoIPTCString("Caption",input,headerLength);
				break;
			
			case kCaptionWriterSet:
				DoIPTCString("Caption Writer",input,headerLength);
				break;
			
			case kCaptionFileFlag:
				printf("**** CaptionFileFlag found in IPTC struct!\n");
				tempChar = (unsigned char)getc(input);
				if (tempChar != kCaptionFileVersion)
					{
					printf("unknown IPTC caption version\n");
					return;
					}
				
				tempChar = (unsigned char)getc(input);		// copyright boolean
				
				printf("PS CopyrightFlag: %s\n", (tempChar?"true":"false") );

				{
				unsigned short *URL = GetUnicodeString(input);	// URL, as unicode string
				
				if (URL != NULL)
					free(URL);
				}
				break;
			
			default:
				// skip over unknown tags
				(void)fseek( input, headerLength, SEEK_CUR );
				break;
			}
	
		position = ftell(input);
		}
	
	printf("\n\n");
}

/* -------------------------------------------------------------- */
/* -------------------------------------------------------------- */
/* -------------------------------------------------------------- */
/* -------------------------------------------------------------- */
/* -------------------------------------------------------------- */