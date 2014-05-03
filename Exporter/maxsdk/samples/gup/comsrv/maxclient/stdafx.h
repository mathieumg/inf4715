// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//


#pragma once

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include "../../../../include/MaxWindowsVersion.h"

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <atlbase.h>
extern CComModule _Module;
#pragma warning(push)
#pragma warning(disable : 4265)
#include <atlcom.h>
#pragma warning(pop)


