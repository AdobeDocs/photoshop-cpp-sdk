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

#include "PIUExceptions.h"

#include "PIGeneral.h"

#include "ASZStringSuite.h"

/******************************************************************************/

PIUException::PIUException (int16 inError, 
		const std::string &inMessage)
	:	mError (inError),
		mMessage (inMessage)
	{
	}

/******************************************************************************/

PIUException::~PIUException ()
	{
	}

/******************************************************************************/

int16 PIUException::GetError () const
	{
	return mError;
	}
	
/******************************************************************************/

const std::string &PIUException::GetMessage () const
	{
	return mMessage;
	}
	
/******************************************************************************/
#if Macintosh
#pragma mark -
#endif
/******************************************************************************/

PIUIllegalArgumentException::PIUIllegalArgumentException (const std::string &inMessage)
	:	_super (paramErr, inMessage)
	{
	}

/******************************************************************************/

PIUIllegalArgumentException::~PIUIllegalArgumentException ()
	{
	}

/******************************************************************************/
#if Macintosh
#pragma mark -
#endif
/******************************************************************************/

PIUIllegalStateException::PIUIllegalStateException (const std::string &inMessage)
	:	_super (paramErr, inMessage)
	{
	}

/******************************************************************************/

PIUIllegalStateException::~PIUIllegalStateException ()
	{
	}

/******************************************************************************/
#if Macintosh
#pragma mark -
#endif
/******************************************************************************/

int16 PIUASException::TranslateASErr (ASErr inASErr)
	{
	switch (inASErr)
		{
		case kASNoError:
			return noErr;
		case kASUnimplementedError:
			return paramErr;
		case kASUserCanceledError:
			return userCanceledErr;
		case kASUnknownErr:
			return paramErr;
		case kASBufferTooSmallErr:
			return nilHandleErr;
		case kASMemoryErr:
			return memFullErr;
		default:
			return paramErr;
		}
	}
	
/******************************************************************************/

PIUASException::PIUASException (ASErr inASErr, const std::string &inMessage)
	:	_super (TranslateASErr (inASErr), inMessage),
		mASErr (inASErr)
	{
	}

/******************************************************************************/

PIUASException::~PIUASException()
	{
	}

/******************************************************************************/

ASErr PIUASException::GetASErr () const
	{
	return mASErr;
	}

/******************************************************************************/
#if Macintosh
#pragma mark -
#endif
/******************************************************************************/

int16 PIUSPException::TranslateSPErr (SPErr inSPErr)
	{
	switch (inSPErr)
		{
		case kSPNoError:
			return noErr;
		case kSPUnimplementedError:
			return paramErr;
		case kSPUserCanceledError:
			return userCanceledErr;
		case kSPOperationInterrupted:
			return userCanceledErr;
		case kSPLogicError:
			return paramErr;
		case kSPBadParameterError:
			return paramErr;
		case kSPSuiteNotFoundError:
			return errPlugInHostInsufficient;
		case kSPSuiteAlreadyExistsError:
			return paramErr;
		case kSPSuiteAlreadyReleasedError:
			return paramErr;
		default:
			return paramErr;
		}
	}
	
/******************************************************************************/

PIUSPException::PIUSPException (SPErr inSPErr, const std::string &inMessage)
	:	_super (TranslateSPErr (inSPErr), inMessage),
		mSPErr (inSPErr)
	{
	}

/******************************************************************************/

PIUSPException::~PIUSPException()
	{
	}

/******************************************************************************/

SPErr PIUSPException::GetSPErr () const
	{
	return mSPErr;
	}

/******************************************************************************/
#if Macintosh
#pragma mark -
#endif
/******************************************************************************/

int16 PIUOSException::TranslateOSErr (OSErr inOSErr)
	{
	return inOSErr;		// Same thing
	}
	
/******************************************************************************/

PIUOSException::PIUOSException (OSErr inOSErr, const std::string &inMessage)
	:	_super (TranslateOSErr (inOSErr), inMessage),
		mOSErr (inOSErr)
	{
	}

/******************************************************************************/

PIUOSException::~PIUOSException()
	{
	}

/******************************************************************************/

OSErr PIUOSException::GetOSErr () const
	{
	return mOSErr;
	}
