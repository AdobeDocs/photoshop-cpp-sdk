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
	import com.adobe.images.JPGEncoder;
	import com.adobe.photoshop.log.Logger;
	
	import flash.display.BitmapData;
	import flash.geom.Rectangle;
	import flash.utils.ByteArray;
	import flash.utils.Endian;
	
	import mx.messaging.AbstractConsumer;
	
	import com.adobe.photoshop.messages.IMessage;

	/**
	 *  An ImageMessage is a Message that contains some image data. Currently the API supports sending JPGs to and from Photoshop.	 
	 *
	 *  @langversion 3.0
	 *  @playerversion Flash 10	 
	 **/
	public class ImageMessage extends Message implements IMessage
	{
		/**		 
		 *  Defined by the Photoshop API
		 **/
		public static const IMAGE_TYPE_JPG:int = 1;
		
		/** 		 
		 *  The BitmapData that this ImageMessage contains, if any.		 
		 **/
		public var bitmapData:BitmapData;
		
		/**
		 *  Constructor
		 **/
		public function ImageMessage()
		{
			super();
			
			this.messageType = Message.MESSAGE_TYPE_IMAGE;
		}
		
		/**
		 *  Converts the ImageMessage to a serialized format, which can be sent to Photoshop.
		 *  Calling this function may take a small amount of time, since it has to encode the Image to a JPG.
		 *  
		 *  @return A ByteArray that contains the raw, serialized format of this ImageMessage.
		 **/
		override public function toStream():ByteArray
		{
			var ba:ByteArray = new ByteArray();
			ba.writeInt(apiVersion);
			ba.writeInt(transactionID);
			ba.writeInt(messageType);
			ba.writeByte(IMAGE_TYPE_JPG);

			// Photoshop expects a stream of JPG data for sent images
			var encoder:JPGEncoder = new JPGEncoder(70);			
			try 
			{				
				var imageBytes:ByteArray = encoder.encode(this.bitmapData);
				ba.writeBytes(imageBytes,0,imageBytes.length);
			}
			catch (te:TypeError)
			{
				Logger.log(this,"Type error while attempting to create an ImageMessage from a bitmap. ImageMessage: 0x0001. " + te.message);			
			}
			catch (e:Error)
			{
				Logger.log(this,"Error while attempting to create an ImageMessage from a bitmap. ImageMessage: 0x0002. " + e.message);
			}
			
			return ba;
		}		
	}
}
