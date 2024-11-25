// Sample code for getting properties on the target layers/doc

var classProperty = charIDToTypeID('Prpr');
var propNull = charIDToTypeID( 'null' );
var classNull = charIDToTypeID( 'null' );
var typeOrdinal = charIDToTypeID('Ordn');
var enumTarget = charIDToTypeID('Trgt');

var classApplication = charIDToTypeID('capp');
var classDocument = charIDToTypeID('Dcmn');
var classLayer = charIDToTypeID('Lyr ');

var propGeneratorSettings = stringIDToTypeID("generatorSettings");
var propJSON = stringIDToTypeID("json");
var propBla = stringIDToTypeID("bla");
var propProperty = stringIDToTypeID("property");
var propArtboard = stringIDToTypeID("artboard");
var propArtboards = stringIDToTypeID("artboards");
var propBounds = stringIDToTypeID("bounds");
var propName = charIDToTypeID('Nm  ');
var propLayerID = charIDToTypeID('LyrI');

var myPropKey = "myPropKey";

var keyTo = charIDToTypeID( 'T   ' );

var actionSet = charIDToTypeID( "setd" );
var actionGet = charIDToTypeID( "getd" );
var actionSendJSON = stringIDToTypeID( "sendJSONToNetworkClient" );

// ----------------------------------------------------------------

	//create reference parameter for a get/set action
	//This returns the "target" form, which is the active/selected doc/layer
	//a reference can be constructed from ids/indeces/names as well.
function targetRef(theClass,theProp) {

	var theRef = new ActionReference();

	theRef.putProperty(classProperty, theProp);
	theRef.putEnumerated(theClass, typeOrdinal, enumTarget);

	return theRef;
}

// ----------------------------------------------------------------

	//Get the value of property theProp from theRef, and send JSON over the backchannel
	//One would normally just use sendDocInfoToNetworkClient to do this in one step,
	//but this demonstrates how you can return the info in a descriptor and
	//process the info in ExtendScript, instead of, or in addition to, the receiving client.

function getRefValue(theRef,theProp) {

	var getDescriptor = new ActionDescriptor();
	getDescriptor.putReference(propNull,theRef);

		//when getting the json/generator settings prop
		//this lets us focus on just one key in generator settings
		//(this is how a plugin would get its own settings)
	if (theProp != undefined) {
		getDescriptor.putString(propProperty,theProp);
	}


	//do the get
	var desc = executeAction( actionGet, getDescriptor, DialogModes.NO );

	//sends JSON for the result over the back channel.
	executeAction( actionSendJSON, desc, DialogModes.NO );
}

// ----------------------------------------------------------------

	//Set the value of property theProp into theRef

function setRefValue(theRef,theVal,theProp) {

	var setDescriptor = new ActionDescriptor();
    setDescriptor.putReference(propNull,theRef);
	setDescriptor.putObject(keyTo,classNull,theVal);

		//when getting the json/generator settings prop
		//this lets us focus on just one key in generator settings
		//(this is how a plugin would get its own settings)
	if (theProp != undefined) {
    	setDescriptor.putString(propProperty,theProp);
	}

	//do the set
	executeAction( actionSet, setDescriptor, DialogModes.NO );
}


// ----------------------------------------------------------------

// Example 1, properties on target layer, document.

getRefValue(targetRef(classLayer,propLayerID));
getRefValue(targetRef(classLayer,propName));

getRefValue(targetRef(classLayer,propArtboard));
getRefValue(targetRef(classDocument,propArtboards));

// ----------------------------------------------------------------

// Example 2
// Getting JSON info from layers/documents (like sendDocInfoToNetworkClient, but returns it as a string)

getRefValue(targetRef(classLayer,propJSON));
getRefValue(targetRef(classDocument,propJSON));

// ----------------------------------------------------------------

// Example 3
// Getting/Setting, custom plugin properties on layers/documents
// myPropKey is a unique string for your plugin
// myPropVal is anything you wish to save in the layer/doc

var myPropVal = new ActionDescriptor();
myPropVal.putBoolean(propBla,true);

setRefValue(targetRef(classDocument,propGeneratorSettings), myPropVal, myPropKey);
setRefValue(targetRef(classLayer,propGeneratorSettings), myPropVal, myPropKey);

getRefValue(targetRef(classDocument,propGeneratorSettings), myPropKey);
getRefValue(targetRef(classLayer,propGeneratorSettings), myPropKey);
