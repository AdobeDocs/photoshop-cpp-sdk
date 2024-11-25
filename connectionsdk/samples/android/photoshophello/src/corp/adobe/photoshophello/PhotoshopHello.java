/**************************************************************************
	ADOBE SYSTEMS INCORPORATED
	 Copyright 2010 Adobe Systems Incorporated
	 All Rights Reserved.

	NOTICE:  Adobe permits you to use, modify, and distribute this file
	in accordance with the terms of the Adobe license agreement accompanying
	it.  If you have received this file from a source other than Adobe, then
	your use, modification, or distribution of it requires the prior written
	permission of Adobe.
**************************************************************************/

/**  Filename: PhotoshopHello.java
	@author Thomas Ruark, Photoshop Engineering, Adobe Systems Incorporated
*/

package corp.adobe.photoshophello;

import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.net.Socket;
import android.app.Activity;
import android.os.Bundle;
import android.widget.EditText;
import android.widget.Button;
import android.view.View;

/**
 * Android version for sending and receiving JavaScript to a Photoshop server.
 * The "Hello, World!" example for controlling Photoshop via another device.
 * NOTE: This is the simple example. See other examples for better Socket 
 * handling around large amounts of data being sent and better error handling.
*/
public class PhotoshopHello extends Activity {
	
	/** Name of the server to connect */
	private EditText mServerNameText = null;
	
	/** Port of the server to use */
	private final int SERVER_PORT = 49494;
	
	/** Password of the server to use */
	private EditText mServerPasswordText = null;
	
	/** JavaScript that will be sent to Photoshop */
	private EditText mInputText = null;
	
	/** Text that comes back from Photoshop */
	private EditText mOutputText = null;
	
	/** Errors and messages that occur */
	private EditText mErrorText = null;
	
	/** Send the JavaScript with this button */
	private Button mConnectBtn = null;
	
	/** The Socket we use to connect to Photoshop */
    private Socket mPhotoshopSocket = null;
    
    /** Output stream for the socket */
    private DataOutputStream mOutputStream = null;
    
    /** Input stream for the socket */
    private DataInputStream mInputStream = null;
    
    /** have we already connected ? */
    private boolean mIsConnected = false;

    /**
     * The types that Photoshop will receive and send.
     * This example only deals with JavaScript
     */    
    @SuppressWarnings("unused")
	private static final int ILLEGAL_TYPE = 0;
    private static final int ERRORSTRING_TYPE = 1;
    private static final int JAVASCRIPT_TYPE = 2;
    @SuppressWarnings("unused")
	private static final int IMAGE_TYPE = 3;
    @SuppressWarnings("unused")
	private static final int PROFILE_TYPE = 4;
    @SuppressWarnings("unused")
	private static final int DATA_TYPE = 5;
    
    /**
     * Current version of the protocol used by Photoshop
     */
    private static final int PROTOCOL_VERSION = 1;
    
    /** version + id + type */ 
	private static final int PROTOCOL_LENGTH = 4 + 4 + 4;
    private static final int COMM_LENGTH = 4;
    private static final int NO_COMM_ERROR = 0;

	/**
     * Each message sent and received will have an ID.
     * If you send more than one make sure your ID's match.
     */
    private int mTransactionID = 0;
    
    /** messages are encrypted, helper class to encrypt and decrypt messages */
    private EncryptDecrypt mEncryptDecrypt = null;
	
    /** Called when the activity is first created. 
     * Connect my local variables to the UI widgets in main.xml.
     */
    @Override
    public void onCreate(Bundle savedInstanceState) {
    	super.onCreate(savedInstanceState);
    	setContentView(R.layout.main);
        mServerNameText = (EditText) findViewById(R.id.serverNameText);
        mServerPasswordText = (EditText) findViewById(R.id.serverPasswordText);
        mInputText = (EditText) findViewById(R.id.inputText);
        mOutputText = (EditText) findViewById(R.id.outputText);
        mErrorText = (EditText) findViewById(R.id.errorText);
        mConnectBtn = (Button) findViewById(R.id.connectBtn);
    } /* end onCreate */

