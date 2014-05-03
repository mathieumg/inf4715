#include "Precompiled.h"                                     
#include "SlidingBridgeComponent.h"                               
                                                               
// DO NOT DECLARE MORE CODE REGIONS. USE THE ONES ALREADY IN THE FILE 
                                                               
//**CodeRegion
#include "VCNUtils\Utilities.h"
#include "VCNAudio\AudioCore.h"
#include "PhysicsComponent.h"
#include "ComponentFactory.h"
//**EndCodeRegion
                                                               
VCNSlidingBridgeComponent::VCNSlidingBridgeComponent()                    
{                                                              
    // Default values                                          
    mTravelDistance = Vector3(0.0f, 0.0f, 0.0f);                   
    mslidingSpeed = 1;                   
    mStartingMovingState = 0;                   
    mslideBridge = LuaTrigger(L"", L"");                   
//**CodeRegion
	mMovingState = (BridgeMovingState)mStartingMovingState;
	mCurrentPosition = Vector3::Zero;
	mTargetPosition = Vector3::Zero;
//**EndCodeRegion
}                                                              
                                                               
void VCNSlidingBridgeComponent::Initialise(const Parameters& params)
{                                                              
    VCNUsableComponent::Initialise(params); 
//**CodeRegion
	SetupPhysicsComponentOnNode(GetOwner());
	mCurrentPosition = GetOwner()->GetLocalTranslation();
	mTargetPosition = mCurrentPosition + mTravelDistance;
//**EndCodeRegion
}                                                              
                                                               
void VCNSlidingBridgeComponent::Update(VCNFloat dt)                 
{                                                              
    VCNUsableComponent::Update(dt);    
//**CodeRegion
	switch (mMovingState)
	{
	case BRIDGE_OPENING:
		UpdateOpening(dt);
		break;
	default:
		break;
	}
	GetOwner()->SetTranslation(mCurrentPosition);
	VCNPhysicsComponent* pComponent = GetPhysicsComponent();
	VCNPhysicActor* pActor = pComponent->GetPhysicsActor();
	pActor->SetKinematicTarget(GetOwner()->GetWorldTransformation());
//**EndCodeRegion
}                                                              
                                                               
bool VCNSlidingBridgeComponent::SetAttribute( const VCNString& attributeName, const VCNString& attributeValue )
{                                                              
    if (attributeName == L"TravelDistance")                       
    {                                                                      
        std::stringstream ss;                                              
        std::string str( attributeValue.begin(), attributeValue.end() );   
        ss << str;                                                         
        ss >> mTravelDistance;                                      
        return true;                                                       
    }                                                                      
    else if (attributeName == L"slidingSpeed")                       
    {                                                                      
        mslidingSpeed = (float)_wtof(attributeValue.c_str());     
        return true;                                                       
    }                                                                      
    else if (attributeName == L"StartingMovingState")                       
    {                                                                      
        mStartingMovingState = (int)_wtoi(attributeValue.c_str());       
        return true;                                                       
    }                                                                      
    else if (attributeName == L"slideBridge")                       
    {                                                                      
        std::stringstream ss;                                              
        std::string str( attributeValue.begin(), attributeValue.end() );   
        ss << str;                                                         
        ss >> mslideBridge;                                      
        return true;                                                       
    }                                                                      
    VCNUsableComponent::SetAttribute(attributeName, attributeValue); 
    return false;                                              
}                                                              
                                                               
VCNIComponent::Ptr VCNSlidingBridgeComponent::Copy() const                          
{                                                                              
    VCNSlidingBridgeComponent* otherComponent = new VCNSlidingBridgeComponent();         
                                                                               
    otherComponent->mTravelDistance = Vector3(mTravelDistance);
    otherComponent->mslidingSpeed = mslidingSpeed;
    otherComponent->mStartingMovingState = mStartingMovingState;
    otherComponent->mslideBridge = LuaTrigger(mslideBridge);
    otherComponent->mIsSelectable = mIsSelectable;
    otherComponent->mUseDistance = mUseDistance;
                                                                               
    return std::shared_ptr<VCNSlidingBridgeComponent>(otherComponent);              
}                                                                              
                                                               
                                                               
//**CodeRegion

void VCNSlidingBridgeComponent::UpdateOpening( float dt )
{
	mCurrentPosition = Vector3::Lerp(mslidingSpeed * dt, mCurrentPosition, mTargetPosition);
	if ((mCurrentPosition-mTargetPosition).Length() < 0.001f)
	{
		mMovingState = BRIDGE_OPENED;
	}

} 

VCNPhysicsComponent* VCNSlidingBridgeComponent::GetPhysicsComponent()
{
	return GetOwner()->GetComponent<VCNPhysicsComponent>();
}

void VCNSlidingBridgeComponent::SetupPhysicsComponentOnNode( VCNNode* node )
{
	if (node->HasComponent<VCNPhysicsComponent>())
	{
		node->RemoveComponent<VCNPhysicsComponent>();
	}

	VCNPhysicsComponent* newComponent = (VCNPhysicsComponent*)ComponentFactory::CreateNewComponent(VCNTXT("physicscomponent"));
	newComponent->SetActive(true);
	newComponent->SetIsDynamic(true);
	newComponent->SetIsKinematic(true);
	newComponent->SetHasGravity(false);
	node->AddComponent(newComponent);
	VCNIComponent::Parameters p;
	newComponent->Initialise(p);
}

void VCNSlidingBridgeComponent::UseComponent()
{
	if (mMovingState == BRIDGE_OPENED)
	{
		return;
	}

	// Return if the door cannot be opened (locked for example)
	if (!mslideBridge.IsEmpty())
	{
		if (mslideBridge.Trigger() == false)
		{
			return;
		}
	}
	// TODO: Put this in the lua code
	mMovingState = BRIDGE_OPENING;
}

//**EndCodeRegion
