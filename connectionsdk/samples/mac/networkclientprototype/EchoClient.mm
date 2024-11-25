/*
 File: EchoClientAppDelegate.m
 
 Abstract: Implementation of the GUI CocoaEcho client.
 
 Disclaimer: IMPORTANT:  This Apple software is supplied to you by Apple
 Computer, Inc. ("Apple") in consideration of your agreement to the
 following terms, and your use, installation, modification or
 redistribution of this Apple software constitutes acceptance of these
 terms.  If you do not agree with these terms, please do not use,
 install, modify or redistribute this Apple software.
 
 In consideration of your agreement to abide by the following terms, and
 subject to these terms, Apple grants you a personal, non-exclusive
 license, under Apple's copyrights in this original Apple software (the
 "Apple Software"), to use, reproduce, modify and redistribute the Apple
 Software, with or without modifications, in source and/or binary forms;
 provided that if you redistribute the Apple Software in its entirety and
 without modifications, you must retain this notice and the following
 text and disclaimers in all such redistributions of the Apple Software. 
 Neither the name, trademarks, service marks or logos of Apple Computer,
 Inc. may be used to endorse or promote products derived from the Apple
 Software without specific prior written permission from Apple.  Except
 as expressly stated in this notice, no other rights or licenses, express
 or implied, are granted by Apple herein, including but not limited to
 any patent rights that may be infringed by your derivative works or by
 other works in which the Apple Software may be incorporated.
 
 The Apple Software is provided by Apple on an "AS IS" basis.  APPLE
 MAKES NO WARRANTIES, EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION
 THE IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY AND FITNESS
 FOR A PARTICULAR PURPOSE, REGARDING THE APPLE SOFTWARE OR ITS USE AND
 OPERATION ALONE OR IN COMBINATION WITH YOUR PRODUCTS.
 
 IN NO EVENT SHALL APPLE BE LIABLE FOR ANY SPECIAL, INDIRECT, INCIDENTAL
 OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 INTERRUPTION) ARISING IN ANY WAY OUT OF THE USE, REPRODUCTION,
 MODIFICATION AND/OR DISTRIBUTION OF THE APPLE SOFTWARE, HOWEVER CAUSED
 AND WHETHER UNDER THEORY OF CONTRACT, TORT (INCLUDING NEGLIGENCE),
 STRICT LIABILITY OR OTHERWISE, EVEN IF APPLE HAS BEEN ADVISED OF THE
 POSSIBILITY OF SUCH DAMAGE.
 
 Copyright © 2005 Apple Computer, Inc., All Rights Reserved
 */

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

#include "PSCryptor.h"
#include <map>
#include <set>
#include <vector>
#include <stdio.h>
#include <os/log.h>

// ccox - hard coded service name for this test app
#define kNameOfService	"_photoshopserver._tcp."

#import "EchoClient.h"

#define qEncryption	1

//EchoClient singleton
static EchoClient* sEchoClient = nil;
static NSString* sTestJS = nil;

// ---------------------------------------------------------------------------

@implementation EchoClient

@synthesize delegate;
@synthesize password;
@synthesize logFilePath;
@synthesize imageFilePath;
@synthesize preferredServer;

// ---------------------------------------------------------------------------

static int sRunningTest = 0;
void StartTestMode() { ++sRunningTest; }
void EndTestMode() { --sRunningTest; }

// ---------------------------------------------------------------------------
//make odd transactions in test mode, even in non-test mode
//then the response handlers can decide what to do with the output
//(echo client doesn't print test transactions to the response window)

static transaction_id sTransactionID = 1; //next transaction id to use

bool IsTestTransaction(transaction_id inTransaction)
    {
    return inTransaction & 1;
    }

transaction_id GetNextTransactionID()
    {
    if (sTransactionID == 0)
        sTransactionID = 1; //leave 0 as the invalid id
        
    if (sRunningTest)
        return (sTransactionID + 1) | 1;
    else
        return ((sTransactionID + 2) | 1) - 1;
    }

transaction_id GetLastTransactionID()
    {
    return sTransactionID; //last one sent
    }


transaction_id IncrementTransactionID()
    {
    sTransactionID = GetNextTransactionID();
    return sTransactionID; //next transaction id to use
    }

// ---------------------------------------------------------------------------

+(EchoClient*)get {
		
	if (sEchoClient == NULL)
        sEchoClient = [[EchoClient alloc] init];
     
    return sEchoClient;
}

// ---------------------------------------------------------------------------

-(NSString*) makeLogFilePath: (NSString*)logFileName
{
    NSFileManager *fileManager= [NSFileManager defaultManager];
    BOOL isDir;
        
    if(![fileManager fileExistsAtPath:[self logFilePath] isDirectory:&isDir])
        if(![fileManager createDirectoryAtPath:[self logFilePath] withIntermediateDirectories:YES attributes:nil error:NULL])
            NSLog(@"Error: Create folder failed %@", [self logFilePath]);
    
    return [[[self logFilePath] stringByAppendingString:@"/"] stringByAppendingString: logFileName];
}

// ---------------------------------------------------------------------------

-(NSNetService*)getConnectedService:(double)waitSeconds {

	while (connectedService == nil && waitSeconds > 0.0)
		{
		//run loop to give NSNetServiceBrowser a chance to see new servers
		[[NSRunLoop currentRunLoop] runUntilDate: [[NSDate date] dateByAddingTimeInterval: .1] ];
		waitSeconds -= .1;
		}

	return connectedService;
}

// ---------------------------------------------------------------------------

-(transaction_id)subscribeEvent:(const char*)eventStr {
    
    return [ self sendJavaScript: [NSString stringWithFormat:
                @"var idNS = stringIDToTypeID( \"networkEventSubscribe\" );"
                "var desc1 = new ActionDescriptor();"
                "desc1.putClass( stringIDToTypeID( \"eventIDAttr\" ), stringIDToTypeID( \"%s\" ) );"
                "executeAction( idNS, desc1, DialogModes.NO );",
                eventStr]
                ];
    
}

