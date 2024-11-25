// ADOBE SYSTEMS INCORPORATED
// Copyright  2009 Adobe Systems Incorporated
// All Rights Reserved
//
// NOTICE:  Adobe permits you to use, modify, and distribute this 
// file in accordance with the terms of the Adobe license agreement
// accompanying it.  If you have received this file from a source
// other than Adobe, then your use, modification, or distribution
// of it requires the prior written permission of Adobe.
//-------------------------------------------------------------------------------

#import "DissolveProxyView.h"
#import "PIGeneral.h"
#import "Dissolve.h"
#import "FilterBigDocument.h"
#import "DissolveController.h"
#import "PIProperties.h"

extern void UpdateProxyBuffer(void);
extern void ResetProxyBuffer(void);

extern DissolveController *gDissolveController;

/* Make sure this is unique to you and everyone you might encounter, search for
"Preventing Name Conflicts" or use this link
http://developer.apple.com/mac/library/documentation/UserExperience/Conceptual/PreferencePanes/Tasks/Conflicts.html
*/
@implementation DissolveProxyView

- (id)initWithFrame:(NSRect)frame
{
    self = [super initWithFrame:frame];
    if (self) 
	{
		currentColor = [NSColor clearColor];
		currentZoomState = ZoomCommandUninited;
		NSLog(@"Dissolve SetColor to clearColor");
    }
    return self; 
}

- (void)layoutSubviews
{
	NSLog(@"Dissolve in layout");
}

- (BOOL)isFlipped
{
	return YES;
}

- (void)setDispositionColor:(int16)newColor
{
	NSLog(@"Dissolve start setColor to %d", newColor);
	switch (newColor) 
	{
		case 0:
			currentColor = [NSColor clearColor];
			NSLog(@"Dissolve end setColor to clearColor");
			break;
		default:
		case 1:
			currentColor = [NSColor blueColor];
			NSLog(@"Dissolve end setColor to blueColor");
			break;
		case 2:
			currentColor = [NSColor redColor];
			NSLog(@"Dissolve end setColor to redColor");
			break;
		case 3:
			currentColor = [NSColor greenColor];
			NSLog(@"Dissolve end setColor to greenColor");
			break;
	}
}

- (CGFloat)getCurrentScaleFactor
{
	/*
		Get the backing scale factor for the view. Don't bother worrying about width versus height
		either will work
	*/
	NSSize backingSize = NSMakeSize(1,1);
	if ([self respondsToSelector:@selector(convertSizeToBacking:)])
    	backingSize = [self convertSizeToBacking: backingSize];
	
	// Make sure we are not 0. It shouldn't happen but we don't want to divide by 0.
    intptr_t simple = 0;
	OSErr error = 0;
    if (gFilterRecord != NULL && gFilterRecord->propertyProcs != NULL &&
        gFilterRecord->propertyProcs->getPropertyProc != NULL)
        
        error = gFilterRecord->propertyProcs->getPropertyProc(kPhotoshopSignature,
                                                              propHDPIAware,
                                                              0, &simple, NULL);
    if (backingSize.width < 1.0 || simple == 0 || error)
		backingSize.width = 1.0;
		
	return backingSize.width;
}


- (void)viewDidChangeBackingProperties
	{
	/*
		You get this message from the OS when your window crosses over monitors. This is the point
		at which, if you have zoom capability, you will want to zoom your view so it looks to be
		realtively the same size on both monitors - it is the same thing Photoshop does.
		
		This message is kind of tricky because you will get this when your view is first created.
		In this case, you do not want to automiatically zoom the view. Additionally you can only
		determine what you are now, not what you were, thus you need to keep track yourself.
	*/
	ZoomCommandState oldState = currentZoomState;
	
	CGFloat scaleFactor = [self getCurrentScaleFactor];
	
	// Compare to 1.0 allowing for epsilon
	
	currentZoomState = scaleFactor > 1.0001 ? ZoomCommandHigh : ZoomCommandLow;

	NSLog(@"Dissolve viewDidChangeBackingProperties, scale: %f", scaleFactor);

	if (oldState != currentZoomState && oldState != ZoomCommandUninited)
		{
		// This is where you automatically zoom your view so that it remains relatively the same
		// size when crossing monitors. You will want to zoom out when going from 2x to 1x and zoom
		// in when going from 1x to 2x (double it or half it!)
		}
	}

