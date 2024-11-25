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

package com.adobe.photoshop.messages
{ 		
	import com.adobe.photoshop.messages.IMessage;
	import flash.utils.ByteArray;

	/**
	 *  A Message is the most generic type of Message that can be sent or received, though the API doesn't 	 
	 *  do either with Messages directly. It's akin to an abstract class. Most likely, you'll want to use something that extends this 
	 *  class, such as ImageMessage or TextMessage.
	 * 
	 *  @langversion 3.0
	 *  @playerversion Flash 10
	 **/
	public class Message implements IMessage
	{
		/**
		 *  @private
		 *  A constant set by the Photoshop API.
		 **/
		public static const MESSAGE_ERROR_TEXT:int = 1;
		
		/**
		 *  @private
		 *  A constant set by the Photoshop API.
		 **/
		public static const MESSAGE_TYPE_TEXT:int = 2;
		
		/**
		 *  @private
		 *  A constant set by the Photoshop API.
		 **/
		public static const MESSAGE_TYPE_IMAGE:int = 3;
		
		/**
		 *  @private
		 *  The version of the Photoshop API this code supports
		 **/
		private static const API_VERSION:int = 1;
		
		/** 
		 *  @public
		 *  The version of the Photoshop API that this class supports--but is settable should you choose to modify or extend this API.
		 **/
		public var apiVersion:int;
		
		//----------------------------------
		//  messageLength
		//----------------------------------
				 		 
		private var _messageLength:int;		
		
		/**
		 * 	The message length of the Photoshop message.
		 * 		 
		 *  @langversion 3.0
		 *  @playerversion Flash 10
		 **/
		public function get messageLength():int
		{
			return _messageLength;
		}
		
		/**
		 * @private
		 **/
		public function set messageLength(value:int):void
		{
			_messageLength = value;
		}
		
		//----------------------------------
		//  transactionID
		//----------------------------------
		
		private var _transactionID:int;
		

		/**
		 * 	The transaction ID of the Photoshop message.
		 * 
		 *  @langversoin 3.0
		 *  @playerversion Flash 10		 
		 **/
		public function get transactionID():int
		{
			return _transactionID;
		}		

		/**
		 * 	@private
		 */
		public function set transactionID(value:int):void
		{
			_transactionID = value;
		}
		
		//----------------------------------
		//  _messageType
		//----------------------------------
		
		private var _messageType:int;

		/**
		 * 	The type of message that IMessage represents.
		 */
		public function get messageType():int
		{
			return _messageType;
		}
		
		/**
		 * 	@private
		 */
		public function set messageType(value:int):void
		{
			_messageType = value;
		}		
			
		/**
		 *  Creates a new message.
		 * 
		 *  @langversion 3.0
		 *  @playerversion Flash 10
		 * 
		 *  @param apiVersion The Photoshop API version that the message is compatible with.
		 **/
		public function Message(apiVersion:int = API_VERSION)
		{
			this.apiVersion = apiVersion; 
		}

		/**
		 *  Converts the message into a stream of raw data, represented as a ByteArray. Used for sending to Photoshop
		 * 
		 *  @langversion 3.0
		 *  @playerversion Flash 10
		 * 
		 *  @return Returns a byte-by-byte representation of this message in a ByteArray of raw data
		 **/		
		public function toStream():ByteArray
		{			
			return new ByteArray();
		}
			
	}
}