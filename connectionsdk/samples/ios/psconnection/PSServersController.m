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
//  PSServersController.m
//  PSTools
//
//  Created by Christoph on 11/9/10.
//  Copyright 2010 Adobe Systems Inc. All rights reserved.
//

#import "PSConnection.h"
#import "PSServersController.h"
#import "SFHFKeychainUtils.h"

@implementation PSServersController

/**********************************************************************************
* Class synthesizication
**********************************************************************************/

@synthesize popoverParentController;
@synthesize popoverController;
@synthesize popoverPresentRect;
@synthesize navController;
@synthesize tableController;
@synthesize helpView;
@synthesize helpViewBoldLabel;
@synthesize helpViewLightLabel;
@synthesize passwordController;
@synthesize passwordHelpLabel;
@synthesize passwordField;
@synthesize connectButton;
@synthesize otherController;
@synthesize otherHelpLabel;
@synthesize otherHostnameField;
@synthesize otherPasswordField;
@synthesize otherConnectButton;
@synthesize helpController;
@synthesize htmlController;
@synthesize activityView;

/**********************************************************************************
* Class overload methods
**********************************************************************************/

- (id) initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil
{
	if ((self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil]) != nil)
	{
		navController = nil;
		selectedServer = nil;
		selectedHost = nil;
		hasLoadedHelp = NO;
	}
	
	return self;
}

- (void) dealloc
{
	selectedServer = nil;
	selectedHost = nil;
	
	popoverController = nil;

    [super dealloc];
}

- (void) viewDidLoad
{
    [super viewDidLoad];

	// sign up for textfield changes
	[passwordField addTarget:self action:@selector(textFieldDidChange:) forControlEvents:UIControlEventEditingChanged];
	[otherHostnameField addTarget:self action:@selector(textFieldDidChange:) forControlEvents:UIControlEventEditingChanged];
	[otherPasswordField addTarget:self action:@selector(textFieldDidChange:) forControlEvents:UIControlEventEditingChanged];

	// add the nav controller to the parent view
	// note that this uses the self.view bounds as the nav controller's frame
	// this is a slick way to stuff the controller into a view
	[self.view addSubview:navController.view];
	navController.view.frame = self.view.bounds;
	
	// localize the help view
	helpViewBoldLabel.text = NSLocalizedString(@"It appears you have not set up Remote Connections in Photoshop CS5.",
											   @"$$$/PSConnection/HaveNotSetUpRemoteConnections=It appears you have not set up Remote Connections in Photoshop CS5.");
	helpViewLightLabel.text = NSLocalizedString(@"To learn how, tap Help above.",
												@"$$$/PSConnection/ToLearnHow=To learn how, tap Help above.");
	
	// localize the table's title and items
	// note that the table is the root controller as
	// specified by the the PSServersController.xib
	navController.topViewController.navigationItem.title = NSLocalizedString(@"Connections", @"$$$/PSConnection/Connections=Connections");
	navController.topViewController.navigationItem.leftBarButtonItem.title = NSLocalizedString(@"New", @"$$$/PSConnection/New=New");
	navController.topViewController.navigationItem.rightBarButtonItem.title = NSLocalizedString(@"Help", @"$$$/PSConnection/Help=Help");

	// localize the other controller
	otherController.navigationItem.title = NSLocalizedString(@"New", @"$$$/PSConnection/New=New");
	otherHostnameField.placeholder = NSLocalizedString(@"Hostname or IP Address", @"$$$/PSConnection/HostnameOrIPAddress=Hostname or IP Address");
	otherPasswordField.placeholder = NSLocalizedString(@"Password", @"$$$/PSConnection/Password=Password");
	[otherConnectButton setTitle:NSLocalizedString(@"Connect", @"$$$/PSConnection/Connect=Connect") forState:UIControlStateNormal];
	
	// localize the password controller
	passwordField.placeholder = otherPasswordField.placeholder;
	[connectButton setTitle:NSLocalizedString(@"Connect", @"$$$/PSConnection/Connect=Connect") forState:UIControlStateNormal];
	
	// localize the help fields
	passwordHelpLabel.text = NSLocalizedString(@"Enter the password found at Edit > Remote Connections in Adobe® Photoshop® CS5 or later.",
											   @"$$$/PSConnection/EnterThePassword=Enter the hostname or IP address found at Edit > Remote Connections in Adobe® Photoshop® CS5 or later.");
	otherHelpLabel.text = NSLocalizedString(@"Enter the hostname or IP address found at Edit > Remote Connections in Adobe® Photoshop® CS5 or later.",
											@"$$$/PSConnection/EnterTheHostname=Enter the hostname or IP address found at Edit > Remote Connections in Adobe® Photoshop® CS5 or later.");

	// localize the help controllers
	helpController.navigationItem.title = NSLocalizedString(@"Help", @"$$$/PSConnection/Help=Help");
	
	// now that everything has been layed out
	// bring the help view above the table view
	//[self.view bringSubviewToFront:helpView];
}

