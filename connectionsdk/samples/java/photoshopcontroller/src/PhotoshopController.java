/**************************************************************************
	ADOBE SYSTEMS INCORPORATED
	 Copyright 2012 Adobe Systems Incorporated
	 All Rights Reserved.

	NOTICE:  Adobe permits you to use, modify, and distribute this file
	in accordance with the terms of the Adobe license agreement accompanying
	it.  If you have received this file from a source other than Adobe, then
	your use, modification, or distribution of it requires the prior written
	permission of Adobe.
**************************************************************************/

/**  Filename: PhotoshopController.java
	@author Thomas Ruark, Photoshop Engineering, Adobe Systems Incorporated
*/

import java.awt.Color;
import java.awt.Dimension;
import java.awt.Image;
import java.awt.Insets;
import java.awt.Toolkit;
import java.awt.event.ItemEvent;
import java.awt.event.ItemListener;
import java.awt.event.KeyEvent;
import java.awt.event.KeyListener;
import java.io.FileWriter;
import java.io.IOException;
import java.io.PrintWriter;
import java.util.Vector;

import javax.swing.BorderFactory;
import javax.swing.JButton;
import javax.swing.JCheckBox;
import javax.swing.JComponent;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JPasswordField;
import javax.swing.JScrollPane;
import javax.swing.JTextArea;
import javax.swing.JTextField;
import javax.swing.UIManager;

/** 
* TODO add support for grouped layers
* TODO document this
* 
*/
public class PhotoshopController extends JPanel {
	JTextField mHostNameTf = new JTextField("10.0.55.152");
	JPasswordField mHostPWTf = new JPasswordField("password");
	JButton mConnectBtn = new JButton("Connect");
	JTextArea mJSInputTA = new JTextArea();
	JScrollPane mScrollPane = new JScrollPane(mJSInputTA);
	JTextArea mJSOutputTA = new JTextArea();
	JLabel mMessagesLbl = new JLabel("Constructor");
	PhotoshopImage mMainView = null;
	JScrollPane mLayersScrollPane = null;
	PhotoshopLayers mLayersView = null;
	Vector<PhotoshopEvent> mCheckboxs = new Vector<PhotoshopEvent>();

	static final long serialVersionUID = 1;
 
	/** images can come in two types, we use strings */
    private static final String JPEG_TYPE_STR = new String("1");
    private static final String PIXMAP_TYPE_STR = new String("2");

    /** for image type we have Pixmap and JPEG */
    private static final int JPEG_TYPE = Integer.parseInt(JPEG_TYPE_STR);
    private static final int PIXMAP_TYPE = Integer.parseInt(PIXMAP_TYPE_STR);
    
    /** Worker thread to process incoming messages from Photoshop */
    MessageProcessor mMessageProcessor = null;
    
    private final int COMPARETO_MATCH = 0;
    
