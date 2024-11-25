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

#import "PSConnection.h"
#import "SynthesizeSingleton.h"
#import "SFHFKeychainUtils.h"

/**********************************************************************************
* Class private declarations
**********************************************************************************/

@interface PSConnection (private)

- (NSUInteger) nextTransactionID;

- (void) flipNetworkByteOrder:(PSNetworkPacket*)packet;

- (void) sendAuthentication;
- (void) sendAuthenticationTimerFired:(NSTimer*)timer;

- (void) connectionAttemptSucceeded:(NetConnection*)connection;
- (void) connectionAttemptFailed:(NetConnection*)connection withError:(PSConnectionError)error;

- (void) informConnectionStartedDelegates;
- (void) informConnectionOpenedDelegates;
- (void) informConnectionClosedDelegates;
- (void) informConnectionFailedDelegates:(PSConnectionError)error;
- (void) informConnectionServersUpdatedDelegates;

- (void) informConnectionDelegatesReceived:(NSUInteger)transaction error:(NSString*)error;
- (void) informConnectionDelegatesReceived:(NSUInteger)transaction javascript:(NSString*)script;
- (void) informConnectionDelegatesReceived:(NSUInteger)transaction image:(UIImage*)image;

@end

/**********************************************************************************
* Class implementation
**********************************************************************************/

@implementation PSConnection

/**********************************************************************************
* Class synthesizication
**********************************************************************************/

SYNTHESIZE_SINGLETON_FOR_CLASS(PSConnection);
@synthesize delegates = _delegates;
@synthesize createNSDataForReceivedImages = _createNSDataForReceivedImages;

/**********************************************************************************
* Class property methods
**********************************************************************************/

/**********************************************************************************
* Class overload methods
**********************************************************************************/

- (id) init
{
	if ((self = [super init]) != nil)
	{
		// create the photoshop server browser
		_browser = [[NetServerBrowser alloc] init];
		_browser.delegate = self;
		[_browser startForService:@"_photoshopserver._tcp" inDomain:@""];

		// no connection until requested
		_server = nil;
		_connection = nil;
		_password = nil;
		
		// connection status
		_status = kPSConnectionStatus_Closed;
		
		// nothing is suspended
		_suspendedServerName = nil;
		_suspendedHost = nil;
		
		// no hosts
		_hosts = nil;
		
		// load any saved hosts
		NSArray* savedHosts = [[NSUserDefaults standardUserDefaults] stringArrayForKey:@"PSConnectionSavedHosts"];
		if (savedHosts != nil && savedHosts.count > 0)
		{
			_hosts = [[NSMutableArray alloc] initWithArray:savedHosts];
		}
		
		// no transaction ID yet
		_transactionID = 0;
		_sentTransations = nil;
		_failedTransactions = nil;
		
		// no cryptor yet
		_cryptor = NULL;
		
		// default is to NOT copy data for received images
		// setting to NO will use the bytes from the stream buffer
		// this is ideal for clients that can use the image immediately
		// however clients that will keep the image for later use should
		// set this property to true and be responsible for releasing
		_createNSDataForReceivedImages = NO;
	}
	
	return self;
}

- (void) dealloc
{
	[_delegates removeAllObjects];
	[_delegates release];

	[_browser release];
	[_connection release];
	
	_server = nil;
	_host = nil;
	
	_suspendedServerName = nil;
	_suspendedHost = nil;
	
	[_hosts release];
	
	[_sentTransations removeAllObjects];
	[_sentTransations release];
	
	[_failedTransactions removeAllObjects];
	[_failedTransactions release];
	
	if (_cryptor != NULL)
	{
		DestroyPSCryptor(_cryptor);
	}
	
	[super dealloc];
}

/**********************************************************************************
* Class delegate methods
**********************************************************************************/

- (void) updateServerList
{
	// if there was a suspended server name
	// then get the list of services and reconnect
	if (_suspendedServerName != nil)
	{
		// loop through all the servers and try to find a match
		for (NSNetService* server in _browser.servers)
		{
			if ([server.name caseInsensitiveCompare:_suspendedServerName] == NSOrderedSame)
			{
				[self openWithServer:server password:_password];
				break;
			}
		}
	}

	// inform the delegates
	[self informConnectionServersUpdatedDelegates];
}

