//
//  main.m
//  CocoaEchoClient
//
//  Created by Chris Parker on 4/25/05.
//  Copyright __MyCompanyName__ 2005. All rights reserved.
//

#import <Cocoa/Cocoa.h>

#include "EchoClient.h"
// ------------------------------------------------------------------------------------------------------

int main(int argc, char *argv[])
{
	int retVal = 0;
	bool batchMode = false;
	double timeOutSeconds = 30.0; //default is 30 seconds
    const char* javaScriptPath = NULL;
    bool init = true;

	//scan command line options, run in batch mode if there are files specified
	
	if (argc > 1)
		{
		NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];
		for (int ii = 1; ii < argc; ++ii)
			{
            if ( !strcmp(argv[ii],"-appPath") || !strcmp(argv[ii],"-password") || !strcmp(argv[ii],"-server")
                || !strcmp(argv[ii],"-logPath") || !strcmp(argv[ii],"-NSDocumentRevisionsDebugMode") )
                {
				//skip argument, we read these when initializing the server connection (see EchoClient init)
                ++ii;
                }
           else if (!strncmp(argv[ii],"-psn",4)) //sent by double-click
                {
                }
                
            else if ( !strcmp(argv[ii],"-timeout")  && ++ii < argc)
                {
                timeOutSeconds = atol(argv[ii]);
                }

            else if ( !strcmp(argv[ii],"-javaScript")  && ++ii < argc)
                {
				 //javascript to test with, must have functions GTTestOne,GTTestVerify,GTTestCleanup.
                 // See SimpleTest.jsx in the app resources for an example
                javaScriptPath = argv[ii];
                }
            else if (!strcmp(argv[ii],"-defaultTest"))
				{
				 //run a SimpleTest.jsx on Shapes.psd (included in the app resources)
				TestOneFile(NULL, javaScriptPath, timeOutSeconds, init);
				init = false;
				batchMode = true;
				}
            else if (!strncmp(argv[ii],"-",1))
				{
				 //run a SimpleTest.jsx on Shapes.psd (included in the app resources)
                 NSLog(@"\n"
                        "Usage: CocaEchoClient\n"
                        "       -appPath [path to photoshop.app to launch]\n"
                        "       -password [server password]\n"
                        "       -server [Photoshop server name]\n"
                        "       -logPath [GTAppendLog log file path]\n"
                        "       -timeout [seconds to wait for reply from server]\n"
                        "       -defaultTest\n"
                        "       -javaScript [path to javascript test to run]\n"
                        "       [testFilePath]\n"
                        "\n"
                        "you may specify multiple -javaScript or [testFilePath] args\n"
                        "[testFilePath] runs a test using the last -javaScript arg,\n"
                        "or SimpleTest.jsx in the app package if no -javaScript arg is specified\n"
                        "-defaultTest runs a test on Shapes.psd from the app package\n"
                        "See SimpleTest.jsx for instructions on how to format a -javaScript arg\n"
                        "Basically, it needs to contain 3 functions:GTTestOne(testFilePath),\n"
                        "GTTestVerify(resultStringFromGTTestOne) and GTTestCleanup(testFilePath)\n"
                        "mail twright@adobe.com for more help/info"
                        );
				return 1;
				}
           else
				{
				 //otherwise assume a file path.
                int testResult = TestOneFile(argv[ii], javaScriptPath, timeOutSeconds,init);
				init = false;
                if (testResult)
                  retVal = testResult;
                
				batchMode = true;
				}
			}
		[pool release];
		}
	
	//if there are no command-line options to process, launch with standard Cocoa GUI
	
	if (!batchMode)
		retVal = NSApplicationMain(argc,  (const char **) argv);

	return retVal;
}
