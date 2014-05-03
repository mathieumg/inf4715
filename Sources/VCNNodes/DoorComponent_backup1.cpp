#include "Precompiled.h"                                     
#include "DoorComponent.h"                               
                                                               
// DO NOT DECLARE MORE CODE REGIONS. USE THE ONES ALREADY IN THE FILE 
                                                               
//**CodeRegion
#include "PhysicsComponent.h"
#include "ComponentFactory.h"
#include "VCNUtils\Utilities.h"
#include "VCNAudio\AudioCore.h"
#include "SoundEmitterComponent.h"
//**EndCodeRegion
                                                               
VCNDoorComponent::VCNDoorComponent()                    
{                                                              
    // Default values                                          
    mOpeningSpeed = 1;                   
    mClosingSpeed = 1;                   
    mStartingMovingState = 0;                   
    mOpenValidationScript = LuaTrigger(L"", L"");                   
    mOnOpenTrigger = LuaTrigger(L"", L"");                   
    mOnCloseTrigger = LuaTrigger(L"", L"");                   
//**CodeRegion
	mMovingState = (DoorMovingState)mStartingMovingState;
//**EndCodeRegion
}                                                              
                                                               
void VCNDoorComponent::Initialise(const Parameters& params)
{                                                              
    VCNUsableComponent::Initialise(params); 
//**CodeRegion
	// Make sure that we have a physics actor
	SetupPhysicsComponentOnNode(GetOwner());
	

	
//**EndCodeRegion
}                                                              
                                                               
void VCNDoorComponent::Update(VCNFloat dt)                 
{                                                              
    VCNUsableComponent::Update(dt);    
//**CodeRegion
	switch (mMovingState)
	{
	case DOOR_OPENING:
		UpdateOpening(dt);
		break;
	case DOOR_CLOSING:
		UpdateClosing(dt);
		break;
	default:
		break;
	}
//**EndCodeRegion
}                                                              
                                                               
bool VCNDoorComponent::SetAttribute( const VCNString& attributeName, const VCNString& attributeValue )
{                                                              
    if (attributeName == L"OpeningSpeed")                       
    {                                                                      
        mOpeningSpeed = (float)_wtof(attributeValue.c_str());     
        return true;                                                       
    }                                                                      
    else if (attributeName == L"ClosingSpeed")                       
    {                                                                      
        mClosingSpeed = (float)_wtof(attributeValue.c_str());     
        return true;                                                       
    }                                                                      
    else if (attributeName == L"StartingMovingState")                       
    {                                                                      
        mStartingMovingState = (int)_wtoi(attributeValue.c_str());       
        return true;                                                       
    }                                                                      
    else if (attributeName == L"OpenValidationScript")                       
    {                                                                      
        std::stringstream ss;                                              
        std::string str( attributeValue.begin(), attributeValue.end() );   
        ss << str;                                                         
        ss >> mOpenValidationScript;                                      
        return true;                                                       
    }                                                                      
    else if (attributeName == L"OnOpenTrigger")                       
    {                                                                      
        std::stringstream ss;                                              
        std::string str( attributeValue.begin(), attributeValue.end() );   
        ss << str;                                                         
        ss >> mOnOpenTrigger;                                      
        return true;                                                       
    }                                                                      
    else if (attributeName == L"OnCloseTrigger")                       
    {                                                                      
        std::stringstream ss;                                              
        std::string str( attributeValue.begin(), attributeValue.end() );   
        ss << str;                                                         
        ss >> mOnCloseTrigger;                                      
        return true;                                                       
    }                                                                      
    VCNUsableComponent::SetAttribute(attributeName, attributeValue); 
    return false;                                              
}                                                              
                                                               
VCNIComponent::Ptr VCNDoorComponent::Copy() const                          
{                                                                              
    VCNDoorComponent* otherComponent = new VCNDoorComponent();         
                                                                               
    otherComponent->mOpeningSpeed = mOpeningSpeed;
    otherComponent->mClosingSpeed = mClosingSpeed;
    otherComponent->mStartingMovingState = mStartingMovingState;
    otherComponent->mOpenValidationScript = LuaTrigger(mOpenValidationScript);
    otherComponent->mOnOpenTrigger = LuaTrigger(mOnOpenTrigger);
    otherComponent->mOnCloseTrigger = LuaTrigger(mOnCloseTrigger);
    otherComponent->mIsSelectable = mIsSelectable;
    otherComponent->mUseDistance = mUseDistance;
                                                                               
    return std::shared_ptr<VCNDoorComponent>(otherComponent);              
}                                                                              
                                                               
                                                               
//**CodeRegion

void VCNDoorComponent::Open()
{
	// Return if the door cannot be opened (locked for example)
	if (!mOpenValidationScript.IsEmpty())
	{
		if (mOpenValidationScript.Trigger() == false)
		{
			return;
		}
	}
	// TODO: Put this in the lua code
	mMovingState = DOOR_OPENING;
	mOnOpenTrigger.Trigger();
}

void VCNDoorComponent::Close()
{
	mMovingState = DOOR_CLOSING;
	mOnCloseTrigger.Trigger();
}


void VCNDoorComponent::UpdateOpening( float dt )
{
 	
} 

void VCNDoorComponent::UpdateClosing( float dt )
{
	
}


VCNPhysicsComponent* VCNDoorComponent::GetPhysicsComponent()
{
	return GetOwner()->GetComponent<VCNPhysicsComponent>();
}

void VCNDoorComponent::UseComponent()
{
	switch (mMovingState)
	{
	case DOOR_CLOSING:
	case DOOR_CLOSED:
		Open();
		break;
	case DOOR_OPENING:
	case DOOR_OPENED:
		Close();
		break;
	default:
		break;
	}

	VCNSoundEmitterComponent* e = GetOwner()->GetComponent<VCNSoundEmitterComponent>();
	if (e != NULL)
	{
		e->Play();
	}
}

void VCNDoorComponent::SetupPhysicsComponentOnNode( VCNNode* node )
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
	if (mIsSelectable)
	{
		node->SetSelectable(true);
	}
}



//**EndCodeRegion