// ---------------------------------------------------------------------------

-(transaction_id)subscribeEvent:(const char*)eventStr withVersion:(const char*)versionStr {

    return [ self sendJavaScript: [NSString stringWithFormat:
                @"var idNS = stringIDToTypeID( \"networkEventSubscribe\" );"
                "var desc1 = new ActionDescriptor();"
                "desc1.putClass( stringIDToTypeID( \"eventIDAttr\" ), stringIDToTypeID( \"%s\" ) );"
                "desc1.putString( stringIDToTypeID( \"version\" ), \"%s\" );"
                "executeAction( idNS, desc1, DialogModes.NO );",
                eventStr,
                versionStr]
            ];
}

// ---------------------------------------------------------------------------

-(transaction_id)unsubscribeEvent:(const char*)eventStr {

    return [ self sendJavaScript: [NSString stringWithFormat:
                @"var idNS = stringIDToTypeID( \"networkEventUnsubscribe\" );"
                "var desc1 = new ActionDescriptor();"
                "desc1.putClass( stringIDToTypeID( \"eventIDAttr\" ), stringIDToTypeID( \"%s\" ) );"
                "executeAction( idNS, desc1, DialogModes.NO );",
                eventStr]
                ];
}

// ---------------------------------------------------------------------------

-(transaction_id) sendJavaScript: (NSString*)inJSString withCatch:(BOOL)inWithCatch {
    
    NSString* stringToSend =  inWithCatch ?
            [NSString stringWithFormat: @"try{%@;}catch(e){e;}", inJSString ] :
            inJSString;
    
    transaction_id transid = [self sendNetworkMessage:[stringToSend dataUsingEncoding:NSUTF8StringEncoding] type:kPSNetJavaScript ];

    LogInfo("%u javaScript sent:\n%@", transid, inJSString);
    
    return transid;
}

// ---------------------------------------------------------------------------

-(transaction_id) sendJavaScript: (NSString*)inJSString {
    
    return [self sendJavaScript: inJSString withCatch: YES];
}

// ---------------------------------------------------------------------------

- (transaction_id)sendDataMessage:(NSData*)dataToSend {


	transaction_id transid =  [self sendNetworkMessage:dataToSend type:kPSNetDataBlob ];

   // LogInfo("%u data sent:\n%@", transid, [dataToSend description]);
    LogInfo("%u data sent", transid);

    return transid;
}

// ---------------------------------------------------------------------------

- (transaction_id)sendCancelCommandMessage:(transaction_id)canceltransid{

	int32_t swabbed_temp = htonl( canceltransid);

    [NSData dataWithBytes:&swabbed_temp length:(NSUInteger)sizeof(int32_t)];


	transaction_id transid =  [self sendNetworkMessage:
                                [NSData dataWithBytes:&swabbed_temp length:(NSUInteger)sizeof(int32_t)]
                                type:kPSNetCancelCommand ];

   // LogInfo("%u data sent:\n%@", transid, [dataToSend description]);
    LogInfo("%u cancel sent", transid);

    return transid;
}

// ---------------------------------------------------------------------------

- (transaction_id)getCurrentEventStatus {

	transaction_id transid =  [self sendNetworkMessage: nil
                                type:kPSNetEventStatus ];

   // LogInfo("%u data sent:\n%@", transid, [dataToSend description]);
    LogInfo("%u event status sent", transid);

    return transid;
}

// ---------------------------------------------------------------------------

- (transaction_id)sendImageMessage:(NSData*)dataToSend  {

	transaction_id transid =  [self sendNetworkMessage:dataToSend type:kPSNetImage ];

 //  LogInfo("%u image sent:\n%@", transid,  [dataToSend description]);
   LogInfo("%u image sent", transid);

    return transid;
}

// ---------------------------------------------------------------------------


- (transaction_id)sendNetworkMessage:(NSData*)dataToSend type:(PSNetworkContentTypes)dataType {


    if (outputStream) {
		size_t prolog_length = 12;	// 3x 32 bit integers, not counting the length value itself
		// com status is outside the encrypted section
		
		size_t plainTextLength = prolog_length + (dataToSend ? [dataToSend length] : 0);
		size_t encryptedLength = plainTextLength;
		NSError *err = NULL;
		
#if qEncryption
		encryptedLength = PSCryptor::GetEncryptedLength (plainTextLength);
#endif

// the length is NOT encrypted
		// write length of message as 32 bit signed int
		// includes all bytes after the length
		int32_t swabbed_temp = htonl( encryptedLength + 4 );
		NSInteger written = [outputStream write:(const uint8_t*)&swabbed_temp maxLength:4];
		if (written != 4)
			{
			err = [outputStream streamError];	// just here for debugging
			NSBeep();
			return 0;
			}
		
		NSStreamStatus status = [outputStream streamStatus];
		if ( status == NSStreamStatusError
			|| status == NSStreamStatusClosed
			|| status == NSStreamStatusNotOpen)
			{
			err = [outputStream streamError];	// just here for debugging
			NSBeep();
			return 0;
			}

        // the communication status is NOT encrypted
		// write communication status value as 32 bit unsigned int
		swabbed_temp = htonl( 0 );
		[outputStream write:(const uint8_t*)&swabbed_temp maxLength:4];


        // this part should be encrypted, until the end of the message
		
		char *tempBuffer = (char *) malloc (encryptedLength);
		
		// protocol version, 32 bit unsigned integer
		swabbed_temp = htonl( 1 );
		memcpy (tempBuffer+0, (const void *) &swabbed_temp, 4);
		
		// transaction id, 32 bit unsigned integer
		swabbed_temp = htonl( IncrementTransactionID() );
		memcpy (tempBuffer+4,(const void *) &swabbed_temp, 4);
		
		// content type, 32 bit unsigned integer
		swabbed_temp = htonl( dataType );
		memcpy (tempBuffer+8, (const void *) &swabbed_temp, 4);
		
		// and the data passed in
        if (dataToSend)
            memcpy (tempBuffer+12, [dataToSend bytes], [dataToSend length]);
		
		uint8* marker = (uint8 *) tempBuffer;
		// now encrypt the message packet
#if qEncryption
		psCryptor->EncryptDecrypt (true, tempBuffer, plainTextLength, tempBuffer, encryptedLength, &encryptedLength);
#endif
		
		// write the text

		NSUInteger remainingToWrite = encryptedLength;
         while (0 < remainingToWrite) {
            NSInteger actuallyWritten = [outputStream write:marker maxLength:remainingToWrite];
			
			status = [outputStream streamStatus];
			if ( actuallyWritten < 0
				|| status == NSStreamStatusError
				|| status == NSStreamStatusClosed
				|| status == NSStreamStatusNotOpen)
				{
				err = [outputStream streamError];
				free (tempBuffer);
				NSBeep();
				return 0;
				}
			
            remainingToWrite -= actuallyWritten;
            marker += actuallyWritten;
        }
		free (tempBuffer);
// encrypt until the end of the message

    }

	return GetLastTransactionID();
}

