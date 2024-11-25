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

/**  Filename: MessageProcessor.java
	@author Thomas Ruark, Photoshop Engineering, Adobe Systems Incorporated
*/

package corp.adobe.photoshopevents;

import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.IOException;
import java.net.Socket;
import java.net.UnknownHostException;
import java.net.InetSocketAddress;
import java.security.InvalidAlgorithmParameterException;
import java.security.InvalidKeyException;
import java.security.NoSuchAlgorithmException;

import javax.crypto.NoSuchPaddingException;

/**
 * Because I am asking for messages to come to me when Photoshop
 * changes something I need to keep a thread running to process them.
 */
class MessageProcessor extends Thread {

	/** The main activity that started this thread */
	private PhotoshopEvents mMainActivity = null;
	
	/** The Socket we use to connect to Photoshop */
    private Socket mPhotoshopSocket = null;
    
	/** Port of the server to use, not an option to change. */
	final private int SERVER_PORT = 49494;
	
    /** Output stream for the socket */
    private DataOutputStream mOutputStream = null;
    
    /** Input stream for the socket */
    private DataInputStream mInputStream = null;
    
    /** have we already connected ? */
    public boolean mIsConnected = false;
    
    /** encode/decode via cipher */
    private EncryptDecrypt mEncryptDecrypt = null;

    /**
     * The types that Photoshop will receive and send.
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
    
    /** no communication error */
    private static final int NO_COMM_ERROR = 0;
    
    /** bytes in the length of communication status */
    private static final int COMM_LENGTH = 4;
    
    /** bytes in the length of length */
    private static final int LENGTH_LENGTH = 4;

    /** length of version + id + type */;
    private static final int PROTOCOL_LENGTH = 4 + 4 + 4;
    
    /**
     * Current version of the protocol used by Photoshop
     */
    private static final int PROTOCOL_VERSION = 1;
    
    /**
     * Each message sent and received will have an ID.
     * If you send more than one make sure your ID's match.
     */
    private int mTransactionID = 0;

    /** Keeps the main thread running */
    private boolean mKeepRunning = false;
 
    MessageProcessor(PhotoshopEvents inActivity, 
    		         String inServerName, 
    		         String inPassword)
    		         
    	throws UnknownHostException, IOException, InvalidAlgorithmParameterException,
    	NoSuchPaddingException, NoSuchAlgorithmException, InvalidKeyException {
    	
   		mMainActivity = inActivity;
   		InetSocketAddress ia = new InetSocketAddress(inServerName, SERVER_PORT);
   		mPhotoshopSocket = new Socket();
   		mPhotoshopSocket.connect(ia, 2000);
   		mOutputStream = new DataOutputStream(mPhotoshopSocket.getOutputStream());
   		mInputStream = new DataInputStream(mPhotoshopSocket.getInputStream());
   		mEncryptDecrypt = new EncryptDecrypt(inPassword);
   		mKeepRunning = true;
        mIsConnected = true;
    }

    
    /** 
	 * Send JavaScript to Photoshop
	 * @param inJavaScript JavaScript string to send
	 */
    public void sendJavaScript(String inJavaScript) {
    	try {
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

    		// we don't encrypt the communication status so we add that length
			int messageLength = COMM_LENGTH + encryptedBytes.length;
			
			mOutputStream.writeInt(messageLength);
			mOutputStream.writeInt(NO_COMM_ERROR);
			mOutputStream.write(encryptedBytes, 0, encryptedBytes.length);
			
		}
		catch (Exception e) {
			;
		}
    }

    
    /**
     * Watch for incoming messages from Photoshop. Send those to the main 
     * thread to process.
     */
    public void run () {
    	
    	final int bytesBeforeProcessing = LENGTH_LENGTH + COMM_LENGTH + PROTOCOL_LENGTH;
    	
		try {

			while (mKeepRunning) {

    			if (mInputStream.available() >= bytesBeforeProcessing) {

    				int messageLength = mInputStream.readInt();
    				int comStatus = mInputStream.readInt();
    				
    				if (messageLength < 0) {
    					continue;
    				}
    				
    				messageLength = messageLength - COMM_LENGTH;

    				// wait for rest of the message
    				long to = System.currentTimeMillis() + 2000;
    				while ((mInputStream.available() < messageLength) && (System.currentTimeMillis() < to)) {
    					to = to + 1;
    				}
    				
    				byte[] messageBytes = new byte[messageLength];
    				int readThisMany = mInputStream.read(messageBytes, 0, messageLength);
    				if (readThisMany != messageLength) {
    					mMainActivity.runOnUiThread(new MessageRunner(mMainActivity, "Maybe time out!", new String("")));
    					continue; // we must of timed out
    				}

    				if (comStatus != NO_COMM_ERROR) {
    					readDecryptedMessage(messageBytes);
    				} else {
    					byte [] decryptedBytes = mEncryptDecrypt.decrypt(messageBytes);
    					readDecryptedMessage(decryptedBytes);
    				}
    			} /* if inputStream.available() */

	    	} /* while keepRunning */

	    	if (mInputStream != null) {
	    		mInputStream.close();
	    	}
	    	if (mOutputStream != null) {
	    		mOutputStream.close();
	    	}
	    	if (mPhotoshopSocket != null) {
	    		mPhotoshopSocket.close();
	    	}

    	} /* try block */
    	
		catch (Exception e) {
			mMainActivity.runOnUiThread(new MessageRunner(mMainActivity, e.toString(), new String("")));
		}
		
    	
    } /* end void run() */


