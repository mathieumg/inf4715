// Copyright 2009 Autodesk, Inc. All rights reserved.
//
// This computer source code and related instructions and comments are the
// unpublished confidential and proprietary information of Autodesk, Inc. and
// are protected under applicable copyright and trade secret law. They may not
// be disclosed to, copied or used by any third party without the prior written
// consent of Autodesk, Inc.

#include <dllutilities.h>
#include "CompassRoseObjClassDesc.h"
#include "compass.h"
#include "sunlight.h"

namespace 
{

void resetPointParams() 
{
	CompassRoseObject::dlgShowAxis = TRUE;
	CompassRoseObject::dlgAxisLength = AXIS_LENGTH;
}

}

int CompassRoseObjClassDesc::IsPublic()
{ 
	return TRUE; 
}

void* CompassRoseObjClassDesc::Create(BOOL loading)
{ 
	return new CompassRoseObject; 
}

const TCHAR * CompassRoseObjClassDesc::ClassName() 
{ 
	static MSTR name = MaxSDK::GetResourceStringAsMSTR(IDS_DB_COMPASS_CDESC);
	return name.data();
}

SClass_ID CompassRoseObjClassDesc::SuperClassID() 
{ 
	return HELPER_CLASS_ID; 
}

Class_ID CompassRoseObjClassDesc::ClassID() 
{ 
	return COMPASS_CLASS_ID; 
}

const TCHAR* CompassRoseObjClassDesc::Category()
{ 
	return _T(""); 
}

void CompassRoseObjClassDesc::ResetClassParams(BOOL fileReset) 
{ 
	if(fileReset)
	{
		resetPointParams(); 
	}
}
