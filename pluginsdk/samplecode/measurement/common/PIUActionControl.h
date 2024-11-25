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

#ifndef __PIUActionControl__
#define __PIUActionControl__

#include <string>

#include "PIActions.h"

/**
 * Static method wrappers to translate a string ID to a DescriptorTypeID.  Sane
 * parameter validation and error handling through exceptions.  See 
 * PIUExceptions.h for more information about exceptions.
 */
class PIUActionControl
	{
	public:

		static DescriptorTypeID GetStringID (const std::string &inStringID);
		static DescriptorTypeID GetStringID (const char *inStringID);

	};

#endif /* __PIUActionControl__ */
