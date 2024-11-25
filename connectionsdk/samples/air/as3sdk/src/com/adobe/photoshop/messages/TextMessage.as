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
	import com.adobe.photoshop.log.Logger;
	
	import flash.utils.ByteArray;
	import com.adobe.photoshop.messages.IMessage;

	/**
	 *  A TextMessage is a Message whose payload contains text data. These can be sent to or from Photoshop, and contain instructions
	 *  or responses regarding commands or events.
	 * 
	 *  @langversion 3.0
	 *  @playerversion Flash 10
	 **/	
	public class TextMessage extends Message implements IMessage
	{
		/**
		 *  The text data that this TextMessage holds, though it can be blank.
		 **/		
		public var message:String;
	
		/**
		 *  Constructor
		 * 
		 *  @langversion 3.0
		 *  @playerversion Flash 10
		 **/
		public function TextMessage()
		{
			super();
			
			this.messageType = Message.MESSAGE_TYPE_TEXT;			
		}		
		
		/**
		 *  Converts the TextMessage to a serialized format, which can be sent to Photoshop.
		 * 
		 *  @langversion 3.0
		 *  @playerversion Flash 10
		 * 		 
		 *  @return A ByteArray that contains the raw, serialized format of this TextMessage.
		 **/
		override public function toStream():ByteArray
		{
			var ba:ByteArray = new ByteArray();
			ba.writeInt(apiVersion);
			ba.writeInt(transactionID);
			ba.writeInt(messageType);
			ba.writeUTFBytes(message);
			
			return ba;
		}
	}
}