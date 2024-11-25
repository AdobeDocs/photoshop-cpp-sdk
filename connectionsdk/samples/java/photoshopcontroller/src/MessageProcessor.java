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
 * Handle all the incoming and outgoing messages in this worker thread.
 * Multiple requests for the same thumb nail may occur as events come
 * in faster than the pixel data for the image arrives. When this occurs
 * we queue up the 2nd request until the first request is filled.
 */
class MessageProcessor extends Thread {

	/**
	 * Save all the information in this helper class.
	 */
	public class ImageIdentifier {
		public int mDocumentID;
		public int mLayerID;
		public int mImageType;
		public int mWidth;
		public int mHeight;
		public int mID;
		ImageIdentifier(int inDocumentID, int inLayerID, int inImageType, int inW, int inH) {
			mDocumentID = inDocumentID;
			mLayerID = inLayerID;
			mImageType = inImageType;
			mWidth = inW;
			mHeight = inH;
			mID = 0;
		}
		public boolean same(ImageIdentifier inID) {
			return (inID.mDocumentID == mDocumentID && inID.mLayerID == mLayerID);
		}
	}
	
	/** We have sent out this request and hope to get an image back */
    private Vector<ImageIdentifier> mPendingImageRequests = new Vector<ImageIdentifier>();

    /** We queue up these messages because the same request is in mPendingImageRequests
     *  and we need to ask again once the previous request comes in.
     */
    private Vector<ImageIdentifier> mQueuedImageRequests = new Vector<ImageIdentifier>();

    /** The main object that started this thread. Our incoming messages go there.  */
	private PhotoshopController mMain = null;
	
	/** Server name */
	private String mServerName = null;
	
	/** Server password */
	private String mServerPassword = null;
	
	/** The Socket we use to connect to Photoshop */
    private Socket mPhotoshopSocket = null;
    
	/** Port number of the server. */
	final private int SERVER_PORT_NUMBER = 49494;
	
    /** Output stream for the socket */
    private DataOutputStream mOutputStream = null;
    
    /** Input stream for the socket */
    private DataInputStream mInputStream = null;
    
    /** have we already connected */
    private boolean mIsConnected = false;
    
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
    
    /** connection time out in milliseconds */
    private final int COMM_TIMEOUT = 10000;
    
    /**
     * Each message sent and received will have an ID.
     * If you send more than one make sure your ID's match.
     */
    private int mTransactionID = 1;
    
    /** Keeps the main thread running waiting for messages from Photoshop */
    private boolean mKeepRunning;
    
    MessageProcessor(PhotoshopController inMain, 
    		         String inServerName, 
    		         String inPassword) {
   		mMain = inMain;
    	mServerName = inServerName;
    	mServerPassword = inPassword;
    }
    
    private boolean connect()
    		         
    	throws UnknownHostException, IOException, InvalidAlgorithmParameterException,
    	NoSuchPaddingException, NoSuchAlgorithmException, InvalidKeyException {
    	
    	if ( ! mIsConnected ) {
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
			mMain.runFromThread("sendJavaScript error", e.toString(), returnID);
			returnID = -1;
		}
		
		return returnID;
		
    }

