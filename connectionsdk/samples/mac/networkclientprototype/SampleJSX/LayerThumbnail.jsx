
// Sample code for getting a layer thumbnail with sendLayerThumbnailToNetworkClient
// By running this file w/ Send JavaScript to Photoshop.. in the Echo client, the thumb 
// for the open document in Photoshop. will be displayed in the window.

var idNS = stringIDToTypeID( "sendLayerThumbnailToNetworkClient" );
var desc1 = new ActionDescriptor();

// ------------------------------------------
//Document or smart object to address:

//"documentID" : optional document id, uses active doc if not specified
//desc1.putInteger( stringIDToTypeID( "documentID" ), 42 ); 

// "placedID" : Photoshop 16.1 and later, optional. reference smart object(s) within the document
//series of "ID" from layer:smartObject:{} or "placedID" from "image:placed:[{}]"

//var placedList = new ActionList();
//placedList.putString( "37b45136-b6b4-1178-944f-cc2a4c1f14fc", ); 
//placedList.putString( "354d49de-b6b4-1178-944f-cc2a4c1f14fc" ); //if nested. within the previous smart object
//placedList.putString( "354d49dd-b6b4-1178-944f-cc2a4c1f14fc" ); //etc, up to 5 ids
//desc1.putList( stringIDToTypeID( "placedID" ), placedList );

// ------------------------------------------
// "width","height": optional maximum dimensions
// composite pixels are scaled to fit these dimensions if necessary
// compare with "transform" below, which is applied to the layers before compositing.

desc1.putInteger( stringIDToTypeID( "width" ), 2048 );
desc1.putInteger( stringIDToTypeID( "height" ), 2048 );

// ------------------------------------------
// "format": 2 means raw pixels (only format supported currently)

desc1.putInteger( stringIDToTypeID( "format" ), 2 );

// ------------------------------------------
// Which layers to include (all layers are included by default)
// Three basic ways to specify layers:

// "selectedLayers": include selected layers:
desc1.putBoolean( stringIDToTypeID( "selectedLayers" ), true );

// "firstLayer","lastLayer": include a layer range: (range needs to include both bounding layers of layer groups (or neithers)
 //desc1.putInteger( stringIDToTypeID( "firstLayer" ), 2 );
 //desc1.putInteger( stringIDToTypeID( "lastLayer" ), 5 );

// "layerID" : include a single layer or group with layerID
// desc1.putBoolean( stringIDToTypeID( "layerID" ), layerID );

// absent any layer specifiers, all layers within the document reference are included.

// ------------------------------------------
// Setting modifiers

//The firstLayer/lastLayer form supports these  optional args (see GetLayerRangeComposite in PSLAPI.h for more info)
//to modify layer/image settings before obtaining thumbnail
//layerSettings is an action list with  one action descriptor per each layer in firstLayer/lastLayer
//if the descriptor has a "layerSettings" key, the descriptor will be used to modify the layer before the thumbnail is retrieved:
//"layerSettings" keys:
//  enabled:boolean (make the layer visible/invisible)
//  blendOptions: (blending settings to use, follows same format as blendOptions imageChanged event)
//  layerEffects: (fx settings to use, follows same format as layerEffects imageChanged event)
//  offset:{horizontal:x vertical:y} (integer offset of layer)
//  vectorMask:{enabled:bool, offset:{horizontal:x vertical:y}, invert:boolean}
//  FXRefPoint:{horizontal:x vertical:y}

//desc1.putObject( stringIDToTypeID( "layerSettings" ), layerSettingsList );
//desc1.putObject( stringIDToTypeID( "imageSettings" ), imageSettingsObject );

//Sample of layerSettings List for layer indeces 1 to 6, that shows every other layer:
//var oddDesc = new ActionDescriptor();
//var evenDesc = new ActionDescriptor();
//var firstLayer = 1;
//var lastLayer = 6;
//var evenDesc = new ActionDescriptor();
//var lsID = stringIDToTypeID( "layerSettings" );
//var evenDescSettings = new ActionDescriptor();
//evenDescSettings.putBoolean( stringIDToTypeID( "enabled" ), false );
//evenDesc.putObject( lsID, lsID, evenDescSettings );
//var settingsList = new ActionList();
//for (var ll = firstLayer; ll <= lastLayer; ++ll)
//{settingsList.putObject( lsID, (ll & 1) ? oddDesc : evenDesc );}
//desc1.putInteger( stringIDToTypeID( "firstLayer" ), firstLayer );
//desc1.putInteger( stringIDToTypeID( "lastLayer" ), lastLayer );
//desc1.putList( stringIDToTypeID( "layerSettings" ), settingsList );


// ------------------------------------------------------------
//"transform": transform to applied to layers before compositing. (default is to use no transform)
//Requires an image duplication and transform on a thread, so it's much slower than using
//width/height above, but gives higher quality results for text, PDF/PSD smart objects, and vector layers.
//If a smart object is PSD/PSD,and has no smart filters, it is re-rendered using the transform (for maximum quality)
//other smart object formats are scaled using the document pixels (due to thread-safe issues)

var transform1 = new ActionDescriptor();

//width/height will scale dimension by a percentage (e.g 50%)
transform1.putDouble( stringIDToTypeID( "width" ), 50.000000 );
transform1.putDouble( stringIDToTypeID( "height" ), 50.000000 );

// miscellaneous options:

//"forceDumbScaling": scale "smart" shapes, such as rounded rects, as normal paths. default is false. 
transform1.putBoolean( stringIDToTypeID( "forceDumbScaling" ), false );

