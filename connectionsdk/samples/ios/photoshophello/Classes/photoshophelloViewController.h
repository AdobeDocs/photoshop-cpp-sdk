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
//  photoshophelloViewController.h
//  photoshophello
//
//  @author Allen Jeng, Photoshop QE Developer, Adobe Systems Inc.
//  Copyright 2010 Adobe Systems. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "ConnectionViewController.h"

@interface photoshophelloViewController : UIViewController <ConnectionViewControllerDelegate, NSStreamDelegate> 
{
	UILabel *connectStatus;
	UILabel *versionLabel;
	IBOutlet UITextView *inputField;
	IBOutlet UITextView *responseField;
	IBOutlet UITextView *colorResponseField;
	UIView *foregroundColorBox;
	UIView *backgroundColorBox;
	
	NSInputStream *inputStream;
	NSOutputStream *outputStream;
	NSMutableData *dataBuffer;
	int	packetBodySize;
	
	BOOL isConnected;
	
}

@property (nonatomic, retain) IBOutlet UILabel *connectStatus;
@property (nonatomic, retain) IBOutlet UILabel *versionLabel;
@property (nonatomic, retain) IBOutlet UITextView *inputField;
@property (nonatomic, retain) IBOutlet UITextView *responseField;
@property (nonatomic, retain) IBOutlet UITextView *colorResponseField;
@property (nonatomic, retain) IBOutlet UIView *foregroundColorBox;
@property (nonatomic, retain) IBOutlet UIView *backgroundColorBox;

-(IBAction) connectButtonTapped:(id) sender;
-(IBAction) clearInputButton;
-(IBAction) clearOutputButton;
-(IBAction) createNewDocButton;
-(IBAction) sendJSButton;
-(IBAction) disconnectFromServerButton;
-(IBAction) testConnectionAliveButton: (id) sender;
-(void)subscribeColorChanges;

-(void)openStreams: (NSString *)nameOfService;
-(void)closeStreams;
-(void)sendJavaScriptMessage:(NSData*)dataToSend;
-(void)sendNetworkMessage:(NSData*)dataToSend type:(int)dataType;

-(void)processForegroundChange: (NSString *)string;
-(void)processBackgroundChange: (NSString *)string;

@end

