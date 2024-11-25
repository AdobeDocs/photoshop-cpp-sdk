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
//  connectionViewController.m
//  transmitimage
//
//  @author Allen Jeng, Photoshop QE Developer, Adobe Systems Inc.
//  Copyright 2011 Adobe Systems. All rights reserved.
//

#import "connectionViewController.h"


@implementation connectionViewController

@synthesize  delegate, serverName, serverPassword, serviceName, isUserDefined;
 

/*
 * the meat of sending info back
 */
-(void) updateInfo
{
	NSArray *values;
	
	values = [[NSArray alloc]
			  initWithObjects:serverName.text,
			  serverPassword.text,
			  nil];
	
	// call the delegate with info
	[delegate connectionSelected:values];
	
	// release what we use
	[values release];
	
}


// Implement viewDidLoad to do additional setup after loading the view, typically from a nib.
- (void)viewDidLoad {
    [super viewDidLoad];
	
	if (isUserDefined)
	{
		serverName.enabled = YES;
	}
	else 
	{
		serverName.enabled = NO;
		serverName.text = self.serviceName;
	}

}


 - (void)viewWillDisappear:(BOOL)animated 
{
	[self updateInfo];
	[super viewWillDisappear:animated];
}




- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation {
    // Overriden to allow any orientation.
    return YES;
}



#pragma mark -
#pragma mark Memory management

- (void)didReceiveMemoryWarning {
    // Releases the view if it doesn't have a superview.
    [super didReceiveMemoryWarning];
    
    // Release any cached data, images, etc. that aren't in use.
}


- (void)viewDidUnload {
    [super viewDidUnload];
    // Release any retained subviews of the main view.
    // e.g. self.myOutlet = nil;
}


- (void)dealloc {
    [super dealloc];
}


@end
