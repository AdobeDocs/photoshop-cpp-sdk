// ADOBE SYSTEMS INCORPORATED
// Copyright  2003 - 2005 Adobe Systems Incorporated
// All Rights Reserved
//
// NOTICE:  Adobe permits you to use, modify, and distribute this 
// file in accordance with the terms of the Adobe license agreement
// accompanying it.  If you have received this file from a source
// other than Adobe, then your use, modification, or distribution
// of it requires the prior written permission of Adobe.
//-------------------------------------------------------------------
/**
 * \file PIAliasSuite.h
 *
 * \brief This file describes the Photoshop Alias Suite for plug-ins.
 *
 * Distribution:
 *	PUBLIC
 *
 */

/**
* @defgroup PicaAliasSuite Allias Suite 
* Allows plug-ins to create aliases. 
*
* <b> Accessing the Suite </b> \n\n
* The Alias suite is referred to as:
* @code
  #define kPSAliasSuite				"Photoshop Alias Utility Suite"
@endcode
* The current version of the Alias suite:
* @code
  #define kPSAliasSuiteVersion		kPSAliasSuiteVersion2
@endcode
* The suite is acquired as follows:
* @code
  PSAliasSuite *sPSAlias;
  error = sSPBasic->AcquireSuite(kPSAliasSuite,
  								 kPSAliasSuiteVersion, 	  
  								 &sPSAlias);
  if (error) goto . . . //handle error
@endcode
* For PICA errors, see SPErrorCodes.h.
* @{
*/


#pragma once

#ifndef __PIAliasSuite__
#define __PIAliasSuite__

//-------------------------------------------------------------------------------
//	Includes.
//-------------------------------------------------------------------------------
#include "PIGeneral.h"

