// ADOBE SYSTEMS INCORPORATED
// Copyright  1993 - 2002 Adobe Systems Incorporated
// All Rights Reserved
//
// NOTICE:  Adobe permits you to use, modify, and distribute this 
// file in accordance with the terms of the Adobe license agreement
// accompanying it.  If you have received this file from a source
// other than Adobe, then your use, modification, or distribution
// of it requires the prior written permission of Adobe.
//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
//
//	File:
//		GetInfoFromPhotoshop.h
//
//	Description:
//		Get all the information you can from the Photoshop actions system. This
//		plug in uses the PIUActionUtils routines to dump this information to a 
//		log file called "Getter.log".
//	
//		NOTE: The "Getter.log" file makes it look like you can set the
//		information. This is not true. I am only using the PIUActionUtils routine
//		so I don't have to rewrite the same code into a different format.
//
//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
//	Includes
//-------------------------------------------------------------------------------
#ifndef __GetInfoFromPhotoshop_H__
#define __GetInfoFromPhotoshop_H__

#include "SPHost.h"
#include "PIUActionUtils.h"
#include "PIActionsPlugIn.h"
#include "PIUGet.h"

// routines available
SPErr GetApplicationInfo(char* logfilename);
SPErr GetHistoryInfo(char* logfilename);
SPErr GetLayerInfo(char* logfilename);
SPErr GetPathInfo(char* logfilename);
SPErr GetActionInfo(char* logfilename);
SPErr GetDocumentInfo(char* logfilename);
SPErr GetChannelInfo(char* logfilename);
SPErr GetBackgroundInfo(char* logfilename);

#endif
// end GetInfoFromPhotoshop.h