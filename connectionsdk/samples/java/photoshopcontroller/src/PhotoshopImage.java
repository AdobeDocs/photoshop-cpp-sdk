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

/**  Filename: PhotoshopImage.java
	@author Thomas Ruark, Photoshop Engineering, Adobe Systems Incorporated
*/


import java.awt.Color;
import java.awt.Dimension;
import java.awt.image.BufferedImage;
import java.io.ByteArrayInputStream;
import java.io.IOException;
import java.util.Calendar;

import javax.imageio.ImageIO;
import javax.swing.BorderFactory;
import javax.swing.BoxLayout;
import javax.swing.ImageIcon;
import javax.swing.JLabel;
import javax.swing.JPanel;

/**
 * This could be a document or a layer thumb nail.
 * Display the name, pixel data, date and time we got it
 * and the width and height.
 */
class PhotoshopImage extends JPanel {

	static final long serialVersionUID = 1;
	 
	/** the bitmap currently being displayed */
	private BufferedImage mBitmap = null;

	/** width of the entire view */
	private int mViewWidth = 0;

	/** height of the entire view */
	private int mViewHeight = 0;

	/** width of the bitmap */
	private int mBitmapWidth = 0;

	/** height of the bitmap */
	private int mBitmapHeight = 0;

	/** document or layer name */
	private  JLabel mNameLbl = null;
	
	/** the pixels are held here */
	private JLabel mImgLbl = null;
	
	/** bitmap goes into an icon */
	private ImageIcon mImgIcon = null;
	
	/** date stamp and width and height information */
	private JLabel mUpdateLbl = null; 
	
	/** unique document ID for these pixels */
	public int mDocumentID = 0;

	/** unique layer ID for these pixels, 
	 * -1 for document composite view 
	 *  0 for background layer */
	public int mLayerID = -1;

	/**
	 * Constructor for our main view.
	 * Draw some dummy image for now.
	 * 
	 * @param inName - the document or layer name
	 * @param inWidth - width of the entire view
	 * @param inHeight - height of the entire view
	 */
	public PhotoshopImage(String inName, int inWidth, int inHeight) {
		mViewWidth = inWidth;
		mViewHeight = inHeight;

		mNameLbl = new JLabel(inName);
		Dimension d1 = mNameLbl.getPreferredSize();

		mBitmapWidth = inWidth;
		mBitmapHeight = inHeight - d1.height * 2; // we have two labels
		
		setLayout(new BoxLayout(this, BoxLayout.Y_AXIS));
		
		int width = mBitmapWidth;
		int height = mBitmapHeight;
		mBitmap = new BufferedImage(mBitmapWidth, mBitmapHeight, BufferedImage.TYPE_INT_ARGB);
		for (int x = 0; x < width; x++) {
			for (int y = 0; y < height; y++) {
				mBitmap.setRGB(x, y, pack8888(x,y,x+y,255));
			}
		}
        
        mUpdateLbl = new JLabel(getTimeStamp() + ":" + width + " : " + height);
        
        mImgIcon = new ImageIcon(mBitmap);

        mImgLbl = new JLabel(mImgIcon, JLabel.CENTER);

        mImgLbl.setBorder(BorderFactory.createLineBorder(Color.black));

		add(mNameLbl);
		add(mImgLbl);
		add(mUpdateLbl);
	}


	/** 
	 * Convert an unsigned byte to an integer
	 *
	 * @param b - byte to convert
	 * @return value for byte
	 */
	private int unsignedByteToInt(byte b) {
		return (int) b & 0xFF;
	}

	/**
	 * This takes components that are already in pre multiplied form, and
	 * packs them into an int in the correct device order.
	 */
	private int pack8888(int r, int g, int b, int a) {
		return (r << 0) | ( g << 8) | (b << 16) | (a << 24);
	}
	
	/** the entire width of the panel */
	public int getViewWidth() {
		return mViewWidth;
	}
	
	/** the entire height of the panel */
	public int getViewHeight() {
		return mViewHeight;
	}

	/** the pixel width */
	public int getBitmapWidth() {
		return mBitmapWidth;
	}

	/** the pixel height */
	public int getBitmapHeight() {
		return mBitmapHeight;
	}

	/** blank out or display */
	public void noImage() {
        mUpdateLbl.setText(getTimeStamp() + " NO IMAGE ");
	}

	/** set the name of what we are viewing */
	public void setName(String inName) {
		mNameLbl.setText(inName);
	}
	
