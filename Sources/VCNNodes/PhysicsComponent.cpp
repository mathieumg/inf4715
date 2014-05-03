#include "Precompiled.h"                                     
#include "PhysicsComponent.h"                               
                                                               
// DO NOT DECLARE MORE CODE REGIONS. USE THE ONES ALREADY IN THE FILE 
                                                               
//**CodeRegion
#include "VCNPhysic\PhysicCore.h"
//**EndCodeRegion
                                                               
VCNPhysicsComponent::VCNPhysicsComponent()                    
{                                                              
    // Default values                                          
    mActive = true;                   
    mIsDynamic = false;                   
    mHasGravity = false;                   
    mIsKinematic = false;                   
    mForceHollow = false;                   
//**CodeRegion
	mPhysicsActor = NULL;
//**EndCodeRegion
}                                                              
                                                               
void VCNPhysicsComponent::Initialise(const Parameters& params)
{                                                              
//**CodeRegion
	if (!mPhysicsActor)
	{
		if (mActive)
		{
			if (mIsDynamic)
			{
				GetOwner()->AddProperty( VCNTXT("Dynamic") );
			}
			if (mIsKinematic)
			{
				GetOwner()->AddProperty( VCNTXT("Kinematic") );
			}
			if (mForceHollow)
			{
				GetOwner()->AddProperty( VCNTXT("ForceHollow") );
			}
			mPhysicsActor = VCNPhysicCore::GetInstance()->CreateActor(GetOwner(), mHasGravity);
		}
	}
//**EndCodeRegion
}                                                              
                                                               
void VCNPhysicsComponent::Update(VCNFloat dt)                 
{                                                              
//**CodeRegion
	
//**EndCodeRegion
}                                                              
                                                               
bool VCNPhysicsComponent::SetAttribute( const VCNString& attributeName, const VCNString& attributeValue )
{                                                              
    if (attributeName == L"Active")                       
    {                                                                      
        mActive = (attributeValue != L"0");                
        return true;                                                       
    }                                                                      
    else if (attributeName == L"IsDynamic")                       
    {                                                                      
        mIsDynamic = (attributeValue != L"0");                
        return true;                                                       
    }                                                                      
    else if (attributeName == L"HasGravity")                       
    {                                                                      
        mHasGravity = (attributeValue != L"0");                
        return true;                                                       
    }                                                                      
    else if (attributeName == L"IsKinematic")                       
    {                                                                      
        mIsKinematic = (attributeValue != L"0");                
        return true;                                                       
    }                                                                      
    else if (attributeName == L"ForceHollow")                       
    {                                                                      
        mForceHollow = (attributeValue != L"0");                
        return true;                                                       
    }                                                                      
    return false;                                              
}                                                              
                                                               
VCNIComponent::Ptr VCNPhysicsComponent::Copy() const                          
{                                                                              
    VCNPhysicsComponent* otherComponent = new VCNPhysicsComponent();         
                                                                               
    otherComponent->mActive = mActive;
    otherComponent->mIsDynamic = mIsDynamic;
    otherComponent->mHasGravity = mHasGravity;
    otherComponent->mIsKinematic = mIsKinematic;
    otherComponent->mForceHollow = mForceHollow;
                                                                               
    return std::shared_ptr<VCNPhysicsComponent>(otherComponent);              
}                                                                              
                                                               
                                                               
//**CodeRegion

void VCNPhysicsComponent::DestroyPhysicsActor()
{
	if (mPhysicsActor)
	{
		if (VCNPhysicCore::IsInstantiated())
		{
			VCNPhysicCore* core = VCNPhysicCore::GetInstance();
			VCNPhysicCore::GetInstance()->RemoveActor(mPhysicsActor);
			mPhysicsActor = NULL;
		}
	}
}
//**EndCodeRegion
