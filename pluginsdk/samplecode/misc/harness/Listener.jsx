// Version: 2019.4.16

// call a function so we can hide variables from other scripts
main();

function main() {

LogMessage( "Starting test: Listener:" );

var errors = 0;

try {

    // =======================================================
    var idAdobeSDKListener = stringIDToTypeID( "AdobeSDK Listener" );
    var desc28 = new ActionDescriptor();
    var idactS = charIDToTypeID( "actS" );
    desc28.putString( idactS, """Default Actions""" );
    var idactN = charIDToTypeID( "actN" );
    desc28.putString( idactN, """Vignette (selection)""" );
    var idactI = charIDToTypeID( "actI" );
    var eventCopy = charIDToTypeID( "copy" );
    desc28.putInteger( idactI, eventCopy );
    executeAction( idAdobeSDKListener, desc28, ShowDialog() );
    
}
catch( e ) {
    ShowAlert( e + ":" + e.line );
    errors++;
    // debugger;
} // end catch

//     1) " FAIL" for failures
//     2) " PASS" for test results OK
//     3) "  BUG" for known bugs, have the file name give the bug number
//     4) "ERROR" this comes from the harness if the script barfed/exception,

if ( errors == 0 ) {
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

function LogMessage( inMessage ) {
    if ( BridgeTalk.isRunning( "estoolkit" ) ) {
        $.write( inMessage );
    }
}

function ShowAlert( inMessage ) {
    if ( typeof gHarness == "undefined" || gHarness.showAlerts ) {
        alert( inMessage );
    }
}

function ShowDialog ( ) {
    if ( typeof gHarness == "undefined" || typeof gHarness.showDialogs == "undefined" ) {
        return DialogModes.NO;
    } 
    return gHarness.showDialogs;
}

// end Listener.jsx
