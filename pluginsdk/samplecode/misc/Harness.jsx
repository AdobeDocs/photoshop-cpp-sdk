// Version: 2018.4.24

// Run all the scripts, *.jsx files, found in the "harness" folder
// The individual scripts can be run one at a time for a visual result
// The individual scripts must conform to the following rules to work with this script

//  A) Return a string with one of the following values:
//     1) " FAIL" for failures
//     2) " PASS" for test results OK
//     3) "  BUG" for known bugs, have the file name give the bug number
//     4) "ERROR" this comes from the harness if the script barfed/exception,
//         counts same as "FAIL"

//  B) Test for the harness global and show results to user accordingly
//     if ( typeof gHarness == "undefined" || gHarness.showAlerts ) {
//         alert( result );
//     }

//  C) Leave Photoshop as you found it (soft requirement)
//     1) setting back preferences
//     2) closing documents you opened

var gHarness = {};

LogMessage( "Starting Harness" );

gHarness.harnessVersion = "2020.2.25";

// start the clock now for performance reports
gHarness.totalTime = new TimeIt();

// accumulater for the time taken inside all the tests, see loop below
gHarness.testTimes = 0;

// counters to track our totals
gHarness.passCount = 0;
gHarness.bugCount = 0;
gHarness.failCount = 0;
gHarness.unknownCount = 0;

///////////////////////////////////////////////////////////////////////////////
// global used by individual tests

gHarness.showAlerts = false;           // to see if they should not show error messages
gHarness.showDialogs = DialogModes.NO; // DialogModes.ERROR or DialogModes.ALL
gHarness.maxTime = 1; // 10 * 60;            // max time any script should take in seconds
gHarness.bringToFront = true;          // keep Photoshop in front (do not execute log files)

///////////////////////////////////////////////////////////////////////////////

// where am I, and other location(s)
gHarness.strLocation = Folder( File( $.fileName ).parent ).toString();
gHarness.strTestHarnessLocation = gHarness.strLocation + "/harness";
gHarness.strLogFileLocation = gHarness.strTestHarnessLocation + "/logfile";

// write out results to a log file
gHarness.strLogFileFullPath = gHarness.strLogFileLocation + "/LogFile.txt";

// create the directory if needed
Folder( gHarness.strLogFileLocation ).create();

gHarness.harnessLog = File( gHarness.strLogFileFullPath );
gHarness.harnessLog.open( "w" );

// write out some header information for versions etc.
gHarness.harnessLog.writeln( "Test Time:               " + Date().toString() );
gHarness.harnessLog.writeln( "Harness Version:         " + gHarness.harnessVersion );
gHarness.harnessLog.writeln( "Application:             " + app.name );
gHarness.harnessLog.writeln( "Application Version:     " + app.version );
gHarness.harnessLog.writeln( "Application Build:       " + app.build );
gHarness.harnessLog.writeln( "Scripting Version:       " + app.scriptingVersion );
gHarness.harnessLog.writeln( "Scripting Build:         " + app.scriptingBuildDate );
gHarness.harnessLog.writeln( "ExtendScript Version:    " + $.version );
gHarness.harnessLog.writeln( "ExtendScript Build:      " + $.build );
gHarness.harnessLog.writeln( "ExtendScript Build Date: " + $.buildDate );
gHarness.harnessLog.writeln();

gHarness.harnessFolder = Folder( gHarness.strTestHarnessLocation );

gHarness.harnessFiles = gHarness.harnessFolder.getFiles( "*.jsx" );

