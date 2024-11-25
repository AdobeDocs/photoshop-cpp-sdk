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

package corp.adobe.photoshopimages;

import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.IOException;
import java.net.Socket;
import java.net.UnknownHostException;
import java.net.InetSocketAddress;
import java.security.InvalidAlgorithmParameterException;
import java.security.InvalidKeyException;
import java.security.NoSuchAlgorithmException;
import java.util.Vector;
import javax.crypto.NoSuchPaddingException;

/**
 * This project is asking for messages from Photoshop when 
 * something changes. Keep a thread running to process them.
 */
class MessageProcessor extends Thread {

	/** The main activity that started this thread */
	private PhotoshopImages mMainActivity = null;
	
	/** Server name */
	private String mServerName = null;
	
	/** Server password */
	private String mServerPassword = null;
	
	/** The Socket we use to connect to Photoshop */
    private Socket mPhotoshopSocket = null;
    
	/** Port of the server to use, not an option to change. */
	final private int SERVER_PORT_NUMBER = 49494;
	
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
    private static final int IMAGE_TYPE = 3;
	@SuppressWarnings("unused")
	private static final int PROFILE_TYPE = 4;
	@SuppressWarnings("unused")
	private static final int DATA_TYPE = 5;
    
    /** no communication error */
    private final int NO_COMM_ERROR = 0;
    
    /** number of bytes for the communication status */
    private final int COMM_LENGTH = 4;
    
    /** number of bytes for the message length */
    private final int LENGTH_LENGTH = 4;

    /** number of bytes for the message header: version + id + type */
    private final int PROTOCOL_LENGTH = 4 + 4 + 4;
    
    /** Current version of the protocol used by Photoshop */
    private final int PROTOCOL_VERSION = 1;
    
    /** connection time out */
    private final int COMM_TIMEOUT = 10000;
    
    /**
     * Each message sent and received will have an ID.
     * If you send more than one make sure your ID's match.
     */
    private int mTransactionID = 0;
    
    private Vector<Integer> mTransactionsToDump;
    
    /** Keeps the main thread running waiting for messages from Photoshop */
    private boolean mKeepRunning;
    
    MessageProcessor(PhotoshopImages inActivity, 
    		         String inServerName, 
    		         String inPassword) {
   		mMainActivity = inActivity;
    	mServerName = inServerName;
    	mServerPassword = inPassword;
    	mTransactionsToDump = new Vector<Integer>();
    }
    
    private boolean connect()
    		         
    	throws UnknownHostException, IOException, InvalidAlgorithmParameterException,
    	NoSuchPaddingException, NoSuchAlgorithmException, InvalidKeyException {
    	
    	if (!mIsConnected) {
    		InetSocketAddress socketAddress = new InetSocketAddress(mServerName, SERVER_PORT_NUMBER);
    		mPhotoshopSocket = new Socket();
    		mPhotoshopSocket.connect(socketAddress, COMM_TIMEOUT);
    		mOutputStream = new DataOutputStream(mPhotoshopSocket.getOutputStream());
    		mInputStream = new DataInputStream(mPhotoshopSocket.getInputStream());
    		mEncryptDecrypt = new EncryptDecrypt(mServerPassword);
    		mKeepRunning = true;
    		mIsConnected = true;
    	}
        return mIsConnected;
    }
    
    private void disconnect() {
        mIsConnected = false;
        mKeepRunning = false;
    	
   		mEncryptDecrypt = null;
   		mInputStream = null;
   		mOutputStream = null;
   		mPhotoshopSocket = null;
    }

