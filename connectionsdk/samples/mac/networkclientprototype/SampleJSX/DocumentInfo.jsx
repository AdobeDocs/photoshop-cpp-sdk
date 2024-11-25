// Sample code for getting document info with sendDocumentInfoToNetworkClient

var idNS = stringIDToTypeID( "sendDocumentInfoToNetworkClient" );
var desc = new ActionDescriptor();

//All parameters are optional

// "version:: optional requested version (you always get the current version back, but this does a sanity check, and errors on an incompatible version)
desc.putString( stringIDToTypeID( "version" ), "1.6.1");

// ----------------------------------------------------------------
//Target of info request (by default gets info for active document)

//"documentID" : optional document id, uses active doc if not specified
//desc.putInteger( stringIDToTypeID( "documentID" ), 42 ); 

// Photoshop 16.1 and later
// "placedID" :  optional. reference smart object(s) within the document
//list of up to five placed IDs that are a path into the smart object hierarchy (each successive id is a smart object nested
//in the previous id.  Similar to placedID in sendLayerThumbnailToNetworkClient.
// of "ID" from layer:smartObject:{} or "placedID" from "image:placed:[{}]"


//var placedList = new ActionList();
//placedList.putString( "9d79145d-ba20-1178-860a-9aaf4074c344" ); 
//desc.putList( stringIDToTypeID( "placedID" ), placedList );

//16.1 and later
// "layerID","firstLayer","lastLayer": 16.1 and later, just get info for these layers
// default is to use all layers.  Similar to sendLayerThumbnailToNetworkClient
// "selectedLayers" : default is false, just get info for the selected layer(s)

//desc.putInteger( stringIDToTypeID( "layerID" ), 5 );
//-or-
//desc.putInteger( stringIDToTypeID( "firstLayer" ), 2 );
//desc.putInteger( stringIDToTypeID( "lastLayer" ), 5 );
//-or-
desc.putBoolean( stringIDToTypeID( "selectedLayers" ), true);

// ----------------------------------------------------------------

// "expandSmartObjects": default is false, recursively get doc info for any smart objects. can be slow
desc.putBoolean( stringIDToTypeID( "expandSmartObjects" ), true);

// "getTextStyles": default is false, return more detailed text info. can be slow
//desc.putBoolean( stringIDToTypeID( "getTextStyles" ), true);

// "getFullTextStyles" : default is false, return all text information (getTextStyles must also be true)
//desc.putBoolean( stringIDToTypeID( "getFullTextStyles" ), false);

// "getDefaultLayerFX": default is false, return all layer fx even if they are disabled
desc.putBoolean( stringIDToTypeID( "getDefaultLayerFX" ), true);

// "getCompLayerSettings": default is false, enumerate layer settings in layer comps
//desc.putBoolean( stringIDToTypeID( "getCompLayerSettings" ), true);

// "getPathData": default is false, return path control points for shapes
//desc.putBoolean( stringIDToTypeID( "getPathData" ), true);

//"imageInfo": return image-wide info (size, resolution etc.), default is !"selectedLayers" (true iff selectedLayers is false or missing)
desc.putBoolean( stringIDToTypeID( "imageInfo" ), true ); 

//"compInfo": return comp info in "comps" array, default is true, default is !"selectedLayers" (true iff selectedLayers is false or missing)
desc.putBoolean( stringIDToTypeID( "compInfo" ), true );

//"layerInfo": return layer info in "layers" array, default is true
desc.putBoolean( stringIDToTypeID( "layerInfo" ), true );

//16.1 and later "includeAncestors": include surrounding layer groups if doing selected layers/range/single layer id. default is true
//should only be used with single layers (otherwise grouping may not be accurate)
//desc.putBoolean( stringIDToTypeID( "includeAncestors" ), false );

// ----------------------------------------------------------------

//"whichInfo": allows more specificity of the attributes returned
//generally follows format of the returned info (see JSON info format)
//helps to reduce the calcuation needed if you just need certain info or certain layers.

//Version notes
//In 16.0 and earlier
//  	Both "layers" and "comps" are required
//		layers:text or layers:path would force expansion (getTextStyles, getPathData to true)
//		presence of whichInfo:placed forces expandSmartObjects to true, if it's absent expandSmartObjects is forced to false
//		individual image attributes aren't supported
//		to specify a layer, layerID/firstLayer/lastLayer need to go in whichInfo->layers
//		layerID needs to be 0, or a valid layer id.
//in 16.1 and later, 
//		Both "layers" and "comps" are optional (if missing no layer or comp info is returned at all)
//		the getTextStyles/getPathData/expandSmartObjects flags are always respected for layers:text, path
//		layerID/firstLayer/lastLayer are read from top level (or from whichInfo.layers if layerID/firstLayer/lastLayer is present there)
//		layerID is optional

//examples

//get just the layer smartObject info

//whichInfo:{
//layers:{smartObject:true}
//}

//get just the comp layer settings

//whichInfo:{
//comps:{layerSettings:true}
//}

