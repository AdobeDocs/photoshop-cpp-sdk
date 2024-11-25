var __generatorTestLogFile = File(__generatorTestLogFilePath);
var __generatorTestProcs = [];

function GTSendDocInfo(docID,sendAllInfo,version) {
    var idNS = stringIDToTypeID( "sendDocumentInfoToNetworkClient" );
    var desc1 = new ActionDescriptor();
	if (docID)
		desc1.putInteger( stringIDToTypeID( "documentID" ), docID);
     
	if (sendAllInfo) {   
		desc1.putBoolean( stringIDToTypeID( "expandSmartObjects" ), true);
    	desc1.putBoolean( stringIDToTypeID( "getTextStyles" ), true);
    	desc1.putBoolean( stringIDToTypeID( "getPathData" ), true);
    	desc1.putBoolean( stringIDToTypeID( "getCompLayerSettings" ), true);
	}
	if (version != undefined) {
		desc1.putString( stringIDToTypeID( "version" ), version);
	}
    executeAction( idNS, desc1, DialogModes.NO );
}

function GTSubscribe(eventStr,version) {
    var idNS = stringIDToTypeID( "networkEventSubscribe" );
    var desc1 = new ActionDescriptor();
    desc1.putClass( stringIDToTypeID( "eventIDAttr" ), stringIDToTypeID(eventStr));
 	if (version != undefined) {
		desc1.putString( stringIDToTypeID( "version" ), version);
	}
   executeAction( idNS, desc1, DialogModes.NO );
}

function GTSubscribeImageChanged(eventStr) { GTSubscribe( "imageChanged" );}

function GTCloseFrontDoc() { try{activeDocument.close(SaveOptions.DONOTSAVECHANGES)}catch(e){} }

function GTStringify(o){    
    if (!o) return 'null';
	if (typeof o != "object") return o;
    var k="",na=typeof(o.length)=="undefined"?1:0,str="";
    for(var p in o){
        if (na) k = "'"+p+ "':";
        if (typeof o[p] == "string") str += k + "'" + o[p]+"',";
        else if (typeof o[p] == "object") str += k + GTStringify(o[p])+",";
        else str += k + o[p] + ",";
    }
    if (na) return "{"+str.slice(0,-1)+"}";
    else return "["+str.slice(0,-1)+"]";
}

function GTWriteLog() {
	if(__generatorTestLogFile.exists) {
		__generatorTestLogFile.open ("e"); __generatorTestLogFile.seek (0,2);
	} else {
		__generatorTestLogFile.open ("w");
	}
	__generatorTestLogFile.encoding = "UTF8";
   for( var i = 0; i < arguments.length; i++ )
        if (arguments[i])
		__generatorTestLogFile.write(GTStringify(arguments[i]) + "\n");
	__generatorTestLogFile.close();
}

function GTClearLog() {__generatorTestLogFile.open ("w"); GTWriteLog.apply(this,arguments); }

function GTRequire(err) {GTWriteLog(err); throw err;}

function GTTest(testFuncs) {

    for( var i = 0; i < arguments.length; i++ ) {
        if (arguments[i].test == undefined && arguments[i].init == undefined)
            GTRequire("ERROR: you at least need a test or init in the GTTest objects, see SimpleTest.jsx for an example");

        if (arguments[i].test == undefined)
            arguments[i].test = function(){};

        if (arguments[i].init == undefined)
            arguments[i].init = function(){};

        if (arguments[i].handleResponse == undefined)
            arguments[i].handleResponse = function(){};

        if (arguments[i].handleException == undefined)
            arguments[i].handleException = function(e){GTWriteLog(e);};
            
         __generatorTestProcs.push(arguments[i]);
	}
}