    /** Called when the activity is gone.
     * Get rid of our background thread.
     */
    @Override
    public void onDestroy() {
        super.onDestroy();
		mIsConnected = false;
		try {
			mPhotoshopSocket.close();
			mOutputStream.close();
			mInputStream.close();
        } 
		catch (Exception e) {
            ;
        }
		mPhotoshopSocket = null;
		mOutputStream = null;
		mInputStream = null;
    }
    
    /**
     * Called when the connect button is clicked connect if not connected
     * and disconnect if we are connected. Switch the text on the UI.
     * Report errors in the error output UI.
     * @param v - the View, not used
     */
    public void connectBtnClick(View v) {
		try {
			if ( ! mIsConnected ) {
				String n = mServerNameText.getText().toString();
				mPhotoshopSocket = new Socket(n, SERVER_PORT);
				mOutputStream = new DataOutputStream(mPhotoshopSocket.getOutputStream());
				mInputStream = new DataInputStream(mPhotoshopSocket.getInputStream());
				mEncryptDecrypt = new EncryptDecrypt(mServerPasswordText.getText().toString());
				mConnectBtn.setText("Disconnect");
				mErrorText.setText("Connected");
				mIsConnected = true;
			} else {
				mIsConnected = false;
				mPhotoshopSocket.close();
				mOutputStream.close();
				mInputStream.close();
				mPhotoshopSocket = null;
				mOutputStream = null;
				mInputStream = null;
				mConnectBtn.setText("Connect");
				mErrorText.setText("Disconnected");
			}
        } 
		catch (Exception e) {
            mErrorText.setText( e.toString() );
        }
    }
    
    
    /**
     * Called when the send button is clicked.
     * Take the inputText and send it to Photoshop
     * Place the returned text in the outputText area.
     * @param v - the View, not used
     */
    public void sendBtnClick(View v) {
		String userInput = mInputText.getText().toString();
		if (userInput.compareTo("?") == 0) {
			showCommands();
		} else if (userInput.compareTo("q") == 0) {
			System.exit(1);
		} else {
			byte [] returnBytes = processJavaScript(userInput, mOutputStream, mInputStream);
			try {
				String returnString = new String(returnBytes, "UTF-8");
				mOutputText.setText(returnString);
			}
			catch(Exception e) {
				mErrorText.setText(e.toString());
			}
		}
    }

