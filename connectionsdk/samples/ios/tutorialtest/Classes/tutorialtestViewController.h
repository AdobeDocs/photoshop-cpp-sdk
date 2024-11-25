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
//  tutorialtestViewController.h
//  tutorialtest
//
//  @author Allen Jeng, Photoshop QE Developer, Adobe Systems Inc.
//  Copyright 2011 Adobe Systems Inc. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "bonjourConnectViewController.h"

@interface tutorialtestViewController : UIViewController 
<bonjourConnectDelegate, UIPopoverControllerDelegate, NSStreamDelegate, 
UINavigationControllerDelegate, UIAlertViewDelegate>
{
	UIPopoverController *popoverConnect;
	IBOutlet UILabel *connectionStatusLabel;
	IBOutlet UITextView *outputMessage;
	
	IBOutlet UILabel *myVersion;
	UIActivityIndicatorView *busyView;
	
	NSInputStream *inputStream;
	NSOutputStream *outputStream;
	NSMutableData *dataBuffer;
	int packetBodySize;
	
	BOOL isConnected;
	NSString *waitState;
	BOOL playAllIsOn;
}

@property (nonatomic, retain) UIPopoverController *popoverConnect;
@property (nonatomic, retain) IBOutlet UILabel *connectionStatusLabel;
@property (nonatomic, retain) IBOutlet UITextView *outputMessage;
@property (nonatomic, retain) IBOutlet UILabel *myVersion;
@property (nonatomic, retain) UIActivityIndicatorView *busyView;

-(IBAction) clearOutputButton;
-(IBAction) connectButton:(id) sender;

-(IBAction) doStep1;
-(IBAction) doStep2;
-(IBAction) doStep3;
-(IBAction) doStep4;
-(IBAction) doAllSteps;

-(void)openStreams;
-(void)closeStreams;
-(void)sendJavaScriptMessage:(NSData*)dataToSend;
-(void)sendNetworkMessage:(NSData*)dataToSend type:(int)dataType;

-(void)initSpinner;
-(void)spinBegin;
-(void)spinEnd;

-(void)logMessage:(NSString *) msg clearLine:(BOOL)wantClearLine;
-(NSString *)getCurrentTimeStamp;

@end