- (void) viewDidUnload
{
    [super viewDidUnload];

	[navController.view removeFromSuperview];
}

- (void) viewWillAppear:(BOOL)animated
{
	[super viewWillAppear:animated];
	
	// init the button state
	otherConnectButton.enabled = NO;
	
	// scroll to the correct row
}

- (void) didReceiveMemoryWarning
{
    // Releases the view if it doesn't have a superview.
    [super didReceiveMemoryWarning];
    
    // Release any cached data, images, etc that aren't in use.
}

/**********************************************************************************
* Class delegate methods
**********************************************************************************/

- (NSInteger) tableView:(UITableView*)tableView numberOfRowsInSection:(NSInteger)section
{
	// get all the hosts and servers
	NSArray* servers = [[PSConnection sharedPSConnection] servers];
	NSArray* hosts = [[PSConnection sharedPSConnection] hosts];

	// show/hide the help view depending on the number of servers and hosts
	tableView.hidden = (servers.count == 0 && hosts.count == 0);

	// hosts always go in the second section
	// or if no servers then hosts go in the first section
	if (section == 1 ||
	   (section == 0 && servers.count == 0))
	{
		return hosts.count;
	}
	
	// if no hosts then servers go in the first section
	if (section == 0 && hosts.count == 0)
	{
		return servers.count;
	}
	
	// if this far there are both hosts and servers
	// but the servers always get the first section
	return servers.count;
}

- (UITableViewCell*) tableView:(UITableView*)tableView cellForRowAtIndexPath:(NSIndexPath*)indexPath
{
	// try to re-use an existing cell
	UITableViewCell* cell = [tableView dequeueReusableCellWithIdentifier:@"PSServersTableViewCell"];
	
	// if the cell is nil
	// create a new one
	if (cell == nil)
	{
		cell = [[UITableViewCell alloc] initWithStyle:UITableViewCellStyleSubtitle reuseIdentifier:@"PSServersTableViewCell"];
		[cell autorelease];
	}
	
	// the name of the server or host
	// this can come from either an NSNetService or a hostname NSString
	NSString* name = nil;
	NSString* detail = nil;
	PSConnectionStatus status = kPSConnectionStatus_Closed;
	
	// if the section is 1 or 0 and no servers
	// then use the hosts list
	if (indexPath.section == 1 ||
	   (indexPath.section == 0 && [[PSConnection sharedPSConnection] hasServers] == NO))
	{
		NSArray* hosts = [[PSConnection sharedPSConnection] hosts];
		name = [hosts objectAtIndex:indexPath.row];
		status = [[PSConnection sharedPSConnection] statusToHost:name];
	}

	// otherwise use the name of the server
	else
	{
		NSArray* servers = [[PSConnection sharedPSConnection] servers];
		NSNetService* server = [servers objectAtIndex:indexPath.row];
		name = [server name];
		status = [[PSConnection sharedPSConnection] statusToServer:server];
	}

	// add the server name
	cell.textLabel.text = name;
	
	// add the server detail
	cell.detailTextLabel.text = detail;
	
	// activity indicator for opening connection
	if (status == kPSConnectionStatus_Connecting)
	{
		UIActivityIndicatorView* view = [[UIActivityIndicatorView alloc] initWithActivityIndicatorStyle:UIActivityIndicatorViewStyleGray];
		[view startAnimating];
		cell.accessoryView = view;
		[view release];
	}

	// checkmark accessory for connected
	else if (status == kPSConnectionStatus_Open)
	{
		cell.accessoryView = nil;
		cell.accessoryType = UITableViewCellAccessoryCheckmark;
	}

	// no connection means no accessory
	else
	{
		cell.accessoryView = nil;
		cell.accessoryType = UITableViewCellAccessoryNone;
	}
	
	// done
	return cell;
}

