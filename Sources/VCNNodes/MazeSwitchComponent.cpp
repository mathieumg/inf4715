#include "Precompiled.h"                                     
#include "MazeSwitchComponent.h"                               
                                                               
// DO NOT DECLARE MORE CODE REGIONS. USE THE ONES ALREADY IN THE FILE 
                                                               
//**CodeRegion
#include "PhysicsComponent.h"
#include "ComponentFactory.h"
//**EndCodeRegion
                                                               
VCNMazeSwitchComponent::VCNMazeSwitchComponent()                    
{                                                              
    // Default values                                          
    mRemoteNodeName = L"";                   
    mToggleSwitch = LuaTrigger(L"", L"");                   
//**CodeRegion
//**EndCodeRegion
}                                                              
                                                               
void VCNMazeSwitchComponent::Initialise(const Parameters& params)
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
                                                               
void VCNMazeSwitchComponent::Update(VCNFloat dt)                 
{                                                              
    VCNUsableComponent::Update(dt);    
//**CodeRegion
//**EndCodeRegion
}                                                              
                                                               
bool VCNMazeSwitchComponent::SetAttribute( const VCNString& attributeName, const VCNString& attributeValue )
{                                                              
    if (attributeName == L"RemoteNodeName")                       
    {                                                                      
        mRemoteNodeName = attributeValue;                           
        return true;                                                       
    }                                                                      
    else if (attributeName == L"ToggleSwitch")                       
    {                                                                      
        std::stringstream ss;                                              
        std::string str( attributeValue.begin(), attributeValue.end() );   
        ss << str;                                                         
        ss >> mToggleSwitch;                                      
        return true;                                                       
    }                                                                      
    VCNUsableComponent::SetAttribute(attributeName, attributeValue); 
    return false;                                              
}                                                              
                                                               
VCNIComponent::Ptr VCNMazeSwitchComponent::Copy() const                          
{                                                                              
    VCNMazeSwitchComponent* otherComponent = new VCNMazeSwitchComponent();         
                                                                               
    otherComponent->mRemoteNodeName = VCNString(mRemoteNodeName);
    otherComponent->mToggleSwitch = LuaTrigger(mToggleSwitch);
    otherComponent->mIsSelectable = mIsSelectable;
    otherComponent->mUseDistance = mUseDistance;
                                                                               
    return std::shared_ptr<VCNMazeSwitchComponent>(otherComponent);              
}                                                                              
                                                               
                                                               
//**CodeRegion
void VCNMazeSwitchComponent::UseComponent()
{
	bool res = mToggleSwitch.Trigger();
	if(res)
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
}
//**EndCodeRegion
