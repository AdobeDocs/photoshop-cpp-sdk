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
//  RootViewController.h
//  firetutorial
//
//  @author Allen Jeng, Photoshop QE Developer, Adobe Systems Inc.
//  Copyright 2011 Adobe Systems Inc. All rights reserved.
//

#import <UIKit/UIKit.h>

@class DetailViewController;

@interface RootViewController : UITableViewController {
    DetailViewController *detailViewController;
	
	NSArray *menuList;
}

@property (nonatomic, retain) IBOutlet DetailViewController *detailViewController;

@end
