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

#ifndef __PIUExceptions__
#define __PIUExceptions__

#include <string>

#include "ASTypes.h"
#include "PITypes.h"

/******************************************************************************/

/**
 * Base class for all exceptions thrown in a plugin.  Contains an
 * error code and message.
 */
class PIUException
	{
	public:

		PIUException (int16 inError, 
				const std::string &inMessage);
		virtual ~PIUException ();

		int16 GetError () const;
		const std::string &GetMessage () const;
				
	private:

		PIUException ();

		int16				mError;
		std::string			mMessage;
		
	};

/******************************************************************************/

/**
 * Exception for illegal arguments to methods or functions.
 */
class PIUIllegalArgumentException : public PIUException
	{
	private:
	
		typedef PIUException _super;
		
	public:

		PIUIllegalArgumentException (const std::string &inMessage);
		virtual	~PIUIllegalArgumentException ();
													
	};

/******************************************************************************/

/**
 * Require that a condition pertaining to a parameter is valid.  Otherwise, 
 * throw a PIUIllegalArgumentException.
 *
 * \param inCondition the condition to test
 */
#define REQUIRE_PARAMETER(inCondition)								\
		do															\
			{														\
			if (!(inCondition))										\
				throw PIUIllegalArgumentException (#inCondition);	\
			}														\
		while (false)

/**
 * Require that a parameter is not NULL.  Otherwise, throw a
 * PIUIllegalArgumentException.
 *
 * \param inNonNull the value to test against NULL
 */
#define REQUIRE_NON_NULL_PARAMETER(inNonNull)						\
		do															\
			{														\
			if ((inNonNull) == NULL)								\
				throw PIUIllegalArgumentException (#inNonNull);		\
			}														\
		while (false)

/******************************************************************************/

/**
 * Exception to signal an illegal state within the code.
 */
class PIUIllegalStateException : public PIUException
	{
	private:
	
		typedef PIUException _super;
		
	public:

		PIUIllegalStateException (const std::string &inMessage);
		virtual	~PIUIllegalStateException ();
													
	};

/******************************************************************************/

/**
 * Require that a condition is valid.  Otherwise, throw a 
 * PIUIllegalStateException.
 *
 * \param inCondition the condition to test
 */
#define REQUIRE(inCondition)										\
		do															\
			{														\
			if (!(inCondition))										\
				throw PIUIllegalStateException (#inCondition);		\
			}														\
		while (false)

/**
 * Require that a value is not NULL.  Otherwise, throw a 
 * PIUIllegalStateException.
 *
 * \param inNonNull the value to test against NULL
 */
#define REQUIRE_NON_NULL(inNonNull)									\
		do															\
			{														\
			if ((inNonNull) == NULL)								\
				throw PIUIllegalStateException (#inNonNull);		\
			}														\
		while (false)
		
/**
 * Throw a PIUIllegalStateException with the specified message.
 *
 * \param inMessage the message to store in the PIUIllegalStateException
 */
#define ThrowLogicErrorDetailed(inMessage)							\
		do															\
			{														\
			throw PIUIllegalStateException (inMessage);				\
			}														\
		while (false)

/******************************************************************************/

/**
 * An exception wrapping an ASErr.  Translate to a plugin error code.
 */
class PIUASException : public PIUException
	{
	public:
	
		static int16 TranslateASErr (ASErr inASErr);

	private:
	
		typedef PIUException _super;

	public:

		PIUASException (ASErr inASErr, const std::string &inMessage);
		virtual ~PIUASException ();

		ASErr GetASErr () const;

	private:

		ASErr	mASErr;
		
	};

/******************************************************************************/

/**
 * Throw a PIUASException with the specified ASErr.
 *
 * \param inErr the ASErr to pass to the PIUASException object thrown
 */
#define ThrowASErr(inErr)											\
		do															\
			{														\
			throw PIUASException (inErr, #inErr);					\
			}														\
		while (false)

/**
 * Throw a PIUASException object if the specified ASErr is not kASNoError.
 *
 * \param inErr the ASErr to test
 */
#define ThrowIfASErr(inErr)											\
		do															\
			{														\
			ASErr __err = (inErr);									\
			if (__err != kASNoError) 								\
				throw PIUASException (__err, #inErr);				\
			}														\
		while (false)

/******************************************************************************/

/**
 * An exception wrapping an SPErr.  Translate to a plugin error code.
 */
class PIUSPException : public PIUException
	{
	public:
	
		static int16 TranslateSPErr (SPErr inSPErr);

	private:
	
		typedef PIUException _super;

	public:

		PIUSPException (SPErr inSPErr, const std::string &inMessage);
		virtual ~PIUSPException ();

		SPErr GetSPErr () const;

	private:

		SPErr	mSPErr;
		
	};

/******************************************************************************/

/**
 * Throw a PIUSPException with the specified SPErr.
 *
 * \param inErr the SPErr to pass to the PIUSPException object thrown
 */
#define ThrowSPErr(inErr)											\
		do															\
			{														\
			throw PIUSPException (inErr, #inErr);					\
			}														\
		while (false)

/**
 * Throw a PIUSPException object if the specified SPErr is not kSPNoError.
 *
 * \param inErr the SPErr to test
 */
#define ThrowIfSPErr(inErr)											\
		do															\
			{														\
			SPErr __err = (inErr);									\
			if (__err != kSPNoError)								\
				throw PIUSPException (__err, #inErr);				\
			}														\
		while (false)

/******************************************************************************/

/**
 * An exception wrapping an OSErr.  Translate to a plugin error code.
 */
class PIUOSException : public PIUException
	{
	public:
	
		static int16 TranslateOSErr (OSErr inOSErr);

	private:
	
		typedef PIUException _super;

	public:

		PIUOSException (OSErr inOSErr, const std::string &inMessage);
		virtual ~PIUOSException ();

		OSErr GetOSErr () const;

	private:

		OSErr	mOSErr;
		
	};

/******************************************************************************/

/**
 * Throw a PIUOSException with the specified OSErr.
 *
 * \param inErr the OSErr to pass to the PIUOSException object thrown
 */
#define ThrowOSErr(inErr)											\
		do															\
			{														\
			throw PIUOSException (inErr, #inErr);					\
			}														\
		while (false)

/**
 * Throw a PIUOSException object if the specified OSErr is not noErr.
 *
 * \param inErr the OSErr to test
 */
#define ThrowIfOSErr(inErr)											\
		do															\
			{														\
			OSErr __err = (inErr);									\
			if (__err != noErr)										\
				throw PIUOSException (__err, #inErr);				\
			}														\
		while (false)

/******************************************************************************/

#endif /* __PIUExceptions__ */
