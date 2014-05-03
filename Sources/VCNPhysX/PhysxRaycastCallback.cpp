#include "Precompiled.h"
#include "PhysxRaycastCallback.h"
#include "VCNNodes\Node.h"
#include "VCNNodes\NodeCore.h"


PhysXRaycastCallback::PhysXRaycastCallback(void)
{
}


PhysXRaycastCallback::~PhysXRaycastCallback(void)
{
}


PxSceneQueryHitType::Enum PhysXRaycastCallback::preFilter( const PxFilterData& filterData, PxShape* shape, PxSceneQueryFilterFlags& filterFlags )
{
    if(shape->getActor().userData == NULL)
    {
        return PxSceneQueryHitType::eNONE;
    }

    return PxSceneQueryHitType::eBLOCK;
}

PxSceneQueryHitType::Enum PhysXRaycastCallback::postFilter( const PxFilterData& filterData, const PxSceneQueryHit& hit )
{
    return PxSceneQueryHitType::eBLOCK;
}
