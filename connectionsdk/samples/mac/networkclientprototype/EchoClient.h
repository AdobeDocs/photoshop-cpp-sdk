/*
 File: EchoClientAppDelegate.m
 
 Abstract: Interface definitions for the GUI client portion of the CocoaEcho example.
 
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


#import <Cocoa/Cocoa.h>
#include <os/log.h>

/**************************************************************************/

enum image_format {
	jpeg_format=1,
	pixmap_format=2
};


enum image_mode {
	rgb_mode=1
};


// protocol definition bits
enum PSNetworkContentTypes {

	kPSNetIllegal = 0,
	kPSNetErrorString = 1,
	kPSNetJavaScript = 2,
	kPSNetImage = 3,
	kPSNetProfile = 4,
	kPSNetDataBlob = 5,
	kPSNetKeepAlive = 6,
	kPSNetFileStream = 7,
	kPSNetCancelCommand = 8,
	kPSNetEventStatus = 9
};

class PSCryptor;

/**************************************************************************/
typedef uint32_t transaction_id;

@protocol EchoClientDelegate
-(void) processErrorMessage:(NSString*)string transaction:(transaction_id) transaction;
-(void) processJavaScriptMessage:(NSString*)string transaction:(transaction_id) transaction;
-(void) processDataMessage:(NSData*)data transaction:(transaction_id) transaction;
-(void) processImageMessage:(NSImage*)image transaction:(transaction_id) transaction;
-(void) processFileMessage:(NSData*)data fileInfo:(NSString*)fileInfo transaction:(transaction_id) transaction;
-(void) connectionOpened;
-(void) connectionClosed;
-(BOOL) shouldSavePassword;
@end

/**************************************************************************/

@interface EchoClient : NSObject<NSNetServiceBrowserDelegate,NSStreamDelegate>
{
    NSNetServiceBrowser*	serviceBrowser;
    NSMutableArray*			serviceList;
	NSMutableIndexSet*		serviceSelections;
	
    NSInputStream*			inputStream;
    NSOutputStream*			outputStream;
    NSMutableData*			dataBuffer;
    PSCryptor               *psCryptor;
	NSNetService*			connectedService;

	size_t					packetBodySize;

	NSString*				password;
	NSString*				preferredServer;
	NSString*				logFilePath;
	id<EchoClientDelegate>  delegate;

}

@property(nonatomic, retain) id<EchoClientDelegate> delegate;
@property(nonatomic, retain) NSString* password;
@property(nonatomic, retain) NSString* logFilePath;
@property(nonatomic, retain) NSString* imageFilePath;
@property(nonatomic, retain) NSString* preferredServer;

+ (EchoClient*)get; //get the single EchoClient

- (void)setPassword: (NSString*) inPassword;
- (NSString*)password;
- (transaction_id)sendJavaScript:(NSString*)stringToSend;
- (transaction_id)sendImageMessage:(NSData*)dataToSend;
- (transaction_id)sendDataMessage:(NSData*)dataToSend;
- (transaction_id)sendCancelCommandMessage:(transaction_id)canceltransid;
- (transaction_id)getCurrentEventStatus;
- (transaction_id)subscribeEvent: (const char *)eventStr;
- (transaction_id)subscribeEvent:(const char*)eventStr withVersion:(const char*)versionStr;
- (transaction_id)unsubscribeEvent: (const char *)eventStr;
- (void)connectToNthService:(int)serviceIndex;
-(NSString*) makeLogFilePath: (NSString*)logFileName;

@end

/**************************************************************************/

template<typename... Args>
void LogInfo(const char* s, Args... args) {
    char str[512];
    snprintf(str, 512, s, args... );
    os_log_info(OS_LOG_DEFAULT, "%s", str);
}

template<typename... Args>
void LogError(const char* s,Args... args) {
    char str[512];
    snprintf(str, 512, s, args... );
    os_log_error(OS_LOG_DEFAULT, "%s", str);
}

inline void LogError(const char* s) {
    os_log_error(OS_LOG_DEFAULT, "%s", s);
}

inline void LogError(NSString* s) {
    LogError([s UTF8String]);
}

inline void LogInfo(const char* s) {
    os_log_info(OS_LOG_DEFAULT, "%s", s);
}

inline void LogInfo(NSString* s) {
    LogInfo([s UTF8String]);
}

/**************************************************************************/

/*for command-line batch test mode */

int TestOneFile(const char* inPath, const char* inTestProcsPath, double inTimeOutSeconds, bool inInit); //test a file in batch mode
bool IsTestTransaction(transaction_id inTransaction);

void StartTestMode();
void EndTestMode(); //marks transactions so we know they are coming from batch mode
