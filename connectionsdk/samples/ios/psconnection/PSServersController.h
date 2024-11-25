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
//  PSServersController.h
//  PSTools
//
//  Created by Christoph on 11/9/10.
//  Copyright 2010 Adobe Systems Inc. All rights reserved.
//

#import <UIKit/UIKit.h>

/**********************************************************************************
* Interface declaration
**********************************************************************************/

@interface PSServersController : UIViewController <UITableViewDataSource,
												   UITableViewDelegate,
												   UITextFieldDelegate,
												   UINavigationControllerDelegate,
												   UIWebViewDelegate>
{
	UINavigationController* navController;
	
	NSNetService*	selectedServer;
	NSString*		selectedHost;

	BOOL	hasLoadedHelp;
}

@property (nonatomic, retain) UIViewController* popoverParentController;
@property (nonatomic, retain) UIPopoverController* popoverController;
@property (assign) CGRect popoverPresentRect;
@property (nonatomic, retain) IBOutlet UINavigationController* navController;
@property (nonatomic, retain) IBOutlet UITableViewController* tableController;
@property (nonatomic, retain) IBOutlet UIView* helpView;
@property (nonatomic, retain) IBOutlet UILabel* helpViewBoldLabel;
@property (nonatomic, retain) IBOutlet UILabel* helpViewLightLabel;
@property (nonatomic, retain) IBOutlet UIViewController* passwordController;
@property (nonatomic, retain) IBOutlet UILabel* passwordHelpLabel;
@property (nonatomic, retain) IBOutlet UITextField* passwordField;
@property (nonatomic, retain) IBOutlet UIButton* connectButton;
@property (nonatomic, retain) IBOutlet UIViewController* otherController;
@property (nonatomic, retain) IBOutlet UILabel* otherHelpLabel;
@property (nonatomic, retain) IBOutlet UITextField* otherHostnameField;
@property (nonatomic, retain) IBOutlet UITextField* otherPasswordField;
@property (nonatomic, retain) IBOutlet UIButton* otherConnectButton;
@property (nonatomic, retain) IBOutlet UIViewController* helpController;
@property (nonatomic, retain) IBOutlet UIWebView* htmlController;
@property (nonatomic, retain) IBOutlet UIActivityIndicatorView* activityView;

- (IBAction) connectButtonUp:(id)sender;
- (IBAction) otherConnectButtonUp:(id)sender;
- (IBAction) otherBarButtonItemUp;
- (IBAction) helpBarButtonItemUp;

- (void) textFieldDidChange:(UITextField*)textfield;

@end
