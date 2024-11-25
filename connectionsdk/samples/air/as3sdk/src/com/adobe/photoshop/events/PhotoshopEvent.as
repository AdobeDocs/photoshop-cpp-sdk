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
	 *  PhotoshopEvents may be dispatched at various points in this API, and can contain useful information for determining the status of the connection and encryption.
	 * 
	 *  @langversion 3.0
	 *  @playerversion Flash 10
	 **/
	public class PhotoshopEvent extends Event
	{
		
		/**		 
		 *  ProgressEvent is dispatched when there has been progress made in sending, receiving, or parsing messages.
		 * 		 		 
		 *  @eventType ProgressEvent		 
		 *  @langversion 3.0
		 *  @playerversion Flash 10		 
		 **/
		public static const PROGRESS:String = "ProgressEvent";
		
		/**
		 *  ErrorEvent is dispatched in the case of an error. The data property will contain additional information regarding the error
		 * 		 
		 *  @eventType ErrorEvent		 
		 *  @langversion 3.0
		 *  @playerversion Flash 10
		 **/		
		public static const ERROR:String = "ErrorEvent";
			
		/**
		 *  ConnectedEvent is dispatched after a connection was successfully established
		 * 
  		 *  @eventType ConnectedEvent		 
		 *  @langversion 3.0
		 *  @playerversion Flash 10
		 **/
		public static const CONNECTED:String = "ConnectedEvent";
		
		/**
		 *  Upon disconnection of a service or connection, a DisconnectedEvent PhotoshopEvent will be dispatched.
		 * 
 		 *  @eventType DisconnectedEvent
		 *  @langversion 3.0
		 *  @playerversion Flash 10
		 **/
		public static const DISCONNECTED:String = "DisconnectedEvent";
		
		/**
		 *  Dispatched after encryption has been successfully initialized
		 *  		 
		 *  @eventType EncryptionSuccessfulEvent
		 *  @langversion 3.0
		 *  @playerversion Flash 10
		 */
		public static const ENCRYPTION_SUCCESS:String = "EncryptionSuccessful";
		
		/**
		 *  Dispatched if encryption or key derivation should fail
		 * 
		 *  @eventType EncryptionFailedEvent
		 *  @langversion 3.0
		 *  @playerversion Flash 10
		 **/
		public static const ENCRYPTION_FAILED:String = "EncryptionFailed";
		
		/**
		 * Contains information related to this event. The data type varies based on the event. 
		 * 		 
		 * @langversion 3.0
		 * @playerversion Flash 10		 
		 */
		public var data:*;
					
		/**
		 * Constructor
		 * 		 
		 * @langversion 3.0
		 * @playerversion Flash 10
		 * 
 		 * @param type The type of PhotoshopEvent
		 **/
		public function PhotoshopEvent(type:String)
		{
			super(type);
		}
	}
}