//"forceSmartPSDPixelScaling": default is false. true scales psd smart objects as pixels instead of layers/vectors
// (much faster, less memory used, good for preview) 
transform1.putBoolean( stringIDToTypeID( "forceSmartPSDPixelScaling" ), false );

//"interpolation": pixel interpolation to use for the transform (like image scale) Default is to use the current Photoshop global preference.
transform1.putEnumerated( stringIDToTypeID( "interpolation" ), stringIDToTypeID( "interpolationType" ), stringIDToTypeID( "automaticInterpolation" ) );

desc1.putObject( stringIDToTypeID( "transform" ), stringIDToTypeID( "transform" ), transform1 );

// ------------------------------------------------------------

//"interpolation": interpolation to use for any width/height scaling (like image scale)
desc1.putEnumerated( stringIDToTypeID( "interpolation" ), stringIDToTypeID( "interpolationType" ), stringIDToTypeID( "nearestNeighbor" ) );

//"clipToDocumentBounds": clip/scale the document bounds with the layers
desc1.putBoolean( stringIDToTypeID( "clipToDocumentBounds" ), false );

// ------------------------------------------------------------
// include*:
// additional layers to include in the thumbnail, to make the thumbnail better match the appearance in Photoshop
// "includeVisible": include the layer type if visible, includeNone: don't include the layer type,  includeAll: force visible and include.
// "includeAdjustors": include adjustment layers above the specified layers.
// "includeAncestors": include group layers that enclose the specified layers (to inherit group masks, blend modes, fx, etc.)
// "includeChildren": include child layers if specified layer is a group
// "includeClipBase": include clip base layer if specified layer is a clipped layer (if false, layer is )
// "includeClipped": include clipped layers if specified layer is a clip base layer (clips other layers)

// defaults are specified below:

desc1.putEnumerated( stringIDToTypeID( "includeAdjustors" ), stringIDToTypeID( "includeLayers" ), stringIDToTypeID( "includeVisible" ) );
desc1.putEnumerated( stringIDToTypeID( "includeAncestors" ), stringIDToTypeID( "includeLayers" ), stringIDToTypeID( "includeAll" ) );
desc1.putEnumerated( stringIDToTypeID( "includeChildren" ), stringIDToTypeID( "includeLayers" ), stringIDToTypeID( "includeVisible" ) );
desc1.putEnumerated( stringIDToTypeID( "includeClipBase" ), stringIDToTypeID( "includeLayers" ), stringIDToTypeID( "includeAll" ) );
desc1.putEnumerated( stringIDToTypeID( "includeClipped" ), stringIDToTypeID( "includeLayers" ), stringIDToTypeID( "includeVisible" ) );

// ------------------------------------------------------------
// "sendThumbnailPixels": send the pixels, default to true

desc1.putBoolean( stringIDToTypeID( "sendThumbnailPixels" ), true );

// "sendThumbnailBounds": send the bounds (as JSON), default to false

desc1.putBoolean( stringIDToTypeID( "sendThumbnailBounds" ), true );

// "sendThumbnailMissingFonts": (default false) sends a list of missing fonts (as JSON) that were required to render the thumb
// if you use a transform, any text layers need to be re-rendered, and therefore may fail if fonts are missing.

desc1.putBoolean( stringIDToTypeID( "sendThumbnailMissingFonts" ), true );


// ------------------------------------------------------------
//"convertToWorkingRGBProfile": optional, if true, the thumbnail is converted to the working RGB space in "Color Settings..."
//desc1.putBoolean( stringIDToTypeID( "convertToWorkingRGBProfile" ), true );

// ------------------------------------------------------------
//
//"useICCProfile": optional, Photoshop 16.1, and later. convert to profile with this name, srgb is "sRGB IEC61966-2.1"

desc1.putString( stringIDToTypeID( "useICCProfile" ), "sRGB IEC61966-2.1");

// ------------------------------------------------------------

//"thread": optional, if true, build the thumbnail on a thread.  By default, the thumbnail is threaded if there is a "transform", otherwise
//it is done on the main thread until a user key/mouse event occurs, then it is cancelled, and restarted on a thread
//thread can be used to override the default (either force the thumb to be started on the main thread or a background thread)
//it may help performance if you know that the thumbnail is either quick (best done on main thread) or slow (best done on background)
//there is a slight memory/performance penalty for threading in that the layer data must be copied before it is threaded

//desc1.putBoolean( stringIDToTypeID( "thread" ), true );

// ------------------------------------------------------------

//“allowDither”/“useColorSettingsDither”: Photoshop 15.0 and later. default to true.
//If 
//1) “allowDither” is true
//2) and either “useColorSettingsDither” is false,
//       or dither is checked in the global color settings (Color Settings… in Photoshop)
//3) and any color/depth conversion would be “lossy” (16 to 8 bit, CMYK to RGB, etc)
//
//then dithering will occur, otherwise there will be no dithering.

//e.g. force dither off.
//desc1.putBoolean( stringIDToTypeID( "allowDither" ), false ); 
//desc1.putBoolean( stringIDToTypeID( "useColorSettingsDither" ), false );

// ------------------------------------------------------------

//"compID"/"compIndex": layer comp id/index to use (this comp is temporarily applied before getting thumbnail)
//You can use sendDocumentInfoToNetworkClient to iterate comps to gather the id/index
//desc1.putInteger( stringIDToTypeID( "compID" ), compID );
//or
//desc1.putInteger( stringIDToTypeID( "compIndex" ), compIndex );



executeAction( idNS, desc1, DialogModes.NO );