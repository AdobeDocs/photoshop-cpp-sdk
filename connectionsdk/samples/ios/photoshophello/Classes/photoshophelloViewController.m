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
//  photoshophelloViewController.m
//  photoshophello
//
//  @author Allen Jeng, Photoshop QE Developer, Adobe Systems Inc.
//  Copyright 2010 Adobe Systems. All rights reserved.
//

#import "photoshophelloViewController.h"
#import "ConnectionViewController.h"

// encryption lib
#include "PSCryptorAPI.h"

// hard coded service name for Hello World test app
#define kNameOfService "_photoshopserver._tcp."
#define kVersionInfo "1.0.8"

static int transaction_id = 1;

// have a cryptor object ready for use
static PSCryptorRef sCryptorRef = NULL;


// subscribe to notifications for color changed events
static int foregroundColor_subscription = -1;
static int backgroundColor_subscription = -1;
static int foregroundColor_transaction = -1;
static int backgroundColor_transaction = -1;

@implementation photoshophelloViewController
@synthesize inputField, responseField, colorResponseField;
@synthesize connectStatus, versionLabel, foregroundColorBox, backgroundColorBox;

/**
  * button method for clearing the input field
  */
-(IBAction) clearInputButton
{
	inputField.text = @"";
}

/**
 * button method for clearing the output field
 */
-(IBAction) clearOutputButton
{
	if (isConnected)
	{
		responseField.text = @"Ready for input...\n";
	}
	else 
	{
		responseField.text = @"Not connected\n";
	}
	
}

/**
 * the button that send create new doc to Photoshop
 */
-(IBAction) createNewDocButton
{
	if (isConnected)
	{	
		NSString *stringToSend = [[NSString alloc] initWithFormat:@"app.documents.add(UnitValue(640, \"px\"),UnitValue(480, \"px\"),72); \n"];
		NSData *dataToSend = [stringToSend dataUsingEncoding:NSUTF8StringEncoding];
		inputField.text = stringToSend;
		[self sendJavaScriptMessage: dataToSend];
	
		[stringToSend release];
	}
	else 
	{
		responseField.text = [NSString stringWithFormat: @"%@ Not connected to any server.\n", responseField.text];
	}

}

/**
  * send what ever was typed in inputField to Photoshop
  */
-(IBAction) sendJSButton
{
	if (isConnected)
	{
		NSData *dataToSend = [inputField.text dataUsingEncoding:NSUTF8StringEncoding];
		[self sendJavaScriptMessage: dataToSend];
	}
	else 
	{
		responseField.text = [NSString stringWithFormat: @"%@ Not connected to any server.\n", responseField.text];	
	}
	
	[inputField resignFirstResponder];
}


/**
  * disconnect from server button method; it calls closeStream
  */
-(IBAction) disconnectFromServerButton
{
	if (inputStream && outputStream)
	{
        [self closeStreams];
		responseField.text = [NSString stringWithFormat: @"%@ \nDisconnect Successful.\n", responseField.text];
		connectStatus.text = @"Connection Status:  Disconnected";
    }
	else 
	{
		responseField.text = [NSString stringWithFormat: @"%@ Already Disconnected.\n", responseField.text];
	}
	
	
}

/*
 *  test to see if server is still alive, get a connection string back
 */
-(IBAction) testConnectionAliveButton: (id) sender
{
	[self sendNetworkMessage:nil type:6];
}


/**
  * connection to server button;  it calls ConnectionViewController
  * @param sender it doesn't care about data passed in.
  */
-(IBAction) connectButtonTapped:(id) sender {
	// I want this to be the delegate for connect view
	ConnectionViewController *cvc = [[ConnectionViewController alloc] init];
	cvc.delegate = self;
	
	// we're use navigation controller
	UINavigationController *nc = [[UINavigationController alloc] initWithRootViewController:cvc];
	
	// before launch, lets set it to NOT take over the entire screen...
	nc.modalPresentationStyle = UIModalPresentationFormSheet;
	
	// launch the connection modal view
	[self presentModalViewController:nc animated:YES];
	
	// done. release them
	[cvc release];
	[nc release];
}

/**
  * Done button pressed.  Receive data from ConnectionViewController when Done button is pressed
  * @param values Array of values passed in. 0: server's broadcast name, 1: server IP or name, 2: password, 3: NSNetService object
  */
