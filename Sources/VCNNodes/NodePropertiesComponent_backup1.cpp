#include "Precompiled.h"                                     
#include "NodePropertiesComponent.h"                               
                                                               
// DO NOT DECLARE MORE CODE REGIONS. USE THE ONES ALREADY IN THE FILE 
                                                               
//**CodeRegion
//**EndCodeRegion
                                                               
VCNNodePropertiesComponent::VCNNodePropertiesComponent()                    
{                                                              
    // Default values                                          
    mIsNodeSelectable = false;                   
    mIsNodeActive = true;                   
    mNodeTag = L"";                   
    mReadableName = L"";                   
//**CodeRegion
//**EndCodeRegion
}                                                              
                                                               
void VCNNodePropertiesComponent::Initialise(const Parameters& params)
{                                                              
//**CodeRegion
	VCNNode* owner = GetOwner();
	owner->SetSelectable(mIsNodeSelectable);
	owner->SetActive(mIsNodeActive);
	owner->SetTag(mNodeTag);
//**EndCodeRegion
}                                                              
                                                               
void VCNNodePropertiesComponent::Update(VCNFloat dt)                 
{                                                              
//**CodeRegion
//**EndCodeRegion
}                                                              
                                                               
bool VCNNodePropertiesComponent::SetAttribute( const VCNString& attributeName, const VCNString& attributeValue )
{                                                              
    if (attributeName == L"IsNodeSelectable")                       
    {                                                                      
        mIsNodeSelectable = (attributeValue != L"0");                
        return true;                                                       
    }                                                                      
    else if (attributeName == L"IsNodeActive")                       
    {                                                                      
        mIsNodeActive = (attributeValue != L"0");                
        return true;                                                       
    }                                                                      
    else if (attributeName == L"NodeTag")                       
    {                                                                      
        mNodeTag = attributeValue;                           
        return true;                                                       
    }                                                                      
    else if (attributeName == L"ReadableName")                       
    {                                                                      
        mReadableName = attributeValue;                           
        return true;                                                       
    }                                                                      
    return false;                                              
}                                                              
                                                               
VCNIComponent::Ptr VCNNodePropertiesComponent::Copy() const                          
{                                                                              
    VCNNodePropertiesComponent* otherComponent = new VCNNodePropertiesComponent();         
                                                                               
    otherComponent->mIsNodeSelectable = mIsNodeSelectable;
    otherComponent->mIsNodeActive = mIsNodeActive;
    otherComponent->mNodeTag = VCNString(mNodeTag);
    otherComponent->mReadableName = VCNString(mReadableName);
                                                                               
    return std::shared_ptr<VCNNodePropertiesComponent>(otherComponent);              
}                                                                              
                                                               
                                                               
//**CodeRegion
//**EndCodeRegion