    /**
     * process the decrypted message
     */
    public void readDecryptedMessage(byte [] messageBytes) {
    	try {
    		int messageIndexer = 0;
	
    		int messageVersion = (messageBytes[messageIndexer++] << 24) + 
							 	+ ((messageBytes[messageIndexer++] & 0xFF) << 16)
							 	+ ((messageBytes[messageIndexer++] & 0xFF) << 8)
							 	+ (messageBytes[messageIndexer++] & 0xFF);

    		if (messageVersion != PROTOCOL_VERSION) {
    			return;
    		}
	
    		int messageID = (messageBytes[messageIndexer++] << 24) + 
                        	+ ((messageBytes[messageIndexer++] & 0xFF) << 16)
                        	+ ((messageBytes[messageIndexer++] & 0xFF) << 8)
                        	+ (messageBytes[messageIndexer++] & 0xFF);
		
    		if (messageID == mTransactionID) {
    			return;
    		}
		
    		int messageType = (messageBytes[messageIndexer++] << 24) + 
    						  + ((messageBytes[messageIndexer++] & 0xFF) << 16)
    						  + ((messageBytes[messageIndexer++] & 0xFF) << 8)
                          	  + (messageBytes[messageIndexer++] & 0xFF);
		
    		if (messageType == JAVASCRIPT_TYPE || messageType == ERRORSTRING_TYPE) {
    			byte [] returnBytes = new byte[messageBytes.length - messageIndexer];
    			for (int i = 0; i < returnBytes.length; i++) {
    				returnBytes[i] = messageBytes[messageIndexer++];
    			}
    			String messageResult = new String(returnBytes, "UTF-8");
    			String [] splitter = messageResult.split("\r", 2);
    			if (splitter.length < 2) {
    				mMainActivity.runOnUiThread(new MessageRunner(mMainActivity, splitter[0], new String("")));;
    			} else {
    				mMainActivity.runOnUiThread(new MessageRunner(mMainActivity, splitter[0], splitter[1]));
    			}
    		}
    	} // end try block
    	catch(Exception e) {
    		mMainActivity.runOnUiThread(new MessageRunner(mMainActivity, e.toString(), new String("")));
    		return;
    	}
	}


    /**
     * let the main thread stop this one
     */
    public void stopProcessing() {
    	mKeepRunning = false;
    }
    
} /* end class MessageProcessor.java */