-(void) doneButtonPressed:(NSArray *) values
{
	// done with the view, make it go away
	[self dismissModalViewControllerAnimated:YES];
	
	// we will assume connected unless otherwise told.
	isConnected = YES;
	
	/* values passed to us:
	 0:  server's broadcast name
	 1:  server's real name
	 2:  the password
	 3:  NSNetService object
	 */
	NSString *broadcastName = (NSString *)[values objectAtIndex:0];
	NSString *ipName = (NSString *)[values objectAtIndex:1];
	NSString *passwd = (NSString *)[values objectAtIndex:2];
	
	if ( (broadcastName.length <= 0) ||
		 (passwd.length <= 0) )
	{
		connectStatus.text = @"Connection:  Not connected";
		isConnected = NO;
	}
	
	// if we're connected to nothing, lets also close off 
	if (inputStream && outputStream)
	{
        [self closeStreams];
		responseField.text = [NSString stringWithFormat: @"%@ Disconnected.\n", responseField.text];
    }
	
	// the service
	if (isConnected)
	{
		// create the cryptor object based on password
		if (sCryptorRef)
			DestroyPSCryptor(sCryptorRef);
		
		sCryptorRef = CreatePSCryptor([passwd UTF8String]);
		

		// if it's user defined, use the IP connect method
		if ([broadcastName isEqualToString:@kUserDefinedService])
		{
			// if manual ip connection is desired, use CFStreamCratePairWithSocketToHost();
			// then cast it to NSInputSteam and NSOutputStream
			CFReadStreamRef readStream;
			CFWriteStreamRef writeStream;
			CFStreamCreatePairWithSocketToHost(kCFAllocatorDefault, (CFStringRef)ipName  , 49494, &readStream, &writeStream);
		    inputStream = (NSInputStream *) readStream;
			outputStream = (NSOutputStream *) writeStream;

			[self openStreams:ipName];
			[self subscribeColorChanges];
			
			if (isConnected)
				responseField.text = [NSString stringWithFormat: @"%@ Connected to %@ : Ready for input...\n", responseField.text, ipName];				
			else 
				responseField.text = [NSString stringWithFormat: @"%@ ERROR:  Problem connecting to %@\n", responseField.text, ipName];								
		}
		else
		{
			NSNetService *selectedService = [values objectAtIndex:3];
			if ([selectedService getInputStream:&inputStream outputStream:&outputStream])
			{
				[self openStreams:selectedService.name];
				[self subscribeColorChanges];
			}
			
			if (isConnected)					
				responseField.text = [NSString stringWithFormat: @"%@ Connected to %@ : Ready for input...\n", responseField.text, selectedService.name];
			else
				responseField.text = [NSString stringWithFormat: @"%@ ERROR:  Problem connecting to %@!  Firewall could be active...\n", responseField.text, selectedService.name];																
		}

	}	
}


/**
 * Initialize subscription to color changes with Photoshop
 */
- (void)subscribeColorChanges 
{
	// subscribe to foreground changes
	{
		NSString * stringToSend = [NSString stringWithUTF8String:"var idNS = stringIDToTypeID( \"networkEventSubscribe\" );"
								   "var desc1 = new ActionDescriptor();"
								   "desc1.putClass( stringIDToTypeID( \"eventIDAttr\" ), stringIDToTypeID( \"foregroundColorChanged\" ) );"
								   "executeAction( idNS, desc1, DialogModes.NO );"  ];
		NSData * dataToSend = [stringToSend dataUsingEncoding:NSUTF8StringEncoding];
		[ self sendJavaScriptMessage: dataToSend ];
		foregroundColor_subscription = transaction_id - 1;
	}
	
	// subscribe to background changes
	{
		NSString * stringToSend2 = [NSString stringWithUTF8String:"var idNS = stringIDToTypeID( \"networkEventSubscribe\" );"
									"var desc1 = new ActionDescriptor();"
									"desc1.putClass( stringIDToTypeID( \"eventIDAttr\" ), stringIDToTypeID( \"backgroundColorChanged\" ) );"
									"executeAction( idNS, desc1, DialogModes.NO );"  ];
		NSData * dataToSend2 = [stringToSend2 dataUsingEncoding:NSUTF8StringEncoding];
		[ self sendJavaScriptMessage: dataToSend2 ];
		backgroundColor_subscription = transaction_id - 1;
	}
	
	// get initial foreground
	{
		NSString * stringToSend = [NSString stringWithUTF8String:"\"ignored\\r\"+app.foregroundColor.rgb.hexValue.toString();"];
		NSData * dataToSend = [stringToSend dataUsingEncoding:NSUTF8StringEncoding];
		[ self sendJavaScriptMessage: dataToSend ];
		foregroundColor_transaction = transaction_id - 1;
	}
	
	// get initial background
	{
		NSString * stringToSend = [NSString stringWithUTF8String:"\"ignored\\r\"+app.backgroundColor.rgb.hexValue.toString();"];
		NSData * dataToSend = [stringToSend dataUsingEncoding:NSUTF8StringEncoding];
		[ self sendJavaScriptMessage: dataToSend ];
		backgroundColor_transaction = transaction_id - 1;
	}
	
}