- (void) connectionAttemptFailed:(NetConnection*)connection
{
	// nothing to do if this is not the PS connection
	if (connection != _connection)
	{
		return;
	}

	// don't monkey with the actual connection
	// but update the flag
	_status = kPSConnectionStatus_Closed;
	_host = nil;
	_server = nil;
	_password = nil;

	// inform the delegates
	// since this error comes directly from the lower level
	// NetConnection class, there actual error is not known
	// most likely though it is a time out error
	[self informConnectionFailedDelegates:kPSConnectionError_TimeOut];
}

- (void) connectionTerminated:(NetConnection*)connection
{
	// nothing to do if this is not the PS connection
	if (connection != _connection)
	{
		return;
	}
	
	// close the connection
	[self close];
}

- (void) receivedNetworkData:(NSData*)data viaConnection:(NetConnection*)connection
{
	// get the packet from the data
	PSNetworkPacket* packet = (PSNetworkPacket*) [data bytes];

	// if authenticating
	// check that this was successfully authenticated
	if (_status == kPSConnectionStatus_Connecting && packet->status == kPSNetworkPacket_StatusEncrypted)
	{
		DebugLog(@"Received good communication status, opening connection");
		[self connectionAttemptSucceeded:connection];
	}
	
	// if authenticating
	// and the communication status is bad
	// then the password was proably incorrect
	else if (_status == kPSConnectionStatus_Connecting && packet->status != kPSNetworkPacket_StatusEncrypted)
	{
		DebugLog(@"Received error communication status, failed authentication");
		[self connectionAttemptFailed:connection withError:kPSConnectionError_BadPassword];
	}
	
	// check the status
	// if it is non-zero then the packet was rejected
	// this is probably due to an authentication error
	else if (packet->status != kPSNetworkPacket_StatusEncrypted)
	{
		DebugLog(@"Received error communication status, not decrypting");
	}

	// decrypt the packet from the comm status and beyond
	// this basically makes a copy of the incoming data
	// and passes everything past the comm status to the decryption
	// package, which munges it into the descriptedData buffer
	// from there a packet pointer is used at the start of the buffer
	// to easily identify all of the fields
	NSMutableData* decryptedData = nil;
	if (packet->status == kPSNetworkPacket_StatusEncrypted)
	{
		decryptedData = [NSMutableData dataWithData:data];
		size_t decryptedLength = decryptedData.length - sizeof(packet->status);
		void* bytesToDecrypt = [decryptedData mutableBytes];
		bytesToDecrypt = (void*)(((char*) bytesToDecrypt) + sizeof(packet->status));
		PSCryptorStatus status = EncryptDecrypt(_cryptor, FALSE, bytesToDecrypt, decryptedLength,
												bytesToDecrypt, decryptedLength, &decryptedLength);
		if (status != kCryptorSuccess)
		{
			DebugLog(@"Failed packet decryption, ignoring message");
			return;
		}
		
		// trim the decrypted data to match the actualy decrypted length
		// note the decrypted data is only the portion after the header
		// but the data object includes the header, so resize including the header
		[decryptedData setLength:decryptedLength + sizeof(packet->status)];
		
		// cast into a packet again
		packet = (PSNetworkPacket*) [decryptedData mutableBytes];
	}

	// now point to the message data
	// this will be different depending on if the message encrypted or not
	// after this point all code should use the packetData point
	// and should not reference data or decrytpedData
	NSData* packetData = (decryptedData == nil ? data : decryptedData);
	NSRange packetDataRange = {sizeof(PSNetworkPacket), [packetData length] - sizeof(PSNetworkPacket)};
	packetData = [packetData subdataWithRange:packetDataRange];

	// everything except the status in the packet will be encrypted
	// but converting from network byte order before decrypting
	// will give bad results, so swap NBO after decrypting
	[self flipNetworkByteOrder:packet];

	// get the ID from the message
	// and remove it from the sent list
	NSNumber* transactionNumber = [NSNumber numberWithUnsignedInt:packet->transaction];
	[_sentTransations removeObject:transactionNumber];
	
	// if it's an error
	// add it to the failed list
	if (packet->type == kPSNetErrorString)
	{
		// strip off the error string at the end of the packet
		// note that since this is an error message, the data
		// is not encrypted so there is no reason to use decrypted data
		NSString* string = [[NSString alloc] initWithData:packetData encoding:NSUTF8StringEncoding];

		// mark as a failed transaction
		[_failedTransactions setObject:string forKey:transactionNumber];
		
		// debug output the error and inform the delegates
		DebugLog(@"Failed with error string: %@", string);
		[self informConnectionDelegatesReceived:packet->transaction error:string];
		
		// clean up
		[string release];
	}
	
	// if it's javascript
	// send it out to the delegates
	else if (packet->type == kPSNetJavaScript)
	{
		DebugLog(@"Received javascript, sending to delegates");
		NSString* script = [[NSString alloc] initWithData:packetData encoding:NSUTF8StringEncoding];
		[self informConnectionDelegatesReceived:packet->transaction javascript:script];
		[script release];
	}

	// if it's an image
	// send it out to the delegates
	// note that the image data is preceeded by a single byte
	// describing the type of image data so strip that off
	// before attempting to make a UIImage from the packet data
	else if (packet->type == kPSNetImage)
	{
		DebugLog(@"Received image, sending to delegates");
		char* bytes = (char*) [packetData bytes];
		
		// create or reference the image
		// for clients that can use the UIImage immediately
		// createNSDataForReceivedImages should be set to NO
		NSData* imageData = nil;
		if (_createNSDataForReceivedImages == NO)
		{
			imageData = [NSData dataWithBytesNoCopy:(bytes + 1) length:([packetData length] - 1) freeWhenDone:NO];
		}
		
		// for clients that will use the UIImage later or on a
		// different thread createNSDataForReceivedImages = YES
		// this will make a copy of the bytes from the stream which
		// requires time and memory
		else
		{
			imageData = [NSData dataWithBytes:(bytes + 1) length:([packetData length] - 1)];
		}
		
		// create the image with the imageData
		// if the image data is being copied there could be
		// a out-of-memory condition which means the data
		// pointer will be nil
		if (imageData != nil)
		{
			// IMPORTANT IMPORTANT IMPORTANT
			// At this time PSConnection only supports receiving
			// JPGs from PS, receiving a pixmap is currently disabled
			// due to difficulties with converting raw RGB to iOS
			// RGBA using a CoreGraphics bitmap context
			UIImage* image = [UIImage imageWithData:imageData];
			[self informConnectionDelegatesReceived:packet->transaction image:image];
		}
	}
	
	// else the only other type left is data
	// send it out to the delegates
	else
	{
		DebugLog(@"Received unknown type of data, ignoring");
		DebugLog(@"Unknown data bytes:%@", [packetData description]);
	}

	// need to do list maintenance so the lists
	// don't grow unbounded
	if (_failedTransactions.count > kMaxFailedTransactions)
	{
		NSArray* sortedKeys = [_failedTransactions keysSortedByValueUsingSelector:@selector(compare:)];
		NSRange rangeToRemove = {0, kMinFailedTransactions};
		NSArray* keysToRemove = [sortedKeys subarrayWithRange:rangeToRemove];
		[_failedTransactions removeObjectsForKeys:keysToRemove];
	}
}

