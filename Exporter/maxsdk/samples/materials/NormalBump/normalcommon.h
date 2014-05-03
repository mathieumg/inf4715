/**********************************************************************
 *<
	FILE: common.h

	DESCRIPTION:	Common items for Normal Bump and Vector Displacement

	CREATED BY: 

	HISTORY:

 *>	Copyright (c) 2010, All Rights Reserved.
 **********************************************************************/

#ifndef __NORMAL_COMMON_H
#define __NORMAL_COMMON_H

#include "Max.h"
#include "resource.h"
#include "istdplug.h"
#include "iparamb2.h"
#include "iparamm2.h"

#include "stdmat.h"
#include <imtl.h>
#include "macrorec.h"
#include "plugapi.h"


#define ALIGN_DWORD	__declspec(align(4))

#include <IMtlRender_Compatibility.h>
#include <mentalray\mentalrayInMax.h>
#include <mentalray\imrShader.h>
#include <mentalray\imrShaderTranslation.h>

extern TCHAR *GetString(int id);

extern HINSTANCE hInstance;

#define SCANLINERENDERER_CLASS_ID				Class_ID(SREND_CLASS_ID,0)


//===========================================================================
//
// Class ReaderWriterLock
//
//===========================================================================

class ReaderWriterLock {
	public:
		ReaderWriterLock()	{bits=0;}
		void				EnterRead();
		void				ExitRead();
		void				EnterWrite();
		void				ExitWrite();
	protected:
		// Lower bits [30..0] is the reader count, upper bit [31] is the writer
		ALIGN_DWORD DWORD	bits;
};


#endif // __NORMAL_COMMON_H
