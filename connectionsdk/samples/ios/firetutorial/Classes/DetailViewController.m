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
//  DetailViewController.m
//  firetutorial
//
//  @author Allen Jeng, Photoshop QE Developer, Adobe Systems Inc.
//  Copyright 2011 Adobe Systems Inc. All rights reserved.
//

#import "DetailViewController.h"
#import "RootViewController.h"
#import "bonjourConnectViewController.h"
#import "PhotoshopLib.h"

// encryption lib
#include "PSCryptorAPI.h"

// version
#define kVersionInfo "1.0.5"

// unique transaction id
static int transaction_id = 1;

// have a cryptor object ready for use
static PSCryptorRef sCryptorRef = NULL;


@interface DetailViewController ()
@property (nonatomic, retain) UIPopoverController *popoverController;
- (void)configureView;
@end



@implementation DetailViewController

@synthesize toolbar, popoverController, detailItem, detailDescriptionLabel;
@synthesize htmlView, buttonControl, connectBtnControl, busyView, versionInfo;

#pragma mark -
#pragma mark Managing the detail item

/*
 * show alert according to the message
 */
-(void) showMessage:(NSString *)title theMessage:(NSString *)msg
{
	UIAlertView *alert = [[UIAlertView alloc]
						  initWithTitle:title 
						  message:msg
						  delegate:nil 
						  cancelButtonTitle:@"OK" 
						  otherButtonTitles:nil];
	[alert show];
	[alert release];
	
}

/*
 * multi-purpose do it button
 */
