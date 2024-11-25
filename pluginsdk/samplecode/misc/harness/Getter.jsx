// Version: 2019.4.16

// make this a function so we can hide these from harness script
main();

function main() {

LogMessage( "Starting test: Getter:" );

var errors = 0;

var getterLog = new File( "~/Desktop/Getter.log" );

getterLog.remove();

// =======================================================
var idGetter = stringIDToTypeID( "459ac2e6-82d1-11d5-9879-00b0d0204936" );
executeAction( idGetter, undefined, DialogModes.NO );

if ( ! getterLog.exists ) 
    errors++;
    
if ( getterLog.length < 100 )
    errors++;
    
if ( ! BringToFront() ) {
    getterLog.execute();
}

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

function BringToFront( ) {
    if ( typeof gHarness == "undefined" || typeof gHarness.bringToFront == "undefined" ) {
        return false;
    } 
    return gHarness.bringToFront;
}

// end Getter.jsx