if ( gHarness.harnessFiles.length == 0 ) {
	gHarness.harnessLog.writeln( "ERROR: No files in harness" );
	gHarness.failCount++;
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
// Global Harness Functions 
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

gHarness.FormatString = function( inNumber ) {
    var strNumber = inNumber.toString();
    strNumber = strNumber.split( "." );
    if ( strNumber.length == 1 )
        strNumber[ 1 ] = "000";
    while ( strNumber[ 0 ].length < 3 )
        strNumber[ 0 ] = " " + strNumber[ 0 ];
    if ( strNumber[ 1 ] > 3 )
        strNumber[ 1 ] = strNumber[ 1 ].substr( 0, 3 );
    while ( strNumber[ 1 ].length < 3 )
        strNumber[ 1 ] = strNumber[ 1 ] + "0";
    return strNumber.join( "." );
}


// eval each file found and track time and results
for ( gHarness.harnessIndex = 0; gHarness.harnessIndex < gHarness.harnessFiles.length; gHarness.harnessIndex++ ) {
	gHarness.timeIt = new TimeIt();
	try {
		if ( gHarness.harnessFiles[ gHarness.harnessIndex ] instanceof File &&
		     gHarness.harnessFiles[ gHarness.harnessIndex ].name != ".DS_Store" ) {
			if ( gHarness.harnessFiles[ gHarness.harnessIndex ].open( "r" ) ) {
				gHarness.fileContents = gHarness.harnessFiles[ gHarness.harnessIndex ].read();
				gHarness.harnessFiles[ gHarness.harnessIndex ].close();
				gHarness.result = eval( gHarness.fileContents );
				if ( gHarness.result == " PASS" ) {
					gHarness.passCount++;
				} else if ( typeof gHarness.result == 'string' && 
                            gHarness.result.search("BUG") != -1 ) {
					gHarness.bugCount++;
				} else if ( gHarness.result == " FAIL" ) {
					gHarness.failCount++;
				} else {
					gHarness.unknownCount++;
				}
                gHarness.timeIt.stop();
                gHarness.testTimes += gHarness.timeIt.getTime();
				gHarness.harnessLog.writeln( gHarness.result + ": " + gHarness.FormatString( gHarness.timeIt.getTime() ) + 
                                             " secs : " + gHarness.harnessFiles[ gHarness.harnessIndex ].name );
			} else {
				gHarness.failCount++;
			}
		}
	}
	catch ( harnessError ) {
		gHarness.harnessLog.writeln( "ERROR: " + gHarness.FormatString( gHarness.timeIt.getElapsed() ) + " secs : " + 
                                     gHarness.harnessFiles[ gHarness.harnessIndex ].name + " : " + harnessError + " : Line : " + 
                                     harnessError.line + " : FileName : " + File( harnessError.fileName ).name );
		gHarness.failCount++;
	}
}

// well I can't report the actual totalTime now can I
gHarness.totalTime.stop();

// write out some footer information
gHarness.harnessLog.writeln();
gHarness.harnessLog.writeln( "     PassCount: " + gHarness.passCount );
gHarness.harnessLog.writeln( "      BugCount: " + gHarness.bugCount );
gHarness.harnessLog.writeln( "     FailCount: " + gHarness.failCount );
gHarness.harnessLog.writeln( "  UnknownCount: " + gHarness.unknownCount );
gHarness.harnessLog.writeln( "        Pass %: " + gHarness.FormatString( gHarness.passCount / ( gHarness.passCount + gHarness.failCount ) * 100 ));
gHarness.harnessLog.writeln( "    Total Time: " + gHarness.FormatString( gHarness.totalTime.getTime()) + " secs" );
gHarness.harnessLog.writeln( "     Test Time: " + gHarness.FormatString( gHarness.testTimes ) + " secs" );
gHarness.harnessLog.writeln( "Harness Time %: " + gHarness.FormatString( ( gHarness.totalTime.getTime() - gHarness.testTimes) / gHarness.totalTime.getTime() * 100 ) );
gHarness.harnessLog.close();
gHarness.harnessLog.execute();



//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
// Global Functions 
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

function TimeIt() {
	// member variables
	this.startTime = new Date();
	this.endTime = new Date();
	
	// member functions
	
	// reset the start time to now
	this.start = function () { 
        this.startTime = new Date(); 
    }
	
	// reset the end time to now
	this.stop = function () { 
        this.endTime = new Date(); 
    }
	
	// get the difference in milliseconds between start and stop
	this.getTime = function () { 
        return (this.endTime.getTime() - this.startTime.getTime()) / 1000; 
    }
	
	// get the current elapsed time from start to now, this sets the endTime
	this.getElapsed = function () { 
        this.endTime = new Date(); return this.getTime(); 
    }
}

function LogMessage( inMessage ) {
	if ( BridgeTalk.isRunning( "estoolkit" ) )
		$.writeln( inMessage );
}

// end Harness.jsx
