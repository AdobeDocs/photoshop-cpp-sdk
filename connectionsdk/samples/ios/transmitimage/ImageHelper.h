//
//  ImageHelper.h
//  transmitimage
//
//  Created by Paul Solt on 9/22/10.
//  Copyright 2010 Paul Solt. All rights reserved.
//  http://paulsolt.com/2010/09/ios-converting-uiimage-to-rgba8-bitmaps-and-back/
//  URL for serving this .h file:
//  https://github.com/PaulSolt/UIImage-Conversion/blob/master/ImageHelper.h
//
//  The following function were created by Allen Jeng from Adobe Systems Inc.
//  convertBitmapRGBA8ToPixmapRGB()
//  padPixmapRGBToBitmapRGBA8()
//  Copyright 2011 Adobe Systems Inc. All rights reserved.
//

#import <Foundation/Foundation.h>


@interface ImageHelper : NSObject {

}


/** Converts a UIImage to RGBA8 bitmap.
 @param image - a UIImage to be converted
 @return a RGBA8 bitmap, or NULL if any memory allocation issues. Cleanup memory with free() when done.
 */
+ (unsigned char *) convertUIImageToBitmapRGBA8:(UIImage *)image;

/** A helper routine used to convert a RGBA8 to UIImage
 @return a new context that is owned by the caller
 */
+ (CGContextRef) newBitmapRGBA8ContextFromImage:(CGImageRef)image;


/** Converts a RGBA8 bitmap to a UIImage. 
 @param buffer - the RGBA8 unsigned char * bitmap
 @param width - the number of pixels wide
 @param height - the number of pixels tall
 @return a UIImage that is autoreleased or nil if memory allocation issues
 */
+ (UIImage *) convertBitmapRGBA8ToUIImage:(unsigned char *)buffer 
								withWidth:(int)width
							   withHeight:(int)height;


/** Given a RGBA8 bitmap raw, remove the 4th data padding
 @param buffer - the RGBA8 unsigned char * bitmap
 @param width - the number of pixels wide
 @param height - the number of pixels tall
 @return a RGB pixmap
 */
+ (unsigned char *) convertBitmapRGBA8ToPixmapRGB:(unsigned char *) buffer
										withWidth:(int)width
									   withHeight:(int)height;


/** Given a RGB pixmap raw, add the 4th data padding
 @param buffer - the RGBA8 unsigned char * bitmap
 @param width - the number of pixels wide
 @param height - the number of pixels tall
 @return a RGBA8 bitmap raw
 */
+ (unsigned char *) padPixmapRGBToBitmapRGBA8:(unsigned char *) buffer
										withWidth:(int)width
									   withHeight:(int)height;


/** Given a RGB pixmap padded, convert it to UIImage
 @param buffer - the RGBA8 unsigned char * bitmap
 @param width - the number of pixels wide
 @param height - the number of pixels tall
 @return UIImage
 */
+ (UIImage *) convertPixmapRGBA8ToUIImage:(unsigned char *) buffer
								withWidth:(int)width
							   withHeight:(int)height;
@end