	/** All the events that Photoshop can send.
	 * Subscribe to all of them, user can click off as needed.
	 * We will also have a separate list of events that out main document
	 * view cares about and our layer views care about.
	 */
    // TODO fix this and move it to the bottom of big list
    // TODO or should I just have an index into the main list
    private final int activeViewChanged_IDX 			= 0;
    private final int closedDocument_IDX 				= 1;
    private final int currentDocumentChanged_IDX 		= 2;
    private final int documentChanged_IDX 				= 3;
    @SuppressWarnings("unused")
	private final int imageChangedAppearance_IDX 		= 4;
    @SuppressWarnings("unused")
    private final int imageChangedColor_IDX 			= 5;
    @SuppressWarnings("unused")
    private final int imageChangedFlatData_IDX 			= 6;
    @SuppressWarnings("unused")
	private final int imageChangedFlattened_IDX 		= 7;
    @SuppressWarnings("unused")
    private final int imageChangedGlobalLight_IDX 		= 8;
    @SuppressWarnings("unused")
    private final int imageChangedLayers_IDX 			= 9;
    @SuppressWarnings("unused")
    private final int imageChangedLayerOrder_IDX 		= 10;
    @SuppressWarnings("unused")
    private final int imageChangedSelection_IDX 		= 11;
    @SuppressWarnings("unused")
    private final int imageChangedSize_IDX 				= 12;
    @SuppressWarnings("unused")
    private final int layerAdded_IDX 					= 13;
    private final int layerChanged_IDX 					= 14;
    @SuppressWarnings("unused")
    private final int layerChangedBlending_IDX 			= 15;
    @SuppressWarnings("unused")
    private final int layerChangedFX_IDX 				= 16;
    @SuppressWarnings("unused")
    private final int layerChangedGeneratedContent_IDX	= 17;
    @SuppressWarnings("unused")
    private final int layerChangedMerged_IDX 			= 18;
    @SuppressWarnings("unused")
    private final int layerChangedPath_IDX 				= 19;
    private final int layerChangedPixels_IDX 			= 20;
    @SuppressWarnings("unused")
    private final int layerChangedPlacedContent_IDX 	= 21;
    @SuppressWarnings("unused")
    private final int layerChangedPosition_IDX 			= 22;
    @SuppressWarnings("unused")
    private final int layerChangedProperties_IDX 		= 23;
    @SuppressWarnings("unused")
    private final int layerChangedProtection_IDX 		= 24;
    @SuppressWarnings("unused")
    private final int layerChangedRasterize_IDX 		= 25;
    @SuppressWarnings("unused")
    private final int layerChangedText_IDX 				= 26;
    @SuppressWarnings("unused")
    private final int ayerChangedTimeContent_IDX 		= 27;
    @SuppressWarnings("unused")
    private final int layerChangedTransform_IDX 		= 28;
    @SuppressWarnings("unused")
    private final int layerChangedUserMask_IDX 			= 29;
    @SuppressWarnings("unused")
    private final int layerChangedVisibility_IDX 		= 30;
    @SuppressWarnings("unused")
    private final int layerChanged3DLight_IDX 			= 31;
    @SuppressWarnings("unused")
    private final int layerChanged3DMaterial_IDX 		= 32;
    @SuppressWarnings("unused")
    private final int layerChanged3DMesh_IDX 			= 33;
    @SuppressWarnings("unused")
    private final int layerChanged3DGroup_IDX 			= 34;
    @SuppressWarnings("unused")
    private final int layerRemoved_IDX 					= 35;
    @SuppressWarnings("unused")
    private final int layerChangedAppearance_IDX		= 36;
    private final int generatorMenuChanged_IDX			= 37;
    private final int newDocumentViewCreated_IDX 		= 38;
    private final int lastPhotoshop_IDX = newDocumentViewCreated_IDX + 1;
    
    @SuppressWarnings("unused")
    private final int [] mMainViewEvents = {activeViewChanged_IDX
    };
    
    // TODO fix this and move it to the bottom of big list
    @SuppressWarnings("unused")
    private final int [] mLayerViewEvents = {activeViewChanged_IDX
    };

    private final String [] mAllPhotoshopEvents = {"activeViewChanged",
	                                               "closedDocument",
	                                               "currentDocumentChanged",
	                                               "documentChanged",
	                                               "imageChangedAppearance",
	                                               "imageChangedColor",
	                                               "imageChangedFlatData",
	                                               "imageChangedFlattened",
	                                               "imageChangedGlobalLight",
	                                               "imageChangedLayers",
	                                               "imageChangedLayerOrder",
	                                               "imageChangedSelection",
	                                               "imageChangedSize",
	                                               "layerAdded",
	                                               "layerChanged",
	                                               "layerChangedBlending",
	                                               "layerChangedFX",
	                                               "layerChangedGeneratedContent",
	                                               "layerChangedMerged",
	                                               "layerChangedPath",
	                                               "layerChangedPixels",
	                                               "layerChangedPlacedContent",
	                                               "layerChangedPosition",
	                                               "layerChangedProperties",
	                                               "layerChangedProtection",
	                                               "layerChangedRasterize",
	                                               "layerChangedText",
	                                               "layerChangedTimeContent",
	                                               "layerChangedTransform",
	                                               "layerChangedUserMask",
	                                               "layerChangedVisibility",
	                                               "layerChanged3DLight",
	                                               "layerChanged3DMaterial",
	                                               "layerChanged3DMesh",
	                                               "layerChanged3DGroup",
	                                               "layerRemoved",
	                                               "layerChangedAppearance",
	                                               "generatorMenuChanged",
	                                               "newDocumentViewCreated"
	};	
    
    private final int subscribeSuccess_IDX 		= 0;
    private final int documentID_IDX 			= 1;
    private final int documentName_IDX 			= 2;
    private final int javaScript_IDX 			= 3;
    

