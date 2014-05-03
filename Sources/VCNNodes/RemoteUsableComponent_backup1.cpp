#include "Precompiled.h"                                     
#include "RemoteUsableComponent.h"                               
                                                               
// DO NOT DECLARE MORE CODE REGIONS. USE THE ONES ALREADY IN THE FILE 
                                                               
//**CodeRegion
#include "NodeCore.h"
#include "PhysicsComponent.h"
#include "ComponentFactory.h"
//**EndCodeRegion
                                                               
VCNRemoteUsableComponent::VCNRemoteUsableComponent()                    
{                                                              
    // Default values                                          
    mRemoteNodeName = L"";                   
//**CodeRegion
//**EndCodeRegion
}                                                              
                                                               
void VCNRemoteUsableComponent::Initialise(const Parameters& params)
{                                                              
    VCNUsableComponent::Initialise(params); 
//**CodeRegion
	mRootNodeID = VCNNodeCore::GetInstance()->GetNodeByName(mRemoteNodeName);

	VCNNode* node = GetOwner();
	if (!node->HasComponent<VCNPhysicsComponent>())
	{
		VCNPhysicsComponent* newComponent = (VCNPhysicsComponent*)ComponentFactory::CreateNewComponent(VCNTXT("physicscomponent"));
		newComponent->SetActive(true);
		newComponent->SetIsDynamic(false);
		newComponent->SetIsKinematic(false);
		newComponent->SetHasGravity(false);
		node->AddComponent(newComponent);
		VCNIComponent::Parameters p;
		newComponent->Initialise(p);
		node->SetSelectable(true);
	}
//**EndCodeRegion
}                                                              
                                                               
void VCNRemoteUsableComponent::Update(VCNFloat dt)                 
{                                                              
    VCNUsableComponent::Update(dt);    
//**CodeRegion
//**EndCodeRegion
}                                                              
                                                               
bool VCNRemoteUsableComponent::SetAttribute( const VCNString& attributeName, const VCNString& attributeValue )
{                                                              
    if (attributeName == L"RemoteNodeName")                       
    {                                                                      
        mRemoteNodeName = attributeValue;                           
        return true;                                                       
    }                                                                      
    VCNUsableComponent::SetAttribute(attributeName, attributeValue); 
    return false;                                              
}                                                              
                                                               
VCNIComponent::Ptr VCNRemoteUsableComponent::Copy() const                          
{                                                                              
    VCNRemoteUsableComponent* otherComponent = new VCNRemoteUsableComponent();         
                                                                               
    otherComponent->mRemoteNodeName = VCNString(mRemoteNodeName);
    otherComponent->mIsSelectable = mIsSelectable;
    otherComponent->mUseDistance = mUseDistance;
                                                                               
    return std::shared_ptr<VCNRemoteUsableComponent>(otherComponent);              
}                                                                              
                                                               
                                                               
//**CodeRegion
void VCNRemoteUsableComponent::UseComponent()
{
	VCNNode* node = VCNNodeCore::GetInstance()->GetNode(mRootNodeID);
	if (node)
	{
		VCNUsableComponent* usable = node->GetComponent<VCNUsableComponent>();
		if (usable)
		{
			usable->UseComponent();
		}
	}
}
//**EndCodeRegion
