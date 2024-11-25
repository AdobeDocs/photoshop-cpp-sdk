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
//  blankexampleViewController.m
//  blankexample
//
//  Created by Allen Jeng on 4/4/11.
//  Copyright 2011 Adobe Systems. All rights reserved.
//

#import "blankexampleViewController.h"
#import "PSConnectionCommands.h"

@implementation blankexampleViewController

@synthesize bottomBarView;


/*
// The designated initializer. Override to perform setup that is required before the view is loaded.
- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil {
    self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil];
    if (self) {
        // Custom initialization
    }
    return self;
}
*/

/*
// Implement loadView to create a view hierarchy programmatically, without using a nib.
- (void)loadView {
}
*/


- (id) initWithNibName:(NSString*)nibNameOrNil bundle:(NSBundle*)nibBundleOrNil
{
	if ((self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil]) != nil)
	{
		connectionController = nil;
	}
	return self;
}


// Implement viewDidLoad to do additional setup after loading the view, typically from a nib.
- (void)viewDidLoad 
{
    [super viewDidLoad];

	// add connection controller
	connectionController = [[PSConnectionController alloc] initWithNibName:@"PSConnectionController" bundle:nil];
	[bottomBarView addSubview:connectionController.view];
	CGRect frame = connectionController.view.bounds;
	frame.origin.x = 430;
	frame.origin.y = 11;
	connectionController.view.frame = frame;
	connectionController.view.autoresizingMask = UIViewAutoresizingFlexibleLeftMargin;
}



// Override to allow orientations other than the default portrait orientation.
- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation {
    return NO;
}

- (void)didReceiveMemoryWarning {
	// Releases the view if it doesn't have a superview.
    [super didReceiveMemoryWarning];
	
	// Release any cached data, images, etc that aren't in use.
}

- (void)viewDidUnload {
	// Release any retained subviews of the main view.
	// e.g. self.myOutlet = nil;
	[connectionController release];
}


- (void)dealloc {
    [super dealloc];
}

@end
