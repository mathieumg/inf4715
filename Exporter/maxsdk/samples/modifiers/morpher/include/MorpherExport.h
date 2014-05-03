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
#pragma once

#ifdef MorphExport
#undef MorphExport
#endif

#ifdef BUILD_MORPHER 
#define MorphExport __declspec( dllexport )
#else
#define MorphExport __declspec( dllimport )
#endif
