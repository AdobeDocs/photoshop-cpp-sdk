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

#include "PIUActionDescriptor.h"

#include "PIUActionControl.h"
#include "PIUExceptions.h"
#include "PIUSuites.h"

/******************************************************************************/

PIUActionDescriptor::PIUActionDescriptor ()
	:	fDescriptor (NULL),
		fOwned (true)
	{
	}

/******************************************************************************/

PIUActionDescriptor::PIUActionDescriptor (PIActionDescriptor inDescriptor, bool inOwned)
	:	fDescriptor (inDescriptor),
		fOwned (inOwned)
	{
	}

/******************************************************************************/

PIUActionDescriptor::~PIUActionDescriptor ()
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

PIUActionDescriptor &PIUActionDescriptor::operator= (PIActionDescriptor inDescriptor)
	{
	REQUIRE_NON_NULL_PARAMETER (inDescriptor);
	
	Free ();
	
	fDescriptor = inDescriptor;
	fOwned = true;
	
	return *this;
	}

/******************************************************************************/

PIUActionDescriptor::operator bool() const
	{
	return (fDescriptor != NULL);
	}

/******************************************************************************/

PIUActionDescriptor::operator PIActionDescriptor () const
	{
	return fDescriptor;
	}

/******************************************************************************/

void PIUActionDescriptor::Make ()
	{
	REQUIRE_NON_NULL (sPSActionDescriptor);

	PIActionDescriptor		descriptor;
	
	Free ();
	
	ThrowIfOSErr (sPSActionDescriptor->Make (&descriptor));
	REQUIRE_NON_NULL (descriptor);
	
	fDescriptor = descriptor;
	fOwned = true;
	}

/******************************************************************************/

void PIUActionDescriptor::Free ()
	{
	REQUIRE_NON_NULL (sPSActionDescriptor);

	if (fDescriptor != NULL)
		{
		if (fOwned)
			sPSActionDescriptor->Free (fDescriptor);
		
		fDescriptor = NULL;
		fOwned = true;
		}
	}

/******************************************************************************/

PIActionDescriptor PIUActionDescriptor::Release ()
	{
	PIActionDescriptor	descriptor;
	
	// Local copy
	descriptor = fDescriptor;
	
	// Forget
	fDescriptor = NULL;
	fOwned = true;
	
	return descriptor;
	}

/******************************************************************************/

uint32 PIUActionDescriptor::GetCount ()
	{
	REQUIRE_NON_NULL (sPSActionDescriptor);

	// Special case to make things a bit easier
	if (fDescriptor == NULL)
		return 0;

	uint32		count;
	
	ThrowIfOSErr (sPSActionDescriptor->GetCount (fDescriptor, &count));
	
	return count;
	}

/******************************************************************************/

bool PIUActionDescriptor::HasKey (const char *inKey)
	{
	return HasKey (PIUActionControl::GetStringID (inKey));
	}
	
/******************************************************************************/

bool PIUActionDescriptor::HasKey (DescriptorKeyID inKey)
	{
	REQUIRE_NON_NULL (sPSActionDescriptor);
	REQUIRE_NON_NULL (fDescriptor);
	
	Boolean		hasKey;

	ThrowIfOSErr (sPSActionDescriptor->HasKey (fDescriptor, inKey, &hasKey));
		
	return (hasKey != 0);
	}

/******************************************************************************/

DescriptorTypeID PIUActionDescriptor::GetType (const char *inKey)
	{
	return GetType (PIUActionControl::GetStringID (inKey));
	}
	
/******************************************************************************/

DescriptorTypeID PIUActionDescriptor::GetType (DescriptorKeyID inKey)
	{
	REQUIRE_NON_NULL (sPSActionDescriptor);
	REQUIRE_NON_NULL (fDescriptor);
	
	DescriptorTypeID	type;

	ThrowIfOSErr (sPSActionDescriptor->GetType (fDescriptor, inKey, &type));
		
	return type;
	}

