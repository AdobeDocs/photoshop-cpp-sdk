//
//  ImageHelper.m
//  transmitimage
//
//  Created by Paul Solt on 9/22/10.
//  Copyright 2010 Paul Solt. All rights reserved.
//  http://paulsolt.com/2010/09/ios-converting-uiimage-to-rgba8-bitmaps-and-back/
//  URL serving this .m file:
//  https://github.com/PaulSolt/UIImage-Conversion/blob/master/ImageHelper.m
//
//  The following function were created by Allen Jeng from Adobe Systems Inc.
//  convertBitmapRGBA8ToPixmapRGB()
//  padPixmapRGBToBitmapRGBA8()
//  Copyright 2011 Adobe Systems Inc. All rights reserved.
//

#import "ImageHelper.h"


@implementation ImageHelper


// ------------------------------------------------------------------------
+ (unsigned char *) convertUIImageToBitmapRGBA8:(UIImage *) image 
{
	CGImageRef imageRef = image.CGImage;
	
	// Create a bitmap context to draw the uiimage into
	CGContextRef context = [self newBitmapRGBA8ContextFromImage:imageRef];
	
	if(!context) 
	{
		return NULL;
	}
	
	size_t width = CGImageGetWidth(imageRef);
	size_t height = CGImageGetHeight(imageRef);
	
	CGRect rect = CGRectMake(0, 0, width, height);
	
	// Draw image into the context to get the raw image data
	CGContextDrawImage(context, rect, imageRef);
	
	// Get a pointer to the data	
	unsigned char *bitmapData = (unsigned char *)CGBitmapContextGetData(context);
	
	// Copy the data and release the memory (return memory allocated with new)
	size_t bytesPerRow = CGBitmapContextGetBytesPerRow(context);
	size_t bufferLength = bytesPerRow * height;
	
	unsigned char *newBitmap = NULL;
	
	if(bitmapData) 
	{
		newBitmap = (unsigned char *)malloc(sizeof(unsigned char) * bytesPerRow * height);
		
		if(newBitmap) 
		{	// Copy the data
			for(int i = 0; i < bufferLength; ++i) 
			{
				newBitmap[i] = bitmapData[i];
			}
		}
		
		free(bitmapData);
		
	} else {
		NSLog(@"Error getting bitmap pixel data\n");
	}
	
	CGContextRelease(context);
	
	return newBitmap;	
}

// ------------------------------------------------------------------------
+ (CGContextRef) newBitmapRGBA8ContextFromImage:(CGImageRef) image {
	CGContextRef context = NULL;
	CGColorSpaceRef colorSpace;
	uint32_t *bitmapData;
	
	size_t bitsPerPixel = 32;
	size_t bitsPerComponent = 8;
	size_t bytesPerPixel = bitsPerPixel / bitsPerComponent;
	
	size_t width = CGImageGetWidth(image);
	size_t height = CGImageGetHeight(image);
	
	size_t bytesPerRow = width * bytesPerPixel;
	size_t bufferLength = bytesPerRow * height;
	
	colorSpace = CGColorSpaceCreateDeviceRGB();
	
	if(!colorSpace) {
		NSLog(@"Error allocating color space RGB\n");
		return NULL;
	}
	
	// Allocate memory for image data
	bitmapData = (uint32_t *)malloc(bufferLength);
	
	if(!bitmapData) {
		NSLog(@"Error allocating memory for bitmap\n");
		CGColorSpaceRelease(colorSpace);
		return NULL;
	}
	
	//Create bitmap context
	
	context = CGBitmapContextCreate(bitmapData, 
									width, 
									height, 
									bitsPerComponent, 
									bytesPerRow, 
									colorSpace, 
									kCGImageAlphaPremultipliedLast);	// RGBA
	if(!context) {
		free(bitmapData);
		NSLog(@"Bitmap context not created");
	}
	
	CGColorSpaceRelease(colorSpace);
	
	return context;	
}

