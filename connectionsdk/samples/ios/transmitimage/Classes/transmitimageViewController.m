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
//  transmitimageViewController.m
//  transmitimage
//
//  @author Allen Jeng, Photoshop QE Developer, Adobe Systems Inc.
//  Copyright 2011 Adobe Systems. All rights reserved.
//

#import "transmitimageViewController.h"
#import "bonjourConnectViewController.h"
#import "ImageHelper.h"

// encryption lib
#include "PSCryptorAPI.h"

// version
#define kVersionInfo "1.0.7"
#define kImageTypeJPEG "JPEG";
#define kImageTypePixmap "PixMap"; 

static int transaction_id = 1;

// have a cryptor object ready for use
static PSCryptorRef sCryptorRef = NULL;


@implementation transmitimageViewController

@synthesize popoverConnect, connectionStatusLabel, outputMessage, busyView;
@synthesize imageDisplay, imagePopover, imageType, imageSwitch, myVersion;


// --------- Misc -------------------
#pragma mark -
#pragma mark Misc


/*
 *  message logging needs (send it to a UITextView)
 */ 
-(void)logMessage:(NSString *) msg clearLine:(BOOL)wantClearLine
{
	if (wantClearLine)
		outputMessage.text = [NSString stringWithFormat:@"[%@] %@\n", [self getCurrentTimeStamp], msg];
	else
		outputMessage.text = [NSString stringWithFormat:@"[%@] %@\n%@", [self getCurrentTimeStamp], msg, outputMessage.text];
}

-(NSString *)getCurrentTimeStamp
{
	NSDate *todayDate = [NSDate date];
	NSDateFormatter *formattedDate = [[NSDateFormatter alloc] init];
	[formattedDate setDateFormat:@"yyyy/MM/dd hh:mma"];
	NSString *dateString = [formattedDate stringFromDate:todayDate];
	[formattedDate release];
	
	return dateString;
}

/*
 *  clear the output text
 */ 
-(IBAction) clearOutputButton
{
	if (isConnected)
		[self logMessage:@"Ready for input" clearLine:YES];
	else
		[self logMessage:@"Not connected" clearLine:YES];
}



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

// --------- image picking -------------------
#pragma mark -
#pragma mark Image Picking


/**
  * picks an image from available photos on iPad
  */
-(IBAction) pickImageButton
{
	// create an instance of image picker controller
	ipc = [[UIImagePickerController alloc] init];
	ipc.delegate = self;
	// set the picker to photo lib
	ipc.sourceType = UIImagePickerControllerSourceTypePhotoLibrary;
	
	imagePopover = [[UIPopoverController alloc] initWithContentViewController:ipc];
	[imagePopover setDelegate:self];

	// launch the picker
	[imagePopover presentPopoverFromRect:self.view.frame inView:self.view 
				permittedArrowDirections:UIPopoverArrowDirectionAny animated:YES];
	
}


/**
  *  so the user cancelled 
  */
-(void)imagePickerControllerDidCancel:(UIImagePickerController *)picker 
{
	// release the picker
	[self releaseImagePickerPopover];
}

/**
  * iPad UIPopoverContoller dissmiss or cancel (eg. user click outside of popover area
  */
-(void)popoverControllerDidDismissPopover:(UIPopoverController *)popoverController
{
	[self releaseImagePickerPopover];
}

/**
  *  do all the release code from one central location
  */
-(void)releaseImagePickerPopover 
{
	if (ipc)
	{
		[ipc dismissModalViewControllerAnimated:YES];
		[ipc.view removeFromSuperview];
		[ipc release];
		ipc = nil;
	}
	
	if (imagePopover)
	{
		[imagePopover dismissPopoverAnimated:YES];
		[imagePopover release];
		imagePopover = nil;
	}
}

/**
  * a photo was selected by the user
  */
-(void)imagePickerController:(UIImagePickerController *)picker 
didFinishPickingMediaWithInfo:(NSDictionary *)info 
{
	// set the image
	imageDisplay.image = [info objectForKey:UIImagePickerControllerOriginalImage];
	imageDisplay.backgroundColor = [UIColor clearColor];
	imageDisplay.opaque = NO;
	imageDisplay.contentMode = UIViewContentModeScaleAspectFill;	
	
	// release the complex popover
	[self releaseImagePickerPopover];
}

/**
 * a switch slider for picking the type of image
 */

