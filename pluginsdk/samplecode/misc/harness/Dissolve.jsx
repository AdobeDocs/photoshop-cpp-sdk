// Version: 2019.4.16

// make this a function so we can hide these from harness script
main();

function main() {

LogMessage( "Starting test: Dissolve:" );

// options for script, tie these into options from harness
// make this stuff controlled by the harness
var maxTime = MaxTime( 5 * 60 ); // was 1 * 60
var iterations = 5; // was 5
var percIncrease = 20;
var undoViaHistory = false;
var undoViaSelectFill = true;
var closeAllOpenFiles = true;
var closeAllAtEnd = false;
var closeEachDoc = true;
var waitForRedraw = true;
var fitOnScreen = true;

var timeIt = new Timer();

// Save the current preferences
var startRulerUnits = preferences.rulerUnits;
var startTypeUnits = preferences.typeUnits;
var startDisplayDialogs = displayDialogs;

// Set Photoshop to use pixels and display no dialogs
preferences.rulerUnits = Units.PIXELS;
preferences.typeUnits = TypeUnits.PIXELS;
displayDialogs = ShowDialog();

var tests = 0;
var errors = 0;

var eArray = new Array();

// the plugin writes this file
var dissolvePluginLog = new File( "~/Desktop/Dissolve.log" );
var dissolveLog = new File( "~/Desktop/DissolveTest.log" );
dissolveLog.open( "w", "TEXT", "????" );
dissolveLog.writeln( "width,\theight,\tdepth,\tpercent,\tmode,\tdoc create time,\tdissolve time" );

var availModes = [ "moD0", "moD1", "moD2", "moD3" ];
var modeIndex = 0;

var maxWidth = 30000;
var minWidth = 256;
var incWidth = parseInt( (maxWidth - minWidth) / iterations );
if ( incWidth == 0 )
	incWidth = 1;

var maxHeight = 30000;
var minHeight = 256;
var incHeight = parseInt( (maxHeight - minHeight) / iterations );
if ( incHeight == 0 )
	incHeight = 1;

// start clean, this really should be some param from the global harness
while (closeAllOpenFiles && documents.length) {
    activeDocument.close(SaveOptions.DONOTSAVECHANGES);
}

var d = BitsPerChannelType.EIGHT;

for ( var w = minWidth; w < maxWidth; w += incWidth ) {
	for ( var h = minHeight; h < maxHeight; h += incHeight ) {
			
		try {
			
			var timeImport = new Timer();
				
			if ( d == BitsPerChannelType.EIGHT )
				d = BitsPerChannelType.SIXTEEN;
			else if (d == BitsPerChannelType.SIXTEEN )
				d = BitsPerChannelType.THIRTYTWO;
			else
				d = BitsPerChannelType.EIGHT;
					
			app.documents.add(UnitValue(w, "px"), 
                              UnitValue(h,"px"), 
                              undefined, 
                              "Dissolve Test " + w + " " + maxWidth + " " + h + " " + maxHeight, 
                              undefined, 
                              undefined, 
                              undefined, 
                              d);
			
            timeImport.stop();
				
			if ( activeDocument.width != w ) 
                error++;
			if ( activeDocument.height != h ) 
                error++;
			if ( ( d == BitsPerChannelType.THIRTYTWO ) && ( activeDocument.bitsPerChannel != BitsPerChannelType.THIRTYTWO ) ) 
                error++;
			if ( ( d == BitsPerChannelType.SIXTEEN ) && ( activeDocument.bitsPerChannel != BitsPerChannelType.SIXTEEN ) ) 
                error++;
			if ( ( d == BitsPerChannelType.EIGHT ) && ( activeDocument.bitsPerChannel != BitsPerChannelType.EIGHT ) )
                error++;
			if ( activeDocument.bitsPerChannel == BitsPerChannelType.ONE ) 
                error++;
				
            if (fitOnScreen)
            	FitOnScreen(); // this makes everything really slow
            
            if (undoViaHistory)
				var historyState = activeDocument.activeHistoryState;
				
			for ( var dPerc = 0; dPerc <= 100; dPerc += percIncrease ) {
				tests++;
				var timeDissolve = new Timer();
				Dissolve(dPerc, availModes[modeIndex]);
				timeDissolve.stop();
				dissolveLog.write(w + ",\t" + h + ",\t" + d + ",\t" + dPerc + ",\t", availModes[modeIndex] );				
				dissolveLog.writeln(",\t" + timeImport.getTime() + ", " + timeDissolve.getTime() );

				if (waitForRedraw)
                	WaitForRedraw();

                if (undoViaHistory)
					activeDocument.activeHistoryState = historyState;

				modeIndex++;
				if (modeIndex >= availModes.length)
					modeIndex = 0;
                
                if ( timeIt.getElapsed() > maxTime ) {
                    w = maxWidth + 1;
                    h = maxHeight + 1;
                    dPerc = 101;
                }
			}

			if (closeEachDoc)
		 	   activeDocument.close(SaveOptions.DONOTSAVECHANGES);
				
		}
		catch(e) {
		    ShowAlert(e + ":" + e.line);
			if ( e.message.search(/cancel/i) != -1 ) {
				w = maxWidth + 1;
				h = maxHeight + 1;
			}
			eArray[eArray.length] = e;
			errors++;
			// debugger;
		} // end catch
	} // end for height
} // end for width
dissolveLog.writeln( errors + " errors. " + tests + " tests in " + timeIt.getElapsed() + " seconds. " + tests/timeIt.getElapsed() + " tests/sec.");
dissolveLog.close();
if ( ! BringToFront()) {
    dissolveLog.execute();
    dissolvePluginLog.execute();
}

// end clean, this really should be some param from the global harness
while (closeAllAtEnd && documents.length) {
    activeDocument.close(SaveOptions.DONOTSAVECHANGES);
}

// Reset the application preferences
preferences.rulerUnits = startRulerUnits;
preferences.typeUnits = startTypeUnits;
displayDialogs = startDisplayDialogs;

//     1) " FAIL" for failures
//     2) " PASS" for test results OK
//     3) "  BUG" for known bugs, have the file name give the bug number
//     4) "ERROR" this comes from the harness if the script barfed/exception,

if (errors == 0) {
	errors = ' PASS';
} else {
	errors = ' FAIL';
}
	
LogMessage( errors + "\n" );

ShowAlert( errors );

return errors;

} // end function main

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/****************************
  Function WaitForRedraw
  Usage: Use it to force Photoshop to redraw the screen before continuing
  Example: 
       WaitForRedraw();
****************************/
function WaitForRedraw() {
	var keyID = charIDToTypeID( "Stte" );
	var desc = new ActionDescriptor();
	desc.putEnumerated( keyID, keyID, charIDToTypeID( "RdCm" ) );
	executeAction( charIDToTypeID( "Wait" ), desc, DialogModes.NO );
}

