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

/**  Filename: MessageRunner.java
	@author Thomas Ruark, Photoshop Engineering, Adobe Systems Incorporated
*/

package corp.adobe.photoshopimages;

/**
 * Helper class to get messages from the thread processing messages from
 * Photoshop (MessageProcessor) to the UI Activity (PhotoshopImages)
 */
class MessageRunner implements Runnable {
	final private PhotoshopImages mHostActivity;
    final private String mCommandStr;
    final private String mExtraStr;
    final private byte [] mMessageBytes;
    final private int mMessageIndexer;
    final private int mMessageID;
    final private int NO_BYTES = -1;
    	
    MessageRunner(PhotoshopImages inActivity, String inCommand, String inExtra, int inID) {
    	mHostActivity = inActivity;
    	mCommandStr = new String(inCommand);
    	mExtraStr = new String(inExtra);
    	mMessageBytes = null;
    	mMessageIndexer = NO_BYTES;
    	mMessageID = inID;
    }
    
    MessageRunner(PhotoshopImages inActivity, byte [] inBytes, int inIndexer, int inID) {
    	mHostActivity = inActivity;
    	mCommandStr = null;
    	mExtraStr = null;
    	mMessageBytes = inBytes;
    	mMessageIndexer = inIndexer;
    	mMessageID = inID;
    }
    
    public void run() {
    	if ( NO_BYTES == mMessageIndexer ) {
    		mHostActivity.runFromThread(mCommandStr, mExtraStr, mMessageID);
    	} else {
    		mHostActivity.runFromThread(mMessageBytes, mMessageIndexer, mMessageID);
    	}
    }
       
}