- (NSInteger) numberOfSectionsInTableView:(UITableView*)tableView
{
	// if there are both servers and hosts
	// then there are two sections
	if ([[PSConnection sharedPSConnection] hasServers] && [[PSConnection sharedPSConnection] hasHosts])
	{
		return 2;
	}
	
	// otherwise there is only one section
	else
	{
		return 1;
	}
}

- (UIView*)tableView:(UITableView*)tableView viewForHeaderInSection:(NSInteger)section
{
	// figure out how many sections there area
	// if there is only one section then nothing to do
	NSInteger numSections = [self numberOfSectionsInTableView:tableView];
	if (numSections == 1)
	{
		return nil;
	}

	// the black label we are using
	// use the width of the table view to determine the label width
	CGFloat labelWidth = tableView.bounds.size.width;
	UILabel* label = [[UILabel alloc] initWithFrame:CGRectMake(0, 0, labelWidth, 40)];
	[label autorelease];
	label.alpha = 0.9;
	label.backgroundColor = [UIColor darkGrayColor];
	label.textColor = [UIColor lightGrayColor];
	label.font = [UIFont boldSystemFontOfSize:12.0];
	label.textAlignment = UITextAlignmentCenter;

	// hosts always in second section
	if (section == 1)
	{
		label.text = NSLocalizedString(@"Other", "$$$/PSConnection/Other=Other");
	}
	
	// this will only return Bonjour if there are hosts
	// since the Bonjour servers are listed first
	else if (section == 0 && numSections == 2)
	{
		label.text = NSLocalizedString(@"Discovered", "$$$/PSConnection/Discovered=Discovered");
	}
	
	// done
	return label;
}

- (BOOL) tableView:(UITableView*)tableView canEditRowAtIndexPath:(NSIndexPath*)indexPath
{
	// if the section is 1
	// or if the section is 0 and there are no servers
	BOOL canEdit = (indexPath.section == 1 ||
				   (indexPath.section == 0 && [[PSConnection sharedPSConnection] hasServers] == NO));
	return canEdit;
}

- (void) tableView:(UITableView*)tableView commitEditingStyle:(UITableViewCellEditingStyle)editingStyle forRowAtIndexPath:(NSIndexPath*)indexPath
{
	// make sure the row is part of the connection's hosts list
	// if there are two sections or a single section and no servers
	if ([[PSConnection sharedPSConnection] hasServers] && indexPath.section == 0)
	{
		return;
	}

	// remove the stored password for the server
	// it's a little sneaky to get the name from the cell
	UITableViewCell* cell = [tableView cellForRowAtIndexPath:indexPath];
	NSString* serviceName = cell.textLabel.text;
	NSError* error = nil;
	[SFHFKeychainUtils deleteItemForUsername:serviceName andServiceName:serviceName error:&error];
	if (error != nil)
	{
		DebugLog(@"Could not delete password: %d - %@", [error code], [error localizedDescription]);
	}

	// if there is only one host
	// then removing it will cause the number of sections to change
	// which means the whole table has to be reloaded
	if ([[[PSConnection sharedPSConnection] hosts] count] == 1)
	{
		[[PSConnection sharedPSConnection] forgetHostAtIndex:indexPath.row];
		[tableController.tableView reloadData];
		return;
	}

	// update the table at that index
	// note that the datasource updating needs to be done
	// during the begin/endUpdates block
	[tableController.tableView beginUpdates];
	[[PSConnection sharedPSConnection] forgetHostAtIndex:indexPath.row];
	NSArray* indexPaths = [NSArray arrayWithObject:indexPath];
	[tableController.tableView deleteRowsAtIndexPaths:indexPaths withRowAnimation:UITableViewRowAnimationMiddle];
	[tableController.tableView endUpdates];
}