-(IBAction) imageTypeSwitch:(id) sender
{
	// ON = JPEG, OFF = Pixmap
	if (imageSwitch.on)
	{
		imageType.text = @kImageTypeJPEG;
		isJPEG = YES;
	}
	else 
	{
		imageType.text = @kImageTypePixmap;
		isJPEG = NO;
	}

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
-(IBAction) connectButton:(id) sender
{
	if ([popoverConnect isPopoverVisible])
	{
		[popoverConnect dismissPopoverAnimated:YES];
	}
	else 
	{
		// popover button pressed!
		
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
			// bonjour connect view
			bonjourConnectViewController *bjc = [[bonjourConnectViewController alloc] init];
			bjc.navigationItem.title = @"Connections";		
			bjc.delegate = self;
			
			UINavigationController *navController =
				[[UINavigationController alloc] initWithRootViewController:bjc];
			
			popoverConnect = [[UIPopoverController alloc] initWithContentViewController:navController];
			popoverConnect.delegate = self;
			
			[bjc release];
			[navController release];
			
			// set the size of pop over
			popoverConnect.popoverContentSize = CGSizeMake(300, 300);
					
			[popoverConnect presentPopoverFromBarButtonItem:sender permittedArrowDirections:UIPopoverArrowDirectionAny animated:YES];
		}
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
		[self logMessage:@"No host/IP provided.  Not connected." clearLine:NO];
	}
	
	if (myPassword.length <= 0)
	{
		// didn't supply a password.
		isConnected = NO;
		okToConnect = NO;
		[self logMessage:@"No password supplied.  Not connected." clearLine:NO];
	}
	
	// if we're still connected, first disconnect
	if (inputStream && outputStream)
	{
        [self closeStreams];
		[self logMessage:@"Disconnected." clearLine:NO];
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
			
			if (isConnected)
				[self logMessage:[NSString stringWithFormat: @"Connected to %@", myServerName] clearLine:NO];
			else
				[self logMessage:[NSString stringWithFormat: @"Problem connecting to %@", myServerName] clearLine:NO];
		}
		else 
		{
			// connect with bonjour object
			if ([myService getInputStream:&inputStream outputStream:&outputStream])
			{
				[self openStreams];
				if (isConnected)
					[self logMessage:[NSString stringWithFormat: @"Connected to %@", myService.name] clearLine:NO];
				else
					[self logMessage:[NSString stringWithFormat: @"Problem connecting to %@", myService.name] clearLine:NO];
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
			[self logMessage:[NSString stringWithFormat:@"ERROR: Did not succesful write to stream because: %@", [err localizedFailureReason]] clearLine:NO];
			if ([[err localizedFailureReason] localizedCaseInsensitiveCompare:@"Operation timed out"] == NSOrderedSame)
				[self closeStreams];
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
			[self logMessage:[NSString stringWithFormat:@"ERROR: %@", [err localizedFailureReason]] clearLine:NO];
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
		else
			[self logMessage:@"PROBLEM: Unable to Encrypt the message because cryptor ref is NULL!" clearLine:NO];
		
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
				[self logMessage:[NSString stringWithFormat:@"ERROR: %@", [err localizedFailureReason]] clearLine:NO];
				free (tempBuffer);
				return;
			}
			
            remainingToWrite -= actuallyWritten;
            marker += actuallyWritten;
        }
		free (tempBuffer);
		// encrypt until the end of message
	}
}


/**
  * ask for JPEG or Pixmap thumbnail of frontmost image sized to fit in the image view
  * sized 728x510, RGB, 8 bit
  */
