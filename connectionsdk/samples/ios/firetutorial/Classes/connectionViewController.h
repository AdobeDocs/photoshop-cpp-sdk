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
//  connectionViewController.h
//  transmitimage
//
//  @author Allen Jeng, Photoshop QE Developer, Adobe Systems Inc.
//  Copyright 2011 Adobe Systems. All rights reserved.
//

#import <UIKit/UIKit.h>

@protocol connectionDelegate
-(void) connectionSelected:(NSArray *) values;
@end

@interface connectionViewController : UIViewController
{
	id<connectionDelegate> delegate;

	UITextField *serverName;
	UITextField *serverPassword;
	
	NSString *serviceName;
	BOOL isUserDefined;
}

@property (nonatomic, assign) id<connectionDelegate> delegate;

@property (nonatomic, retain) IBOutlet UITextField *serverName;
@property (nonatomic, retain) IBOutlet UITextField *serverPassword;

@property (nonatomic, retain) NSString *serviceName;
@property (nonatomic, assign) BOOL isUserDefined;

-(void) updateInfo;

@end
