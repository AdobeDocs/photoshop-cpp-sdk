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
//		Listener.cpp
// 
//	Description:
//		Persistent automation plug-in that registers to be notified
//		of events.
//
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
//	Includes
//-------------------------------------------------------------------------------

#include "Listener.h"
#include "PITerminology.h"
#include "PIStringTerminology.h"
#include "Logger.h"

#ifndef MAX_PATH
#define MAX_PATH	256
#endif

// global suite pointers for our plug-in to use
SPBasicSuite			*sSPBasic = NULL; // this is part of the AutoPluginMain message
SPPluginRef				gPlugInRef = NULL;

//-------------------------------------------------------------------------------
//	Globals -- Define global variables for plug-in scope.
//-------------------------------------------------------------------------------
std::string * gActionName = NULL;
std::string * gActionSet = NULL;
DescriptorEventID gEventID = 0;

static bool notifierOn = false;

Listener_t* gListenerList = NULL;

//	--------------------------------------------------------------------------------

// There are a lot of events in Listener.log output that have nothing to do
// with the ability to play them back. They are mostly for other features in need of
// information. Many third party developers are now confused as to what they see
// in the output of ScriptListenerJS.log. They should of been implemented as hidden
// events that folks had to specifically request vs in the 'all' mix.
// I think eventNotify was for this purpose.

// all pointers so we can control creation

// strings we want removed from our default ignore list
vector<std::string> * gSubtractStrings = NULL;

// strings we want added to our default ignore list
vector<std::string> * gAddStrings = NULL;

// runtime IDs of all things to ignore, including strings and IDs from lists above
vector<DescriptorEventID> * gRunIDs = NULL;



//-------------------------------------------------------------------------------
//	Prototypes.
//-------------------------------------------------------------------------------

// Initialize our parameters:
static void Initialize (void);

// Our listening notifier:
static void Listener
	(
	/* IN */	DescriptorEventID		event,		// Incoming event.
	/* IN */	PIActionDescriptor		descriptor,	// Event descriptor.
	/* IN */	PIDialogRecordOptions	options,	// Outgoing dialog options.
	/* IN */	void*					data		// Your user data. 
);

// Our Event Dump notifier:
static void EventDumper
	(
	/* IN */	DescriptorEventID		event,		// Incoming event.
	/* IN */	PIActionDescriptor		descriptor,	// Event descriptor.
	/* IN */	PIDialogRecordOptions	options,	// Outgoing dialog options.
	/* IN */	void*					data		// Your user data. 
);

// Startup call (used to install notifier):
static SPErr Startup (void);

// Shutdown call (used to remove notifier):
static SPErr Shutdown (void);

// Register to receive a notification:
static SPErr DoRegister (void);

// Track listeners in a list:
static Listener_t* AddListenerToList (void);

// Tell Photoshop to update menus and redraw
static void AppRefresh (void);

// See if the event is in our ignore list
static bool InIgnoreList (DescriptorEventID event);

// Send a message to a UXP plugin
static void SendToUXP (void);

// In case the UXP plugin sends me a message
static void MyUXPNotifier(PIActionDescriptor descriptor);


//-------------------------------------------------------------------------------
//
//	AutoPluginMain
//
//	All calls to the plug-in module come through this routine.
//	It must be placed first in the resource.  To achieve this,
//	most development systems require this be the first routine
//	in the source.
//
//	The entrypoint will be "pascal void" for Macintosh,
//	"void" for Windows.
//
//-------------------------------------------------------------------------------
DLLExport SPAPI SPErr AutoPluginMain(
	const char* caller,	// who is calling
	const char* selector, // what do they want
	void* message	// what is the message
)
{
	SPErr error = kSPNoError;

	try 
	{
		//all messages contain a SPMessageData*
		SPMessageData* basicMessage;
		basicMessage = (SPMessageData*) message;
		sSPBasic = basicMessage->basic;
		gPlugInRef = basicMessage->self;

		// check for SP interface callers
		if (sSPBasic->IsEqual(caller, kSPInterfaceCaller))
		{
			// pop an about box
			if (sSPBasic->IsEqual(selector, kSPInterfaceAboutSelector))
			{
				DoAbout(gPlugInRef, kListenerAboutID);
			}
			// time to start our plug in
			if (sSPBasic->IsEqual(selector, kSPInterfaceStartupSelector))
				error = Startup();

			// time to end our plug in
			if (sSPBasic->IsEqual(selector, kSPInterfaceShutdownSelector))
				error = Shutdown();
		}
		
		// Photoshop is calling us
		if (sSPBasic->IsEqual(caller, kPSPhotoshopCaller))
		{
			// the one and only message 
			if (sSPBasic->IsEqual(selector, kPSDoIt))
			{
				//now that we know more we can cast the message
				//to a PSActionsPlugInMessage* and get the action parameters
				PSActionsPlugInMessage* actionsMessage;
				actionsMessage = (PSActionsPlugInMessage*) message;
				PIActionParameters* actionParams;
				actionParams = actionsMessage->actionParameters;
				error = Execute(actionParams);
			}
		}
	}

	catch(...)
	{
		if (error == 0)
			error = kSPBadParameterError;
	}

	return error;
}

