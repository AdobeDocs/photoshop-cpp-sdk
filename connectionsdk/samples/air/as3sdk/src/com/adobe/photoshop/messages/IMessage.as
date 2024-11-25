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
	import flash.utils.ByteArray;

	/**
	 *  An interface that defines the requirements for an Object being sent, received, and parsed as a Photoshop message
	 * 
	 *  @langversion 3.0
	 *  @playerversion Flash 10
	 **/	 
	public interface IMessage
	{
		/**		 
		 * 	The message length of the Photoshop message.
		 */
		function get messageLength():int;
		function set messageLength(value:int):void;
		
		/**
		 * 	The transaction ID of the Photoshop message.
		 */
		function get transactionID():int;
		function set transactionID(value:int):void;
		
		/**
		 * 	The type of message that IMessage represents.
		 */
		function get messageType():int;
		function set messageType(value:int):void;			
		
		function toStream():ByteArray;
	}
}