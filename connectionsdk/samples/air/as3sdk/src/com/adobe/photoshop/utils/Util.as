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

package com.adobe.photoshop.utils
{
	/**
	 *  A class that tends to contain static helper functions, which are mostly useful for the internal API. However, if you're extending the functionality of this API in your own code,	 
	 *  you maay find its methods useful.
	 * 
	 *  @langversion 3.0
	 *  @playerversion Flash 10	 
	 **/
	public class Util
	{
		/**
		 *  Constructor
		 * 
		 *  @langversion 3.0
		 *  @playerversion Flash 10
		 **/
		public function Util()
		{
			return;
		}
		
		/**
		 *  Takes a string of text that contains parameters in the format of $1, $2, $3, ... , $n , and expects an 
		 * 	array of n length. The function will cycle through the available parameters and replace the tokens in the input string, in order.
		 *  If the number of parameters is less than the number of tokens in the string, the output will still contain the tokens.
		 * 		 
		 *  @langversoin 3.0
		 *  @playerversion Flash 10
		 * 
		 *  @param textToModify The input string that contains tokens of the form $1, $2, $3, ... , $n
		 *  @param ... rest . A number of Strings, containing the text that should be placed into the tokens
		 * 
		 *  @return Returns the modified version of the input string
		 **/ 
		public static function replaceParams(textToModify:String, ... rest):String
		{		
			for ( var x:uint = 0; x<rest.length; x++ )
			{				
				textToModify = textToModify.replace(new RegExp("\\$" + (x+1)),rest[x]);				
			}			
			
			return textToModify;
		}
	}
}