// This code contains NVIDIA Confidential Information and is disclosed to you 
// under a form of NVIDIA software license agreement provided separately to you.
//
// Notice
// NVIDIA Corporation and its licensors retain all intellectual property and
// proprietary rights in and to this software and related documentation and 
// any modifications thereto. Any use, reproduction, disclosure, or 
// distribution of this software and related documentation without an express 
// license agreement from NVIDIA Corporation is strictly prohibited.
// 
// ALL NVIDIA DESIGN SPECIFICATIONS, CODE ARE PROVIDED "AS IS.". NVIDIA MAKES
// NO WARRANTIES, EXPRESSED, IMPLIED, STATUTORY, OR OTHERWISE WITH RESPECT TO
// THE MATERIALS, AND EXPRESSLY DISCLAIMS ALL IMPLIED WARRANTIES OF NONINFRINGEMENT,
// MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE.
//
// Information and code furnished is believed to be accurate and reliable.
// However, NVIDIA Corporation assumes no responsibility for the consequences of use of such
// information or for any infringement of patents or other rights of third parties that may
// result from its use. No license is granted by implication or otherwise under any patent
// or patent rights of NVIDIA Corporation. Details are subject to change without notice.
// This code supersedes and replaces all information previously supplied.
// NVIDIA Corporation products are not authorized for use as critical
// components in life support devices or systems without express written approval of
// NVIDIA Corporation.
//
// Copyright (c) 2008-2012 NVIDIA Corporation. All rights reserved.
// Copyright (c) 2004-2008 AGEIA Technologies, Inc. All rights reserved.
// Copyright (c) 2001-2004 NovodeX AG. All rights reserved.  

#ifndef SAMPLE_UTILS_H
#define SAMPLE_UTILS_H

#include "common/PxPhysXCommon.h"
#include "foundation/PxVec3.h"

	//Integer representation of a floating-point value.
	#define PX_IR(x)			((PxU32&)(x))

	// PT: TODO: move those helpers to a shared place, this is also used in the SDK

	PX_INLINE PxReal degtorad(PxReal d)
	{
		return d * PxPi / 180.0f;
	}

	PX_INLINE void computeBasis(const PxVec3& dir, PxVec3& right, PxVec3& up)
	{
		// Derive two remaining vectors
		if(dir.y>0.9999f)
		{
			right = PxVec3(1.0f, 0.0f, 0.0f);
		}
		else
		{
			right = (PxVec3(0.0f, 1.0f, 0.0f).cross(dir));
			right.normalize();
		}

		up = dir.cross(right);
	}

	PX_INLINE void computeBasis(const PxVec3& p0, const PxVec3& p1, PxVec3& dir, PxVec3& right, PxVec3& up)
	{
		// Compute the new direction vector
		dir = p1 - p0;
		dir.normalize();

		// Derive two remaining vectors
		computeBasis(dir, right, up);
	}

#endif
