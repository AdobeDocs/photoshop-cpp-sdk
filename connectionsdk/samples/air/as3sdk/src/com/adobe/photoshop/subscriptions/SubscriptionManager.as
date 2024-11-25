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

package com.adobe.photoshop.subscriptions
{
	import com.adobe.photoshop.connection.PhotoshopConnection;
	import com.adobe.photoshop.connection.TransactionManager;
	import com.adobe.photoshop.dispatchers.MessageDispatcher;
	import com.adobe.photoshop.events.SubscriptionEvent;
	import com.adobe.photoshop.log.Logger;
	import com.adobe.photoshop.messages.TextMessage;
	
	import flash.display.Graphics;
	import flash.events.EventDispatcher;	

	/**
	 * The SubscriptionManager gives you an easy way to get data from Photoshop that pertains to things like color changes, changed tools or settings, and other user events. 
	 * For example, if you add an event listener for a SubscriptionEvent.FOREGROUND_COLOR_CHANGE_EVENT, you'll receive a SubscriptionEvent.FOREGROUND_COLOR_CHANGE_EVENT whenever the user changes the foreground color
	 * in Photoshop. (Assuming that the API is able to successfully send the subscription message to Photoshop. This requires that you set up the PhotoshopConnection properly.)
	 * 
	 * You may attach event listeners for any type of SubscriptionEvent, and the SubscriptionManager will make the required calls to Photoshop to set up (or deactivate) the subscription.  
	 **/
	public class SubscriptionManager extends EventDispatcher
	{
		/**
		 * 	Dispatched when the foreground color changes
		 *
		 *  @eventType com.adobe.photoshop.events.SubscriptionEvent.FOREGROUND_COLOR_CHANGED_EVENT
		 *  
		 *  @langversion 3.0
		 *  @playerversion Flash 10
		 */	
		[Event(name="SubscriptionEventForegroundColorChanged", type="com.adobe.photoshop.events.SubscriptionEvent")]
		
		// Subscriptions are mapped by index, which corresponds to the ints in the Subscription class
		private var subscriptions:Array;		
		private var pc:PhotoshopConnection;		
		
		/**
		 *  Constructor. 
		 * 
		 *  @param pc The PhotoshopConnection that the SubscriptionManager should use when sending subscription-related data. Note that setting this parameter will also set the SubscriptionManager
		 *   property inside of the PhotoshopConnection. Thus, as soon as you create this SubscriptionManager, the PhotoshopConnection that you pass in will use it for all future parsing of subscription-related data.
		 *   If you have highly customized logic that somehow involves using multiple SubscriptionManagers and/or PhotoshopConenctions, you should restore the pc.subscriptionManager property to its previous value. 
		 *
		 *  @langversion 3.0
		 *  @playerversion Flash 10
		 **/
		public function SubscriptionManager(pc:PhotoshopConnection) 
		{
			super();
			
			this.pc = pc;
			this.pc.subscriptionManager = this;
			this.subscriptions = new Array();					
		}
		
		/**
		 *  @private
		 *  Subscribe to the specified type of subscription. This function is called when you add an event listener for the relevant SubscriptionEvent.
		 * 
		 *  @param subscriptionType An int representing the type of subscription. These types are controlled in a global, static, constant way in the Subscription class.
		 *   e.g., pass in Subscription.SUBSCRIPTION_EVENTS_COLOR_FOREGROUND_CHANGED, rather than an int of 0. This will ensure future compatability should the API change.
		 * 
		 *  @return Returns false if you are already subscribed to this type of subscrtipion.
		 * 
		 *  @langversion 3.0
		 *  @playerversion Flash 10
		 **/
		private function subscribe(subscriptionType:int):Boolean 
		{
			var succ:Boolean = true;
			
			var se:SubscriptionEvent;
			var em:String;
			
			if ( this.subscriptions[subscriptionType] )
			{
				// Already subscribed
				succ = false;
				
				se = new SubscriptionEvent(SubscriptionEvent.ALREADY_SUBSCRIBED);
				em = "You're already subscribed to this subscription. SubscriptionManager: 0x0001";
				Logger.log(this,em);
				se.data = em;
				this.dispatchEvent(se);
			}
			else
			{
				var s:Subscription = new Subscription(subscriptionType);
				this.subscriptions[subscriptionType] = s;
				s.transactionID = TransactionManager.getInstance().getNextTransactionID();
				var tm:TextMessage = MessageDispatcher.generateTextMessageForSubscribingOrUnsubscribing(s,true);
				this.pc.encryptAndSendData(tm.toStream());
				
				se = new SubscriptionEvent(SubscriptionEvent.SUBSCRIBED);
				em = "Successfully sent subscription request with transactionID of " + s.transactionID + ". SubscriptionManager: 0x0004";
				Logger.log(this,em);
				se.data = em;
				this.dispatchEvent(se);
			}
			
			return succ;
		}
		
		/**
		 *  Handles a subscription-related text message response from Photoshop. Unless you are manually creating TextMessages (perhaps based on raw data you receive from Photoshop), 
		 *  you should not need to call this function. 
		 * 
		 *  @param tm The TextMessage that contains text information pertaining to a subscription-related Photoshop event
		 * 
		 *  @langversion 3.0
		 *  @playerversion Flash 10   
		 **/
		public function handleSubscriptionTextMessage(tm:TextMessage):void
		{
			var s:Subscription = determineWhichSubscriptionByID(tm.transactionID);
			var color:String;
			var se:SubscriptionEvent;
			
			// First we see if Photoshop is just sending us "[ActionDescriptor]". If so, we go no further.
			// In the future, this would be a good hook to send out an additional message, indicating that Photoshop confirms the subscription.
			if ( tm.message.substr(0,18) == "[ActionDescriptor]" ) return;
			
			// Broken out into individual cases in order to facilitate future subscription-specific logic.
			switch ( s.subscriptionType )
			{
				case Subscription.SUBSCRIPTION_EVENTS_COLOR_FOREGROUND_CHANGED:
					// "foregroundColorChanged\r######"
					color = tm.message.substr(23,6); 					
					Logger.log(this, "The foreground color is " + color + ". SubscriptionManager: 0x0005");					
					se = new SubscriptionEvent(SubscriptionEvent.FOREGROUND_COLOR_CHANGED_EVENT);
					se.data = color;
					this.dispatchEvent(se);					
					break;
				
				case Subscription.SUBSCRIPTION_EVENTS_COLOR_BACKGROUND_CHANGED:
					// "backgroundColorChanged\r######"
					color = tm.message.substr(23,6);					
					Logger.log(this, "The background color is " + color + ". SubscriptionManager: 0x0006");					
					se = new SubscriptionEvent(SubscriptionEvent.BACKGROUND_COLOR_CHANGED_EVENT);
					se.data = color;
					this.dispatchEvent(se);				
					break;

				case Subscription.SUBSCRIPTION_EVENTS_COLOR_SETTINGS_CHANGED:
					// "colorSettingsChanged"
					se = new SubscriptionEvent(SubscriptionEvent.COLOR_SETTINGS_CHANGED_EVENT);
					se.data = tm.message;
					this.dispatchEvent(se);					
					break;

				case Subscription.SUBSCRIPTION_EVENTS_PREFERENCES_CHANGED:
					// "preferencesChanged"
					se = new SubscriptionEvent(SubscriptionEvent.PREFERENCES_CHANGED_EVENT);
					se.data = tm.message;
					this.dispatchEvent(se);
					break;

				case Subscription.SUBSCRIPTION_EVENTS_KEYBOARD_SHORTCUTS_CHANGED:
					// "keyboardShortcutsChanged"
					se = new SubscriptionEvent(SubscriptionEvent.KEYBOARD_SHORTCUTS_CHANGED_EVENT);
					se.data = tm.message;
					this.dispatchEvent(se); 
					break;					

				case Subscription.SUBSCRIPTION_EVENTS_SCREEN_MODE_CHANGED:
					// "screenModeChanged"
					se = new SubscriptionEvent(SubscriptionEvent.SCREEN_MODE_CHANGED_EVENT);
					se.data = tm.message;
					this.dispatchEvent(se);
					break;

				case Subscription.SUBSCRIPTION_EVENTS_QUICK_MASK_STATE_CHANGED:
					// "quickMaskStateChanged\r(enter|exit|???)"
					se = new SubscriptionEvent(SubscriptionEvent.QUICK_MASK_STATE_CHANGED_EVENT);
					se.data = tm.message;
					this.dispatchEvent(se);
					break;

				case Subscription.SUBSCRIPTION_EVENTS_TOOL_CHANGED:
					// "toolChanged\r..."				
					se = new SubscriptionEvent(SubscriptionEvent.TOOL_CHANGED_EVENT);					
					se.data = tm.message;
					this.dispatchEvent(se);
					break;

				case Subscription.SUBSCRIPTION_EVENTS_ACTIVEVIEW_CHANGED:
					// 
					se = new SubscriptionEvent(SubscriptionEvent.ACTIVEVIEW_CHANGED_EVENT);
					se.data = tm.message;
					this.dispatchEvent(se);
					break;

				case Subscription.SUBSCRIPTION_EVENTS_DOCUMENT_CLOSED:
										
					// "closedDocument\r##"					
					se = new SubscriptionEvent(SubscriptionEvent.DOCUMENT_CLOSED_EVENT);
					se.data = tm.message;
					this.dispatchEvent(se);					
					break;
				
				case Subscription.SUBSCRIPTION_EVENTS_CURRENTDOCUMENT_CHANGED:
					// 
					se = new SubscriptionEvent(SubscriptionEvent.CURRENTDOCUMENT_CHANGED_EVENT);
					se.data = tm.message;
					this.dispatchEvent(se);
					break;

				case Subscription.SUBSCRIPTION_EVENTS_DOCUMENTVIEW_CREATED:
					// 
					se = new SubscriptionEvent(SubscriptionEvent.DOCUMENTVIEW_CREATED_EVENT);
					se.data = tm.message;
					this.dispatchEvent(se);
					break;

				case Subscription.SUBSCRIPTION_EVENTS_DOCUMENT_CHANGED:
					// "documentChanged\r##"
					se = new SubscriptionEvent(SubscriptionEvent.DOCUMENT_CHANGED_EVENT);
					se.data = tm.message;
					this.dispatchEvent(se);
					break;

				default:
					break;
			}
		}
		
		/**
		 *  @private
		 *  Determines which Subscription Object (if any) is assigned to handle the given transaction, based on ID.
		 *  
		 *  @param transactionID The ID to use when checking for a mapping between IDs and Subscription Objects.
		 * 
		 *  @return Returns a Subscription (which can be null) of the assigned Subscription	 
		 **/
		private function determineWhichSubscriptionByID(transactionID:int):Subscription
		{
			var s:Subscription;
			
			for ( var x:int=0; x<this.subscriptions.length; x++ )
			{
				if ( this.subscriptions[x] )
				{					
					if ( this.subscriptions[x].transactionID == transactionID )
					{
						s = this.subscriptions[x];
						break;
					}
				}
			}																				
			
			return s;
		}
		
		/**		 
		 *  Unsubscribes from a subscription, such that you won't receive the relevant updates from Photoshop or this API.
		 *  This function is automatically called when you remove an event listener for the relevant SubscriptionEvent. However, you may call this function yourself
		 *  if you wish to force Photoshop to unsubscribe you from an event, if you believe that your application has subscribed, had an error, and has left Photoshop in a confused state about active subscriptions.
		 *  If this is the case, be sure to set the following parameters correctly.
		 *  
		 *  @param subscriptionType The Type of subscription that you want to unsubscribe from
		 *  @param forceSendMessage Set this to true if you want the API to send the message, regardless if you're actually subscribed to this subscription. Useful for debugging your own applications.
		 * 
		 *  @return Returns false if you aren't already subscribed to the given type of subscription AND if forceSendMessage is set to false
		 * 
		 * 	@langversion 3.0
		 *  @playerversion Flash 10		 
		 **/
		private function unsubscribe(subscriptionType:int, forceSendMessage:Boolean = false):Boolean
		{
			var succ:Boolean = true;
			
			var se:SubscriptionEvent;
			var em:String;
			
			if ( !this.subscriptions[subscriptionType] && !forceSendMessage )
			{
				// Not subscribed
				succ = false;
				
				se = new SubscriptionEvent(SubscriptionEvent.NOT_SUBSCRIBED);
				em = "You're not subscribed to this subscription! SubscriptionManager: 0x0002";
				Logger.log(this,em);
				se.data = em;
				this.dispatchEvent(se);
			}
			else
			{
				this.subscriptions[subscriptionType] = null;
								
				var s:Subscription = new Subscription(subscriptionType);
				//this.subscriptions[subscriptionType] = s;
				s.transactionID = TransactionManager.getInstance().getNextTransactionID();
				var tm:TextMessage = MessageDispatcher.generateTextMessageForSubscribingOrUnsubscribing(s,false);
				this.pc.encryptAndSendData(tm.toStream());
				
				se = new SubscriptionEvent(SubscriptionEvent.UNSUBSCRIBED);
				em = "Successfully unsubscribed. SubscriptionManager: 0x0003";
				Logger.log(this,em);
				se.data = em;
				this.dispatchEvent(se);
			}
							
			return succ;
		}
		
		/**		 
		 *  Determines if the SubscriptionManager is subscribed, based on the type of subscriptiopn
		 *  
		 *  @param subscriptionType The Type of subscription that you want to check
		 * 
		 *  @return Returns true if you're currently subscribed to this type of subscription
		 * 
		 * 	@langversion 3.0
		 *  @playerversion Flash 10		 
		 **/		
		public function isSubscribedBySubscriptionType(subscriptionType:int):Boolean
		{
			var subscribed:Boolean = false;
			
			if ( this.subscriptions[subscriptionType] )
			{
				subscribed = true;	
			}
			
			return subscribed;
		}
		
		/**		 
		 *  Determines if the SubscriptionManager is subscribed, based on the transactionID. The transactionID is how both Photoshop and this API keep track of the mapping 
		 *  between events and subscriptions. 
		 *  
		 *  @param subscriptionID The transactionID that you want to check
		 * 
		 *  @return Returns true if this transactionID is mapped to an active subscription
		 * 
		 * 	@langversion 3.0
		 *  @playerversion Flash 10		 
		 **/		
		public function isSubcribedByTransactionID(transactionID:int):Boolean
		{
			var subscribed:Boolean = false;
			var s:Subscription;
			
			for ( var x:int=0; x<subscriptions.length; x++ )
			{
				if ( subscriptions[x] ) 
				{
					s = subscriptions[x];
					if ( s.transactionID == transactionID )
					{
						subscribed = true;
						break;
					}
				}
			}

			return subscribed;
		}
		
		/**
		 *  Attaches an event listener of the given type. If this type is a SubscriptionEvent, this function will send the necessary messages to Photoshop to indicate that you wish to subscribe. 
		 * 
		 *  @langversion 3.0
		 *  @playerversion Flash 10
		 **/
		override public function addEventListener(type:String, listener:Function, useCapture:Boolean=false, priority:int=0, useWeakReference:Boolean=false):void
		{
			super.addEventListener(type,listener,useCapture,priority,useWeakReference);				
			switch ( type )
			{
				case SubscriptionEvent.BACKGROUND_COLOR_CHANGED_EVENT:
					this.subscribe(Subscription.SUBSCRIPTION_EVENTS_COLOR_BACKGROUND_CHANGED);
					break;
				case SubscriptionEvent.FOREGROUND_COLOR_CHANGED_EVENT:
					this.subscribe(Subscription.SUBSCRIPTION_EVENTS_COLOR_FOREGROUND_CHANGED);
					break;
				case SubscriptionEvent.ACTIVEVIEW_CHANGED_EVENT:
					this.subscribe(Subscription.SUBSCRIPTION_EVENTS_ACTIVEVIEW_CHANGED);
					break;
				case SubscriptionEvent.COLOR_SETTINGS_CHANGED_EVENT:
					this.subscribe(Subscription.SUBSCRIPTION_EVENTS_COLOR_SETTINGS_CHANGED);
					break;
				case SubscriptionEvent.CURRENTDOCUMENT_CHANGED_EVENT:
					this.subscribe(Subscription.SUBSCRIPTION_EVENTS_CURRENTDOCUMENT_CHANGED);
					break;
				case SubscriptionEvent.DOCUMENT_CHANGED_EVENT:
					this.subscribe(Subscription.SUBSCRIPTION_EVENTS_DOCUMENT_CHANGED);
					break;
				case SubscriptionEvent.DOCUMENT_CLOSED_EVENT:
					this.subscribe(Subscription.SUBSCRIPTION_EVENTS_DOCUMENT_CLOSED);
					break;
				case SubscriptionEvent.DOCUMENTVIEW_CREATED_EVENT:
					this.subscribe(Subscription.SUBSCRIPTION_EVENTS_DOCUMENTVIEW_CREATED);
					break;
				case SubscriptionEvent.KEYBOARD_SHORTCUTS_CHANGED_EVENT:
					this.subscribe(Subscription.SUBSCRIPTION_EVENTS_KEYBOARD_SHORTCUTS_CHANGED);
					break;
				case SubscriptionEvent.PREFERENCES_CHANGED_EVENT:
					this.subscribe(Subscription.SUBSCRIPTION_EVENTS_PREFERENCES_CHANGED);
					break;
				case SubscriptionEvent.QUICK_MASK_STATE_CHANGED_EVENT:
					this.subscribe(Subscription.SUBSCRIPTION_EVENTS_QUICK_MASK_STATE_CHANGED);
					break;
				case SubscriptionEvent.SCREEN_MODE_CHANGED_EVENT:
					this.subscribe(Subscription.SUBSCRIPTION_EVENTS_SCREEN_MODE_CHANGED);
					break;
				case SubscriptionEvent.TOOL_CHANGED_EVENT:
					this.subscribe(Subscription.SUBSCRIPTION_EVENTS_TOOL_CHANGED);
					break;
				default:
					break;
			}		
		}
		
		/**
		 *  Removes an event listener of the given type. If this type is a SubscriptionEvent, this function will send the necessary messages to Photoshop to indicate that you wish to unsubscribe. 
		 * 
		 *  @langversion 3.0
		 *  @playerversion Flash 10
		 **/
		override public function removeEventListener(type:String, listener:Function, useCapture:Boolean=false):void
		{
			super.removeEventListener(type,listener,useCapture);
			switch ( type )
			{
				case SubscriptionEvent.BACKGROUND_COLOR_CHANGED_EVENT:
					this.unsubscribe(Subscription.SUBSCRIPTION_EVENTS_COLOR_BACKGROUND_CHANGED);
					break;
				case SubscriptionEvent.FOREGROUND_COLOR_CHANGED_EVENT:
					this.unsubscribe(Subscription.SUBSCRIPTION_EVENTS_COLOR_FOREGROUND_CHANGED);
					break;
				case SubscriptionEvent.ACTIVEVIEW_CHANGED_EVENT:
					this.unsubscribe(Subscription.SUBSCRIPTION_EVENTS_ACTIVEVIEW_CHANGED);
					break;
				case SubscriptionEvent.COLOR_SETTINGS_CHANGED_EVENT:
					this.unsubscribe(Subscription.SUBSCRIPTION_EVENTS_COLOR_SETTINGS_CHANGED);
					break;
				case SubscriptionEvent.CURRENTDOCUMENT_CHANGED_EVENT:
					this.unsubscribe(Subscription.SUBSCRIPTION_EVENTS_CURRENTDOCUMENT_CHANGED);
					break;
				case SubscriptionEvent.DOCUMENT_CHANGED_EVENT:
					this.unsubscribe(Subscription.SUBSCRIPTION_EVENTS_DOCUMENT_CHANGED);
					break;
				case SubscriptionEvent.DOCUMENT_CLOSED_EVENT:
					this.unsubscribe(Subscription.SUBSCRIPTION_EVENTS_DOCUMENT_CHANGED);
					break;
				case SubscriptionEvent.DOCUMENTVIEW_CREATED_EVENT:
					this.unsubscribe(Subscription.SUBSCRIPTION_EVENTS_DOCUMENTVIEW_CREATED);
					break;
				case SubscriptionEvent.KEYBOARD_SHORTCUTS_CHANGED_EVENT:
					this.unsubscribe(Subscription.SUBSCRIPTION_EVENTS_KEYBOARD_SHORTCUTS_CHANGED);
					break;
				case SubscriptionEvent.PREFERENCES_CHANGED_EVENT:
					this.unsubscribe(Subscription.SUBSCRIPTION_EVENTS_PREFERENCES_CHANGED);
					break;
				case SubscriptionEvent.QUICK_MASK_STATE_CHANGED_EVENT:
					this.unsubscribe(Subscription.SUBSCRIPTION_EVENTS_QUICK_MASK_STATE_CHANGED);
					break;
				case SubscriptionEvent.SCREEN_MODE_CHANGED_EVENT:
					this.unsubscribe(Subscription.SUBSCRIPTION_EVENTS_SCREEN_MODE_CHANGED);
					break;
				case SubscriptionEvent.TOOL_CHANGED_EVENT:
					this.unsubscribe(Subscription.SUBSCRIPTION_EVENTS_TOOL_CHANGED);
					break;
				default: 					
					break;
			}
			
		}
	}
}