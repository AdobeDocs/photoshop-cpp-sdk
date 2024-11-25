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

/**  Filename: ToolStringConverter.java
	@author Thomas Ruark, Photoshop Engineering, Adobe Systems Incorporated
*/

package corp.adobe.photoshopevents;


/**
 * Two strings in resources map the names for humans and 
 * IDs for machines. The main activity needs to pull them
 * from the resources and initialize the arrays. 
 */
class ToolStringConverter {
	
	static public String [] mToolNames;
	
	static public String [] mToolIDs;
	
	static public String UItoToolID(String inTool) {
		if (mToolNames == null || mToolIDs == null || mToolIDs.length != mToolNames.length) 
			return "";
		for (int i = 0; i < mToolIDs.length; i++)
			if (mToolNames[i].compareTo(inTool) == 0)
				return mToolIDs[i];
		return "";
	}
	
	static public String ToolIDtoUI(String inID) {
		if (mToolNames == null || mToolIDs == null || mToolIDs.length != mToolNames.length) 
			return new String("");
		for (int i = 0; i < mToolIDs.length; i++)
			if (mToolIDs[i].compareTo(inID) == 0)
				return mToolNames[i];
		return "";
	}
}
