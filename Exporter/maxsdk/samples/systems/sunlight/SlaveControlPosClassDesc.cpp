// Copyright 2009 Autodesk, Inc. All rights reserved.
//
// This computer source code and related instructions and comments are the
// unpublished confidential and proprietary information of Autodesk, Inc. and
// are protected under applicable copyright and trade secret law. They may not
// be disclosed to, copied or used by any third party without the prior written
// consent of Autodesk, Inc.

#include "SlaveControlPosClassDesc.h"
#include "sunclass.h"
#include "sunlight.h"
#include <dllutilities.h>


int SlaveControlMatrix3ClassDesc::IsPublic() 
{ 
	return 0; 
}

void * SlaveControlMatrix3ClassDesc::Create(BOOL) 
{ 
	return new SlaveControl(false); 
}

const TCHAR * SlaveControlMatrix3ClassDesc::ClassName() 
{
	static MSTR class_name = MaxSDK::GetResourceStringAsMSTR(IDS_SLAVE_POS_CLASS);
	return class_name.data();
}

SClass_ID SlaveControlMatrix3ClassDesc::SuperClassID() 
{ 
	return CTRL_MATRIX3_CLASS_ID; 
}

Class_ID SlaveControlMatrix3ClassDesc::ClassID() 
{ 
	return Class_ID(SLAVE_CONTROL_CID1, SLAVE_CONTROL_CID2); 
}

// The slave controllers don't appear in any of the drop down lists, 
// so they just return a null string.
const TCHAR* SlaveControlMatrix3ClassDesc::Category() 
{ 
	return _T(""); 
}


