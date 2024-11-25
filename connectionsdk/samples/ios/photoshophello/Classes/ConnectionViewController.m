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
//  ConnectionViewController.m
//  HelloWorld
//
//  @author Allen Jeng, Photoshop QE Developer, Adobe Systems Inc.
//  Copyright 2010 Adobe Systems. All rights reserved.
//

// hard coded service name for Hello World test app
#define kNameOfService "_photoshopserver._tcp."

#import "ConnectionViewController.h"


@implementation ConnectionViewController

@synthesize serverBroadcastName, serverName, serverPassword, delegate;
@synthesize serviceBrowser, serviceList, selectedService, myTableView, busyView;

/**
  * we're delegate for table view, so we need to tell it how many rows
  * @param tableView the table view object requesting this info
  * @param section index number identifying a section in tableView
  * @return the number of rows in section
  */
- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section
{
	return serviceList.count;
}

/**
  * fill the table view (need to happen for every cell
  */
- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath
{
	// ask for availablity -- reuse
	UITableViewCell *cell = [tableView dequeueReusableCellWithIdentifier:@"cell"];
	
	// create a cell
	if (cell == nil)
	{
		cell = [[UITableViewCell alloc]
						initWithStyle:UITableViewCellStyleDefault 
						reuseIdentifier:@"cell"];
	}
	
	// fill it with contents
	NSNetService *tempService = [serviceList objectAtIndex:indexPath.row];
	cell.textLabel.text = tempService.name;
	
	// return it
	return cell;
}


/**
  *  Set the title header for the table
  */
- (NSString *)tableView:(UITableView *)tableView titleForHeaderInSection:(NSInteger)section
{
	NSString *serverListTitle = [[NSString alloc] initWithFormat:@"Server List"];
	
	[serverListTitle autorelease];
	return serverListTitle;
}


/**
  * When a cell is selected, assign the selected value to a place holder
  */
- (void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath {
	
    UITableViewCell *selectCell = [tableView cellForRowAtIndexPath:indexPath];
	serverBroadcastName.text = selectCell.textLabel.text;
	serverName.text = @"";
	serverPassword.text = @"";
	
	selectedService = [serviceList objectAtIndex:indexPath.row];
}


/**
 * Initialize UIActivityIndicatorView
 */
-(void)initSpinner
{
	busyView = [[[UIActivityIndicatorView alloc] 
				 initWithActivityIndicatorStyle:UIActivityIndicatorViewStyleWhiteLarge] autorelease];

	// 324, 320
	
	
	CGPoint theCenter = CGPointMake(myTableView.frame.size.width, 
									myTableView.frame.size.height);
	busyView.center = theCenter;
	[self.view addSubview:busyView];
}

/**
 * Start the busy spinning
 */
-(void)spinBegin
{
	[busyView startAnimating];
}

/**
 * Stop spinning
 */
-(void)spinEnd
{
	[busyView stopAnimating];
	[busyView hidesWhenStopped];
}


/**
  * init necessary bonjour services and navigation bar button 
  */
- (void)viewDidLoad 
{
	[super viewDidLoad];
	
	// because there are no buttons to dismiss this modal dialog, I need to load a "Done" button bar
	// also, when button Done is pressed, we're going to call our own buttonPressed() function
	self.navigationItem.rightBarButtonItem = [[[UIBarButtonItem alloc]
											   initWithBarButtonSystemItem:UIBarButtonSystemItemDone 
											   target:self 
											   action:@selector(buttonPressed:) ]
											  autorelease];
	
	// the bonjour service goes here
	serviceBrowser = [[NSNetServiceBrowser alloc] init];
	serviceList = [[NSMutableArray alloc] init];
	[serviceBrowser setDelegate:self];
	
	// have a user defined at the very top
	NSNetService *userDefinedService = [[NSNetService alloc] initWithDomain:@"" type:@kNameOfService name:@kUserDefinedService port:49494];
	[serviceList addObject:userDefinedService];
	[userDefinedService release];
	
	[serviceBrowser searchForServicesOfType:@kNameOfService inDomain:@""];
	[self initSpinner];
}

/**
  * When user hit the DONE button in our modal dialog, we need to send these data
  */
-(void) buttonPressed:(id) sender 
{
	NSArray *values;
	
	values = [[NSArray alloc]
			  initWithObjects:serverBroadcastName.text, 
			  serverName.text, 
			  serverPassword.text,
			  selectedService, nil];		

	// put up the busy spinner
	[NSThread detachNewThreadSelector:@selector(spinBegin) toTarget:self withObject:nil];
	
	// call the delegate and pass the info as NSArray
	// the caller receives an array of values
	[delegate doneButtonPressed:values];
	
	// release
	[values release];
}

/**
  * used by service browser - this gets called if a service appears.  Update the tableview here.
  */
- (void)netServiceBrowser:(NSNetServiceBrowser *)aNetServiceBrowser didFindService:(NSNetService *)aNetService moreComing:(BOOL)moreComing
{
	if (![serviceList containsObject:aNetService])
	{
		[serviceList addObject:aNetService];
		[myTableView reloadData];
	}
}

/**
  * used by service browser - when a service disappears, this guy gets called.  Update the tableview here/
  */
- (void)netServiceBrowser:(NSNetServiceBrowser *)aNetServiceBrowser didRemoveService:(NSNetService *)aNetService moreComing:(BOOL)moreComing
{
	if ([serviceList containsObject:aNetService])
	{
		[serviceList removeObject:aNetService];
		[myTableView reloadData];
	}
}

// --------------------------------------------

/**
  *  auto rotation support is ON by default for iPad
  */
- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation {
    // Overriden to allow any orientation.
    return YES;
}


- (void)didReceiveMemoryWarning {
    // Releases the view if it doesn't have a superview.
    [super didReceiveMemoryWarning];
    
    // Release any cached data, images, etc that aren't in use.
}


- (void)viewDidUnload {
    [super viewDidUnload];
    // Release any retained subviews of the main view.
    // e.g. self.myOutlet = nil;
	
	[NSThread detachNewThreadSelector:@selector(spinEnd) toTarget:self withObject:nil];	
}

/**
  * release all objects we create.  That includes the bonjour service
  */
- (void)dealloc {
    [super dealloc];
	delegate = nil;
	[serverBroadcastName release];
	[serverName release];
	[serverPassword release];
	[serviceList release];
	[serviceBrowser release];
}


@end
