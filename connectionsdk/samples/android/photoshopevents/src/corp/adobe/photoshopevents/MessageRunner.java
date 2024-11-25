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

package corp.adobe.photoshopevents;

/**
 * Helper class to get messages from the thread processing messages from
 * Photoshop (MessageProcessor) to the UI Activity (PhotoshopEvents)
 */
class MessageRunner implements Runnable {
	final private PhotoshopEvents mHostActivity;
    final private String mCommandStr;
    final private String mExtraStr;
    	
    MessageRunner(PhotoshopEvents inActivity, String inCommand, String inExtra) {
    	mHostActivity = inActivity;
    	mCommandStr = new String(inCommand);
    	mExtraStr = new String(inExtra);
    }
    	
    public void run() {
    	mHostActivity.runFromThread(mCommandStr, mExtraStr);
    }
       
}
