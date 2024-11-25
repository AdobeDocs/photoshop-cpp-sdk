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
//  tutorialtestViewController.m
//  tutorialtest
//
//  @author Allen Jeng, Photoshop QE Developer, Adobe Systems Inc.
//  Copyright 2011 Adobe Systems Inc. All rights reserved.
//

#import "tutorialtestViewController.h"
#import "bonjourConnectViewController.h"

// encryption lib
#include "PSCryptorAPI.h"

// version
#define kVersionInfo "1.0.3"
#define kStep2Wait "STEP2WAIT"
#define kStep3Wait "STEP3WAIT"
#define kStep4Wait "STEP4WAIT"
#define kStepNone "NOWAIT"


// unique transaction id
static int transaction_id = 1;

// have a cryptor object ready for use
static PSCryptorRef sCryptorRef = NULL;


@implementation tutorialtestViewController
@synthesize popoverConnect;
@synthesize connectionStatusLabel;
@synthesize myVersion;
@synthesize busyView;
@synthesize outputMessage;

// --------- Misc -------------------
#pragma mark -
#pragma mark Misc


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
 *  message logging needs (send it to a UITextView)
 */ 
-(void)logMessage:(NSString *) msg clearLine:(BOOL)wantClearLine
{
	if (wantClearLine)
		outputMessage.text = [NSString stringWithFormat:@"[%@] %@\n", [self getCurrentTimeStamp], msg];
	else
		outputMessage.text = [NSString stringWithFormat:@"[%@] %@\n%@", [self getCurrentTimeStamp], msg, outputMessage.text];
}

/*
 *  returns current time stamp as NSString
 */ 
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

// ------------------ Tutorial Actions -------------------------------
#pragma mark -
#pragma mark Tutorial Actions

/*
 * the paint brush size JS according to desired brush size
 */
-(NSString *) paintBrushSize: (float)brushSize
{
	NSString *nsStr1 = [NSString stringWithUTF8String: "	var idsetd = charIDToTypeID( \"setd\" );\r"
						"var desc2 = new ActionDescriptor();\r"
						"var idnull = charIDToTypeID( \"null\" );\r"
						"var ref2 = new ActionReference();\r"
						"var idBrsh = charIDToTypeID( \"Brsh\" );\r"
						"var idOrdn = charIDToTypeID( \"Ordn\" );\r"
						"var idTrgt = charIDToTypeID( \"Trgt\" );\r"
						"ref2.putEnumerated( idBrsh, idOrdn, idTrgt );\r"
						"desc2.putReference( idnull, ref2 );\r"
						"var idT = charIDToTypeID( \"T   \" );\r"
						"var desc3 = new ActionDescriptor();\r"
						"var idmasterDiameter = stringIDToTypeID( \"masterDiameter\" );\r"
						"var idPxl = charIDToTypeID( \"#Pxl\" );\r"
						"desc3.putUnitDouble( idmasterDiameter, idPxl, "];
	
	NSString *nsStr2 = [NSString stringWithUTF8String:" );\r"
						"var idBrsh = charIDToTypeID( \"Brsh\" );\r"
						"desc2.putObject( idT, idBrsh, desc3 );\r"
						"executeAction( idsetd, desc2, DialogModes.NO );"];
	
	NSString *finalJS = [NSString stringWithFormat:@"%@%f%@", nsStr1, brushSize, nsStr2];
	return finalJS;
}

/*
 * JS for changing foreground color in PS
 */
-(NSString *) setColorJS:(int)red colorGreen:(int)green colorBlue:(int)blue
{
	NSString *mystr = [NSString stringWithUTF8String:"var theColor = new SolidColor();\r"
					   "theColor.rgb.red = "];
	
	NSString *finalStr = [NSString stringWithFormat:@"%@%@", mystr, [NSString stringWithFormat:@"%d",red]];
	
	mystr = [NSString stringWithUTF8String:";\r theColor.rgb.green = "];
	finalStr = [NSString stringWithFormat:@"%@%@%@", finalStr, mystr, [NSString stringWithFormat:@"%d",green]];
	
	mystr = [NSString stringWithUTF8String:";\r theColor.rgb.blue = "];
	finalStr = [NSString stringWithFormat:@"%@%@%@", finalStr, mystr, [NSString stringWithFormat:@"%d",blue]];
	
	mystr = [NSString stringWithUTF8String:";\r app.foregroundColor = theColor;"];
	finalStr = [NSString stringWithFormat:@"%@%@", finalStr, mystr];
	
	return finalStr;
}

/*
 * JS for setting selection brush size
 */