/******************************************************************************/

void PIUActionDescriptor::Erase (const char *inKey)
	{
	Erase (PIUActionControl::GetStringID (inKey));
	}
	
/******************************************************************************/

void PIUActionDescriptor::Erase (DescriptorKeyID inKey)
	{
	REQUIRE_NON_NULL (sPSActionDescriptor);
	REQUIRE_NON_NULL (fDescriptor);

	ThrowIfOSErr (sPSActionDescriptor->Erase (fDescriptor, inKey)); 
	}

/******************************************************************************/

int32 PIUActionDescriptor::GetInteger (const char *inKey)
	{
	return GetInteger (PIUActionControl::GetStringID (inKey));
	}
	
/******************************************************************************/

int32 PIUActionDescriptor::GetInteger (DescriptorKeyID inKey)
	{
	REQUIRE_NON_NULL (sPSActionDescriptor);
	REQUIRE_NON_NULL (fDescriptor);

	int32			value;

	ThrowIfOSErr (sPSActionDescriptor->GetInteger (fDescriptor, inKey, &value));
	
	return value;
	}

/******************************************************************************/

void PIUActionDescriptor::PutInteger (const char *inKey, int32 inValue)
	{
	PutInteger (PIUActionControl::GetStringID (inKey), inValue);
	}
	
/******************************************************************************/

void PIUActionDescriptor::PutInteger (DescriptorKeyID inKey, int32 inValue)
	{
	REQUIRE_NON_NULL (sPSActionDescriptor);
	REQUIRE_NON_NULL (fDescriptor);

	ThrowIfOSErr (sPSActionDescriptor->PutInteger (fDescriptor, inKey, inValue));
	}

/******************************************************************************/

double PIUActionDescriptor::GetFloat (const char *inKey)
	{
	return GetFloat (PIUActionControl::GetStringID (inKey));
	}
	
/******************************************************************************/

double PIUActionDescriptor::GetFloat (DescriptorKeyID inKey)
	{
	REQUIRE_NON_NULL (sPSActionDescriptor);
	REQUIRE_NON_NULL (fDescriptor);

	double			value;

	ThrowIfOSErr (sPSActionDescriptor->GetFloat (fDescriptor, inKey, &value));
	
	return value;
	}

/******************************************************************************/

void PIUActionDescriptor::PutFloat (const char *inKey, double inValue)
	{
	PutFloat (PIUActionControl::GetStringID (inKey), inValue);
	}
	
/******************************************************************************/

void PIUActionDescriptor::PutFloat (DescriptorKeyID inKey, double inValue)
	{
	REQUIRE_NON_NULL (sPSActionDescriptor);
	REQUIRE_NON_NULL (fDescriptor);

	ThrowIfOSErr (sPSActionDescriptor->PutFloat (fDescriptor, inKey, inValue));
	}

/******************************************************************************/

bool PIUActionDescriptor::GetBoolean (const char *inKey)
	{
	return GetBoolean (PIUActionControl::GetStringID (inKey));
	}

/******************************************************************************/

bool PIUActionDescriptor::GetBoolean (DescriptorKeyID inKey)
	{
	REQUIRE_NON_NULL (sPSActionDescriptor);
	REQUIRE_NON_NULL (fDescriptor);

	Boolean			value;

	ThrowIfOSErr (sPSActionDescriptor->GetBoolean (fDescriptor, inKey, &value));

	return (value != 0);
	}

/******************************************************************************/

void PIUActionDescriptor::PutBoolean (const char *inKey, bool inValue)
	{
	PutBoolean (PIUActionControl::GetStringID (inKey), inValue);
	}
	
/******************************************************************************/

void PIUActionDescriptor::PutBoolean (DescriptorKeyID inKey, bool inValue)
	{
	REQUIRE_NON_NULL (sPSActionDescriptor);
	REQUIRE_NON_NULL (fDescriptor);

	ThrowIfOSErr (sPSActionDescriptor->PutBoolean (fDescriptor, inKey, inValue));
	}