/**
  *  takes a hex color and translate it into a UIColor object
  *  @param hexColor the hex color
  *  @return the translated color object
  */
static UIColor * ColorFromString (NSString *hexColor )
{	
	if ( [hexColor length] < 6)
		return NULL;
	
	long color_value = strtol( [hexColor UTF8String], (char **)NULL, 16 );
	
	unsigned char red = (color_value >> 16) & 0xFF;
	unsigned char green = (color_value >> 8) & 0xFF;
	unsigned char blue = color_value & 0xFF;
	
	UIColor *theColor = [ UIColor colorWithRed:(red/255.0f) green:(green/255.0f) blue:(blue/255.0f) alpha:1.0 ];
	
	return theColor;
}

/**
  *  parse the string returned, of the style: "toolChanged\rsomeTool" find first return, take substring from that index
  *  @param string incoming string with color from Photoshop
  *  @return NSColor representation of the color
  */
static UIColor * ColorFromResponseString( NSString *string )
{
	NSRange range = [string rangeOfString:@"\r" ];
	if (range.length == 0)
		return NULL;
	
	NSString *hexColor = [string substringFromIndex: (range.location + 1) ];
	
	return ColorFromString( hexColor );
}

/**
  *  change the foreground color of UIView foregroundColorBox
  */
- (void)processForegroundChange: (NSString *)string 
{	
	UIColor *theColor = ColorFromResponseString( string );
	
	if (theColor != NULL)
	{
		[UIView beginAnimations:nil context:nil];
		foregroundColorBox.backgroundColor = theColor;
		[UIView commitAnimations];
	}
}

/**
 *  change the background color of UIView backgroundColorBox
 */
- (void)processBackgroundChange: (NSString *)string 
{	
	UIColor *theColor = ColorFromResponseString( string );
	
	if (theColor != NULL)
	{
		[UIView beginAnimations:nil context:nil];
		backgroundColorBox.backgroundColor = theColor;
		[UIView commitAnimations];
	}
}