-(IBAction) doItButton
{
	if (!isConnected)
	{
		[self showMessage:@"Info" theMessage:@"Please connect to Photoshop by clicking on \"Connect to PS\" button"];
		return;
	}

	NSString *stringToSend;
	NSData *dataToSend;
	
	// else -- lets do stuff!
	switch (currentSelection) 
	{
		case 0:
			break;
			
		case 1:
			{
				// create the new doc
				stringToSend = [PhotoshopLib createNewDoc:500 docHeight:500 docName:@"Fire"];
				dataToSend = [stringToSend dataUsingEncoding:NSUTF8StringEncoding];
				[self sendJavaScriptMessage:dataToSend];
				
				// fill background with black
				stringToSend = [PhotoshopLib fillLayerWithColor:0 colorGreen:0 colorBlue:0];
				dataToSend = [stringToSend dataUsingEncoding:NSUTF8StringEncoding];
				[self sendJavaScriptMessage:dataToSend];
			}
			break;
		
		case 2:
			{
				// create the text layer
				stringToSend = [PhotoshopLib createTextJS:@"FIRE" 
												 fontName:@"Arial-BoldMT" 
												 fontSize:200 
											 fontColorRed:255 
										   fontColorGreen:255 
											fontColorBlue:255 
												positionX:0.2 
												positionY:4.6];
				dataToSend = [stringToSend dataUsingEncoding:NSUTF8StringEncoding];
				[self sendJavaScriptMessage:dataToSend];
				
				// rasterize the layer
				stringToSend = [PhotoshopLib rasterlizeActiveLayerJS ];
				dataToSend = [stringToSend dataUsingEncoding:NSUTF8StringEncoding];
				[self sendJavaScriptMessage:dataToSend];
				
				// duplicate the layer without switching active layer
				stringToSend = [PhotoshopLib duplicateActiveLayerJS ];
				dataToSend = [stringToSend dataUsingEncoding:NSUTF8StringEncoding];
				[self sendJavaScriptMessage:dataToSend];
				
				// hide the dup - which is now layer 0
				stringToSend = [PhotoshopLib setActiveLayerVisibleJS:NO layerNumber:0];
				dataToSend = [stringToSend dataUsingEncoding:NSUTF8StringEncoding];
				[self sendJavaScriptMessage:dataToSend];				
			}
			break;
	
		case 3:
			{
				// rotate the main layer 90 degrees on the bottom left anchor
				stringToSend = [PhotoshopLib rotateActiveLayerJS:90 anchorPosition:kAnchorPositionBottomLeft];
				dataToSend = [stringToSend dataUsingEncoding:NSUTF8StringEncoding];
				[self sendJavaScriptMessage:dataToSend];				
			}
			break;

		case 4:
			{
				// apply wind 5 times
				for (int i=0; i < 5; i++) 
				{
					stringToSend = [PhotoshopLib applyWindJS];
					dataToSend = [stringToSend dataUsingEncoding:NSUTF8StringEncoding];
					[self sendJavaScriptMessage:dataToSend];				
				}
			}
			break;

		case 5:
			{
				// rotate that layer back with -90 degrees on the top left anchor
				stringToSend = [PhotoshopLib rotateActiveLayerJS:-90 anchorPosition:kAnchorPositionTopLeft];
				dataToSend = [stringToSend dataUsingEncoding:NSUTF8StringEncoding];
				[self sendJavaScriptMessage:dataToSend];
			}
			break;

		case 6:
			{
				// apply gaussian blur with radius 4 pixels
				stringToSend = [PhotoshopLib gaussianBlurActiveLayerJS:4];
				dataToSend = [stringToSend dataUsingEncoding:NSUTF8StringEncoding];
				[self sendJavaScriptMessage:dataToSend];
			}
			break;

		case 7:
			{
				// select the smudge tool
				stringToSend = [PhotoshopLib switchToolJS:kToolSmudge];
				dataToSend = [stringToSend dataUsingEncoding:NSUTF8StringEncoding];
				[self sendJavaScriptMessage:dataToSend];
				
				// set the brush size to 50
				stringToSend = [PhotoshopLib paintBrushSize:50.0];
				dataToSend = [stringToSend dataUsingEncoding:NSUTF8StringEncoding];
				[self sendJavaScriptMessage:dataToSend];
				
				// send the smudge binary script to Photoshop
				NSString *blobObject = [[NSBundle mainBundle] pathForResource:@"smudge-fire" ofType:@""];
				NSData *blobData = [[NSData alloc] initWithContentsOfFile:blobObject];
				[self sendNetworkMessage:blobData type:5];
				
				[blobData release];
				
				// note that there's extra code in the receive section (handleEvents) 
				// that waits for a file URL so that painting code can be played
			}
			break;

		case 8:
			{
				// prep for colorize of fire
				// duplicate the background layer, link the active with the dup, merge the linked, and rename it
				stringToSend = [PhotoshopLib mergeWithCopyOfBGLayerJS:@"new FIRE"];
				dataToSend = [stringToSend dataUsingEncoding:NSUTF8StringEncoding];
				[self sendJavaScriptMessage:dataToSend];				
			}
			break;
		
		case 9:
			{
				// set hue saturation:  hue: 30, sat: 90, lightness:0, colorize: true
				stringToSend = [PhotoshopLib setHueSaturationJS:30 
												saturationValue:90 
												 lightnessValue:0 
													   colorize:YES];
				dataToSend = [stringToSend dataUsingEncoding:NSUTF8StringEncoding];
				[self sendJavaScriptMessage:dataToSend];
			}
			break;

		case 10:
			{
				// intesify the fire
				// duplicate and reanme active layer to "strong fire"
				stringToSend = [PhotoshopLib duplicateActiveLayerJS:@"strong fire"];
				dataToSend = [stringToSend dataUsingEncoding:NSUTF8StringEncoding];
				[self sendJavaScriptMessage:dataToSend];
				
				// set hue sat with -20, 0, 0, false
				stringToSend = [PhotoshopLib setHueSaturationJS:-20 
												saturationValue:0 
												 lightnessValue:0 
													   colorize:NO];
				dataToSend = [stringToSend dataUsingEncoding:NSUTF8StringEncoding];
				[self sendJavaScriptMessage:dataToSend];
				
				// set blend mode to overlay
				stringToSend = [PhotoshopLib changeActiveLayerBlendModeOverlay];
				dataToSend = [stringToSend dataUsingEncoding:NSUTF8StringEncoding];
				[self sendJavaScriptMessage:dataToSend];
			}
			break;

		case 11:
			{
				// switch active layer to top most layer
				stringToSend = [PhotoshopLib setActiveLayerVisibleJS:YES layerNumber:0];
				dataToSend = [stringToSend dataUsingEncoding:NSUTF8StringEncoding];
				[self sendJavaScriptMessage:dataToSend];

				// switch to that layer
				stringToSend = [PhotoshopLib switchActiveLayer:0];
				dataToSend = [stringToSend dataUsingEncoding:NSUTF8StringEncoding];
				[self sendJavaScriptMessage:dataToSend];
				
				// lock transparent pixel (so that we're painting on pixels only
				stringToSend = [PhotoshopLib lockActiveLayerTransparentPixel:YES];
				dataToSend = [stringToSend dataUsingEncoding:NSUTF8StringEncoding];
				[self sendJavaScriptMessage:dataToSend];

				// do orange yellow orange gradient on text
				stringToSend = [PhotoshopLib applyOrangeYellowGradient];
				dataToSend = [stringToSend dataUsingEncoding:NSUTF8StringEncoding];
				[self sendJavaScriptMessage:dataToSend];

				// do special orange/red shadow
				stringToSend = [PhotoshopLib setSpecialShadow];
				dataToSend = [stringToSend dataUsingEncoding:NSUTF8StringEncoding];
				[self sendJavaScriptMessage:dataToSend];
			}
			break;

			
		case 12:
			// final - finished step, nothing to do.  PLUS... the Do It button should be disabled, hence, we can never reach case 12
			break;
			
		default:
			break;
	} 
	
	
}