- (void) tableView:(UITableView*)tableView didSelectRowAtIndexPath:(NSIndexPath*)indexPath
{
	// always deselect the row
	// the selected state is shown by the checkbox
	[tableView deselectRowAtIndexPath:indexPath animated:NO];
	
	// if the section is 1 or 0 and no servers
	// then use the hosts list
	// note that if using a host, there is no selected server
	if (indexPath.section == 1 ||
	   (indexPath.section == 0 && [[PSConnection sharedPSConnection] hasServers] == NO))
	{
		NSArray* hosts = [[PSConnection sharedPSConnection] hosts];
		selectedHost = [hosts objectAtIndex:indexPath.row];
		selectedServer = nil;
	}

	// otherwise use the server directly
	// note that if using a server there is no selected host
	else
	{
		// the server is retained to be later passed into
		// openWithServer(), however it may go away between
		// now and then so it is retained to prevent a dealloc
		// in connectButtonUp() it will be released regardless
		// of whether the connection was successful or not
		NSArray* servers = [[PSConnection sharedPSConnection] servers];
		selectedServer = [servers objectAtIndex:indexPath.row];
		[selectedServer retain];
		selectedHost = nil;
	}
	
	// if currently connected to that server
	// disconnect and return
	if ([[PSConnection sharedPSConnection] statusToServer:selectedServer] == kPSConnectionStatus_Open ||
		[[PSConnection sharedPSConnection] statusToHost:selectedHost] == kPSConnectionStatus_Open)
	{
		selectedServer = nil;
		selectedHost = nil;
		[[PSConnection sharedPSConnection] close];
		UITableViewCell* cell = [tableView cellForRowAtIndexPath:indexPath];
		cell.accessoryView = nil;
		cell.accessoryType = UITableViewCellAccessoryNone;
		return;
	}

	// show the password controller
	[navController pushViewController:passwordController animated:YES];
}

- (UITableViewCellEditingStyle) tableView:(UITableView*)tableView editingStyleForRowAtIndexPath:(NSIndexPath*)indexPath
{
	return UITableViewCellEditingStyleDelete;
}

- (NSString*) tableView:(UITableView*)tableView titleForDeleteConfirmationButtonForRowAtIndexPath:(NSIndexPath*)indexPath
{
	return NSLocalizedString(@"Forget", "$$$/PSConnection/Forget=Forget");
}

- (BOOL) textFieldShouldReturn:(UITextField*)textField
{
	// handle the other hostname field
	if (textField == otherHostnameField)
	{
		[self otherConnectButtonUp:otherConnectButton];
		return YES;
	}
	
	// handle the other password field
	if (textField == otherPasswordField)
	{
		[self otherConnectButtonUp:otherConnectButton];
		return YES;
	}
	
	// handle the  password field
	if (textField == passwordField)
	{
		[self connectButtonUp:connectButton];
		return YES;
	}
	
	// if this far
	// then nothing to do
	return NO;
}

- (void)navigationController:(UINavigationController*)navigationController willShowViewController:(UIViewController*)viewController animated:(BOOL)animated
{
	if (viewController == otherController)
	{
		otherHostnameField.text = nil;
		otherPasswordField.text = nil;
		otherConnectButton.enabled = NO;
	}
	
	else if (viewController == passwordController)
	{
		NSString* serviceName = (selectedServer != nil ? selectedServer.name : selectedHost);
		NSError* error = nil;
		NSString* password = [SFHFKeychainUtils getPasswordForUsername:serviceName andServiceName:serviceName error:&error];
		passwordField.text = password;
		connectButton.enabled = (passwordField.text != nil);
	}
}

- (void)navigationController:(UINavigationController*)navigationController didShowViewController:(UIViewController*)viewController animated:(BOOL)animated
{
	if (viewController == helpController)
	{
		// if the webview doesn't have a URL
		// then the help hasn't been loaded yet
		if (htmlController.request.URL == nil)
		{
			// look up the current language and build a path to that
			// resource in the app bundle
			NSString* language = [[NSLocale preferredLanguages] objectAtIndex:0];
			NSString* resourcePath = [NSString stringWithFormat:@"%@/help/%@", [[NSBundle mainBundle] resourcePath], language];
			
			// check if that localized resources exists in the bundle
			// if not default to use the EN help files
			if ([[NSFileManager defaultManager] fileExistsAtPath:resourcePath] == NO)
			{
				resourcePath = [NSString stringWithFormat:@"%@/help/en", [[NSBundle mainBundle] resourcePath]];
			}
			
			// at this point the resource path should be valid
			// so create a URL to it, set the base URL and load the html
			NSURL* baseURL = [[NSURL alloc] initFileURLWithPath:resourcePath isDirectory:YES];
			NSString* htmlPath = [NSString stringWithFormat:@"%@/KVLRHelp.html", resourcePath];
			NSString* html = [NSString stringWithContentsOfFile:htmlPath encoding:NSUTF8StringEncoding error:nil];
			[htmlController loadHTMLString:html baseURL:baseURL];
		}
		
		// otherwise if it does have a URL
		// the help has been loaded and the controller
		// just needs to be resized
		else
		{
			[self webViewDidFinishLoad:htmlController];
		}
	}
	
	else if (viewController == tableController)
	{
		[popoverController setPopoverContentSize:CGSizeMake(320, 320) animated:animated];
		[popoverController presentPopoverFromRect:popoverPresentRect inView:popoverParentController.view
						 permittedArrowDirections:UIPopoverArrowDirectionAny animated:YES];
	}
	
	else if (viewController == passwordController)
	{
		if (passwordField.text == nil)
		{
			[passwordField becomeFirstResponder];
		}
	}
	
	else if (viewController == otherController)
	{
		[otherHostnameField becomeFirstResponder];
	}
}