/**
 * The code that send javascript to stream
 * @param dataToSend a UTF8 encoded string
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
        int remainingToWrite = 0;
		
		if (dataToSend != nil)
			remainingToWrite = [dataToSend length];
		
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
		
		// check to see if we're able to write to stream
		if (bytesWritten != 4)
		{
			NSError *err = [outputStream streamError];
			responseField.text = [NSString stringWithFormat:@"%@\nERROR: Did not successful write to stream because: %@ \n", responseField.text, [err localizedFailureReason] ];
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
			 status == NSStreamStatusNotOpen )
		{
			NSError *err = [outputStream streamError];
			responseField.text = [NSString stringWithFormat:@"%@ \n ERROR: %@ \n", responseField.text, [err localizedFailureReason] ];
			if ([[err localizedFailureReason] localizedCaseInsensitiveCompare:@"Operation timed out"] == NSOrderedSame)
				[self closeStreams];
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
		if (dataToSend != nil)
		{
			uint8_t * dataBuf = (uint8_t *)[dataToSend bytes];
			memcpy (tempBuffer+12, dataBuf, remainingToWrite);
		}
			
		uint8_t * marker = (uint8_t *) tempBuffer;
			
		// now encrypt the message packet
		if (sCryptorRef)
			EncryptDecrypt (sCryptorRef, true, tempBuffer, plainTextLength, tempBuffer, encryptedLength, &encryptedLength);
		else 
			responseField.text = [NSString stringWithFormat:@"%@ \n PROBLEM: Unable to Encrypt the message because cryptor ref is NULL !\n", responseField.text ];
		
		// write the text
		remainingToWrite = encryptedLength;
        while (0 < remainingToWrite) {
            int actuallyWritten = 0;
            actuallyWritten = [outputStream write:marker maxLength:remainingToWrite];
			
			// make sure we wrote to stream successfully
			status = [outputStream streamStatus];
			if (actuallyWritten == 0 ||
				status == NSStreamStatusError ||
				status == NSStreamStatusClosed ||
				status == NSStreamStatusNotOpen)
			{
				NSError *err = [outputStream streamError];
				responseField.text = [NSString stringWithFormat:@"%@ \n ERROR: %@ \n", responseField.text, [err localizedFailureReason] ];
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
			unsigned int actuallyRead = 0;
			
            istream = (NSInputStream *)aStream;
            if (!dataBuffer) 
			{
                dataBuffer = [[NSMutableData alloc] initWithCapacity:2048];
            }
			
			actuallyRead = [istream read:buffer maxLength:1024];
			
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
					
					// if incoming message is color change, then don't display message
					BOOL needOutput = YES;
					
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
							responseField.text = [NSString stringWithFormat:@"%@ \n ERROR:  Decryption failed.  Wrong password. \n", responseField.text];
						}
					}
					else 
					{
						if (com_status != 0)
							responseField.text = [NSString stringWithFormat:@"%@ \n ERROR:  Problem with communication, possible wrong password.\n", responseField.text];
						
						if (!sCryptorRef)
							responseField.text = [NSString stringWithFormat:@"%@ \n ERROR:  Cryptor Ref is NULL, possible reason being that password was not supplied or password binding function failed.\n", responseField.text];
					}

					// Interpret encrypted section
					if (!skip_message)
					{
						// version, 32 bit unsigned int, network byte order
						unsigned long protocol_version = *((unsigned long *)(buffer + 4));
						protocol_version = ntohl( protocol_version );
						
						if (protocol_version != 1)
						{
							// either the message is corrupted or the protocol is newer.
							responseField.text = [NSString stringWithFormat:@"%@ \n Incoming protocol version is different the expected. (or the message is corrupted.)  Not processing.\n", responseField.text];
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
								responseField.text = [NSString stringWithFormat: @"%@ \n Incoming data is IMAGE.  Skipping\n", responseField.text];
								
								if (image_type == 1) // JPEG
								{
									responseField.text = [NSString stringWithFormat: @"%@ \n By the way, incoming image is JPEG\n", responseField.text];
								}
								else if (image_type == 2) // Pixmap
								{
									responseField.text = [NSString stringWithFormat: @"%@ \n By the way, incoming image is Pixmap\n", responseField.text];
								}
								else 
								{
									responseField.text = [NSString stringWithFormat: @"%@ \n Unknown image type\n", responseField.text];
								}
														  
							}
							else 
							{
								// Set the response string
								NSString * string = [[NSString alloc] initWithBytes:received_data  length:received_length encoding:NSUTF8StringEncoding ];
								
								// see if this is a response we're looking for
								if (content != 1)
								{
									if (transaction == foregroundColor_subscription
										|| transaction == foregroundColor_transaction)
									{
										[self processForegroundChange:string];
										needOutput = NO;
										colorResponseField.text = string;
									}
									if (transaction == backgroundColor_subscription
										|| transaction == backgroundColor_transaction)
									{
										[self processBackgroundChange:string];
										needOutput = NO;
										colorResponseField.text = string;
									}
								}
								
								if (needOutput)
									responseField.text = [NSString stringWithFormat: @"%@ \n %@ \n", responseField.text, string ]; 
								
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
				else {
					// Not enough data yet. Will wait.
					break;
				}
			}
			
            break;
        case NSStreamEventEndEncountered:;
            [self closeStreams];
			responseField.text = [NSString stringWithFormat: @"%@ End encountered, closing stream.\n", responseField.text];
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
- (void)openStreams: (NSString *)nameOfService {
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
	connectStatus.text = [NSString stringWithFormat:@"Connected to %@", nameOfService];

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
	connectStatus.text = @"Connection Status:  DISCONNECTED";
	if (sCryptorRef)
		DestroyPSCryptor(sCryptorRef);
	sCryptorRef = NULL;
}


/**
  * Setup additional items that needs initializing
  */
- (void)viewDidLoad {
    [super viewDidLoad];

	[responseField setScrollEnabled:YES];
	isConnected = NO;
	versionLabel.text = [NSString stringWithFormat:@"Version %@", @kVersionInfo];
}



/** 
  * Override to allow orientations other than the default portrait orientation.
  */
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

/**
  * deallocate misc items and close the stream if they're open
  */
- (void)dealloc {
    [super dealloc];

	if (inputStream && outputStream)
	{
        [self closeStreams];
		responseField.text = [NSString stringWithFormat: @"%@ \nDisconnected.\n", responseField.text];
    }
	
	[responseField release];
	[dataBuffer release];
}

@end
