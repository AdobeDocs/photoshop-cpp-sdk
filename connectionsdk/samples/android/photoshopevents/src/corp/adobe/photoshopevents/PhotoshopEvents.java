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

/**  Filename: PhotoshopEvents.java
	@author Thomas Ruark, Photoshop Engineering, Adobe Systems Incorporated
*/

package corp.adobe.photoshopevents;


// import java.io.IOException;
// import java.net.UnknownHostException;

import android.app.Activity;
import android.graphics.Color;
import android.os.Bundle;
import android.os.SystemClock;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.Spinner;
import android.widget.Toast;
import android.view.MenuInflater;
import android.view.View;
import android.view.Menu;
import android.view.MenuItem;
import android.widget.TextView;
import android.widget.AdapterView.OnItemSelectedListener;
import android.widget.AdapterView;


/**
 * Android version for setting up and getting events from a Photoshop server.
 * This version is going to try and connect at launch and the button will
 * reflect the state of the connection. Clicking on it will bring up a dialog
 * to pick the server name and server port.
*/
public class PhotoshopEvents extends Activity
    implements ColorPickerDialog.OnColorChangedListener, ConfigureDialog.OnDoneListener {    

	/** Name of the server to connect */
	private String mServerNameText = new String("192.168.1.101");
	
	/** Server password to use */
	private String mServerPasswordText = new String("Swordfish");

	/** some constants to make things clearer */
	final private boolean BACKGROUND = false;
	final private boolean FOREGROUND = true;
	
	/** set this before popping the color picker dialog */
	private boolean mPickingForeground = true;

	/** Foreground color and button */
	private TextView mForegroundTxt = null;
	private Button mForegroundBtn = null;
	private int mForeColor = Color.parseColor("#000000");
	
	/** Background color and button */
	private Button mBackgroundBtn = null;
	private int mBackColor = Color.parseColor("#FFFFFF");
	
	/** unique string to split document names, this hopefully will
	 * allow us to pass all kinds of nice document names
	 */
	final private String SPLITTER = new String("8F6AFB7E-EC1F-4b6f-AD15-C1AF34221EED");

	/** Special string for us to track that documents are open */
	final private String mNoOpenDocuments = new String("No open documents");

	/** Current tool and list of all tools */
	private Spinner mToolSpn = null;
	
	/** Flag so we don't get events from Photoshop back to Photoshop via the spinner listener. */
	private boolean mInternalToolSpinnerUpdate = false;
	
	/** Current document and list of all documents */
	private Spinner mDocSpn = null;
	
	/** Flag so we don't get events from Photoshop back to Photoshop via the spinner listener. */
	private boolean mInternalDocumentSpinnerUpdate = false;
	
    /** Worker thread to process incoming messages from Photoshop */
    MessageProcessor mMessageProcessor = null;
    
    /** Messages that will come back to us from Photoshop when things change on the Photoshop side */
    final private String FOREGROUND_COLOR_CHANGED_STR = new String("foregroundColorChanged");
    final private String BACKGROUND_COLOR_CHANGED_STR = new String("backgroundColorChanged");
    final private String TOOL_CHANGED_STR = new String("toolChanged");
    final private String CLOSED_DOCUMENT_STR = new String("closedDocument");
    final private String NEW_DOCUMENT_VIEW_CREATED_STR = new String("newDocumentViewCreated");
    final private String CURRENT_DOCUMENT_CHANGED_STR = new String("currentDocumentChanged");
    final private String ACTIVE_VIEW_CHANGED_STR = new String("activeViewChanged");
    
    /** Some made up names I created to send messages to myself */
    final private String SUBSCRIBE_SUCCESS_STR = new String("subscribeSuccess");
    final private String DOCUMENT_NAMES_CHAGNED_STR = new String("documentNamesChanged");
    final private String ACTIVE_DOCUMENT_STR = new String("activeDocument");

	/** Handy flag for debugging the messages from Photoshop or back from the JavaScript sent */
	boolean mShowAllMessages = false;

    /** Called when the activity is first created. 
     * Connect my local variables to the UI widgets in main.xml.
     */
    @Override
    public void onCreate(Bundle savedInstanceState) {
    	
    	long startTime = SystemClock.uptimeMillis();
    	
    	super.onCreate(savedInstanceState);
    	setContentView(R.layout.main);
    
    	mForegroundTxt = (TextView) findViewById(R.id.foregroundTxt);
    	mForegroundBtn = (Button) findViewById(R.id.foregroundBtn);
    	mBackgroundBtn = (Button) findViewById(R.id.backgroundBtn);
    	mToolSpn = (Spinner) findViewById(R.id.toolSpn);
    	mDocSpn = (Spinner) findViewById(R.id.docSpn);
       
        ArrayAdapter<CharSequence> adapter = ArrayAdapter.createFromResource(
        		this, R.array.tools_array, android.R.layout.simple_spinner_item);
        adapter.setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item);
        mToolSpn.setAdapter(adapter);

        /**
         * Select the tool when we change the spinner
         */
        mToolSpn.setOnItemSelectedListener(
        		new OnItemSelectedListener() {
        			public void onItemSelected(
                            AdapterView<?> parent, View view, int position, long id) {
        				if (mInternalToolSpinnerUpdate) {
        					mInternalToolSpinnerUpdate = false;
        				} else {
        					toolsChangeHandler(parent.getItemAtPosition(position).toString());
        				}
                    }

                    public void onNothingSelected(AdapterView<?> parent) {
                        ; // do nothing
                    }
                });

        /**
         * Select the document when we change the spinner
         */
        mDocSpn.setOnItemSelectedListener(
        		new OnItemSelectedListener() {
        			public void onItemSelected(
                            AdapterView<?> parent, View view, int position, long id) {
        				if (mInternalDocumentSpinnerUpdate) {
        					mInternalDocumentSpinnerUpdate = false;
        				} else {
        					docChangeHandler(parent.getItemAtPosition(position).toString());
        				}
                    }

                    public void onNothingSelected(AdapterView<?> parent) {
                        ; // do nothing
                    }
                });

		
		ToolStringConverter.mToolNames = getResources().getStringArray(R.array.tools_array);
		ToolStringConverter.mToolIDs = getResources().getStringArray(R.array.toolids_array);

        tryToConnect();
        
        showMessage("Launch time " + new Integer((int)((int)SystemClock.uptimeMillis() - (int)startTime)).toString());
    
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
     * Called from ColorPickerDialog to let us know the new color.
     */
    public void colorChanged(int color) {
        if (mPickingForeground) {
        	mForegroundBtn.setBackgroundColor(color);
        	mForeColor = color;
        } else {
        	mBackgroundBtn.setBackgroundColor(color);
        	mBackColor = color;
        }
        setPhotoshopColor(mPickingForeground, color);
    }


    /**
     * Called by the ConfigureDialog class
     */
    public void optionsChanged(String server, String password) {
    	mServerNameText = server;
    	mServerPasswordText = password;
    	tryToConnect();
    }
    
    /**
     * Send the color to Photoshop
     * @param inForeground - true for foreground, false for background
     * @param inColor - Android Color value
     */
    private void setPhotoshopColor(boolean inForeground, int inColor) {
    	String s = "var c = new SolidColor;";
    	s += "c.rgb.red =   " + Color.red(inColor) + ";";
    	s += "c.rgb.green = " + Color.green(inColor) + ";";
    	s += "c.rgb.blue =  " + Color.blue(inColor) + ";";
    	if (inForeground)
    		s += "app.foregroundColor = c;";
    	else
    		s += "app.backgroundColor = c;";
    	s += "'Color Message Sent';";
    	if (mMessageProcessor != null)
    		mMessageProcessor.sendJavaScript(s);
    }


    /**
     * Ask Photoshop to give us the color.
     * I'm going to fake the same message that Photoshop would send us
     * if a user changed the color.
     * @param inForeground - true for foreground, false for background
     * @return Color as AARRGGBB, AA (alpha) is not supported, RRGGBB
     */
    private void getPhotoshopColor(boolean inForeground) {
    	String s = "";
    	if (inForeground)
    		s += "'" + FOREGROUND_COLOR_CHANGED_STR + "'";
    	else
    		s += "'" + BACKGROUND_COLOR_CHANGED_STR + "'";
    	s += " + String.fromCharCode(13) + app.";
    	if (inForeground)
    		s += "foregroundColor.";
    	else
    		s += "backgroundColor.";
    	s += "rgb.hexValue.toString();";
    	if (mMessageProcessor != null)
    		mMessageProcessor.sendJavaScript(s);
    }

    
    /**
     * Show a short dialog of issues
     * @param inMessage - text to display
     */
    private void showMessage(String inMessage) {
    	Toast.makeText(PhotoshopEvents.this, inMessage, Toast.LENGTH_SHORT).show();
    }

    
    /**
     * Try to connect to Photoshop
     */
    private void tryToConnect() {
    	try {
    		mMessageProcessor = new MessageProcessor(this, mServerNameText, mServerPasswordText);
    		mMessageProcessor.start();
            if (mMessageProcessor.mIsConnected) {
    			mForegroundTxt.setTextColor(Color.parseColor("#FFFFFF"));
            	setupEvents();
            	getDocNames();
            	getActiveDocumentName();
            	getActiveTool();
            	getColors();
    		} else {
    			mForegroundTxt.setTextColor(Color.parseColor("#FF0000"));
            }
    	}
    	catch (Exception e) {
            showMessage( e.toString() );
			mForegroundTxt.setTextColor(Color.parseColor("#FF0000"));
        } 
    }

    
    /**
     * Ask Photoshop to send us when a document, tool or color has changed.
     * Get the event ID after or we will think Photoshop just sent us that event.
     * 
     */
    private void setupEvents() {
    	subscribeToEvent(FOREGROUND_COLOR_CHANGED_STR);
    	subscribeToEvent(BACKGROUND_COLOR_CHANGED_STR);
    	subscribeToEvent(TOOL_CHANGED_STR);
    	subscribeToEvent(CLOSED_DOCUMENT_STR);
    	subscribeToEvent(NEW_DOCUMENT_VIEW_CREATED_STR);
    	subscribeToEvent(CURRENT_DOCUMENT_CHANGED_STR);
    	subscribeToEvent(ACTIVE_VIEW_CHANGED_STR);
    	subscribeToEvent("colorSettingsChanged");
    	subscribeToEvent("keyboardShortcutsChanged");
    	subscribeToEvent("preferencesChanged");
    	subscribeToEvent("quickMaskStateChanged");
    	subscribeToEvent("screenModeChanged");
    	subscribeToEvent("gaussianBlur");
    }

    /**
     * Ask Photoshop for all the document names, put them in Spinner
     */
    private void getDocNames() {
    	String s = "var docnames = '';";
    	s += "for (var i = 0; i < app.documents.length; i++)";
    	s += "{";
    	s += "  docnames += app.documents[i].name;";
    	s += "  if ((i + 1) != app.documents.length) {";
    	s += "    docnames += '" + SPLITTER + "'";
    	s += "  }";
    	s += "}";
    	s += "if (docnames.length == 0) {";
    	s += "  docnames = '" + mNoOpenDocuments + "';";
    	s += "}";
    	s += "'" + DOCUMENT_NAMES_CHAGNED_STR + "' + String.fromCharCode(13) + docnames;";
    	if (mMessageProcessor != null)
    		mMessageProcessor.sendJavaScript(s);
    }
    
    
    /**
     * Called from the thread to give us the document names
     */
    private void updateDocSpinner(String inDocNames) {
    	if (inDocNames.length() > 0) {
    		String[] docNames = inDocNames.split(SPLITTER);
    		ArrayAdapter<String> adapter = new ArrayAdapter<String>(this, android.R.layout.simple_spinner_item, docNames);
    		adapter.setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item);
    		mInternalDocumentSpinnerUpdate = true;
    		mDocSpn.setAdapter(adapter);
    	}
    }

    
    /** 
     * Ask Photoshop to give us the current name of the active document
     * so our spinner is in sync.
     */
    private void getActiveDocumentName() {
    	String s = "var s = '" + ACTIVE_DOCUMENT_STR + "' + String.fromCharCode(13);";
    	s += "if (documents.length) s += activeDocument.name; else s += '" + mNoOpenDocuments + "'; s;";
    	if (mMessageProcessor != null)
    		mMessageProcessor.sendJavaScript(s);
    }
    
    
    /**
     * Called from the thread to give us the active document name
     */
    private void updateActiveDocument(String inActiveDocument) {
    	if (inActiveDocument.length() > 0) {
        	int i = 0;
        	boolean docSet = false;
        	for (; i < mDocSpn.getCount(); i++) {
        		if (mDocSpn.getItemAtPosition(i).toString().compareTo(inActiveDocument) == 0) {
        			docSet = true;
        			break;
        		}
        	}
        	if (docSet) {
        		if (mDocSpn.getSelectedItemPosition() != i) {
            		mInternalDocumentSpinnerUpdate = true;
            		mDocSpn.setSelection(i);
        		}
        	} else {
        		showMessage("No entry for " + inActiveDocument);
        	}
    	} 
    }

    
    /**
     * Ask Photoshop what the current tool is. 
     * It will return the ID as a string which we then look up.
     * Then we set the spinner to that tool.
     */
    private void getActiveTool() {
    	String s = "";
    	s += "var ref = new ActionReference();";
    	s += "ref.putProperty( charIDToTypeID( 'Prpr' ), charIDToTypeID( 'Tool' ) );";
    	s += "ref.putEnumerated( charIDToTypeID( 'capp' ),  charIDToTypeID( 'Ordn' ), charIDToTypeID( 'Trgt' ) );";
    	s += "var desc = executeActionGet( ref );";
    	s += "var theTool = desc.getEnumerationType( charIDToTypeID( 'Tool' ) );";
    	s += "'" + TOOL_CHANGED_STR + "' + String.fromCharCode(13) + typeIDToStringID( theTool );";
    	if (mMessageProcessor != null)
    		mMessageProcessor.sendJavaScript(s);
    }
    
    
    /**
     * Ask Photoshop what the current foreground and background
     * colors are. Set up the button background colors and set
     * up our trackers of last color.
     */
    private void getColors() {
    	getPhotoshopColor(FOREGROUND);
    	getPhotoshopColor(BACKGROUND);
    }

    
    /**
     * Someone picked a new document from the spinner.
     * Tell Photoshop to switch that document to the front.
     * @param inDoc - name of document to switch to
     */
    private void docChangeHandler(String inDoc) {
    	if (inDoc.compareTo(mNoOpenDocuments) != 0) {
    		String s = "try { app.activeDocument = app.documents['" + inDoc + "'];";
    		s +=       "'Changed Document' + String.fromCharCode(13) + 'OK'; }";
    		s +=       "catch(e) { 'Changed Document' + String.fromCharCode(13) + 'FAILED'; }";
    		if (mMessageProcessor != null)
    			mMessageProcessor.sendJavaScript(s);
    	}
    }

    
    /**
     * Helper routine that maps the nice name from the spinner.
     * To the ID of the tool for Photoshop.
     * @param inTool - nice name from spinner
     */
    private void toolsChangeHandler(String inTool) {
    	String selectedTool = ToolStringConverter.UItoToolID(inTool);
		String s = "";
		s += "try {";
		s += " var id93 = charIDToTypeID( 'slct' );";
		s += " var desc32 = new ActionDescriptor();";
		s += " var id94 = charIDToTypeID( 'null' );";
		s += " var ref30 = new ActionReference();";
		s += " ref30.putClass( stringIDToTypeID( '" + selectedTool + "' ) );";
		s += " desc32.putReference( id94, ref30 );";
		s += " executeAction( id93, desc32, DialogModes.NO );";
		s += " 'SelectTool' + String.fromCharCode(13) + 'OK'; ";
		s += "}";
		s += "catch(e) {";
		s += " 'SelectTool' + String.fromCharCode(13) + 'FAIL';";
		s += "}";
		
		if (mMessageProcessor != null)
			mMessageProcessor.sendJavaScript(s);
    }


    /**
     * Called when the foreground button is clicked by the user.
     * Let the user pick a color and send that to Photoshop.
     * @param v - the View, not used
     */
    public void foregroundBtnClick(View v) {
    	mPickingForeground = true;
    	new ColorPickerDialog(this, this, mForeColor).show();
    }

    
    /**
     * Called when the background button is clicked by the user.
     * Let the user pick a color and send that to Photoshop.
     * @param v - the View ? not used
     */
    public void backgroundBtnClick(View v) {
    	mPickingForeground = false;
    	new ColorPickerDialog(this, this, mBackColor).show();
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
		s += "'" + SUBSCRIBE_SUCCESS_STR + "'";
		if (mMessageProcessor != null)
			mMessageProcessor.sendJavaScript(s);
	}

	
    /**
     * Called from the worker thread to tell us
     * something has happened with documents
     * for now I'm just going to get the list again
     */
    public void setDocChanged() {
    	getDocNames();
    	getActiveDocumentName();
    }

    
    /**
     * Called from the worker thread to set the new tool
     */
    public void setTool(String inTool) {
    	if (inTool != null) {
    		String toolName = ToolStringConverter.ToolIDtoUI(inTool);
        	int i = 0;
        	boolean toolSet = false;
        	for (; i < mToolSpn.getCount() && ! toolSet; i++) {
        		if (mToolSpn.getItemAtPosition(i).toString().compareTo(toolName) == 0) {
        			toolSet = true;
        			break;
        		}
        	}
        	if (toolSet) {
        		if (mToolSpn.getSelectedItemPosition() != i) {
        			mInternalToolSpinnerUpdate = true;
        			mToolSpn.setSelection(i);
        		}
        	} else {
        		showMessage("No entry for " + inTool);
        	}
    	}
    }


    /**
     * Called from the worker thread to set the current color
     * @param inFore set the foreground color, false background
     * @param inColor new color value as RRGGBB string
     */
    private void setColor(boolean inForeground, String inColor) {
    	int c = 0;
		if (inColor.length() == 6) {
			c = Color.parseColor("#" + inColor);
	    	Button b = mForegroundBtn;
			if (inForeground) {
				mForeColor = c;
			} else {
				mBackColor = c;
				b = mBackgroundBtn;
			}
			b.setBackgroundColor(c);
			b.setText(inColor);
			if (Color.green(c) < 100 ||
			    Color.red(c) < 100 ||
			    Color.blue(c) < 100) {
				b.setTextColor(Color.WHITE);
			} else {
				b.setTextColor(Color.BLACK);
			}
		}
    }
    
    /**
     * The thread will call us here with either a message from the
     * Photoshop JavaScript we sent or from something happening
     * in from Photoshop getting changed. This is why we have code
     * above that fakes the same messages as Photoshop will send us as
     * we really don't care where the originated from.
     */
    public void runFromThread(String inCommand, String inExtra) {
    	if (mShowAllMessages) {
    		showMessage(inCommand + ":" + inExtra);
    	}
    	if (inCommand.compareTo(FOREGROUND_COLOR_CHANGED_STR) == 0) {
    		setColor(true, inExtra);
        } else if (inCommand.compareTo(BACKGROUND_COLOR_CHANGED_STR) == 0) {
        	setColor(false, inExtra);
        } else if (inCommand.compareTo(CLOSED_DOCUMENT_STR) == 0) {
        	setDocChanged();
        } else if (inCommand.compareTo(NEW_DOCUMENT_VIEW_CREATED_STR) == 0) {
        	setDocChanged();
        } else if (inCommand.compareTo(CURRENT_DOCUMENT_CHANGED_STR) == 0) {
        	setDocChanged();
        } else if (inCommand.compareTo(ACTIVE_VIEW_CHANGED_STR) == 0) {
        	setDocChanged();
        } else if (inCommand.compareTo(TOOL_CHANGED_STR) == 0) {
        	setTool(inExtra);
        } else if (inCommand.compareTo(DOCUMENT_NAMES_CHAGNED_STR) == 0) {
        	updateDocSpinner(inExtra);
        } else if (inCommand.compareTo(ACTIVE_DOCUMENT_STR) == 0) {
        	updateActiveDocument(inExtra);
       	} else if (inCommand.compareTo(SUBSCRIBE_SUCCESS_STR) == 0) {
        	; // this is good nothing to do for these
        } else {
        	showMessage(inCommand + ":" + inExtra);
        }
    }
 } /* end public class PhotoshopEvents extends Activity */
// end PhotoshopEvents.java
