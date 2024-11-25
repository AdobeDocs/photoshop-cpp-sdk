// Version: 2019.4.16

// call a function so we can hide variables from other scripts
main();

function main() {

LogMessage( "Starting test: GradientImport:" );

// Save the current preferences
var startRulerUnits = preferences.rulerUnits;
var startTypeUnits = preferences.typeUnits;
var startDisplayDialogs = displayDialogs;

// Set Photoshop to use pixels and display no dialogs
preferences.rulerUnits = Units.PIXELS;
preferences.typeUnits = TypeUnits.PIXELS;
displayDialogs = ShowDialog();

var maxTime = MaxTime( 5 * 60 );
var iterations = 5;

var timeIt = new Timer();

var tests = 0;
var errors = 0;

var eArray = new Array();

var gradientImportLog = new File( "~/Desktop/GradientImport.log" );
gradientImportLog.open( "w", "TEXT", "????" );
gradientImportLog.writeln( "width,\theight,\tmode,\tinvert,\ttransparent,\tImport Time" );

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

var modes = [ "indX", "rgbC", "bitM", "gryS" ];
var modesIndex = 0;

var invert = false;
var transparent = false;
var itIndex = 0;

// start clean
while (documents.length) {
    activeDocument.close(SaveOptions.DONOTSAVECHANGES);
}

for ( var w = minWidth; w < maxWidth; w += incWidth ) {
	for ( var h = minHeight; h < maxHeight; h += incHeight ) {
			
		try {
			
			var timeImport = new Timer();
				
             GradientImport( w, h, modes[modesIndex], invert, transparent );
             
             timeImport.stop();
             
             gradientImportLog.writeln( w + ",\t" + h + ",\t" + modes[modesIndex]+ ",\t" + invert+ ",\t" + transparent + ",\t" + timeImport.getElapsed() );
             
             FitOnScreen();
             WaitForRedraw();
             
			if ( activeDocument.width != w ) 
                 error++; // alert( activeDocument.width + ", " + w );
			if ( activeDocument.height != h ) 
                 error++; // alert( activeDocument.height + ", " + h );
			if ( ( ( modes[modesIndex] == "indX" || modes[modesIndex] == "rgbC" || modes[modesIndex] == "grayS" ) 
                    && activeDocument.bitsPerChannel != BitsPerChannelType.EIGHT ) ||
			     ( modes[modesIndex] == "bitM" && activeDocument.bitsPerChannel != BitsPerChannelType.ONE ) ) 
                 error++; // alert( activeDocument.bitsPerChannel + ", " + modes[modesIndex] );

             // activeDocument.close(SaveOptions.DONOTSAVECHANGES);

             if ( invert && transparent ) {
                 modesIndex++;
                 invert = false;
                 transparent = false;
             }
             if ( modesIndex > ( modes.length - 1 ) )
                 modesIndex = 0;
             if ( ! invert ) {
                if ( ! transparent ) {
                    transparent = true;
                } else {
                    invert = true;
                    transparent = false;
                }
             } else {
                 if ( ! transparent ) {
                     transparent = true;
                 } else {
                     error++;
                     ShowAlert('should not get here, above code should reset everything'); // invert = false;
                 }
             }
         
             if ( timeIt.getElapsed() > maxTime ) {
                 w = maxWidth + 1;
                 h = maxHeight + 1;
             }
         
             tests++;
	
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
gradientImportLog.writeln( errors + " errors. " + tests + " tests in " + timeIt.getElapsed() + " seconds. " + tests/timeIt.getElapsed() + " tests/sec.");
gradientImportLog.close();
if ( ! BringToFront() ) {
    gradientImportLog.execute();
}

// end clean
while (documents.length) {
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

//////////////////////////////////////////////////////////////////////////////
// GradientImport()
// mode, "indX", "rgbC", "bitM", "gryS"
//////////////////////////////////////////////////////////////////////////////
function GradientImport( width, height, mode, invert, transparent ) {
    var idImpr = charIDToTypeID( "Impr" );
    var desc4 = new ActionDescriptor();
    var idUsng = charIDToTypeID( "Usng" );
    var desc5 = new ActionDescriptor();
    var idmulK = charIDToTypeID( "mulK" );
    var list2 = new ActionList();
    var desc6 = new ActionDescriptor();
    var idHrzn = charIDToTypeID( "Hrzn" );
    var idPxl = charIDToTypeID( "#Pxl" );
    desc6.putUnitDouble( idHrzn, idPxl, width );
    var idVrtc = charIDToTypeID( "Vrtc" );
    var idPxl = charIDToTypeID( "#Pxl" );
    desc6.putUnitDouble( idVrtc, idPxl, height );
    var idMd = charIDToTypeID( "Md  " );
    var idgrmT = charIDToTypeID( "grmT" );
    var idMode = charIDToTypeID( mode );
    desc6.putEnumerated( idMd, idgrmT, idMode );
    if ( invert ) {
        var idInvr = charIDToTypeID( "Invr" );
        desc6.putBoolean( idInvr, true );
    }
    if ( transparent ) {
        var idalpT = charIDToTypeID( "alpT" );
        desc6.putBoolean( idalpT, true );
    }
    var idmulS = charIDToTypeID( "mulS" );
    list2.putObject( idmulS, desc6 );
    desc5.putList( idmulK, list2 );
    var idgraD = charIDToTypeID( "graD" );
    desc4.putObject( idUsng, idgraD, desc5 );
    executeAction( idImpr, desc4, ShowDialog() );
}

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

// end GradientImport.jsx
