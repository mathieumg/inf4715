///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
///
/// @TODO Move to VCNCore
/// 

#ifndef VICUNA_CONSTANTS
#define VICUNA_CONSTANTS

#pragma once

#include "Types.h"

// Max values
const VCNUShort kMaxUShort = (VCNUShort)-1;
const VCNFloat kMinFloat = -FLT_MAX;
const VCNFloat kMaxFloat = FLT_MAX;

const VCNNodeID kRootNodeID = 0;
const VCNNodeID kInvalidNodeID = (VCNNodeID)-1;

// Resource core ID
const VCNResID kInvalidResID = (VCNResID)-1;

// Animation core ID
const VCNAnimID kInvalidAnimID = (VCNAnimID)-1;

// Maximum number of texture coordinate sets that are supported
#define MAX_TEX_COORD_SETS 4

// PI is a handy constant to carry around
const VCNFloat VCNPI = 3.14159265f;

#endif