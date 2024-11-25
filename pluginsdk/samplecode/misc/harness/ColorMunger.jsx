// Version: 2018.4.24

// call a function so we can hide variables from other scripts
main();

function main() {

LogMessage( "Starting test: ColorMunger:" );

// Save the current preferences
var startRulerUnits = preferences.rulerUnits;
var startTypeUnits = preferences.typeUnits;
var startDisplayDialogs = displayDialogs;

// Set Photoshop to use pixels and display no dialogs
preferences.rulerUnits = Units.PIXELS;
preferences.typeUnits = TypeUnits.PIXELS;
displayDialogs = DialogModes.NO;

var errors = 0;

try {
    
    app.documents.add();

    // It would be nice if this thing would give back an answer!
    var idclrM = charIDToTypeID( "clrM" );
    var desc18 = new ActionDescriptor();
    var idFrgC = charIDToTypeID( "FrgC" );
    var desc19 = new ActionDescriptor();
    var idRd = charIDToTypeID( "Rd  " );
    desc19.putInteger( idRd, 12 );
    var idGrn = charIDToTypeID( "Grn " );
    desc19.putInteger( idGrn, 12 );
    var idBl = charIDToTypeID( "Bl  " );
    desc19.putInteger( idBl, 12 );
    var idRGBC = charIDToTypeID( "RGBC" );
    desc18.putObject( idFrgC, idRGBC, desc19 );
    var idClr = charIDToTypeID( "Clr " );
    var idtyCN = charIDToTypeID( "tyCN" );
    var idcRGB = charIDToTypeID( "cRGB" );
    desc18.putEnumerated( idClr, idtyCN, idcRGB );
    var idClrz = charIDToTypeID( "Clrz" );
    var idtyCN = charIDToTypeID( "tyCN" );
    var idcmyK = charIDToTypeID( "cmyK" );
    desc18.putEnumerated( idClrz, idtyCN, idcmyK );
    executeAction( idclrM, desc18, DialogModes.NO );
    
    activeDocument.close( SaveOptions.DONOTSAVECHANGES );
    
}
catch(e) {
    alert(e + ":" + e.line);
	if ( e.message.search(/cancel/i) != -1 ) {
		w = maxWidth + 1;
		h = maxHeight + 1;
	}
	eArray[eArray.length] = e;
	errors++;
	// debugger;
} // end catch

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


function LogMessage( inMessage ) {
    if ( BridgeTalk.isRunning( "estoolkit" ) )
        $.write( inMessage );
}


// end ColorMunger.jsx