//get all info for the layer with layerID, id (you can specify layerID in the top level descriptor in 16.1 and later)
//whichInfo:{
//layers:{allInfo:true}
//}

//get just the layer text info 
//whichInfo:{
//layers:{text:true}
//}

//get just the image size
//whichInfo:{
//bounds:true
//}

//equivalent to no whichInfo (get all info)
//whichInfo:{
//
//allInfo:true
//layers:{allInfo:true}
//comps:{allInfo:true}
//}

var whichInfoDesc = new ActionDescriptor();
var layerInfoDesc = new ActionDescriptor();
var compInfoDesc = new ActionDescriptor();


//layer target qualifier
//Version 16.0 and earlier: specify the layer or layers for whichInfo in layers descriptor.
//As noted above, n 16.1 and later, you also can specify layerID/firstLayer/lastLayer in the top level descriptor
//In 16.0 and earlier, layerID must be 0 or a valid layerID
//Version 16.1: if any layerID/selectedLayers/firstLayer/lastLayer is here, it will override the top level descriptor

layerInfoDesc.putInteger( stringIDToTypeID( "layerID" ), 0 );  //use 0 if multiple layers
//and
//layerInfoDesc.putBoolean( stringIDToTypeID( "selectedLayers" ), true ); 
//-or- a range:
layerInfoDesc.putInteger( stringIDToTypeID( "firstLayer" ), 2 );
layerInfoDesc.putInteger( stringIDToTypeID( "lastLayer" ), 5 );

//layer attributes
//Specify which info to get.  Add true for each param from JSON layer object you want
//allInfo gets everything
//layerInfoDesc.putBoolean( stringIDToTypeID( "allInfo" ), true ); 
//-or- one or more individual attributes, e.g.

layerInfoDesc.putBoolean( stringIDToTypeID( "type" ), true ); 
layerInfoDesc.putBoolean( stringIDToTypeID( "name" ), true ); 
// layerInfoDesc.putBoolean( stringIDToTypeID( "protection" ), true ); 
// layerInfoDesc.putBoolean( stringIDToTypeID( "bounds" ), true ); 
// layerInfoDesc.putBoolean( stringIDToTypeID( "visible" ), true ); 
// layerInfoDesc.putBoolean( stringIDToTypeID( "clipped" ), true ); 
// layerInfoDesc.putBoolean( stringIDToTypeID( "fill" ), true ); 
// layerInfoDesc.putBoolean( stringIDToTypeID( "blendOptions" ), true ); 
// layerInfoDesc.putBoolean( stringIDToTypeID( "layerEffects" ), true ); 
// layerInfoDesc.putBoolean( stringIDToTypeID( "strokeStyle" ), true ); 
// layerInfoDesc.putBoolean( stringIDToTypeID( "mask" ), true ); 
// layerInfoDesc.putBoolean( stringIDToTypeID( "pixels" ), true ); 
// layerInfoDesc.putBoolean( stringIDToTypeID( "generatorSettings" ), true ); 
// layerInfoDesc.putBoolean( stringIDToTypeID( "path" ), true ); 
// layerInfoDesc.putBoolean( stringIDToTypeID( "text" ), true ); 
// layerInfoDesc.putBoolean( stringIDToTypeID( "smartObject" ), true ); 
layerInfoDesc.putBoolean( stringIDToTypeID( "artboard" ), true ); 

//comp attributes
//include layer settings in comp reporting
compInfoDesc.putBoolean( stringIDToTypeID( "layerSettings" ), true ); 

//image attributes
//16.1 only (16.0 and earlier will ignore these except for placed which forces the value of expandSmartObjects)

//whichInfoDesc.putBoolean( stringIDToTypeID( "resolution" ), true );
//whichInfoDesc.putBoolean( stringIDToTypeID( "globalLight" ), true );
whichInfoDesc.putBoolean( stringIDToTypeID( "file" ), true );
whichInfoDesc.putBoolean( stringIDToTypeID( "placed" ), true ); //16.1 ignores, uses expandSmartObjects
//whichInfoDesc.putBoolean( stringIDToTypeID( "selection" ), true );
//whichInfoDesc.putBoolean( stringIDToTypeID( "generatorSettings" ), true );
whichInfoDesc.putBoolean( stringIDToTypeID( "bounds" ), true );

//16.0 and earlier requires both layers/comps, in 16.1 and later they are optional (missing implies no comp/layer info)

whichInfoDesc.putObject( stringIDToTypeID( "layers" ), stringIDToTypeID( "layers" ), layerInfoDesc );
whichInfoDesc.putObject( stringIDToTypeID( "comps" ), stringIDToTypeID( "comps" ), compInfoDesc );
whichInfoDesc.putBoolean( stringIDToTypeID( "cloudWorkareaDirectory" ), true );
desc.putObject( stringIDToTypeID( "whichInfo" ), stringIDToTypeID( "whichInfo" ), whichInfoDesc );

executeAction( idNS, desc, DialogModes.NO );