- (void)drawRect:(NSRect)rect 
{

	// I appear to get called on another thread
	// during first display
	if ( ! [NSThread isMainThread] ) 
	{
		// this seems to work
		[self setNeedsDisplay:YES];
		return;
		
		// return; can't do that because I get a blank preview

		/* this will crash but would seem like the right thing to do
		[gDissolveController performSelectorOnMainThread:@selector(updateProxy:)
							   withObject:NULL
							waitUntilDone:NO];
		 */

		/* might work if i could figure out rect to pass in
		[self performSelectorOnMainThread:@selector(drawRect:)
 	                                withObject:rect
									waitUntilDone:YES];
		 */
	}
	
	
	CGFloat scaleFactor = [self getCurrentScaleFactor];
	
	NSLog(@"Dissolve drawRect, w: %f, h: %f, x: %f, y: %f, scale: %f", rect.size.width, rect.size.height, rect.origin.x, rect.origin.y, scaleFactor);

	/*
		We need to deal in both logical units (NSView space) and pixelData units (the image space unit)
	*/

	// Create a temporary rectangle of the drawing area which is in pixelData space
	NSRect drawRectPixelData = NSMakeRect (rect.origin.x * scaleFactor,
										rect.origin.y * scaleFactor,
										rect.size.width * scaleFactor,
										rect.size.height * scaleFactor);
	
	short pixelDataHeight = static_cast <short> (drawRectPixelData.size.height);
	short pixelDataWidth = static_cast <short> (drawRectPixelData.size.width);
	
	// Setup proxyRect in pixelData coordinates
	gData->proxyRect.top = 0;
	gData->proxyRect.left = 0;
	gData->proxyRect.bottom = pixelDataHeight;
	gData->proxyRect.right = pixelDataWidth;
	
	// A temporary pixelData rect for working
	Rect pixelDataRect;
	pixelDataRect.top = gData->proxyRect.top;
	pixelDataRect.left = gData->proxyRect.left;
	pixelDataRect.bottom = gData->proxyRect.bottom;
	pixelDataRect.right = gData->proxyRect.right;
	
	SetupFilterRecordForProxy();
	CreateProxyBuffer();
	CreateDissolveBuffer(gData->proxyWidth, gData->proxyHeight);
	ResetProxyBuffer();
	UpdateProxyBuffer();
	
	// Note: buffer width and height are in pixelData space
	short bufferHeight = gData->proxyHeight;
	short bufferWidth  = gData->proxyWidth;
	
	/* Set up the output map. */

	// For this example, GetInRect returns pixelData space units
	VRect inRectPixelData = GetInRect();

	// Setup the outMap in pixelData units
	PSPixelMap outMap;
	outMap.version       = 1;
	outMap.bounds.top    = inRectPixelData.top;
	outMap.bounds.left   = inRectPixelData.left;
	outMap.bounds.bottom = inRectPixelData.bottom;
	outMap.bounds.right  = inRectPixelData.right;
	outMap.imageMode     = DisplayPixelsMode(gFilterRecord->imageMode);
	outMap.rowBytes      = gData->proxyWidth;
	outMap.colBytes		 = 1;
	outMap.planeBytes	 = gData->proxyPlaneSize;
	outMap.baseAddr		 = gData->proxyBuffer;
	
	outMap.mat			= NULL;
	outMap.masks		= NULL;
	outMap.maskPhaseRow = 0;
	outMap.maskPhaseCol = 0;
	
	/* 
		Compute where we are going to display it. Lets do this from pixelData units and convert it
		to logical space later.
	*/
	
	short pixelDataDstRow = static_cast <short> (drawRectPixelData.origin.x);
	short pixelDataDstCol = static_cast <short> (drawRectPixelData.origin.y);
	
	if (pixelDataHeight > bufferHeight)
	{
		
		Rect eraseArea;
		eraseArea.top = gData->proxyRect.top;
		eraseArea.left = gData->proxyRect.left;
		eraseArea.bottom = gData->proxyRect.bottom;
		eraseArea.right = gData->proxyRect.right;

		pixelDataDstRow += (pixelDataHeight - bufferHeight) / 2;
		
		eraseArea.bottom = pixelDataDstRow;

		pixelDataRect.top = pixelDataDstRow;
			
		eraseArea = pixelDataRect;
		eraseArea.top = pixelDataDstRow + bufferHeight;
	
		pixelDataRect.bottom = pixelDataDstRow + bufferHeight;
		
		pixelDataHeight = bufferHeight;
			
	}
			
	if (pixelDataWidth > bufferWidth)
	{
		
		Rect eraseArea = pixelDataRect;

		pixelDataDstCol += (pixelDataWidth - bufferWidth) / 2;
		
		eraseArea.right = pixelDataDstCol;
		
		pixelDataRect.left = pixelDataDstCol;
			
		eraseArea = pixelDataRect;
		eraseArea.left = pixelDataDstCol + bufferWidth;
	
		pixelDataRect.right = pixelDataDstCol + bufferWidth;
		
		pixelDataWidth = bufferWidth;
			
	}

	/* Compute the source. */
	
	// Note: for this example: GetOutRect returns pixelData space
	VRect outRectPixelData = GetOutRect();

	VRect pixelSrcRect;
	
	pixelSrcRect.top    = outRectPixelData.top;
	pixelSrcRect.left   = outRectPixelData.left;
	
	if (bufferHeight > pixelDataHeight)
		pixelSrcRect.top += (bufferHeight - pixelDataHeight) / 2;
		
	if (bufferWidth > pixelDataWidth)
		pixelSrcRect.left += (bufferWidth - pixelDataWidth) / 2;
		
	pixelSrcRect.bottom = pixelSrcRect.top  + pixelDataHeight;
	pixelSrcRect.right  = pixelSrcRect.left + pixelDataWidth;
	
	// Create our destination rectangle for displayPixels in logical space
	VRect logicalSrcRect;
	logicalSrcRect.left = pixelSrcRect.left / scaleFactor;
	logicalSrcRect.top = pixelSrcRect.top / scaleFactor;
	logicalSrcRect.right = pixelSrcRect.right / scaleFactor;
	logicalSrcRect.bottom = pixelSrcRect.bottom / scaleFactor;
	
	/* Display the data. */
	
	// Setup global target rectangle for this blit in logical units
	PSPlatformContext windowContext;
	CGContextRef cgContext = (CGContextRef) [[NSGraphicsContext currentContext] graphicsPort];
	windowContext.fCGContextRef = (void *) cgContext;
	windowContext.fScreenRect.top = logicalSrcRect.top;
	windowContext.fScreenRect.left = logicalSrcRect.left;
	windowContext.fScreenRect.bottom = logicalSrcRect.bottom;
	windowContext.fScreenRect.right = logicalSrcRect.right;

	// Setup our rows and col in logical units
	short logicalDstRow = pixelDataDstRow / scaleFactor;
	short logicalDstCol = pixelDataDstCol / scaleFactor;
	
	if (gFilterRecord->displayPixels != NULL)
		(*(gFilterRecord->displayPixels)) (&outMap, &logicalSrcRect, logicalDstRow, logicalDstCol, &windowContext);

	[gDissolveController updateCursor];
}

@end

// end DissolveProxyView.m
