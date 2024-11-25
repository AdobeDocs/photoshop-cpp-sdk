// Version 2020.10.5

// Listener example now ignores certain events that Photoshop emits.
// Use this script to turn them all back on or pick and choose.

try {
    
var listenerID = stringIDToTypeID("AdobeSDK Listener");

// if you want to see that event subtract it from the default list
var keySubID = stringIDToTypeID("subtract");

// if you do not want to see an event add it to the default list
// var keyAddID = stringIDToTypeID("add");

// currently ignore all of these
var seeThemAll = ["notify",
                  "MRUFileListChanged",
                  "modalStateChanged",
                  "homeScreenVisibilityChanged", 
                  "nglProfileChanged",
                  "layersFiltered", 
                  "fontListChanged", 
                  "toolModalStateChanged",
                  "invokeCommand",
                  "historyStateChanged",
                  "featureInfo"
                  ];

// I make you do it one at a time instead of having the plugin handle a list
// Somebody had to be lazy, I picked the C++ me instead of the JavaScript me
for ( var i = 0; i < seeThemAll.length; i++ ) {
    var d = new ActionDescriptor;
    d.putString(keySubID, seeThemAll[i]);
    executeAction(listenerID, d, DialogModes.NO);
}

} catch(e) {
    alert(e + " : " + e.line);
}

