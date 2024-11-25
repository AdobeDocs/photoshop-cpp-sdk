/**************************************************************************
	ADOBE SYSTEMS INCORPORATED
	 Copyright 2010 Adobe Systems Incorporated
	 All Rights Reserved.

	NOTICE:  Adobe permits you to use, modify, and distribute this file
	in accordance with the terms of the Adobe license agreement accompanying
	it.  If you have received this file from a source other than Adobe, then
	your use, modification, or distribution of it requires the prior written
	permission of Adobe.
**************************************************************************/

/**  Filename: EncryptDecrypt.java
	@author Thomas Ruark, Photoshop Engineering, Adobe Systems Incorporated
*/

import java.io.*;
import java.net.*;

public class PhotoshopCommandLine {

    static private boolean mShowMessages = true;

    public static void main(String[] args) throws IOException {
	
        try {
	    Socket photoshopSocket = null;
	    DataOutputStream outputStream = null;
	    DataInputStream inputStream = null;
	    if (args.length != 2) {
		System.err.println("Syntax is \"java PhotoshopCommandLine ServerName Password\" please try again.");
		return;
	    }
            photoshopSocket = new Socket(args[0], 49494);
            outputStream = new DataOutputStream(photoshopSocket.getOutputStream());
            inputStream = new DataInputStream(photoshopSocket.getInputStream());
	    
	    PhotoshopProtocol photoshopProtocol = new PhotoshopProtocol(args[1]);
	    BufferedReader bufferedReader = new BufferedReader(new InputStreamReader(System.in, "UTF-8"));
	    String userInput;
	    System.out.println("\nReady for input, type in JavaScript or type ? for commands: ");
	    while ((userInput = bufferedReader.readLine()) != null) {
		if (userInput.compareTo("?") == 0) {
		    ShowCommands();
		} else if (userInput.compareTo("q") == 0) {
		    break;
		} else if (userInput.compareTo("s") == 0) {
		    mShowMessages = ! mShowMessages;
		    photoshopProtocol.mShowMessages = mShowMessages;
		    System.out.println("Show messages is now: " + mShowMessages);
		} else {
		    byte[] bytesStr = photoshopProtocol.processJavaScript(userInput, outputStream, inputStream);
		    byte[] newBytesStr = new byte[bytesStr.length];
		    int j = 0;
		    for (int i = 0; i < bytesStr.length; i++) {
			if (bytesStr[i] != 13) {
			    newBytesStr[j++] = bytesStr[i];
			} else {
			    newBytesStr[j++] = ' ';
			}
		    }
		    String returnString = new String(newBytesStr);
		    System.out.println("Echo result is: " + returnString + "\nReady for input: ");
		}
	    }
	    bufferedReader.close();
	    outputStream.close();
	    inputStream.close();
	    photoshopSocket.close();
        } 
	catch (UnknownHostException e) {
            System.err.println("Don't know about host: " + args[0]);
            System.exit(1);
        } 
	catch (Exception e) {
            System.err.println("Couldn't get I/O for the connection to: " + args[0]);
            System.exit(1);
        }
    }
    
    public static void ShowCommands() {
		System.out.println("q for quit");
		System.out.println("s for messages toggle");
		System.out.println("? for this list");
    }
}
