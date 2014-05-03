#include "Precompiled.h"                                     
#include "CircularWallRotatingComponent.h"                               
                                                               
// DO NOT DECLARE MORE CODE REGIONS. USE THE ONES ALREADY IN THE FILE 
                                                               
//**CodeRegion
#include "VCNUtils\Assert.h"
#include "PhysicsComponent.h"
#include "..\PhysX-3.2.2_PC_SDK_Core\Include\PxRigidDynamic.h"
//**EndCodeRegion
                                                               
VCNCircularWallRotatingComponent::VCNCircularWallRotatingComponent()                    
{                                                              
    // Default values                                          
    mSpeed = 0.01;                   
//**CodeRegion
//**EndCodeRegion
}                                                              
                                                               
void VCNCircularWallRotatingComponent::Initialise(const Parameters& params)
{                                                              
//**CodeRegion
//**EndCodeRegion
}                                                              
                                                               
void VCNCircularWallRotatingComponent::Update(VCNFloat dt)                 
{                                                              
//**CodeRegion
	VCNPhysicsComponent* physxComponent = GetOwner()->GetComponent<VCNPhysicsComponent>();
	if (physxComponent && physxComponent->GetIsDynamic())
	{
		physxComponent->GetPhysicsActor()->SetAngularDamping(0.0f);
		physxComponent->GetPhysicsActor()->SetAngularVelocity(Vector3(0.0f, mSpeed, 0.0f));
		GetOwner()->SetRotation(VCNQuat(physxComponent->GetPhysicsActor()->GetTransform()));
	}
//**EndCodeRegion
}                                                              
                                                               
bool VCNCircularWallRotatingComponent::SetAttribute( const VCNString& attributeName, const VCNString& attributeValue )
{                                                              
    if (attributeName == L"Speed")                       
    {                                                                      
        mSpeed = _wtof(attributeValue.c_str());            
        return true;                                                       
    }                                                                      
    return false;                                              
}                                                              
                                                               
VCNIComponent::Ptr VCNCircularWallRotatingComponent::Copy() const                          
{                                                                              
    VCNCircularWallRotatingComponent* otherComponent = new VCNCircularWallRotatingComponent();         
                                                                               
    otherComponent->mSpeed = mSpeed;
                                                                               
    return std::shared_ptr<VCNCircularWallRotatingComponent>(otherComponent);              
}                                                                              
                                                               
                                                               
//**CodeRegion
//**EndCodeRegion
