/**********************************************************************
 *<
	FILE: polycnt.h

	DESCRIPTION:  Basic includes for modules in polygon count

	CREATED BY: greg finch

	HISTORY: created 9, May 1997

 *>	Copyright (c) 1997, All Rights Reserved.
 **********************************************************************/

#ifndef __POLYCNT__H__
#define __POLYCNT__H__

#include <stdio.h>
#include "max.h"
#include "resource.h"
#include "iparamm.h"
#include "bmmlib.h"
#include "utilapi.h"

TCHAR *GetString(int id);

#define POLYGON_COUNTER_CLASS_ID	Class_ID(0x2d63282b,0x5aad3048)

extern ClassDesc* GetPolyCounterDesc();

#endif