//-------------------------------------------------------------------------------
//
//	Execute
//
//	Main routine.
//
//-------------------------------------------------------------------------------
SPErr Execute(PIActionParameters* actionParams)
{
	SPErr error = kSPNoError;
	
	// Default parameters for all globals.
	Initialize(); 
	
	// Override globals with new descriptor info.
	ReadScriptParams(actionParams); 
	
	// Determine if we need to pop our dialog:
	PIDialogPlayOptions playInfo = actionParams->playInfo;	
	
	// Others = plugInDialogDontDisplay / plugInDialogSilent
	if (playInfo == plugInDialogDisplay)
	{	
		// Go ahead and display a dialog:
		error = DoUI();
		if (error != kSPNoError)
		{
			// delete the globals created by the UI
			// I should remove this global crap
			if (gActionSet != NULL)
			{
				delete gActionSet;
				gActionSet = NULL;
			}
			if (gActionName != NULL)
			{
				delete gActionName;
				gActionName = NULL;
			}
		}
	}

	// register for this event
	if (error == kSPNoError)
		error = DoRegister();

	// write our script parameters to the scripting system
	if (error == kSPNoError)
		WriteScriptParams(actionParams);
    
    // refresh the app so our menu item is not disabled
    AppRefresh();

	// Let the UXP plugin know that I am alive
	SendToUXP();
    
	// Save our add/subtract list for next time we launch
    WritePreferences();
    
	return error;
}	

//-------------------------------------------------------------------------------
//
//	Initialize
//
//	Reset parameters.
//
//-------------------------------------------------------------------------------
static void Initialize (void)
{
	// Listeners are only loaded (initialized) once, so don't re-initialize
	// parameters here each time we're called but instead favor file-scope
	// initialization.
}

//-------------------------------------------------------------------------------
//
//	Listener
//
//	This is the routine that gets notified for any events that we've
//	registered for.
//
//	You can only have one listening proc registered per event, but you
//	can have the same listening proc for multiple events.
//
//	Use "eventAll" to get notification of all actions events.
//
//	MACINTOSH WARNING: Due to resource management problems, when this
//	routine is called, **YOUR RESOURCE FORK IS CLOSED**.  If you need
//	resources from your fork, load them during startup or call your
//	own plug-in using a Play() command.
//
//	The notifier is intended to be used to make intelligent decisions
//	and dispatch to other things such as external apps.  Keep its
//	execution time down, especially if you're catching "eventAll" or
//	the user will notice an overall slowdown and bang their machine
//	thinking something's wrong.
//	
//-------------------------------------------------------------------------------
static void Listener
	(
	/* IN */	DescriptorEventID		event,		// Incoming event.
	/* IN */	PIActionDescriptor		/* descriptor */,	// Event descriptor.
	/* IN */	PIDialogRecordOptions	/* options */,		// Outgoing dialog options.
	/* IN */	void*					data				// Your user data. 
)
{
    // do not throw back into Photoshop on callbacks
    try
    {
        // This listener dispatches an action, stored in data:
        if (data != NULL)
        {
            Listener_t* listener = (Listener_t*)data;

            if (listener != NULL)
            {
                SPErr error = PIUActionsPlayByName((char*)(listener->actionSet.c_str()),
				                           	       (char*)(listener->actionName.c_str()));
                if (error)
                {
                    (void)sPSActionControl->RemoveNotify(gPlugInRef, event);
                }
            }
            else
            {
                (void)sPSActionControl->RemoveNotify(gPlugInRef, event);
            }
        }
        else
        {
            (void)sPSActionControl->RemoveNotify(gPlugInRef, event);
        }
	}
    
    catch (...)
    {
        (void)sPSActionControl->RemoveNotify(gPlugInRef, event);
    }

}

