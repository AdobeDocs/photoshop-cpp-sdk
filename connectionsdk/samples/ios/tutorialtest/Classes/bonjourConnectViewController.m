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
//  bonjourConnectViewController.m
//  transmitimage
//
//  @author Allen Jeng, Photoshop QE Developer, Adobe Systems Inc.
//  Copyright 2011 Adobe Systems. All rights reserved.
//

#import "bonjourConnectViewController.h"
#import "connectionViewController.h"


@implementation bonjourConnectViewController

@synthesize myTableView, delegate, serverName, serverPwd, isIPConnection;
@synthesize serviceBrowser, serviceList, selectedService;

#pragma mark -
#pragma mark View lifecycle


- (void)viewDidLoad {
    [super viewDidLoad];

    // Preserve selection between presentations.
    self.clearsSelectionOnViewWillAppear = NO;
 
	// init the services
	serviceBrowser = [[NSNetServiceBrowser alloc] init];
	serviceList = [[NSMutableArray alloc] init];
	[serviceBrowser setDelegate:self];
	
	// have an user defined item so that user can CHOOSE
	NSNetService *userDefinedService = [[NSNetService alloc] initWithDomain:@"" type:@kNameOfService name:@kUserDefineName port:49494];
	[serviceList addObject:userDefinedService];
	[userDefinedService release];
	
	[serviceBrowser searchForServicesOfType:@kNameOfService inDomain:@""];
	
	isIPConnection = NO;
}



- (void)viewWillAppear:(BOOL)animated 
{
    [super viewWillAppear:animated];
	self.contentSizeForViewInPopover = CGSizeMake(300, 300);
}

/*
- (void)viewDidAppear:(BOOL)animated {
    [super viewDidAppear:animated];
}
*/
/*
- (void)viewWillDisappear:(BOOL)animated {
    [super viewWillDisappear:animated];
}
*/

/*
- (void)viewDidDisappear:(BOOL)animated 
{
    [super viewDidDisappear:animated];
}
*/

- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation {
    // Override to allow orientations other than the default portrait orientation.
    return YES;
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


/**
 * when the delegate returns from the other view, I want data pass back to me.
 */
-(void) connectionSelected:(NSArray *) values
{
	/* values passed to us:
	 0:  server's name or IP
	 1:  the password
	 */
	
	self.serverName = [values objectAtIndex:0];
	self.serverPwd = [values objectAtIndex:1];
	
	// if this guy gets called, it means that the user has selected a service and 
	// we're GOOD to go with passing info to whom ever is our deledate
	/* the value that we want to pass
	 0: server's name or IP
	 1: the password
	 2: NSNetService object
	*/
	
	NSArray *data = [[NSArray alloc] 
					 initWithObjects:self.serverName,
					 self.serverPwd,
					 self.selectedService,
					 nil];
	
	// send the data
	[delegate bonjourConnectSelected:data];
	
	[data release];
}

// --------------------------------------------

#pragma mark -
#pragma mark Table view data source

- (NSInteger)numberOfSectionsInTableView:(UITableView *)tableView {
    // Return the number of sections.
    return 1;
}


/*
 * Return the number of rows in the section.
 */
- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section {
    return serviceList.count;
}


/*
 * Customize the appearance of table view cells.
 */
- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath {
    
	// ask for availablity -- reuse
    static NSString *CellIdentifier = @"Cell";
    UITableViewCell *cell = [tableView dequeueReusableCellWithIdentifier:CellIdentifier];
	
	// create a cell
    if (cell == nil) 
	{
        cell = [[[UITableViewCell alloc] 
				 initWithStyle:UITableViewCellStyleDefault 
				 reuseIdentifier:CellIdentifier] autorelease];
    }
    
	// fill it with contents
	NSNetService *tempService = [serviceList objectAtIndex:indexPath.row];
	cell.textLabel.text = tempService.name;
    
	// mark it with ">" so that users knows it will show them another screen
//	if (indexPath.row <= 1)
		cell.accessoryType = UITableViewCellAccessoryDisclosureIndicator;
	
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


#pragma mark -
#pragma mark Table view delegate

/*
 *  when user selects an item, we present the connector
 */
- (void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath {

	connectionViewController *connViewController = 
		[[connectionViewController alloc]
		 initWithNibName:@"connectionViewController" bundle:nil];
	
	// assign/remember the choice
	selectedService = [serviceList objectAtIndex:indexPath.row];
	
	// reset the name just in case someone decides to change their mind and pick another service
	self.serverName = selectedService.name;
	connViewController.serviceName = self.serverName;
	
	// easy tracking for if this is a user defined service name or not (need to know when binding/opening stream)
	isIPConnection = [selectedService.name isEqualToString:@kUserDefineName];
	connViewController.isUserDefined = isIPConnection;
	
	// set the delegate
	connViewController.delegate = self;
	
	// set the size of that view
	connViewController.contentSizeForViewInPopover = CGSizeMake(300, 300);
	
	// activate the view
	[self.navigationController pushViewController:connViewController animated:YES];
	
	[connViewController	release];		
	
}


#pragma mark -
#pragma mark Memory management

- (void)didReceiveMemoryWarning {
    // Releases the view if it doesn't have a superview.
    [super didReceiveMemoryWarning];
    
    // Relinquish ownership any cached data, images, etc. that aren't in use.
}

- (void)viewDidUnload {
    // Relinquish ownership of anything that can be recreated in viewDidLoad or on demand.
    // For example: self.myOutlet = nil;
}


- (void)dealloc {
    [super dealloc];
	
	[serviceList release];
	[serviceBrowser release];
}


@end

