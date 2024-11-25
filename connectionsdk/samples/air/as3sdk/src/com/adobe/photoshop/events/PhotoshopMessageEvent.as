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
	import com.adobe.photoshop.messages.IMessage;
	
	import flash.events.Event;

	/**
	 *  A PhotoshopMessageEvent is similar to PhotoshopEvent in that the API may choose to dispatch it during execution, however, it contains information specific to sending, receiving, and 
	 *  parsing Messages.
	 * 
	 *  @langversion 3.0
	 *  @playerversion Flash 10
	 **/
	public class PhotoshopMessageEvent extends Event
	{
		/**
		 *  A MessageReceivedEvent is dispatched when a service or connection has received an IMessage.
		 * 	
		 *  @eventType MessageReceivedEvent		 
		 *  @langversion 3.0
		 *  @playerversion Flash 10
		 **/
		public static const MESSAGE_RECEIVED:String = "MessageReceivedEvent";
		
		/**
		 *  Dispatched when a Message has been received, but it contains information indicating that there's been an error
		 *  	
		 *  @eventType ErrorMessageReceivedEvent		 
		 *  @langversion 3.0
		 *  @playerversion Flash 10
		 **/
		public static const ERROR_MESSAGE_RECEIVED:String = "ErrorMessageReceivedEvent";	
		
		/**
		 *  A PhotoshopMessageEvent with the MessageSentEvent is dispatched when a message has been successfully sent
		 * 		 
		 *  @eventType MessageSentEvent		 
		 *  @langversion 3.0
		 *  @playerversion Flash 10
		 **/
		public static const MESSAGE_SENT:String = "MessageSentEvent";		
		
		/**
		 *  The IMessage that has been received, if applicable.
		 * 		 
		 *  @langversion 3.0
		 *  @playerversion Flash 10
		 **/
		public var message:IMessage;
		
		/**
		 * Constructor.
		 * 		 
		 * @langversion 3.0
		 * @playerversion Flash 10
		 * 		
		 * @param type The Type of PhotoshopMessageEvent
		 **/
		public function PhotoshopMessageEvent(type:String)
		{
			super(type);
		}
	}
}