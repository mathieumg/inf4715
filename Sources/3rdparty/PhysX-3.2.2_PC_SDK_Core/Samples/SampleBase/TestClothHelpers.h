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

#ifndef TEST_CLOTH_HELPERS_H
#define TEST_CLOTH_HELPERS_H

#include "PxShape.h"
#include "PsArray.h"
#include "cloth/PxCloth.h"
#include "cloth/PxClothTypes.h"
#include "cloth/PxClothCollisionData.h"
#include "cooking/PxClothMeshDesc.h"
#include "cooking/PxCooking.h"
#include "Test.h"


template<typename T> class SampleArray;

namespace Test
{
	/// simple utility functions for PxCloth
	class ClothHelpers
	{
	public:

		// border flags
		enum 
		{
			NONE = 0,
			BORDER_TOP		= (1 << 0),
			BORDER_BOTTOM	= (1 << 1),
			BORDER_LEFT		= (1 << 2),
			BORDER_RIGHT	= (1 << 3)
		};

		// solver types
		enum SolverType 
        {
            eMIXED = 1 << 0, // eSTIFF for vertical fiber, eFAST for everything else
            eFAST = 1 << 1,  // eFAST for everything
            eSTIFF = 1 << 2, // eSTIFF for everything
			eZEROSTRETCH = 1 << 3 // eZEROSTRETCH for zero stretch fiber, eFAST for everything else
        };

		// attach cloth border 
		static bool attachBorder(PxCloth& cloth, PxU32 borderFlag);

		// constrain cloth particles that overlap the given shape
		static bool attachClothOverlapToShape(PxCloth& cloth, PxShape& shape,PxReal radius = 0.1f);

		// copy mesh points to initial position of the particle and assign default mass of 1
		static bool createDefaultParticles(const PxClothMeshDesc& meshDesc, PxClothParticle* clothParticles, PxReal massPerParticle = 1.0);

		// create cloth fabric from mesh descriptor
		static PxClothFabric* createFabric(PxPhysics &physics, PxCooking &cooking, const PxClothMeshDesc &desc, const PxVec3& gravityDir);

		// create cloth mesh descriptor for a grid mesh defined along two (u,v) axis.
		static bool createMeshGrid(
			PxReal sizeU, PxReal sizeV, PxU32 numU, PxU32 numV, PxVec3 dirU, PxVec3 dirV,
			SampleArray<PxVec3>& vertexBuffer, SampleArray<PxU32>& primitiveBuffer, SampleArray<PxReal> &uvs,
			PxClothMeshDesc &meshDesc);

		// create cloth mesh descriptor for a grid mesh on XZ plane
		static bool createMeshGrid(PxReal sizeX, PxReal sizeZ, PxU32 numX, PxU32 numZ,
			SampleArray<PxVec3>& vertexBuffer, SampleArray<PxU32>& primitveBuffer, SampleArray<PxReal> &uvs,
			PxClothMeshDesc &meshDesc);

		// create cloth mesh from obj file (user must provide vertex, primitive, and optionally texture coord buffer)
		static bool createMeshFromObj(const char* filename, PxReal scale, const PxQuat* rot, const PxVec3* offset, 
			SampleArray<PxVec3>& vertexBuffer, SampleArray<PxU32>& primitiveBuffer, SampleArray<PxReal>* textureBuffer, 
			PxClothMeshDesc &meshDesc);

		// fill cloth mesh descriptor from vertices and primitives
		static void fillClothMeshDesc(SampleArray<PxVec3> &vertexBuffer, SampleArray<PxU32>& primitiveBuffer,
			PxClothMeshDesc &meshDesc);

		// create capsule data in local space of pose
		static bool createCollisionCapsule(const PxTransform &pose, const PxVec3 &center0, PxReal r0, const PxVec3 &center1, PxReal r1, 
			SampleArray<PxClothCollisionSphere> &spheres, SampleArray<PxU32> &indexPairs);

		// create virtual particle samples
		static bool createVirtualParticles(PxCloth& cloth, PxClothMeshDesc& meshDesc, int numSamples);

		// get world bounds containing all the colliders and the cloth
		static PxBounds3 getAllWorldBounds(PxCloth& cloth);

		// get particle location from the cloth
		static bool getParticlePositions(PxCloth&cloth, SampleArray<PxVec3> &positions);

		// set motion constraint radius
		static bool setMotionConstraints(PxCloth &cloth, PxReal radius);

		// set particle location from the cloth
		static bool setParticlePositions(PxCloth&cloth, const SampleArray<PxVec3> &positions, bool useConstrainedOnly = true, bool useCurrentOnly = true);

		// set solver type for all the phases
		static bool setSolverType(PxCloth& cloth, PxU32 type);

		// set stiffness for all the phases
		static bool setStiffness(PxCloth& cloth, PxReal stiffness);
	};
}


#endif