-(NSString *) setSelectionBrushSizeJS:(float) brushSize
{
	NSString *nsStr1 = [NSString stringWithUTF8String: "	var idsetd = charIDToTypeID( \"setd\" );\r"
						"var desc5 = new ActionDescriptor();\r"
						"var idnull = charIDToTypeID( \"null\" );\r"
						"var ref4 = new ActionReference();\r"
						"var idBrsh = charIDToTypeID( \"Brsh\" );\r"
						"var idOrdn = charIDToTypeID( \"Ordn\" );\r"
						"var idTrgt = charIDToTypeID( \"Trgt\" );\r"
						"ref4.putEnumerated( idBrsh, idOrdn, idTrgt );\r"
						"desc5.putReference( idnull, ref4 );\r"
						"var idT = charIDToTypeID( \"T   \" );\r"
						"var desc6 = new ActionDescriptor();\r"
						"var idDmtr = charIDToTypeID( \"Dmtr\" );\r"
						"var idPxl = charIDToTypeID( \"#Pxl\" );\r"
						"desc6.putUnitDouble( idDmtr, idPxl,"];
						
	NSString *nsStr2 = [NSString stringWithUTF8String:");\r"
						"var idHrdn = charIDToTypeID( \"Hrdn\" );\r"
						"var idPrc = charIDToTypeID( \"#Prc\" );\r"
						"desc6.putUnitDouble( idHrdn, idPrc, 100.0 );\r"
						"var idAngl = charIDToTypeID( \"Angl\" );\r"
						"var idAng = charIDToTypeID( \"#Ang\" );\r"
						"desc6.putUnitDouble( idAngl, idAng, 0 );\r"
						"var idRndn = charIDToTypeID( \"Rndn\" );\r"
						"var idPrc = charIDToTypeID( \"#Prc\" );\r"
						"desc6.putUnitDouble( idRndn, idPrc, 100.0 );\r"
						"var idSpcn = charIDToTypeID( \"Spcn\" );\r"
						"var idPrc = charIDToTypeID( \"#Prc\" );\r"
						"desc6.putUnitDouble( idSpcn, idPrc, 25.0 );\r"
						"var idIntr = charIDToTypeID( \"Intr\" );\r"
						"desc6.putBoolean( idIntr, true );\r"
						"var idflipX = stringIDToTypeID( \"flipX\" );\r"
						"desc6.putBoolean( idflipX, false );\r"
						"var idflipY = stringIDToTypeID( \"flipY\" );\r"
						"desc6.putBoolean( idflipY, false );\r"
						"var idcomputedBrush = stringIDToTypeID( \"computedBrush\" );\r"
						"desc5.putObject( idT, idcomputedBrush, desc6 );\r"
						"executeAction( idsetd, desc5, DialogModes.NO );"];
	
	NSString *finalJS = [NSString stringWithFormat:@"%@%f%@", nsStr1, brushSize, nsStr2];
	return finalJS;
}

/*
 * Action code run
 */
-(NSString *) playStrokeJS:(NSString *) path
{
	NSString *newStr;
	NSString *finalStr;
	
	newStr = [NSString stringWithUTF8String:"var descriptor = new ActionDescriptor();\r"
						"descriptor.putPath(stringIDToTypeID(\"path\"), new File(\""];

	finalStr = [NSString stringWithFormat:@"%@%@", newStr, path];

	newStr = [NSString stringWithUTF8String:"\") );\r"
									"descriptor.putBoolean(stringIDToTypeID(\"tool\"), true);\r"
									"var commandString = stringIDToTypeID(\"playbackStrokeFromFile\");\r"
									"executeAction(commandString, descriptor, DialogModes.NO);"];
	finalStr = [NSString stringWithFormat:@"%@%@", finalStr, newStr];
	
	return finalStr;
}

/*
 * choose the brush tool
 */
-(NSString *) useBrushTool
{
	NSString *brushToolJS = [NSString stringWithUTF8String:"var idslct = charIDToTypeID( \"slct\" );\r"
							 "var desc2 = new ActionDescriptor();\r"
							 "var idnull = charIDToTypeID( \"null\" );\r"
							 "var ref2 = new ActionReference();\r"
							 "var idPbTl = charIDToTypeID( \"PbTl\" );\r"
							 "ref2.putClass( idPbTl );\r"
							 "desc2.putReference( idnull, ref2 );\r"
							 "executeAction( idslct, desc2, DialogModes.NO );"];
	
	return brushToolJS;
}

/*
 * choose the quick select tool
 */
-(NSString *) useQuickSelectTool
{
	NSString *brushToolJS = [NSString stringWithUTF8String:"var idslct = charIDToTypeID( \"slct\" );\r"
							 "var desc2 = new ActionDescriptor();\r"
							 "var idnull = charIDToTypeID( \"null\" );\r"
							 "var ref2 = new ActionReference();\r"
							 "var idquickSelectTool = stringIDToTypeID( \"quickSelectTool\" );\r"
							 "ref2.putClass( idquickSelectTool );\r"
							 "desc2.putReference( idnull, ref2 );\r"
							 "executeAction( idslct, desc2, DialogModes.NO );"];
	
	return brushToolJS;
}