/*
 *  Step for doing the painting:  We need to wait for Photoshop to give us a URL to the blob object
 */
-(void) doPaintStep:(NSString *) path
{
	NSString *stringToSend = [PhotoshopLib playStrokeJS:path];
	NSData * dataToSend = [stringToSend dataUsingEncoding:NSUTF8StringEncoding];
	[ self sendJavaScriptMessage:dataToSend ];
}


/*
 When setting the detail item, update the view and dismiss the popover controller if it's showing.
 */
- (void)setDetailItem:(id)newDetailItem {
    if (detailItem != newDetailItem) {
        [detailItem release];
        detailItem = [newDetailItem retain];
        
        // Update the view.
        [self configureView];
    }

    if (self.popoverController != nil) {
        [self.popoverController dismissPopoverAnimated:YES];
    }        
}


- (void)configureView {
    // Update the user interface for the detail item.
	if ([[detailItem description] isEqualToString:@"intro"])
	{
		[buttonControl setHidden:YES];
		currentSelection = 0;
	}
	else if ([[detailItem description] isEqualToString:@"finish"])
	{
		[buttonControl setHidden:YES];
		currentSelection = 12;
	}
	else
	{
		[buttonControl setHidden:NO];
		currentSelection = [[detailItem description] intValue];
	}
	
	NSString *pathToHTML = [[NSBundle mainBundle] pathForResource:[detailItem description] ofType:@"html"];
	NSURL *url = [NSURL fileURLWithPath:pathToHTML isDirectory:NO];	
	[htmlView loadRequest:[NSURLRequest requestWithURL:url]];
}


#pragma mark -
#pragma mark Split view support

- (void)splitViewController: (UISplitViewController*)svc willHideViewController:(UIViewController *)aViewController withBarButtonItem:(UIBarButtonItem*)barButtonItem forPopoverController: (UIPopoverController*)pc {
    
    barButtonItem.title = @"Fire Tutorial";
    NSMutableArray *items = [[toolbar items] mutableCopy];
    [items insertObject:barButtonItem atIndex:0];
    [toolbar setItems:items animated:YES];
    [items release];
    self.popoverController = pc;
}


// Called when the view is shown again in the split view, invalidating the button and popover controller.
- (void)splitViewController: (UISplitViewController*)svc willShowViewController:(UIViewController *)aViewController invalidatingBarButtonItem:(UIBarButtonItem *)barButtonItem {
    
    NSMutableArray *items = [[toolbar items] mutableCopy];
    [items removeObjectAtIndex:0];
    [toolbar setItems:items animated:YES];
    [items release];
    self.popoverController = nil;
}