//-------------------------------------------------------------------------------
//
//	EventDumper
//
//	This is the routine that gets notified for all events from Photoshop.
//
//	You can only have one listening proc registered per event, but you
//	can have the same listening proc for multiple events.
//
//	Use "eventAll" to get notification of all actions events.   
//
//	MACINTOSH WARNING: Due to resource management problems, when this
//	routine is called, **YOUR RESOURCE FORK IS CLOSED**.  If you need
//	resources from your fork, load them during startup or call your
//	own plug-in using a Play() command.
//
//	The notifier is intended to show an automation plug in programmer
//	how to put event descriptors together. The DumpDescriptor routine
//	takes the event that just happened and walks through the descriptor
//	dumping everything that it finds.
//
//	NOTE: THIS IS FOR THE DEBUG BUILD OF THIS PLUG IN ONLY. THE DEBUG
//	LIBRARY IS RATHER LARGE AND YOU PROBABLY DON'T WANT IT IN YOUR
//	SHIPPING PLUG IN.  
//	
//-------------------------------------------------------------------------------
static void EventDumper
	(
	/* IN */	DescriptorEventID		 event ,		// Incoming event.
	/* IN */	PIActionDescriptor		 descriptor ,	// Event descriptor.
	/* IN */	PIDialogRecordOptions	 /*options*/,		// Outgoing dialog options.
	/* IN */	void*					/*data*/			// Your user data. 
)
{
 
    // do not throw back into Photoshop on callbacks
    try
    {
        static char logfilename[MAX_PATH];
        static TryState gotFullPath = kUnknown;

		if (InIgnoreList(event))
			return;
		
        if (kUnknown == gotFullPath)
        {
            gotFullPath = kFalse;

            if (GetFullPathToDesktop(logfilename, MAX_PATH))
            {
                logfilename[0] = '\0';
                return;
            }

            if (PIstrlcat(logfilename, "Listener.log", MAX_PATH-1) >= MAX_PATH-1)
                return;

            gotFullPath = kTrue;

        }

        if (kTrue == gotFullPath)
            PIUDumpDescriptor(event, descriptor, logfilename);
    }
    catch(...)
    {
        // ignore error but try and
        // remove myself from this event as I am having some trouble it seems
        (void)sPSActionControl->RemoveNotify(gPlugInRef, eventAll);
    }

}

