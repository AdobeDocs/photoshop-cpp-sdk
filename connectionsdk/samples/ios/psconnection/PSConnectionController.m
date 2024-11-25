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
//  PSConnectionController.m
//  pspalette
//
//  Created by Christoph on 10/28/10.
//  Copyright 2010 Adobe Systems Inc. All rights reserved.
//

#import "PSConnection.h"
#import "PSConnectionController.h"

@interface PSConnectionController (private)

- (void) hideLabel:(NSString*)string animated:(BOOL)animated withDelay:(NSTimeInterval)delay;
- (void) showPhotoshopLabelAnimationDidStop:(NSString*)animationID finished:(NSNumber*)finished context:(void*)context;
- (void) connectionStartedTimerFired:(NSTimer*)timer;

@end

@implementation PSConnectionController

/**********************************************************************************
* Class synthesizication
**********************************************************************************/

@synthesize button;
@synthesize	label;
@synthesize	labelView;

/**********************************************************************************
* Class overload methods
**********************************************************************************/

- (id) initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil
{
    if ((self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil]))
	{
		connectionStartedTimer;
    }
    return self;
}

- (void) viewDidLoad
{
    [super viewDidLoad];

	// disable the photoshop button to start with
	// this puts the button into the default state
	button.enabled = YES;

	// localize the connected string and calculate
	// the offset based on the length of the string
	photoshopLabelString = NSLocalizedString(@"Connected", @"$$$/PSConnection/Connected=Connected");
	photoshopLabelStringSize = [photoshopLabelString sizeWithFont:label.font];
	photoshopLabelOffsetToHide = label.bounds.size.width - photoshopLabelStringSize.width;
	photoshopLabelAnimationDuration = 0.5;

	// hide the photoshop label to start with
	label.alpha = 0;
	CGPoint center = label.center;
	center.x += photoshopLabelOffsetToHide;
	label.center = center;
	
	// become the PSConnection delegate
	[[PSConnection sharedPSConnection] addDelegate:self];
}

- (void)didReceiveMemoryWarning {
    // Releases the view if it doesn't have a superview.
    [super didReceiveMemoryWarning];
    
    // Release any cached data, images, etc that aren't in use.
}

- (void)viewDidUnload
{
    [super viewDidUnload];
    // Release any retained subviews of the main view.
    // e.g. self.myOutlet = nil;
}


- (void) dealloc
{
    [super dealloc];
	
	// stop the timer if running
	if (connectionStartedTimer != nil && [connectionStartedTimer isValid])
	{
		[connectionStartedTimer invalidate];
		connectionStartedTimer = nil;
	}
}

/**********************************************************************************
* Class delegate methods
**********************************************************************************/

- (void) connectionStarted:(PSConnection*)connection
{
	// stop the timer if running
	if (connectionStartedTimer != nil && [connectionStartedTimer isValid])
	{
		[connectionStartedTimer invalidate];
		connectionStartedTimer = nil;
	}
	
	// start the timer
	connectionStartedTimer = [NSTimer scheduledTimerWithTimeInterval:2.0
															  target:self
															selector:@selector(connectionStartedTimerFired:)
															userInfo:nil
															 repeats:NO];
	
	// ask the table in the servers controller to update
	if (serversController != nil)
	{
		[serversController.tableController.tableView reloadData];
	}
}

- (void) connectionOpened:(PSConnection*)connection
{
	// stop the timer if necessary
	// stop the timer if running
	if (connectionStartedTimer != nil && [connectionStartedTimer isValid])
	{
		[connectionStartedTimer invalidate];
		connectionStartedTimer = nil;
	}

	// update to show a connection
	button.selected = YES;
	[self showLabel:NSLocalizedString(@"Connected", @"$$$/PSConnection/Connected=Connected")
		   animated:YES];
	
	// ask the table in the servers controller to update
	if (serversController != nil)
	{
		[serversController.tableController.tableView reloadData];
	}
	
	// dismiss the pop-over
	[self dismissPopover:YES];
}

- (void) connectionClosed:(PSConnection*)connection
{
	// stop the timer if necessary
	// stop the timer if running
	if (connectionStartedTimer != nil && [connectionStartedTimer isValid])
	{
		[connectionStartedTimer invalidate];
		connectionStartedTimer = nil;
	}

	// update the button
	button.selected = NO;

	// this is kind of sneaky
	// it will only change the label to Disconnected if the label
	// is currently Connected, and that works fine for when an invalid
	// password error is sent with connectionFailed(), but it' not really
	// robust or fool-proof, probably should track the last error sent instead
	if ([label.text compare:NSLocalizedString(@"Connected", @"$$$/PSConnection/Connected=Connected")] == NSOrderedSame)
	{
		label.text = NSLocalizedString(@"Disconnected", @"$$$/PSConnection/Disconnected=Disconnected");
	}
	
	// ask the table in the servers controller to update
	if (serversController != nil)
	{
		[serversController.tableController.tableView reloadData];
	}
}

