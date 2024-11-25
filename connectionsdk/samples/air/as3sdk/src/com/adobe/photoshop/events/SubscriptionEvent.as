/*
	PhotoshopSDK
	API and Architecture: Daniel Koestler (blogs.adobe.com/koestler)
	Encryption and Architecture: Renaun Erickson (renaun.com)
*/
/**************************************************************************
 ADOBE SYSTEMS INCORPORATED
 Copyright 2011 Adobe Systems Incorporated
 All Rights Reserved.
 NOTICE: Adobe permits you to use, modify, and distribute this file
 in accordance with the terms of the Adobe license agreement accompanying
 it. If you have received this file from a source other than Adobe, then
 your use, modification, or distribution of it requires the prior written
 permission of Adobe.
 **************************************************************************/

package com.adobe.photoshop.events
{
	import flash.events.Event;
	
	/**
	 *  A SubscriptionEvent is dispatched by classes such as the SubscriptionManager, and will contain useful information regarding the user events in Photoshop.
	 **/
	public class SubscriptionEvent extends Event
	{
		/**		 
		 *  Used when a Subcription has been successfully registered.
		 * 		 		 
		 *  @eventType SubscriptionEventSubscribed
		 *  @langversion 3.0
		 *  @playerversion Flash 10		 
		 **/
		public static const SUBSCRIBED:String = "SubscriptionEventSubscribed";
		
		/**		 
		 *  Dispatched when a subscription was requested, but the API is already subscribed to that type.
		 * 		 		 
		 *  @eventType SubscriptionEventAlreadySubscribed
		 *  @langversion 3.0
		 *  @playerversion Flash 10		 
		 **/
		public static const ALREADY_SUBSCRIBED:String = "SubscriptionEventAlreadySubscribed";
		
		/**		 
		 *  An unsubscription request was successful. 
		 * 		 		 
		 *  @eventType SubscriptionEventUnsubscribed
		 *  @langversion 3.0
		 *  @playerversion Flash 10		 
		 **/
		public static const UNSUBSCRIBED:String = "SubscriptionEventUnsubscribed";
		
		/**		 
		 *  A subscription event was attempted (or a check made for subscription status), and the API isn't subscribed to that type.  
		 * 		 		 
		 *  @eventType SubscriptionEventNotSubscribed
		 *  @langversion 3.0
		 *  @playerversion Flash 10		 
		 **/
		public static const NOT_SUBSCRIBED:String = "SubscriptionEventNotSubscribed";
		
		/**		 
		 *  The foreground color was changed in Photoshop
		 * 		 		 
		 *  @eventType SubscriptionEventForegroundColorChanged
		 *  @langversion 3.0
		 *  @playerversion Flash 10		 
		 **/
		public static const FOREGROUND_COLOR_CHANGED_EVENT:String = "SubscriptionEventForegroundColorChanged";
		
		/**		 
		 *  The background color was changed in Photoshop
		 * 		 		 
		 *  @eventType SubscriptionEventBackgroundColorChanged
		 *  @langversion 3.0
		 *  @playerversion Flash 10		 
		 **/
		public static const BACKGROUND_COLOR_CHANGED_EVENT:String = "SubscriptionEventBackgroundColorChanged";
		
		
		/**		 
		 *  Photoshop's color settings were changed.
		 * 		 		 
		 *  @eventType SubscriptionEvent
		 *  @langversion 3.0
		 *  @playerversion Flash 10		 
		 **/
		public static const COLOR_SETTINGS_CHANGED_EVENT:String = "SubscriptionEventColorSettingsChanged";
		
		/**		 
		 *  Photoshop's preferences were changed
		 * 		 		 
		 *  @eventType SubscriptionEvent
		 *  @langversion 3.0
		 *  @playerversion Flash 10		 
		 **/
		public static const PREFERENCES_CHANGED_EVENT:String = "SubscriptionEventPreferencesChanged";
		
		/**		 
		 *  Photoshop's keyboard shortcuts were changed.
		 * 		 		 
		 *  @eventType SubscriptionEvent
		 *  @langversion 3.0
		 *  @playerversion Flash 10		 
		 **/
		public static const KEYBOARD_SHORTCUTS_CHANGED_EVENT:String = "SubscriptionEventKeyboardShortcutsChanged";
		
		/**		 
		 *  Photoshop's screen mode (full screen, normal, etc.) was changed.
		 * 		 		 
		 *  @eventType SubscriptionEvent
		 *  @langversion 3.0
		 *  @playerversion Flash 10		 
		 **/
		public static const SCREEN_MODE_CHANGED_EVENT:String = "SubscriptionEventScreenModeChanged";
		
		/**		 
		 *  The user entered or exited Photoshop's quick mask
		 * 		 		 
		 *  @eventType SubscriptionEvent
		 *  @langversion 3.0
		 *  @playerversion Flash 10		 
		 **/
		public static const QUICK_MASK_STATE_CHANGED_EVENT:String = "SubscriptionEventQuickMaskStateChanged";
		
		/**		 
		 *  The user changed the currently selected tool in Photoshop
		 * 		 		 
		 *  @eventType SubscriptionEvent
		 *  @langversion 3.0
		 *  @playerversion Flash 10		 
		 **/
		public static const TOOL_CHANGED_EVENT:String = "SubscriptionEventToolChanged";
		
		/**		 
		 *  The "Activeview" changed in Photoshop
		 * 		 		 
		 *  @eventType SubscriptionEvent
		 *  @langversion 3.0
		 *  @playerversion Flash 10		 
		 **/
		public static const ACTIVEVIEW_CHANGED_EVENT:String = "SubscriptionEventActiveviewChanged";
		
		/**		 
		 *  A document was closed in Photoshop
		 * 		 		 
		 *  @eventType SubscriptionEvent
		 *  @langversion 3.0
		 *  @playerversion Flash 10		 
		 **/
		public static const DOCUMENT_CLOSED_EVENT:String = "SubscriptionEventDocumentClosed";
		
		/**		 
		 *  The current document was changed in Photoshop
		 * 		 		 
		 *  @eventType SubscriptionEvent
		 *  @langversion 3.0
		 *  @playerversion Flash 10		 
		 **/
		public static const CURRENTDOCUMENT_CHANGED_EVENT:String = "SubscriptionEventCurrentdocumentChanged";
		
		/**		 
		 *  A "documentview" was created in Photoshop
		 * 		 		 
		 *  @eventType SubscriptionEvent
		 *  @langversion 3.0
		 *  @playerversion Flash 10		 
		 **/
		public static const DOCUMENTVIEW_CREATED_EVENT:String = "SubscriptionEventDocumentviewCreated";
		
		/**		 
		 *  A document was changed in Photoshop
		 * 		 		 
		 *  @eventType SubscriptionEvent
		 *  @langversion 3.0
		 *  @playerversion Flash 10		 
		 **/
		public static const DOCUMENT_CHANGED_EVENT:String = "SubscriptionEventDocumentChanged";

		/**
		 *  Data associated with this event. This will contain any information returned from Photoshop. In some cases this information may contain the document id, 
		 *   a returned color, etc. In other cases, it will simply contain a string returned from Photoshop, reiterating the type of change the occured.
		 **/
		public var data:*;
		
		/**
		 *  Creates a new SubscriptionEvent.
		 * 
		 *  @langversion 3.0
		 *  @playerversion Flash 10
		 * 
		 *  @param type The Type of SubscriptionEvent
		 *  @param bubbles 
		 *  @param cancelable
		 **/
		public function SubscriptionEvent(type:String, bubbles:Boolean=false, cancelable:Boolean=false)
		{
			super(type, bubbles, cancelable);
		}
	}
}