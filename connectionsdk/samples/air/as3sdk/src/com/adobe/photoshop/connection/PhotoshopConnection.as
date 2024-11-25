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
	import com.adobe.photoshop.crypto.PBKDF2;
	import com.adobe.photoshop.events.PhotoshopEvent;
	import com.adobe.photoshop.events.PhotoshopMessageEvent;
	import com.adobe.photoshop.log.Logger;
	import com.adobe.photoshop.messages.ImageMessage;
	import com.adobe.photoshop.messages.Message;
	import com.adobe.photoshop.messages.TextMessage;
	import com.adobe.photoshop.subscriptions.SubscriptionManager;
	import com.hurlant.crypto.Crypto;
	import com.hurlant.crypto.symmetric.ICipher;
	import com.hurlant.crypto.symmetric.IVMode;
	import com.hurlant.util.Base64;
	import com.hurlant.util.Hex;
	import com.hurlant.util.der.Type;
	
	import flash.display.Bitmap;
	import flash.display.BitmapData;
	import flash.display.Loader;
	import flash.errors.EOFError;
	import flash.errors.IOError;
	import flash.errors.IllegalOperationError;
	import flash.events.AsyncErrorEvent;
	import flash.events.ErrorEvent;
	import flash.events.Event;
	import flash.events.EventDispatcher;
	import flash.events.HTTPStatusEvent;
	import flash.events.IOErrorEvent;
	import flash.events.ProgressEvent;
	import flash.events.SecurityErrorEvent;
	import flash.net.Socket;
	import flash.utils.ByteArray;	

	/**
	 *  Dispatched when a message is received from Photoshop.
	 *
	 *  @eventType com.adobe.photoshop.events.PhotoshopMessageEvent.MESSAGE_RECEIVED
	 *  
	 *  @langversion 3.0
	 *  @playerversion Flash 10
	 */
	[Event(name="MessageReceivedEvent", type="com.adobe.photoshop.events.PhotoshopMessageEvent")]	
	
	/**
	 *  Dispatched when a message is sent to Photoshop.
	 *
	 *  @eventType com.adobe.photoshop.events.PhotoshopMessageEvent.MESSAGE_SENT
	 *  
	 *  @langversion 3.0
	 *  @playerversion Flash 10
	 */
	[Event(name="MessageSentEvent", type="com.adobe.photoshop.events.PhotoshopMessageEvent")]
	
	/**
	 *  Dispatched when an error message is received from Photoshop.
	 *
	 *  @eventType com.adobe.photoshop.events.PhotoshopMessageEvent.ERROR_MESSAGE_RECEIVED
	 *  
	 *  @langversion 3.0
	 *  @playerversion Flash 10
	 */
	[Event(name="ErrorMessageReceivedEvent", type="com.adobe.photoshop.events.PhotoshopMessageEvent")]
	
	/**
	 *  Dispatched when there is progress in parsing a response or sending a message. Useful for large transfers.
	 *
	 *  @eventType com.adobe.photoshop.events.PhotoshopEvent.PROGRESS
	 *  
	 *  @langversion 3.0
	 *  @playerversion Flash 10
	 */	
	[Event(name="ProgressEvent", type="com.adobe.photoshop.events.PhotoshopEvent")]	
	
	/**
	 *  Dispatched when there's been an error in either data transfer, encryption, or connection management.
	 *
	 *  @eventType com.adobe.photoshop.events.PhotoshopEvent.ERROR
	 *  
	 *  @langversion 3.0
	 *  @playerversion Flash 10
	 */	
	[Event(name="ErrorEvent", type="com.adobe.photoshop.events.PhotoshopEvent")]
	
	/**
	 *  Dispatched should encryption or key derivation fail (or if initializing encryption from a provided key fails) 
	 *  
	 *  @eventType com.adobe.photoshop.events.PhotoshopEvent.ENCRYPTION_FAILED
	 * 
	 *  @langversion 3.0
	 *  @playerVersion Flash 10
	 **/
	[Event(name="EncryptionFailedEvent", type="com.adobe.photoshop.events.PhotoshopEvent")]
	
	/**
	 *  Dispatched when the connection has received an image
	 *
	 *  @eventType com.adobe.photoshop.events.PhotoshopEvent.IMAGE_RECEIVED
	 *  
	 *  @langversion 3.0
	 *  @playerversion Flash 10
	 */		
	[Event(name="ImageReceivedEvent", type="com.adobe.photoshop.events.PhotoshopEvent")]
	
	/**
	 *  Dispatched when the connection to Photoshop has been established
	 *
	 *  @eventType com.adobe.photoshop.events.PhotoshopEvent.CONNECTED
	 *  
	 *  @langversion 3.0
	 *  @playerversion Flash 10
	 */	
	[Event(name="ConnectedEvent", type="com.adobe.photoshop.events.PhotoshopEvent")]	
	
	/**
	 *  Dispatched when the connection is terminated
	 *
	 *  @eventType com.adobe.photoshop.events.PhotoshopEvent.DISCONNECTED
	 *  
	 *  @langversion 3.0
	 *  @playerversion Flash 10
	 */	
	[Event(name="DisconnectedEvent", type="com.adobe.photoshop.events.PhotoshopEvent")]
	
	/**
	 *  Dispatched when the key has been encrypted, and the connection is ready to send and receive data.
	 *
	 *  @eventType com.adobe.photoshop.events.PhotoshopEvent.ENCRYPTION_SUCCESS
	 *  
	 *  @langversion 3.0
	 *  @playerversion Flash 10
	 */
	[Event(name="encryptionSuccess", type="com.adobe.photoshop.events.PhotoshopEvent")]
	
	/**
	 * 	The Photoshop connection class sets up encryption and handles socket transmission of data and messages to and from Photoshop.
	 *
	 *  The first step is to make a connection to Photoshop using connect(), and then initialize the encryption. You can do this by deriving a new key
	 *   based on a password, using initEncryption(), or by using initEncryptionFromKey() and passing in a pre-derived key. This is convenient for mobile devices,
	 *   as deriving the key anew is a costly process (~3 seconds), and will block your UI from updating during the derivation process. The best way to do this is
	 *   to call initEncryption() once, then call getKey() and save it somewhere. You can provide that key in initEncryptionFromKey(), but note that your requests to Photoshop
	 *   will fail if the key is out of date (usually if the password changed).
	 * 
	 *  After successfully connecting and initializing encryption, you may send data to Photoshop, and receive results. There are a few classes that make this process easier.
	 *  
	 *  Look into the MessageDispatcher and SubscriptionManager classes, and read the documentation. You can also create Messages yourself using the official Photoshop SDK documentation,
	 *   and simply call encryptAndSendData() in this class, if you want lower-level control.    
	 *     
	 *  @langversion 3.0
	 *  @playerversion Flash 10
	 */
	public class PhotoshopConnection extends EventDispatcher 
	{
		/**
		 * The Photoshop API version that this code supports
		 **/
		public static const PHOTOSHOP_CONNECTION_API_VERSION:int = 1;		
		
		/**
		 * The SubscriptionManager that this connection should use when determining if a response is tied to a subscription.
		 * This must be set to the SubscriptionManager that you've attached your event listeners to.
		 **/
		public var subscriptionManager:SubscriptionManager;
				
		/**
		 * @private
		 * The agreed-upon salt
		 **/
		private const SALT:String = "Adobe Photoshop";
		
		/**
		 * @private
		 * The agreed-upon number of iterations in generating the key
		 **/
		private const NUM_ITERATIONS:int = 1000;
		
		/**
		 * @private
		 * The agreed-upon number of bytes in the derived key
		 **/
		private const NUM_BYTES:int = 24;
		
		/**
		 *  @private
		 *  Handles the loading for all received images from Photoshop 
		 */
		private var imageLoader:Loader;
		
		/**
		 *  @private
		 *  PBKDF2 is used for deriving the encryption key, necessary for encrypting and decrypting 
		 *  all data to and from Photoshop (with the exception of unencrypted responses--typically error messages regarding
		 *  encryption. 
		 */
		private var pbkdf2:PBKDF2;
		
		/**
		 *  @private
		 *  The derived key, used for encryption and decryption.
		 */
		private var key:String;
		
		/**
		 *  @private
		 *  An Initializion Vector used during encryption and decryption.
		 */
		private var iv:String;
		
		/**
		 *  @private
		 *  The state machine that is responsible for assembling incoming, raw data into a payload, which can then be decrypted
		 *  and parsed after being filled.  
		 **/
		private var state:Object;
		
		/**
		 *  @private
		 *  The resolvable hostname of the Photoshop server to connect to
		 **/
		private var serverName:String;
		
		/**
		 *  @private
		 *  The port of the Photoshop server to connect to
		 **/
		private var serverPort:int;	
		
		/**
		 *  @private
		 *  The socket object used for TCP communication
		 **/
		private var socket:Socket;			
		
		/**
		 *  Constructor. Creates a new Photoshop connection. No connection is attempted in this function, nor is the encryption set up.
		 *  Use the connect() and initEncryption() classes for these tasks.
		 *
		 *  @langversion 3.0
		 *  @playerversion Flash 10		 
		 **/				 
		public function PhotoshopConnection()
		{
			super();
		}
		
		/**
		 *  Opens a Photoshop data connection to Photoshop. You may call this function before you call initEncryption(),
		 *  but you will have to initialize the encryption before you can successfully communicate with Photoshop.
		 * 
		 *  @langversion 3.0
		 *  @playerversion Flash 10		
		 * 
		 *  @param serverName IP address or resolvable hostname of the server
		 *  @param serverPort Port to connect to. Default is 49494
		 * 
		 **/ 
		public function connect(serverName:String, serverPort:int = 49494):void
		{
			if ( serverName == "" || serverPort <= 0 ) return;
			
			this.serverName = serverName;
			this.serverPort = serverPort;
			
			if ( !this.socket )				
			{
				try
				{
					this.socket = new Socket();
				}
				catch ( se:SecurityError )
				{
					Logger.log(this,"Caught a SecurityError while trying to connect! KelvarConnection: 0x000A");
					handleFatalError(se);
				}
				this.socket.addEventListener(Event.CONNECT, onConnect);	
				this.socket.addEventListener(Event.CLOSE, onClose);
				this.socket.addEventListener(ErrorEvent.ERROR, onError);
				this.socket.addEventListener(IOErrorEvent.IO_ERROR, onIOError);
				this.socket.addEventListener(ProgressEvent.SOCKET_DATA, onSocketData);
				this.socket.addEventListener(SecurityErrorEvent.SECURITY_ERROR, onSecurityError);
			}
			try 
			{
				this.socket.connect(this.serverName, this.serverPort);	
			}
			catch (e:Error)
			{
				Logger.log(this,"Caught an Error while trying to connect. PhotoshopConnection: 0x0006"); 
				handleFatalError(e);
			}
		}
		
		/**
		 * @private
		 **/
		private function handleFatalError(e:Error):void
		{
			this.disconnect();
			var ke:PhotoshopEvent = new PhotoshopEvent(PhotoshopEvent.DISCONNECTED);
			ke.data = "There was an unrecoverable error, which caused the connection to attempt to close. PhotoshopConnection: 0x001E. " + e.message; 
			this.dispatchEvent(ke);
		}
		
		/**
		 * Disconnects the connection to Photoshop
		 * 
		 * @langversion 3.0
		 * @playerversion Flash 10		
		 **/ 
		public function disconnect():void
		{
			if ( this.socket && this.socket.connected )
			{
				try 
				{					
					this.socket.close();
				}
				catch ( ioe:IOError )
				{
					Logger.log(this,"Caught an IOError while trying to close socket...");
					if ( this.socket.connected )
					{
						Logger.log(this,"...Unable to disconnect socket! 0x0007");
					}
					else 					
					{
						Logger.log(this,"...Socket is closed. PhotoshopConnection: 0x0008");
						var ke:PhotoshopEvent = new PhotoshopEvent(PhotoshopEvent.DISCONNECTED);
						ke.data = "The connection is now closed. PhotoshopConnection: 0x001F. ";
						this.dispatchEvent(ke);
					}
				}
			}
		}		

		/**
		 *  Will both encrypt and send data to Photoshop. However, it doesn't ensure that your data is properly formatted.
		 *  If you're interested in creating and dispatching properly formatted messages, look into the MessageDispatcher class.  
		 * 
  		 *  @langversion 3.0
		 *  @playerversion Flash 10		 
		 *  
		 * @param data The payload to encrypt and send
		 * 
		 * @return Returns true or false based on whether the operations were successful
		 **/
		public function encryptAndSendData(data:ByteArray):Boolean
		{
			var succ:Boolean = encryptData(data);
			if ( succ )
			{
				succ = sendDatagram(data);	
			}
			return succ;	
		}
		
		/**
		 * Encrypts data such that Photoshop will be able to decrypt it. This won't ensure that your data is properly formatted, however. 
		 * If you're interested in creating and dispatching properly formatted messages, look into the MessageDispatcher class.  
		 * This function requires that the developer has already initialized the encryption.
		 * 
		 * @langversion 3.0
		 * @playerversion Flash 10		 
		 *  
		 * @param data The data to encrypt
		 * 
		 * @return Returns true or false based on whether the operations were successful
		 **/
		public function encryptData(data:ByteArray):Boolean
		{
			var succ:Boolean = true;
			var e:String;
			var ke:PhotoshopEvent;
			
			if ( !this.key || this.key.length == 0 ) 
			{
				e = "Can't send encrypted message--key doesn't exist! Did you initialize encryption?";		
				Logger.log(this,e + " PhotoshopConnection: 0x000F");
				ke = new PhotoshopEvent(PhotoshopEvent.ERROR);
				ke.data = e;
				this.dispatchEvent(ke);				
				return false;
			}
			
			if ( !this.socket || !this.socket.connected )
			{ 
				e = "Can't send encrypted message--not connected!";
				Logger.log(this,e + " PhotoshopConnection: 0x0010");
				ke = new PhotoshopEvent(PhotoshopEvent.ERROR);
				ke.data = e;
				this.dispatchEvent(ke);
				return false;
			}
			
			Logger.log(this,"Encrypting message. PhotoshopConnection: 0x0012");
			var keyBA:ByteArray = Hex.toArray(this.key);
			var aes:ICipher = Crypto.getCipher("des3-cbc", keyBA);			
			
			if ( aes is IVMode )
			{
				var ivmode:IVMode = aes as IVMode;
				var t:ByteArray = new ByteArray();
				t.writeInt(0);
				t.writeInt(0);
				t.writeInt(0);
				t.writeInt(1); // Force it to have > blockSize of 0s
				
				ivmode.IV = t;
				this.iv = Hex.fromArray(ivmode.IV);
			}			
			
			var time:Number = new Date().getTime();			
			try 
			{
				aes.encrypt(data);
			}
			catch (e:Error)
			{
				succ = false;
				e = "Error while encrypting data!";
				Logger.log(this,e + " PhotoshopConnection: 0x0019");
				ke = new PhotoshopEvent(PhotoshopEvent.ERROR);
				ke.data = e;
				this.dispatchEvent(ke);
			}
			finally 
			{
				time = new Date().getTime() - time;			
				Logger.log(this,"Encryption attempt complete. Took " + time + " ms. PhotoshopConnection: 0x0016");					
			}
						
			return succ;
		}
		
		/**
		 * Sends raw data to Photoshop, without encrypting it or wrapping it in a properly formatted message. 
		 * If you want to generate your own message and dispatch it, look at the MessageDispatcher class.
		 * If you want to encrypt your data before pushing it to Photoshop, you'll want to use encryptAndSendData(), or encryptData() 
		 *                          
		 * @langversion 3.0         
	 	 * @playerversion Flash 10
		 * 	
		 * @param data The payload of data
		 *
		 * @return Returns false if the operation failed immediately. 
		 **/
		public function sendDatagram(data:ByteArray):Boolean
		{
			var messageLength:int = 4 + data.length;
			var succ:Boolean = true;
			
			var e:String;
			var ke:PhotoshopEvent;
			
			this.socket.writeInt(messageLength);
			this.socket.writeInt(0); // Comm status 0 -- okay
			
			try 
			{
				this.socket.writeBytes(data);
				this.socket.flush();
			}
			catch (ioe:IOError)
			{
				succ = false;
				e = "Error while sending data to Photoshop!";
				Logger.log(this,e + " PhotoshopConnection: 0x001B. " + ioe.message);
				ke = new PhotoshopEvent(PhotoshopEvent.ERROR);
				ke.data = e;
				this.dispatchEvent(ke);	
			}
			catch (re:RangeError)
			{
				succ = false;
				e = "Error while sending data to Photoshop!";
				Logger.log(this,e + " PhotoshopConnection: 0x001C. " + re.message);
				ke = new PhotoshopEvent(PhotoshopEvent.ERROR);
				ke.data = e;
				this.dispatchEvent(ke);
			}
			
			if ( succ ) 
			{				
				Logger.log(this,"Message sent. PhotoshopConnection: 0x001D");
				var kme:PhotoshopMessageEvent = new PhotoshopMessageEvent(PhotoshopMessageEvent.MESSAGE_SENT);				
				this.dispatchEvent(kme);
			}
			
			return succ;
		}
		
		/**
		 * @private
		 * Removes event listeners and destroys the socket. Should only be called after the connection has been closed.
		 **/
		private function cleanUp():void
		{
			if ( !this.socket ) return;						
			this.socket.removeEventListener(Event.CONNECT, onConnect);
			this.socket.removeEventListener(Event.CLOSE, onClose);		
			this.socket.removeEventListener(ErrorEvent.ERROR, onError);
			this.socket.removeEventListener(IOErrorEvent.IO_ERROR, onIOError);
			this.socket.removeEventListener(ProgressEvent.SOCKET_DATA, onSocketData);
			this.socket.removeEventListener(SecurityErrorEvent.SECURITY_ERROR, onSecurityError);
			this.socket = null;
		}
		
		
		/**
		 * Derives the key necessary for all communication with Photoshop. This is a costly function. However, it must be called
		 *  before communication can be successful. It's recommended that you inform the user that initializing enryption will take some time before
		 *  calling this function.
		 * 
		 * @langversion 3.0
		 * @playerversion Flash 10
		 * 
		 * @param password The pre-shared password between Photoshop and your client.		 		
		 **/   
		public function initEncryption(password:String = "PhotoshopSDK"):void
		{
			this.pbkdf2 = new PBKDF2(password,this.SALT,this.NUM_ITERATIONS,this.NUM_BYTES);
			
			Logger.log(this,"Deriving key. PhotoshopConnection: 0x000C");
			
			try
			{
		
				var t:Number = new Date().getTime();			
				this.pbkdf2.deriveKey(onKeyStatus, onKeyResult); 				
				t = new Date().getTime() - t;
			}
			catch (error:Error)
			{
				// TODO - should we throw and error if this error's out for whatever reason?
				Logger.log(this,"Encryption Error. PhotoshopConnection: 0x0100");
				var ke:PhotoshopEvent = new PhotoshopEvent(PhotoshopEvent.ERROR);
				ke.data = error.toString();
				this.dispatchEvent(ke);
			}
			Logger.log(this,"Key derivation took " + t + " ms. PhotoshopConnection: 0x000D");
		}
		
		
		/**
		 *  Returns the derived key. This is handy if you want to save the key and provide the next time your application 
		 *   connects to Photoshop...this will bypass the costly key derivation subroutines inside of initEncryption().
		 *  See also initializeEncryptionFromKey();
		 * 
		 *  @return The derived key. Null if you haven't yet successfully called initEncryption()  
		 * 
		 *  @langversion 3.0
		 *  @playerversion Flash 10
		 **/
		public function getKey():String
		{			
			return this.key;
		}
		
		/**
		 *  Initializes encryption using a key that you provide. Useful if you want to store the key and speed up the
		 *   encryption process. You may call this function and then proceed to make calls to Photoshop without having to call 
		 *   initEncryption(). 
		 *  If you provide an incorrect key, you'll only know after your first attempt to communicate with Photoshop.
		 *  If you store a key value and the user changes his or her password, your saved key will be invalid!
		 * 
		 *  @param key The key you wish to use for encryption. Most easily obtained by calling initEncryption(), and then calling getKey() and saving it.
		 *  
		 *  @langversion 3.0
		 *  @playerversion Flash 10
		 **/
		public function initEncryptionFromKey(key:String):void
		{							
			onKeyResult(key);
		}
		
		/**
		 * @private
		 **/
		private function onKeyStatus(value:*):void
		{
			
		}
		
		/**
		 * @private
		 **/
		private function onKeyResult(key:String):void
		{
			var pe:PhotoshopEvent;
			if ( !key ) 
			{
				Logger.log(this,"Key derivation failed! PhotoshopConnection: 0xA002");
				pe = new PhotoshopEvent(PhotoshopEvent.ENCRYPTION_FAILED);				
				this.dispatchEvent(pe);
			}
			this.key = key;	
			Logger.log(this,"Encryption Success. PhotoshopConnection: 0x000E");
			pe = new PhotoshopEvent(PhotoshopEvent.ENCRYPTION_SUCCESS);
			pe.data = key;
			this.dispatchEvent(pe);
		}
		
		/**
		 * @private
		 **/
		private function onConnect(e:Event):void
		{			
			Logger.log(this,"Socket Connected. PhotoshopConnection: 0x0001");
			var ke:PhotoshopEvent = new PhotoshopEvent(PhotoshopEvent.CONNECTED);
			ke.data = e.toString();
			this.dispatchEvent(ke);
		}
		
		/**
		 * @private
		 **/
		private function onClose(e:Event):void
		{
			Logger.log(this,"Close socket. PhotoshopConnection: 0x0002");
			var ke:PhotoshopEvent = new PhotoshopEvent(PhotoshopEvent.DISCONNECTED);
			this.dispatchEvent(ke);
			this.cleanUp();
		}
		
		/**
		 * @private
		 **/	
		private function onError(ee:ErrorEvent):void
		{
			Logger.log(this,"Error. PhotoshopConnection: 0x0003");
			var ke:PhotoshopEvent = new PhotoshopEvent(PhotoshopEvent.ERROR);
			ke.data = ee.toString();
			this.dispatchEvent(ke);
		}
		
		/**
		 * @private
		 **/
		private function onIOError(ioee:IOErrorEvent):void
		{
			Logger.log(this,"IOErrorEvent. PhotoshopConnection: 0x0004");
			var ke:PhotoshopEvent = new PhotoshopEvent(PhotoshopEvent.ERROR);
			ke.data = ioee.toString();
			this.dispatchEvent(ke);
		}
		
		/**
		 * @private
		 **/
		private function onSocketData(pe:ProgressEvent):void
		{
			Logger.log(this,"SocketData. PhotoshopConnection: 0x0005");
			
			// Init state machine
			if ( this.state && this.state.stage == -1 )
			{
				// Handle parsing failure
				Logger.log(this,"Fatal error while parsing response from the server! PhotoshopConnection: 0x0013");
				return; 
			}
			
			if ( !this.state ) 
			{
				this.state = new Object();
				this.state.stage = 0;				
			}
			
			step();
		}
		
		/**
		 * @private
		 * 
		 * State machine for asynchronous server response handling. 
		 * (Responses may include large amounts of data.)
		 * 
		 * Execution begins by retrieving the message length and comm status, which are both unencrypted.
		 * The comm status, if set to anything but zero, indicates that the remainder of the message will be unencrypted.
		 * If the message is encrypted, a payload ByteArray will be filled with all available data from the socket.
		 * This step in the state machine can be called any time the socket receives data, and it will assume that all incoming data
		 *  is to complete the remainder of the message payload.
		 * Once the payload is filled (based on the transmitted value for the message length), it is decrypted, and the state machine reset 
		 *  for the next datagram.
		 * 
		 * @langversion 3.0
		 * @playerversion Flash 10
		 **/ 		 
		private function step():void
		{
			var debug:Object = this.state;			
			
			// Drain the socket of all available data
			while ( this.socket.bytesAvailable )
			{				
				switch ( this.state.stage )
				{
					case 0:
						response_ParseMessageLength();						
						this.state.stage++;
						break;
					case 1:
						response_ParseCommStatus();
						// TODO: If 0, encrypted...anything else, un-encrypted	
						this.state.stage++;
						break;
					case 2:
						response_FillPayload();						
						break;
					default:				
						break;
				}	
			}
			
			// Has the server sent all the payload data that it promised?
			// If so, we will enter the third and final stage of the response handling state machine.
			Logger.log(this,"bytes loaded so far: " + this.state.payload.bytesAvailable);
			Logger.log(this,"promised message length: " + this.state.messageLength + " (minus 4)"); 
			if ( this.state.payload.bytesAvailable >= this.state.messageLength - 4 )
			{
				if ( this.state.commStatus == 0 )
				{
					response_ParseEncryptedMessage();
				}
				else
				{
					response_ParseUnencryptedMessage();
				}
				
				// Handling of the message is complete. Reset the state machine, so the next datagram can be processed.
				this.state = null;
				
				// Ensure that we're done with the state machine.
				return;
			}
			
			// If the server hasn't sent all available payload data, the next SocketData event will resume at stage 2
		}				
		
		/**
		 * @private
		 * 
		 * One of the steps in the state machine
		 **/		
		private function response_ParseEncryptedMessage():void
		{
			Logger.log(this,"Entire message has been received. PhotoshopConnection 0x0014");
			var t:Number = new Date().getTime();
			
			// Decrypt the payload, which is stored in the state machine 
			var succ:Boolean = decryptMessage();
			
			t = new Date().getTime() - t;
			Logger.log(this,"Decryption attempt of the server datagram took " + t + " ms. EncryptedKelvarConnection: 0x0015");
							
			if ( succ )
			{				
				var apiVersion:int = this.state.payload.readInt();
				var transactionID:int = this.state.payload.readInt();
				var messageType:int = this.state.payload.readInt();
				
				Logger.log(this,"MessageType[0x0202]: " + messageType + " transID: " + transactionID);
				var messageString:String;
				var tm:TextMessage;
				var kme:PhotoshopMessageEvent;
				
				switch ( messageType )
				{
					case Message.MESSAGE_ERROR_TEXT:	
						// Create the Message		
						messageString = this.state.payload.readUTFBytes(this.state.payload.length - 12);
						
						Logger.log(this,"Message[0x0500]: " + messageString);
						
						tm = new TextMessage();
						tm.message = messageString;
						tm.apiVersion = apiVersion;
						tm.transactionID = transactionID;
						tm.messageType = messageType;
						
						// If this message has a tranasctionID that the SubscriptionManager is using, let the SubscriptionManager handle all further parsing
						if ( this.subscriptionManager && this.subscriptionManager.isSubcribedByTransactionID(tm.transactionID) )
						{								
							Logger.log(this, "This Message.MESSAGE_ERROR_TEXT belongs to an active subscription! Handing it off to the SubscriptionManager. PhotoshopConnection:0x0020");
							this.subscriptionManager.handleSubscriptionTextMessage(tm);
						}
						else 
						{						
							// Dispatch the message
							kme = new PhotoshopMessageEvent(PhotoshopMessageEvent.ERROR_MESSAGE_RECEIVED);
							kme.message = tm;
							this.dispatchEvent(kme);
						}
						break;
					
					case Message.MESSAGE_TYPE_TEXT:	
						// Create the Message
						messageString = this.state.payload.readUTFBytes(this.state.payload.length - 12);
						
						Logger.log(this,"Message[0x0200]: " + messageString);
						
						tm = new TextMessage();
						tm.message = messageString;
						tm.apiVersion = apiVersion;
						tm.transactionID = transactionID;
						tm.messageType = messageType;						
											
						// If this message has a tranasctionID that the SubscriptionManager is using, let the SubscriptionManager handle all further parsing
						if ( this.subscriptionManager && this.subscriptionManager.isSubcribedByTransactionID(tm.transactionID) )
						{								
							Logger.log(this, "This Message.MESSAGE_TYPE_TEXT belongs to an active subscription! Handing it off to the SubscriptionManager. PhotoshopConnection:0x0021");
							this.subscriptionManager.handleSubscriptionTextMessage(tm);
						}
						else
						{
							// Dispatch the message						
							kme = new PhotoshopMessageEvent(PhotoshopMessageEvent.MESSAGE_RECEIVED);
							kme.message = tm;
							this.dispatchEvent(kme);
						}
						break;
					
					case Message.MESSAGE_TYPE_IMAGE:
						var imageType:int = this.state.payload.readByte();																	

						if ( imageType == ImageMessage.IMAGE_TYPE_JPG )
						{
							// Will hand it off to the SubscriptionManager if appropriate, or dispatch an event
							loadJPGFromByteArray(this.state.payload, transactionID);									
						}						
						break;
					
					default:
						break;
				}
				
			}

			// Event dispatching in the case of un-decryptable data is handled by the try {} catch {} block that surrounds
			//  the ICipher Object, so that the listeners can obtain pertinent error information. This includes cases where the 
			//  length of the payload exceeds what the server promise to deliver. Thus, handling the !succ case isn't necessary.  			
		}
		
		
		/**
		 * @private
		 **/		
		private var loaderTransactionID:int = -1;
		
		/**
		 * @private
		 **/
		private function loadJPGFromByteArray(ba:ByteArray, transactionID:int):void
		{
			loaderTransactionID = transactionID;
			
			imageLoader = new Loader();
			imageLoader.contentLoaderInfo.addEventListener(Event.COMPLETE,onLoaderComplete);
			imageLoader.contentLoaderInfo.addEventListener(AsyncErrorEvent.ASYNC_ERROR,onAsyncError);	
			imageLoader.contentLoaderInfo.addEventListener(Event.UNLOAD,onUnload);
			imageLoader.contentLoaderInfo.addEventListener(Event.INIT,onLoaderInit);
			imageLoader.contentLoaderInfo.addEventListener(IOErrorEvent.IO_ERROR,onLoaderIOError);			
			imageLoader.contentLoaderInfo.addEventListener(ProgressEvent.PROGRESS,onLoaderProgress);
			imageLoader.contentLoaderInfo.addEventListener(SecurityErrorEvent.SECURITY_ERROR,onSecurityError);
			
			var imageBytes:ByteArray = new ByteArray();
			
			ba.readBytes(imageBytes,0,ba.bytesAvailable);
			
			try 
			{
				imageLoader.loadBytes(imageBytes);				
			}
			catch (ioe:IOError)
			{
				Logger.log(this,"Caught io error");
				cleanUp();
			}
			catch (se:SecurityError)
			{
				Logger.log(this,"Caught security error");
				cleanUp();
			}
			catch (ioe:IllegalOperationError)
			{
				Logger.log(this,"Caught illegal operation error");
				cleanUp();
			}
			
			function onSecurityError(se:SecurityErrorEvent):void
			{
				Logger.log(imageLoader,"on loader security error");	
				cleanUp();
			}
			
			function onLoaderProgress(pe:ProgressEvent):void
			{
				Logger.log(imageLoader,"on loader progress");
				var ke:PhotoshopEvent = new PhotoshopEvent(PhotoshopEvent.PROGRESS);
				ke.data = "Progress while loading a JPG response. PhotoshopConnection: 0x0022";
				dispatchEvent(ke);
			}
			
			function onUnload(e:Event):void
			{
				Logger.log(imageLoader,"Image loader unloaded");
				cleanUp();
			}
			
			function cleanUp():void
			{
				imageLoader.contentLoaderInfo.removeEventListener(Event.COMPLETE,onLoaderComplete);
				imageLoader.contentLoaderInfo.removeEventListener(AsyncErrorEvent.ASYNC_ERROR,onAsyncError);	
				imageLoader.contentLoaderInfo.removeEventListener(Event.UNLOAD,onUnload);
				imageLoader.contentLoaderInfo.removeEventListener(Event.INIT,onLoaderInit);
				imageLoader.contentLoaderInfo.removeEventListener(IOErrorEvent.IO_ERROR,onLoaderIOError);			
				imageLoader.contentLoaderInfo.removeEventListener(ProgressEvent.PROGRESS,onLoaderProgress);
				imageLoader.contentLoaderInfo.removeEventListener(SecurityErrorEvent.SECURITY_ERROR,onSecurityError);								
			}			
			function onAsyncError(asee:AsyncErrorEvent):void
			{
				Logger.log(imageLoader,"Loader async error");
				cleanUp();
			}
			
		    function onLoaderInit(event:Event):void
			{
				Logger.log(imageLoader,"Loader init");				
			}
						
			function onLoaderIOError(ioe:IOErrorEvent):void
			{
				Logger.log(imageLoader,"Loader IO error");
				cleanUp();
			}			
			
			function onLoaderComplete(e:Event):void
			{
				Logger.log(imageLoader,"on loader complete");	
				if ( imageLoader.content is Bitmap )
				{
					Logger.log(imageLoader,"Bitmap loaded");			
					Logger.log(imageLoader,"Width:" + imageLoader.content.width + ", height: " + imageLoader.content.height);
					var bd:BitmapData = (imageLoader.content as Bitmap).bitmapData.clone();

					var im:ImageMessage = new ImageMessage();
					im.bitmapData = bd;
					im.transactionID = loaderTransactionID;
					im.messageType = com.adobe.photoshop.messages.Message.MESSAGE_TYPE_IMAGE;
					
					if ( subscriptionManager && subscriptionManager.isSubcribedByTransactionID(im.transactionID) )
					{								
						Logger.log(this, "This ImageMessage belongs to an active subscription! Handing it off to the SubscriptionManager. PhotoshopConnection:0x0022");	
					}
					else
					{
						// Dispatch the message
						var kme:PhotoshopMessageEvent = new PhotoshopMessageEvent(PhotoshopMessageEvent.MESSAGE_RECEIVED);
						kme.message = im;
						dispatchEvent(kme);
					}
					
					loaderTransactionID = -1;
				}
				cleanUp();
			}
		}
		
		/**
		 * @private
		 * 
		 * Useful for logging the raw contents of a ByteArray.
		 * 
		 * @langversion 3.0
		 * @playerversion Flash 10
		 * 
		 * @param ba The ByteArray to log
		 **/
		private function printBA(ba:ByteArray):void
		{
			var position:int = ba.position;
			ba.position = 0;
			var s:String = "";
			
			Logger.log(this,"\n\nFull byte array. (Bytes available: " + ba.bytesAvailable + ")");
			while ( ba.bytesAvailable )
			{
				s += ba.readUnsignedByte() + ":";
			}			
			Logger.log(this,s);
			
			ba.position = position;			
			Logger.log(this,"\n\nByte array with position offset. (Bytes available: " + ba.bytesAvailable + ")");
			s = "";
			while ( ba.bytesAvailable )
			{
				s += ba.readUnsignedByte() + ":";
			}
			Logger.log(this,s + "\n\n");
			
			ba.position = position;
			
		}
		
		/**
		 * @private
		 **/
		private function decryptMessage():Boolean
		{
			var succ:Boolean = true;
			var ba:ByteArray = new ByteArray();												
			var keyBA:ByteArray = Hex.toArray(this.key);			
			ba = this.state.payload;
			var aes:ICipher = Crypto.getCipher("des3-cbc", keyBA);
			Logger.log(this,"AES block size is: " + aes.getBlockSize());
			
			if (aes is IVMode)
			{
				var ivmode:IVMode = aes as IVMode;
				var t:ByteArray = new ByteArray();
				t.writeInt(0);
				t.writeInt(0);				
				t.writeInt(0);
				t.writeInt(1); // force it to have > blockSize of 0s
				ivmode.IV = t;
			}
			
			try 
			{
				aes.decrypt(ba);	
			}
			catch (e:Error)
			{
				var eMessage:String = "Error while decrypting a message from the server!";					
				Logger.log(this, eMessage + " PhotoshopConection: 0x0017. " + e.toString());
				var ke:PhotoshopEvent = new PhotoshopEvent(PhotoshopEvent.ERROR);
				ke.data = e;
				this.dispatchEvent(ke);
				
				succ = false;
			}
			
			return succ;
		}
		
		/**
		 * @private
		 **/		
		private function keyToByteArray(value:String):ByteArray
		{
			return Hex.toArray(Hex.fromString(value));
		}
		
		/**
		 * @private
		 **/
		private function valueToByteArray(value:String):ByteArray
		{
			return Base64.decodeToByteArray(value);
		}
		
		/**
		 * @private
		 * 
		 * One of the steps in the state machine
		 **/
		private function response_FillPayload():Boolean
		{
			var succ:Boolean = true;
			if ( !this.state.payload ) 
			{
				this.state.payload = new ByteArray();				
			}
									
			var p:ByteArray = this.state.payload;
			var len:int = this.state.messageLength; 
			Logger.log(this,"The socket has " + this.socket.bytesAvailable + " bytes available.");
			
			var offset:int = 0;
			if ( this.state.payload.length > 0) 
			{
				offset = this.state.payload.length;
			}
			
			this.socket.readBytes(this.state.payload,offset,this.socket.bytesAvailable);
			return succ;
		}
		
		/**
		 * @private
		 * 
		 * One of the steps in the state machine
		 **/
		private function response_ParseMessageLength():Boolean
		{
			var succ:Boolean = true;
			try 
			{
				this.state.messageLength = this.socket.readInt();				
			}
			catch (e:Error)
			{
				succ = false;
			}
			return succ;
		}

		/**
		 * @private 
		 * 
		 * One of the steps in the state machine
		 **/
		private function response_ParseCommStatus():Boolean
		{
			var succ:Boolean = true;
			try 
			{
				this.state.commStatus = this.socket.readInt();				
			}
			catch (e:Error)
			{
				succ = false;
			}
			return succ;
		}
		
		/**
		 * @private
		 * 
		 * One of the steps in the state machine
		 **/
		private function response_ParseAPIVersion():Boolean
		{
			var succ:Boolean = true;
			try 
			{
				this.state.apiVersion = this.socket.readInt();
			}
			catch (e:Error)
			{
				succ = false;
			}
			return succ;
		}
		
		/**
		 * @private
		 * 
		 * One of the steps in the state machine
		 **/
		private function response_ParseTransactionID():Boolean
		{
			var succ:Boolean = true;
			try 
			{
				this.state.transactionID = this.socket.readInt();
			}
			catch (e:Error)
			{
				succ = false;
			}
			return succ;
		}
			
		/**
		 * @private
		 * 
		 * One of the steps in the state machine
		 **/
		private function response_ParseMessageType():Boolean
		{
			var succ:Boolean = true;
			try 
			{
				this.state.messageType = this.socket.readInt();
			}
			catch (e:Error)
			{
				succ = false;
			}
			return succ;
		}

		/**
		 * @private
		 * 
		 * One of the steps in the state machine
		 */
		private function response_ParseUnencryptedMessage():Boolean
		{
			var succ:Boolean = true;
					
			Logger.log(this,"Received an unencrypted response. PhotoshopConnection: 0x0018");
			
			var APIVersion:int = this.state.payload.readInt();
			var transactionID:int = this.state.payload.readInt();
			var messageType:int = this.state.payload.readInt();
			try 
			{
				var m:String = this.state.payload.readUTFBytes(this.state.messageLength - 16);
			}
			catch ( e:Error )
			{
				// TODO: Handle error
			}
				
			var tm:TextMessage = new TextMessage();
			tm.message = m;
			tm.transactionID = transactionID;
			tm.messageType = messageType;
			tm.apiVersion = APIVersion;
			
			var kme:PhotoshopMessageEvent = new PhotoshopMessageEvent(PhotoshopMessageEvent.ERROR_MESSAGE_RECEIVED);
			kme.message = tm;
			this.dispatchEvent(kme);
			
			return succ;		
		}
		
		/**
		 * @private
		 **/
		private function onSecurityError(see:SecurityErrorEvent):void
		{
			Logger.log(this,"SecurityErrorEvent. PhotoshopConnection: 0x0009");
		}				
	}
}