- (void) connectionFailed:(PSConnection*)connection withError:(PSConnectionError)error
{
	// stop the timer if necessary
	// stop the timer if running
	if (connectionStartedTimer != nil && [connectionStartedTimer isValid])
	{
		[connectionStartedTimer invalidate];
		connectionStartedTimer = nil;
	}

	// update the buttons
	button.selected = NO;
	
	// get a human readable form of the error
	NSString* errorString = [PSConnection stringForError:error];
	
	// show the error
	[self showLabel:errorString animated:YES];
		   
	// ask the table in the servers controller to update
	if (serversController != nil)
	{
		[serversController.tableController.tableView reloadData];
	}
}

- (void) connectionServersUpdated:(PSConnection*)connection
{
	// ask the table in the servers controller to update
	if (serversController != nil)
	{
		[serversController.tableController.tableView reloadData];
	}
}

- (void) popoverControllerDidDismissPopover:(UIPopoverController*)controller
{
	// clean it up
	[serversController release];
	serversController = nil;
	
	// and the popover
	[popoverController release];
	popoverController = nil;
}

/**********************************************************************************
* Class private methods
**********************************************************************************/

- (void) showLabel:(NSString*)string animated:(BOOL)animated
{
	// set the label text
	if (string != nil && string.length > 0)
	{
		label.text = string;
	}
	
	// measure the length of the string currently in the label
	// the string could be longer than the label but will get clipped
	CGSize stringSize = CGSizeMake(label.bounds.size.width, 0);
	NSString* currentString = label.text;
	if (currentString != nil && currentString.length > 0)
	{
		CGSize offset = [currentString sizeWithFont:label.font];
		stringSize.width = offset.width;
	}

	// only apply the offset the label is not animating
	CGPoint center = labelView.center;
	center.x += stringSize.width;
	label.center = center;
	
	// slide the label from underneath the button
	[UIView beginAnimations:@"ShowPhotoshopLabel" context:nil];
	[UIView setAnimationDelegate:self];
	[UIView setAnimationDidStopSelector:@selector(showPhotoshopLabelAnimationDidStop:finished:context:)];
	[UIView setAnimationCurve:UIViewAnimationCurveEaseOut];
	[UIView setAnimationDuration:photoshopLabelAnimationDuration];
	label.alpha = 1.0;
	label.center = labelView.center;
	[UIView commitAnimations];
}

- (void) hideLabel:(NSString*)string animated:(BOOL)animated withDelay:(NSTimeInterval)delay
{
	// set the label text
	if (string != nil && string.length > 0)
	{
		label.text = string;
	}
	
	// measure the length of the string currently in the label
	// the string could be longer than the label but will get clipped
	CGSize stringSize = CGSizeMake(label.bounds.size.width, 0);
	NSString* currentString = label.text;
	if (currentString != nil && currentString.length > 0)
	{
		CGSize offset = [currentString sizeWithFont:label.font];
		stringSize.width = offset.width;
	}

	// slide the label away under the button
	[UIView beginAnimations:@"HidePhotoshopLabel" context:nil];
	[UIView setAnimationDelay:delay];
	[UIView setAnimationCurve:UIViewAnimationCurveEaseIn];
	[UIView setAnimationDuration:photoshopLabelAnimationDuration];
	label.alpha = 0;
	CGPoint center = labelView.center;
	center.x += stringSize.width;
	label.center = center;
	[UIView commitAnimations];
}

- (void) showPhotoshopLabelAnimationDidStop:(NSString*)animationID finished:(NSNumber*)finished context:(void*)context
{
	[self hideLabel:nil animated:YES withDelay:10.0];
}

- (void) connectionStartedTimerFired:(NSTimer*)timer
{
	// stop the timer if running
	if (connectionStartedTimer != nil && [connectionStartedTimer isValid])
	{
		[connectionStartedTimer invalidate];
		connectionStartedTimer = nil;
	}
	
	// show a connecting message
	NSString* string = NSLocalizedString(@"Connecting", @"$$$/PSConnection/Connecting=Connecting");
	[self showLabel:string animated:YES];
}

/**********************************************************************************
* Class public methods
**********************************************************************************/

- (IBAction) buttonTouchUpInside:(id)sender
{
	// create servers controller
	serversController = [[PSServersController alloc] initWithNibName:@"PSServersController" bundle:nil];
	
	// present it in a popover
	popoverController = [[UIPopoverController alloc] initWithContentViewController:serversController];
	popoverController.popoverContentSize = CGSizeMake(320, 320);
	[popoverController presentPopoverFromRect:button.frame inView:self.view permittedArrowDirections:UIPopoverArrowDirectionAny animated:YES];
	popoverController.delegate = self;

	// set the popover reference
	serversController.popoverParentController = self;
	serversController.popoverController = popoverController;
	serversController.popoverPresentRect = button.frame;
}

- (void) dismissPopover:(BOOL)animated
{
	// nothing to do if no pop-over
	if (popoverController == nil)
	{
		return;
	}
	
	// dismiss the popover
	[popoverController dismissPopoverAnimated:animated];
	[self popoverControllerDidDismissPopover:popoverController];
}

@end