#pragma mark -
#pragma mark Rotation support

// Ensure that the view controller supports rotation and that the split view can therefore show in both portrait and landscape.
- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation {
    return YES;
}

// ---------------------------------------------
#pragma mark -
#pragma mark Busy View Support

/**
 * Initialize UIActivityIndicatorView
 */
-(void)initSpinner
{
	busyView = [[[UIActivityIndicatorView alloc] 
				 initWithActivityIndicatorStyle:UIActivityIndicatorViewStyleWhiteLarge] autorelease];
	
	CGPoint theCenter = (CGPoint) [self.view center];
	busyView.center = theCenter;
	[self.view addSubview:busyView];
}

/**
 * Start the busy spinning
 */
-(void)spinBegin
{
	[busyView startAnimating];
}

/**
 * Stop spinning
 */
-(void)spinEnd
{
	[busyView stopAnimating];
	[busyView hidesWhenStopped];
}


// ---------------------------------------------
#pragma mark -
#pragma mark Bonjour Connections

/**
 *  handle alert dialog button press
 */
-(void)alertView:(UIAlertView *)alertView clickedButtonAtIndex:(NSInteger)buttonIndex
{
	if (buttonIndex == 0)
	{
		// user wants to disconnect
		[self closeStreams];
	}
}

/* 
 * handle popover code for bonjour connect
 */
-(IBAction) connectPSButton:(id) sender
{
	
	// check to see if the user is connected.  If so, ask if they want to disconnect
	if (isConnected)
	{
		UIAlertView *askDisconnectDialog = [[UIAlertView alloc]
											initWithTitle:@"Connect/Disconnect" 
											message:@"You are currently connected, would you like to disconnect?" 
											delegate:self 
											cancelButtonTitle:@"Disconnect" 
											otherButtonTitles:@"No", nil];
		[askDisconnectDialog show];
		[askDisconnectDialog release];
	}
	else 
	{
		//build our custom popover view
		UIViewController* popoverContent = [[UIViewController alloc] init];
		UIView* popoverView = [[UIView alloc] initWithFrame:CGRectMake(0, 0, 300, 300)];
		
		popoverView.backgroundColor = [UIColor greenColor];
		popoverContent.view = popoverView;

		//resize the popover view shown
		//in the current view to the view's size
		popoverContent.contentSizeForViewInPopover = CGSizeMake(300, 300);
		
		// bonjour connect view
		bonjourConnectViewController *bjc = [[bonjourConnectViewController alloc] init];
		bjc.navigationItem.title = @"Connections";		
		bjc.delegate = self;
		
		UINavigationController *navController =
		[[UINavigationController alloc] initWithRootViewController:bjc];
		
		//create a popover controller
		self.popoverController = [[UIPopoverController alloc] initWithContentViewController:navController];
		
		[bjc release];
		[navController release];
		
		//present the popover view non-modal with a
		//refrence to the button pressed within the current view
		[self.popoverController presentPopoverFromRect:connectBtnControl.frame
												inView:self.view
							  permittedArrowDirections:UIPopoverArrowDirectionAny
											  animated:YES];
		
		//release the popover content
		[popoverView release];
		[popoverContent release];
	}
	
}

/*
 * handle the delegate from bonjour connection
 */
