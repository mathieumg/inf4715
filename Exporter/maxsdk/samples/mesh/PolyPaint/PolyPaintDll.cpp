//**************************************************************************/
// Copyright (c) 1998-2010 Autodesk, Inc.
// All rights reserved.
// 
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information written by Autodesk, Inc., and are
// protected by Federal copyright law. They may not be disclosed to third
// parties or copied or duplicated in any form, in whole or in part, without
// the prior written consent of Autodesk, Inc.
//**************************************************************************/

// include only after you've included your dll's resource.h
#include <WindowsDefines.h>
#include <plugapi.h>
#include "PolyPaint.h"

BOOL WINAPI DllMain(HINSTANCE hinstDLL, ULONG fdwReason, LPVOID) 
{
	if (DLL_PROCESS_ATTACH == fdwReason)
	{
		MaxSDK::Util::UseLanguagePackLocale();
		DisableThreadLibraryCalls(hinstDLL);
	}
	else if (DLL_PROCESS_DETACH == fdwReason)
	{
		MeshPaintMgr::DestroyInstance();
	}
	return(TRUE);
}



