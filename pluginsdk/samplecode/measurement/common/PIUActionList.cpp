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

#include "PIUActionList.h"

#include "PIUActionControl.h"
#include "PIUExceptions.h"
#include "PIUSuites.h"

/******************************************************************************/

PIUActionList::PIUActionList ()
	:	fList (NULL),
		fOwned (true)
	{
	}

/******************************************************************************/

PIUActionList::PIUActionList (PIActionList inList, bool inOwned)
	:	fList (inList),
		fOwned (inOwned)
	{
	}

/******************************************************************************/

PIUActionList::~PIUActionList ()
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

PIUActionList &PIUActionList::operator= (PIActionList inList)
	{
	REQUIRE_NON_NULL_PARAMETER (inList);
	
	Free ();
	
	fList = inList;
	fOwned = true;
	
	return *this;
	}

/******************************************************************************/

PIUActionList::operator bool () const
	{
	return (fList != NULL);
	}

/******************************************************************************/

PIUActionList::operator PIActionList () const
	{
	return fList;
	}

/******************************************************************************/

void PIUActionList::Make ()
	{
	REQUIRE_NON_NULL (sPSActionList);

	PIActionList		list;
	
	Free ();
	
	ThrowIfOSErr (sPSActionList->Make (&list));
	REQUIRE_NON_NULL (list);
	
	fList = list;
	fOwned = true;
	}

/******************************************************************************/

void PIUActionList::Free ()
	{
	REQUIRE_NON_NULL (sPSActionList);

	if (fList != NULL)
		{
		if (fOwned)
			sPSActionList->Free (fList);
			
		fList = NULL;
		fOwned = true;
		}
	}

/******************************************************************************/

PIActionList PIUActionList::Release ()
	{
	PIActionList	list;
	
	// Local copy
	list = fList;
	
	// Forget
	fList = NULL;
	fOwned = true;
	
	return list;
	}

/******************************************************************************/

uint32 PIUActionList::GetCount ()
	{
	REQUIRE_NON_NULL (sPSActionList);

	// Special case to make things a bit easier
	if (fList == NULL)
		return 0;

	uint32		count;
	
	ThrowIfOSErr (sPSActionList->GetCount (fList, &count));
	
	return count;
	}

/******************************************************************************/

DescriptorTypeID PIUActionList::GetType (uint32 inIndex)
	{
	REQUIRE_NON_NULL (sPSActionList);
	REQUIRE_NON_NULL (fList);
	
	DescriptorTypeID	type;

	ThrowIfOSErr (sPSActionList->GetType (fList, inIndex, &type));
		
	return type;
	}

/******************************************************************************/

int32 PIUActionList::GetInteger (uint32 inIndex)
	{
	REQUIRE_NON_NULL (sPSActionList);
	REQUIRE_NON_NULL (fList);

	int32			value;

	ThrowIfOSErr (sPSActionList->GetInteger (fList, inIndex, &value));
	
	return value;
	}

/******************************************************************************/

void PIUActionList::PutInteger (int32 inValue)
	{
	REQUIRE_NON_NULL (sPSActionList);
	REQUIRE_NON_NULL (fList);

	ThrowIfOSErr (sPSActionList->PutInteger (fList, inValue));
	}

/******************************************************************************/

double PIUActionList::GetFloat (uint32 inIndex)
	{
	REQUIRE_NON_NULL (sPSActionList);
	REQUIRE_NON_NULL (fList);

	double			value;

	ThrowIfOSErr (sPSActionList->GetFloat (fList, inIndex, &value));
	
	return value;
	}

/******************************************************************************/

void PIUActionList::PutFloat (double inValue)
	{
	REQUIRE_NON_NULL (sPSActionList);
	REQUIRE_NON_NULL (fList);

	ThrowIfOSErr (sPSActionList->PutFloat (fList, inValue));
	}

/******************************************************************************/

bool PIUActionList::GetBoolean (uint32 inIndex)
	{
	REQUIRE_NON_NULL (sPSActionList);
	REQUIRE_NON_NULL (fList);

	Boolean			value;

	ThrowIfOSErr (sPSActionList->GetBoolean (fList, inIndex, &value));

	return (value != 0);
	}

/******************************************************************************/

void PIUActionList::PutBoolean (bool inValue)
	{
	REQUIRE_NON_NULL (sPSActionList);
	REQUIRE_NON_NULL (fList);

	ThrowIfOSErr (sPSActionList->PutBoolean (fList, inValue));
	}

/******************************************************************************/