    /** 
	 * Send JavaScript to Photoshop and get the result.
	 * @param inJavaScript string from the UI to process
	 * @param inOutputStream output stream on the socket
	 * @param inInputStream input stream on the socket
	 * @return UTF-8 byte string coming back from Photoshop
	 */
    private byte [] processJavaScript(String inJavaScript, DataOutputStream inOutputStream, DataInputStream inInputStream) {

		byte [] outputStr = null;

		try {

			/* WRITE */
			byte [] strBytes = inJavaScript.getBytes("UTF-8");
		    byte [] allBytes = new byte[PROTOCOL_LENGTH + strBytes.length + 1];

		    int byteIndexer = 0;
		    
	        allBytes[byteIndexer++] = (byte)(PROTOCOL_VERSION >>> 24);
	        allBytes[byteIndexer++] = (byte)(PROTOCOL_VERSION >>> 16);
	        allBytes[byteIndexer++] = (byte)(PROTOCOL_VERSION >>> 8);
	        allBytes[byteIndexer++] = (byte)(PROTOCOL_VERSION);
	            
	        int messageID = mTransactionID++;
	            
	        allBytes[byteIndexer++] = (byte)(messageID >>> 24);
	        allBytes[byteIndexer++] = (byte)(messageID >>> 16);
	        allBytes[byteIndexer++] = (byte)(messageID >>> 8);
	        allBytes[byteIndexer++] = (byte)(messageID);
	            
	        allBytes[byteIndexer++] = (byte)(JAVASCRIPT_TYPE >>> 24);
	        allBytes[byteIndexer++] = (byte)(JAVASCRIPT_TYPE >>> 16);
	        allBytes[byteIndexer++] = (byte)(JAVASCRIPT_TYPE >>> 8);
	        allBytes[byteIndexer++] = (byte)(JAVASCRIPT_TYPE);
	            
	        for (int i = 0; i < strBytes.length; i++) {
	          	allBytes[byteIndexer++] = strBytes[i];
	        }

		    allBytes[byteIndexer++] = (byte)0x0a; // this is \n

		    byte [] encryptedBytes = mEncryptDecrypt.encrypt(allBytes);
		    
		    // Don't encrypt the communication status so we add that length
		    int messageLength = COMM_LENGTH + encryptedBytes.length;

		    inOutputStream.writeInt(messageLength);
		    inOutputStream.writeInt(NO_COMM_ERROR);
		    inOutputStream.write(encryptedBytes, 0, encryptedBytes.length);


		    /* READ */
		    messageLength = inInputStream.readInt();
		    int comStatus = inInputStream.readInt();
		    
		    /* communication status of NO_COMM_ERROR (0) is not encrypted just read out the string */
		    if (comStatus != NO_COMM_ERROR) {

		    	inInputStream.readInt(); // skipping version check
		    	inInputStream.readInt(); // skipping transaction check
		    	int messageType = inInputStream.readInt();
		    	if (messageType == JAVASCRIPT_TYPE || messageType == ERRORSTRING_TYPE) {
		    		messageLength -= PROTOCOL_LENGTH;
		    		outputStr = new byte[messageLength + 1];
		    		inInputStream.read(outputStr, 0, messageLength);
		    	} else {
		    		mErrorText.setText("Unknown type: " + messageType);
			    }

		    	int inAvailable = inInputStream.available();
		    	for (int i = 0; i < inAvailable; i++) {
		    		inInputStream.readByte();
		    	}

		    	
			/* communication status of 0 is encrypted message */
		    } else {
		    	
		    	messageLength = messageLength - COMM_LENGTH;

		    	// wait for rest of the message
		    	long to = System.currentTimeMillis() + 2000;
		    	while ((inInputStream.available() < messageLength) && (System.currentTimeMillis() < to)) {
		    		to = to + 1;
		    	}
	    				
		    	byte[] messageBytes = new byte[messageLength];
		    	int readThisMany = inInputStream.read(messageBytes, 0, messageLength);
		    	if (readThisMany != messageLength) {
		    		mErrorText.setText("Didn't get the entire message, must have timed out!");
		    	}
		    	byte [] decryptedBytes = mEncryptDecrypt.decrypt(messageBytes);
	    		int messageIndexer = 0;
	    		int messageVersion = (decryptedBytes[messageIndexer++] << 24)
	    		                     + ((decryptedBytes[messageIndexer++] & 0xFF) << 16)
	    		                     + ((decryptedBytes[messageIndexer++] & 0xFF) << 8)
	    		                     + (decryptedBytes[messageIndexer++] & 0xFF);
	    		if (messageVersion != PROTOCOL_VERSION) {
	    			mErrorText.setText("Didn't get the entire message, must have timed out!");
	    		}
	    		messageID = (decryptedBytes[messageIndexer++] << 24)
	    		            + ((decryptedBytes[messageIndexer++] & 0xFF) << 16)
	    		            + ((decryptedBytes[messageIndexer++] & 0xFF) << 8)
	    		            + (decryptedBytes[messageIndexer++] & 0xFF);

	    		int messageType = (decryptedBytes[messageIndexer++] << 24)
	    		                  + ((decryptedBytes[messageIndexer++] & 0xFF) << 16)
	    		                  + ((decryptedBytes[messageIndexer++] & 0xFF) << 8)
	    		                  + (decryptedBytes[messageIndexer++] & 0xFF);

	    		if (messageType == JAVASCRIPT_TYPE || messageType == ERRORSTRING_TYPE) {
	    			outputStr = new byte[decryptedBytes.length - messageIndexer];
	    			for (int i = 0; i < outputStr.length; i++) {
	    				outputStr[i] = decryptedBytes[messageIndexer++];
	    			}
	    		} else {
	    			mErrorText.setText("We don't do other message types: " + messageType);
	    		}
		    }
		}
		catch (Exception e) {
			System.out.println("Exception processJavaScript: " + e.toString());
		}
		return outputStr;
    }

    /**
     * Show all the commands this takes 
     */
    private void showCommands() {
		mErrorText.setText("q for quit\n? for this list");
    }

} /* end public class PhotoshopHello extends Activity */
// end PhotoshopHello.java
