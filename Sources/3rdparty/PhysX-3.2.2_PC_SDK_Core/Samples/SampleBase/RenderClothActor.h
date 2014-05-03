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

#ifndef RENDER_CLOTH_ACTOR_H
#define RENDER_CLOTH_ACTOR_H

#include "RenderBaseActor.h"
#include "RenderPhysX3Debug.h"

#include "SampleAllocator.h"
#include "SampleArray.h"

#include "cloth/PxCloth.h"
#include "cooking/PxClothMeshDesc.h"

namespace SampleRenderer
{
	class Renderer;
    class RendererClothShape;
}

class RenderCapsuleActor;

class RenderClothActor : public RenderBaseActor
{
public:
	RenderClothActor(SampleRenderer::Renderer& renderer, const PxCloth& cloth, const PxClothMeshDesc &desc, const PxReal* uvs = NULL, const PxVec3 &color = PxVec3(0.5f, 0.5f, 0.5f), PxReal capsuleScale = 1.0f);

	virtual								~RenderClothActor();

	virtual void						update(float deltaTime);

	typedef shdfnd::Array<RenderCapsuleActor*> CollisionActorArray;

	const CollisionActorArray&			getSphereActors() { return mSphereActors; }
	const CollisionActorArray&			getCapsuleActors() { return mCapsuleActors; }

private:
	void								updateRenderShape();

private:
	SampleRenderer::Renderer&			mRenderer;
	const PxCloth&                      mCloth; 

	// copied mesh structure 
	PxU32                               mNumFaces;
	PxU16*                              mFaces;

	// collision data used for debug rendering
	PxClothCollisionSphere*             mSpheres;
	PxU32*                              mIndexPairs;
	PxClothCollisionPlane*				mPlanes;
	PxU32*								mConvexMasks;
	PxU32                               mNumPlanes, mNumConvexes;

	// texture uv (used only for render)
	PxReal*								mUV;

	PxVec3								mRendererColor;
	PxReal                              mCapsuleScale;

    SampleRenderer::RendererClothShape* mClothRenderShape;

	// collision shapes render actors
	CollisionActorArray mSphereActors;
	CollisionActorArray mCapsuleActors;
};

#endif
