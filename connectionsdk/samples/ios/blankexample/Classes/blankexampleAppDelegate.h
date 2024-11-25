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
//  blankexampleAppDelegate.h
//  blankexample
//
//  Created by Allen Jeng on 4/4/11.
//  Copyright 2011 Adobe Systems. All rights reserved.
//

#import <UIKit/UIKit.h>

@class blankexampleViewController;

@interface blankexampleAppDelegate : NSObject <UIApplicationDelegate> {
    UIWindow *window;
    blankexampleViewController *viewController;
}

@property (nonatomic, retain) IBOutlet UIWindow *window;
@property (nonatomic, retain) IBOutlet blankexampleViewController *viewController;

@end

