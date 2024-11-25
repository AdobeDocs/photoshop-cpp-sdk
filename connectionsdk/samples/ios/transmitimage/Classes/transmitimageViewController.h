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
//  transmitimageViewController.h
//  transmitimage
//
//  @author Allen Jeng, Photoshop QE Developer, Adobe Systems Inc.
//  Copyright 2011 Adobe Systems. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "bonjourConnectViewController.h"

@interface transmitimageViewController : UIViewController 
<bonjourConnectDelegate, UIPopoverControllerDelegate, NSStreamDelegate, 
UIImagePickerControllerDelegate, UINavigationControllerDelegate,
UIAlertViewDelegate>
{
	UIPopoverController *popoverConnect;
	IBOutlet UILabel *connectionStatusLabel;
	IBOutlet UITextView *outputMessage;
	
	IBOutlet UISwitch *imageSwitch;
	IBOutlet UILabel *imageType;
	IBOutlet UILabel *myVersion;
	
	UIPopoverController *imagePopover;
	UIImagePickerController *ipc;
	IBOutlet UIImageView *imageDisplay;
	
	UIActivityIndicatorView *busyView;
	
	NSInputStream *inputStream;
	NSOutputStream *outputStream;
	NSMutableData *dataBuffer;
	int packetBodySize;
	
	BOOL isConnected;
	BOOL isJPEG;
}

@property (nonatomic, retain) UIPopoverController *popoverConnect;
@property (nonatomic, retain) IBOutlet UILabel *connectionStatusLabel;
@property (nonatomic, retain) IBOutlet UITextView *outputMessage;
@property (nonatomic, retain) UIImageView *imageDisplay;
@property (nonatomic, retain) UIPopoverController *imagePopover;
@property (nonatomic, retain) IBOutlet UILabel *imageType;
@property (nonatomic, retain) IBOutlet UISwitch *imageSwitch;
@property (nonatomic, retain) IBOutlet UILabel *myVersion;
@property (nonatomic, retain) UIActivityIndicatorView *busyView;

-(IBAction) clearOutputButton;
-(IBAction) connectButton:(id) sender;
-(IBAction) pickImageButton;
-(IBAction) imageTypeSwitch:(id) sender;
-(IBAction) getImageFromPhotoshop:(id)sender;
-(IBAction) sendImageToPhotoshop:(id) sender;

-(void)openStreams;
-(void)closeStreams;
-(void)sendJavaScriptMessage:(NSData*)dataToSend;
-(void)sendNetworkMessage:(NSData*)dataToSend type:(int)dataType;

-(void)initSpinner;
-(void)spinBegin;
-(void)spinEnd;

-(void)logMessage:(NSString *) msg clearLine:(BOOL)wantClearLine;
-(NSString *)getCurrentTimeStamp;

-(void)releaseImagePickerPopover;

@end

