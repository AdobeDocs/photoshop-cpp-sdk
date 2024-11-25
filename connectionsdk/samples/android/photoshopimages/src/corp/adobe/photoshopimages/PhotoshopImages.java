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

/**  Filename: PhotoshopImages.java
	@author Thomas Ruark, Photoshop Engineering, Adobe Systems Incorporated
*/

package corp.adobe.photoshopimages;

import android.app.Activity;
import android.os.Bundle;
import android.widget.Toast;
import android.view.MenuInflater;
import android.view.Menu;
import android.view.MenuItem;
import java.lang.Double;
import java.util.Vector;


/**
 * Android version for getting images from Photoshop.
 * This version is going to try and connect at launch and the background will
 * reflect the state of the connection.
*/
public class PhotoshopImages extends Activity
	implements ConfigureDialog.OnDoneListener {    

	/** Name of the server to connect */
	private String mServerNameText = new String("192.168.16.1");
	
	/** Server password to use */
	private String mServerPasswordText = new String("Swordfish");

	/** have the border color reflect our connection status */
	final private int BACKGROUND_COLOR_CONNECTED = 0xFFCCCCCC;
	final private int BACKGROUND_COLOR_DISCONNECTED = 0xFFFF0000;
	
	/** images can come in two types, we use strings */
    private static final String JPEG_TYPE_STR = new String("1");
    private static final String PIXMAP_TYPE_STR = new String("2");

    /** for image type we have Pixmap and JPEG */
    private static final int JPEG_TYPE = 1;
    private static final int PIXMAP_TYPE = 2;
    
    /** Worker thread to process incoming messages from Photoshop */
    MessageProcessor mMessageProcessor = null;
    
    /** Messages that will come back to us from Photoshop when things change in Photoshop */
    private static final String CLOSED_DOCUMENT_STR = new String("closedDocument");
    private static final String NEW_DOCUMENT_VIEW_CREATED_STR = new String("newDocumentViewCreated");
    private static final String CURRENT_DOCUMENT_CHANGED_STR = new String("currentDocumentChanged");
    private static final String ACTIVE_VIEW_CHANGED_STR = new String("activeViewChanged");
    private static final String DOCUMENT_CHANGED_STR = new String("documentChanged");
    
    /** Some made up names I created to send messages to myself */
    private static final String SUBSCRIBE_SUCCESS_STR = new String("subscribeSuccess");
    private static final String DOCOMENT_NAMES_CHANGED_STR = new String("documentNamesChanged");
    private static final String ACTIVE_DOCUMENT_STR = new String("activeDocument");
    private static final String DOCUMENT_ID_STR = new String("documentID");

    /** the current document ID */
    private String mActiveDocumentID = new String("0");
    
    /** main view for this Activity */
    private ImagesView mMainView = null;
    
    /** try to track how far behind I am in requests */
    private int mOutStandingImageRequests = 0;
    
    /** outstanding image request transaction IDs */
    private Vector<Integer> mOutStandingTransactionIDs = null;
    
    /** time I requested last image */
    long mRequestTime = System.currentTimeMillis();
    
    /** timing of last image, not correct when there is a backlog of requests */
    double mLastTime = 0.0;
    
    /** Called when the activity is first created. 
     * Start ImagesView
     */
    @Override
    public void onCreate(Bundle savedInstanceState) {
    	super.onCreate(savedInstanceState);
   		mOutStandingTransactionIDs = new Vector<Integer>();
    	tryToConnect();
   		mMainView = new ImagesView(this, (null != mMessageProcessor && mMessageProcessor.mIsConnected) ? BACKGROUND_COLOR_CONNECTED : BACKGROUND_COLOR_DISCONNECTED);
    	setContentView(mMainView);
    	
    } /* end onCreate */


    /** Called when the activity is gone.
     * Get rid of our background thread.
     */
    @Override
    public void onDestroy() {
        super.onDestroy();
        mMessageProcessor.stopProcessing();
    }

    /**
     * Menu code
     */
    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
    	MenuInflater inflater = getMenuInflater();
    	inflater.inflate(R.menu.menu, menu);
    	return true;
    }
    
    
    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
    	switch (item.getItemId()) {
    		case R.id.configure:
    			new ConfigureDialog(this, this, mServerNameText, mServerPasswordText).show();
    			return true;
    		default:
    			return super.onOptionsItemSelected(item);    
    	}
    }

    
    /**
     * Called by the ConfigureDialog class
     */
    public void optionsChanged(String server, String password) {
    	mServerNameText = server;
    	mServerPasswordText = password;
    	tryToConnect();
    	mMainView.setBorderColor(null != mMessageProcessor && mMessageProcessor.mIsConnected ? BACKGROUND_COLOR_CONNECTED : BACKGROUND_COLOR_DISCONNECTED);
    }
    
    /**
     * Request image data from Photoshop.
     * JPEG (1) or Pixmap (2) 
     * @param inImageID - unique ID of image given to me from Photoshop
     */
    private void getPhotoshopImage(String inImageID, boolean wantPixmap, int w, int h) {
    	boolean withIDs = true;
    	if (w <= 1 || h <= 1 || inImageID.length() == 0) { 
    		return; 
    	}
    	String s = "if (documents.length) {";
		s += "var idNS = stringIDToTypeID( 'sendDocumentThumbnailToNetworkClient' );";
		s += "var desc1 = new ActionDescriptor();";
		if (withIDs)
 			s += "desc1.putInteger( stringIDToTypeID( 'documentID' )," + inImageID + ");";
		s += "desc1.putInteger( stringIDToTypeID( 'width' )," + w + ");";
		s += "desc1.putInteger( stringIDToTypeID( 'height' )," + h + ");";
		s += "desc1.putInteger( stringIDToTypeID( 'format' )," + (wantPixmap ? PIXMAP_TYPE_STR : JPEG_TYPE_STR) + ");";
		s += "executeAction( idNS, desc1, DialogModes.NO );";
    	s += "'Image Request Sent';";
		s += "}";
    	if (mMessageProcessor != null) {
    		int id = mMessageProcessor.sendJavaScript(s);
    		dumpAllCurrentImageRequests();
    		mOutStandingTransactionIDs.add(id); 
    	}
    }


    /**
     * Show a short dialog of issues
     * @param inMessage - text to display
     */
    private void showMessage(String inMessage) {
    	Toast.makeText(PhotoshopImages.this, inMessage, Toast.LENGTH_SHORT).show();
    }

    
    /**
     * Try to connect to Photoshop
     */
    private void tryToConnect() {
    	try {
    		mMessageProcessor = new MessageProcessor(this, mServerNameText, mServerPasswordText);
    		mMessageProcessor.start();
           	getActiveDocID();
           	setupEvents();
    	}
    	catch (Exception e) {
            showMessage( e.toString() );
            mMessageProcessor = null;
        } 
    }

    
    /**
     * Ask Photoshop to send us when a document is added, deleted, opened or changed.
     */
    private void setupEvents() {
    	subscribeToEvent(CLOSED_DOCUMENT_STR);
    	subscribeToEvent(NEW_DOCUMENT_VIEW_CREATED_STR);
    	subscribeToEvent(CURRENT_DOCUMENT_CHANGED_STR);
    	subscribeToEvent(ACTIVE_VIEW_CHANGED_STR);
    	subscribeToEvent(DOCUMENT_CHANGED_STR);
    }
    
    /**
     * Ask Photoshop for the current image
     */
    private void getImage() {
    	getPhotoshopImage(mActiveDocumentID, false /* wantPixmap */, mMainView.getBitmapWidth(), mMainView.getBitmapHeight());
    	mOutStandingImageRequests++;
    	setTitle("Ps Images " + Integer.toString(mOutStandingImageRequests) + " " + Double.toString(mLastTime) + " " + Integer.toString(mOutStandingTransactionIDs.size()));
    	mRequestTime = System.currentTimeMillis();
    }
    
    
    /**
     * The view has changed size, we need to recalculate 
     */
    public void onSizeChanged() {
		mMainView.setGrid();
    	if (mActiveDocumentID.length() > 0 && mActiveDocumentID.compareTo("0") != 0) {
    		getImage();
    	}
    }

    /**
     * ID of the active document from Photoshop
     */
    private void setDocID(String inID) {
    	boolean getNewImage = false;
    	if (inID != mActiveDocumentID) {
    		getNewImage = true;
    	}
    	mActiveDocumentID = inID;
    	if (getNewImage) {
    		if (mActiveDocumentID.compareTo("0") != 0) {
    			getImage();
    		} else {
    			mMainView.noImages();
    		}
    	}
    }

    
    /**
     * Ask Photoshop for the current document ID
     */
    private void getActiveDocID() {
    	String s = "var docID = '0';";
    	s += "if (app.documents.length)";
    	s += "{";
    	s += "  docID = activeDocument.id;";
    	s += "}";
    	s += "'" + DOCUMENT_ID_STR + "' + String.fromCharCode(13) + docID;";
    	if (mMessageProcessor != null) {
    		mMessageProcessor.sendJavaScript(s);
    	}
    }
    
    
    /**
     * Ask Photoshop to tell us when certain things happen.
     * 
     * NOTE: Make sure you set up the handler in the worker
     * thread to let the main thread know what is going on.
     * 
     * @param inEvent
     */
	private void subscribeToEvent(String inEvent) {
		String s = "var idNS = stringIDToTypeID( 'networkEventSubscribe' );";
		s += "var desc1 = new ActionDescriptor();";
		s += "desc1.putClass( stringIDToTypeID( 'eventIDAttr' ), stringIDToTypeID( '" + inEvent + "' ) );";
		s += "executeAction( idNS, desc1, DialogModes.NO );";
		s += "'" + SUBSCRIBE_SUCCESS_STR + "' + String.fromCharCode(13) + 'YES';";
		if (mMessageProcessor != null) {
			mMessageProcessor.sendJavaScript(s);
		}
	}

	
    /**
     * Called from the worker thread to tell us
     * something has happened with documents
     */
    public void setDocChanged() {
    	getActiveDocID();
    }

    
    /**
     * The thread will call us here with either a message from the
     * Photoshop JavaScript we sent or from something happening
     * in from Photoshop getting changed. This is why we have code
     * above that fakes the same messages as Photoshop will send us as
     * we really don't care where they originated from.
     * @param inBytes
     * @param inIndexer
     * @param inID

     */
    public void runFromThread(String inCommand, String inExtra, int inID) {
    	if (inCommand.compareTo(CLOSED_DOCUMENT_STR) == 0) {
        	setDocChanged();
        } else if (inCommand.compareTo(NEW_DOCUMENT_VIEW_CREATED_STR) == 0) {
        	setDocChanged();
        } else if (inCommand.compareTo(CURRENT_DOCUMENT_CHANGED_STR) == 0) {
        	setDocChanged();
        } else if (inCommand.compareTo(ACTIVE_VIEW_CHANGED_STR) == 0) {
        	setDocChanged();
        } else if (inCommand.compareTo(DOCOMENT_NAMES_CHANGED_STR) == 0) {
        	setDocChanged();
        } else if (inCommand.compareTo(DOCUMENT_ID_STR) == 0) {
        	setDocID(inExtra);
        } else if (inCommand.compareTo(DOCUMENT_CHANGED_STR) == 0) {
        	getImage();
        } else if (inCommand.compareTo(ACTIVE_DOCUMENT_STR) == 0) {
        	setDocChanged();
       	} else if (inCommand.compareTo(SUBSCRIBE_SUCCESS_STR) == 0) {
        	; // setTitle(Integer.toString(mSubscribeCount)); // this is good nothing to do for these
        } else {
        	showMessage(inCommand + ":" + inExtra);
        }
    }
    
    
    /**
     * Photoshop just sent me a image.
     * @param inBytes
     * @param inIndexer
     * @param inID
     */
    public void runFromThread(byte [] inBytes, int inIndexer, int inID) {
    	byte format = inBytes[inIndexer++];
    	if (JPEG_TYPE == format) {
    		String result = mMainView.createBitmapFromJPEG(inBytes, inIndexer);
    		if (null != result) {
    			showMessage(result);
    		}
    	} else if (PIXMAP_TYPE == format) {
    		String result = mMainView.createBitmap(inBytes, inIndexer);
    		if (null != result) {
    			showMessage(result);
    		}
    	}
		mMainView.invalidate();
    	mOutStandingImageRequests--;
    	mLastTime = ((double)System.currentTimeMillis() - (double)mRequestTime) / 1000.0;
    	setTitle("Ps Images " + Integer.toString(mOutStandingImageRequests) + " " + Double.toString(mLastTime) + " " + Integer.toString(mOutStandingTransactionIDs.size()));
    }

    /** a new image has been requested, dump all pending requests as they arrive */
    private void dumpAllCurrentImageRequests() {
		if (mOutStandingTransactionIDs.size() > 0) {
			for (int i = mOutStandingTransactionIDs.size() - 1; mOutStandingTransactionIDs.size() > 0; i++) {
				Integer id = mOutStandingTransactionIDs.elementAt(i);
				mMessageProcessor.dumpTransaction(id);
				mOutStandingTransactionIDs.removeElementAt(i);
			}
		}
	}

} /* end public class PhotoshopImages extends Activity */

// end PhotoshopImages.java