-(void) bonjourConnectSelected:(NSArray *) values
{
	/* values passed to us
	 0: server's name or IP
	 1: the password
	 2: NSNetService object	 
	 */
	NSString *myServerName = (NSString *)[values objectAtIndex:0];
	NSString *myPassword = (NSString *)[values objectAtIndex:1];
	NSNetService *myService = [values objectAtIndex:2];
	BOOL okToConnect = YES;
	
	// keep it busy if someone decides to give none existing IP or firewall is blocking signal
	[NSThread detachNewThreadSelector:@selector(spinBegin) toTarget:self withObject:nil];
	
	if (myServerName.length <= 0)
	{
		// user didn't supply a user defined name!  Don't try connecting
		isConnected = NO;
		okToConnect = NO;
	}
	
	if (myPassword.length <= 0)
	{
		// didn't supply a password.
		isConnected = NO;
		okToConnect = NO;
		[self showMessage:@"Info" theMessage:@"No password supplied.  Not connected."];
	}
	
	// if we're still connected, first disconnect
	if (inputStream && outputStream)
	{
        [self closeStreams];
    }
	
	// make sure we're OK to connect before trying this!!
	if (okToConnect)
	{
		// create the cryptor object based on password
		if (sCryptorRef)
			DestroyPSCryptor(sCryptorRef);
		
		sCryptorRef = CreatePSCryptor([myPassword UTF8String]);
		
		if ([myService.name isEqualToString: @kUserDefineName])
		{
			// we have an user defined IP
			CFReadStreamRef readStream;
			CFWriteStreamRef writeStream;
			CFStreamCreatePairWithSocketToHost(kCFAllocatorDefault, (CFStringRef)myServerName  , 49494, &readStream, &writeStream);
		    inputStream = (NSInputStream *) readStream;
			outputStream = (NSOutputStream *) writeStream;
			
			[self openStreams];			
		}
		else 
		{
			// connect with bonjour object
			if ([myService getInputStream:&inputStream outputStream:&outputStream])
			{
				[self openStreams];
			}
			
		}		
	}
	
	[NSThread detachNewThreadSelector:@selector(spinEnd) toTarget:self withObject:nil];
}


// ------------------ Message sending -------------------------------
#pragma mark -
#pragma mark Message Sending

/**
 * a wrapper for normal data send of Javascript
 */
- (void)sendJavaScriptMessage:(NSData*)dataToSend 
{	
	[self sendNetworkMessage:dataToSend type:2 ];
}


/**
 * The code that send javascript to stream
 * @param dataToSend a UTF8 encoded string
 */