/******************************************************************************/

std::string PIUActionDescriptor::GetString (const char *inKey)
	{
	return GetString (PIUActionControl::GetStringID (inKey));
	}
	
/******************************************************************************/

std::string PIUActionDescriptor::GetString (DescriptorKeyID inKey)
	{
	REQUIRE_NON_NULL (sPSActionDescriptor);
	REQUIRE_NON_NULL (fDescriptor);

	std::string		value;
	char			*buffer = NULL;

	try
		{
		uint32		length;

		// Get the length
		ThrowIfOSErr (sPSActionDescriptor->GetStringLength (fDescriptor, inKey,
				&length));
			
		// Add one for the null terminator (unfortunately, this is necessary)
		length++;

		// Get the string into a temporary buffer
		buffer = new char[length];
		if (buffer == NULL)
			ThrowOSErr (memFullErr);
		ThrowIfOSErr (sPSActionDescriptor->GetString (fDescriptor, inKey,
				buffer, length));

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

void PIUActionDescriptor::PutString (const char *inKey, const std::string &inValue)
	{
	PutString (PIUActionControl::GetStringID (inKey), inValue.c_str ());
	}
	
/******************************************************************************/

void PIUActionDescriptor::PutString (DescriptorKeyID inKey, const std::string &inValue)
	{
	PutString (inKey, inValue.c_str ());
	}

/******************************************************************************/

void PIUActionDescriptor::PutString (const char *inKey, const char *inValue)
	{
	PutString (PIUActionControl::GetStringID (inKey), inValue);
	}
	
/******************************************************************************/

void PIUActionDescriptor::PutString (DescriptorKeyID inKey, const char *inValue)
	{
	REQUIRE_NON_NULL_PARAMETER (inValue);
	REQUIRE_NON_NULL (sPSActionDescriptor);
	REQUIRE_NON_NULL (fDescriptor);
	
	ThrowIfOSErr (sPSActionDescriptor->PutString (fDescriptor, inKey, 
			const_cast<char *> (inValue)));
	}
	
/******************************************************************************/

ASZString PIUActionDescriptor::GetZString (const char *inKey)
	{
	return GetZString (PIUActionControl::GetStringID (inKey));
	}

/******************************************************************************/

ASZString PIUActionDescriptor::GetZString (DescriptorKeyID inKey)
	{
	REQUIRE_NON_NULL (sPSActionDescriptor);
	REQUIRE_NON_NULL (fDescriptor);
	
	ASZString		value;

	ThrowIfOSErr (sPSActionDescriptor->GetZString (fDescriptor, inKey, &value));

	return value;
	}

/******************************************************************************/

void PIUActionDescriptor::PutZString (const char *inKey, ASZString inValue)
	{
	PutZString (PIUActionControl::GetStringID (inKey), inValue);
	}

/******************************************************************************/

void PIUActionDescriptor::PutZString (DescriptorKeyID inKey, ASZString inValue)
	{
	REQUIRE_NON_NULL (sPSActionDescriptor);
	REQUIRE_NON_NULL (fDescriptor);
	
	ThrowIfOSErr (sPSActionDescriptor->PutZString (fDescriptor, inKey, inValue));
	}

/******************************************************************************/

PIActionDescriptor PIUActionDescriptor::GetObject (const char *inKey)
	{
	return GetObject (PIUActionControl::GetStringID (inKey));
	}

/******************************************************************************/

PIActionDescriptor PIUActionDescriptor::GetObject (DescriptorKeyID inKey)
	{
	REQUIRE_NON_NULL (sPSActionDescriptor);
	REQUIRE_NON_NULL (fDescriptor);

	DescriptorClassID	type;
	PIActionDescriptor	value;

	ThrowIfOSErr (sPSActionDescriptor->GetObject (fDescriptor, inKey, &type, &value));
	
	return value;
	}

/******************************************************************************/

void PIUActionDescriptor::PutObject (const char *inKey, const char *inType, 
		PIActionDescriptor inValue)
	{
	PutObject (PIUActionControl::GetStringID (inKey), 
			PIUActionControl::GetStringID (inType), inValue);
	}
	
/******************************************************************************/

void PIUActionDescriptor::PutObject (DescriptorKeyID inKey, DescriptorClassID inType, 
		PIActionDescriptor inValue)
	{
	REQUIRE_NON_NULL_PARAMETER (inValue);
	REQUIRE_NON_NULL (sPSActionDescriptor);
	REQUIRE_NON_NULL (fDescriptor);

	ThrowIfOSErr (sPSActionDescriptor->PutObject (fDescriptor, inKey, inType, inValue));
	}

/******************************************************************************/

PIActionList PIUActionDescriptor::GetList (const char *inKey)
	{
	return GetList (PIUActionControl::GetStringID (inKey));
	}
	
/******************************************************************************/

PIActionList PIUActionDescriptor::GetList (DescriptorKeyID inKey)
	{
	REQUIRE_NON_NULL (sPSActionDescriptor);
	REQUIRE_NON_NULL (fDescriptor);
	
	PIActionList	value;

	ThrowIfOSErr (sPSActionDescriptor->GetList (fDescriptor, inKey, &value));
	
	return value;
	}

/******************************************************************************/

void PIUActionDescriptor::PutList (const char *inKey, PIActionList inValue)
	{
	PutList (PIUActionControl::GetStringID (inKey), inValue);
	}
	
/******************************************************************************/

void PIUActionDescriptor::PutList (DescriptorKeyID inKey, PIActionList inValue)
	{
	REQUIRE_NON_NULL_PARAMETER (inValue);
	REQUIRE_NON_NULL (sPSActionDescriptor);
	REQUIRE_NON_NULL (fDescriptor);

	ThrowIfOSErr (sPSActionDescriptor->PutList (fDescriptor, inKey, inValue));
	}

/******************************************************************************/

int32 PIUActionDescriptor::GetDataLength (const char *inKey)
	{
	return GetDataLength (PIUActionControl::GetStringID (inKey));
	}
	
/******************************************************************************/

int32 PIUActionDescriptor::GetDataLength (DescriptorKeyID inKey)
	{
	REQUIRE_NON_NULL (sPSActionDescriptor);
	REQUIRE_NON_NULL (fDescriptor);
	
	int32		length;

	ThrowIfOSErr (sPSActionDescriptor->GetDataLength (fDescriptor, inKey, &length));
	
	return length;
	}

/******************************************************************************/

void PIUActionDescriptor::GetData (const char *inKey, void *outValue)
	{
	GetData (PIUActionControl::GetStringID (inKey), outValue);
	}
	
/******************************************************************************/

void PIUActionDescriptor::GetData (DescriptorKeyID inKey, void *outValue)
	{
	REQUIRE_NON_NULL_PARAMETER (outValue);
	REQUIRE_NON_NULL (sPSActionDescriptor);
	REQUIRE_NON_NULL (fDescriptor);
	
	ThrowIfOSErr (sPSActionDescriptor->GetData (fDescriptor, inKey, outValue));
	}

/******************************************************************************/

void PIUActionDescriptor::PutData (const char *inKey, void *inValue, int32 inLength)
	{
	PutData (PIUActionControl::GetStringID (inKey), inValue, inLength);
	}
	
/******************************************************************************/

void PIUActionDescriptor::PutData (DescriptorKeyID inKey, void *inValue, int32 inLength)
	{
	REQUIRE_NON_NULL_PARAMETER (inValue);
	REQUIRE_NON_NULL (sPSActionDescriptor);
	REQUIRE_NON_NULL (fDescriptor);

	ThrowIfOSErr (sPSActionDescriptor->PutData (fDescriptor, inKey, inLength, inValue));
	}