/**********************************************************************************
* Class private methods
**********************************************************************************/

- (NSUInteger) nextTransactionID
{
	// increment the transaction count
	++_transactionID;

	// create the transaction queue if necessary
	if (_sentTransations == nil)
	{
		_sentTransations = [[NSMutableArray alloc] initWithObjects:[NSNumber numberWithUnsignedInt:_transactionID], nil];
	}
	
	// otherwise just update it
	{
		[_sentTransations addObject:[NSNumber numberWithUnsignedInt:_transactionID]];
	}
	
	// also create the failed queue if necessary
	if (_failedTransactions == nil)
	{
		_failedTransactions = [[NSMutableDictionary alloc] initWithCapacity:0];
	}
	
	// done
	return(_transactionID);
}

- (void) flipNetworkByteOrder:(PSNetworkPacket*)packet
{
	// note that the communication status is NOT
	// in network byte order, only the other fields
	packet->version = ntohl(packet->version);
	packet->transaction = ntohl(packet->transaction);
	packet->type = ntohl(packet->type);
}

- (void) sendAuthentication
{
	// don't send an authentication if already doing so
	if (_status == kPSConnectionStatus_Connecting)
	{
		return;
	}
	
	// mark as authenticating
	_status = kPSConnectionStatus_Connecting;
	
	// schedule to send an authentication packet
	[NSTimer scheduledTimerWithTimeInterval:0.5 target:self selector:@selector(sendAuthenticationTimerFired:) userInfo:nil repeats:NO];
}