- (IBAction)getImageFromPhotoshop:(id)sender 
{
	NSString *stringToSend;
	NSString *infoMessage;

	if (isConnected)
	{
		if (isJPEG)
		{  // I use 728 x 510 b/c that's the size of my ImageView control
			if (UIDeviceOrientationIsPortrait(self.interfaceOrientation))
			{
				stringToSend = [NSString stringWithUTF8String:"var idNS = stringIDToTypeID( \"sendDocumentThumbnailToNetworkClient\" );\r"
								   "var desc1 = new ActionDescriptor();\r"
								   "desc1.putInteger( stringIDToTypeID( \"width\" ), 728 );\r"
								   "desc1.putInteger( stringIDToTypeID( \"height\" ), 510 );\r"
								   "desc1.putInteger( stringIDToTypeID( \"format\" ), 1 );\r"
								   "executeAction( idNS, desc1, DialogModes.NO );"  ];
			}
			else 
			{
				stringToSend = [NSString stringWithUTF8String:"var idNS = stringIDToTypeID( \"sendDocumentThumbnailToNetworkClient\" );\r"
								"var desc1 = new ActionDescriptor();\r"
								"desc1.putInteger( stringIDToTypeID( \"width\" ), 510 );\r"
								"desc1.putInteger( stringIDToTypeID( \"height\" ), 728 );\r"
								"desc1.putInteger( stringIDToTypeID( \"format\" ), 1 );\r"
								"executeAction( idNS, desc1, DialogModes.NO );"  ];				
			}

			infoMessage = [[NSString alloc] initWithString:@"Getting JPEG from Photoshop...\n"];
		}
		else 
		{
			if (UIDeviceOrientationIsPortrait(self.interfaceOrientation))
			{
				stringToSend = [NSString stringWithUTF8String:"var idNS = stringIDToTypeID( \"sendDocumentThumbnailToNetworkClient\" );"
									   "var desc1 = new ActionDescriptor();"
									   "desc1.putInteger( stringIDToTypeID( \"width\" ), 728 );"
									   "desc1.putInteger( stringIDToTypeID( \"height\" ), 510 );"
									   "desc1.putInteger( stringIDToTypeID( \"format\" ), 2 );"
									   "executeAction( idNS, desc1, DialogModes.NO );"  ];
			}
			else
			{
				stringToSend = [NSString stringWithUTF8String:"var idNS = stringIDToTypeID( \"sendDocumentThumbnailToNetworkClient\" );"
								"var desc1 = new ActionDescriptor();"
								"desc1.putInteger( stringIDToTypeID( \"width\" ), 510 );"
								"desc1.putInteger( stringIDToTypeID( \"height\" ), 728 );"
								"desc1.putInteger( stringIDToTypeID( \"format\" ), 2 );"
								"executeAction( idNS, desc1, DialogModes.NO );"  ];
			}
			infoMessage = [[NSString alloc] initWithString:@"Getting Pixmap from Photoshop...\n"];
		}

		NSData * dataToSend = [stringToSend dataUsingEncoding:NSUTF8StringEncoding];
		[ self sendJavaScriptMessage: dataToSend ];
	}
	else 
	{
		// nice try
		infoMessage = [[NSString alloc] initWithString:@"Try connecting to PS before sending\n"];
	}
	[self logMessage:infoMessage clearLine:NO];
	[infoMessage release];
}

/**
  *  Take whatever is in ImageView and send it to Photoshop
  */
-(IBAction) sendImageToPhotoshop:(id) sender
{
	// with large images, this could take some time!!
	[NSThread detachNewThreadSelector:@selector(spinBegin) toTarget:self withObject:nil];
	
	if (imageDisplay.image && isConnected)
	{
		if (isJPEG)
		{
			[self logMessage: [NSString stringWithFormat:@"INFO: Sending image of width = %1.2f, height = %1.2f to PS...", 
							   imageDisplay.image.size.width, imageDisplay.image.size.height] clearLine:NO]; 
			
			NSData *imageData = UIImageJPEGRepresentation(imageDisplay.image, 1.0);
			
			// need one byte before the image data for format type
			NSMutableData *image_message = [NSMutableData alloc];
			
			unsigned char format = 1; // JPEG
			[image_message appendBytes:(const void *)&format length:1];
			[image_message appendData:imageData];
			
			[self sendNetworkMessage:image_message type:3 ];  // type 3 = JPEG
			
			[image_message release];
		}
		else 
		{			

			[self logMessage: [NSString stringWithFormat:@"INFO: Sending pixmap of width = %1.2f, height = %1.2f to PS...", 
							   imageDisplay.image.size.width, imageDisplay.image.size.height] clearLine:NO]; 

			// first, translate UIImage to rawRGBA8
			unsigned char *buffer = [ImageHelper convertUIImageToBitmapRGBA8:imageDisplay.image];
			int width = imageDisplay.image.size.width;
			int height = imageDisplay.image.size.height;
			int planes = 3;
			
			// next, remove the extra plane;  from 4 to 3.  Note that Photoshop accepts both
			unsigned char *newBuffer = [ImageHelper convertBitmapRGBA8ToPixmapRGB:buffer withWidth:width withHeight:height];			
			NSMutableData *imageData = [[NSMutableData alloc] initWithBytes:(const void *)newBuffer length:(width * height * planes)];
			
			// construct the header
			NSMutableData *image_message = [NSMutableData alloc];
			
			unsigned char format = 2;		// Pixmap
			[ image_message appendBytes:(const void *)&format length:1 ];
			
			// 4 bytes uint32 width
			unsigned int temp = htonl( width );
			[ image_message appendBytes:(const void *)&temp length:4 ];
			
			// 4 bytes uint32 height
			temp = htonl( height );
			[ image_message appendBytes:(const void *)&temp length:4 ];
			
			// 4 bytes uint32 rowBytes
			temp = htonl( planes*width );
			[ image_message appendBytes:(const void *)&temp length:4 ];
			
			// 1 byte color mode = 1 = RGB
			unsigned char tempC = 1;
			[ image_message appendBytes:(const void *)&tempC length:1 ];
			
			// 1 byte channel count
			tempC = planes;
			[ image_message appendBytes:(const void *)&tempC length:1 ];
			
			// 1 byte bits per channel
			tempC = 8;
			[ image_message appendBytes:(const void *)&tempC length:1 ];
			
			// append the bits
			[ image_message appendData: imageData ];
			
			// send the message
			[ self sendNetworkMessage:image_message type:3 ];

			// cleanup
			[ image_message release ];
			[imageData release];
			
			if (buffer)
				free(buffer);
			if (newBuffer)
				free(newBuffer);
		}

	}
	else 
	{
		if (isConnected)
			[self logMessage:@"INFO: Select an image before hitting send!" clearLine:NO];
		else
			[self logMessage:@"INFO: Not sending, not connected to Photoshop." clearLine:NO];
	}

	[NSThread detachNewThreadSelector:@selector(spinEnd) toTarget:self withObject:nil];
}

