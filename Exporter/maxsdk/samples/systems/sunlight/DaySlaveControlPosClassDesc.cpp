// Copyright 2009 Autodesk, Inc. All rights reserved.
//
// This computer source code and related instructions and comments are the
// unpublished confidential and proprietary information of Autodesk, Inc. and
// are protected under applicable copyright and trade secret law. They may not
// be disclosed to, copied or used by any third party without the prior written
// consent of Autodesk, Inc.

#include "DaySlaveControlPosClassDesc.h"
#include "sunclass.h"
#include "sunlight.h"
#include <dllutilities.h>


int DaySlaveControlMatrix3ClassDesc::IsPublic()
{ 
	return 0; 
}

void * DaySlaveControlMatrix3ClassDesc::Create(BOOL) 
{ 
	return new SlaveControl(true); 
}

const TCHAR * DaySlaveControlMatrix3ClassDesc::ClassName() 
{
	static MSTR name = MaxSDK::GetResourceStringAsMSTR(IDS_SLAVE_POS_CLASS);
	return name.data();
}

SClass_ID DaySlaveControlMatrix3ClassDesc::SuperClassID() 
{ 
	return CTRL_MATRIX3_CLASS_ID; 
}

Class_ID DaySlaveControlMatrix3ClassDesc::ClassID() 
{ 
	return Class_ID(DAYLIGHT_SLAVE_CONTROL_CID1, DAYLIGHT_SLAVE_CONTROL_CID2); 
}

// The slave controllers don't appear in any of the drop down lists, 
// so they just return a null string.
const TCHAR* DaySlaveControlMatrix3ClassDesc::Category() 
{ 
	return _T(""); 
}
