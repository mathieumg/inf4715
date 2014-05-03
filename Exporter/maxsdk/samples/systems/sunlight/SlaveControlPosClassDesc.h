// Copyright 2009 Autodesk, Inc. All rights reserved.
//
// This computer source code and related instructions and comments are the
// unpublished confidential and proprietary information of Autodesk, Inc. and
// are protected under applicable copyright and trade secret law. They may not
// be disclosed to, copied or used by any third party without the prior written
// consent of Autodesk, Inc.

#pragma once

#include <maxapi.h>

class SlaveControlMatrix3ClassDesc : public ClassDesc 
{
public:
	int IsPublic();
	void * Create(BOOL loading = FALSE);
	const TCHAR * ClassName();
	SClass_ID SuperClassID();
	Class_ID ClassID();
	// The slave controllers don't appear in any of the drop down lists, 
	// so they just return a null string.
	const TCHAR* Category();
};

// In Rampage, we corrected the superclass ID (from POSITION).  To support old files, 
// we need to have a ClassDesc that matches the old SClassID
class LegacySlaveControlPosClassDesc : public SlaveControlMatrix3ClassDesc
{
	SClass_ID SuperClassID() { return CTRL_POSITION_CLASS_ID; }
	const TCHAR * ClassName()  { return NULL; } // will not expose via mxs
};