- (BOOL) webView:(UIWebView*)webView shouldStartLoadWithRequest:(NSURLRequest*)request navigationType:(UIWebViewNavigationType)navigationType
{
	// if the type is other
	// then this is probably the load help html request
	if (navigationType != UIWebViewNavigationTypeOther)
	{
		[[UIApplication sharedApplication] openURL:[request URL]];
		return NO;
	}

	//done
	return YES;
}

- (void)webViewDidStartLoad:(UIWebView *)webView
{
	if (hasLoadedHelp == NO)
	{
		[activityView startAnimating];
	}
}

- (void)webViewDidFinishLoad:(UIWebView *)webView
{
	hasLoadedHelp = YES;
	[activityView stopAnimating];
	[popoverController setPopoverContentSize:CGSizeMake(480, 512) animated:YES];
}

/**********************************************************************************
* Class events
**********************************************************************************/

- (IBAction) connectButtonUp:(id)sender
{
	// close any open connection
	[[PSConnection sharedPSConnection] close];

	// connect to the server or host
	if (selectedHost != nil && selectedServer == nil)
	{
		[[PSConnection sharedPSConnection] openWithHost:selectedHost password:passwordField.text];
	}
	else if (selectedHost == nil && selectedServer != nil)
	{
		[[PSConnection sharedPSConnection] openWithServer:selectedServer password:passwordField.text];
		[selectedServer release];
	}

	// pop the password controller off the stack
	[navController popViewControllerAnimated:YES];
	
	// reload the table
	// this should cause the checkbox for the
	// selected server's row to update correctly
	[tableController.tableView reloadData];
}

- (IBAction) otherConnectButtonUp:(id)sender
{
	// connect to it
	[[PSConnection sharedPSConnection] openWithHost:otherHostnameField.text password:otherPasswordField.text];
	
	// reload the table to pick up the new server
	[tableController.tableView reloadData];
	
	// figure out the index of the newly added host
	NSUInteger hostIndex = [[[PSConnection sharedPSConnection] hosts] indexOfObject:otherHostnameField.text];
	if (hostIndex != NSNotFound)
	{
		NSUInteger section = [self numberOfSectionsInTableView:tableController.tableView] - 1;
		NSIndexPath* hostIndexPath = [NSIndexPath indexPathForRow:hostIndex inSection:section];
		[tableController.tableView scrollToRowAtIndexPath:hostIndexPath atScrollPosition:UITableViewScrollPositionMiddle animated:NO];
	}
	
	// pop the controller off the stack
	[navController popViewControllerAnimated:YES];
}

- (IBAction) otherBarButtonItemUp
{
	[navController pushViewController:otherController animated:YES];
}

- (IBAction) helpBarButtonItemUp
{
	[navController pushViewController:helpController animated:YES];
}

- (void) textFieldDidChange:(UITextField*)textfield
{
	if (textfield == otherHostnameField || textfield == otherPasswordField)
	{
		BOOL enabled = (otherHostnameField.text != nil && otherHostnameField.text.length > 0 &&
						otherPasswordField.text != nil && otherPasswordField.text.length > 0);
		otherConnectButton.enabled = enabled;
	}

	if (textfield == passwordField)
	{
		BOOL enabled = passwordField.text != nil && passwordField.text.length > 0;
		connectButton.enabled = enabled;
	}
}

/**********************************************************************************
* Class public methods
**********************************************************************************/

@end
