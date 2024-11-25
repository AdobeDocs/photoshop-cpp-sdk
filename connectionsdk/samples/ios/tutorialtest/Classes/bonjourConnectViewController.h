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
//  bonjourConnectViewController.h
//  transmitimage
//
//  @author Allen Jeng, Photoshop QE Developer, Adobe Systems Inc.
//  Copyright 2011 Adobe Systems. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "connectionViewController.h"

// hard coded service name
#define kNameOfService "_photoshopserver._tcp."
// used to ID user defined stuff
#define kUserDefineName "User Defined"


@protocol bonjourConnectDelegate
-(void) bonjourConnectSelected:(NSArray *) values;
@end


@interface bonjourConnectViewController : UITableViewController <NSNetServiceBrowserDelegate, connectionDelegate>
{
	id<bonjourConnectDelegate> delegate;
	
	UITableView *myTableView;
	
	NSNetServiceBrowser *serviceBrowser;
	NSMutableArray *serviceList;
	NSNetService *selectedService;
	
	NSString *serverName;
	NSString *serverPwd;
	BOOL isIPConnection;
}

@property (nonatomic, assign) id<bonjourConnectDelegate> delegate;

@property (nonatomic, retain) IBOutlet UITableView *myTableView;

@property (readwrite, retain) NSNetServiceBrowser *serviceBrowser;
@property (readwrite, retain) NSMutableArray *serviceList;
@property (readwrite, retain) NSNetService *selectedService;

@property (nonatomic, retain) NSString *serverName;
@property (nonatomic, retain) NSString *serverPwd;
@property (nonatomic, assign) BOOL isIPConnection;

@end
