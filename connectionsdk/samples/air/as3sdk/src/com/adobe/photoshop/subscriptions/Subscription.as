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
	/**
	 *  A Subscription contains some useful information regarding either a returned or dispatched request. In addition, it stores an internal enumeration 
	 *   regarding the type of subscription and an integer API ID.
	 **/
	public class Subscription
	{

		public static const SUBSCRIPTION_EVENTS_COLOR_FOREGROUND_CHANGED:int = 0;
		public static const SUBSCRIPTION_EVENTS_COLOR_BACKGROUND_CHANGED:int = 1;
		public static const SUBSCRIPTION_EVENTS_COLOR_SETTINGS_CHANGED:int = 2; 
		public static const SUBSCRIPTION_EVENTS_PREFERENCES_CHANGED:int = 3;
		public static const SUBSCRIPTION_EVENTS_KEYBOARD_SHORTCUTS_CHANGED:int = 4;
		public static const SUBSCRIPTION_EVENTS_SCREEN_MODE_CHANGED:int = 5;
		public static const SUBSCRIPTION_EVENTS_QUICK_MASK_STATE_CHANGED:int = 6;
		public static const SUBSCRIPTION_EVENTS_TOOL_CHANGED:int = 7;
		public static const SUBSCRIPTION_EVENTS_ACTIVEVIEW_CHANGED:int = 8;
		public static const SUBSCRIPTION_EVENTS_DOCUMENT_CLOSED:int = 9;
		public static const SUBSCRIPTION_EVENTS_CURRENTDOCUMENT_CHANGED:int = 10;
		public static const SUBSCRIPTION_EVENTS_DOCUMENTVIEW_CREATED:int = 11;
		public static const SUBSCRIPTION_EVENTS_DOCUMENT_CHANGED:int = 12;
		
		/**
		 *  The transaction ID of this Subscription
		 **/
		public var transactionID:int;
		
		/**
		 *  The Type of this subscription
		 **/
		public var subscriptionType:int;
	
		/**
		 *  Creates a new Subscription
		 *
		 *  @langversion 3.0
		 *  @playerversion Flash 10
		 * 
		 *  @param subscriptionType The Type of subscription to create
		 **/
		public function Subscription(subscriptionType:int)
		{
			this.subscriptionType = subscriptionType;
		}
	}
}
