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

package com.adobe.photoshop.connection
{
	/**
	 *  The TransactionManager is a class that makes it easy to always grab the next transaction ID. It can be used in a singleton pattern. You may manage transaction IDs yourself,
	 *   but the API uses this class, and it will then be up to your application to not create IDs that conflict with the APIs requests. This may become particularly tricky
	 *   if you also use the API's SubscriptionManager. If you send requests with your own transaction IDs and Photoshop responds with those IDs, it's possible you could trigger
	 *   the SubscriptionManager for any active subscriptions, leading to undetermined behavior (probably parsing errors or SubscriptionEvents with confusing data). 
	 * 
	 *  @langversion 3.0
	 *  @playerversion Flash 10
	 */
	public class TransactionManager
	{
		/**
		 *  The instance of the TransactionManager
		 **/
		public static var inst:TransactionManager
		
		private var _transactionID:int; 
		
		/**
		 *  Creates a new TransactionManager, and sets the internal transaction counter to 0
		 **/
		public function TransactionManager()
		{
			this._transactionID = 0;
		}
		
		/**
		 *  Gets the next available transaction ID.
		 * 
		 *  @return The next transaction ID that can be used
		 **/
		public function getNextTransactionID():int
		{
			return ++this._transactionID;
		}
		
		/**
		 *  Gets the current transaction ID. In many cases, this ID will represent a message that's already been sent.
		 * 
		 *  @return The current ID
		 **/
		public function getCurrentTransactionID():int
		{
			return this._transactionID;
		}
		
		/** 
		 *  Sets the transaction ID, overriding the internal counter.
		 * 
		 *  @param value The ID to set
		 **/
		public function set transactionID(value:int):void
		{
			this._transactionID = value;
		}
		
		/**
		 *  Cleans up this instance of the TransactionManager
		 **/
		public function destroy():void
		{
			inst = null;
		}
		
		/** 
		 *  Gets a singleton reference to a global transaction manager.
		 * 
		 *  @return A global, singleton-pattern TransactionManager instance
		 **/ 
		public static function getInstance():TransactionManager
		{
			if ( !inst )
			{
				inst = new TransactionManager();				
			}
			
			return inst; 
		}
	}
}