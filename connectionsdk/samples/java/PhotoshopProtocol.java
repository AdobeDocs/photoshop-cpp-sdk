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

/**  Filename: PhotoshopProtocol.java
	@author Thomas Ruark, Photoshop Engineering, Adobe Systems Incorporated
*/

import java.net.*;
import java.io.*;
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

/* protocol is
   4 bytes length of the message
   4 bytes communication status
   4 bytes version
   4 bytes transaction id
   4 bytes type
   ... actual information depending on type
*/

public class PhotoshopProtocol {

    /** flag for verbose output */
    public boolean mShowMessages = true;

    /** values for transaction type */
    private static final int ILLEGAL_TYPE = 0;
    private static final int ERRORSTRING_TYPE = 1;
    private static final int JAVASCRIPT_TYPE = 2;
    private static final int IMAGE_TYPE = 3;
    private static final int PROFILE_TYPE = 4;
	private static final int DATA_TYPE = 5;
    
    /** current protocol version */
    private static final int PROTOCOL_VERSION = 1;
    
    /** length of the header not including the actual length byte
	or the communication status */
    private static final int PROTOCOL_LENGTH = 4 + 4 + 4;
    private static final int COMM_LENGTH = 4;
    private static final int NO_COMM_ERROR = 0;

    /** bump this value every time a message gets sent,
	 *  the reply message will match the ID
	*/
    public int mTransactionID = 0;

    /** messages are encrypted, helper class to encrypt and decrypt messages */
    private EncryptDecrypt mEncryptDecrypt = null;

    PhotoshopProtocol(String inPassword) throws UnknownHostException, 
						IOException, 
						InvalidAlgorithmParameterException,
						NoSuchPaddingException, 
						NoSuchAlgorithmException, 
						InvalidKeyException {

	mEncryptDecrypt = new EncryptDecrypt(inPassword);

    }
    