- (void)sendNetworkMessage:(NSData*)dataToSend type:(int)dataType 
{
	
	// put up a busy spinner.  If we're pumping something big to PS, we want the user to know that it's not hung
	[NSThread detachNewThreadSelector:@selector(spinBegin) toTarget:self withObject:nil];
	
	
    if (outputStream) 
	{
        int remainingToWrite = [dataToSend length];
		
		/* -------------------------------------------------------
		 we're writing things in this order:
		 Unencrypted:
		 1. total message length (not including the length itself)
		 2. communication stat
		 Encrypted:
		 3. Protocol version
		 4. Transaction ID
		 5. Message type
		 6. The message itself		 
		 ------------------------------------------------------- */ 
		int prolog_length = 12; // 3x 32 bit integers, not counting the length value itself
		
		int plainTextLength = prolog_length + remainingToWrite;
		size_t encryptedLength = plainTextLength;
		
		encryptedLength = CryptorGetEncryptedLength(plainTextLength);
		
		// ---- UNENCRYPTED PART ------
		// write length of message as 32 bit signed int, includes all bytes after the length
		int swabbed_temp = htonl( encryptedLength + 4 );
		NSInteger bytesWritten = [outputStream write:(const uint8_t*)&swabbed_temp maxLength:4];	
		
		// check to make sure we're able to write to stream
		if (bytesWritten != 4)
		{
			NSError *err = [outputStream streamError];
			[self showMessage:@"ERROR" theMessage:[NSString stringWithFormat:@"Did not succesful write to stream because: %@", [err localizedFailureReason]]];
			if ([[err localizedFailureReason] localizedCaseInsensitiveCompare:@"Operation timed out"] == NSOrderedSame)
				[self closeStreams];	
			
			// stop spinning
			[NSThread detachNewThreadSelector:@selector(spinEnd) toTarget:self withObject:nil];
			
			return;
		}
		
		/* 
		 stream status has the following status:  (check Apple's Developer Reference for more info)
		 NSStreamStatusNotOpen = 0,
		 NSStreamStatusOpening = 1,
		 NSStreamStatusOpen = 2,
		 NSStreamStatusReading = 3,
		 NSStreamStatusWriting = 4,
		 NSStreamStatusAtEnd = 5,
		 NSStreamStatusClosed = 6,
		 NSStreamStatusError = 7		 
		 */
		NSStreamStatus status = [outputStream streamStatus];
		if ( status == NSStreamStatusError ||
			status == NSStreamStatusClosed ||
			status == NSStreamStatusNotOpen)
		{
			NSError *err = [outputStream streamError];
			[self showMessage:@"Error" theMessage:[NSString stringWithFormat:@"ERROR: %@", [err localizedFailureReason]]];
			
			// stop spinning
			[NSThread detachNewThreadSelector:@selector(spinEnd) toTarget:self withObject:nil];
			
			return;
		}
		
		// the communication status is NOT encrypted
		// write communication status value as 32 bit unsigned int
		swabbed_temp = htonl( 0 );
		[outputStream write:(const uint8_t*)&swabbed_temp maxLength:4];
		
		// ------------------------------------------------
		// Encrypted section, until the end of the message		
		char *tempBuffer = (char *) malloc (encryptedLength);
		
		// protocol version, 32 bit unsigned integer
		swabbed_temp = htonl( 1 );
		memcpy (tempBuffer+0, (const void *) &swabbed_temp, 4);
		
		// transaction id, 32 bit unsigned integer
		swabbed_temp = htonl( transaction_id++ );
		memcpy (tempBuffer+4,(const void *) &swabbed_temp, 4);
		
		// content type, 32 bit unsigned integer
		swabbed_temp = htonl( dataType );		// javascript = 2
		memcpy (tempBuffer+8, (const void *) &swabbed_temp, 4);
		
		// the data to transmit
        unsigned char * marker = (unsigned char *)[dataToSend bytes];
		memcpy (tempBuffer+12, marker, remainingToWrite);
		
		marker = (unsigned char *) tempBuffer;
		
		// now encrypt the message packet
		if (sCryptorRef)
			EncryptDecrypt (sCryptorRef, true, tempBuffer, plainTextLength, tempBuffer, encryptedLength, &encryptedLength);
		
		// write the text
		remainingToWrite = encryptedLength;
        while (0 < remainingToWrite) 
		{
            int actuallyWritten = 0;
            actuallyWritten = [outputStream write:marker maxLength:remainingToWrite];
			
			status = [outputStream streamStatus];
			if (actuallyWritten == 0 ||
				status == NSStreamStatusError ||
				status == NSStreamStatusClosed ||
				status == NSStreamStatusNotOpen)
			{
				NSError *err = [outputStream streamError];
				[self showMessage:@"Error" theMessage:[NSString stringWithFormat:@"ERROR: %@", [err localizedFailureReason]]];
				free (tempBuffer);
				
				// stop spinning
				[NSThread detachNewThreadSelector:@selector(spinEnd) toTarget:self withObject:nil];
				
				return;
			}
			
            remainingToWrite -= actuallyWritten;
            marker += actuallyWritten;
        }
		free (tempBuffer);
		// encrypt until the end of message
		
		// stop spinning
		[NSThread detachNewThreadSelector:@selector(spinEnd) toTarget:self withObject:nil];		
	}
	
}

// ------------------ Steaming Events Handling -------------------------------
#pragma mark -
#pragma mark Stream Events Handling

/**
 * Handle steam events. Part of NSStreamDelegate Protocol
 * @param aStream incoming stream
 * @param streamEvent the stream event
 */
