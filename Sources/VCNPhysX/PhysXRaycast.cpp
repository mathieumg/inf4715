#include "Precompiled.h"
#include "PhysXRaycast.h"
#include "VCNNodes\NodeCore.h"
#include "VCNNodes\Node.h"
#include "GameApp\StateMachine.h"
#include "GameApp\PlayState.h"
#include "PxSceneQueryFiltering.h"
#include "PhysxRaycastCallback.h"
#include "VCNUtils\RenderTypes.h"
#include "VCNRenderer\RenderCore.h"

PxSceneQueryFilterCallback* PhysXRaycast::mFilterCallback = new PhysXRaycastCallback();

PhysXRaycast::PhysXRaycast(physx::PxScene* scene)
    : mScene(scene)
{
}


PhysXRaycast::~PhysXRaycast(void)
{
}

VCNBool PhysXRaycast::raycast( PxReal dist, const Vector3& dir, const Vector3& pos, VCNNode*& node, bool AI) const
{
    const PxVec3 origin = PxVec3(pos.x, pos.y, pos.z);
    const PxVec3 pxDir = PxVec3(dir.x, dir.y, dir.z);
    PxRaycastHit hit;
	PxSceneQueryHit queryHit;

    const PxSceneQueryCache* cache = persistentCache.shape ? &persistentCache : NULL;
	const static PxSceneQueryFlags outputFlags = PxSceneQueryFlag::eIMPACT | PxSceneQueryFlag::eDISTANCE;
	const static PxSceneQueryFlags outputFlagsAI;
    const static PxSceneQueryFilterData normFilterData( PxSceneQueryFilterFlag::eDYNAMIC | PxSceneQueryFilterFlag::eSTATIC | PxSceneQueryFilterFlag::ePREFILTER );
	const static PxSceneQueryFilterData aiFilterData(PxSceneQueryFilterFlag::eDYNAMIC | PxSceneQueryFilterFlag::eSTATIC);

    const PxSceneQueryFilterData& filterData = AI ? aiFilterData : normFilterData;

	bool result = false;

	if (!AI)
	{
		result = mScene->raycastSingle(origin, pxDir, dist, outputFlags, hit, filterData, mFilterCallback, cache);

	}
	else
	{
		result = mScene->raycastAny(origin, pxDir, dist, queryHit);

	}

    if(result)
    {
		if (!AI)
		{
			persistentCache.shape = hit.shape;
			persistentCache.faceIndex = hit.faceIndex;
			node = (VCNNode*)(hit.shape->getActor().userData);
		}
		else
		{
			persistentCache.shape = queryHit.shape;
			persistentCache.faceIndex = queryHit.faceIndex;
			node = (VCNNode*)(queryHit.shape->getActor().userData);
		}
        

        if(node == NULL)
        {
            return false;
        }
    }
    else
    {
        persistentCache = PxSceneQueryCache();
    }

    return result;
}