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
#ifndef _DISSOLVE_H
#define _DISSOLVE_H

#include "PIDefines.h"
#include "PITypes.h"
#include "PIAbout.h"
#include "PIFilter.h"
#include "PIUtilities.h"

typedef struct Parameters
{
	int16 percent;
	int16 disposition;
	Boolean ignoreSelection;
} Parameters, *ParametersPtr;

typedef struct Data
{
	FilterColor color;
	FilterColor colorArray[4];
	Boolean queryForParameters;
	BufferID dissolveBufferID;
	Ptr dissolveBuffer;
	VRect proxyRect;
	float scaleFactor;
	BufferID proxyBufferID;
	Ptr proxyBuffer;
	int32 proxyWidth;
	int32 proxyHeight;
	int32 proxyPlaneSize;
} Data;

extern FilterRecord* gFilterRecord;
extern Data* gData;
extern int16* gResult;
extern Parameters* gParams;
extern SPBasicSuite * sSPBasic;

void SetupFilterRecordForProxy(void);
void CopyColor(FilterColor& destination, const FilterColor& source);
void SetColor(FilterColor& destination, 
			  const uint8 a, 
			  const uint8 b, 
			  const uint8 c, 
			  const uint8 d);
void CreateDissolveBuffer(const int32 width, const int32 height);
void UpdateDissolveBuffer(const int32 width, const int32 height);
void DeleteDissolveBuffer(void);
void CreateProxyBuffer(void);
extern "C" void ResetProxyBuffer(void);
extern "C" void UpdateProxyBuffer(void);
void DeleteProxyBuffer(void);
int32 DisplayPixelsMode(int16 mode);


#endif