    /** Loop until someone shuts us down. */
    public void run () {

      try {
    	  
    	connect();
    	
    	/* handy flag for debugging purposes, should be false for release */
    	boolean dontTimeout = false;
    	
    	final int bytesBeforeProcessing = LENGTH_LENGTH + COMM_LENGTH + PROTOCOL_LENGTH;
    	
    	while (mKeepRunning) {
    	
    		try {

    			// drop our CPU usage from 25% to nothing by adding this
    			Thread.sleep(100);
    			
    			if (mInputStream.available() >= bytesBeforeProcessing) {

    				int messageLength = mInputStream.readInt();

    				int comStatus = mInputStream.readInt();
    				
    				if (messageLength < bytesBeforeProcessing || comStatus < 0) {
    					mMain.runFromThread("Bad length or com status!", new String(""), -1);
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
    					mMain.runFromThread("Time out!", new String(""), -1);
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
    			mMain.runFromThread(e.toString(), new String(""), -1);
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

      } /* top level try block for function */

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
     * Process the message bytes and give result back to main thread
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
    		
    		/** Pull the message from the pending queue.
    		 * Look to see if the queue has the same document and layer
    		 * ready for another request. If it does, just dump this message.
    		 */
    		int documentID = 0;
    		int layerID = 0;
			for (int pi = 0; pi < mPendingImageRequests.size(); pi++) {
				ImageIdentifier id = mPendingImageRequests.elementAt(pi);
				if (id.mID == messageID) {
					documentID = id.mDocumentID;
					layerID = id.mLayerID;
					mPendingImageRequests.removeElementAt(pi);
	    			for (int qi = 0; qi < mQueuedImageRequests.size(); qi++) {
	    				ImageIdentifier qID = mQueuedImageRequests.elementAt(qi);
	    				if (id.same(qID)) {
	    					qID.mID = getPhotoshopImage(qID);
	    					if (qID.mID != -1)
	    						mPendingImageRequests.add(qID);
	    					mQueuedImageRequests.removeElementAt(qi);
	    					return;
	    				}
	    			}
					break;
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
    				mMain.runFromThread(splitter[0], splitter[1], messageID);
    			} else {
    				mMain.runFromThread("ERROR: Too many or no slash r in message ", Integer.toString(splitter.length), -1);
    			}
    		} else if (messageType == IMAGE_TYPE) {
    			mMain.runFromThread(messageBytes, messageIndexer, documentID, layerID);
    		}
    	} // end try block
    	catch(Exception e) {
    		mMain.runFromThread(e.toString(), new String(""), -1);
    		return;
    	}
	}

    /**
     * Let the object that created us destroy us.
     */
    public void stopProcessing() {
    	mKeepRunning = false;
    }
    
    /**
     * Request image data from Photoshop.
     * If I already have a request I'm waiting for put this request in the queue
     * if it is not already in the queue.
     *  
     * @param inDocumentID - unique ID of image given to me from Photoshop
     * @param inLayerID - unique ID of layer given to me from Photoshop, "0" means get the entire image
     * @param inImageType - JPEG ("1") or Pixmap ("2")
     * @param w - requested width I want, I may not get that exact value
     * @param h - requested height I want, I may not get that exact value
     */
    public void requestPhotoshopImage(int inDocumentID, int inLayerID, int inImageType, int w, int h) {
    	if (w <= 1 || h <= 1 || inDocumentID == 0) { 
    		return; 
    	}
    	
    	ImageIdentifier inID = new ImageIdentifier(inDocumentID, inLayerID, inImageType, w, h);
    	
		if (mPendingImageRequests.size() > 0) {
			for (int i = 0; i < mPendingImageRequests.size(); i++) {
				ImageIdentifier id = mPendingImageRequests.elementAt(i);
				if (inID.same(id)) {
					if (mQueuedImageRequests.size() > 0) {
						for (int a = 0; a < mQueuedImageRequests.size(); a++) {
							ImageIdentifier qID = mQueuedImageRequests.elementAt(a);
							if (inID.same(qID)) {
								return;
							}
						}
					}
					mQueuedImageRequests.add(inID);
					return; // in the queue, we can return now
				}
			}
		}

		inID.mID = getPhotoshopImage(inID);
		if (inID.mID != -1)
			mPendingImageRequests.add(inID);

    }

    /** The real request to Photoshop to send us the layer or document thumb nail.
     * 
     * @param inID - document, layer, width, height, image type
     * @return the unique ID for this request
     */
    private int getPhotoshopImage(ImageIdentifier inID) {
    	String s = "try { if (documents.length) {";
    	if (inID.mLayerID == -1)
    		s += "var idNS = stringIDToTypeID( 'sendDocumentThumbnailToNetworkClient' );";
    	else
       		s += "var idNS = stringIDToTypeID( 'sendLayerThumbnailToNetworkClient' );";
		s += "var desc = new ActionDescriptor();";
		s += "desc.putInteger( stringIDToTypeID( 'documentID' )," + inID.mDocumentID + ");";
		if (inID.mLayerID != -1)
			s += "desc.putInteger( stringIDToTypeID( 'layerID' )," + inID.mLayerID + ");";
		s += "desc.putInteger( stringIDToTypeID( 'width' )," + inID.mWidth + ");";
		s += "desc.putInteger( stringIDToTypeID( 'height' )," + inID.mHeight + ");";
		s += "desc.putInteger( stringIDToTypeID( 'format' )," + inID.mImageType + ");";
		s += "executeAction( idNS, desc, DialogModes.NO );";
    	s += "'Image Request Sent' + String.fromCharCode(13) + 'OK';";
		s += "}";
		s += "} catch(e) { 'Image Request Sent' + String.fromCharCode(13) + 'ERROR : ' + e.toString(); }";
   		return sendJavaScript(s);
    }
    
    /**  
     * @return How many requests are in our queue
     */
    public int queuedRequests() {
    	return mQueuedImageRequests.size();
    }
    
    /** 
     * @return How many requests we already have in to Photoshop
     */
    public int pendingRequests() {
    	return mPendingImageRequests.size();
    }

    
} /* end class MessageProcessor */
