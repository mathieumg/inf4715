#pragma once

//**************************************************************************/
// Copyright (c) 2011 Autodesk, Inc.
// All rights reserved.
// 
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information written by Autodesk, Inc., and are
// protected by Federal copyright law. They may not be disclosed to third
// parties or copied or duplicated in any form, in whole or in part, without
// the prior written consent of Autodesk, Inc.
//**************************************************************************/
// DESCRIPTION: Parameter IDs For Ellipse Plug In
// AUTHOR: 
//***************************************************************************/

#include <splshape.h>
#include <iparamm.h>
#include <simpspl.h>
#include <assert1.h>
#include <iparamb2.h>
#include <iparamm2.h>
#include <ReferenceSaveManager.h>

#define MIN_LENGTH		float(0)
#define MAX_LENGTH		float( 1.0E30)
#define MIN_WIDTH		float(0)
#define MAX_WIDTH		float( 1.0E30)
#define MIN_THICK		float(-1.0E30)
#define MAX_THICK		float( 1.0E30)

#define DEF_LENGTH		float(0.0)
#define DEF_WIDTH		float(0.0)

#define CREATE_EDGE 0
#define CREATE_CENTER 1

// Vector length for unit circle
#define CIRCLE_VECTOR_LENGTH 0.5517861843f

#define ELLISPE_LENGTH_DEFAULT 0.f
#define ELLISPE_WIDTH_DEFAULT 0.f
#define ELLISPE_THICKNESS_DEFAULT 0.f
#define ELLIPSE_OUTLINE_DEFAULT FALSE

// The parameters for this shape
enum ParamBlocks
{
	ellipse_params,

	// The number of parameter blocks that holds instance parameters
	ellipse_num_param_blocks,

	//Class parameter
	ellipse_type_in_params,
	ellipse_create_params
};

enum EllipseParamsElements
{
	PB_LENGTH,
	PB_WIDTH,
	PB_THICKNESS,
	PB_OUTLINE,

	PB_NUM_PARM_ELEMENTS
};

enum EllipseTypeInParamElements
{
	PB_TI_POS,
	PB_TI_LENGTH,
	PB_TI_WIDTH,
	PB_TI_THICKNESS,

	PB_NUM_TYPE_IN_ELEMENTS
};

enum EllipseCreateParamElemets
{
	PB_CREATEMETHOD,

	PB_NUM_CREATE_ELEMENTS
};


