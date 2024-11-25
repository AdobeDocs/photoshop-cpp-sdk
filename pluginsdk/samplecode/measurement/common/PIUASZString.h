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

#ifndef __PIUASZString__
#define __PIUASZString__

#include <string>

#include "ASZStringSuite.h"

/**
 * Wrap an ASZString object to allow for easy creation, automatic destruction and
 * cleanup, sane error handling through exceptions, easy conversion to 
 * ASUnicode strings, standard ZString replacement, and romanization of integers
 * via a C++ object interface.  See PIUExceptions.h for more 
 * information about exceptions.
 *
 * An empty string may be represented by either a NULL ASZString or a valid
 * ASZString where the ASZStringSuite function IsEmpty returns true.  Both
 * are encapsulated by the PIUASZString::IsEmpty method.
 */
class PIUASZString
	{
	public:

		static ASZString GetEmpty ();
		static PIUASZString RomanizationOf (ASInt32 inValue);

	public:

		PIUASZString ();
		PIUASZString (const PIUASZString &inASZString);
		PIUASZString (const ASUnicode *inUnicodeString);
		PIUASZString (const char *inCString);
		PIUASZString (const std::string &inCString);
		virtual ~PIUASZString ();

		PIUASZString &operator= (const ASUnicode *inUnicodeString);
		PIUASZString &operator= (const char *inCString);
		PIUASZString &operator= (const std::string &inCString);

		void Assign (const ASUnicode *inUnicodeString);
		void Assign (const char *inCString);
		void Assign (const std::string &inCString);

		void Free ();
		ASZString Release ();
		
		bool IsEmpty () const;

		PIUASZString &Replace (int /*ASUInt32*/ inIndex, ASZString inReplacement);

		operator ASZString* ();
		operator ASZString () const;

		const ASUnicode *GetUnicodeString ();
		ASUInt32 GetUnicodeStringLength ();

	private:
	
		void AllocateUnicode ();
		void FreeUnicode ();
	
	private:
		
		ASZString				mASZString;
		ASUnicode *				mASUnicodeString;
		ASUInt32				mASUnicodeLength;

	};

#endif /* __PIUASZString__ */