    /** 
	 * Send JavaScript to Photoshop the run method will get the result
	 * and pass it back to the main UI thread.
	 * @param inJavaScript JavaScript string to send
	 * @result return the transactionID, -1 if something goes wrong
	 */
    public int sendJavaScript(String inJavaScript) {
    	
    	if (null == inJavaScript) {
    		return -1;
    	}
    	
    	int returnID = mTransactionID;

    	mTransactionID++;
        
    	try {

    		connect();
    		
    		byte [] strBytes = inJavaScript.getBytes("UTF-8");
    		byte [] allBytes = new byte[PROTOCOL_LENGTH + strBytes.length + 1];
    		
    		int byteIndexer = 0;
    		
            allBytes[byteIndexer++] = (byte)(PROTOCOL_VERSION >>> 24);
            allBytes[byteIndexer++] = (byte)(PROTOCOL_VERSION >>> 16);
            allBytes[byteIndexer++] = (byte)(PROTOCOL_VERSION >>> 8);
            allBytes[byteIndexer++] = (byte)(PROTOCOL_VERSION);
            
            int messageID = returnID;
            
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

    		// the communication status is not encrypted, add that length
			int messageLength = COMM_LENGTH + encryptedBytes.length;
			
			mOutputStream.writeInt(messageLength);
			mOutputStream.writeInt(NO_COMM_ERROR);
			mOutputStream.write(encryptedBytes, 0, encryptedBytes.length);
			
		}
		catch (Exception e) {
			mMainActivity.runOnUiThread(new MessageRunner(mMainActivity, "sendJavaScript error", e.toString(), returnID));;
			returnID = -1;
		}
		
		return returnID;
		
    }

    
    public void run () {

      try {
    	  
    	connect();
    	
    	/* handy flag for debugging purposes, should be false for release */
    	boolean dontTimeout = false;
    	
    	final int bytesBeforeProcessing = LENGTH_LENGTH + COMM_LENGTH + PROTOCOL_LENGTH;
    	
    	while (mKeepRunning) {
    	
    		try {

    			if (mInputStream.available() >= bytesBeforeProcessing) {

    				int messageLength = mInputStream.readInt();

    				int comStatus = mInputStream.readInt();
    				
    				if (messageLength < bytesBeforeProcessing || comStatus < 0) {
    					mMainActivity.runOnUiThread(new MessageRunner(mMainActivity, "Bad length or com status!", new String(""), -1));
    					mKeepRunning = false;
    					continue;
    				}
    				
    				messageLength = messageLength - COMM_LENGTH;
    				
    				byte[] messageBytes = new byte[messageLength];
    				int offSet = 0;
    				int bytesToComplete = messageLength;
    				long currentTime = System.currentTimeMillis();
    				long timeoutNumber = currentTime + COMM_TIMEOUT;
    				boolean done = false;
    				while ( ! done && mKeepRunning && (currentTime < timeoutNumber)) {
    					int available = mInputStream.available();
    					if (available > bytesToComplete ) {
    						available = bytesToComplete;
    					}
    					if (available > 0) {
    						mInputStream.read(messageBytes, offSet, available);
    						offSet += available;
    						bytesToComplete -= available;
    						if (offSet >= messageLength) {
    							done = true;
    						}
    					}
    					currentTime = System.currentTimeMillis();
    					if (dontTimeout) {
    						currentTime = 0;
    					}
    				}
    				
    				if (currentTime >= timeoutNumber) {
    					mKeepRunning = false;
    					mMainActivity.runOnUiThread(new MessageRunner(mMainActivity, "Time out!", new String(""), -1));
    					continue;
    				}

    				if (comStatus != NO_COMM_ERROR) {
    					readDecryptedMessage(messageBytes);
    				} else {
    					byte [] decryptedBytes = mEncryptDecrypt.decrypt(messageBytes);
    					readDecryptedMessage(decryptedBytes);
    				}
    				
    			} /* if inputStream.available() */
    		
    		} /* try block */
    		
    		
    		catch (Exception e) {
    			mMainActivity.runOnUiThread(new MessageRunner(mMainActivity, e.toString(), new String(""), -1));
    			disconnect();
    			continue;
    		}
    	
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

      } /* global try block for function */

      catch(UnknownHostException uhe) {
    	  disconnect();
      }
      catch(IOException ioe) {
    	  disconnect();
      }
      catch(InvalidAlgorithmParameterException iape) {
    	  disconnect();
      }
      catch(NoSuchPaddingException nspe) {
    	  disconnect();
      }
      catch(NoSuchAlgorithmException nsae) {
    	  disconnect();
      }
      catch(InvalidKeyException ike) {
    	  disconnect();
      }
    } /* end void run() */


    /**
     * process the message bytes and give result back to main thread
     */
    public void readDecryptedMessage(byte [] messageBytes) {
    	try {
    		int messageIndexer = 0;
	
    		int messageVersion = (messageBytes[messageIndexer++] << 24)
							 	+ ((messageBytes[messageIndexer++] & 0xFF) << 16)
							 	+ ((messageBytes[messageIndexer++] & 0xFF) << 8)
							 	+ (messageBytes[messageIndexer++] & 0xFF);

    		if (messageVersion != PROTOCOL_VERSION) {
    			return;
    		}
	
    		int messageID = (messageBytes[messageIndexer++] << 24)
                        	+ ((messageBytes[messageIndexer++] & 0xFF) << 16)
                        	+ ((messageBytes[messageIndexer++] & 0xFF) << 8)
                        	+ (messageBytes[messageIndexer++] & 0xFF);
		
    		if (messageID == mTransactionID) {
    			return;
    		}
    		
    		if (mTransactionsToDump.size() > 0) {
    			for (int i = 0; i < mTransactionsToDump.size(); i++) {
    				Integer id = mTransactionsToDump.elementAt(i);
    				if (id == messageID) {
    					mTransactionsToDump.removeElementAt(i);
    					return;
    				}
    			}
    		}
		
    		int messageType = (messageBytes[messageIndexer++] << 24)
    						  + ((messageBytes[messageIndexer++] & 0xFF) << 16)
    						  + ((messageBytes[messageIndexer++] & 0xFF) << 8)
                          	  + (messageBytes[messageIndexer++] & 0xFF);
		
			if (messageType == JAVASCRIPT_TYPE || messageType == ERRORSTRING_TYPE) {
				byte [] returnBytes = new byte[messageBytes.length - messageIndexer];
				for (int i = 0; i < returnBytes.length; i++) {
					returnBytes[i] = messageBytes[messageIndexer++];
				}
    			String messageResult = new String(returnBytes, "UTF-8");
    			String [] splitter = messageResult.split("\r");
    			if (splitter.length == 2) {
    				mMainActivity.runOnUiThread(new MessageRunner(mMainActivity, splitter[0], splitter[1], messageID));
    			} else {
    				return;
    			}
    		} else if (messageType == IMAGE_TYPE) {
    			mMainActivity.runOnUiThread(new MessageRunner(mMainActivity, messageBytes, messageIndexer, messageID));
    		}
    	} // end try block
    	catch(Exception e) {
    		mMainActivity.runOnUiThread(new MessageRunner(mMainActivity, e.toString(), new String(""), -1));
    		return;
    	}
	}


    /**
     * let the main thread stop this one
     */
    public void stopProcessing() {
    	mKeepRunning = false;
    }
    
    public void dumpTransaction(int inTransaction) {
    	mTransactionsToDump.add(inTransaction);
    }
    
    
} /* end class MessageProcessor */