- (void)stream:(NSStream *)aStream handleEvent:(NSStreamEvent)streamEvent {
    NSInputStream * istream;
    switch(streamEvent) 
	{
        case NSStreamEventHasBytesAvailable:;
			
			UInt8 buffer[1024];
			
            istream = (NSInputStream *)aStream;
            if (!dataBuffer) 
			{
                dataBuffer = [[NSMutableData alloc] initWithCapacity:2048];
            }
			
			NSInteger actuallyRead = [istream read:buffer maxLength:1024];
			
			// make sure we're reading stuff
			if (actuallyRead < 0)
			{
				[self closeStreams];
				break;
			}
			
			[dataBuffer appendBytes:buffer length:actuallyRead];
			
			// see if we have enough to process, loop over messages in buffer
			while( YES ) 
			{
				
				// Did we read the header yet?
				if ( packetBodySize == -1 ) 
				{
					// Do we have enough bytes in the buffer to read the header?
					if ( [dataBuffer length] >= sizeof(int) ) {
						// extract length
						memcpy(&packetBodySize, [dataBuffer bytes], sizeof(int));
						packetBodySize = ntohl( packetBodySize );		// size is in network byte order
						
						// remove that chunk from buffer
						NSRange rangeToDelete = {0, sizeof(int)};
						[dataBuffer replaceBytesInRange:rangeToDelete withBytes:NULL length:0];
					}
					else {
						// We don't have enough yet. Will wait for more data.
						break;
					}
				}
				
				// We should now have the header. Time to extract the body.
				if ( [dataBuffer length] >= ((NSUInteger) packetBodySize) ) 
				{
					// We now have enough data to extract a meaningful packet.
					const int kPrologLength = 16;
					char *buffer = (char *)[dataBuffer bytes];
					
					// fetch the communication status
					unsigned long com_status = *((unsigned long *)(buffer + 0));
					com_status = ntohl( com_status );
					
					// decrypt the message
					size_t decryptedLength = (size_t) packetBodySize - 4;  // don't include com status
					
					int skip_message = 0;
					
					if (com_status == 0 && sCryptorRef)
					{
						PSCryptorStatus decryptResult = EncryptDecrypt (sCryptorRef, false, buffer+4, decryptedLength, buffer+4, decryptedLength, &decryptedLength);
						
						if (kCryptorSuccess != decryptResult)
						{
							// failed to decrypt.  Ingore messageg and disconnect
							skip_message = 1;
							[self showMessage:@"Warning" theMessage:@"The password was incorrect"];
						}
					}
					else 
					{
						if ( (com_status != 0) || (!sCryptorRef) )
						{
							[self showMessage:@"Warning" theMessage:@"The password was incorrect"];
						}
					}
					
					
					if (!skip_message)
					{
						// version, 32 bit unsigned int, network byte order
						unsigned long protocol_version = *((unsigned long *)(buffer + 4));
						protocol_version = ntohl( protocol_version );
						
						if (protocol_version != 1)
						{
							// either the message is corrupted or the protocol is newer.
							skip_message = 1;
						}
						
						if (!skip_message)
						{
							// transaction, 32 bit unsigned int, network byte order
							unsigned long transaction = *((unsigned long *)(buffer + 8));
							transaction = ntohl( transaction );
							
							// content type, 32 bit unsigned int, network byte order
							unsigned long content = *((unsigned long *)(buffer + 12));
							content = ntohl( content );
							
							unsigned char *received_data = (unsigned char *)(buffer+kPrologLength);
							int received_length = (decryptedLength-(kPrologLength-4));
							
							if (content == 3) // image data
							{
								// process image data
								// unsigned char image_type = *((unsigned char *)received_data);
							}
							else 
							{
								// Set the response string
								NSString * string = [[NSString alloc] initWithBytes:received_data  length:received_length encoding:NSUTF8StringEncoding ];								
								
								// some things requires something from PS, such as blob sending
								if ( [string hasPrefix:@"/private/var/folders"])
								{
									// mac path
									[self doPaintStep:string];
								}
								else if ([string hasPrefix:@"C:\\Users\\"])
								{
									// Windows path
									NSString *newStr = [[NSString alloc] initWithString: [string stringByReplacingOccurrencesOfString:@"\\" withString:@"/"]];
									[self doPaintStep:newStr];
									[newStr release];
								}
								
								
								[string release];
							}
						}
						
					}
					
					// Remove that chunk from buffer
					NSRange rangeToDelete = {0, packetBodySize};
					[dataBuffer replaceBytesInRange:rangeToDelete withBytes:NULL length:0];
					
					// We have processed the packet. Resetting the state.
					packetBodySize = -1;
				}
				else 
				{
					// Not enough data yet. Will wait.
					if ( ((NSUInteger) packetBodySize) < 0 )
					{
						[self showMessage:@"Warning" theMessage:[NSString stringWithFormat:@"Negative packet size of %d",  ((NSUInteger) packetBodySize)]];
					}					
					break;
				}
			}
			
            break;
        case NSStreamEventEndEncountered:;
            [self closeStreams];
			[self showMessage:@"Disconnect warning" theMessage:@"Photoshop has shutdown the connection"];
            break;
        case NSStreamEventHasSpaceAvailable:
        case NSStreamEventErrorOccurred:
        case NSStreamEventOpenCompleted:
        case NSStreamEventNone:
        default:
            break;
    }
}

