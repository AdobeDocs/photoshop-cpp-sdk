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

package com.adobe.photoshop.log
{

	/**
	 *  Logger allows you to specify logging targets, and uses the StructuredLogTestingSDK. Log targeting is left up to your application. Logging and output to the console 	 
	 *  will be suppressed unless you specify targets.
	 *  
	 *  @langversion 3.0
	 *  @playerversion Flash 10
	 **/
	public class Logger
	{				
		/**
		 *  Constructor
		 * 
		 *  @public
		 *  @langversion 3.0
		 *  @playerversion Flash 10
		 **/
		public function Logger()
		{
			
		}
	
		/**
		 *  The static function used for all logging in the API.
		 *    		 
		 *  @langversion 3.0
		 *  @playerversion Flash 10
		 * 
		 *  @param logObject A reference to the instance of the Object that's calling this function. 
		 *  @param message The information you wish to send to SLog's registered targets
		 **/
		public static function log(logObject:Object,message:String):void
		{			
			SLog.info(logObject, message);				
		}
	}
}