// ------------------------------------------------------------------------
+ (UIImage *) convertBitmapRGBA8ToUIImage:(unsigned char *) buffer 
								withWidth:(int) width
							   withHeight:(int) height {
	
	
	size_t bufferLength = width * height * 4;
	CGDataProviderRef provider = CGDataProviderCreateWithData(NULL, buffer, bufferLength, NULL);
	size_t bitsPerComponent = 8;
	size_t bitsPerPixel = 32;
	size_t bytesPerRow = 4 * width;
	
	CGColorSpaceRef colorSpaceRef = CGColorSpaceCreateDeviceRGB();
	if(colorSpaceRef == NULL) {
		NSLog(@"Error allocating color space");
		CGDataProviderRelease(provider);
		return nil;
	}
	
	CGBitmapInfo bitmapInfo = kCGBitmapByteOrderDefault; 
	CGColorRenderingIntent renderingIntent = kCGRenderingIntentDefault;
	
	CGImageRef iref = CGImageCreate(width, 
									height, 
									bitsPerComponent, 
									bitsPerPixel, 
									bytesPerRow, 
									colorSpaceRef, 
									bitmapInfo, 
									provider,	// data provider
									NULL,		// decode
									YES,			// should interpolate
									renderingIntent);
	
	uint32_t* pixels = (uint32_t*)malloc(bufferLength);
	
	if(pixels == NULL) {
		NSLog(@"Error: Memory not allocated for bitmap");
		CGDataProviderRelease(provider);
		CGColorSpaceRelease(colorSpaceRef);
		CGImageRelease(iref);		
		return nil;
	}
	
	CGContextRef context = CGBitmapContextCreate(pixels, 
												 width, 
												 height, 
												 bitsPerComponent, 
												 bytesPerRow, 
												 colorSpaceRef, 
												 kCGImageAlphaPremultipliedLast); 
	
	if(context == NULL) {
		NSLog(@"Error context not created");
		free(pixels);
	}
	
	UIImage *image = nil;
	if(context) {
		
		CGContextDrawImage(context, CGRectMake(0.0f, 0.0f, width, height), iref);
		
		CGImageRef imageRef = CGBitmapContextCreateImage(context);
		
		// Support both iPad 3.2 and iPhone 4 Retina displays with the correct scale
		if([UIImage respondsToSelector:@selector(imageWithCGImage:scale:orientation:)]) {
			float scale = [[UIScreen mainScreen] scale];
			image = [UIImage imageWithCGImage:imageRef scale:scale orientation:UIImageOrientationUp];
		} else {
			image = [UIImage imageWithCGImage:imageRef];
		}
		
		CGImageRelease(imageRef);	
		CGContextRelease(context);	
	}
	
	CGColorSpaceRelease(colorSpaceRef);
	CGImageRelease(iref);
	CGDataProviderRelease(provider);
	
	if(pixels) {
		free(pixels);
	}	
	return image;
}

// ------------------------------------------------------------------------
+ (unsigned char *) convertBitmapRGBA8ToPixmapRGB:(unsigned char *) buffer
										withWidth:(int)width
									   withHeight:(int)height;
{
	unsigned char *newBitmap = NULL;
	int oldBufferLength = width * height * 4;  // have to use this trick because any color with 0x00 would throw length function off
	int newBitmapLength = width * height * 3;
	
	newBitmap = (unsigned char *)malloc(sizeof(unsigned char) * newBitmapLength);
	
	if(newBitmap) 
	{	
		// Copy the data
		int idxToBuffer = 0;
		
		for(int i = 0; i < oldBufferLength; ++i) 
		{
			if ( (i + 1) % 4 != 0)
			{
				// wouldn't want it to blow up
				if (idxToBuffer < newBitmapLength)
					newBitmap[idxToBuffer++] = buffer[i];
			}
			// else throw the 4th item away
		}
	}
	
	return newBitmap;
}

// ------------------------------------------------------------------------
+ (unsigned char *) padPixmapRGBToBitmapRGBA8:(unsigned char *) buffer
									withWidth:(int)width
								   withHeight:(int)height
{
	unsigned char *newBitmap = NULL;
	int newBitmapLength = width * height * 4;
	int i, j;
	
	newBitmap = (unsigned char *)malloc(sizeof(unsigned char) * newBitmapLength);
	
	if(newBitmap) 
	{	
		// Copy the data
		int idxToBuffer = 0;

		for (i=0; i < height; ++i) 
		{
			for (j=0; j < width; ++j)
			{
				int offset = i * width * 4 + j * 4;
				newBitmap[ offset + 0 ] = buffer[idxToBuffer++];
				newBitmap[ offset + 1 ] = buffer[idxToBuffer++];
				newBitmap[ offset + 2 ] = buffer[idxToBuffer++];
				newBitmap[ offset + 3 ] = 255;  // pad it
			}
		}
	}
	
	return newBitmap;
}


// ------------------------------------------------------------------------
+ (UIImage *) convertPixmapRGBA8ToUIImage:(unsigned char *) buffer
								withWidth:(int)width
							   withHeight:(int)height
{
	CGDataProviderRef provider = CGDataProviderCreateWithData(NULL,
															  buffer, 
															  width * height * 4,
															  NULL);
	CGColorSpaceRef colorSpaceRef = CGColorSpaceCreateDeviceRGB();
	CGBitmapInfo bitmapInfo = kCGBitmapByteOrderDefault;
	CGColorRenderingIntent renderingIntent = kCGRenderingIntentDefault;
	CGImageRef imageRef = CGImageCreate(width,
										height,
										8,
										32,
										4*width,colorSpaceRef,
										bitmapInfo,
										provider,NULL,NO,renderingIntent);

	UIImage *newImage = [[UIImage alloc] initWithCGImage:imageRef];
	
	CGImageRelease(imageRef);
	CGDataProviderRelease(provider);
	return newImage;
}


@end
