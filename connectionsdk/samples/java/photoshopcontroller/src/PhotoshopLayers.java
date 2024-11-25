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


import java.awt.Component;
import java.util.Vector;

import javax.swing.BoxLayout;
import javax.swing.JPanel;

/**
 * One scroll able panel to show all the layers of the
 * current Photoshop document
 */
class PhotoshopLayers extends JPanel {

	static final long serialVersionUID = 1;
	 
	/** stick a scroll bar on it */
	// JScrollPane mScrollPane = null;

	/** all the layers in the document */
	 Vector<PhotoshopImage> mLayersView = new Vector<PhotoshopImage>();
	 
	 Vector<Component> mComponents = new Vector<Component>();
	
	/** our default size */
	private final int mLayersViewSize = 200;
	
	/** for test purposes create some dummy images */
	private final boolean mCreateFakes = false;

	/**
	 * Constructor for our view.
	 */
	public PhotoshopLayers(int inX, int inY, int inWidth, int inHeight) {
		setLayout(new BoxLayout(this, BoxLayout.Y_AXIS));
		
		if (mCreateFakes) {
			mLayersView.addElement(new PhotoshopImage("Layer 1", mLayersViewSize, mLayersViewSize));
			mLayersView.addElement(new PhotoshopImage("Layer 2", mLayersViewSize, mLayersViewSize));
			mLayersView.addElement(new PhotoshopImage("Layer 3", mLayersViewSize, mLayersViewSize));
			mLayersView.addElement(new PhotoshopImage("Layer 4", mLayersViewSize, mLayersViewSize));
			mLayersView.addElement(new PhotoshopImage("Layer 5", mLayersViewSize, mLayersViewSize));
			mLayersView.addElement(new PhotoshopImage("Layer 6", mLayersViewSize, mLayersViewSize));
			mLayersView.addElement(new PhotoshopImage("Layer 7", mLayersViewSize, mLayersViewSize));
		
			mComponents.addElement(add(mLayersView.elementAt(0)));
			mComponents.addElement(add(mLayersView.elementAt(1)));
			mComponents.addElement(add(mLayersView.elementAt(2)));
			mComponents.addElement(add(mLayersView.elementAt(3)));
			mComponents.addElement(add(mLayersView.elementAt(4)));
			mComponents.addElement(add(mLayersView.elementAt(5)));
			mComponents.addElement(add(mLayersView.elementAt(6)));
		}
	}

	/** is this what i want? I don't think so */
	public void add(String inName, int inDocumentID, int inLayerID) {
		PhotoshopImage pi = new PhotoshopImage(inName, mLayersViewSize, mLayersViewSize);
		pi.mDocumentID = inDocumentID;
		pi.mLayerID = inLayerID;
		mLayersView.addElement(pi);
		add(pi);
	}
	
	/** return how many images I have */
	public int count() {
		return mLayersView.size();
	}
	
	/** return one of my images */
	public PhotoshopImage elementAt(int inIndex) {
		if (inIndex >= 0 && inIndex < mLayersView.size())
			return mLayersView.elementAt(inIndex);
		else
			return null;
	}
	
	public void removeAllImages() {
		mLayersView.removeAllElements();
		removeAll();
		revalidate();
		repaint();
	}
	
	public void removeImageAt(int inIndex) {
		mLayersView.removeElementAt(inIndex);
		remove(getComponent(inIndex));
		revalidate();
		repaint();
		// TODO check for one layer delete working then fix accordingly validate();
	}
	
	
} /* end PhotoshopEvent class */

