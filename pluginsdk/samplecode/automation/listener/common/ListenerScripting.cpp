// ADOBE SYSTEMS INCORPORATED
// Copyright  1993 - 2002 Adobe Systems Incorporated
// All Rights Reserved
//
// NOTICE:  Adobe permits you to use, modify, and distribute this 
// file in accordance with the terms of the Adobe license agreement
// accompanying it.  If you have received this file from a source
// other than Adobe, then your use, modification, or distribution
// of it requires the prior written permission of Adobe.
//-------------------------------------------------------------------
//-------------------------------------------------------------------------------
//
//	File:
//		ListenerScripting.cpp
//
//	Description:
//		Persistent plug-in scripting source.
//
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
//	Includes
//-------------------------------------------------------------------------------

#include "Listener.h"
#include "PIStringTerminology.h"

//-------------------------------------------------------------------------------
//	Prototypes.
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
//
//	ReadScriptParams
//
//	Checks the parameters against scripting-returned parameters, if any, and
//	updates the globals to match ones provided by the scripting system.
//  TODO: Clean up error handling. I can now get two types of directives:
//        a. name/set/event for adding to my list
//        b. subtract/add
//        It all works but not very clean, errors over written, etc.
//
//-------------------------------------------------------------------------------

SPErr ReadScriptParams( PIActionParameters* actionParams )
{
	SPErr error = kSPNoError;
	
	// We should put this whole list of listeners, but instead we'll put just
	// the last listener from the list:
	
	PIActionDescriptor descriptor = actionParams->descriptor;
				
	// If we got a valid descriptor, grab our key out of it:
	if ( descriptor != NULL )
	{
		uint32 stringLength = 0;
		error = sPSActionDescriptor->GetStringLength( descriptor, 
													  keyPIActionSet, 
													  &stringLength );
		if ( error == kSPNoError )
			gActionSet = new string;
		
		if ( gActionSet == NULL )
			error = kSPOutOfMemoryError;

		vector<char> vc( stringLength + 1 );
		
		if ( error == kSPNoError )
			error = sPSActionDescriptor->GetString( descriptor, 
													keyPIActionSet, 
													&vc[0],
													stringLength + 1 );
		
		if ( error == kSPNoError )
			gActionSet->assign( vc.begin(), vc.begin() + stringLength );

		stringLength = 0;

		if ( error == kSPNoError )
			error = sPSActionDescriptor->GetStringLength( descriptor, 
														  keyPIAction, 
														  &stringLength );
		if ( error == kSPNoError )
			gActionName = new string;

		if ( gActionName == NULL )
			error = kSPOutOfMemoryError;
		
		if ( error == kSPNoError )
		{
			vc.clear();
			vc.resize( stringLength + 1 );
			error = sPSActionDescriptor->GetString( descriptor, 
													keyPIAction, 
													&vc[0],
													stringLength + 1 );
		}
		if ( error == kSPNoError )
			gActionName->assign( vc.begin(), vc.begin() + stringLength );

		int32 t = 0;
		error = sPSActionDescriptor->GetInteger( descriptor,
												 keyPIActionID,
												 &t );

		// this was added later and can be missing
		if ( error == kSPNoError )
			gEventID = t;
			
		DescriptorTypeID keyID = 0;
		error = sPSActionControl->StringIDToTypeID( ksubtractStr, &keyID );
		Boolean hasKey = 0;
		if ( error == kSPNoError )
			error = sPSActionDescriptor->HasKey( descriptor,
												 keyID,
												 &hasKey );
		if ( error == kSPNoError && hasKey )
		{
			stringLength = 0;
			error = sPSActionDescriptor->GetStringLength( descriptor,
														  keyID,
														  &stringLength );
			if ( error == kSPNoError && stringLength )
			{
				vc.clear();
				vc.resize( stringLength + 1 );
				error = sPSActionDescriptor->GetString( descriptor,
														keyID,
														&vc[0],
														stringLength + 1 );
				if ( error == kSPNoError )
				{
					std::string s;
					s.assign( vc.begin(), vc.begin() + stringLength );
					SubtractFromList( s );
				}
			 }
		}

		keyID = 0;
		error = sPSActionControl->StringIDToTypeID( kaddStr, &keyID );
		hasKey = 0;
		if ( error == kSPNoError )
			error = sPSActionDescriptor->HasKey( descriptor,
												 keyID,
												 &hasKey );
		if ( error == kSPNoError && hasKey )
		{
			stringLength = 0;
			error = sPSActionDescriptor->GetStringLength( descriptor,
														  keyID,
														  &stringLength );
			if ( error == kSPNoError && stringLength )
			{
				vc.clear();
				vc.resize( stringLength + 1 );
				error = sPSActionDescriptor->GetString( descriptor,
				                                        keyID,
				                                        &vc[0],
				                                        stringLength + 1 );
				if ( error == kSPNoError )
				{
					std::string s;
					s.assign( vc.begin(), vc.begin() + stringLength );
					AddToList( s );
				}
			}
		}

	}

	return error;
	
} // end ReadScriptParams
		