/**
  *  this processes the received JPG data
  */
-(void)processReceivedJPEG: (NSData *)data
{
	// note that this would be NULL if the data isn't readable
	UIImage *incomingImage = [[UIImage alloc] initWithData:data];
	
	if (incomingImage)
	{
		imageDisplay.image = nil;
		imageDisplay.image = incomingImage;
		imageDisplay.backgroundColor = [UIColor clearColor];
		imageDisplay.opaque = NO;
		imageDisplay.contentMode = UIViewContentModeScaleAspectFit;
		
		
		[incomingImage release];
		[self logMessage:[NSString stringWithFormat:@"INFO: RECEIVING image of width = %1.2f, height = %1.2f from PS...", 
						   imageDisplay.image.size.width, imageDisplay.image.size.height] clearLine:NO];
		
	}
	else 
	{
		[self logMessage:@"WARNING: Incoming JPEG image not readable!" clearLine:NO];
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
	
	// with large data, this could take time...
	[NSThread detachNewThreadSelector:@selector(spinBegin) toTarget:self withObject:nil];
	
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
				[self logMessage:@"ERROR: Problem reading from steam!  Closing stream." clearLine:NO];
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
							[self logMessage:@"ERROR:  Decryption failed.  Wrong password." clearLine:NO];
						}
					}
					else 
					{
						if (com_status != 0)
							[self logMessage:@"ERROR:  Problem with communication, possible wrong password." clearLine:NO];
						
						if (!sCryptorRef)
							[self logMessage:@"ERROR:  Cryptor Ref is NULL, possible reason being that password was not supplied or password binding function failed." clearLine:NO];
					}
					
					
					if (!skip_message)
					{
						// version, 32 bit unsigned int, network byte order
						unsigned long protocol_version = *((unsigned long *)(buffer + 4));
						protocol_version = ntohl( protocol_version );
						
						if (protocol_version != 1)
						{
							// either the message is corrupted or the protocol is newer.
							[self logMessage:@"Incoming protocol version is different the expected. (or the message is corrupted.)  Not processing." clearLine:NO];
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
								unsigned char image_type = *((unsigned char *)received_data);								
								
								if (image_type == 1) // JPEG
								{
									int jpeg_length = received_length - 1;
									NSData *imageData = [NSData dataWithBytes:(received_data+1) length:jpeg_length];
									[self processReceivedJPEG:imageData];
								}
								else if (image_type == 2) // Pixmap
								{
									BOOL imageError = NO;
									
									int pixmap_prolog_length = 3*4 + 3 + 1;
									
									int width = *((unsigned long *)(received_data + 1));
									width = ntohl(width);
									
									int height = *((unsigned long *)(received_data + 5));
									height = ntohl( height );
									
									int rowBytes = *((unsigned long *)(received_data + 9));
									rowBytes = ntohl( rowBytes );
									
									int mode = *((unsigned char *)received_data + 13);	// must be 1 for now
									int channels = *((unsigned char *)received_data + 14);
									int bitsPerChannel = *((unsigned char *)received_data + 15);	// must be 8 for now
									
									// this is the original raw data from Photoshop.  It's in RGB format
									const unsigned char *temp_raw_data = received_data + pixmap_prolog_length;									
									int raw_length = received_length - pixmap_prolog_length;
									
									// need to convert it to an RGBA8 because iOS requires it.  If you try to use
									// plain RGB pixmap, the bind will probably return a NULL object
									unsigned char *raw_data = [ImageHelper padPixmapRGBToBitmapRGBA8:(unsigned char *)temp_raw_data withWidth:width withHeight:height];
									
									if (mode != 1 || bitsPerChannel != 8)
										imageError = YES;
									
									// 4k being a largish display size
									if (width < 0 || width > 4096
										|| height < 0 || height > 4096)
										imageError = YES;
									
									if (channels < 0 || channels > 4
										|| rowBytes < ((width*channels*bitsPerChannel)/8))
										imageError = YES;
									
									if (raw_length < (height*rowBytes))
										imageError = YES;
									
									
									if (!imageError)
									{
										/* *** EXTRA NOTE! ****  pixmap on iOS is very unstable - at least for me
										 * My first problem with it is that if it's not RGBA, iOS flat out rejects it with a NULL ptr
										 * Once the RGB raw pixmap is padded, it will display only ONCE.  At this point, the UIImage ptr
										 * inside UIImageView is whacked.  If you assign anything to it, it will blow up.
										 * unless you have a serious need for pixmap, try using JPG
										 */
										UIImage *imageToDisplay = [ImageHelper convertBitmapRGBA8ToUIImage:raw_data withWidth:width withHeight:height];
										
										// clear it
										imageDisplay.image = nil;										
										imageDisplay.image = imageToDisplay;
										imageDisplay.backgroundColor = [UIColor clearColor];
										imageDisplay.opaque = NO;
										imageDisplay.contentMode = UIViewContentModeScaleAspectFill;

										[imageToDisplay release];
										
										
										[self logMessage:[NSString stringWithFormat:@"INFO: RECEIVING pixmap of width = %1.2f, height = %1.2f from PS...", 
														  imageDisplay.image.size.width, imageDisplay.image.size.height] clearLine:NO];
										
										free(raw_data);
										
									}
									else
									{
										[self logMessage:@"Bad parameters for Pixmap!" clearLine:NO];
									}
								}
								else 
								{
									[self logMessage:@"Unknown image type." clearLine:NO];									
								}
								
							}
							else 
							{
								// Set the response string
								NSString * string = [[NSString alloc] initWithBytes:received_data  length:received_length encoding:NSUTF8StringEncoding ];
								[self logMessage:string clearLine:NO];
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
					
					// but double check that the packet size is not negative!
					if ( ((NSUInteger) packetBodySize) < 0)
						[self logMessage:[NSString stringWithFormat:@"WARNING!  Negative packet size of %d detected!", (NSUInteger) packetBodySize] clearLine:NO];
					break;
				}
			}
			
            break;
        case NSStreamEventEndEncountered:;
            [self closeStreams];
			[self logMessage:@"End encountered, closing stream." clearLine:NO];
            break;
        case NSStreamEventHasSpaceAvailable:
        case NSStreamEventErrorOccurred:
        case NSStreamEventOpenCompleted:
        case NSStreamEventNone:
        default:
            break;
    }
	
	[NSThread detachNewThreadSelector:@selector(spinEnd) toTarget:self withObject:nil];
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
	connectionStatusLabel.text = @"Connected";
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
	connectionStatusLabel.text = @"Disconnected";
	[self logMessage:@"Stream closed, disconnected from PS." clearLine:NO];
	if (sCryptorRef)
		DestroyPSCryptor(sCryptorRef);
	sCryptorRef = NULL;
}

// -----------------------------------------
#pragma mark -
#pragma mark Controller View Source

// Implement viewDidLoad to do additional setup after loading the view, typically from a nib.
- (void)viewDidLoad 
{
    [super viewDidLoad];
	
	[outputMessage setScrollEnabled:YES];
	isConnected = NO;
	isJPEG = YES;
	imageType.text = @kImageTypeJPEG;
	myVersion.text = [NSString stringWithFormat:@"Version: %@", @kVersionInfo];
	[self initSpinner];
}



// Override to allow orientations other than the default portrait orientation.
- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation {
    return YES;
}

- (void)didReceiveMemoryWarning {
	// Releases the view if it doesn't have a superview.
    [super didReceiveMemoryWarning];
	
	// Release any cached data, images, etc that aren't in use.
}

- (void)viewDidUnload {
	// Release any retained subviews of the main view.
	// e.g. self.myOutlet = nil;
}


- (void)dealloc 
{
	[popoverConnect release];
    [super dealloc];
}

@end