- (void) sendAuthenticationTimerFired:(NSTimer*)timer
{
	// the protocol is stateless so there is not handshake
	// packet, instead send some harmless javascript
	[self sendJavascript:@"app.foregroundColor"];
}

- (void) connectionAttemptSucceeded:(NetConnection *)connection
{
	// this is akin to connectionAttemptFailed except
	// for the success case, it's really just to keep the
	// network packet received code cleaner
	_status = kPSConnectionStatus_Open;

	// store the password
	// note that the service name depends on whether the
	// connection was made to a service or hostname
	// also note that there is no specific username associated
	// since the PS connection is single user only
	NSString* serviceName = (_server != nil ? _server.name : _host);
	NSError* error = nil;
	[SFHFKeychainUtils storeUsername:serviceName andPassword:_password forServiceName:serviceName updateExisting:YES error:&error];
	if (error != nil)
	{
		DebugLog(@"Could not store password: %d - %@", [error code], [error localizedDescription]);
	}

	// inform the delegate
	[self informConnectionOpenedDelegates];
}

- (void) connectionAttemptFailed:(NetConnection*)connection withError:(PSConnectionError)error
{
	// nothing to do if this is not the PS connection
	if (connection != _connection)
	{
		return;
	}

	// don't monkey with the actual connection
	// but update the flag
	_status = kPSConnectionStatus_Closed;
	_host = nil;
	_server = nil;
	_password = nil;

	// inform the delegates
	[self informConnectionFailedDelegates:error];
}

- (void) informConnectionStartedDelegates
{
	for (id<PSConnectionDelegate> delegate in _delegates)
	{
		if ([delegate respondsToSelector:@selector(connectionStarted:)])
		{
			[delegate connectionStarted:self];
		}
	}
}

- (void) informConnectionOpenedDelegates
{
	for (id<PSConnectionDelegate> delegate in _delegates)
	{
		if ([delegate respondsToSelector:@selector(connectionOpened:)])
		{
			[delegate connectionOpened:self];
		}
	}
}

- (void) informConnectionClosedDelegates
{
	for (id<PSConnectionDelegate> delegate in _delegates)
	{
		if ([delegate respondsToSelector:@selector(connectionClosed:)])
		{
			[delegate connectionClosed:self];
		}
	}
}

- (void) informConnectionFailedDelegates:(PSConnectionError)error
{
	for (id<PSConnectionDelegate> delegate in _delegates)
	{
		if ([delegate respondsToSelector:@selector(connectionFailed:withError:)])
		{
			[delegate connectionFailed:self withError:error];
		}
	}
}

- (void) informConnectionServersUpdatedDelegates
{
	for (id<PSConnectionDelegate> delegate in _delegates)
	{
		if ([delegate respondsToSelector:@selector(connectionServersUpdated:)])
		{
			[delegate connectionServersUpdated:self];
		}
	}
}

- (void) informConnectionDelegatesReceived:(NSUInteger)transaction error:(NSString*)error
{
	for (id<PSConnectionDelegate> delegate in _delegates)
	{
		if ([delegate respondsToSelector:@selector(connection:received:error:)])
		{
			[delegate connection:self received:transaction error:error];
		}
	}
}

- (void) informConnectionDelegatesReceived:(NSUInteger)transaction javascript:(NSString*)script
{
	for (id<PSConnectionDelegate> delegate in _delegates)
	{
		if ([delegate respondsToSelector:@selector(connection:received:javascript:)])
		{
			[delegate connection:self received:transaction javascript:script];
		}
	}
}

