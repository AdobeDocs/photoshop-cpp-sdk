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

#import "NetConnection.h"
#import <Foundation/Foundation.h>
#import "PSCryptorAPI.h"
#import "NetServer.h"
#import "NetServerBrowserDelegate.h"
#import "NetServerBrowser.h"

// PS server port as defined in PS 
#define kPhotoshopServerPort 49494

// constants for possible communcation status
#define kPSNetworkPacket_StatusEncrypted 0x00000000
#define kPSNetworkPacket_StatusError 0x00000001

// invalid or nil transaction ID
#define kNoTransactionID 0
#define kErrorTransactionID 0

#define kPSNetProtocolCurrentVersion 1
//const int kPSNetProtocolBaseVersion = 1;
//const int kPSNetProtocolCurrentVersion = 1;

// max is when old failed transactions are thrown away
// min is what the list length is reduced when the max is hit
#define kMinFailedTransactions 20
#define kMaxFailedTransactions 50

#ifdef DEBUG
#	define DebugLog(fmt, ...) NSLog((@"%s [Line %d] " fmt), __PRETTY_FUNCTION__, __LINE__, ##__VA_ARGS__);
#else
#	define DebugLog(...)
#endif

typedef enum
{
	kPSNetIllegal = 0,
	kPSNetErrorString = 1,
	kPSNetJavaScript = 2,
	kPSNetImage = 3,
	kPSNetProfile = 4,
	kPSNetDataBlob = 5
} PSNetworkContentTypes;

typedef enum
{
	kPSNetImageEncoding_Raw = 0,
	kPSNetImageEncoding_JPG
} PSNetImageEncoding;

typedef struct PSNetworkPacket_s
{
	unsigned int status;
	unsigned int version;
	unsigned int transaction;
	unsigned int type;
} PSNetworkPacket;

// note that this while the struct defines the
// the raw image header, but it is not explicitly
// used in the connection code, it's really just
// for visualizing the structure
typedef struct PSRawImage_s
{
	unsigned char format;
	unsigned int width;
	unsigned int height;
	unsigned int rowBytes;
	unsigned char mode;
	unsigned char channels;
	unsigned char bitsPerChannel; 
} PSRawImage;

typedef enum
{
	kPSConnectionError_None,
	kPSConnectionError_BadPassword,
	kPSConnectionError_TimeOut,
	kPSConnectionError_NotFound,
	kPSConnectionError_Unknown
} PSConnectionError;

typedef enum
{
	kPSConnectionStatus_Closed,
	kPSConnectionStatus_Connecting,
	kPSConnectionStatus_Open,
	kPSConnectionStatus_Suspended
} PSConnectionStatus;

@class PSConnection;

@protocol PSConnectionDelegate <NSObject>

- (void) connectionOpened:(PSConnection*)connection;
- (void) connectionClosed:(PSConnection*)connection;

@optional

// detailed connection events
- (void) connectionStarted:(PSConnection*)connection;
- (void) connectionFailed:(PSConnection*)connection withError:(PSConnectionError)error;
- (void) connectionServersUpdated:(PSConnection*)connection;

// received message callbacks
- (void) connection:(PSConnection*)connection received:(NSUInteger)transaction error:(NSString*)error;
- (void) connection:(PSConnection*)connection received:(NSUInteger)transaction javascript:(NSString*)script;
- (void) connection:(PSConnection*)connection received:(NSUInteger)transaction image:(UIImage*)image;

@end

@interface PSConnection : NSObject <NetServerBrowserDelegate,
									NetConnectionDelegate>
{
	NSMutableArray*	_delegates;

	NSNetService*	_server;
	NSString*		_host;
	NSString*		_password;

	NSString*		_suspendedServerName;
	NSString*		_suspendedHost;
	NSString*		_suspendedPassword;

	NetConnection*		_connection;
	PSConnectionStatus	_status;

	NetServerBrowser*	_browser;

	NSMutableArray*	_hosts;

	NSUInteger				_transactionID;
	NSMutableArray*			_sentTransations;
	NSMutableDictionary*	_failedTransactions;
	
	PSCryptorRef	_cryptor;
	
	NSTimer*	_connectingTimer;
}

@property (readonly) NSArray* delegates;
@property (assign) BOOL createNSDataForReceivedImages;

+ (PSConnection*) sharedPSConnection;

+ (NSString*) stringForError:(PSConnectionError)error;

- (void) addDelegate:(id)delegate;
- (void) removeDelegate:(id)delegate;

- (BOOL) isOpen;
- (BOOL) isOpenToServer:(NSNetService*)server;
- (BOOL) isOpenToHost:(NSString*)host;

- (PSConnectionStatus) statusToServer:(NSNetService*)server;
- (PSConnectionStatus) statusToHost:(NSString*)host;
- (PSConnectionStatus) status;

- (void) openWithHost:(NSString*)host password:(NSString*)password;
- (void) openWithServer:(NSNetService*)server password:(NSString*)password;

- (void) close;

- (void) suspend;
- (void) resume;

- (BOOL) hasServers;
- (NSArray*) servers;

- (BOOL) hasHosts;
- (NSArray*) hosts;

- (void) forgetHost:(NSString*)host;
- (void) forgetHostAtIndex:(NSUInteger)index;

- (NSUInteger) sendNetworkPacket:(PSNetworkPacket*)packet withData:(NSData*)data;
- (NSUInteger) sendJavascript:(NSString*)script;
- (NSUInteger) sendImage:(UIImage*)image encoding:(PSNetImageEncoding)encoding;
- (NSUInteger) sendData:(NSData*)data;

// sendDataBlob is deprecated, please use sendData
- (NSUInteger) sendDataBlob:(NSData*)data;

- (BOOL) hasPendingTransaction:(NSUInteger)transactionID;

@end
