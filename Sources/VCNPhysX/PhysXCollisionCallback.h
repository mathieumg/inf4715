#pragma once
#include "..\3rdparty\PhysX-3.2.2_PC_SDK_Core\Include\characterkinematic\PxController.h"

class PhysXCollisionCallback : public PxUserControllerHitReport
{
public:
    PhysXCollisionCallback();
    ~PhysXCollisionCallback();

    virtual void onShapeHit( const PxControllerShapeHit& hit );
    virtual void onControllerHit( const PxControllersHit& hit );
    virtual void onObstacleHit( const PxControllerObstacleHit& hit );
};

