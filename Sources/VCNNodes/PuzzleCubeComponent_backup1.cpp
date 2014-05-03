#include "Precompiled.h"                                     
#include "PuzzleCubeComponent.h"                               
                                                               
// DO NOT DECLARE MORE CODE REGIONS. USE THE ONES ALREADY IN THE FILE 
                                                               
//**CodeRegion
#include "ComponentFactory.h"
#include "PhysicsComponent.h"
#include "NodeCore.h"
//**EndCodeRegion
                                                               
VCNPuzzleCubeComponent::VCNPuzzleCubeComponent()                    
{                                                              
    // Default values                                          
    mPuzzlePiece = 1;                   
    mPushForce = 1;                   
    mLockOnAxis = true;                   
//**CodeRegion
	mPushingState = Not_Pushing;
	mLastPushingState = Not_Pushing;
//**EndCodeRegion
}                                                              
                                                               
void VCNPuzzleCubeComponent::Initialise(const Parameters& params)
{                                                              
//**CodeRegion
	// Make sure that we have a PhysicsComponent
	VCNPhysicsComponent* testPtr = GetOwner()->GetComponent<VCNPhysicsComponent>();
	if (testPtr == NULL)
	{
		// Need to add one
		VCNPhysicsComponent* newComponent = (VCNPhysicsComponent*)ComponentFactory::CreateNewComponent(VCNTXT("physicscomponent"));
		newComponent->SetActive(true);
		newComponent->SetIsDynamic(true);
		newComponent->SetHasGravity(false);
		GetOwner()->AddComponent(newComponent);
		VCNIComponent::Parameters p;
		newComponent->Initialise(p);
	}


//**EndCodeRegion
}                                                              
                                                               
void VCNPuzzleCubeComponent::Update(VCNFloat dt)                 
{                                                              
//**CodeRegion
	VCNPhysicsComponent* physicComponent = GetOwner()->GetComponent<VCNPhysicsComponent>();
	if (physicComponent != NULL)
	{
        VCNPhysicActor* actor = physicComponent->GetPhysicsActor();
		
		if (mLastPushingState == Not_Pushing && mPushingState != Not_Pushing)
		{
            // Start pushing
			actor->SetLinearVelocity(mPushDirection * mPushForce);

		}
		else if (mLastPushingState != Not_Pushing && mPushingState == Not_Pushing)
		{
            // Stopped pushing
            actor->SetLinearVelocity(Vector3(0));
		}
		else if (mLastPushingState != Not_Pushing && mPushingState != Not_Pushing)
		{
			// Keep pushing
            actor->SetLinearVelocity(mPushDirection * mPushForce);
		}
		const Matrix4& physicsTransform = actor->GetTransform();

		VCNNode* parentNode = VCNNodeCore::GetInstance()->GetNode(GetOwner()->GetParent());
		
		GetOwner()->SetRotation( physicsTransform.GetRotation() );
		GetOwner()->SetTranslation( physicsTransform.GetTranslation() - (parentNode ? (parentNode->GetWorldTranslation()) : (Vector3::Zero)) );
	}
	mLastPushingState = mPushingState;
    mPushingState = Not_Pushing;
//**EndCodeRegion
}                                                              
                                                               
bool VCNPuzzleCubeComponent::SetAttribute( const VCNString& attributeName, const VCNString& attributeValue )
{                                                              
    if (attributeName == L"PuzzlePiece")                       
    {                                                                      
        mPuzzlePiece = (int)_wtoi(attributeValue.c_str());       
        return true;                                                       
    }                                                                      
    else if (attributeName == L"PushForce")                       
    {                                                                      
        mPushForce = (float)_wtof(attributeValue.c_str());     
        return true;                                                       
    }                                                                      
    else if (attributeName == L"LockOnAxis")                       
    {                                                                      
        mLockOnAxis = (attributeValue != L"0");                
        return true;                                                       
    }                                                                      
    return false;                                              
}                                                              
                                                               
VCNIComponent::Ptr VCNPuzzleCubeComponent::Copy() const                          
{                                                                              
    VCNPuzzleCubeComponent* otherComponent = new VCNPuzzleCubeComponent();         
                                                                               
    otherComponent->mPuzzlePiece = mPuzzlePiece;
    otherComponent->mPushForce = mPushForce;
    otherComponent->mLockOnAxis = mLockOnAxis;
                                                                               
    return std::shared_ptr<VCNPuzzleCubeComponent>(otherComponent);              
}                                                                              
                                                               
                                                               
//**CodeRegion
void VCNPuzzleCubeComponent::OnCharacterHit( const Vector3& hitDir )
{
	mPushingState = Pushing;
	mPushDirection = hitDir;
}
//**EndCodeRegion