// ---------------------------------------------------------------------------

//Call SecKeychainAddGenericPassword to add a new password to the keychain:

const char* kServiceName = "com.adobe.cocoaechoclient";

static OSStatus SetKeychainPassword (NSString* password, NSString* serverName)
	{
	void *existingPasswordData = nil;
	UInt32 existingPasswordLength = nil;
	SecKeychainItemRef itemRef = nil;

    const char *passwordUTF8 = [password UTF8String];
	UInt32 passwordLength = (UInt32)strlen(passwordUTF8);

	const char * accountName = [serverName UTF8String];
	UInt32 accountNameLength = (UInt32)strlen(accountName);

	UInt32 serviceNameLength =  (UInt32)strlen(kServiceName);

	OSStatus status = SecKeychainFindGenericPassword (  NULL,
							serviceNameLength, kServiceName,
							accountNameLength,  accountName,
							&existingPasswordLength,  &existingPasswordData, &itemRef );

	if (status == noErr)
		{
		status = SecKeychainItemFreeContent ( NULL, existingPasswordData);
		status = SecKeychainItemModifyAttributesAndData ( itemRef, NULL, passwordLength, passwordUTF8 );
		}
    else if (status == errSecItemNotFound)
		status = SecKeychainAddGenericPassword ( NULL,
						serviceNameLength, kServiceName,
						accountNameLength, accountName,
						passwordLength, passwordUTF8, NULL);
    return (status);
 }

// ---------------------------------------------------------------------------

//Call SecKeychainFindGenericPassword to get a password from the keychain:
static NSString * GetKeychainPassword (NSString* serverName)
{
	SecKeychainItemRef itemRef = nil;
    void *keyChainPasswordData;
	UInt32 keyChainPasswordLength;
    NSString* keyChainPassword = nil;

	const char * accountName = [serverName UTF8String];
	UInt32 accountNameLength = (UInt32)strlen(accountName);

	UInt32 serviceNameLength =  (UInt32)strlen(kServiceName);

	OSStatus status = SecKeychainFindGenericPassword (  NULL, 
                 serviceNameLength, kServiceName,
                 accountNameLength, accountName,
                 &keyChainPasswordLength, &keyChainPasswordData, &itemRef );
				 
	if (status == noErr)
		{
		keyChainPassword = [[[NSString alloc] 
						initWithData: [NSData dataWithBytes:keyChainPasswordData length:keyChainPasswordLength] 
						encoding:NSUTF8StringEncoding] autorelease];
		SecKeychainItemFreeContent ( NULL,  keyChainPasswordData   );
		}
	return keyChainPassword;
}


// ---------------------------------------------------------------------------

- (id) init {
    
    sEchoClient = self; //we are created from the nib

	serviceBrowser = [[NSNetServiceBrowser alloc] init];
	serviceList = [[NSMutableArray alloc] init];
    serviceSelections = [[NSMutableIndexSet alloc] init];
    
	[serviceBrowser setDelegate:self];

  //see if password and/or app path (Photoshop app were specified on command-line)
    NSUserDefaults *args = [NSUserDefaults standardUserDefaults];
 
    if (args)
        {
        NSString* passwordArg = [args stringForKey:@"password"];
        NSString* serverArg = [args stringForKey:@"server"];
        NSString* logPathArg = [[args stringForKey:@"logPath"] stringByStandardizingPath];
        NSString* appPath = [[args stringForKey:@"appPath"] stringByStandardizingPath];
        NSString* imagePath = [args stringForKey:@"imagePath"];
       
        if (appPath)
            {
            if (![[NSWorkspace sharedWorkspace] launchApplication: appPath])
                LogError("Failed to launch %@", appPath);
            }
            
        // set default password
        if (passwordArg)
            self.password = passwordArg;
        
        if (logPathArg)
            self.logFilePath = logPathArg;

       if (imagePath)
            self.imageFilePath = imagePath;

        if (serverArg)
            self.preferredServer = serverArg;
        }

   if (self.logFilePath == nil)
        self.logFilePath = [@"~/EchoClientResults" stringByExpandingTildeInPath];

   if (self.imageFilePath == nil)
        self.imageFilePath = [@"~" stringByExpandingTildeInPath];
 
    [serviceBrowser searchForServicesOfType:@kNameOfService inDomain:@""];

	return self;
}

// ---------------------------------------------------------------------------

- (void)dealloc {
	[self disconnectFromService];
	[dataBuffer release];
	
	[serviceBrowser release];
	[serviceList release];
	
	self.password = nil;
    self.delegate = nil;
    self.logFilePath = nil;
    self.imageFilePath = nil;
    self.preferredServer = nil;
    
	[super dealloc];
}

// ---------------------------------------------------------------------------

#pragma mark -
#pragma mark NSNetServiceBrowser delegate methods

