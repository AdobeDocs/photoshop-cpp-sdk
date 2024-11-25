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

#include "PIUASZString.h"

#include "PIUExceptions.h"
#include "PIUSuites.h"

/******************************************************************************/

ASZString PIUASZString::GetEmpty ()
	{
	REQUIRE_NON_NULL (sASZString);

	return sASZString->GetEmpty ();
	}

/******************************************************************************/

PIUASZString PIUASZString::RomanizationOf (ASInt32 inValue)
	{
	REQUIRE_NON_NULL (sASZString);

	PIUASZString		asZString;
	
	ThrowIfASErr (sASZString->MakeRomanizationOfInteger (inValue, asZString));
	REQUIRE (!asZString.IsEmpty ());
	
	return asZString;
	}
	
/******************************************************************************/
#if Macintosh
#pragma mark -
#endif
/******************************************************************************/

PIUASZString::PIUASZString ()
	:	mASZString (NULL),
		mASUnicodeString (NULL),
		mASUnicodeLength (0)
	{
	}

/******************************************************************************/

PIUASZString::PIUASZString (const PIUASZString &inASZString)
	:	mASZString (NULL),
		mASUnicodeString (NULL),
		mASUnicodeLength (0)
	{
	// Copy, if not none
	if (inASZString.mASZString != NULL)
		{
		REQUIRE_NON_NULL (sASZString);

		ThrowIfASErr (sASZString->Copy (inASZString.mASZString, &mASZString));
		}
	}

/******************************************************************************/

PIUASZString::PIUASZString (const ASUnicode *inUnicodeString)
	:	mASZString (NULL),
		mASUnicodeString (NULL),
		mASUnicodeLength (0)
	{
	Assign (inUnicodeString);
	}

/******************************************************************************/

PIUASZString::PIUASZString (const char *inCString)
	:	mASZString (NULL),
		mASUnicodeString (NULL),
		mASUnicodeLength (0)
	{
	Assign (inCString);
	}

/******************************************************************************/

PIUASZString::PIUASZString (const std::string &inCString)
	:	mASZString (NULL),
		mASUnicodeString (NULL),
		mASUnicodeLength (0)
	{
	Assign (inCString);
	}

/******************************************************************************/

PIUASZString::~PIUASZString ()
    {
    try
        {
        Free ();
        }
    catch(...)
        {
        ; // don't throw in a destructor std::terminate gets called on the mac
        }
    }

/******************************************************************************/

PIUASZString& PIUASZString::operator= (const ASUnicode *inUnicodeString)
	{
	Assign (inUnicodeString);
	
	return *this;
	}

/******************************************************************************/

PIUASZString& PIUASZString::operator= (const char *inCString)
	{
	Assign (inCString);
	
	return *this;
	}

/******************************************************************************/

PIUASZString& PIUASZString::operator= (const std::string &inCString)
	{
	Assign (inCString);
	
	return *this;
	}

/******************************************************************************/

void PIUASZString::Assign (const ASUnicode *inUnicodeString)
	{
	REQUIRE_NON_NULL_PARAMETER (inUnicodeString);
	REQUIRE_NON_NULL (sASZString);

	ASZString			asZString = NULL;
	
	try
		{
		unsigned long		length;

		// Calculate the length of the string
		length = 0;
		for (const ASUnicode *unicodeString = inUnicodeString;
				*unicodeString != 0; unicodeString++)
			length++;

		// Translate to a ASZString
		ThrowIfASErr (sASZString->MakeFromUnicode ((ASUnicode *)inUnicodeString, length, &asZString));

		// Free anything that we currently have
		Free ();
	
		// Remember
		mASZString = asZString;
		asZString = NULL;
		}
	catch (...)
		{
		if (asZString != NULL)
			sASZString->Release (asZString);
		throw;
		}
	}

/******************************************************************************/

void PIUASZString::Assign (const char *inCString)
	{
	REQUIRE_NON_NULL_PARAMETER (inCString);
		
	return Assign (std::string (inCString));
	}
	
/******************************************************************************/

void PIUASZString::Assign (const std::string &inCString)
	{
	REQUIRE_NON_NULL (sASZString);

	ASZString			asZString = NULL;
	
	try
		{
		// Translate to a ASZString
		ThrowIfASErr (sASZString->MakeFromCString (inCString.c_str (), 
				inCString.length (), &asZString));

		// Free anything that we currently have
		Free ();
	
		// Remember
		mASZString = asZString;
		asZString = NULL;
		}
	catch (...)
		{
		if (asZString != NULL)
			sASZString->Release (asZString);
		throw;
		}
	}

/******************************************************************************/

void PIUASZString::Free ()
	{
	FreeUnicode ();
	
	if (mASZString != NULL)
		{
		REQUIRE_NON_NULL (sASZString);
		
		sASZString->Release (mASZString);
		mASZString = NULL;
		}
	}

/******************************************************************************/

ASZString PIUASZString::Release ()
	{
	ASZString		asZString;
	
	// Save a local copy, forget it
	asZString = mASZString;
	mASZString = NULL;
	
	// Free everything else
	Free ();
	
	return asZString;
	}
	
/******************************************************************************/

bool PIUASZString::IsEmpty () const
	{
	REQUIRE_NON_NULL (sASZString);
	
	return 0 != sASZString->IsEmpty (mASZString);
	}

/******************************************************************************/

PIUASZString& PIUASZString::Replace (int /*ASUInt32*/ inIndex, ASZString inReplacement)
	{
	REQUIRE_NON_NULL (sASZString);
	REQUIRE_NON_NULL (mASZString);
	
	ThrowIfASErr (sASZString->Replace (mASZString, inIndex, inReplacement));
	
	FreeUnicode ();
	
	return *this;
	}

/******************************************************************************/

PIUASZString::operator ASZString *()
	{
	REQUIRE (mASZString == NULL);
	
	return &mASZString;
	}
	
/******************************************************************************/

PIUASZString::operator ASZString () const
	{
	return mASZString;
	}

/******************************************************************************/

const ASUnicode * PIUASZString::GetUnicodeString ()
	{
	AllocateUnicode ();
	
	return mASUnicodeString;
	}
	
/******************************************************************************/

ASUInt32 PIUASZString::GetUnicodeStringLength ()
	{
	AllocateUnicode ();
	
	return mASUnicodeLength;
	}

/******************************************************************************/

void PIUASZString::AllocateUnicode ()
	{
	REQUIRE_NON_NULL (sASZString);
	
	// If we don't have the unicode string, then allocate
	if (mASUnicodeString == NULL)
		{
		ASUInt32		unicodeLength;
		ASUnicode *		unicodeString = NULL;

		try
			{
			// Determine size of buffer
			unicodeLength = sASZString->LengthAsUnicodeCString (mASZString);
			REQUIRE (unicodeLength > 0);
			
			// Allocate
			unicodeString = new ASUnicode[unicodeLength];
			if (unicodeString == NULL)
				ThrowOSErr (memFullErr);

			// Get the unicode string
			ThrowIfASErr (sASZString->AsUnicodeCString (mASZString, unicodeString, unicodeLength, true));
			}
		catch (...)
			{
			if (unicodeString != NULL)
				{
				delete[] unicodeString;
				unicodeString = NULL;
				}

			throw;
			}

		// Remember
		mASUnicodeString = unicodeString;
		mASUnicodeLength = unicodeLength;
		}
	}
	
/******************************************************************************/

void PIUASZString::FreeUnicode ()
	{
	if (mASUnicodeString != NULL) 
		{
		delete[] mASUnicodeString;
		mASUnicodeString = NULL;
		}
		
	mASUnicodeLength = 0;
	}