	/** Some made up names I created to send messages to myself */
    private final String [] mMadeUpEvents = { "subscribeSuccess",
    	                                      "documentID",
    	                                      "documentName",
    	                                      "javaScript"
    };

    /** time I requested last image */
    long mRequestTime = System.currentTimeMillis();
    
    /** timing of last image, not correct when there is a backlog of requests */
    double mLastTime = 0.0;
 
    /** try to auto connect on launch */
    public boolean mAutoConnect = false;

    /** logging on or off */
    static public boolean mLoggingOn = true;
    /** log all my messages here */
	static public final String mFileOut = new String("PhotoshopControllerLog.txt");
	
	public PhotoshopController() {
		
		setLayout(null);
		
		mMainView = new PhotoshopImage("Document 1", 390, 470);
		
		mHostNameTf.setBorder(BorderFactory.createLineBorder(Color.black));
		
		mHostPWTf.setBorder(BorderFactory.createLineBorder(Color.black));
		mJSInputTA.setBorder(BorderFactory.createLineBorder(Color.black));
		mJSOutputTA.setBorder(BorderFactory.createLineBorder(Color.black));

		mLayersView = new PhotoshopLayers(500, 50, 250, 850);
		mLayersScrollPane = new JScrollPane();
		mLayersScrollPane.setBounds(500, 50, 250, 820);
		mLayersScrollPane.setViewportView(mLayersView);

		mHostNameTf.setToolTipText("Press this to attempt connection.");

		add(mHostNameTf);
		add(mHostPWTf);
		add(mConnectBtn);
		
		add(mScrollPane);
		add(mJSOutputTA);
		
		add(mMainView);
		
		add(mLayersScrollPane);
		
		add(mMessagesLbl);
		
		addAllCheckboxs();

		// manually layout my items
		
		Insets insets = getInsets();
		Dimension size = mHostNameTf.getPreferredSize();
		mHostNameTf.setBounds(25 + insets.left, 5 + insets.top,
	             	          size.width + 20, size.height + 5);
		mHostPWTf.setBounds(200 + insets.left, 5 + insets.top,
	             	        size.width + 20, size.height + 5);
		mConnectBtn.setBounds(400 + insets.left, 5 + insets.top,
	             	          size.width + 20, size.height + 5);
		mScrollPane.setBounds(25, 600, 400, 120);
		mJSOutputTA.setBounds(25, 750, 400, 120);
		mMainView.setBounds(25, 50, 400, 600);
	
		mLayersView.setBounds(500, 50, 160, 800);
	
		mMessagesLbl.setBounds(25, 970, 400, 20);
		
		fixCheckBoxLayout();
	
		setSize(300 + insets.left + insets.right,
	            125 + insets.top + insets.bottom);
		setPreferredSize(new Dimension(1030, 1000));

   		mConnectBtn.addActionListener(new java.awt.event.ActionListener() {
			public void actionPerformed(java.awt.event.ActionEvent evt) {
				connectBtnActionPerformed(evt);
			}
		});
   		
   			
   	    KeyListener keyListener = new KeyListener() {
   	      public void keyPressed(KeyEvent keyEvent) {
   	    	  int keyCode = keyEvent.getKeyCode();
   	    	  if (keyEvent.isControlDown() && keyCode == KeyEvent.VK_ENTER) {
   	    		  String s = "result = eval('" + mJSInputTA.getText() + "');";
   	    		  s += "'" + mMadeUpEvents[javaScript_IDX] + "' + String.fromCharCode(13) + result;";
   	    		  mMessageProcessor.sendJavaScript(s); 
   	    	  }
   	      }

   	      public void keyReleased(KeyEvent keyEvent) {
   	      }

   	      public void keyTyped(KeyEvent keyEvent) {
   	      }

   	    };
   	    
   	    mJSInputTA.addKeyListener(keyListener);

   	    if (mAutoConnect)
   	    	tryToConnect();
   	    
	}

	private PhotoshopEvent addPhotoshopEvent(String inEvent) {
		PhotoshopEvent pe = new PhotoshopEvent(inEvent, 0);
		pe.setSelected(true);
		pe.addItemListener(new ItemListener() {
	      public void itemStateChanged(ItemEvent e) {
	    	JCheckBox cb = (JCheckBox)e.getItem();
	    	PhotoshopEvent pe = (PhotoshopEvent)cb.getParent();
			subscribeToEvent(pe.getEvent(), cb.isSelected());
	        logIt("Checked," + cb.getText() + "," + cb.isSelected());
	      }
	    });
		
		return pe;
		
	}
	
