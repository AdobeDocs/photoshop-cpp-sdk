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

#ifndef __PIUActionDescriptor__
#define __PIUActionDescriptor__

#include <string>

#include "PIActions.h"

/**
 * Wrap a PIActionDescriptor object to allow for easy creation, automatic 
 * destruction and cleanup, sane parameter validation and error handling 
 * through exceptions, easy getters and putters that allow both DescriptorKeyID 
 * and string-based keys, all via a C++ object interface.  See 
 * PIUExceptions.h for more information about exceptions.
 *
 * The PIActionDescriptor may be owned or not.  If owned, then the wrapper 
 * takes care of freeing the PIActionDescriptor as necessary.
 */
class PIUActionDescriptor
	{
	public:
	
		PIUActionDescriptor ();
		PIUActionDescriptor (PIActionDescriptor inDescriptor, bool inOwned = true);
		virtual ~PIUActionDescriptor ();
		
		PIUActionDescriptor &operator= (PIActionDescriptor inDescriptor);
		
		operator bool () const;

		operator PIActionDescriptor () const;
		
		void Make ();
		void Free ();
		
		PIActionDescriptor Release ();

		uint32 GetCount ();
		
		bool HasKey (const char *inKey);
		bool HasKey (DescriptorKeyID inKey);
		
		DescriptorTypeID GetType (const char *inKey);
		DescriptorTypeID GetType (DescriptorKeyID inKey);
		
		void Erase (const char *inKey);
		void Erase (DescriptorKeyID inKey);
		
		int32 GetInteger (const char *inKey);
		int32 GetInteger (DescriptorKeyID inKey);
		void PutInteger (const char *inKey, int32 inValue);
		void PutInteger (DescriptorKeyID inKey, int32 inValue);
		
		double GetFloat (const char *inKey);
		double GetFloat (DescriptorKeyID inKey);
		void PutFloat (const char *inKey, double inValue);
		void PutFloat (DescriptorKeyID inKey, double inValue);
		
		bool GetBoolean (const char *inKey);
		bool GetBoolean (DescriptorKeyID inKey);
		void PutBoolean (const char *inKey, bool inValue);
		void PutBoolean (DescriptorKeyID inKey, bool inValue);
		
		std::string GetString (const char *inKey);
		std::string GetString (DescriptorKeyID inKey);
		void PutString (const char *inKey, const std::string &inValue);
		void PutString (DescriptorKeyID inKey, const std::string &inValue);
		void PutString (const char *inKey, const char *inValue);
		void PutString (DescriptorKeyID inKey, const char *inValue);
		
		ASZString GetZString (const char *inKey);
		ASZString GetZString (DescriptorKeyID inKey);
		void PutZString (const char *inKey, ASZString inValue);
		void PutZString (DescriptorKeyID inKey, ASZString inValue);
		
		PIActionDescriptor GetObject (const char *inKey);
		PIActionDescriptor GetObject (DescriptorKeyID inKey);
		void PutObject (const char *inKey, const char *inType, PIActionDescriptor inValue);
		void PutObject (DescriptorKeyID inKey, DescriptorClassID inType, PIActionDescriptor inValue);
		
		PIActionList GetList (const char *inKey);
		PIActionList GetList (DescriptorKeyID inKey);
		void PutList (const char *inKey, PIActionList inList);
		void PutList (DescriptorKeyID inKey, PIActionList inList);
		
		int32 GetDataLength (const char *inKey);
		int32 GetDataLength (DescriptorKeyID inKey);
		void GetData (const char *inKey, void *outValue);
		void GetData (DescriptorKeyID inKey, void *outValue);
		void PutData (const char *inKey, void *inValue, int32 inLength);
		void PutData (DescriptorKeyID inKey, void *inValue, int32 inLength);
		
	private:
	
		PIUActionDescriptor (const PIUActionDescriptor &inDescriptor);
		PIUActionDescriptor &operator= (const PIUActionDescriptor &inDescriptor);
		
	private:
	
		PIActionDescriptor	fDescriptor;
		bool				fOwned;

	};

#endif /* __PIUActionDescriptor__ */
