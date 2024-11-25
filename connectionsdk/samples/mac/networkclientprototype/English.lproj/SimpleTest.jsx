
// built-ins:

// GTSubscribe(eventString) => subscribe to event (e.g. "imageChanged")
// GTSubscribeImageChanged() => GTSubscribe("imageChanged")
// GTSendDocInfo(docID) => perform sendDocumentInfoToNetworkClient on specified doc, sends JSON describing doc
// GTSendFrontDocInfo() => perform sendDocumentInfoToNetworkClient on front doc
// GTWriteLog() => append/write to a log file
// GTCloseFrontDoc()


// First each file, we call the procedures passed to GTTest:
// "init" is called before the first file
// "test" is called for each object one the file.
// any javascript results from "test" (return "result"; for example) are sent to "handleResponse"
// any image results (from sendLayerThumbnailToNetworkClient or sendDocumentThumbnailToNetworkClient), are sent to "handleResponse" as a TIFF File.
// any uncaught exceptions from any of the above, are sent to "handleException"

// "handleResponse" and "handleException" are optional (but "test" is required)

// GTTest will keep waiting until every handleResponse has been called and returns nothing (undefined), or there is an uncaught exception in one of the handlers

// The whole javascript file is executed once per each test/response (so mind any side-effects and execution time)
// also don't put any returns here, as extra commands are appended to the end

function ThrowErr(b,err,s) {if (!b) {GTWriteLog(err,s); throw err;}}

function GetDocInfo(response)
    {
    var info;
    if (response.search("imageChanged") >= 0)
        info = eval("(" + response.replace("imageChanged\r","") + ")");
    else
        info = eval("(" + response + ")"); //eval as JSON

    if (info == undefined) {
        GTWriteLog("\nINVALID JSON:",response);
       throw "ERROR: invalid doc info/change, non-JSON";
        }
        
    if (info.id == undefined && info.documentID == undefined) {
        GTWriteLog("\nINVALID doc format:",info);
        throw "ERROR: invalid doc info/change, id or documentID not found";
    }
    return info;
   }

function GetLayers(docInfo)
    { if (  docInfo.layerEvents != undefined )  return docInfo.layerEvents;  else if (  docInfo.layers != undefined )  return docInfo.layers; }

function GetLayerID(layerInfo) {  if ( layerInfo.layerID != undefined )  return layerInfo.layerID; else  return layerInfo.id; }

function GetObject(response)  {
    var fileInfo = eval("(" + response + ")"); //eval as JSON
    if (fileInfo == undefined)  throw "ERROR: invalid doc info/change, non-JSON";
    return fileInfo;
   }

function IsDocClosed(info)  { return info.closed != undefined ||  (info.imageEvents != undefined && info.imageEvents.imageClosed != undefined); }

function GetSmartObject(layer) { 
    if (layer.layerChangedPlacedContent)
        return layer.layerChangedPlacedContent;
    else
        return layer.smartObject;
}

GTTest(
    //first test, writes headers, subscribes/listens to changes
    {
    init:
		function(){
			GTClearLog("==============================================", Date());
			},

    test: 
		function(filePath){
			GTWriteLog( "---------------------------------------------", "Testing File:", filePath);
			GTSubscribeImageChanged("1.0.0");
			},
		
    handleResponse:
        function(response){

            var continueTest = true; //some defined value to keep the test alive
        
            //response should either be:
            //undefined, the result of  GTSubscribeImageChanged (the last thing in test)
            //"keepMoving" what we return to continue the test
            //or a doc info JSON response for the actual image changed subscription
            //we end the test when we see a close event by returning nothing

            if (response != undefined && response != "keepGoing") {
                var info = GetDocInfo(response);
                GTWriteLog("\ndoc change:",info);
                if (IsDocClosed(info))
                    continueTest = false; //return nothing to complete the test.
            }

            if (continueTest)
                return "keepGoing";  //some defined value to keep the test alive
            else
                return;
           }
    },
	//second "test". This opens the current file (from the command line) as a photoshop document,
	//then requests the document info (returned as JSON), when it receives the document info,
	//it writes it to the log, and then closes the document, and ends the test (by returning nothing)
	//Closing the doc sends an imageClosed event, which ends the first test.

    {
    test:  function(filePath) {
            
            if (app.open (new File(filePath)) == null)
                throw "ERROR: file not found";
            
            GTSendDocInfo(null,true,"1.0.0"); //get front doc, with smart objects
            },

    handleResponse:
    
            //response should either be:
            //undefined, the result of GTSendDocInfo (the last thing in test)
            //"keepMoving" what we return to continue the test
            //or a doc info JSON response for GTSendDocInfo
            //or a fileInfo (from the sendDocumentStreamToNetworkClient)
            
            //once we see the fileInfo (or there is no smart objects, 
            //we close the doc, and return nothing, which ends this test.
            //the first test ends when it sees the doc closed change
            
            function(response,fileInfo){

                var continueTest = true;

				if (response == undefined || response == "keepGoing") {

                }
				else if (fileInfo != undefined) {
                    if(!response.exists) //if fileInfo is defined, first param must be File
                        throw "ERROR: temp file for smart object not found";
					GTWriteLog("\nsmart object file info:",GetObject(fileInfo));
                    continueTest = false; //done
				}
				else  {
                    var docInfo = GetDocInfo(response);

					GTWriteLog("\ndoc info:",docInfo);

                    var layers = GetLayers(docInfo);
                    continueTest = false;
                        
					for( var ll = 0; ll < layers.length; ll++ ) {
                        var layer = layers[ll];
                        var smartObj = GetSmartObject(layer);
                        if (smartObj != undefined) {
                            var id = GetLayerID(layer);
                            GTWriteLog("\nlayer id:", id, "\nsmart object:",smartObj);
                                
                            var idNS = stringIDToTypeID( "sendDocumentStreamToNetworkClient" );
                            var desc1 = new ActionDescriptor();
                            desc1.putInteger( stringIDToTypeID( "layerID" ), id);
                            executeAction( idNS, desc1, DialogModes.NO );

                            //try again with some of the optional parameters.
                            //this writes a file range to a temp file local to the server.
                            //(this is assumes file size is at least 1024k)
                            
                            desc1.putLargeInteger( stringIDToTypeID( "position" ), 512 );
                            desc1.putLargeInteger( stringIDToTypeID( "size" ), 512 );
                            desc1.putBoolean( stringIDToTypeID( "path" ), true);
                            executeAction( idNS, desc1, DialogModes.NO );
                            continueTest = true; //if we find a smart object, finish test when info comes back
                        }
					}
				}
			   

           if (continueTest)
                return "keepGoing";  //some defined value to keep the test alive
            else
                GTCloseFrontDoc(); //return nothing to complete this test, and send a close event to end the first test
            }
    }
);