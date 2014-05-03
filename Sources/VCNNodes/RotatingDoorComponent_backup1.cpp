#include "Precompiled.h"                                     
#include "RotatingDoorComponent.h"                               
                                                               
// DO NOT DECLARE MORE CODE REGIONS. USE THE ONES ALREADY IN THE FILE 
                                                               
//**CodeRegion
#include "VCNPhysic\PhysicActor.h"
#include "PhysicsComponent.h"
//**EndCodeRegion
                                                               
VCNRotatingDoorComponent::VCNRotatingDoorComponent()                    
{                                                              
    // Default values                                          
    mRotateClockwise = true;                   
    mMaxRotationAngle = 90;                   
//**CodeRegion
	mRotationAngle = 0.0f;
	mClosedRotationAngle = 0.0f;
	mOpenedRotationAngle = 90.0f;
//**EndCodeRegion
}                                                              
                                                               
void VCNRotatingDoorComponent::Initialise(const Parameters& params)
{                                                              
    VCNDoorComponent::Initialise(params); 
//**CodeRegion
	ApplyBaseRotation();
//**EndCodeRegion
}                                                              
                                                               
void VCNRotatingDoorComponent::Update(VCNFloat dt)                 
{                                                              
    VCNDoorComponent::Update(dt);    
//**CodeRegion
	VCNQuat q = VCNQuat::FromEuler(0.0f, VCN::DEG_TO_RAD_VCN(mRotationAngle), 0.0f);
	q.Normalize();
	GetOwner()->SetRotation(q);
	VCNPhysicsComponent* pComponent = GetPhysicsComponent();
	VCNPhysicActor* pActor = pComponent->GetPhysicsActor();
	pActor->SetKinematicTarget(GetOwner()->GetWorldTransformation());
//**EndCodeRegion
}                                                              
                                                               
bool VCNRotatingDoorComponent::SetAttribute( const VCNString& attributeName, const VCNString& attributeValue )
{                                                              
    if (attributeName == L"RotateClockwise")                       
    {                                                                      
        mRotateClockwise = (attributeValue != L"0");                
        return true;                                                       
    }                                                                      
    else if (attributeName == L"MaxRotationAngle")                       
    {                                                                      
        mMaxRotationAngle = (float)_wtof(attributeValue.c_str());     
        return true;                                                       
    }                                                                      
    VCNDoorComponent::SetAttribute(attributeName, attributeValue); 
    return false;                                              
}                                                              
                                                               
VCNIComponent::Ptr VCNRotatingDoorComponent::Copy() const                          
{                                                                              
    VCNRotatingDoorComponent* otherComponent = new VCNRotatingDoorComponent();         
                                                                               
    otherComponent->mRotateClockwise = mRotateClockwise;
    otherComponent->mMaxRotationAngle = mMaxRotationAngle;
    otherComponent->mOpeningSpeed = mOpeningSpeed;
    otherComponent->mClosingSpeed = mClosingSpeed;
    otherComponent->mStartingMovingState = mStartingMovingState;
    otherComponent->mOpenValidationScript = LuaTrigger(mOpenValidationScript);
    otherComponent->mOnOpenTrigger = LuaTrigger(mOnOpenTrigger);
    otherComponent->mOnCloseTrigger = LuaTrigger(mOnCloseTrigger);
    otherComponent->mIsSelectable = mIsSelectable;
    otherComponent->mUseDistance = mUseDistance;
                                                                               
    return std::shared_ptr<VCNRotatingDoorComponent>(otherComponent);              
}                                                                              
                                                               
                                                               
//**CodeRegion

void VCNRotatingDoorComponent::UpdateClosing( float dt )
{
	mRotationAngle = VCN::Lerp(mOpeningSpeed * dt, mRotationAngle, mClosedRotationAngle);
	if (abs(mRotationAngle-mClosedRotationAngle) < 0.0001f)
	{
		mMovingState = DOOR_CLOSED;
	}
}


void VCNRotatingDoorComponent::UpdateOpening( float dt )
{
	mRotationAngle = VCN::Lerp(mOpeningSpeed * dt, mRotationAngle, mOpenedRotationAngle);
	if (abs(mRotationAngle-mOpenedRotationAngle) < 0.0001f)
	{
		mMovingState = DOOR_OPENED;
	}
}


void VCNRotatingDoorComponent::ApplyBaseRotation()
{
	VCNQuat currentRotation = GetOwner()->GetLocalRotation();
	float x, y, z;
	currentRotation.GetEulers(&x, &y, &z);
	float rotationAngle = mRotateClockwise ? (-1.0f * mMaxRotationAngle) : (mMaxRotationAngle);

	mOpenedRotationAngle = y + rotationAngle;
	mRotationAngle = y;
	mClosedRotationAngle = y;
}

//**EndCodeRegion
