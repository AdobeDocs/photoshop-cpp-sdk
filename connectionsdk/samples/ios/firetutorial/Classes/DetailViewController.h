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
//  DetailViewController.h
//  firetutorial
//
//  @author Allen Jeng, Photoshop QE Developer, Adobe Systems Inc.
//  Copyright 2011 Adobe Systems Inc. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "bonjourConnectViewController.h"

@interface DetailViewController : UIViewController 
<UIPopoverControllerDelegate, UISplitViewControllerDelegate, bonjourConnectDelegate, 
NSStreamDelegate, UINavigationControllerDelegate, UIAlertViewDelegate> 
{
    
    UIPopoverController *popoverController;
    UIToolbar *toolbar;
    
    id detailItem;
    UILabel *detailDescriptionLabel;
	UIWebView *htmlView;
	UIButton *buttonControl;
	UIButton *connectBtnControl;
	UILabel *versionInfo;
	
	UIActivityIndicatorView *busyView;

	NSInputStream *inputStream;
	NSOutputStream *outputStream;
	NSMutableData *dataBuffer;
	int packetBodySize;
	
	BOOL isConnected;
	int currentSelection;
}

@property (nonatomic, retain) IBOutlet UIToolbar *toolbar;

@property (nonatomic, retain) id detailItem;
@property (nonatomic, retain) IBOutlet UILabel *detailDescriptionLabel;
@property (nonatomic, retain) IBOutlet UIWebView *htmlView;
@property (nonatomic, retain) IBOutlet UIButton *buttonControl;
@property (nonatomic, retain) IBOutlet UIButton *connectBtnControl;
@property (nonatomic, retain) UIActivityIndicatorView *busyView;
@property (nonatomic, retain) IBOutlet UILabel *versionInfo;

-(IBAction) doItButton;
-(IBAction) connectPSButton:(id) sender;

-(void)openStreams;
-(void)closeStreams;
-(void)sendJavaScriptMessage:(NSData*)dataToSend;
-(void)sendNetworkMessage:(NSData*)dataToSend type:(int)dataType;

-(void)initSpinner;
-(void)spinBegin;
-(void)spinEnd;


@end
