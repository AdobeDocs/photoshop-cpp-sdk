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

package com.adobe.photoshop.dispatchers
{
	import com.adobe.photoshop.connection.PhotoshopConnection;
	import com.adobe.photoshop.connection.TransactionManager;
	import com.adobe.photoshop.messages.IMessage;
	import com.adobe.photoshop.messages.Message;
	import com.adobe.photoshop.messages.TextMessage;
	import com.adobe.photoshop.subscriptions.Subscription;
	import com.adobe.photoshop.tools.Tool;
	import com.adobe.photoshop.utils.Util;
	
	import flash.display.Loader;
	import flash.net.URLRequest;
	
	import mx.messaging.AbstractConsumer;

	/**
	 *  The MessageDispatcher is an abstraction layer on top of the PhotoshopConnection, which makes it easy to send 
	 *   properly formatted messages to Photoshop. You can either call some of the helper functions that will create and dispatch
	 *   the messages for you (createNewDocument, requestThumbnail, etc.), or create your own IMessage and call sendMessage().
	 * 
	 *  Refer to the Photoshop scripting documentation to see the format of the messages you should send.   
	 * 
	 *  @langversion 3.0
	 *  @playerversion Flash 10
	 */
	public class MessageDispatcher
	{
		/**
		 *  Creates a new document
		 */
		protected static const MESSAGE_TEXT_DOCUMENTS_ADD:String = "documents.add($1, $2, $3)\n";
				
		/**		 
		 *	Update us when the keyboard shortcuts change
		 */
		protected static const MESSAGE_TEXT_KEYBOARD_SHORTCUTS_CHANGED:String = ( <![CDATA[
			var idNS = stringIDToTypeID( "$1" );
			var desc1 = new ActionDescriptor();
			desc1.putClass( stringIDToTypeID( "eventIDAttr" ), stringIDToTypeID( "keyboardShortcutsChanged" ) );
			executeAction( idNS, desc1, DialogModes.NO );
			]]> ).toString();		
		
		/**		 
		 *	Update us when the screen mode changes 
		 */
		protected static const MESSAGE_TEXT_SCREEN_MODE_CHANGED:String = ( <![CDATA[
			var idNS = stringIDToTypeID( "$1" );
			var desc1 = new ActionDescriptor();
			desc1.putClass( stringIDToTypeID( "eventIDAttr" ), stringIDToTypeID( "screenModeChanged" ) );
			executeAction( idNS, desc1, DialogModes.NO );
			]]> ).toString();
		
		/**		 
		 *	Update us when the quick mask state changes
		 */
		protected static const MESSAGE_TEXT_QUICK_MASK_STATE_CHANGED:String = ( <![CDATA[
			var idNS = stringIDToTypeID( "$1" );
			var desc1 = new ActionDescriptor();
			desc1.putClass( stringIDToTypeID( "eventIDAttr" ), stringIDToTypeID( "quickMaskStateChanged" ) );
			executeAction( idNS, desc1, DialogModes.NO );
			]]> ).toString();
		
		/**		 
		 *	Update us when the selected tool changes 
		 */
		protected static const MESSAGE_TEXT_TOOL_CHANGED:String = ( <![CDATA[
			var idNS = stringIDToTypeID( "$1" );
			var desc1 = new ActionDescriptor();
			desc1.putClass( stringIDToTypeID( "eventIDAttr" ), stringIDToTypeID( "toolChanged" ) );
			executeAction( idNS, desc1, DialogModes.NO );
			]]> ).toString();
		
		/**		 
		 *	Update us when a document is changed
		 */
		protected static const MESSAGE_TEXT_DOCUMENT_CHANGED:String = ( <![CDATA[
			var idNS = stringIDToTypeID( "$1" );
			var desc1 = new ActionDescriptor();
			desc1.putClass( stringIDToTypeID( "eventIDAttr" ), stringIDToTypeID( "documentChanged" ) );
			executeAction( idNS, desc1, DialogModes.NO );
			]]> ).toString();
		
		
		/**		 
		 *	Update us when a new DocumentView is created
		 */
		protected static const MESSAGE_TEXT_DOCUMENTVIEW_CREATED:String = ( <![CDATA[
			var idNS = stringIDToTypeID( "$1" );
			var desc1 = new ActionDescriptor();
			desc1.putClass( stringIDToTypeID( "eventIDAttr" ), stringIDToTypeID( "newDocumentViewCreated" ) );
			executeAction( idNS, desc1, DialogModes.NO );
			]]> ).toString();
		
		/**		 
		 *	Update us when the current document changes
		 */
		protected static const MESSAGE_TEXT_CURRENTDOCUMENT_CHANGED:String = ( <![CDATA[
			var idNS = stringIDToTypeID( "$1" );
			var desc1 = new ActionDescriptor();
			desc1.putClass( stringIDToTypeID( "eventIDAttr" ), stringIDToTypeID( "currentDocumentChanged" ) );
			executeAction( idNS, desc1, DialogModes.NO );
			]]> ).toString();
		
		/**		 
		 *	Update us when the active view changes
		 */
		protected static const MESSAGE_TEXT_ACTIVEVIEW_CHANGED:String = ( <![CDATA[
			var idNS = stringIDToTypeID( "$1" );
			var desc1 = new ActionDescriptor();
			desc1.putClass( stringIDToTypeID( "eventIDAttr" ), stringIDToTypeID( "activeViewChanged" ) );
			executeAction( idNS, desc1, DialogModes.NO );
			]]> ).toString();
		
		/**		 
		 *	Update us when a document is closed 
		 */
		protected static const MESSAGE_TEXT_DOCUMENT_CLOSED:String = ( <![CDATA[
			var idNS = stringIDToTypeID( "$1" );
			var desc1 = new ActionDescriptor();
			desc1.putClass( stringIDToTypeID( "eventIDAttr" ), stringIDToTypeID( "closedDocument" ) );
			executeAction( idNS, desc1, DialogModes.NO );
			]]> ).toString();
		
		/**		 
		 *	Update us when the preferences change
		 */
		protected static const MESSAGE_TEXT_PREFERENCES_CHANGED:String = ( <![CDATA[
			var idNS = stringIDToTypeID( "$1" );
			var desc1 = new ActionDescriptor();
			desc1.putClass( stringIDToTypeID( "eventIDAttr" ), stringIDToTypeID( "preferencesChanged" ) );
			executeAction( idNS, desc1, DialogModes.NO );
			]]> ).toString();
		
		/**		 
		 *	Update us when the color settings change
		 */
		protected static const MESSAGE_TEXT_COLOR_SETTINGS_CHANGED:String = ( <![CDATA[
			var idNS = stringIDToTypeID( "$1" );
			var desc1 = new ActionDescriptor();
			desc1.putClass( stringIDToTypeID( "eventIDAttr" ), stringIDToTypeID( "colorSettingsChanged" ) );
			executeAction( idNS, desc1, DialogModes.NO );
			]]> ).toString();

		/**		 
		 *	Instructs Photoshop to either subscribe or unsubcribe from background color changes
		 */
		protected static const MESSAGE_TEXT_COLOR_BACKGROUND:String = ( <![CDATA[
			var idNS = stringIDToTypeID( "$1" );
			var desc1 = new ActionDescriptor();
			desc1.putClass( stringIDToTypeID( "eventIDAttr" ), stringIDToTypeID( "backgroundColorChanged" ) );
			executeAction( idNS, desc1, DialogModes.NO );
			]]> ).toString();
		
		/**		 
		 *	Instructs Photoshop to either subscribe or unsubscribe from foreground color changes
		 */
		protected static const MESSAGE_TEXT_COLOR_FOREGROUND:String = ( <![CDATA[
			var idNS = stringIDToTypeID( "$1" );
			var desc1 = new ActionDescriptor();
			desc1.putClass( stringIDToTypeID( "eventIDAttr" ), stringIDToTypeID( "foregroundColorChanged" ) );
			executeAction( idNS, desc1, DialogModes.NO );
			]]> ).toString();
				
		/**
		 *  Requests that Photoshop send us a JPG of the current document
		 */
		protected static const MESSAGE_TEXT_DOCUMENTS_SEND_THUMBNAIL:String = ( <![CDATA[
			var idNS = stringIDToTypeID( "sendDocumentThumbnailToNetworkClient" );
			var desc1 = new ActionDescriptor();
			desc1.putInteger( stringIDToTypeID( "width" ), $1 );
			desc1.putInteger( stringIDToTypeID( "height" ), $2 );
			desc1.putInteger( stringIDToTypeID( "format" ), $3 );
			executeAction( idNS, desc1, DialogModes.NO ); ]]> ).toString();
						
		/**
		 * Asks Photoshop to tell us what tool is currently selected
		 **/
		protected static const MESSAGE_TEXT_TOOLS_WHICH:String = ( <![CDATA[
			var ref = new ActionReference();
			ref.putProperty( charIDToTypeID( 'Prpr' ), charIDToTypeID( 'Tool' ) );
			ref.putEnumerated( charIDToTypeID( 'capp' ), charIDToTypeID( 'Ordn' ), charIDToTypeID( 'Trgt' ) );
			var desc = executeActionGet( ref );
			var theTool = desc.getEnumerationType( charIDToTypeID( 'Tool' ) );
			typeIDToStringID( theTool ); ]]> ).toString();
		
		/**
		 * Tells Photoshop to select a tool
		 **/
		protected static const MESSAGE_TEXT_TOOLS_SET:String = ( <![CDATA[
			var idslct = charIDToTypeID( "slct" );
			var desc2 = new ActionDescriptor();
			var idnull = charIDToTypeID( "null" );
			var ref2 = new ActionReference();
			var theTool = stringIDToTypeID( "$1" );
			ref2.putClass( theTool );
			desc2.putReference( idnull, ref2 );
			executeAction( idslct, desc2, DialogModes.NO ); ]]> ).toString();

		protected static const MESSAGE_TEXT_BRUSH_SIZE_SET:String = ( <![CDATA[			
			var idsetd = charIDToTypeID( "setd" );
			var desc2 = new ActionDescriptor();
			var idnull = charIDToTypeID( "null" );
			var ref2 = new ActionReference();
			var idBrsh = charIDToTypeID( "Brsh" );
			var idOrdn = charIDToTypeID( "Ordn" );
			var idTrgt = charIDToTypeID( "Trgt" );
			ref2.putEnumerated( idBrsh, idOrdn, idTrgt );
			desc2.putReference( idnull, ref2 );
			var idT = charIDToTypeID( "T   " );
			var desc3 = new ActionDescriptor();
			var idmasterDiameter = stringIDToTypeID( "masterDiameter" );
			var idPxl = charIDToTypeID( "#Pxl" );
			desc3.putUnitDouble( idmasterDiameter, idPxl, $1 );
			var idBrsh = charIDToTypeID( "Brsh" );
			desc2.putObject( idT, idBrsh, desc3 );
			executeAction( idsetd, desc2, DialogModes.NO ); ]]> ).toString();
		
		/**
		 *  @private
		 *  The PhotoshopConnection to use when sending messages
		 */
		private var pc:PhotoshopConnection;		
		
		/**
		 *  Constructor. Creates a new MessageDispatcher.
		 *
		 *  @langversion 3.0
		 *  @playerversion Flash 10
		 * 
		 *  @param pc The PhotoshopConnection to use when sending out messages		 
		 **/		
		public function MessageDispatcher(pc:PhotoshopConnection)
		{
			this.pc = pc;			
		}				
		
		/**
		 *  Dispatches a message instructing Photoshop to create a new document with the specified parameters. 
		 *
		 *  @langversion 3.0
		 *  @playerversion Flash 10
		 * 
		 *  @param transactionID You can override the default value if you wish to send the message with a custom transactionID. If you leave it as
		 * 	 					 -1, the message dispatcher will keep track of transactionIDs for you. It's recommended that you don't mix these two systems,
		 * 					  	 as it might lead to conflicts and unpredictable behavior.
		 *  @param width The width of the document to create, in pixels
		 *  @param height The height of the document to create, in pixels
		 *  @param ppi The density of the document to create, in pixels per inch
		 **/		
		public function createNewDocument(transactionID:int = -1, width:int = 640, height:int = 480, ppi:int = 72):void
		{
			var messageText:String = Util.replaceParams(MESSAGE_TEXT_DOCUMENTS_ADD,width, height, ppi);
			var message:TextMessage = generateTextMessage(messageText, transactionID);
			this.pc.encryptAndSendData(message.toStream());
		}
		
		
		/**
		 *  Dispatches a message instructing Photoshop to set the current tool to the toolType. See com.adobe.photoshop.tools.Tool for a list of toolTypes.
		 *
		 *  @langversion 3.0
		 *  @playerversion Flash 10
		 * 
		 *  @param transactionID You can override the default value if you wish to send the message with a custom transactionID. If you leave it as
		 * 	 					 -1, the message dispatcher will keep track of transactionIDs for you. It's recommended that you don't mix these two systems,
		 * 					  	 as it might lead to conflicts and unpredictable behavior.
		 *  @param toolType A String representation that specifies the type of tool you want selected. See the Tool class. 
		 **/
		public function setTool(transactionID:int = -1, toolType:String = Tool.TOOLS_BRUSH):void
		{
			var messageText:String = Util.replaceParams(MESSAGE_TEXT_TOOLS_SET,toolType);
			var message:TextMessage = generateTextMessage(messageText, transactionID);
			this.pc.encryptAndSendData(message.toStream());
		}
		
		/**
		 *  Instructs Photoshop to respond and tell us what tool is currently selected. 
		 *
		 *  @langversion 3.0
		 *  @playerversion Flash 10
		 * 
		 *  @param transactionID You can override the default value if you wish to send the message with a custom transactionID. If you leave it as
		 * 	 					 -1, the message dispatcher will keep track of transactionIDs for you. It's recommended that you don't mix these two systems,
		 * 					  	 as it might lead to conflicts and unpredictable behavior.
		 **/
		public function getTool(transactionID:int = -1):void
		{			
			var message:TextMessage = generateTextMessage(MESSAGE_TEXT_TOOLS_WHICH, transactionID);
			this.pc.encryptAndSendData(message.toStream());
		}
		
		/**
		 *  Dispatches a message requesting that  Photoshop return an image of the currently opened document. 
		 *
		 *  @langversion 3.0
		 *  @playerversion Flash 10
		 * 
		 *  @param transactionID You can override the default value if you wish to send the message with a custom transactionID. If you leave it as
		 * 	 					 -1, the message dispatcher will keep track of transactionIDs for you. It's recommended that you don't mix these two systems,
		 * 					  	 as it might lead to conflicts and unpredictable behavior.
		 *  @param width The width of the returned image, in pixels
		 *  @param height The height of the returned image, in pixels
		 *  @param format The format of the returned image, according to the Photoshop API. Leaving this as 1 will return a JPG. Passing in other values
		 * 				  may have unintended effects, or require that you parse the returned image yourself. 
		 **/	
		public function requestThumbnail(transactionID:int = -1, width:int = 640, height:int = 480, format:int = 1):void
		{
			var messageText:String = Util.replaceParams(MESSAGE_TEXT_DOCUMENTS_SEND_THUMBNAIL,width, height, format);
			var message:TextMessage = generateTextMessage(messageText, transactionID);
			this.pc.encryptAndSendData(message.toStream());
		}
				
		/** 
		 *  Creates a TextMessage, given a String and an optional transactionID.
		 *
		 *  @langversion 3.0
		 *  @playerversion Flash 10
		 * 
		 *  @param text	The payload of the TextMessage to be created
		 *  @param transactionID You can override the default value if you wish to send the message with a custom transactionID. If you leave it as
		 * 	 					 -1, the message dispatcher will keep track of transactionIDs for you. It's recommended that you don't mix these two systems,
		 * 					  	 as it might lead to conflicts and unpredictable behavior.
		 *  @return The generated TextMessage 		
		 **/	
		public static function generateTextMessage(text:String, transactionID:int = -1):TextMessage
		{
			var tm:TextMessage = new TextMessage();			
			tm.message = text;		
			
			// Should we use the internal transactionID, or use a passed-in value?
			if ( transactionID >= 0 )
			{
				tm.transactionID = transactionID;
			}
			else
			{
				tm.transactionID = TransactionManager.getInstance().getNextTransactionID();
			}
				
			return tm;
		}
		
		/**
		 * Takes an IMessage and sends it to Photoshop. The amount of time this function blocks will vary, depending on the 
		 *  type of message, and its size. It will send the data in an asynchronous manner, but some requisite steps, such as converting 
		 *  a Bitmap to a JPG, may take a short amount of time.
		 * 
		 * @param message The IMessage Object to send to Photoshop
		 **/
		public function sendMessage(message:IMessage):void
		{
			this.pc.encryptAndSendData(message.toStream());
		}
			
		/**
		 * Generates a TextMessage that can be sent to Photoshop, which will instruct photoshop to either subscribe or unsubscribe from the given Subscription.
		 * This function looks at the Subscription.subscriptionType to determine the exact Photoshop command that should be placed in the TextMessage. This function is used
		 * by the SubscriptionManager, and it's likely that you don't want to call it yourself. However, this function may be useful if you're creating your own Subscription pattern. 
		 * If that's the case, also look at the code/comments for the Subscription, SubscriptionEvent, and SubscriptionManager.
		 * 
		 * @langversion 3.0
		 * @playerversion Flash 10
		 * 
		 * @param subscription The Subscription that should be subscribed to or unsubscribed from
		 * @param subscribe A boolean. If true, the TextMessage will contain a command that tells Photoshop to susbcribe. If false, it contains an unsubscribe command.
		 * 
		 * @return A TextMessage containing the command for Photoshop		 
		 **/
		public static function generateTextMessageForSubscribingOrUnsubscribing(subscription:Subscription, subscribe:Boolean = true):TextMessage
		{
			var tm:TextMessage;			
			var commandString:String  = "";
			var subscriptionCommand:String;
			
			if ( subscribe )
			{
				subscriptionCommand = "networkEventSubscribe";
			}
			else
			{
				subscriptionCommand = "networkEventUnsubscribe";
			}						
						
			switch ( subscription.subscriptionType )
			{
				case Subscription.SUBSCRIPTION_EVENTS_COLOR_FOREGROUND_CHANGED:
					commandString = MESSAGE_TEXT_COLOR_FOREGROUND;					
					break;				
				case Subscription.SUBSCRIPTION_EVENTS_COLOR_BACKGROUND_CHANGED:
					commandString = MESSAGE_TEXT_COLOR_FOREGROUND;					
					break;
				case Subscription.SUBSCRIPTION_EVENTS_ACTIVEVIEW_CHANGED:
					commandString = MESSAGE_TEXT_ACTIVEVIEW_CHANGED;					
					break;
				case Subscription.SUBSCRIPTION_EVENTS_COLOR_SETTINGS_CHANGED:
					commandString = MESSAGE_TEXT_COLOR_SETTINGS_CHANGED;					
					break;
				case Subscription.SUBSCRIPTION_EVENTS_CURRENTDOCUMENT_CHANGED:
					commandString = MESSAGE_TEXT_CURRENTDOCUMENT_CHANGED;					
					break;
				case Subscription.SUBSCRIPTION_EVENTS_DOCUMENT_CHANGED:
					commandString = MESSAGE_TEXT_DOCUMENT_CHANGED;					
					break;
				case Subscription.SUBSCRIPTION_EVENTS_DOCUMENT_CLOSED:
					commandString = MESSAGE_TEXT_DOCUMENT_CLOSED;					
					break;
				case Subscription.SUBSCRIPTION_EVENTS_DOCUMENTVIEW_CREATED:
					commandString = MESSAGE_TEXT_DOCUMENTVIEW_CREATED;					
					break;
				case Subscription.SUBSCRIPTION_EVENTS_KEYBOARD_SHORTCUTS_CHANGED:
					commandString = MESSAGE_TEXT_KEYBOARD_SHORTCUTS_CHANGED;					
					break;
				case Subscription.SUBSCRIPTION_EVENTS_PREFERENCES_CHANGED:
					commandString = MESSAGE_TEXT_PREFERENCES_CHANGED;					
					break;
				case Subscription.SUBSCRIPTION_EVENTS_QUICK_MASK_STATE_CHANGED:
					commandString = MESSAGE_TEXT_QUICK_MASK_STATE_CHANGED;					
					break;
				case Subscription.SUBSCRIPTION_EVENTS_SCREEN_MODE_CHANGED:
					commandString = MESSAGE_TEXT_SCREEN_MODE_CHANGED;					
					break;
				case Subscription.SUBSCRIPTION_EVENTS_TOOL_CHANGED:
					commandString = MESSAGE_TEXT_TOOL_CHANGED;					
					break;
				default:
					break;
			}
			
			commandString = Util.replaceParams(commandString,subscriptionCommand);
			tm = generateTextMessage(commandString,subscription.transactionID);
			
			return tm;
		}
	}
}