//-------------------------------------------------------------------------------
//
//	Startup
//   
//	On the startup call we register our notifier to catch any actions
//	that we're interested in.
//
//	The startup sequence is based on your PiPL.  If you have
//
//		Persistent {},
//
//	Your plug-in will stay resident until Photoshop is quit.  If you have
//
//		Messages { startupRequired, doesNotPurgeCache, shutdownRequired, acceptProperty },
//
//	Your plug-in will receive startup and shutdown messages.  You can also
//	create a combination of the startup/shutdown calls by putting "no" in front
//	of the unnecessary call, such as "noStartupRequired".  purgeCache and
//	acceptProperty are required parameters that are not used in Photoshop 5.0
//	but may be in the future.
//
//	You must have a Persistent property for the Messages property to be
//	respected.
//
//	DO NOT do long processes at Startup.  The user will think something
//	happened during loading and bonk their machine, getting irate.
//
//	The normal calling process for a persistent plug-in with messages:
//		Reload
//		Startup
//		Execute (if user selects you or you are played)
//		Shutdown
//		Unload
//
//	GUARANTEES:
//		If you return from Reload with no error,
//			you are guaranteed an Unload call.
//		If you return from Startup with no error,
//			you are guaranteed a Shutdown call (if you asked for one.)
//
//-------------------------------------------------------------------------------
static SPErr Startup (void)
{
	SPErr error = kSPNoError;
	
	// Lets register our actions notification routine.  NOTE: It is our
	// responsibility to unregister this when we're done with it.  If we
	// don't and this plug-in gets unloaded, its a ticking time bomb --
	// whatever event we've registered for will eventually happen and the
	// host will call this routine which won't be there and it'll all go
	// boom.
	if (!notifierOn)
	{
		error = sPSActionControl->AddNotify(gPlugInRef,
											eventAll, // event you want to listen for
											EventDumper, // Proc for listening routine.
											NULL); // User data.
											
		if (!error)
			notifierOn = true;
	}
	
	// these are strings someone actually wants us to remove from the list
	gSubtractStrings = new vector<std::string>;

	// these are strings someone actually wants us to add to the list
	gAddStrings = new vector<std::string>;

	// these are our default strings that we do not want in the log file
	gRunIDs = new vector<DescriptorEventID>;
	
	DescriptorEventID eventID = 0;
	sPSActionControl->StringIDToTypeID(knotifyStr, &eventID);
	gRunIDs->push_back(eventID);
	sPSActionControl->StringIDToTypeID(kMRUFileListChangedStr, &eventID);
	gRunIDs->push_back(eventID);
	sPSActionControl->StringIDToTypeID(kmodalStateChanged, &eventID);
	gRunIDs->push_back(eventID);
	sPSActionControl->StringIDToTypeID(khomeScreenVisibilityChangedStr, &eventID);
	gRunIDs->push_back(eventID);
	sPSActionControl->StringIDToTypeID(knglProfileChangedStr, &eventID);
	gRunIDs->push_back(eventID);
	sPSActionControl->StringIDToTypeID(klayersFilteredStr, &eventID);
	gRunIDs->push_back(eventID);
	sPSActionControl->StringIDToTypeID(kfontListChangedStr, &eventID);
	gRunIDs->push_back(eventID);
	sPSActionControl->StringIDToTypeID(kinvokeCommandStr, &eventID);
	gRunIDs->push_back(eventID);
	sPSActionControl->StringIDToTypeID(ktoolModalStateChangedStr, &eventID);
	gRunIDs->push_back(eventID);
	sPSActionControl->StringIDToTypeID(khistoryStateChangedStr, &eventID);
	gRunIDs->push_back(eventID);
	sPSActionControl->StringIDToTypeID(kfeatureInfoStr, &eventID);
	gRunIDs->push_back(eventID);
	sPSActionControl->StringIDToTypeID(kpluginsLoadedNamesStr, &eventID);
	gRunIDs->push_back(eventID);
	sPSActionControl->StringIDToTypeID(kuiInfoStr, &eventID);
	gRunIDs->push_back(eventID);
	sPSActionControl->StringIDToTypeID(kowlActionStr, &eventID);
	gRunIDs->push_back(eventID);
	sPSActionControl->StringIDToTypeID(knewDocumentStr, &eventID);
	gRunIDs->push_back(eventID);
	sPSActionControl->StringIDToTypeID(khostFocusChangedStr, &eventID);
	gRunIDs->push_back(eventID);
	
	sUXP->AddUXPMessageListener(gPlugInRef, MyUXPNotifier);

	ReadPreferences();

	return error;
}

//-------------------------------------------------------------------------------
//
//	Shutdown
//
//	It's our responsibility to unregister any notifiers or memory we allocated
//	at startup.  So we iterate through the listener list, both removing
//	any notifiers and cleaning out memory.
//
//-------------------------------------------------------------------------------
static SPErr Shutdown (void)
{
	SPErr error = kSPNoError;
	
	Listener_t* thisList = gListenerList;
	
	while (thisList != NULL)
	{
		gEventID = thisList->eventID;
	
		error = sPSActionControl->RemoveNotify(gPlugInRef,
											   gEventID);

		gEventID = 0;
		thisList->eventID = 0;
		
		Listener_t* nextList = thisList->next;
		delete thisList;
		
		thisList = nextList;
	}		

	if (notifierOn)
			error = sPSActionControl->RemoveNotify(
				gPlugInRef,
				eventAll);				// Event we registered.

	// clean up after ourselves
	PIUSuitesRelease();

	return error;
}
	
//-------------------------------------------------------------------------------
//
//	DoRegister
//
//	This registers to receive notifications of an event.
//
//-------------------------------------------------------------------------------
static SPErr DoRegister (void)
{
	SPErr error = kSPNoError;
	
	// Once you run this, gActionName, gActionSet, gEventID are all
	// set to NULL (so they aren't accidentally deleted by someone)
	// and you must use newListener->actionName, newListener->actionSet,
	// and newListener->eventID to get at this info:
	if (gActionName && gActionSet && gEventID)
	{
		Listener_t* newListener = AddListenerToList();

		error = sPSActionControl->AddNotifyLevel1(gPlugInRef,
												  newListener->eventID, // event to listen for
												  Listener, // Proc for listening routine.
												  (void*)newListener); // User data.
	}
	
	return error;
}

