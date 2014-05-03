/**********************************************************************
 *<
	FILE: ViewportManager.h

	DESCRIPTION:	Includes for Plugins

	CREATED BY:

	HISTORY:

 *>	Copyright (c) 2000, All Rights Reserved.
 **********************************************************************/

#ifndef __VIEWPORTMANAGER__H
#define __VIEWPORTMANAGER__H

#include <maxtypes.h>
#include <ihardwareshader.h>
class MtlBase;

#define VIEWPORTMANAGERCONTROL_CLASS_ID	Class_ID(0x286308e0, 0x5b309c41)

extern TCHAR *GetString(int id);

extern HINSTANCE hInstance;
extern BOOL CheckForDX();

/*! This Interface allows access to the various components of the ViewportManager.  
This includes finding out how to judge IsValidMaterial. */
class IViewportManager
{
public:
	virtual BOOL IsValidMaterial( MtlBase* mtl ) = 0;
};


#endif // __VIEWPORTMANAGER__H