// =======================================================

/*
 * Step 1:  Create a new document of 7 x 5 inch
 */
-(IBAction) doStep1
{
	if (!isConnected)
	{
		[self showMessage:@"Info" theMessage:@"You're not connected to Photoshop!  Please connect to Photoshop before sending"];
		return;
	}
	
	[self logMessage:@"Creating a new doc of 7x5" clearLine:NO];
    NSString * stringToSend = [NSString stringWithUTF8String:"app.documents.add (UnitValue(7, \"inch\"), UnitValue(5, \"inch\"), 72);"];
    NSData * dataToSend = [stringToSend dataUsingEncoding:NSUTF8StringEncoding];
	[ self sendJavaScriptMessage: dataToSend ];	
}

/*
 * Step 2:  Draw with paint brush size 56 and paint with blue-ish color
 */
-(IBAction) doStep2
{
	NSString *stringToSend;
	NSData * dataToSend;

	if (!isConnected)
	{
		[self showMessage:@"Info" theMessage:@"You're not connected to Photoshop!  Please connect to Photoshop before sending"];
		return;
	}
	
	// set wait state for play all
	waitState = @kStep2Wait;
	
	// set the color
	[self logMessage:@"Set foreground color to blue-ish color" clearLine:NO];
	stringToSend = [[NSString alloc] initWithString:[self setColorJS:0 colorGreen:91 colorBlue:227]];
	dataToSend = [stringToSend dataUsingEncoding:NSUTF8StringEncoding];
	[ self sendJavaScriptMessage:dataToSend ];
	[stringToSend release];

	// make sure brush is selected!
	stringToSend = [[NSString alloc] initWithString:[self useBrushTool]];
	dataToSend = [stringToSend dataUsingEncoding:NSUTF8StringEncoding];
	[self sendJavaScriptMessage:dataToSend];
	[stringToSend release];
	
	// set brush size
	[self logMessage:@"Setting brush size to 56" clearLine:NO];
	stringToSend = [[NSString alloc] initWithString:[self paintBrushSize:30.0]];
	dataToSend = [stringToSend dataUsingEncoding:NSUTF8StringEncoding];
	[ self sendJavaScriptMessage:dataToSend ];
	[stringToSend release];
	
	// send the brush code
	[self logMessage:@"Sending brush painting to PS" clearLine:NO];
	NSString *blobObject = [[NSBundle mainBundle] pathForResource:@"stroke1" ofType:@""];
	NSData *blobData = [[NSData alloc] initWithContentsOfFile:blobObject];

	[self sendNetworkMessage:blobData type:5];
	[blobData release];
	
	// play the painting code - this happens later since we need PS to give us the path to the file
}


/*
 *  Step for doing the painting:  We need to wait for Photoshop to give us a URL to the blob object
 */
-(void) doPaintStep:(NSString *) path
{
	NSString *stringToSend = [self playStrokeJS:path];
	NSData * dataToSend = [stringToSend dataUsingEncoding:NSUTF8StringEncoding];
	[ self sendJavaScriptMessage:dataToSend ];

	[self logMessage:@"Play the painting code" clearLine:NO];
	
	if (playAllIsOn)
	{
		// if we're waiting for step to 2 to play the painting command, it just happened above, lets play step 3
		if ([waitState isEqualToString:@kStep2Wait])
		{
			[self doStep3];
		}
		// waiting for step 3
		else if ([waitState isEqualToString:@kStep3Wait])
		{
			[self doStep4];
		}
		else
		{
			// that means either all has played or we have an unknown state.
			// in either case, turn off play all
			playAllIsOn = NO;
			waitState = @kStepNone;
		}
	}
	else 
	{
		// if we're not playing all, clear the wait state
		waitState = @kStepNone;
	}

}


/*
 *  Step 3:  Draw again with paint brush size 70 with pink-ish color
 */
