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
#import <EchoClient.h>


/**************************************************************************/
//UI delegate/controller when we are in GUI mode

@interface EchoClientAppDelegate : NSObject<EchoClientDelegate>
{
    IBOutlet id responseField;
    IBOutlet id passwordField;
    IBOutlet id serverTableView;
    IBOutlet id foregroundColorWidget;
    IBOutlet id backgroundColorWidget;
    IBOutlet id imageWidget;
    IBOutlet id connectionStatusWidget;
    IBOutlet id savePasswordInKeychainCheckbox;
    IBOutlet id interpolationMethod;
    IBOutlet id previewType;
    IBOutlet id compsMenu;

    IBOutlet id thumbnailDimensions;
    IBOutlet id thumbnailXScaleFactorText;
    IBOutlet id thumbnailYScaleFactorText;
    IBOutlet id thumbnailScaleLinked;

    IBOutlet id curEventID;
    IBOutlet id curEventName;
    IBOutlet id curEventTime;

    IBOutlet id cancelNetworkCommand;

    IBOutlet id getAllTextStyleInfoMenuItem;
    IBOutlet id getDefaultLayerFXMenuItem;

    IBOutlet id subscribeChangesMenuItem;

    IBOutlet id clipLayerThumbsToDocMenuItem;
    IBOutlet id forceDumbScalingMenuItem;
    IBOutlet id forceSmartPSDPixelScalingMenuItem;
    IBOutlet id useRectScalingMenuItem;
    IBOutlet id forceFXScalingMenuItem;

    IBOutlet id includeAdjustorsMenuItem;
    IBOutlet id includeAncestorsMenuItem;
    IBOutlet id includeClippedMenuItem;
    IBOutlet id includeChildrenMenuItem;
    IBOutlet id includeClipBaseMenuItem;

    IBOutlet id generatorMenusAddedMenuItem;
    IBOutlet id generatorMenusEnabledMenuItem;
    IBOutlet id generatorMenuItem1;
    IBOutlet id generatorMenuItem2;

    IBOutlet id pollActiveEventMenuItem;
}

- (IBAction)setPassword:(id)sender;
- (IBAction)getColorPressed:(id)sender;
- (IBAction)invertPressed:(id)sender;
- (IBAction)scriptErrorPressed:(id)sender;
- (IBAction)sendJPEGPressed:(id)sender;
- (IBAction)sendPixmapPressed:(id)sender;
- (IBAction)sendInfoPressed:(id)sender;
- (IBAction)saveICCProfile:(id)sender;
- (IBAction)sendLayerInfoPressed:(id)sender;
- (IBAction)sendBlobPressed:(id)sender;
- (IBAction)sendHighASCIIPressed:(id)sender;
- (IBAction)serverClicked:(id)sender;
- (IBAction)interpolationChanged:(id)sender;
- (IBAction)previewTypeChanged:(id)sender;
- (IBAction)scaleFactorTextChanged:(id)sender;
- (IBAction)togglePSMenuState:(id)sender;
- (IBAction)releaseSmartObjects:(id)sender;

- (IBAction)toggleMenuItem:(id)sender;
- (IBAction)toggleChangeSubscription:(id)sender;

- (IBAction)sendUserJS:(id)sender;

- (IBAction)openChosenFile:(id)sender;
- (IBAction)openTestFile:(id)sender;
- (IBAction)runChosenTests:(id)sender;
- (IBAction)runChosenFiles:(id)sender;
- (IBAction)runChosenTestsAndFiles:(id)sender;
- (IBAction)runDefaultTest:(id)sender;

- (IBAction)setDocGeneratorSettings:(id)sender;
- (IBAction)getDocGeneratorSettings:(id)sender;
- (IBAction)getDocJSON:(id)sender;

- (IBAction)setLayerGeneratorSettings:(id)sender;
- (IBAction)getLayerGeneratorSettings:(id)sender;
- (IBAction)getLayerJSON:(id)sender;

- (IBAction)addGeneratorMenus:(id)sender;
- (IBAction)cancelCurrentNetworkCommand:(id)sender;

@end
