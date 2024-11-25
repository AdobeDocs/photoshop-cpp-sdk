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
//  ConnectionViewController.h
//  HelloWorld
//
//  @author Allen Jeng, Photoshop QE Developer, Adobe Systems Inc.
//  Copyright 2010 Adobe Systems. All rights reserved.
//

#import <UIKit/UIKit.h>

// user defined string, for user defined selection
#define kUserDefinedService "Use IP Address"

// because this is going to be a modal dialog that pops asking for connection info, I need this to be a delegate
@protocol ConnectionViewControllerDelegate
-(void) doneButtonPressed:(NSArray *) values;
@end

@interface ConnectionViewController : UIViewController <UITableViewDataSource, UITableViewDelegate, NSNetServiceBrowserDelegate>
{
	id<ConnectionViewControllerDelegate> delegate;

	UITableView *myTableView;
	UITextField *serverBroadcastName;
	UITextField *serverName;
	UITextField *serverPassword;

	IBOutlet UIActivityIndicatorView *busyView;
	
	NSNetServiceBrowser *serviceBrowser;
	NSMutableArray *serviceList;
	NSNetService *selectedService;
}

@property (nonatomic, retain) id<ConnectionViewControllerDelegate> delegate;

@property (nonatomic, retain) IBOutlet UITableView *myTableView;
@property (nonatomic, retain) IBOutlet UITextField *serverBroadcastName;
@property (nonatomic, retain) IBOutlet UITextField *serverName;
@property (nonatomic, retain) IBOutlet UITextField *serverPassword;

@property (readwrite, retain) NSNetServiceBrowser *serviceBrowser;
@property (readwrite, retain) NSMutableArray *serviceList;
@property (readwrite, retain) NSNetService *selectedService;

@property (nonatomic, retain) IBOutlet UIActivityIndicatorView *busyView;

-(void)initSpinner;
-(void)spinBegin;
-(void)spinEnd;

@end
