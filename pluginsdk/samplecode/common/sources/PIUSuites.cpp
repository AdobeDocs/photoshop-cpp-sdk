// ADOBE SYSTEMS INCORPORATED
// Copyright  1993 - 2002 Adobe Systems Incorporated
// All Rights Reserved
//
// NOTICE:  Adobe permits you to use, modify, and distribute this 
// file in accordance with the terms of the Adobe license agreement
// accompanying it.  If you have received this file from a source
// other than Adobe, then your use, modification, or distribution
// of it requires the prior written permission of Adobe.
//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
//
//	File:
//		PIUSuites.cpp
//
//	Description:
//		Utility routines to acquire and release suites for your plug in
//
//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
//	Includes
//-------------------------------------------------------------------------------
#include "PIUSuites.h"

// global suite pointers
AutoSuite<PSActionDescriptorProcs> sPSActionDescriptor(kPSActionDescriptorSuite, kPSActionDescriptorSuiteVersion);
AutoSuite<PSActionDescriptorProcs3> sPSActionDescriptor3(kPSActionDescriptorSuite, kPSActionDescriptorSuiteVersion3);
AutoSuite<PSActionDescriptorProcs2> sPSActionDescriptor2(kPSActionDescriptorSuite, kPSActionDescriptorSuiteVersion2);
AutoSuite<PSActionControlProcs> sPSActionControl(kPSActionControlSuite, kPSActionControlSuiteVersion);
AutoSuite<PSActionControlProcs4> sPSActionControl4(kPSActionControlSuite, kPSActionControlSuiteVersion4);
AutoSuite<PSActionControlProcsOld> sPSActionControlOld(kPSActionControlSuite, kPSActionControlSuiteVersion3);
AutoSuite<PSBasicActionControlProcs> sPSBasicActionControl(kPSBasicActionControlSuite, kPSBasicActionControlSuiteVersion);
AutoSuite<PSBasicActionControlProcsOld> sPSBasicActionControlOld(kPSBasicActionControlSuite, kPSBasicActionControlSuiteVersion2);
AutoSuite<PSActionReferenceProcs> sPSActionReference(kPSActionReferenceSuite, kPSActionReferenceSuiteVersion);
AutoSuite<PSActionReferenceProcsOld> sPSActionReferenceOld(kPSActionReferenceSuite, kPSActionReferenceSuitePrevVersion);
AutoSuite<PSActionListProcs> sPSActionList(kPSActionListSuite, kPSActionListSuiteVersion);
AutoSuite<PSActionListProcs2> sPSActionList2(kPSActionListSuite, kPSActionListSuiteVersion2);
AutoSuite<PSActionListProcs1> sPSActionList1(kPSActionListSuite, kPSActionListSuiteVersion1);
AutoSuite<PSDescriptorRegistryProcs> sPSRegistry(kPSDescriptorRegistrySuite, kPSDescriptorRegistrySuiteVersion);
AutoSuite<PSUIHooksSuite1> sPSUIHooks(kPSUIHooksSuite, kPSUIHooksSuiteVersion1);
AutoSuite<PSHandleSuite2> sPSHandle(kPSHandleSuite, kPSHandleSuiteVersion2);
AutoSuite<ASZStringSuite1> sASZString(kASZStringSuite, kASZStringSuiteVersion1);
AutoSuite<ASZStringSuite2> sASZString2(kASZStringSuite, kASZStringSuiteVersion2);
AutoSuite<SPAccessSuite> sSPAccess(kSPAccessSuite, kSPAccessSuiteVersion);
AutoSuite<PSGetFileListSuite4> sPSFileList(kPSGetFileListSuite, kPSGetFileListSuiteVersion4);
AutoSuite<PSBufferSuite1> sPSBuffer(kPSBufferSuite, kPSBufferSuiteVersion1);
AutoSuite<PSBufferSuite2> sPSBuffer64(kPSBufferSuite, kPSBufferSuiteVersion2);
AutoSuite<BufferProcs> sPSBufferProcs(kPIBufferSuite, kPIBufferSuiteVersion);
AutoSuite<PSChannelPortsSuite1> sPSChannelProcs(kPSChannelPortsSuite, kPSChannelPortsSuiteVersion3);
AutoSuite<SPRuntimeSuite> sSPRuntime(kSPRuntimeSuite, kSPRuntimeSuiteVersion);
AutoSuite<PropertyProcs> sPSProperty(kPIPropertySuite, kPIPropertySuiteVersion);
AutoSuite<PSAliasSuite> sPSAlias(kPSAliasSuite, kPSAliasSuiteVersion);
AutoSuite<ResourceProcs> sPSResource(kPIResourceSuite, kPIResourceSuiteVersion);
AutoSuite<JSScriptingSuite2> sScripting(kJSScriptingSuite, kJSScriptingSuiteVersion);
AutoSuite<PsUXPSuite1> sUXP(kPSUXPSuite, kPSUXPSuiteVersion1);



void PIUSuitesRelease(void)
{
	sPSActionDescriptor.Unload();
    sPSActionDescriptor3.Unload();
    sPSActionDescriptor2.Unload();
	sPSActionControl.Unload();
    sPSActionControl4.Unload();
    sPSActionControlOld.Unload();
	sPSBasicActionControl.Unload();
    sPSBasicActionControlOld.Unload();
	sPSActionReference.Unload();
	sPSActionReferenceOld.Unload();
	sPSActionList.Unload();
	sPSActionList2.Unload();
	sPSActionList1.Unload();
	sPSRegistry.Unload();
	sPSUIHooks.Unload();
	sPSHandle.Unload();
	sASZString.Unload();
	sASZString2.Unload();
	sSPAccess.Unload();
	sPSFileList.Unload();
	sPSBuffer.Unload();
    sPSBuffer64.Unload();
    sPSBufferProcs.Unload();
	sPSChannelProcs.Unload();
	sSPRuntime.Unload();
	sPSProperty.Unload();
	sPSAlias.Unload();
	sPSResource.Unload();
	sScripting.Unload();
	sUXP.Unload();
}

// end PIUSuites.cpp