	private void addAllCheckboxs() {
		if (lastPhotoshop_IDX != mAllPhotoshopEvents.length)
			logIt("Master index does not match my length " + lastPhotoshop_IDX + " != " + mAllPhotoshopEvents.length);
		for (int i = 0; i < mAllPhotoshopEvents.length; i++) {
			PhotoshopEvent pe = addPhotoshopEvent(mAllPhotoshopEvents[i]);
			mCheckboxs.add(pe);
			add(pe);
		}
		for (int i = 0; i < mMadeUpEvents.length; i++) {
			PhotoshopEvent pe = addPhotoshopEvent(mMadeUpEvents[i]);
			mCheckboxs.add(pe);
			add(pe);
		}
	}

	private void fixCheckBoxLayout() {
		int dy = 20;
		for (int i = 0; i < mCheckboxs.size(); i++) {
			mCheckboxs.elementAt(i).setBounds(800, dy, 250, 20);
			dy += 20;
		}
	}
	
	private void updateCheckBoxCount(String inString) {
		boolean foundIt = false;
		for (int i = 0; i < mCheckboxs.size()&& ! foundIt; i++) {
			PhotoshopEvent pe = mCheckboxs.elementAt(i);
			if (pe.getEvent().compareTo(inString) == COMPARETO_MATCH) {
				pe.incrementCount();
				foundIt = true;
			}
		}
	}

   	private void connectBtnActionPerformed(java.awt.event.ActionEvent evt)  {
   		tryToConnect();
   		pretendToBeNode();
   	}

	/**
     * Create the GUI and show it.  For thread safety,
     * this method should be invoked from the
     * event-dispatching thread.
     */
    private static void createAndShowGUI() 
	{
    	try {
    		UIManager.setLookAndFeel(UIManager.getSystemLookAndFeelClassName());
    	} catch(Exception e) {
    		logIt("Error setting native LAF: " + e);
    	}
    	
    	
        JFrame frame = new JFrame("Photoshop Controller v0.5");
        frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);

        // try to give us a better icon, this won't let us quit and doesn't work 
    	//java.net.URL url = ClassLoader.getSystemResource("PSClientTestAppIcon.png");
    	//Toolkit kit = Toolkit.getDefaultToolkit();
    	//Image img = kit.createImage(url);
    	//frame.setIconImage(img);

    	JComponent newContentPane = new PhotoshopController();
        newContentPane.setOpaque(true);
        frame.setContentPane(newContentPane);

