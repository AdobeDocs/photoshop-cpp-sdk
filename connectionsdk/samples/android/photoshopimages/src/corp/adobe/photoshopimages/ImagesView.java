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

/**  Filename: ImagesView.java
	@author Thomas Ruark, Photoshop Engineering, Adobe Systems Incorporated
*/

package corp.adobe.photoshopimages;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Canvas;
import android.graphics.Paint;
import android.view.View;

/**
 * Display the current document from Photoshop
 * 
 *
 */
class ImagesView extends View {

	/** border size */
	final static private int BORDER_SIZE = 10;

	/** the bitmap currently being displayed */
	private Bitmap mBitmap = null;

	/** width of entire view */
	private int mViewWidth = 0;

	/** height of entire view */
	private int mViewHeight = 0;

	/** width of bitmap */
	private int mBitmapWidth = 0;

	/** height of bitmap */
	private int mBitmapHeight = 0;

	/** border color shows connection status */
	private int mBorderColor = 0;

	
	/**
	 * Constructor for our main view.
	 * 
	 * @param context
	 * @param inBorderColor
	 */
	public ImagesView(Context context, int inBorderColor) {
		super(context);
		mBorderColor = inBorderColor;
		setFocusable(true);
        setFocusableInTouchMode(true);
	}


	/** convert an unsigned byte to an integer */
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


	/** our border color tells if we are connected */
	public void setBorderColor(int inBorderColor) {
		mBorderColor = inBorderColor;
		invalidate();
	}


	/** ideally photoshop would fill up our entire width */
	public int getBitmapWidth() {
		return mBitmapWidth;
	}


	/** ideally photoshop would fill up our entire height */
	public int getBitmapHeight() {
		return mBitmapHeight;
	}

	/** allow the main Activity to set us for no images */
	public void noImages() {
		mBitmap = null;
		invalidate();
	}
	
	/**
	 * Draw the bitmap we currently have.
	 */
	@Override 
	protected void onDraw(Canvas canvas) {
		canvas.drawColor(mBorderColor);            
		if (null != mBitmap) {
			int xOffset = (mViewWidth - mBitmap.getWidth()) / 2;
			if (xOffset < BORDER_SIZE) {
				xOffset = BORDER_SIZE;
			}
			int yOffset = (mViewHeight - mBitmap.getHeight()) / 2;
			if (yOffset < BORDER_SIZE) {
				yOffset = BORDER_SIZE;
			}
			int w = mBitmap.getWidth();
			int h = mBitmap.getHeight();
			if (w > 0 && h > 0) {
				canvas.drawBitmap(mBitmap, xOffset, yOffset, null);
			}
		} else {
			Paint paint = new Paint();
			paint.setAntiAlias(true);
			paint.setTextSize(60);
			canvas.drawText("No images", 100, 100, paint);
		}
	}


	/**
	 * After we initialize we get this call to tell us our current size.
	 */
	@Override
	protected void onSizeChanged(int w, int h, int oldw, int oldh) {
		mViewWidth = w;
		mViewHeight = h;
		((PhotoshopImages)getContext()).onSizeChanged();
	}


	/**
	 * Calculate the width and height of image
	 */
	public void setGrid() {

		mBitmapWidth =  mViewWidth  - BORDER_SIZE * 2;
		mBitmapHeight = mViewHeight - BORDER_SIZE * 2;

	}


	/**
	 * The thread has called us with the bytes from Photoshop
	 * Make a Bitmap from the Pixmap
	 * @param inBytes
	 * @param inIndexer
	 * @return error messages as String
	 */
	public String createBitmap(byte [] inBytes, int inIndexer) {
		int width =    (inBytes[inIndexer++] << 24) + ((inBytes[inIndexer++] & 0xFF) << 16) + ((inBytes[inIndexer++] & 0xFF) << 8) + (inBytes[inIndexer++] & 0xFF);
		int height =   (inBytes[inIndexer++] << 24) + ((inBytes[inIndexer++] & 0xFF) << 16) + ((inBytes[inIndexer++] & 0xFF) << 8) + (inBytes[inIndexer++] & 0xFF);
		int rowBytes = (inBytes[inIndexer++] << 24)	+ ((inBytes[inIndexer++] & 0xFF) << 16)	+ ((inBytes[inIndexer++] & 0xFF) << 8) + (inBytes[inIndexer++] & 0xFF);
		byte colorMode = inBytes[inIndexer++];
		if (1 != colorMode) return new String("Bad color mode");
		byte channelCount = inBytes[inIndexer++];
		if (1 != channelCount && 3 != channelCount) return new String("Bad channel count");
		byte bitsPerChannel = inBytes[inIndexer++];
		if (8 != bitsPerChannel) return new String("Bad bits per pixel");
		int extra = rowBytes - width * channelCount;
		mBitmap = Bitmap.createBitmap(width, height, Bitmap.Config.ARGB_8888);
		for (int y = 0; y < height; y++, inIndexer += extra) {
			for (int x = 0; x < width; x++) {
				int color = 0;
				// 1 or 3 for now
				if (channelCount == 1) {
					color = pack8888(inBytes[inIndexer], inBytes[inIndexer], inBytes[inIndexer], 255);
					inIndexer++;
				} else {
					color = pack8888(unsignedByteToInt(inBytes[inIndexer + 2]), unsignedByteToInt(inBytes[inIndexer + 1]), unsignedByteToInt(inBytes[inIndexer]), 255);
					inIndexer += 3;
				}
				mBitmap.setPixel(x, y, color);
			}
		}
		return null;
	}

	/**
	 * The thread has called us with a JPEG from Photoshop
	 * Make a Bitmap from the JPEG bytes
	 * @param inBytes
	 * @param inIndexer
	 * @return error messages as String
	 */
	public String createBitmapFromJPEG(byte [] inBytes, int inIndexer) {
		mBitmap = BitmapFactory.decodeByteArray(inBytes, inIndexer, inBytes.length - inIndexer);
		return null;
	}

} /* end ImagesView class */