//////////////////////////////////////////////////////////////////////////////
// WaitNSeconds, slow the script down so you can watch and figure out issues
//////////////////////////////////////////////////////////////////////////////
function WaitNSeconds(seconds) {
   startDate = new Date();
   endDate = new Date();
   while ((endDate.getTime() - startDate.getTime()) < (1000 * seconds)) 
		endDate = new Date();
}

//////////////////////////////////////////////////////////////////////////////
// FitOnScreen, fits the document and redraws the screen
//////////////////////////////////////////////////////////////////////////////
function FitOnScreen() {
	var id45 = charIDToTypeID( "slct" );
    var desc7 = new ActionDescriptor();
    var id46 = charIDToTypeID( "null" );
	var ref1 = new ActionReference();
	var id47 = charIDToTypeID( "Mn  " );
	var id48 = charIDToTypeID( "MnIt" );
	var id49 = charIDToTypeID( "FtOn" );
	ref1.putEnumerated( id47, id48, id49 );
	desc7.putReference( id46, ref1 );
	executeAction( id45, desc7, DialogModes.NO );
}

//////////////////////////////////////////////////////////////////////////////
// Dissolve, given a percentage and mode
//////////////////////////////////////////////////////////////////////////////
function Dissolve( dPercent, dMode ) { 
	var id18 = stringIDToTypeID( "d9543b0c-3c91-11d4-97bc-00b0d0204936" );
    var desc3 = new ActionDescriptor();
    var id19 = charIDToTypeID( "Amnt" );
    var id20 = charIDToTypeID( "#Prc" );
    desc3.putUnitDouble( id19, id20, dPercent );
    var id21 = charIDToTypeID( "disP" );
    var id22 = charIDToTypeID( "mooD" );
    var id23 = charIDToTypeID( dMode );
    desc3.putEnumerated( id21, id22, id23 );
	executeAction( id18, desc3, ShowDialog() );
}

//////////////////////////////////////////////////////////////////////////////
// DissolveOld, our old scripting params was this
// an interesting test would be to see the old plug-in vs the current one
//////////////////////////////////////////////////////////////////////////////
function DissolveOld( dPercent, dMode ) {
    var id12 = charIDToTypeID( "disS" );
    var desc3 = new ActionDescriptor();
    var id13 = charIDToTypeID( "Amnt" );
    var id14 = charIDToTypeID( "#Prc" );
    desc3.putUnitDouble( id13, id14, dPercent );
    var id15 = charIDToTypeID( "disP" );
    var id16 = charIDToTypeID( "mooD" );
    var id17 = charIDToTypeID( dMode );
    desc3.putEnumerated( id15, id16, id17 );
    executeAction( id12, desc3, ShowDialog() );
}

//////////////////////////////////////////////////////////////////////////////
// Library for timing things in JavaScript
//////////////////////////////////////////////////////////////////////////////
function Timer() {
	// member variables
	this.startTime = new Date();
	this.endTime = new Date();
	
	// member functions
	
	// reset the start time to now
	this.start = function () { this.startTime = new Date(); }
	
	// reset the end time to now
	this.stop = function () { this.endTime = new Date(); }
	
	// get the difference in milliseconds between start and stop
	this.getTime = function () { return (this.endTime.getTime() - this.startTime.getTime()) / 1000; }
	
	// get the current elapsed time from start to now, this sets the endTime
	this.getElapsed = function () { this.endTime = new Date(); return this.getTime(); }
}

function LogMessage( inMessage ) {
	if ( BridgeTalk.isRunning( "estoolkit" ) )
		$.write( inMessage );
}

function ShowAlert( inMessage ) {
    if ( typeof gHarness == "undefined" || gHarness.showAlerts ) {
        alert( inMessage );
    }
}

function ShowDialog( ) {
    if ( typeof gHarness == "undefined" || typeof gHarness.showDialogs == "undefined" ) {
        return DialogModes.NO;
    } 
    return gHarness.showDialogs;
}

function MaxTime( inTime ) {
    if ( typeof gHarness == "undefined" || typeof gHarness.maxTime == "undefined" ) {
        return inTime;
    } 
    return gHarness.maxTime;
}

function BringToFront( ) {
    if ( typeof gHarness == "undefined" || typeof gHarness.bringToFront == "undefined" ) {
        return false;
    } 
    return gHarness.bringToFront;
}

// end Dissolve.jsx