/**
 * Opens both inputStream and outputSteam.
 */
- (void)openStreams {
    [inputStream retain];
    [outputStream retain];
    [inputStream setDelegate:self];
    [outputStream setDelegate:self];
    [inputStream scheduleInRunLoop:[NSRunLoop currentRunLoop] forMode:NSDefaultRunLoopMode];
    [outputStream scheduleInRunLoop:[NSRunLoop currentRunLoop] forMode:NSDefaultRunLoopMode];
    [inputStream open];
    [outputStream open];
	packetBodySize = -1;
	isConnected = YES;
	detailDescriptionLabel.text = @"Connected"; 
}

/**
 * Closes both inputStream and outputSteam.
 */
- (void)closeStreams {
    [inputStream close];
    [outputStream close];
    [inputStream removeFromRunLoop:[NSRunLoop currentRunLoop] forMode:NSDefaultRunLoopMode];
    [outputStream removeFromRunLoop:[NSRunLoop currentRunLoop] forMode:NSDefaultRunLoopMode];
    [inputStream setDelegate:nil];
    [outputStream setDelegate:nil];
    [inputStream release];
    [outputStream release];
    inputStream = nil;
    outputStream = nil;
	packetBodySize = -1;
	isConnected = NO;
	detailDescriptionLabel.text = @"Disconnected";
	if (sCryptorRef)
		DestroyPSCryptor(sCryptorRef);
	sCryptorRef = NULL;
}


#pragma mark -
#pragma mark View lifecycle


 // Implement viewDidLoad to do additional setup after loading the view, typically from a nib.
- (void)viewDidLoad {
    [super viewDidLoad];
	
	NSString *pathToHTML = [[NSBundle mainBundle] pathForResource:@"intro" ofType:@"html"];
	NSURL *url = [NSURL fileURLWithPath:pathToHTML isDirectory:NO];	
	[htmlView loadRequest:[NSURLRequest requestWithURL:url]];
	
	versionInfo.text = [NSString stringWithFormat:@"Version: %@", @kVersionInfo];
	
	currentSelection = 0;
	isConnected = NO;
	[self initSpinner];
}


/*
- (void)viewWillAppear:(BOOL)animated {
    [super viewWillAppear:animated];
}
*/
/*
- (void)viewDidAppear:(BOOL)animated {
    [super viewDidAppear:animated];
}
*/
/*
- (void)viewWillDisappear:(BOOL)animated {
    [super viewWillDisappear:animated];
}
*/
/*
- (void)viewDidDisappear:(BOOL)animated {
    [super viewDidDisappear:animated];
}
*/

- (void)viewDidUnload {
    // Release any retained subviews of the main view.
    // e.g. self.myOutlet = nil;
    self.popoverController = nil;
}


#pragma mark -
#pragma mark Memory management

/*
- (void)didReceiveMemoryWarning {
    // Releases the view if it doesn't have a superview.
    [super didReceiveMemoryWarning];
    
    // Release any cached data, images, etc that aren't in use.
}
*/

- (void)dealloc {
    [popoverController release];
    [toolbar release];
    
    [detailItem release];
    [detailDescriptionLabel release];
    [super dealloc];
}

@end
