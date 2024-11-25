// Version: 2020.9.29

main();

function main() {

LogMessage( "Starting test: FindPluginInSystemInfo:" );

var fileLog = new File( "~/Desktop/FindPluginInSystemInfo.log" );
fileLog.open( "w" );
var errors = 0;
var sysInfo = app.systemInformation;
var failStart = sysInfo.search( "Plug-ins that failed to load:" );
if ( failStart == -1 ) {
    // Adobe Photoshop 2021 renamed Plug-ins to Plugins
    failStart = sysInfo.search( "Plugins that failed to load:" );
    if ( failStart == -1 ) {
        errors++;
        fileLog.writeln( "No failed to load found" );
    }
}
var failEnd = sysInfo.search( "Extensions:");
if ( failEnd == -1 ) {
    errors++;
    fileLog.writeln( "No flash found" );
}
if ( failEnd < failStart ) {
    errors++;
    fileLog.writeln( "failEnd < failStart" );
}
   
// TODO double check the statistics version, "Generic Plug-in" and 3dsimulator 
// one plug-in could have more than one entry, statistics and SelectoramaShape
// some plug-ins don't show up, see above TODO to confirm
var pluginList = [ 
 "AutomationFilter",     "ColorMunger",         "Dissolve",    "Getter", 
 "GradientImport",       "Hidden",              "History",     "LayerFormat", 
 "Listener",             "Measurement Sample",  "NearestBase", "Outbound", 
 "Poor Man's Type Tool", "Propetizer",          "Selectorama", "Shape", 
 "SimpleFormat",         "Entropy",             "Kurtosis",    "Maximum",
 "Mean",                 "Median",              "Minimum",     "Range", 
 "Skewness",             "Standard Deviation",  "Summation",   "Variance", 
 "TextAuto",             "TextFormat" ];
                   
if ( $.os.search(/windows/i) != -1 )
	pluginList.push( "MFC PlugIn" );

var pluginIndex = [];

for ( var i = 0; i < pluginList.length; i++ ) {
    pluginIndex[i] = sysInfo.search( pluginList[i] );
    // $.writeln( pluginList[i] + " = " + pluginIndex[i] );
    if ( pluginIndex[i] == -1 ) {
        errors++;
        fileLog.writeln( "Plug in not found: " + pluginList[i] );
    }
    else if ( pluginIndex[i] > failStart && pluginIndex[i] < failEnd ) {
        errors++;
        fileLog.writeln( "pluginIndex[i] > failStart && pluginIndex[i] < failEnd " +  pluginList[i] );
    }
}

fileLog.close();

if (errors && ! BringToFront() ) {
	fileLog.execute();
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
}

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

// end FindPluginInSystemInfo.jsx