- (void)doDisconnect {

    if (connectedService) {

		[inputStream close];
		[outputStream close];
		[inputStream removeFromRunLoop:[NSRunLoop currentRunLoop] forMode:NSDefaultRunLoopMode];
		[outputStream removeFromRunLoop:[NSRunLoop currentRunLoop] forMode:NSDefaultRunLoopMode];
		[inputStream setDelegate:nil];
		[outputStream setDelegate:nil];
		[inputStream release];
		[outputStream release];
		[connectedService release];
		
		inputStream = nil;
		outputStream = nil;
		packetBodySize = -1;
		connectedService = nil;
        
        delete psCryptor;
        psCryptor = NULL;
	}
}

// ---------------------------------------------------------------------------

- (void)disconnectFromService {

    if (connectedService) {
    
        LogInfo("Disconnecting from %@", [connectedService name]);
        [self doDisconnect];
        [self.delegate connectionClosed];
	}
}

// ---------------------------------------------------------------------------

- (void)notifyConnected {

    if (connectedService)
        {
        LogInfo("Connected to %@", [connectedService name]);
        [self.delegate connectionOpened];

        if ([self.delegate shouldSavePassword])
            SetKeychainPassword (self.password, [connectedService name]);
        }
}

// ---------------------------------------------------------------------------

- (void)connectToService:(NSNetService*)selectedService {

    [self disconnectFromService];
    bool connected = false;

    if ([selectedService getInputStream:&inputStream outputStream:&outputStream])
        {
        LogInfo("Connecting to %@...", [selectedService name]);

        connectedService = [selectedService retain];
        [inputStream retain];
        [outputStream retain];
        [inputStream setDelegate:self];
        [outputStream setDelegate:self];

        [inputStream scheduleInRunLoop:[NSRunLoop currentRunLoop] forMode:NSDefaultRunLoopMode];
        [outputStream scheduleInRunLoop:[NSRunLoop currentRunLoop] forMode:NSDefaultRunLoopMode];
        [inputStream open];
        [outputStream open];
        packetBodySize = -1;


        // lifetime of the string is shared with the NSString it came from

		if (self.password)
			{
			psCryptor = new PSCryptor ([ self.password cStringUsingEncoding:NSUTF8StringEncoding ]);
			//test the connection
			connected =  [[self sendJavaScriptGetResult:@"\"Connected\";" ] isEqualToString:@"Connected"];
			}

		if (!connected)
			{

			NSString * keyChainPassword = GetKeychainPassword ([selectedService name]);
			if (keyChainPassword)
				{
				psCryptor = new PSCryptor ([ keyChainPassword cStringUsingEncoding:NSUTF8StringEncoding ]);
                connected =  [[self sendJavaScriptGetResult:@"\"Connected\";" ] isEqualToString:@"Connected"];
				if (connected)
					self.password = keyChainPassword;
				}
			}
		}
    
    if (!connected)
        [self doDisconnect]; //cleanup
    
    else if (connectedService)
        [self notifyConnected];
    
}

// ---------------------------------------------------------------------------

- (void)connectToNthService:(int)serviceIndex {

	[self connectToService: [serviceList objectAtIndex:serviceIndex]];

}

// ---------------------------------------------------------------------------

// We broadcast the willChangeValueForKey: and didChangeValueForKey: for the NSTableView binding to work.

- (void)netServiceBrowser:(NSNetServiceBrowser *)aNetServiceBrowser didFindService:(NSNetService *)aNetService moreComing:(BOOL)moreComing {
    if (![serviceList containsObject:aNetService]) {
    
        [self willChangeValueForKey:@"serviceList"];
        [serviceList addObject:aNetService];
        [self didChangeValueForKey:@"serviceList"];

        //no connection, no selection, and just one service.
        //select it and try to connect
        
        //connect to the server if it's the requested server or the first one (if no requested server)
        //and there's no connection
        
       if (connectedService == nil &&
            (self.preferredServer == nil || [[aNetService name] isEqualToString: self.preferredServer ]))
			{
            [self connectToService: aNetService ];
            
            if (connectedService)
                {
                NSInteger index = [serviceList indexOfObject: connectedService]; //fixup selection
                if (![serviceSelections containsIndex: index])
                    {
                    [self willChangeValueForKey:@"serviceSelections"];
                    [serviceSelections addIndex:index];
                    [self didChangeValueForKey:@"serviceSelections"];
                    }
                }
     
			}
    }
}

// ---------------------------------------------------------------------------

- (void)netServiceBrowser:(NSNetServiceBrowser *)aNetServiceBrowser didRemoveService:(NSNetService *)aNetService moreComing:(BOOL)moreComing {
    if ([serviceList containsObject:aNetService]) {

        NSInteger index = [serviceList indexOfObject: aNetService]; //fixup selection
        if ([serviceSelections containsIndex: index])
            {
            [self willChangeValueForKey:@"serviceSelections"];
            [serviceSelections removeIndex: index];
            [self didChangeValueForKey:@"serviceSelections"];
            }

        [self willChangeValueForKey:@"serviceList"];
        [serviceList removeObject:aNetService];
        [self didChangeValueForKey:@"serviceList"];

		if (connectedService && [[aNetService name] isEqualToString: [connectedService name] ] ) {
			[self disconnectFromService];
		}
    }
}


// ---------------------------------------------------------------------------

#pragma mark -
#pragma mark Stream methods