//-------------------------------------------------------------------------------
//	C++ wrapper.
//-------------------------------------------------------------------------------
#ifdef __cplusplus
extern "C" {
#endif

//-------------------------------------------------------------------------------
//	UI Alias suite.
//-------------------------------------------------------------------------------
/// Unique identifier for the Alias Suite.
#define kPSAliasSuite				"Photoshop Alias Utility Suite"


//-------------------------------------------------------------------------------
//	
//-------------------------------------------------------------------------------

#if MSWindows
typedef struct AliasRecord* AliasPtr;
typedef AliasPtr* AliasHandle;
#endif

/**
* NOTE that on the Mac, this suite should be considered deprecated as of
* Photoshop 17.0. Please use bookmarks instead. See GetBookmark and
* PutBookmark function documentation in the ActionDescriptor and ActionList
* suites.
*/

/** 
* Makes a new Mac OS alias from a C String.
* @deprecated Please use NSURL functions to create bookmarks instead
* @param cStringPathToFile The C string from which to make an alias.
* @param alias [OUT] The alias, returned.
* @returns Non-zero error if failure.
*/
typedef	SPAPI SPErr			(*MacNewAliasFromCStringProc) (const char *cStringPathToFile, AliasHandle *alias  );

/** 
* Makes a new Mac OS alias from a file specification reference.
* @deprecated Please use NSURL functions to create bookmarks instead
* @param fsRefToFile The file specification reference from which to make an alias.
* @param alias [OUT] The alias, returned.
* @returns Non-zero error if failure.
*/
typedef	SPAPI SPErr			(*MacNewAliasFromFSRefProc)( const void *fsRefToFile, AliasHandle *alias  );
	
/** 
* Makes a new Mac OS alias from a file specification directory reference and a C String.
* @deprecated Please use NSURL functions to create bookmarks instead
* @param cStringPathToFile The C string from which to make an alias.  This is the file name.
* @param fsRefToParentDir The file specification directory reference from which to make an alias. This is the path to the file.
* @param alias [OUT] The alias, returned.
* @returns Non-zero error if failure.
*/
typedef	SPAPI SPErr			(*MacNewAliasFromNameAndFSRefProc)( const void *cfStringFilename, const void *fsRefToParentDir, AliasHandle *alias );
	
/** 
* Makes a new Windows alias from a Unicode string path.
* @param filePathAsUTF16 The file path encoded in UTF16.
* @param alias [OUT] The alias, returned.
* @returns Non-zero error if failure.
*/
typedef	SPAPI SPErr			(*WinNewAliasFromWidePathProc)( const uint16 *filePathAsUTF16, AliasHandle *alias );

/** 
* Makes a new Windows alias from a PIDL.
* @param pidlToFile
* @param alias [OUT] The alias, returned.
* @returns Non-zero error if failure.
*/
typedef	SPAPI SPErr			(*WinNewAliasFromPIDLProc)( const void *pidlToFile, AliasHandle *alias );

/** 
* Makes a new alias from a cloud document path.
* @param cloudPath The C string with UTF8 encoding containing the cloud path from which to make an alias.
* @param alias [OUT] The alias, returned.
* @returns Non-zero error if failure.
*/
typedef	SPAPI SPErr			(*NewAliasFromCloudPathProc) ( const char *cloudPath, AliasHandle *alias  );

/** Used to query whether or not an alias is representing a cloud file.
If it is, then it is valid to call CloudReferenceFromAlias or 
CloudPathFromAlias to get the cloud asset information.
@param alias            The alias to query. This may not be null.
@param isCloud [OUT]    True if the alias is representing a cloud file.
*/
typedef  SPAPI SPErr		(*IsCloudFileProc) ( AliasHandle alias, SPBoolean* isCloud );
 
/** Used to get the cloud asset reference for an alias that is related to a
cloud file. This is the complete asset reference.
This method should only be called if IsCloudFile returns true.
@param alias            The alias to query. This may not be null.
@param reference [OUT]  The asset reference for the cloud asset, Callers should treat
                        this value as an opaque structure and callers should not
                        try to create or alter such references using string operations.
*/
typedef  SPAPI SPErr		(*CloudReferenceFromAliasProc) ( AliasHandle alias, ASZString* reference );
 
/** Used to get the cloud asset reference path for an alias that is related to a
cloud file. This is the short asset reference.
This method should only be called if IsCloudFile returns true.
@param alias            The alias to query. This may not be null
@param reference [OUT]  The asset reference for the cloud asset, Callers should treat
                        this value as an opaque structure and callers should not
                        try to create or alter such references using string operations.
*/
typedef  SPAPI SPErr		(*CloudPathFromAliasProc) ( AliasHandle alias, ASZString* reference );
 
/** Used to create an alias from a cloud asset reference.
@param reference        The (cloud) asset reference for the cloud file. This value
                        is typically obtained from a previous call to CloudReferenceFromAlias 
@param alias [OUT]      The alias, returned.
*/
typedef  SPAPI SPErr		(*NewAliasFromCloudReferenceProc) ( const char *reference, AliasHandle *alias );

/** Used to create an alias from a unicode path.
@param unicodePath      The unicode path to the file. Use this routine for 
                        creating the alias from a previous CloudPathFromAlias call.
@param alias [OUT]      The alias, returned.
*/
typedef	SPAPI SPErr			(*UnicodePathToAliasProc)( const uint16 * unicodePath, AliasHandle* alias );

/** Used to create a unicode path from an alias.
@param alias             The alias to query. This may not be null. 
@param unicodePath [OUT] The unicode path, returned.
@param length            The maximum number of UTF16 characters to copy into unicodePath.
*/
typedef SPAPI SPErr			(*AliasToUnicodePathProc)( const AliasHandle alias, uint16 * unicodePath, uint32 length );




//-------------------------------------------------------------------------------
//	 Alias suite versions 1 & 2.
//-------------------------------------------------------------------------------

/// Current version of the Alias Suite
#define kPSAliasSuiteVersion1		1
#define kPSAliasSuiteVersion2		2
#define kPSAliasSuiteVersion		kPSAliasSuiteVersion2	// Current version.

/** The set of routines available in the Alias Suite*/
typedef struct 
	{
		// kPSAliasSuiteVersion1
		MacNewAliasFromCStringProc MacNewAliasFromCString;				/**< Function pointer to \c MacNewAliasFromCStringProc. */
		MacNewAliasFromFSRefProc MacNewAliasFromFSRef;					/**< Function pointer to \c MacNewAliasFromFSRefProc. */
		MacNewAliasFromNameAndFSRefProc MacNewAliasFromNameAndFSRef;	/**< Function pointer to \c MacNewAliasFromNameAndFSRefProc. */
		WinNewAliasFromWidePathProc WinNewAliasFromWidePath;			/**< Function pointer to \c WinNewAliasFromWidePathProc. */
		WinNewAliasFromPIDLProc WinNewAliasFromPIDL;					/**< Function pointer to \c WinNewAliasFromPIDLProc. */
	} PSAliasSuite1;

typedef struct 
	{
		// kPSAliasSuiteVersion1
		MacNewAliasFromCStringProc MacNewAliasFromCString;				/**< Function pointer to \c MacNewAliasFromCStringProc. */
		MacNewAliasFromFSRefProc MacNewAliasFromFSRef;					/**< Function pointer to \c MacNewAliasFromFSRefProc. */
		MacNewAliasFromNameAndFSRefProc MacNewAliasFromNameAndFSRef;	/**< Function pointer to \c MacNewAliasFromNameAndFSRefProc. */
		WinNewAliasFromWidePathProc WinNewAliasFromWidePath;			/**< Function pointer to \c WinNewAliasFromWidePathProc. */
		WinNewAliasFromPIDLProc WinNewAliasFromPIDL;					/**< Function pointer to \c WinNewAliasFromPIDLProc. */
		
		// kPSAliasSuiteVersion2
		NewAliasFromCloudPathProc NewAliasFromCloudPath;				/**< Function pointer to \c NewAliasFromCloudPathProc. */
		IsCloudFileProc IsCloudFile;									/**< Function pointer to \c IsCloudFileProc. */
		CloudReferenceFromAliasProc CloudReferenceFromAlias;			/**< Function pointer to \c CloudReferenceFromAliasProc. */
		CloudPathFromAliasProc CloudPathFromAlias;						/**< Function pointer to \c CloudPathFromAliasProc. */
		NewAliasFromCloudReferenceProc NewAliasFromCloudReference;		/**< Function pointer to \c NewAliasFromCloudReferenceProc. */
		UnicodePathToAliasProc UnicodePathToAlias;						/**< Function pointer to \c UnicodePathToAliasProc. */
		AliasToUnicodePathProc AliasToUnicodePath;						/**< Function pointer to \c AliasToUnicodePathProc. */
	} PSAliasSuite2;

typedef PSAliasSuite2 PSAliasSuite;

//-------------------------------------------------------------------------------
//	C++ wrapper.
//-------------------------------------------------------------------------------
#ifdef __cplusplus
}
#endif

/** @} */
#endif // __PIAliasSuite__

