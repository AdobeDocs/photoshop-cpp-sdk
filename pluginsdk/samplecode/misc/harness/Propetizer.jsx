// Version: 2018.4.24

// call a function so we can hide variables from other scripts
main();

function main() {

LogMessage( "Starting test: Propetizer:" );

var timeIt = new Timer();

var errors = 0;

var eArray = new Array();

try {

if ( documents.length == 0 )
    documents.add();

// =======================================================
var idgriD = charIDToTypeID( "griD" );
    var desc3 = new ActionDescriptor();
    var idHrzn = charIDToTypeID( "Hrzn" );
    desc3.putDouble( idHrzn, 50.000000 );
    var idVrtc = charIDToTypeID( "Vrtc" );
    desc3.putDouble( idVrtc, 100.000000 );
    var idwate = charIDToTypeID( "wate" );
    desc3.putBoolean( idwate, true );
executeAction( idgriD, desc3, DialogModes.NO );

if ( activeDocument.historyStates[activeDocument.historyStates.length-1].name != "Propetizer" )
    errors++;

// =======================================================
var idgriD = charIDToTypeID( "griD" );
    var desc4 = new ActionDescriptor();
    var idHrzn = charIDToTypeID( "Hrzn" );
    desc4.putDouble( idHrzn, -100.000000 );
    var idVrtc = charIDToTypeID( "Vrtc" );
    desc4.putDouble( idVrtc, -200.000000 );
executeAction( idgriD, desc4, DialogModes.NO );

if ( activeDocument.historyStates[activeDocument.historyStates.length-1].name != "Propetizer" && 
     activeDocument.historyStates[activeDocument.historyStates.length-2].name != "Propetizer" )
    errors++;

	// end clean
	while (documents.length) {
    	activeDocument.close(SaveOptions.DONOTSAVECHANGES);
	}

}
catch(e) {
	eArray[eArray.length] = e;
	errors++;
} // end catch

//     1) " FAIL" for failures
//     2) " PASS" for test results OK
//     3) "  BUG" for known bugs, have the file name give the bug number
//     4) "ERROR" this comes from the harness if the script barfed/exception,

if (errors == 0) {
	errors = ' PASS';
} else {
	errors = ' FAIL';
}
	
LogMessage(errors + "\n");

if ( typeof gHarness == "undefined" || gHarness.showAlerts ) {
	alert( errors );
}

return errors;

} // end function main

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

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

// end Propetizer.jsx
