// Version: 2018.4.24

// call a function so we can hide variables from other scripts
main();

function main() {

LogMessage( "Starting test: SelectoramaShape:" );

var timeIt = new Timer();

var errors = 0;

var eArray = new Array();

try {

if ( documents.length == 0 )
    documents.add();

// =======================================================
var idAdobeSDKspShape = stringIDToTypeID( "AdobeSDK Shape" );
    var desc18 = new ActionDescriptor();
    var idShpsp = charIDToTypeID( "Shp " );
    var idtshP = charIDToTypeID( "tshP" );
    var idshPzero = charIDToTypeID( "shP0" );
    desc18.putEnumerated( idShpsp, idtshP, idshPzero );
    var idkcrE = charIDToTypeID( "kcrE" );
    var idtcrE = charIDToTypeID( "tcrE" );
    var idcrEzero = charIDToTypeID( "crE0" );
    desc18.putEnumerated( idkcrE, idtcrE, idcrEzero );
executeAction( idAdobeSDKspShape, desc18, DialogModes.NO );

if ( activeDocument.historyStates[activeDocument.historyStates.length-1].name != "Shape" )
    errors++;

if ( activeDocument.selection.bounds[0].as('px') <= 0 )
	errors ++;

if ( activeDocument.selection.bounds[1].as('px') <= 0 )
	errors ++;

if ( activeDocument.selection.bounds[2].as('px') <= 0 )
	errors ++;

if ( activeDocument.selection.bounds[3].as('px') <= 0 )
	errors ++;

var oldLayerCount = activeDocument.layers.length;

// =======================================================
var idAdobeSDKspSelectorama = stringIDToTypeID( "AdobeSDK Selectorama" );
    var desc13 = new ActionDescriptor();
    var idArspsp = charIDToTypeID( "Ar  " );
    var idtseL = charIDToTypeID( "tseL" );
    var idshLtwo = charIDToTypeID( "shL2" );
    desc13.putEnumerated( idArspsp, idtseL, idshLtwo );
    var idAmnt = charIDToTypeID( "Amnt" );
    var idshPrc = charIDToTypeID( "#Prc" );
    desc13.putUnitDouble( idAmnt, idshPrc, 50.000000 );
    var idChns = charIDToTypeID( "Chns" );
    var idtcoM = charIDToTypeID( "tcoM" );
    var iduseT = charIDToTypeID( "useT" );
    desc13.putEnumerated( idChns, idtcoM, iduseT );
    var idkcrE = charIDToTypeID( "kcrE" );
    var idtcrE = charIDToTypeID( "tcrE" );
    var idcrEtwo = charIDToTypeID( "crE2" );
    desc13.putEnumerated( idkcrE, idtcrE, idcrEtwo );
executeAction( idAdobeSDKspSelectorama, desc13, DialogModes.NO );

if ( oldLayerCount != activeDocument.layers.length - 1 )
	errors++;

if ( activeDocument.historyStates[activeDocument.historyStates.length-1].name != "Selectorama" )
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

// end SelectoramaShape.jsx