        frame.pack();
        frame.setVisible(true);

	}

    public static void main(String[] args) {
        javax.swing.SwingUtilities.invokeLater(new Runnable() {
            public void run() {
                createAndShowGUI();
            }
        });
    }


    /**
     * Show a short dialog of issues
     * @param inMessage - text to display
     */
    private void showMessage(String inMessage) {
    	mMessagesLbl.setText(inMessage);
    }

    
    /**
     * Try to connect to Photoshop
     */
    private void tryToConnect() {
    	try {
    		mMessageProcessor = new MessageProcessor(this, mHostNameTf.getText(), new String(mHostPWTf.getPassword()));
    		mMessageProcessor.start();
           	getAllIDs();
    		getAllNames();
           	subscribeToAllEvents();
           	showMessage("Connected");
    	}
    	catch (Exception e) {
            logIt( "tryToConnect exception: " + e.toString() );
            mMessageProcessor = null;
            setAllIDs("0");
            showMessage("Not Connected");
        } 
    }

    
    /**
     * Ask Photoshop to send us all the events it knows.
     */
    private void subscribeToAllEvents() {
    	for (int i = 0; i < mAllPhotoshopEvents.length; i++) {
    		subscribeToEvent(mAllPhotoshopEvents[i], true /* subscribe */ );
    	}
    }
    
    /**
     * Ask Photoshop for the current composite and all layers
     */
    private void getAllImages() {
   		mMessageProcessor.requestPhotoshopImage(mMainView.mDocumentID, 
   				                                mMainView.mLayerID, 
   				                                JPEG_TYPE, 
   				                                mMainView.getBitmapWidth(), 
   				                                mMainView.getBitmapHeight());
   		logIt("Main request of :" + 
   			  mMainView.mDocumentID + "," + 
   			  mMainView.mLayerID + "," + 
   			  mMainView.getBitmapWidth() + "," + 
   			  mMainView.getBitmapHeight());
   		
    	for (int i = 0; i < mLayersView.count(); i++) {
    		PhotoshopImage pi = mLayersView.elementAt(i);
    		if (pi.mDocumentID != 0 && pi.mLayerID != -1) {
    			mMessageProcessor.requestPhotoshopImage(pi.mDocumentID, 
    					                                pi.mLayerID, 
    					                                PIXMAP_TYPE, 
    					                                pi.getBitmapWidth(), 
    					                                pi.getBitmapHeight());
    			logIt("Layer request of :" + 
    				  pi.mDocumentID + "," +
    				  pi.mLayerID + "," + 
    				  pi.getBitmapWidth() + "," + 
    				  pi.getBitmapHeight());
    		}
    	}
    	showMessage("Ps Images " + Integer.toString(mMessageProcessor.queuedRequests()) + " " + Double.toString(mLastTime) + " " + Integer.toString(mMessageProcessor.pendingRequests()));
    	mRequestTime = System.currentTimeMillis();
    }
    
    /** 
     * Make a request to get this image.
     * @param inInfo - comma separated value of document and layer ID
     */
    private void getOneImage(String inInfo) {
    	String [] splitter = inInfo.split(",");
    	if (splitter.length != 2)
    		return;
    	int documentID = Integer.parseInt(splitter[0]);
    	int layerID = Integer.parseInt(splitter[1]);
    	getOneImage(documentID, layerID);
    }
    
    /** 
     * Make a request to get this image.
     * @param inDocumentID - document ID
     * @param inLayerID - layer ID
     */
     private void getOneImage(int inDocumentID, int inLayerID) {
    	if (mMainView.mDocumentID == inDocumentID && mMainView.mLayerID == inLayerID) {
    		mMessageProcessor.requestPhotoshopImage(mMainView.mDocumentID, 
    				                                mMainView.mLayerID, 
    				                                JPEG_TYPE, 
    				                                mMainView.getBitmapWidth(), 
    				                                mMainView.getBitmapHeight());
       		logIt("One Main request of :" + 
       			  mMainView.mDocumentID + "," + 
       			  mMainView.mLayerID + "," + 
       			  mMainView.getBitmapWidth() + "," + 
       			  mMainView.getBitmapHeight());
       		
    	} else {
    		for (int i = 0; i < mLayersView.count(); i++) {
    			PhotoshopImage pi = mLayersView.elementAt(i);
    			if (pi.mDocumentID == inDocumentID && pi.mLayerID == inLayerID) {
    				mMessageProcessor.requestPhotoshopImage(pi.mDocumentID, 
    						                                pi.mLayerID, 
    						                                PIXMAP_TYPE, 
    						                                pi.getBitmapWidth(), 
    						                                pi.getBitmapHeight());
        			logIt("One Layer request of :" + 
        				  pi.mDocumentID + "," + 
        				  pi.mLayerID + "," + 
        				  pi.getBitmapWidth() + "," + 
        				  pi.getBitmapHeight());
        			
    				break;
    			}
    		}
       	}
    	showMessage("Ps Images " + Integer.toString(mMessageProcessor.queuedRequests()) + " " + Double.toString(mLastTime) + " " + Integer.toString(mMessageProcessor.pendingRequests()));
    	mRequestTime = System.currentTimeMillis();
    }
    
    /**
     * IDs of the active document and all the layers from Photoshop
     * @param inIDs - comma separated list of IDs, first one is document ID
     */
    private void setAllIDs(String inIDs) {
    	mLayersView.removeAllImages();
		String [] splitter = inIDs.split(",");
		mMainView.mDocumentID = Integer.parseInt(splitter[0]);
    	for (int i = 1; i < splitter.length; i++) {
    		mLayersView.add("Fake", mMainView.mDocumentID, Integer.parseInt( splitter[ i  ] ) );
    	}
    	if (mMainView.mDocumentID != 0)
    		getAllImages();
    	else
    		mMainView.noImage();
    }

    /**
     * Ask Photoshop for the current document ID and all the layer IDs
     * The unique ID is so all kinds of characters can come in.
     */
    private void setAllNames(String inNames) {
		String [] splitter = inNames.split("556f6f30-ed57-4903-8da4-de913098173a");
    	mMainView.setName(splitter[0]);
    	for (int i = 0; i < mLayersView.count() && i < (splitter.length - 1); i++) {
    		PhotoshopImage pi = mLayersView.elementAt(i);
    		pi.setName(splitter[i+1]);
    	}
    }
    
    /**
     * Ask Photoshop for the current document ID and all the layer IDs
     */
    private void getAllIDs() {
    	String s = "var docIDs = [0];";
    	s += "if (app.documents.length)";
    	s += "{";
    	s += "  docIDs[0] = activeDocument.id;";
    	s += "  for (var i = 0; i < activeDocument.artLayers.length; i++) {";
    	s += "    docIDs.push(activeDocument.artLayers[i].id);";
    	s += "  }";
    	s += "}";
    	s += "'" + mMadeUpEvents[documentID_IDX] + "' + String.fromCharCode(13) + docIDs;";
    	if (mMessageProcessor != null) {
    		mMessageProcessor.sendJavaScript(s);
    	}
    }
    
    private void pretendToBeNode() {
    	String s = "var IDStr = stringIDToTypeID( 'ID' );";
    	s += "var knodeConnectionStr = stringIDToTypeID( 'nodeConnection' );";
    	s += "var desc = new ActionDescriptor();";
    	s += "desc.putLargeInteger( IDStr, 4147 );";
    	s += "executeAction( knodeConnectionStr, desc, DialogModes.NO );";
    	s += "'" + mAllPhotoshopEvents[generatorMenuChanged_IDX] + "' + String.fromCharCode(13);";
    	if (mMessageProcessor != null) {
    		mMessageProcessor.sendJavaScript(s);
    	}

    }
    /**
     * Ask Photoshop for the current document name and all the layer names
     */
    private void getAllNames() {
    	String s = "var docNames = 'Unknown';";
    	s += "if (app.documents.length)";
    	s += "{";
    	s += "  docNames = activeDocument.name;";
    	s += "  for (var i = 0; i < activeDocument.artLayers.length; i++) {";
    	s += "    docNames += '556f6f30-ed57-4903-8da4-de913098173a';";
    	s += "    docNames += activeDocument.artLayers[i].name;";
    	s += "  }";
    	s += "}";
    	s += "'" + mMadeUpEvents[documentName_IDX] + "' + String.fromCharCode(13) + docNames;";
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
	private void subscribeToEvent(String inEvent, boolean inSubscribe) {
		String s;
		if (inSubscribe)
			s = "var idNS = stringIDToTypeID( 'networkEventSubscribe' );";
		else
			s = "var idNS = stringIDToTypeID( 'networkEventUnsubscribe' );";
		s += "var desc1 = new ActionDescriptor();";
		s += "desc1.putClass( stringIDToTypeID( 'eventIDAttr' ), stringIDToTypeID( '" + inEvent + "' ) );";
		s += "executeAction( idNS, desc1, DialogModes.NO );";
		s += "'" + mMadeUpEvents[subscribeSuccess_IDX] + "' + String.fromCharCode(13) + 'YES';";
		if (mMessageProcessor != null) {
			mMessageProcessor.sendJavaScript(s);
		}
	}

	
    /**
     * Called from the worker thread to tell us
     * something has happened with documents
     */
    public void setDocChanged() {
    	getAllIDs();
    	getAllNames();
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
    	logIt("runFromThread " + inCommand + " : " + inExtra);
    	updateCheckBoxCount(inCommand);
    	if (inCommand.compareTo(mAllPhotoshopEvents[closedDocument_IDX]) == COMPARETO_MATCH) {
        	setDocChanged();
        } else if (inCommand.compareTo(mAllPhotoshopEvents[newDocumentViewCreated_IDX]) == COMPARETO_MATCH) {
        	setDocChanged();
        } else if (inCommand.compareTo(mAllPhotoshopEvents[currentDocumentChanged_IDX]) == COMPARETO_MATCH) {
        	setDocChanged();
        } else if (inCommand.compareTo(mAllPhotoshopEvents[activeViewChanged_IDX]) == COMPARETO_MATCH) {
        	setDocChanged();
        } else if (inCommand.compareTo(mMadeUpEvents[documentID_IDX]) == COMPARETO_MATCH) {
        	setAllIDs(inExtra);
        } else if (inCommand.compareTo(mMadeUpEvents[documentName_IDX]) == COMPARETO_MATCH) {
        	setAllNames(inExtra);
        } else if (inCommand.compareTo(mAllPhotoshopEvents[documentChanged_IDX]) == COMPARETO_MATCH) {
        	getOneImage(mMainView.mDocumentID, mMainView.mLayerID);
        } else if (inCommand.compareTo(mAllPhotoshopEvents[layerChanged_IDX]) == COMPARETO_MATCH) {
        	getOneImage(inExtra);
        } else if (inCommand.compareTo(mAllPhotoshopEvents[layerChangedPixels_IDX]) == COMPARETO_MATCH) {
        	getOneImage(inExtra);
       	} else if (inCommand.compareTo(mMadeUpEvents[subscribeSuccess_IDX]) == COMPARETO_MATCH) {
        	; // nothing on purpose
       	} else if (inCommand.compareTo("Image Request Sent") == COMPARETO_MATCH) {
        	; // nothing on purpose
        } else if (inCommand.compareTo(mMadeUpEvents[javaScript_IDX]) == COMPARETO_MATCH) {
        	mJSOutputTA.setText(inExtra);
        } else if (inCommand.compareTo(mAllPhotoshopEvents[generatorMenuChanged_IDX]) == COMPARETO_MATCH) {
        	mJSOutputTA.setText(inExtra);
        } else {
        	showMessage(inCommand + ":" + inExtra);
        }
    }
    
    
    /**
     * Photoshop just sent me a image, figure out which view needs it.
     * @param inBytes - all the bytes for the image
     * @param inIndexer - the offset we are currently at in the inBytes array
     * @param inDocumentID - string for the id of the document this image pertains to
     * @param inLayerID - string for the id of the layer this image pertains to, "0" means entire image
     */
    public void runFromThread(byte [] inBytes, int inIndexer, int inDocumentID, int inLayerID) {    	
    	PhotoshopImage updateImage = mMainView;
    	// TODO double check this as I get out of sync pretty easy, for now just default to mainView
    	//if (inDocumentID == mMainView.documentID && inLayerID == mMainView.layerID) {
    	//	updateImage = mMainView;
    	//} else {
    		for (int i = 0; i < mLayersView.count(); i++) {
    			PhotoshopImage pi = mLayersView.elementAt(i);
    			if (pi.mLayerID == inLayerID && pi.mDocumentID == inDocumentID){
    				updateImage = pi;
    				break;
    			}
    		}   
    	//}
    	//if (updateImage == null) {
    	//	showMessage("Could not find something to update!");
    	//	return;
    	//}
    	byte format = inBytes[inIndexer++];
    	if (JPEG_TYPE == format) {
    		String result = updateImage.createBitmapFromJPEG(inBytes, inIndexer);
    		if (null != result) {
    			logIt(result);
    			showMessage(result);
    		}
    	} else if (PIXMAP_TYPE == format) {
    		String result = updateImage.createBitmap(inBytes, inIndexer);
    		if (null != result) {
    			logIt(result);
    			showMessage(result);
    		}
    	}
    	updateImage.invalidate();
    	mLastTime = ((double)System.currentTimeMillis() - (double)mRequestTime) / 1000.0;
    	showMessage("Ps Images " + 
    			            Integer.toString(mMessageProcessor.queuedRequests()) + " " + 
    			            Double.toString(mLastTime) + " " + 
    			            Integer.toString(mMessageProcessor.pendingRequests()));
		logIt("Bytes of :" + inDocumentID + "," + inLayerID);
    }

    /** 
     * Log to disk to help with debugging.
     * 
     * @see PhotoshopController#mLoggingOn mLoggingOn
     * @see PhotoshopController#mFileOut mFileOut
     * 
     * @param inString - message to log to file
     */
    static public void logIt(String inString) {
    	try {
    		if ( ! mLoggingOn ) return;
    		PrintWriter out = new PrintWriter(new FileWriter(mFileOut, true /* append to end */));
			out.println(inString);
			out.close();
    	} catch (IOException e) {
    		e.printStackTrace();
    	}
    }
    
} /* end public class PhotoshopController class */

// end PhotoshopController.java

	// TODO do something with these
	// #define kconvertToWorkingRGBProfileStr "convertToWorkingRGBProfile"
	// #define kincludeNoneStr "includeNone"
	// #define kincludeVisibleStr "includeVisible"
	// #define kincludeAllStr "includeAll"

// TODO do something with all the options to the image grabbing
// see layer thumbnail email about descriptor



