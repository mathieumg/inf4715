//**************************************************************************/
// Copyright (c) 1998-2007 Autodesk, Inc.
// All rights reserved.
// 
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information written by Autodesk, Inc., and are
// protected by Federal copyright law. They may not be disclosed to third
// parties or copied or duplicated in any form, in whole or in part, without
// the prior written consent of Autodesk, Inc.
//**************************************************************************/
/*===========================================================================*\
	FILE: suntypes.h

	DESCRIPTION: Some type definitions for the sunlight system.

	HISTORY: Adapted by John Hutchinson 10/08/97 
			

	Copyright (c) 1996, All Rights Reserved.
 \*==========================================================================*/

#pragma once

struct JulianCalendar { 
	double days; 
	int subday;
	long epoch;
}; 

struct uTimevect { 
	unsigned short i; 
	unsigned short j;
	unsigned short k;
}; 