-(IBAction) doStep3
{
	NSString *stringToSend;
	NSData * dataToSend;

	if (!isConnected)
	{
		[self showMessage:@"Info" theMessage:@"You're not connected to Photoshop!  Please connect to Photoshop before sending"];
		return;
	}	
	
	// set the wait state for step 3 (need this for play all)
	waitState = @kStep3Wait;

	// set the color
	[self logMessage:@"Set foreground color to pink-ish color" clearLine:NO];
	stringToSend = [[NSString alloc] initWithString:[self setColorJS:219 colorGreen:135 colorBlue:217]];
	dataToSend = [stringToSend dataUsingEncoding:NSUTF8StringEncoding];
	[ self sendJavaScriptMessage:dataToSend ];
	[stringToSend release];

	// make sure brush is selected!
	stringToSend = [[NSString alloc] initWithString:[self useBrushTool]];
	dataToSend = [stringToSend dataUsingEncoding:NSUTF8StringEncoding];
	[self sendJavaScriptMessage:dataToSend];
	[stringToSend release];
	
	// set the brush size to 70
	[self logMessage:@"Setting the brush size to 70" clearLine:NO];
	stringToSend = [[NSString alloc] initWithString:[self paintBrushSize:60.0]];
	dataToSend = [stringToSend dataUsingEncoding:NSUTF8StringEncoding];
	[ self sendJavaScriptMessage:dataToSend ];
	[stringToSend release];

	// send the brush code
	[self logMessage:@"Sending painting code to PS" clearLine:NO];
	NSString *blobObject = [[NSBundle mainBundle] pathForResource:@"stroke2" ofType:@""];
	NSData *blobData = [[NSData alloc] initWithContentsOfFile:blobObject];
	
	[self sendNetworkMessage:blobData type:5];
	[blobData release];
	
	// play the painting code - this happens later since we need PS to give us the path to the file
}

/*
 *  Step 4:  Use quick selection tool to select a portion of the pink area
 */
-(IBAction) doStep4
{
	NSString *stringToSend;
	NSData *dataToSend;
	NSString *blobObject;
	NSData *blobData;

	if (!isConnected)
	{
		[self showMessage:@"Info" theMessage:@"You're not connected to Photoshop!  Please connect to Photoshop before sending"];
		return;
	}	
	
	// set wait state for play all
	waitState = @kStep4Wait;

	// make sure quick select tool is selected.
	stringToSend = [[NSString alloc] initWithString:[self useQuickSelectTool]];
	dataToSend = [stringToSend dataUsingEncoding:NSUTF8StringEncoding];
	[self sendJavaScriptMessage:dataToSend];
	[stringToSend release];
	
	// set the brush size to 25
	[self logMessage:@"Setting the selection brush size to 25" clearLine:NO];
	stringToSend = [[NSString alloc] initWithString:[self setSelectionBrushSizeJS:25.0]];
	dataToSend = [stringToSend dataUsingEncoding:NSUTF8StringEncoding];
	[ self sendJavaScriptMessage:dataToSend ];
	[stringToSend release];
	
	// send the brush code
	[self logMessage:@"Sending selection brush painting to PS" clearLine:NO];
	blobObject = [[NSBundle mainBundle] pathForResource:@"selection" ofType:@""];
	blobData = [[NSData alloc] initWithContentsOfFile:blobObject];
	
	[self sendNetworkMessage:blobData type:5];
	[blobData release];
	
	// play the painting code - this happens later since we need PS to give us the path to the file	
}

/*
 *  do 1 - 4
 */
-(IBAction) doAllSteps
{
	if (!isConnected)
	{
		[self showMessage:@"Info" theMessage:@"You're not connected to Photoshop!  Please connect to Photoshop before sending"];
		return;		
	}
	
	playAllIsOn = YES;
	[self doStep1];
	[self doStep2];
	
	// can't streamline everything since all normal code will get play first
	// painting code need to wait for an URL for the paint to get played.
	
	// the other 2 steps are handled in play script
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
			[self logMessage:[NSString stringWithFormat:@"ERROR: Did not succesful write to stream because: %@", [err localizedFailureReason]] clearLine:NO];
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
			[self logMessage:[NSString stringWithFormat:@"ERROR: %@", [err localizedFailureReason]] clearLine:NO];
			
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
									[self logMessage:@"Incoming data is JPG.  Ignoring." clearLine:NO];
								}
								else if (image_type == 2) // Pixmap
								{
									[self logMessage:@"Incoming data is Pixmap.  Ignoring." clearLine:NO];
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
				else {
					// Not enough data yet. Will wait.
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

/*
// The designated initializer. Override to perform setup that is required before the view is loaded.
- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil {
    self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil];
    if (self) {
        // Custom initialization
    }
    return self;
}
*/

/*
// Implement loadView to create a view hierarchy programmatically, without using a nib.
- (void)loadView {
}
*/



// Implement viewDidLoad to do additional setup after loading the view, typically from a nib.
- (void)viewDidLoad {
    [super viewDidLoad];
	
	isConnected = NO;
	myVersion.text = [NSString stringWithFormat:@"Version: %@", @kVersionInfo];
	[self initSpinner];
	waitState = @kStepNone;
	playAllIsOn = NO;
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


- (void)dealloc {
	[popoverConnect release];
    [super dealloc];
}

@end