//-------------------------------------------------------------------------------
//
//	AppRefresh
//
//	Tell Photoshop to invalidate everything and redraw. Time consuming!
//
//-------------------------------------------------------------------------------
static void AppRefresh (void)
{
	PIActionDescriptor desc = NULL;
    PIActionDescriptor result = NULL;
    OSErr error = sPSActionDescriptor->Make(&desc);
    if (!error)
        error = sPSActionDescriptor->PutEnumerated(desc, keyState, typeState, enumRedrawComplete);
    if (!error)
        error = sPSActionControl->Play(&result, eventWait, desc, plugInDialogSilent);
    if (desc)
        sPSActionDescriptor->Free(desc);
    if (result)
        sPSActionDescriptor->Free(result);
}

//-------------------------------------------------------------------------------
//
//	FindListenerListEnd
//
//	Find the end of the Listener list
//
//-------------------------------------------------------------------------------
Listener_t* FindListenerListEnd (void)
{
	Listener_t* thisList = gListenerList;
	
	while (thisList != NULL && thisList->next != NULL)
		thisList = thisList->next;

	return thisList;
}
	
//-------------------------------------------------------------------------------
//
//	AddListenerToList
//
//	Add an event + action name + action set to the end of the list
//
//-------------------------------------------------------------------------------
static Listener_t* AddListenerToList (void)
{
	Listener_t* listEnd = FindListenerListEnd();
	
	Listener_t* newListener = new Listener_t;
	
	newListener->eventID = gEventID;	
	gEventID = 0;
		
	// Same for action name:
	newListener->actionName = *gActionName;
	gActionName = NULL;
	
	newListener->actionSet = *gActionSet;
	gActionSet = NULL;
		
	newListener->next = NULL;
	
	if (listEnd != NULL)
		listEnd->next = newListener;
	else
		gListenerList = newListener;
	
	return newListener;
}

//-------------------------------------------------------------------------------
//
//	InIgnoreList
//
//	See if the event is in our ignore list.
//
//-------------------------------------------------------------------------------
static bool InIgnoreList (DescriptorEventID event)
{
	vector<DescriptorEventID>::iterator it = find(gRunIDs->begin(), gRunIDs->end(), event);
	if (it != gRunIDs->end())
		return true;
	return false;
}

//-------------------------------------------------------------------------------
//
//	SubtractFromList
//
//	Find the string and subtract it from the ignore list. We want to see it.
//  Add it to the subract list so we can remember it the next time we are launched.
//
//-------------------------------------------------------------------------------
void SubtractFromList (const std::string &s)
{
	DescriptorEventID eventID = 0;
	sPSActionControl->StringIDToTypeID(s.c_str(), &eventID);
	vector<DescriptorEventID>::iterator it =
		find(gRunIDs->begin(), gRunIDs->end(), eventID);
	if (it != gRunIDs->end())
	{
		gRunIDs->erase(it);
		gSubtractStrings->push_back(s);
	}
}

//-------------------------------------------------------------------------------
//
//	SubtractFromList
//
//	Find the strings in the list and subtract it from the ignore list.
//  We want to see it.
//  Add it to the subract list so we can remember it the next time we are launched.
//
//-------------------------------------------------------------------------------
void SubtractFromList(PIActionList subList)
{
	uint32 listCount = 0;
	OSErr error = sPSActionList->GetCount(subList, &listCount);
	for (uint32 i = 0; i < listCount; i++)
	{
		uint32 stringLength = 0;
		error = sPSActionList->GetStringLength(subList, i, &stringLength);
		if (error == kSPNoError && stringLength)
		{
			std::vector<char> vc(stringLength + 1);
			error = sPSActionList->GetString(subList, i, &vc[0], stringLength + 1);
			if (error == kSPNoError)
			{
				std::string s;
				s.assign(vc.begin(), vc.begin() + stringLength);
				SubtractFromList(s);
			}
		}
	}
}

