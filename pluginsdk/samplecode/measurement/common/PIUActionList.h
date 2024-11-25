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

#ifndef __PIUActionList__
#define __PIUActionList__

#include <string>

#include "PIActions.h"

/**
 * Wrap a PIActionList object to allow for easy creation, automatic 
 * destruction and cleanup, sane parameter validation and error handling 
 * through exceptions, easy getters and putters, all via a C++ object interface.
 * See PIUExceptions.h for more information about exceptions.
 *
 * The PIActionList may be owned or not.  If owned, then the wrapper takes care
 * of freeing the PIActionList as necessary.
 */
class PIUActionList
	{
	public:

		PIUActionList ();
		PIUActionList (PIActionList inList, bool inOwned = true);
		virtual ~PIUActionList ();
		
		PIUActionList &operator= (PIActionList inList);

		operator bool () const;

		operator PIActionList () const;
		
		void Make ();
		void Free ();
		
		PIActionList Release ();
		
		uint32 GetCount ();
		
		DescriptorTypeID GetType (uint32 index);
		
		int32 GetInteger (uint32 inIndex);
		void PutInteger (int32 inValue);
		
		double GetFloat (uint32 inIndex);
		void PutFloat (double inValue);
		
		bool GetBoolean (uint32 inIndex);
		void PutBoolean (bool inValue);
		
		std::string GetString (uint32 inIndex);
		void PutString (const std::string &inValue);
		void PutString (const char *inValue);
		
		ASZString GetZString (uint32 inIndex);
		void PutZString (ASZString inValue);
		
		PIActionDescriptor GetObject (uint32 inIndex);
		void PutObject (const char *inType, PIActionDescriptor inValue);
		void PutObject (DescriptorClassID inType, PIActionDescriptor inValue);
		
		PIActionList GetList (uint32 inIndex);
		void PutList (PIActionList inList);
		
		int32 GetDataLength (uint32 inIndex);
		void GetData (uint32 inIndex, void *outValue);
		void PutData (void *inValue, int32 inLength);
		
	private:
	
		PIUActionList (const PIUActionList &inList);
		PIUActionList &operator= (const PIUActionList &inList);
		
	private:
	
		PIActionList	fList;
		bool			fOwned;

	};

#endif /* __PIUActionList__ */