- (void) informConnectionDelegatesReceived:(NSUInteger)transaction image:(UIImage*)image
{
	for (id<PSConnectionDelegate> delegate in _delegates)
	{
		if ([delegate respondsToSelector:@selector(connection:received:image:)])
		{
			[delegate connection:self received:transaction image:image];
		}
	}
}

/**********************************************************************************
* Class public methods
**********************************************************************************/

+ (NSString*) stringForError:(PSConnectionError)error
{
	if (error == kPSConnectionError_None)
	{
		return NSLocalizedString(@"No Error", @"$$$/PSConnection/NoError=No Error");
	}
	else if (error == kPSConnectionError_BadPassword)
	{
		return NSLocalizedString(@"Invalid Password", @"$$$/PSConnection/InvalidPassword=Invalid Password");
	}
	else if (error == kPSConnectionError_TimeOut)
	{
		return NSLocalizedString(@"Server Not Responding", @"$$$/PSConnection/ServerNotResponding=Server Not Responding");
	}
	else if (error == kPSConnectionError_NotFound)
	{
		return NSLocalizedString(@"Server Not Found", @"$$$/PSConnection/ServerNotFound=Server Not Found");
	}
	else
	{
		return NSLocalizedString(@"Could Not Connect", @"$$$/PSConnection/CouldNotConnect=Could Not Connect");
	}
}

- (void) addDelegate:(id)delegate
{
	// init the delegate array
	if (_delegates == nil)
	{
		_delegates = [[NSMutableArray alloc] initWithCapacity:1];
	}
	
	// only add the delegate if it's not already in the list
	if ([_delegates containsObject:delegate] == NO)
	{
		[_delegates addObject:delegate];
	}
}

- (void) removeDelegate:(id)delegate
{
	if (_delegates != nil)
	{
		[_delegates removeObject:delegate];
	}
}

- (BOOL) isOpen
{
	return _status == kPSConnectionStatus_Open;
}

- (BOOL) isOpenToServer:(NSNetService*)server
{
	return [self statusToServer:server] == kPSConnectionStatus_Open;
}

- (BOOL) isOpenToHost:(NSString*)host
{
	return [self statusToHost:host] == kPSConnectionStatus_Open;
}

- (PSConnectionStatus) statusToServer:(NSNetService*)server
{
	if (_server != nil && server != nil && server == _server)
	{
		return _status;
	}
	else
	{
		return kPSConnectionStatus_Closed;
	}
}

- (PSConnectionStatus) statusToHost:(NSString*)host
{
	if (_host != nil && host != nil && [host compare:_host] == NSOrderedSame)
	{
		return _status;
	}
	else
	{
		return kPSConnectionStatus_Closed;
	}
}

- (PSConnectionStatus) status
{
	return _status;
}

- (void) openWithHost:(NSString*)host password:(NSString*)password
{
	// create the host list
	if (_hosts == nil)
	{
		_hosts = [[NSMutableArray alloc] initWithObjects:host, nil];
		[[NSUserDefaults standardUserDefaults] setObject:_hosts forKey:@"PSConnectionSavedHosts"];
	}
	
	// check if the host is already in the list
	// the when a host is added the lists is sorted
	// this needs to be done BEFORE the list is persisted
	if ([_hosts containsObject:host] == NO)
	{
		[_hosts addObject:host];
		[_hosts sortUsingSelector:@selector(compare:)];
		[[NSUserDefaults standardUserDefaults] setObject:_hosts forKey:@"PSConnectionSavedHosts"];
	}

	// make sure the password is valid
	if (password == nil)
	{
		return;
	}
	
	// no server to remember
	// but remember the host and password
	_host = host;
	
	// remember the password
	if (password != _password)
	{
		[_password release];
		_password = [[NSString alloc] initWithString:password];
	}
	
	// reset the connection
	if (_connection != nil)
	{
		[_connection close];
		[_connection release];
		_connection = nil;
	}
	
	// open a new connection
	_connection = [[NetConnection alloc] initWithHostAddress:host andPort:kPhotoshopServerPort];
	_connection.delegate = self;
	[_connection connect];

	// encryption is not used until all
	// server and SDK code has been updated
	// always destroy a cryptor
	// never allow them to be re-used
	if (_cryptor != NULL)
	{
		DestroyPSCryptor(_cryptor);
		_cryptor = NULL;
	}
	
	// create a cryptor object
	// cryptors are only used for an open connection
	// they are never re-used for another connection
	const char* passwordCString = [_password cStringUsingEncoding:NSUTF8StringEncoding];
	_cryptor = CreatePSCryptor(passwordCString);

	// send an authentication packet
	[self sendAuthentication];
	
	// inform the delegates
	[self informConnectionStartedDelegates];
}

