// ADOBE SYSTEMS INCORPORATED
// (c) Copyright  2020 Adobe Systems Incorporated
// All Rights Reserved
//
// NOTICE:  Adobe permits you to use, modify, and distribute this 
// file in accordance with the terms of the Adobe license agreement
// accompanying it.  If you have received this file from a source
// other than Adobe, then your use, modification, or distribution
// of it requires the prior written permission of Adobe.
//-------------------------------------------------------------------
/**
 *	\file PIResourceDefines.h
 *
 *	\brief This file contains helper macros to simplify defining the
 *         entry point into a plug-in
 *
 *	Distribution:
 *		PUBLIC
 *
 */

#ifndef PI_RESOURCE_DEFINES_H
#define PI_RESOURCE_DEFINES_H

/******************************************************************************/
// Macro helpers to define plugin entry points
/******************************************************************************/

#if defined(__PIWin__)
  #if __PIWin__
    #if defined(_M_AMD64)
        #define CodeEntryPointWin64 CodeWin64X86
    #elif defined(_M_ARM64)
        #define CodeEntryPointWin64 CodeWin64ARM
    #else
        #error unknown architecture
    #endif
  #endif
#endif

#endif // PI_RESOURCE_DEFINES_H
