#include "Precompiled.h"                                     
#include "CharacterStartingPointComponent.h"                               
                                                               
// DO NOT DECLARE MORE CODE REGIONS. USE THE ONES ALREADY IN THE FILE 
                                                               
//**CodeRegion
#include "VCNPhysic\PhysicController.h"
#include "VCNPhysic\PhysicCore.h"
#include "NodeCore.h"
//**EndCodeRegion
                                                               
VCNCharacterStartingPointComponent::VCNCharacterStartingPointComponent()                    
{                                                              
    // Default values                                          
    mIsEnabled = true;                   
//**CodeRegion
	
//**EndCodeRegion
}                                                              
                                                               
void VCNCharacterStartingPointComponent::Initialise(const Parameters& params)
{                                                              
//**CodeRegion
	if (mIsEnabled)
	{
		// The node becomes the character controller
		GetOwner()->AddProperty(VCNTXT("Dynamic"));
		VCNNodeCore::GetInstance()->PushName(VCNTXT("Character"), GetOwner()->GetNodeID());
		VCNPhysicController* characterPhysicController = VCNPhysicCore::GetInstance()->CreateCharacterController(GetOwner());
		VCN_ASSERT(characterPhysicController);
		GetOwner()->AddProperty(VCNTXT("PhysicController"), characterPhysicController);
	}
//**EndCodeRegion
}                                                              
                                                               
void VCNCharacterStartingPointComponent::Update(VCNFloat dt)                 
{                                                              
//**CodeRegion
//**EndCodeRegion
}                                                              
                                                               
bool VCNCharacterStartingPointComponent::SetAttribute( const VCNString& attributeName, const VCNString& attributeValue )
{                                                              
    if (attributeName == L"IsEnabled")                       
    {                                                                      
        mIsEnabled = (attributeValue != L"0");                
        return true;                                                       
    }                                                                      
    return false;                                              
}                                                              
                                                               
VCNIComponent::Ptr VCNCharacterStartingPointComponent::Copy() const                          
{                                                                              
    VCNCharacterStartingPointComponent* otherComponent = new VCNCharacterStartingPointComponent();         
                                                                               
    otherComponent->mIsEnabled = mIsEnabled;
                                                                               
    return std::shared_ptr<VCNCharacterStartingPointComponent>(otherComponent);              
}                                                                              
                                                               
                                                               
//**CodeRegion
//**EndCodeRegion
