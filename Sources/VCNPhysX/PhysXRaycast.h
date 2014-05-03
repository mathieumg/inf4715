#ifndef VCNPHYSXRAYCAST_H
#define VCNPHYSXRAYCAST_H

#include "VCNUtils\Types.h"
#include "VCNUtils\Vector.h"

class VCNNode;

class PhysXRaycast
{
    public:
        PhysXRaycast(PxScene* scene);
        ~PhysXRaycast();

        VCNBool raycast(PxReal dist, const Vector3& dir, const Vector3& pos, VCNNode*& node, bool AI = false) const;

    private:
        PxScene* mScene;
        static PxSceneQueryFilterCallback* mFilterCallback;
        mutable PxSceneQueryCache persistentCache;
};

#endif