//-------------------------------------------------------------------------------
//
//	AddToList
//
//	Add this event to the list to ignore.
//
//-------------------------------------------------------------------------------
void AddToList(const std::string &s)
{
	DescriptorEventID eventID = 0;
	sPSActionControl->StringIDToTypeID(s.c_str(), &eventID);
	vector<DescriptorEventID>::iterator it = find(gRunIDs->begin(), gRunIDs->end(), eventID);
	if (it == gRunIDs->end())
	{
		gRunIDs->push_back(eventID);
		gAddStrings->push_back(s);
	}
}

//-------------------------------------------------------------------------------
//
//	AddToList
//
//	Add the strings in the list to the ignore list.
//  We do not want to see it.
//  Add it to the add list so we can remember it the next time we are launched.
//
//-------------------------------------------------------------------------------
void AddToList(PIActionList subList)
{
	uint32 listCount = 0;
	SPErr error = sPSActionList->GetCount(subList, &listCount);
	for ( uint32 i = 0; i < listCount; i++ )
	{
		uint32 stringLength = 0;
		error = sPSActionList->GetStringLength(subList, i, &stringLength);
		if (error == kSPNoError && stringLength)
		{
			std::vector<char> vc(stringLength + 1);
			error = sPSActionList->GetString(subList, i, &vc[0], stringLength + 1);
			if (error == kSPNoError)
			{
				std::string s;
				s.assign(vc.begin(), vc.begin() + stringLength);
				AddToList(s);
			}
		}
	}
}

//-------------------------------------------------------------------------------
//
//	SendToUXP
//
//	C++ plugins can communicate with UXP plugins.
//
//-------------------------------------------------------------------------------
void SendToUXP (void)
{
	Logger logFile("ListenerUXP");
	PIActionDescriptor descriptor = NULL;
	OSErr error = sPSActionDescriptor->Make(&descriptor);
	if (error == kSPNoError && descriptor)
	{
		error = sPSActionDescriptor->PutString(descriptor, keyMessage, "Hello, UXP!");
		if (error == kSPNoError)
			error = sUXP->SendUXPMessage(gPlugInRef, "AdobeSDK UXP", descriptor);
		if (error == kSPNoError)
		{
			logFile.Write("Message sent!");
		} else {
			logFile.Write("Message NOT sent: ");
			logFile.Write(error, kWriteEOL);
		}
	} else {
		logFile.Write("Something is not correct error: ");
		logFile.Write(error);
		logFile.Write(", descriptor: ");
		logFile.Write((int32)((intptr_t)descriptor), kWriteEOL);
	}
	if (descriptor)
		sPSActionDescriptor->Free(descriptor);
}

//-------------------------------------------------------------------------------
//
//	MyUXPNotifier
//
//	My UXP plugin just sent me a message. Unpack it and log it.
//
//-------------------------------------------------------------------------------
void MyUXPNotifier(PIActionDescriptor descriptor)
{
	Logger logFile("ListenerUXP");
	if (descriptor)
	{
		Boolean hasKey = 0;
		OSErr error = sPSActionDescriptor->HasKey(descriptor, keyMessage, &hasKey);
		if (error == kSPNoError && hasKey)
		{
			uint32 stringLength = 0;
			error = sPSActionDescriptor->GetStringLength(descriptor, keyMessage, &stringLength);
			if (error == kSPNoError && stringLength)
			{
				std::vector<char> vc(stringLength + 1);
				error = sPSActionDescriptor->GetString(descriptor, keyMessage, &vc[0], stringLength + 1);
				if (error == kSPNoError)
				{
					std::string s;
					s.assign(vc.begin(), vc.begin() + stringLength);
					logFile.Write("Message recieved: " + s);
				} else {
					logFile.Write("Error getting string: ");
					logFile.Write(error, kWriteEOL);
				}
			} else {
				logFile.Write("Error in string length: ");
				logFile.Write(error);
				logFile.Write(", length: ");
				logFile.Write((int32)stringLength, kWriteEOL);
			}
		} else {
			logFile.Write("Error in hasKey: ");
			logFile.Write(error);
			logFile.Write(", hasKey: ");
			logFile.Write(hasKey, kWriteEOL);
		}
	} else {
		logFile.Write("No descriptor!", kWriteEOL);
	}
	// one message is enough
	sUXP->RemoveUXPMessageListener(gPlugInRef);
}

// end Listener.cpp