- (void) openWithServer:(NSNetService*)server password:(NSString*)password
{
	// make sure the server is still valid
	if ([_browser.servers containsObject:server] == NO)
	{
		[self informConnectionFailedDelegates:kPSConnectionError_NotFound];
		return;
	}
	
	// make sure the password is valid
	if (password == nil)
	{
		return;
	}
	
	// remember the server name now since it
	// needs to be retained through a suspend()
	[_suspendedServerName release];
	_suspendedServerName = server.name;
	[_suspendedServerName retain];
	
	// remember the server
	_server = server;

	// remember the password
	if (password != _password)
	{
		[_password release];
		_password = [[NSString alloc] initWithString:password];
	}

	// reset the connection
	if (_connection != nil)
	{
		[_connection close];
		[_connection release];
		_connection = nil;
	}
	
	// open a new connection
	_connection = [[NetConnection alloc] initWithNetService:server];
	_connection.delegate = self;
	[_connection connect];

	// encryption is not used until all
	// server and SDK code has been updated
	// always destroy a cryptor
	// never allow them to be re-used
	if (_cryptor != NULL)
	{
		DestroyPSCryptor(_cryptor);
		_cryptor = NULL;
	}
	
	// create a cryptor object
	// cryptors are only used for an open connection
	// they are never re-used for another connection
	const char* passwordCString = [_password cStringUsingEncoding:NSUTF8StringEncoding];
	_cryptor = CreatePSCryptor(passwordCString);
	
	// send an authentication packet
	[self sendAuthentication];
	
	// inform the delegates
	[self informConnectionStartedDelegates];
}

- (void) close
{
	// forget server and/or host
	_host = nil;
	_server = nil;
	
	// also need to forget the suspended server
	[_suspendedServerName release];
	_suspendedServerName = nil;

	// close the connection
	[_connection close];
	_status = kPSConnectionStatus_Closed;
	
	// inform the delegates
	[self informConnectionClosedDelegates];
}

- (void) suspend
{
	// stop the browser
	[_browser stop];

	// remember the server name and host
	// actually the suspended server name was set in openWithServer()
	// because the name doesn't seem to be available at this point
	_suspendedHost = _host;
	
	// close the connection
	// that [self close] cannot be used because it
	// will clean up the suspended stuff
	if ([self isOpen] == YES)
	{
		[_connection close];
		[self informConnectionClosedDelegates];
	}
	
	// clean up
	_host = nil;
	_server = nil;
}

- (void) resume
{
	// start browsing again
	[_browser startForService:@"_photoshopserver._tcp" inDomain:@""];

	// if the host was suspended open a connection again
	// note that checking for a suspended server doesn't make sense
	// because the connection has just started browsing for net services
	// it will take a while to for the service to be found again
	if (_suspendedServerName == nil && _suspendedHost != nil)
	{
		// reopen the connection
		[self openWithHost:_suspendedHost password:_password];
		
		// clean up
		_suspendedHost = nil;
	}
}

- (BOOL) hasServers
{
	return (_browser != nil && _browser.servers.count > 0);
}

- (NSArray*) servers
{
	return _browser.servers;
}

- (BOOL) hasHosts
{
	return (_hosts.count > 0);
}

- (NSArray*) hosts
{
	return _hosts;
}

- (void) forgetHost:(NSString*)host
{
	// nothing to do if no hosts list
	if (_hosts == nil)
	{
		return;
	}
	
	// close the connection if this is the selected host
	if (_host != nil && [host compare:_host] == NSOrderedSame)
	{
		[self close];
	}
	
	// remove the host from the list
	[_hosts removeObject:host];
	
	// persist the change
	// this needs to be done EVERY time the _hosts
	// list is modified, note that openWithHost()
	// is where a host is added to the list
	[[NSUserDefaults standardUserDefaults] setObject:_hosts forKey:@"PSConnectionSavedHosts"];
}