	/**
	 * The thread has called us with the bytes from Photoshop
	 * Make a Bitmap from the Pixmap
	 * @param inBytes - all the bytes for the image
	 * @param inIndexer - the offset we are currently at in the inBytes array
	 * @return error or warning message as String, null if all goes well
	 */
	public String createBitmap(byte [] inBytes, int inIndexer) {
		int width =    (inBytes[inIndexer++] << 24) + ((inBytes[inIndexer++] & 0xFF) << 16) + ((inBytes[inIndexer++] & 0xFF) << 8) + (inBytes[inIndexer++] & 0xFF);
		int height =   (inBytes[inIndexer++] << 24) + ((inBytes[inIndexer++] & 0xFF) << 16) + ((inBytes[inIndexer++] & 0xFF) << 8) + (inBytes[inIndexer++] & 0xFF);
		int rowBytes = (inBytes[inIndexer++] << 24)	+ ((inBytes[inIndexer++] & 0xFF) << 16)	+ ((inBytes[inIndexer++] & 0xFF) << 8) + (inBytes[inIndexer++] & 0xFF);
		byte colorMode = inBytes[inIndexer++];
		if (1 != colorMode) return new String("Bad color mode");
		byte channelCount = inBytes[inIndexer++];
		if (1 != channelCount && 3 != channelCount && 4 != channelCount) return new String("Bad channel count");
		byte bitsPerChannel = inBytes[inIndexer++];
		if (8 != bitsPerChannel) return new String("Bad bits per pixel");
		int extra = rowBytes - width * channelCount;
		mBitmap = new BufferedImage(width, height, BufferedImage.TYPE_INT_ARGB_PRE);
		int by = 0;
		for (int y = 0; y < height; y++, inIndexer += extra, by++) {
			int bx = 0;
			for (int x = 0; x < width; x++, bx++) {
				int color = 0;
				if (channelCount == 1) {
					color = pack8888(inBytes[inIndexer], inBytes[inIndexer], inBytes[inIndexer], 255);
					inIndexer++;
				} else if (channelCount == 3) {
					color = pack8888(unsignedByteToInt(inBytes[inIndexer + 2]), unsignedByteToInt(inBytes[inIndexer + 1]), unsignedByteToInt(inBytes[inIndexer]), 255);
					inIndexer += 3;
				} else if (channelCount == 4) {
					color = pack8888(unsignedByteToInt(inBytes[inIndexer + 3]), unsignedByteToInt(inBytes[inIndexer + 2]), unsignedByteToInt(inBytes[inIndexer + 1]), unsignedByteToInt(inBytes[inIndexer]));
					inIndexer += 4;
				}
				mBitmap.setRGB(bx, by, color);
			}
		}
		mImgIcon = new ImageIcon(mBitmap);
        mImgLbl.setIcon(mImgIcon);
        mUpdateLbl.setText(getTimeStamp() + " : " + width + " : " + height);
        if (width > mBitmapWidth || height > mBitmapHeight)
        	return new String(width + " > " + mBitmapWidth + " || " + height + " > " + mBitmapHeight);
        if (inIndexer != inBytes.length)
        	return new String(inIndexer + " != " + inBytes.length);
		return null;
	}

	/**
	 * The thread has called us with a JPEG from Photoshop
	 * Make a Bitmap from the JPEG bytes
	 * @param inBytes - all the bytes for the image
	 * @param inIndexer - the offset we are currently at in the inBytes array
	 * @return error or warning message as String, null if all goes well
	 */
	public String createBitmapFromJPEG(byte [] inBytes, int inIndexer) {
		try {
			ByteArrayInputStream bais = new ByteArrayInputStream(inBytes, inIndexer, inBytes.length - inIndexer);
			mBitmap = ImageIO.read(bais);
			mImgIcon = new ImageIcon(mBitmap);
	        mImgLbl.setIcon(mImgIcon);
	        mUpdateLbl.setText("JPEG:" + getTimeStamp() + " : " + mBitmap.getWidth() + " : " + mBitmap.getHeight());
		}
		catch(IOException ioe) {
			return ioe.getLocalizedMessage();
		}
		return null;
	}

	/** 
	 * Return HOUR:MINUTE:SECOND:MILLISECOND
	 */
	private String getTimeStamp() {
		Calendar c = Calendar.getInstance();
		String s = c.get(Calendar.HOUR_OF_DAY) + ":" + c.get(Calendar.MINUTE)+ ":" + c.get(Calendar.SECOND) + ":" + c.get(Calendar.MILLISECOND);
		return s;
	}
} /* end PhotoshopImage class */

