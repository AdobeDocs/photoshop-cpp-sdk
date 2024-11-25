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

//
//  PSConnectionController.h
//  pspalette
//
//  Created by Christoph on 10/28/10.
//  Copyright 2010 Adobe Systems Inc. All rights reserved.
//

#import "PSConnection.h"
#import "PSServersController.h"
#import <UIKit/UIKit.h>


@interface PSConnectionController : UIViewController <PSConnectionDelegate,
													  UIPopoverControllerDelegate>
{
	PSServersController*	serversController;
	UIPopoverController*	popoverController;
	
	NSString*			photoshopLabelString;
	CGSize				photoshopLabelStringSize;
	CGFloat				photoshopLabelOffsetToHide;
	NSTimeInterval		photoshopLabelAnimationDuration;
	
	NSTimer*	connectionStartedTimer;
}

@property (nonatomic, retain) IBOutlet UIButton* button;
@property (nonatomic, retain) IBOutlet UILabel* label;
@property (nonatomic, retain) IBOutlet UIView* labelView;

- (IBAction) buttonTouchUpInside:(id)sender;

- (void) dismissPopover:(BOOL)animated;

- (void) showLabel:(NSString*)string animated:(BOOL)animated;

@end