- (void) forgetHostAtIndex:(NSUInteger)index
{
	// make sure the index is valid
	if (_hosts == nil || index >= _hosts.count)
	{
		return;
	}
	
	// close the connection if this is the selected host
	NSString* host = [_hosts objectAtIndex:index];
	if (_host != nil && [host compare:_host] == NSOrderedSame)
	{
		[self close];
	}
	
	// remove the index and persist
	[_hosts removeObjectAtIndex:index];
	[[NSUserDefaults standardUserDefaults] setObject:_hosts forKey:@"PSConnectionSavedHosts"];
}

- (NSUInteger) sendNetworkPacket:(PSNetworkPacket*)packet withData:(NSData*)data
{
	// always clear the communication status field
	packet->status = kPSNetworkPacket_StatusEncrypted;

	// add a transaction ID to the packet
	NSUInteger transaction = [self nextTransactionID];
	packet->transaction = transaction;

	// flip the packet bytes into network order
	[self flipNetworkByteOrder:packet];
	
	// use the connection's write buffer for the length, packet and data
	// this saves on memory and CPU since a copy is not made
	// note that using this buffer directly requires calling
	// sendNetworkDataFromWriteBuffer() to actually send the message
	NSUInteger packetAndDataLength = sizeof(PSNetworkPacket) + data.length;
	NSMutableData* buffer = [_connection writeBuffer];
	[buffer appendBytes:&packetAndDataLength length:sizeof(NSUInteger)];
	[buffer appendBytes:packet length:sizeof(PSNetworkPacket)];
	[buffer appendData:data];

	// the number of bytes to not encrypt starting from the front of the buffer
	// the buffer length and packet.status should not be encrypted
	NSUInteger numBytesToSkip = sizeof(NSUInteger) + sizeof(unsigned int);

	// figure out how much is going to be encrypted
	// that is the total length of the buffer minus the
	// status part of the packet
	NSUInteger length = buffer.length - numBytesToSkip;
	size_t encryptedLength = CryptorGetEncryptedLength((size_t)length);
	NSInteger numBytesToAdd = encryptedLength - length;
	if (numBytesToAdd > 0)
	{
		[buffer increaseLengthBy:(NSUInteger)numBytesToAdd];
	}
	
	// encrypt the packet but only everything after the status
	void* bytesToEncrypt = [buffer mutableBytes];
	bytesToEncrypt = (void*)(((char*) bytesToEncrypt) + numBytesToSkip);
	PSCryptorStatus status = EncryptDecrypt(_cryptor, TRUE, bytesToEncrypt, length,
											bytesToEncrypt, encryptedLength, &encryptedLength);

	// make sure the packet was encrypted properly
	if (status != kCryptorSuccess)
	{
		DebugLog(@"Failed packet encryption, not sending message");
		return kNoTransactionID;
	}
	
	// before sending off make sure the length at the beginning
	// of the buffer has the correct value in it i.e. the number
	// of bytes after the length bytes
	NSUInteger bufferLength = buffer.length;
	bufferLength -= sizeof(NSUInteger);
	*((NSUInteger*)[buffer mutableBytes]) = htonl(bufferLength);

	// since the connection's write buffer was used to load
	// the message, it can be sent immediately without any
	// additional processing
	[_connection sendNetworkDataFromWriteBuffer];
	
	// done
	return transaction;
}

- (NSUInteger) sendJavascript:(NSString*)script
{
	// make a packet header
	// note that the transaction ID will be set in sendNetworkPacket:withData
	// so it is not necessary to set here
	PSNetworkPacket packet;
	packet.version = kPSNetProtocolCurrentVersion;
	packet.type = kPSNetJavaScript;

	// get the script bytes
	NSData* data = [script dataUsingEncoding:NSUTF8StringEncoding];
	
	// done
	return [self sendNetworkPacket:&packet withData:data];
}