std::string PIUActionList::GetString (uint32 inIndex)
	{
	REQUIRE_NON_NULL (sPSActionList);
	REQUIRE_NON_NULL (fList);

	std::string		value;
	char			*buffer = NULL;

	try
		{
		uint32		length;

		// Get the length
		ThrowIfOSErr (sPSActionList->GetStringLength (fList, inIndex, &length));
			
		// Add one for the null terminator (unfortunately, this is necessary)
		length++;

		// Get the string into a temporary buffer
		buffer = new char[length];
		if (buffer == NULL)
			ThrowOSErr (memFullErr);
		ThrowIfOSErr (sPSActionList->GetString (fList, inIndex, buffer, length));

		// Remember
		value = std::string (buffer);

		delete[] buffer;
		buffer = NULL;
		}
	catch (...)
		{
		if (buffer != NULL)
			{
			delete[] buffer;
			buffer = NULL;
			}
		throw;
		}
	
	return value;
	}

/******************************************************************************/

void PIUActionList::PutString (const std::string &inValue)
	{
	PutString (inValue.c_str ());
	}

/******************************************************************************/

void PIUActionList::PutString (const char *inValue)
	{
	REQUIRE_NON_NULL_PARAMETER (inValue);
	REQUIRE_NON_NULL (sPSActionList);
	REQUIRE_NON_NULL (fList);
	
	ThrowIfOSErr (sPSActionList->PutString (fList, const_cast<char *> (inValue)));
	}
	
/******************************************************************************/

ASZString PIUActionList::GetZString (uint32 inIndex)
	{
	REQUIRE_NON_NULL (sPSActionList);
	REQUIRE_NON_NULL (fList);
	
	ASZString		value;

	ThrowIfOSErr (sPSActionList->GetZString (fList, inIndex, &value));

	return value;
	}

/******************************************************************************/

void PIUActionList::PutZString (ASZString inValue)
	{
	REQUIRE_NON_NULL (sPSActionList);
	REQUIRE_NON_NULL (fList);
	
	ThrowIfOSErr (sPSActionList->PutZString (fList, inValue));
	}

/******************************************************************************/

PIActionDescriptor PIUActionList::GetObject (uint32 inIndex)
	{
	REQUIRE_NON_NULL (sPSActionList);
	REQUIRE_NON_NULL (fList);

	DescriptorClassID	type;
	PIActionDescriptor	value;

	ThrowIfOSErr (sPSActionList->GetObject (fList, inIndex, &type, &value));
	
	return value;
	}

/******************************************************************************/

void PIUActionList::PutObject (const char *inType, PIActionDescriptor inValue)
	{
	PutObject (PIUActionControl::GetStringID (inType), inValue);
	}
	
/******************************************************************************/

void PIUActionList::PutObject (DescriptorClassID inType, PIActionDescriptor inValue)
	{
	REQUIRE_NON_NULL_PARAMETER (inValue);
	REQUIRE_NON_NULL (sPSActionList);
	REQUIRE_NON_NULL (fList);

	ThrowIfOSErr (sPSActionList->PutObject (fList, inType, inValue));
	}

/******************************************************************************/

PIActionList PIUActionList::GetList (uint32 inIndex)
	{
	REQUIRE_NON_NULL (sPSActionList);
	REQUIRE_NON_NULL (fList);
	
	PIActionList	value;

	ThrowIfOSErr (sPSActionList->GetList (fList, inIndex, &value));
	
	return value;
	}

/******************************************************************************/

void PIUActionList::PutList (PIActionList inValue)
	{
	REQUIRE_NON_NULL_PARAMETER (inValue);
	REQUIRE_NON_NULL (sPSActionList);
	REQUIRE_NON_NULL (fList);

	ThrowIfOSErr (sPSActionList->PutList (fList, inValue));
	}

/******************************************************************************/

int32 PIUActionList::GetDataLength (uint32 inIndex)
	{
	REQUIRE_NON_NULL (sPSActionList);
	REQUIRE_NON_NULL (fList);
	
	int32		length;

	ThrowIfOSErr (sPSActionList->GetDataLength (fList, inIndex, &length));
	
	return length;
	}

/******************************************************************************/

void PIUActionList::GetData (uint32 inIndex, void *outValue)
	{
	REQUIRE_NON_NULL_PARAMETER (outValue);
	REQUIRE_NON_NULL (sPSActionList);
	REQUIRE_NON_NULL (fList);
	
	ThrowIfOSErr (sPSActionList->GetData (fList, inIndex, outValue));
	}

/******************************************************************************/

void PIUActionList::PutData (void *inValue, int32 inLength)
	{
	REQUIRE_NON_NULL_PARAMETER (inValue);
	REQUIRE_NON_NULL (sPSActionList);
	REQUIRE_NON_NULL (fList);

	ThrowIfOSErr (sPSActionList->PutData (fList, inLength, inValue));
	}
