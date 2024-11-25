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

#include "PIUActionControl.h"

#include "PIUExceptions.h"
#include "PIUSuites.h"

//AutoSuite<PSBasicActionControlProcs>	sPSBasicActionControl (
//												kPSBasicActionControlSuite, 
//												kPSBasicActionControlSuiteVersion);

/******************************************************************************/

DescriptorTypeID PIUActionControl::GetStringID (const std::string &inStringID)
	{
	return GetStringID (inStringID.c_str ());
	}
	
/******************************************************************************/

DescriptorTypeID PIUActionControl::GetStringID (const char *inStringID)
	{
	REQUIRE_NON_NULL_PARAMETER (inStringID);
	REQUIRE_NON_NULL (sPSBasicActionControl);
	
	DescriptorTypeID	typeID;
	
	ThrowIfOSErr (sPSBasicActionControl->StringIDToTypeID ((char *)inStringID, &typeID));
		
	return typeID;
	}