//-------------------------------------------------------------------------------
//
//	WriteScriptParams
//
//	Takes our parameters from our global variables and writes them out to the
//	scripting system, which hands it all back to the host.
//
//-------------------------------------------------------------------------------

SPErr WriteScriptParams( PIActionParameters* actionParams )
{
	SPErr error = kSPNoError; // we'll return any error with this

	Listener_t* listener = FindListenerListEnd();

	if (sPSActionDescriptor != NULL && listener != NULL)
	{ // Make sure we have a valid suite before doing this.
		PIActionDescriptor	descriptor = 0;	// token to write our parameters to
		
		error = sPSActionDescriptor->Make(&descriptor);
		
		if (error == kSPNoError && descriptor != 0)
		{
			error = sPSActionDescriptor->PutString( descriptor,
													keyPIActionSet,
													(char*)(listener->actionSet.c_str()) );
				
			if (error == kSPNoError)
			{
				error = sPSActionDescriptor->PutString( descriptor,
														keyPIAction,
														(char*)(listener->actionName.c_str()) );
			}

			if (error == kSPNoError)
			{
				error = sPSActionDescriptor->PutInteger( descriptor,
														 keyPIActionID,
														 listener->eventID );
			}
				
		}
			
		// Now stuff our return descriptor for return to the host:
		if (error == kSPNoError && actionParams != NULL)
		{
			if (actionParams->descriptor)
				sPSActionDescriptor->Free( actionParams->descriptor );

			actionParams->descriptor = descriptor;
			actionParams->recordInfo = plugInDialogOptional;
		}
		else
		{ 
			if (descriptor)
				sPSActionDescriptor->Free( descriptor );
		}
	}
	
	return error;
		
} // end WriteScriptParams
	
//-------------------------------------------------------------------------------
//
//	ReadPreferences
//
//	Read our add and subtract list from Photoshop preferences.
//  TODO: Get our Listener_t list and add in what we did last time. 
//
//-------------------------------------------------------------------------------

void ReadPreferences( void )
{
	PIActionDescriptor descriptor = 0;
	sPSRegistry->Get( plugInName, &descriptor );
	
	if ( descriptor )
	{
		DescriptorTypeID keyID = 0;
		sPSActionControl->StringIDToTypeID( ksubtractStr, &keyID );
		Boolean hasKey = 0;
		sPSActionDescriptor->HasKey( descriptor, keyID, &hasKey );
		if ( hasKey )
		{
			PIActionList subList = 0;
			sPSActionDescriptor->GetList( descriptor, keyID, &subList );
			SubtractFromList( subList );
			sPSActionList->Free( subList );
		}

		keyID = 0;
		sPSActionControl->StringIDToTypeID( kaddStr, &keyID );
		hasKey = 0;
		sPSActionDescriptor->HasKey( descriptor, keyID, &hasKey );
		if ( hasKey )
		{
			PIActionList subList = 0;
			sPSActionDescriptor->GetList( descriptor, keyID, &subList );
			AddToList( subList );
			sPSActionList->Free( subList );
		}
	}
}

//-------------------------------------------------------------------------------
//
//	WritePreferences
//
//	Store our add and subtract list in Photoshop preferences.
//  TODO: Remember our Listener_t so we remember between Launches of Photoshop
//
//-------------------------------------------------------------------------------

void WritePreferences( void )
{
	PIActionDescriptor descriptor = 0;
	PIActionList subList = 0;
	PIActionList addList = 0;
	DescriptorKeyID keyID = 0;

	if ( gSubtractStrings->size() )
	{
		sPSActionDescriptor->Make( &descriptor );
		sPSActionList->Make( &subList );

		if ( descriptor && subList )
		{
			vector<std::string>::iterator it = gSubtractStrings->begin();
			while ( it != gSubtractStrings->end() )
			{
				sPSActionList->PutString( subList, it->c_str() );
				it++;
			}
			sPSActionControl->StringIDToTypeID( ksubtractStr, &keyID );
			sPSActionDescriptor->PutList( descriptor, keyID, subList );
		}
	}

	if ( gAddStrings->size() )
	{
		if ( ! descriptor )
		{
			sPSActionDescriptor->Make( &descriptor );
		}

		sPSActionList->Make( &addList );

		if ( descriptor && addList )
		{
			vector<std::string>::iterator it = gAddStrings->begin();
			while ( it != gAddStrings->end() )
			{
				sPSActionList->PutString( addList, it->c_str() );
				it++;
			}
			sPSActionControl->StringIDToTypeID( kaddStr, &keyID );
			sPSActionDescriptor->PutList( descriptor, keyID, addList );
		}
	}

	if ( descriptor )
	{
		sPSRegistry->Register( plugInName, descriptor, true );
		sPSActionDescriptor->Free( descriptor );
	}
	
	if ( subList )
	{
		sPSActionList->Free( subList );
	}
	
	if ( addList )
	{
		sPSActionList->Free( addList );
	}
}
//-------------------------------------------------------------------------------

// end ListenerScripting.cpp
