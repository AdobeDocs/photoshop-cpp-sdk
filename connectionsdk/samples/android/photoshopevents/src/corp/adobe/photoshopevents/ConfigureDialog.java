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

/**  Filename: ConfigureDialog.java
	@author Thomas Ruark, Photoshop Engineering, Adobe Systems Incorporated
*/

package corp.adobe.photoshopevents;

import corp.adobe.photoshopevents.R;

import android.os.Bundle;
import android.app.Dialog;
import android.content.Context;
import android.view.View;
import android.widget.EditText;
import android.widget.Button;

/**
 * Pop a dialog to allow us to configure our settings.
 */
public class ConfigureDialog extends Dialog {

    public interface OnDoneListener {
        void optionsChanged(String server, String password);
    }

    private OnDoneListener mListener;
    private String mInitialServer;
    private String mInitialPassword;

    private EditText mServer;
    private EditText mPassword;
    private Button   mOKBtn;
        
    public ConfigureDialog(Context context,
    					   OnDoneListener listener,
                           String initialServer,
                           String initialPassword) {
        super(context);
        
        mListener = listener;
        mInitialServer = initialServer;
        mInitialPassword = initialPassword;
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        setContentView(R.layout.configure_dialog);
        setTitle("Configure Photoshop");
    	mServer = (EditText)findViewById(R.id.serverNameText);
    	mServer.setText(mInitialServer);
    	mPassword = (EditText)findViewById(R.id.serverPasswordText);
    	mPassword.setText(mInitialPassword);
    	mOKBtn = (Button)findViewById(R.id.okBtn);

    	mOKBtn.setOnClickListener(new View.OnClickListener() {
    		public void onClick(View v) {
    	    	mListener.optionsChanged(mServer.getText().toString(), mPassword.getText().toString());
    	    	dismiss();
    		}
    	});
    	
    }

}
