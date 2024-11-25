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

#import <Cocoa/Cocoa.h>

#include "PSIntTypes.h"


typedef enum

	{
	ZoomCommandUninited,
	ZoomCommandHigh,
	ZoomCommandLow
	
	} ZoomCommandState;

/* Make sure this is unique to you and everyone you might encounter, search for
"Preventing Name Conflicts" or use this link
http://developer.apple.com/mac/library/documentation/UserExperience/Conceptual/PreferencePanes/Tasks/Conflicts.html
*/
@interface DissolveProxyView : NSView {
	NSColor * currentColor;
	ZoomCommandState currentZoomState;
}

- (CGFloat)getCurrentScaleFactor;
- (void)setDispositionColor:(int16)newColor;
- (BOOL)isFlipped;


@end

// end DissolveProxyView.h
