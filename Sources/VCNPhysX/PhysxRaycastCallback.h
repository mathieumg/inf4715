#pragma once
#include "PxSceneQueryFiltering.h"

class PhysXRaycastCallback :
    public PxSceneQueryFilterCallback
{
public:
    PhysXRaycastCallback(void);
    ~PhysXRaycastCallback(void);

    virtual PxSceneQueryHitType::Enum preFilter( const PxFilterData& filterData, PxShape* shape, PxSceneQueryFilterFlags& filterFlags );
    virtual PxSceneQueryHitType::Enum postFilter( const PxFilterData& filterData, const PxSceneQueryHit& hit );

};