- (NSUInteger) sendImage:(UIImage*)image encoding:(PSNetImageEncoding)encoding
{
	// make a packet header
	PSNetworkPacket packet;
	packet.version = kPSNetProtocolCurrentVersion;
	packet.type = kPSNetImage;
	
	// based on the encoding
	// munge the image bytes down to an NSData
	NSData* data = nil;
	if (encoding == kPSNetImageEncoding_Raw)
	{
		// pre-fetch some image info
		size_t numBitsPerChannel = CGImageGetBitsPerComponent(image.CGImage);
		size_t numChannels = CGImageGetBitsPerPixel(image.CGImage) / numBitsPerChannel;
		size_t numBytesPerRow = CGImageGetBytesPerRow(image.CGImage);

		// allocate a buffer big enough for the image header and all image bytes
		// note that sizeof(PSRawImage) will return 20 due to padding and packing
		NSUInteger length = 16;
		length += image.size.width * image.size.height * numChannels;
		NSMutableData* imageData = [NSMutableData dataWithLength:length];
		void* headerBytes = [imageData mutableBytes];
		void* imageBytes = (void*)(((char*)([imageData mutableBytes])) + 16);

		// it would be nice to use the PSRawImage struct but sizeof() will
		// pad and pack to 4 byte boundaries, so the struct ends up incorrect
		// instead just use offsets into the buffer to set the various fields
		// it's not as pretty or as understandable, but it works
		char* headerChars = (char*)headerBytes;
		*headerChars = 2;			// pixmap
		*((unsigned int*)(headerChars + 1)) = htonl((unsigned int)image.size.width);
		*((unsigned int*)(headerChars + 5)) = htonl((unsigned int)image.size.height);
		*((unsigned int*)(headerChars + 9)) = htonl((unsigned int)numBytesPerRow);
		*(headerChars + 13) = 1;	// RGB mode
		*(headerChars + 14) = numChannels;
		*(headerChars + 15) = numBitsPerChannel;
		
		// create an RGBA context
		// note that creating a kCGImageAlphaNone never seems to work
		// it seems all images on iOS are RGBA no matter what
		CGColorSpaceRef colorspace = CGImageGetColorSpace(image.CGImage);
		CGContextRef context =  CGBitmapContextCreate(imageBytes,
													  image.size.width, 
													  image.size.height, 
													  numBitsPerChannel,
													  numBytesPerRow,
													  colorspace, 
													  kCGImageAlphaPremultipliedLast);
		UIGraphicsPushContext(context);
		CGContextTranslateCTM(context, 0.0, image.size.height);
		CGContextScaleCTM(context, 1.0, -1.0);
		[image drawInRect:CGRectMake(0.0, 0.0, image.size.width, image.size.height)];
		UIGraphicsPopContext();
		
		// clean up
		CGColorSpaceRelease(colorspace);
		CGContextRelease(context);

		// done
		data = imageData;
	}
	
	// JPG
	// note that is encoded as best quality
	else if (encoding == kPSNetImageEncoding_JPG)
	{
		unsigned char type = 1;
		NSMutableData* imageData = [[NSMutableData alloc] autorelease];
		[imageData appendBytes:(const void*)&type length:1];	// JPG
		[imageData appendData:UIImageJPEGRepresentation(image, 1.0)];
		data = imageData;
	}

	// make sure there is data to send
	if (data == nil)
	{
		DebugLog(@"Invalid encoding or unsupported bitmap format, not sending image");
		return kNoTransactionID;
	}
	
	// done
	return [self sendNetworkPacket:&packet withData:data];
}

- (NSUInteger) sendData:(NSData*)data
{
	// make a packet header
	// note that the transaction ID will be set in sendNetworkPacket:withData
	// so it is not necessary to set here
	PSNetworkPacket packet;
	packet.version = kPSNetProtocolCurrentVersion;
	packet.type = kPSNetDataBlob;
	
	// done
	return [self sendNetworkPacket:&packet withData:data];
}

// sendDataBlob is deprecated, please use sendData
// note that both are functionally identical, but sendData
// is fits better with the API
- (NSUInteger) sendDataBlob:(NSData*)data
{
	return [self sendData:data];
}

- (BOOL) hasPendingTransaction:(NSUInteger)transactionID
{
	// turn the ID into an NSNumber
	NSNumber* transactionNumber = [NSNumber numberWithUnsignedInt:transactionID];
	
	// try to find it in the transaction list
	return [_sentTransations containsObject:transactionNumber];
}

@end