    public byte [] processJavaScript(String theInput, DataOutputStream out, DataInputStream in) {

	if (mShowMessages) {
	    System.err.println("Start processJavaScript");
	}

	byte [] theOutput = null;

	try {

	    /* WRITE */
	    byte [] strBytes = theInput.getBytes("UTF-8");
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

	    if (mShowMessages) {
		System.err.println( "Bytes to encrypt: " + allBytes.length );
		for (int i = 0; i < allBytes.length; i++) {
		    System.err.print( Integer.toString( ( allBytes[i] & 0xff ) + 0x100, 16).substring( 1 ) + " " );
		    if (i > 0 && (((i+1) % 8) == 0)) System.err.println();
		}
		System.err.println();
	    }

	    byte [] encryptedBytes = mEncryptDecrypt.encrypt(allBytes);
	    
	    if (mShowMessages) {
		System.err.println( "Bytes encrypted: " + encryptedBytes.length);
		for (int i = 0; i < encryptedBytes.length; i++) {
		    System.err.print( Integer.toString( ( encryptedBytes[i] & 0xff ) + 0x100, 16).substring( 1 ) + " " );
		    if (i > 0 && (((i+1) % 8) == 0)) System.err.println();
		}
		System.err.println();
	    }

	    // we don't encrypt the communication status so we add that length
	    int messageLength = COMM_LENGTH + encryptedBytes.length;

	    if (mShowMessages) {
		System.err.println("Message length: " + messageLength);
	    }

	    out.writeInt(messageLength);
	    out.writeInt(NO_COMM_ERROR);
	    out.write(encryptedBytes, 0, encryptedBytes.length);

	    if (mShowMessages) {
		System.err.println("Sending JavaScript to Server");
	    }

	    /* READ */
	    int inLength = in.readInt();

	    if (mShowMessages) {
		System.err.println("Reading length: " + inLength);
	    }

	    int inComStatus = in.readInt();

	    if (mShowMessages) {
		System.err.println("Reading com status: " + inComStatus);
	    }

	    if (inComStatus != 0) {
		int inVersion = in.readInt();

		if (mShowMessages) {
		    System.err.println("Reading version: " + inVersion);
		}

		int inTransaction = in.readInt();

		if (mShowMessages) {
		    System.err.println("Reading ID: " + inTransaction);
		}

		int inType = in.readInt();

		if (mShowMessages) {
		    System.err.println("Reading Type: " + inType);
		}

		if (inType == JAVASCRIPT_TYPE || inType == ERRORSTRING_TYPE) {
		    inLength -= PROTOCOL_LENGTH;
		    theOutput = new byte[inLength + 1];
		    int rr = in.read(theOutput, 0, inLength);

		    if (mShowMessages) {
			for (int i = 0; i < theOutput.length; i++) {
			    System.err.print(theOutput[i] + " ");
			}
			System.err.println("\nReading Bytes: " + rr);
		    }

		} else {

		    System.err.println("Unknown type: " + inType);
		    
		}

		int inAvailable = in.available();
		for (int i = 0; i < inAvailable; i++) {
		    int inByte = in.readByte();

		    if (mShowMessages) {
			System.err.println("Reading more bytes: " + inByte);
		    }

		}

		if (mShowMessages) {
		    System.err.println("Done reading comm error from Server");
		}

		// no error the message is encrypted
	    } else {
		inLength = inLength - 4;
		long to = System.currentTimeMillis() + 2000;
		while ((in.available() < inLength) && (System.currentTimeMillis() < to)) {
		    to = to + 1; // wait for the entire message
		}
    				
		byte[] messageBytes = new byte[inLength];
		int readThisMany = in.read(messageBytes, 0, inLength);

		if (mShowMessages) {
		    System.err.println("Read this encrypted message: " + readThisMany);
		    for (int i = 0; i < messageBytes.length; i++) {
			System.err.print( Integer.toString( ( messageBytes[i] & 0xff ) + 0x100, 16).substring( 1 ) + " " );
			if (i > 0 && (((i+1) % 8) == 0)) System.err.println();
		    }
		    System.err.println();
		}

		byte [] decryptedBytes = mEncryptDecrypt.decrypt(messageBytes);

		if (mShowMessages) {
		    System.err.println("Decrypted message: " + decryptedBytes.length);
		    for (int i = 0; i < decryptedBytes.length; i++) {
			System.err.print( Integer.toString( ( decryptedBytes[i] & 0xff ) + 0x100, 16).substring( 1 ) + " " );
			if (i > 0 && (((i+1) % 8) == 0)) System.err.println();
		    }
		    System.err.println();
		}

    		int messageIndexer = 0;
    		int messageVersion = (decryptedBytes[messageIndexer++] << 24) 
		    + ((decryptedBytes[messageIndexer++] & 0xFF) << 16)
		    + ((decryptedBytes[messageIndexer++] & 0xFF) << 8)
		    + (decryptedBytes[messageIndexer++] & 0xFF);

		if (mShowMessages) {
		    System.err.println("Reading version: " + messageVersion);
		}

    		messageID = (decryptedBytes[messageIndexer++] << 24) 
		    + ((decryptedBytes[messageIndexer++] & 0xFF) << 16)
		    + ((decryptedBytes[messageIndexer++] & 0xFF) << 8)
		    + (decryptedBytes[messageIndexer++] & 0xFF);

		if (mShowMessages) {
		    System.err.println("Reading messageID: " + messageID);
		}

    		int messageType = (decryptedBytes[messageIndexer++] << 24) + 
		    + ((decryptedBytes[messageIndexer++] & 0xFF) << 16)
		    + ((decryptedBytes[messageIndexer++] & 0xFF) << 8)
		    + (decryptedBytes[messageIndexer++] & 0xFF);

		if (mShowMessages) {
		    System.err.println("Reading messageType: " + messageType);
		}

		if (messageType == JAVASCRIPT_TYPE || messageType == ERRORSTRING_TYPE) {
		    theOutput = new byte[decryptedBytes.length - messageIndexer];
		    for (int i = 0; i < theOutput.length; i++) {
			theOutput[i] = decryptedBytes[messageIndexer++];
		    }
		} else {

		    System.err.println("We don't do other message types: " + messageType);

		}
	    }
	}
	catch (Exception e) {
	    System.out.println("Exception processJavaScript: " + e.toString());
	}
	return theOutput;
    }
}
