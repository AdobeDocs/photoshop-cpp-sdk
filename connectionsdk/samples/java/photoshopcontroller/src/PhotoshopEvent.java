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

/**  Filename: PhotoshopEvent.java
	@author Thomas Ruark, Photoshop Engineering, Adobe Systems Incorporated
*/


import java.awt.Dimension;
import java.awt.event.ItemListener;

import javax.swing.BoxLayout;
import javax.swing.JCheckBox;
import javax.swing.JLabel;
import javax.swing.JPanel;

/**
 * List out a specific event by name and 
 * keep a running total of how many times we got the event.
 */
class PhotoshopEvent extends JPanel {

	static final long serialVersionUID = 1;
	 
	/** width of entire view */
	private int mViewWidth = 0;

	/** height of entire view */
	private int mViewHeight = 0;

	/** our event name */
	private JCheckBox mNameCb = null;
	
	/** our event count */
	private JLabel mCountLbl = null;
	
	/**
	 * Constructor for our main view.
	 * 
	 * @param inName - name of the event we are tracking
	 * @param inCount - current count to init with
	 */
	public PhotoshopEvent(String inName, int inCount) {
		mNameCb = new JCheckBox(inName);
		mCountLbl = new JLabel(String.valueOf(inCount));

		Dimension d = mCountLbl.getPreferredSize();
		d.setSize(d.width + 100, d.height);

		mViewWidth = d.width;
		mViewHeight = d.height;

		setLayout(new BoxLayout(this, BoxLayout.X_AXIS));
    
		add(mNameCb);
		add(mCountLbl);
	}


	/** current width */
	public int getViewWidth() {
		return mViewWidth;
	}
	
	/** current height */
	public int getViewHeight() {
		return mViewHeight;
	}
	
	/** event occured up my count by 1 */
	public void incrementCount() {
		int i = Integer.parseInt(mCountLbl.getText());
		mCountLbl.setText(String.valueOf(i+1));
	}
	
	/** report back my current count */
	public int getCount() {
		int i = Integer.parseInt(mCountLbl.getText());
		return i;
	}

	/** report back the event I am tracking */
	public String getEvent() {
		return mNameCb.getText();
	}
	
	/** turn me on or off */
	public void setSelected(boolean inValue) {
		mNameCb.setSelected(inValue);
	}
	
	/** click tracker so I can turn me on or off */
	public void addItemListener(ItemListener inItemListener) {
		mNameCb.addItemListener(inItemListener);
	}
	
} /* end PhotoshopEvent class */

