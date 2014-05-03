#include "Precompiled.h"                                     
#include "SlidingDoorComponent.h"                               
                                                               
// DO NOT DECLARE MORE CODE REGIONS. USE THE ONES ALREADY IN THE FILE 
                                                               
//**CodeRegion
#include "NodeCore.h"
#include "VCNPhysic\PhysicActor.h"
#include "PhysicsComponent.h"
//**EndCodeRegion
                                                               
VCNSlidingDoorComponent::VCNSlidingDoorComponent()                    
{                                                              
    // Default values                                          
    mTravelDistance = Vector3(0.0f, 0.0f, 0.0f);                   
    mIsLeftPart = true;                   
    mOtherPartName = L"";                   
//**CodeRegion
	mTargetPositionLeft = Vector3::Zero;
	mTargetPositionRight = Vector3::Zero;
//**EndCodeRegion
}                                                              
                                                               
void VCNSlidingDoorComponent::Initialise(const Parameters& params)
{                                                              
    VCNDoorComponent::Initialise(params); 
//**CodeRegion
	mOtherNodeID = VCNNodeCore::GetInstance()->GetNodeByName(mOtherPartName);
	if (mIsLeftPart)
	{
		mStartingLeftPosition = GetOwner()->GetLocalTranslation();
		VCNNode* otherNode = GetOtherPartNode();
		if (otherNode != NULL)
		{
			mStartingRightPosition = otherNode->GetLocalTranslation();
		}
	}
	else
	{
		mStartingRightPosition = GetOwner()->GetLocalTranslation();
		VCNNode* otherNode = GetOtherPartNode();
		if (otherNode != NULL)
		{
			mStartingLeftPosition = otherNode->GetLocalTranslation();
		}
	}
	mCurrentLeftPosition = mStartingLeftPosition;
	mCurrentRightPosition = mStartingRightPosition;

	VCNNode* otherNode = GetOtherPartNode();
	if (otherNode != NULL)
	{
		SetupPhysicsComponentOnNode(otherNode);
	}
	ComputeTargetPosition();
//**EndCodeRegion
}                                                              
                                                               
void VCNSlidingDoorComponent::Update(VCNFloat dt)                 
{                                                              
    VCNDoorComponent::Update(dt);    
//**CodeRegion
	VCNNode* otherNode = GetOtherPartNode();
	if (mIsLeftPart)
	{
		GetOwner()->SetTranslation(mCurrentLeftPosition);
		if (otherNode != NULL)
		{
			otherNode->SetTranslation(mCurrentRightPosition);
		}
	}
	else
	{
		GetOwner()->SetTranslation(mCurrentRightPosition);
		if (otherNode != NULL)
		{
			otherNode->SetTranslation(mCurrentLeftPosition);
		}
	}
	VCNPhysicsComponent* pComponent = GetPhysicsComponent();
	VCNPhysicActor* pActor = pComponent->GetPhysicsActor();
	pActor->SetKinematicTarget(GetOwner()->GetWorldTransformation());
	if (otherNode != NULL)
	{
		VCNPhysicsComponent* pComponent2 = otherNode->GetComponent<VCNPhysicsComponent>();
		VCNPhysicActor* pActor2 = pComponent2->GetPhysicsActor();
		pActor2->SetKinematicTarget(otherNode->GetWorldTransformation());
	}
//**EndCodeRegion
}                                                              
                                                               
bool VCNSlidingDoorComponent::SetAttribute( const VCNString& attributeName, const VCNString& attributeValue )
{                                                              
    if (attributeName == L"TravelDistance")                       
    {                                                                      
        std::stringstream ss;                                              
        std::string str( attributeValue.begin(), attributeValue.end() );   
        ss << str;                                                         
        ss >> mTravelDistance;                                      
        return true;                                                       
    }                                                                      
    else if (attributeName == L"IsLeftPart")                       
    {                                                                      
        mIsLeftPart = (attributeValue != L"0");                
        return true;                                                       
    }                                                                      
    else if (attributeName == L"OtherPartName")                       
    {                                                                      
        mOtherPartName = attributeValue;                           
        return true;                                                       
    }                                                                      
    VCNDoorComponent::SetAttribute(attributeName, attributeValue); 
    return false;                                              
}                                                              
                                                               
VCNIComponent::Ptr VCNSlidingDoorComponent::Copy() const                          
{                                                                              
    VCNSlidingDoorComponent* otherComponent = new VCNSlidingDoorComponent();         
                                                                               
    otherComponent->mTravelDistance = Vector3(mTravelDistance);
    otherComponent->mIsLeftPart = mIsLeftPart;
    otherComponent->mOtherPartName = VCNString(mOtherPartName);
    otherComponent->mOpeningSpeed = mOpeningSpeed;
    otherComponent->mClosingSpeed = mClosingSpeed;
    otherComponent->mStartingMovingState = mStartingMovingState;
    otherComponent->mOpenValidationScript = LuaTrigger(mOpenValidationScript);
    otherComponent->mOnOpenTrigger = LuaTrigger(mOnOpenTrigger);
    otherComponent->mOnCloseTrigger = LuaTrigger(mOnCloseTrigger);
    otherComponent->mIsSelectable = mIsSelectable;
    otherComponent->mUseDistance = mUseDistance;
                                                                               
    return std::shared_ptr<VCNSlidingDoorComponent>(otherComponent);              
}                                                                              
                                                               
                                                               
//**CodeRegion


void VCNSlidingDoorComponent::ComputeTargetPosition()
{
	Vector3 delta = mIsLeftPart ? (-1.0f * mTravelDistance) : (mTravelDistance);
	mTargetPositionLeft = GetOwner()->GetLocalTranslation() - delta;
	mTargetPositionRight = GetOwner()->GetLocalTranslation() + delta;
}

void VCNSlidingDoorComponent::UpdateClosing( float dt )
{
	mCurrentLeftPosition = Vector3::Lerp(mClosingSpeed * dt, mCurrentLeftPosition, mStartingLeftPosition);
	mCurrentRightPosition = Vector3::Lerp(mClosingSpeed * dt, mCurrentRightPosition, mStartingRightPosition);
	if ((mCurrentLeftPosition-mStartingLeftPosition).Length() < 0.0001f)
	{
		mMovingState = DOOR_CLOSED;
	}
}

void VCNSlidingDoorComponent::UpdateOpening( float dt )
{
	mCurrentLeftPosition = Vector3::Lerp(mOpeningSpeed * dt, mCurrentLeftPosition, mTargetPositionLeft);
	mCurrentRightPosition = Vector3::Lerp(mOpeningSpeed * dt, mCurrentRightPosition, mTargetPositionRight);
	if ((mCurrentLeftPosition-mTargetPositionLeft).Length() < 0.0001f)
	{
		mMovingState = DOOR_OPENED;
	}
}

VCNNode* VCNSlidingDoorComponent::GetOtherPartNode()
{
	VCNNode* otherNode = VCNNodeCore::GetInstance()->GetNode(mOtherNodeID);
	return otherNode;
}





//**EndCodeRegion
