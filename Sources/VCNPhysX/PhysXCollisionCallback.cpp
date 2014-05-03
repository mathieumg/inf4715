#include "Precompiled.h"
#include "PhysXCollisionCallback.h"
#include "..\VCNNodes\PuzzleCubeComponent.h"


PhysXCollisionCallback::PhysXCollisionCallback(void)
{
}


PhysXCollisionCallback::~PhysXCollisionCallback(void)
{
}

void PhysXCollisionCallback::onShapeHit( const PxControllerShapeHit& hit )
{
    if(hit.shape->getActor().userData == nullptr)
    {
        return;
    }

    VCNNode* hitNode = (VCNNode*) hit.shape->getActor().userData;
    VCNPuzzleCubeComponent* component = hitNode->GetComponent<VCNPuzzleCubeComponent>();
    if(component != NULL)
    {
        Vector3 impulsion = Vector3(hit.dir.x, hit.dir.y, hit.dir.z);
        component->OnCharacterHit(impulsion);
    }
}

void PhysXCollisionCallback::onControllerHit( const PxControllersHit& hit )
{


}

void PhysXCollisionCallback::onObstacleHit( const PxControllerObstacleHit& hit )
{
}