- (void)stream:(NSStream *)aStream handleEvent:(NSStreamEvent)streamEvent {
    NSInputStream * istream;

	@try {

    switch(streamEvent) {
		case NSStreamEventHasBytesAvailable:
			{

			UInt8 buffer[1024];
			
            istream = (NSInputStream *)aStream;
            if (!dataBuffer) {
                dataBuffer = [[NSMutableData alloc] initWithCapacity:2048];
            }
			
			NSInteger actuallyRead = [istream  read:buffer maxLength:1024];
			
			if (actuallyRead < 0)
				{
				NSBeep();
                LogError("Read failure, disconnecting");
				[self disconnectFromService];
				break;
				}
			
			[dataBuffer appendBytes:buffer length:actuallyRead];

			// see if we have enough to process, loop over messages in buffer
			while( YES ) {
			
				// Did we read the header yet?
				if ( packetBodySize == -1 ) {
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
				if ( [dataBuffer length] >= ((NSUInteger) packetBodySize) ) {
				  // We now have enough data to extract a meaningful packet.
					const size_t kPrologLength = 16;
					char *buffer = (char *) [dataBuffer bytes];
					
					unsigned long com_status = *((unsigned long *)(buffer + 0));
					com_status = ntohl( com_status );

					
					// decrypt the message
					size_t decryptedLength = (size_t) packetBodySize - 4;	// don't include com status
					
					int skip_message = 0;
					
#if qEncryption
					if (com_status == 0 && psCryptor)
						{
						PSCryptorStatus result = psCryptor->EncryptDecrypt (false, buffer+4, decryptedLength, buffer+4, decryptedLength, &decryptedLength);
						
						if (kCryptorSuccess != result)
							{
							// failure to decrypt, ignore this message and disconnect
                            LogError("Decryption failure, disconnecting");
							skip_message = 1;
                             [self disconnectFromService];
							}
						}
#endif
					
					if (!skip_message)
						{
				  
						// protocol version, 32 bit unsigned int, network byte order
						uint32_t protocol_version = *((uint32_t *)(buffer + 4));
						protocol_version = ntohl( protocol_version );
						
						if (protocol_version != 1)
							{
							// corrupt, or protocol is newer than what we understand
							skip_message = 1;
							}
						
						if (!skip_message)
							{
							// transaction, 32 bit unsigned int, network byte order
							uint32_t transaction = *((uint32_t *)(buffer + 8));
							transaction = ntohl( transaction );
							
							// content type, 32 bit unsigned int, network byte order
							uint32_t content = *((uint32_t *)(buffer + 12));
							content = ntohl( content );
							
							if (com_status != 0)
								content = kPSNetErrorString; // error string
							
							uint8_t *received_data = (uint8_t *)(buffer+kPrologLength);
							size_t received_length = (decryptedLength-(kPrologLength-4));

							// see if this is a response we're looking for
							if (content == kPSNetImage)		// image data
								{
								// process as image data
								
								uint8_t image_format = *((uint8_t *)received_data);
								
								if (image_format == jpeg_format)	// JPEG
									{
									size_t jpeg_length = received_length - 1;

									NSData *imagedata = [NSData dataWithBytes:(received_data+ 1) length:jpeg_length];
									NSImage *ourImage = [[[NSImage alloc] initWithData:imagedata] autorelease];	// this will be NULL if the data isn't readable
									[self imageMessage: ourImage  transaction: transaction];
									}
								else if (image_format == pixmap_format)		// Pixmap
									{
									bool imageError = false;
									
									uint32_t pixmap_prolog_length = 3*4 + 3 + 1;

									uint32_t width = *((uint32_t *)(received_data + 1));
									width = ntohl(width);
									
									uint32_t height = *((uint32_t *)(received_data + 5));
									height = ntohl( height );
									
									uint32_t rowBytes = *((uint32_t *)(received_data + 9));
									rowBytes = ntohl( rowBytes );
									
									uint8_t mode = *((uint8_t *)received_data + 13);	// must be 1 (rgb) for now
									uint8_t channels = *((uint8_t *)received_data + 14);
									uint8_t bitsPerChannel = *((uint8_t *)received_data + 15);	// must be 8 for now
									
									const uint8_t *raw_data = received_data + pixmap_prolog_length;
									size_t raw_length = received_length - pixmap_prolog_length;
									
									if (mode != rgb_mode || bitsPerChannel != 8)
										imageError = true;
									
									// 4k being a largish display size
									if (width > 4096 || height > 4096)
										imageError = true;
									
									if (channels < 0 || channels > 4
										|| rowBytes < ((width*channels*bitsPerChannel)/8))
										imageError = true;
									
									if (raw_length < (height*rowBytes))
										imageError = true;
									
									uint8_t *bitmapData = 0;
									NSBitmapImageRep *imageRep = 0;
									
									if (!imageError && width && height && channels)
										{
										// NULL so the rep will allocate it's own memory, and we won't have to keep the message buffer around
										imageRep = [[[NSBitmapImageRep alloc] initWithBitmapDataPlanes:NULL
																		pixelsWide:width
																		pixelsHigh:height
																		bitsPerSample:bitsPerChannel
																		samplesPerPixel:channels
																		hasAlpha:(channels>3)
																		isPlanar:FALSE
																		colorSpaceName:NSCalibratedRGBColorSpace
																		bitmapFormat:(NSAlphaFirstBitmapFormat|NSAlphaNonpremultipliedBitmapFormat)
																		bytesPerRow:rowBytes
																		bitsPerPixel:(bitsPerChannel*channels) ] autorelease];

										// copy the message data into the image rep
										if (imageRep)
											bitmapData = [imageRep bitmapData];
										}

                                    LogInfo("Image, width:%d height:%d channels:%d bitsPerChannel:%d mode:%d\n",width, height, channels, bitsPerChannel, mode);

									if (bitmapData)
										{
										memcpy( bitmapData, raw_data, raw_length );

										NSSize imageSize;
										imageSize.width = width;
										imageSize.height = height;
										

										NSImage *ourImage = [[[NSImage alloc] initWithSize:imageSize] autorelease];
										[ourImage addRepresentation:imageRep ];
										[self imageMessage: ourImage  transaction: transaction];
										}
									else
										{
										//bad image/params
										[self imageMessage: nil  transaction: transaction];
										}
									
									}
								
								}
							else 
								{
								// Set the response string

								if (content == kPSNetErrorString || content == kPSNetJavaScript)
									{
									NSString * string = [[[NSString alloc] initWithBytes:received_data  length:received_length encoding:NSUTF8StringEncoding ] autorelease];

									if (content == kPSNetErrorString)
										[self errorMessage: string transaction:transaction];
									else
										[self javaScriptMessage: string transaction:transaction];
									}
								else {
									if (content == kPSNetFileStream)
										{
										uint32_t headerLen = ntohl(*((uint32_t *)received_data));

										if (headerLen+4 > received_length)
											[self errorMessage: [NSString stringWithFormat:@"Bogus Header Length %u in FileStream content",(unsigned int)headerLen] transaction:transaction];
										else
											{
											NSString * fileInfo = [[[NSString alloc] initWithBytes:received_data+4  length:headerLen encoding:NSUTF8StringEncoding ] autorelease];

											NSData * data = [NSData dataWithBytes:received_data+4+headerLen  length:received_length-4-headerLen];
											[self fileMessage: data fileInfo: fileInfo transaction:transaction];
											}
										}
									else
										{
										NSData * data = [NSData dataWithBytes:received_data  length:received_length];
										[self dataMessage: data transaction:transaction];
										}
									}
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
			}

            break;
        case NSStreamEventEndEncountered:
        case NSStreamEventErrorOccurred:
            LogError("NSStream Error");
            [self disconnectFromService];
            break;
        case NSStreamEventHasSpaceAvailable:
        case NSStreamEventOpenCompleted:
        case NSStreamEventNone:
        default:
            break;
    }

	}

	@catch(id exception) {

        LogError( [exception description]);
		
	}
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------


struct TransactionHandler
{
    TransactionHandler() {}
    virtual ~TransactionHandler();

	virtual void HandleJavaScript(NSString* , bool inFromHandleResponse) {};
    virtual void HandleError(NSString* ) {};
    virtual void HandleImage(NSImage * ) {};
    virtual void HandleFile(NSData *,NSString*) {};
    virtual void HandleData(NSData * ) {};
};

// ---------------------------------------------------------------------------

static std::map<transaction_id,TransactionHandler*> sTransactions;
static std::set<transaction_id> sHandleResponseTransactions;

// ---------------------------------------------------------------------------

TransactionHandler::~TransactionHandler() {

	for (std::map<transaction_id,TransactionHandler*>::iterator ii = sTransactions.begin(); ii != sTransactions.end(); )
		{
		std::map<transaction_id,TransactionHandler*>::iterator jj = ii++;
		if (jj->second == this)
			{
			sTransactions.erase(jj);
			sHandleResponseTransactions.erase(jj->first);
			}
		}

}

// ---------------------------------------------------------------------------

struct GetResultHandler : public TransactionHandler
{
    NSString* fResult;
    bool fTrimResult;
    GetResultHandler(bool inTrimResult=false) : fResult(0), fTrimResult(inTrimResult) {}
    ~GetResultHandler() {[fResult release];}
    virtual void HandleJavaScript(NSString* inString, bool inFromHandleResponse)
        {
        if (fTrimResult)
            {
            if ([inString length])
                inString = [inString stringByTrimmingCharactersInSet: [ NSCharacterSet whitespaceAndNewlineCharacterSet]];
            else
                return;
            }
            
        if (inString)
            fResult = (fResult == nil) ?
                            [[NSString stringWithString: inString] retain]
                        :   [[[fResult autorelease] stringByAppendingString: inString] retain];
        }
};

// ---------------------------------------------------------------------------

-(void) imageMessage:(NSImage *)image transaction:(transaction_id) transaction {

   LogInfo("%u image received:\n%@ ",transaction, image);

	TransactionHandler* handler = sTransactions[transaction];
    if (handler && image)
        handler->HandleImage(image);
		
    [self.delegate processImageMessage:image transaction:transaction];
}

// ---------------------------------------------------------------------------

-(void) javaScriptMessage:(NSString*)string transaction:(transaction_id) transaction {

	LogInfo("%u javaScript received:\n%@",transaction, string);

	TransactionHandler* handler = sTransactions[transaction];

	if (handler && string)
        handler->HandleJavaScript(string,sHandleResponseTransactions.find(transaction) != sHandleResponseTransactions.end());

    [self.delegate processJavaScriptMessage:string transaction:transaction];
}

// ---------------------------------------------------------------------------

-(void) errorMessage:(NSString*)string transaction:(transaction_id) transaction {

	if (![string isEqual: @"Error: No image open"] ) //ignore these, we ask for things without checking for an image
		{
		LogError("error transaction %u: %@",transaction, string);

		TransactionHandler* handler = sTransactions[transaction];

		if (handler && string)
			handler->HandleError(string);

		[self.delegate processErrorMessage:string transaction:transaction];
		}
}

// ---------------------------------------------------------------------------

-(void) fileMessage:(NSData*)data fileInfo:(NSString*)fileInfo transaction:(transaction_id) transaction {

	LogInfo("%u file received",transaction);

	TransactionHandler* handler = sTransactions[transaction];
    if (handler && data)
        handler->HandleFile(data,fileInfo);

    [self.delegate processFileMessage:data fileInfo:fileInfo transaction:transaction];
}

// ---------------------------------------------------------------------------

-(void) dataMessage:(NSData*)data transaction:(transaction_id) transaction {

	LogInfo("%u data received",transaction);

	TransactionHandler* handler = sTransactions[transaction];
    if (handler && data)
        handler->HandleData(data);

    [self.delegate processDataMessage:data transaction:transaction];
}

// ---------------------------------------------------------------------------

-(transaction_id) sendJavaScript: (NSString*)inJSString withHandler:(TransactionHandler*)inHandler forHandleResponse:(BOOL)handleResponse withCatch:(BOOL)catchExceptions{

    transaction_id nextTransaction = GetNextTransactionID();
    
	transaction_id mappedTransaction = nextTransaction;
	sTransactions[nextTransaction]= inHandler; //set before, in case it responds right away

	if (handleResponse)
		sHandleResponseTransactions.insert(nextTransaction); //set before, in case it responds right away

    transaction_id transaction = [self sendJavaScript: inJSString withCatch:catchExceptions];
    
	if (transaction != mappedTransaction)
		[self errorMessage:@"ERROR: transaction is wrong %u" transaction: nextTransaction];

	if (sTransactions[transaction] != inHandler)
		[self errorMessage:@"ERROR: transaction is not mapped %u" transaction: nextTransaction];

	return transaction;
}

// ---------------------------------------------------------------------------

-(NSString*) sendJavaScriptGetResult: (NSString*)inJSString  {

    GetResultHandler resultHandler;

	transaction_id transaction = [ self sendJavaScript:inJSString withHandler: &resultHandler forHandleResponse:NO withCatch:YES];
    
    const double waitSeconds = 2.0;
    
    NSDate* endDate = [[NSDate date] dateByAddingTimeInterval: waitSeconds];

    while (resultHandler.fResult == nil)
        {
        NSDate* present = [NSDate date];
        [[NSRunLoop currentRunLoop] runUntilDate: [present dateByAddingTimeInterval: .1] ];
        
        if ([present compare: endDate] != NSOrderedAscending)
            break;
        }

    if (resultHandler.fResult  == nil)
        LogError("Timed out on transaction %u", transaction);

	return [[resultHandler.fResult retain] autorelease];
}


// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

NSDate* getPathDate(NSString* inPath)
    {
    return [[[NSFileManager defaultManager] attributesOfItemAtPath:inPath error: nil] fileModificationDate];
    }

-(NSString*) getTestJS:(NSString*)testJSPath  {

        NSError* error = nil;

        {

        NSString* kTestPrologueJSPath = @"TestPrologue.jsx";
        NSString *jsFunctionsPath = [[NSBundle mainBundle] pathForResource:kTestPrologueJSPath ofType:nil];

        if (jsFunctionsPath == nil)
            LogError("Couldn't find Javascript builtins at %@", kTestPrologueJSPath);

        else
            {
            NSStringEncoding enc;

             //build up "prologue" to send before the user-specified test Javascript
			[sTestJS release];
            
            NSString* jsFunctions =  [NSString stringWithContentsOfFile:jsFunctionsPath usedEncoding:&enc error:&error];
            
            if (error)
                LogError("Couldn't load Javascript at %@. %@", jsFunctionsPath, [error localizedDescription]);

            NSString* jsGlobals = [[@"var __generatorTestLogFilePath = \"" stringByAppendingString: [self makeLogFilePath: @"TestLog.txt"]] stringByAppendingString: @"\";"];

            NSString* prologJS = [[NSString stringWithString:jsGlobals] stringByAppendingString:jsFunctions];
     
            NSString* testJS = [NSString stringWithContentsOfFile: testJSPath usedEncoding:&enc error:&error];
     
            if (error)
                LogError("Couldn't load Javascript at %@. %@", testJSPath, [error localizedDescription]);
            else
                sTestJS = [[prologJS stringByAppendingString: testJS] retain];
            }
       }


      return error ? nil : sTestJS;
}


// ---------------------------------------------------------------------------

-(NSString*) stringJSParam: (NSString*)inParam  {

	if ([inParam isEqualTo:@"undefined"]) //return nothing from handleResponse when finished
		return inParam;
	
	inParam =  [inParam stringByReplacingOccurrencesOfString:@"\\" withString:@"\\\\"];
	inParam =  [inParam stringByReplacingOccurrencesOfString:@"\"" withString:@"\\\""];
	inParam =  [inParam stringByReplacingOccurrencesOfString:@"\r" withString:@"\\r"];
	inParam =  [NSString stringWithFormat:@"\"%@\"", inParam];

	return inParam;
}

// ---------------------------------------------------------------------------

-(NSString*) dataJSParam: (NSData*)inData{
	static int sFileNumber = 0;
	NSString* path = [ [[EchoClient get] imageFilePath]  stringByAppendingFormat: @"/data_%d.dat", sFileNumber++];
	[inData writeToFile: path atomically: YES];
	return  [NSString stringWithFormat:@"File(\"%@\")", path];
}

// ---------------------------------------------------------------------------

static std::vector<NSString*> sTempFileList;

-(NSString*) fileJSParam: (NSData*)inData  info:(NSString*)inInfo{ //inInfo is JSON with MIME type
	static int sFileNumber = 0;
	NSString* path = [ [[EchoClient get] imageFilePath]  stringByAppendingFormat: @"/ps_echoclient_temp_file_%d", sFileNumber++];
    
    sTempFileList.push_back([path retain]);
    
	[inData writeToFile: path atomically: YES];
	return  [NSString stringWithFormat:@"File(\"%@\"),%@", path,[self stringJSParam:inInfo ]];
}

// ---------------------------------------------------------------------------

-(NSString*) imageJSParam: (NSImage*)inImage  {

	static int sImageNumber = 0;
	NSString* path = [ [[EchoClient get] imageFilePath]  stringByAppendingFormat: @"/image_%d.tiff", sImageNumber++];
	[[inImage TIFFRepresentation] writeToFile: path atomically: YES];
	return  [NSString stringWithFormat:@"File(\"%@\")", path];
}

// ---------------------------------------------------------------------------

-(transaction_id) callTest: (NSInteger)inTestNum withName: (NSString*)inProc andParam: (NSString*)inParam andHandler:(TransactionHandler*)inHandler {

	transaction_id transaction = [ [EchoClient get] sendJavaScript:
			[sTestJS stringByAppendingFormat:
					@"try{var procs = __generatorTestProcs[%d];procs.%@(%@);}catch(e){procs.handleException(e); \"__generatorTestException\";}",
						(int)inTestNum, inProc, inParam]
								 withHandler: inHandler forHandleResponse: [inProc isEqualTo:@"handleResponse"] withCatch:NO];
 
    return transaction;
}
@end

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

//this is what we use to handle responses from the various user-specified test procs in the javascript files
//TestHandler handles responses/errors/images returned by the "test" proc (by sending them onto the user's "handleResponse/Image/Error" js
//It also has sub-handlers (fResponseHandler,fImageHandler) for handling the responses from handleResponse/handleImage themselves

struct TestHandler : public TransactionHandler
{
    int fTestIndex;
    enum State{eActive,eFinished,eError};
    State fState;
    
    TestHandler(int inTestIndex) :
		fTestIndex(inTestIndex), fState(eActive){}

    virtual void HandleJavaScript(NSString* inString,bool inHandleResponse)
        {
		//special internal response that indicates an uncaught exception (error)
		if ([inString isEqualTo:@"__generatorTestException"])
            HandleError(inString);

		if (fState >= eFinished)
			return;

		else
			{
			if (inHandleResponse && [inString isEqualTo:@"undefined"]) //return nothing from handleResponse when finished
				{
                fState = eFinished;
				return;
				}

			[ [EchoClient get] callTest: fTestIndex withName: @"handleResponse" andParam: [[EchoClient get] stringJSParam: inString] andHandler: this];
			}
        }

    virtual void HandleError(NSString* inString)
        {
        TransactionHandler::HandleError(inString);
        fState = eError;
        }
    
    virtual void HandleImage(NSImage* inImage)
        {
		if (fState >= eFinished)
			return;

        [ [EchoClient get] callTest: fTestIndex withName: @"handleResponse" andParam: [[EchoClient get] imageJSParam: inImage] andHandler: this];
        }

    virtual void HandleData(NSData* inData)
		{
		if (fState >= eFinished)
			return;
        [ [EchoClient get] callTest: fTestIndex withName: @"handleResponse" andParam: [[EchoClient get] dataJSParam: inData] andHandler: this];
		}

    virtual void HandleFile(NSData* inData,NSString*inInfo)
		{
		if (fState >= eFinished)
			return;
       [ [EchoClient get] callTest: fTestIndex withName: @"handleResponse" andParam: [[EchoClient get] fileJSParam: inData info:inInfo] andHandler: this];
		}
};

// ---------------------------------------------------------------------------


int TestOneFile(const char* inFilePath, const char* inTestProcsPath, double inTimeOutSeconds, bool inInit)
{
    NSString* kDefaultTestFile = @"Shapes.psd";
    NSString* kDefaultJavaScript = @"SimpleTest.jsx";
	bool success = true;

    NSString* filePath = inFilePath ?
                                    [NSString stringWithUTF8String:inFilePath] :
                                    [[NSBundle mainBundle] pathForResource:kDefaultTestFile ofType:nil];
    
    NSString* testProcsPath = inTestProcsPath ?
                                    [NSString stringWithUTF8String:inTestProcsPath] :
                                    [[NSBundle mainBundle] pathForResource:kDefaultJavaScript ofType:nil];

    EchoClient* echoClient = [EchoClient get];

    //wait for the service to connect.
    //We are in batch mode, so this is all we can do to get something to send our Javascript to.
 
    NSNetService* service = [echoClient getConnectedService:inTimeOutSeconds];

    if (service == nil)
        LogError("Timed out waiting for connection");

    if (filePath == nil)
        LogError("Couldn't load test file at %@", filePath);

    if (testProcsPath == nil)
        LogError("Couldn't load test JavaScript file at %@", filePath);

    if ( service && echoClient && testProcsPath && filePath)
        {
       NSString* testJS  = [ echoClient getTestJS: testProcsPath ];

        if (testJS)
            {
            LogInfo("Testing File: %@ using Javascript: %@",
                    (inFilePath ? filePath : kDefaultTestFile),
                    (inTestProcsPath ? testProcsPath : kDefaultJavaScript));

			NSString*  response = [echoClient sendJavaScriptGetResult:[NSString stringWithFormat: @"%@;__generatorTestProcs.length;", testJS ]];
			
            int numTests = [response intValue];
            if (numTests == 0)
                {
                LogError (response);
                LogError (@"No tests found, or bogus format. See Simpletest.jsx in the app package for an example");
                success = false;
                }
                
            std::vector<TestHandler*> testHandlers;

			for (int tt = 0; tt < numTests; ++tt)
				testHandlers.push_back(new TestHandler(tt));

			if (inInit)
				{
				for (int tt = 0; tt < numTests; ++tt)
					[ echoClient callTest: tt withName: @"init" andParam: [[EchoClient get] stringJSParam: filePath]  andHandler: testHandlers[tt]];
				}

			for (int tt = 0; tt < numTests; ++tt)
                [ echoClient callTest: tt withName: @"test" andParam: [[EchoClient get] stringJSParam: filePath] andHandler: testHandlers[tt]];

            NSDate* endDate = [[NSDate date] dateByAddingTimeInterval: inTimeOutSeconds];
            
            bool pendingTests = true;

            while (pendingTests)
                {
                pendingTests = false;

                 for (int tt = 0; tt < numTests; ++tt)
                    {
                    if (testHandlers[tt]->fState == TestHandler::eError)
						{
                        pendingTests = false;
						success = false;
						break;
						}
					
                    else if (testHandlers[tt]->fState < TestHandler::eFinished)
                        pendingTests = true;
                    }

                NSDate* present = [NSDate date];
            
                if (!pendingTests || [present compare: endDate] != NSOrderedAscending)
                    break;

                [[NSRunLoop currentRunLoop] runUntilDate: [present dateByAddingTimeInterval: .1] ];
                }
 
           if (pendingTests)
                {
				success = false;
				for (int tt = 0; tt < numTests; ++tt)
                    {
                    if (testHandlers[tt]->fState < TestHandler::eFinished)
						LogError ("Test %d timed out waiting for a response", tt);
                    }
                }
			
			for (int tt = 0; tt < testHandlers.size(); ++tt)
			   delete testHandlers[tt];
			}
        }

    if (!success)
        LogError("Test failed");
    
        //make sure we clean up our file droppings
    for (int tt = 0; tt < sTempFileList.size(); ++tt)
        {
        const char* pathCStr = [sTempFileList[tt] cStringUsingEncoding:NSUTF8StringEncoding ];
        remove(pathCStr);
        [sTempFileList[tt] release];
        }
    sTempFileList.clear();
    
    return success ? 0 : 1;
}

