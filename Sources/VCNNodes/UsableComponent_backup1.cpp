#include "Precompiled.h"                                     
#include "UsableComponent.h"                               
                                                               
// DO NOT DECLARE MORE CODE REGIONS. USE THE ONES ALREADY IN THE FILE 
                                                               
//**CodeRegion
//**EndCodeRegion
                                                               
VCNUsableComponent::VCNUsableComponent()                    
{                                                              
    // Default values                                          
    mIsSelectable = true;                   
    mUseDistance = 5;                   
//**CodeRegion
	mIsBeingPointed = false;
//**EndCodeRegion
}                                                              
                                                               
void VCNUsableComponent::Initialise(const Parameters& params)
{                                                              
//**CodeRegion
//**EndCodeRegion
}                                                              
                                                               
void VCNUsableComponent::Update(VCNFloat dt)                 
{                                                              
//**CodeRegion
//**EndCodeRegion
}                                                              
                                                               
bool VCNUsableComponent::SetAttribute( const VCNString& attributeName, const VCNString& attributeValue )
{                                                              
    if (attributeName == L"IsSelectable")                       
    {                                                                      
        mIsSelectable = (attributeValue != L"0");                
        return true;                                                       
    }                                                                      
    else if (attributeName == L"UseDistance")                       
    {                                                                      
        mUseDistance = (float)_wtof(attributeValue.c_str());     
        return true;                                                       
    }                                                                      
    return false;                                              
}                                                              
                                                               
VCNIComponent::Ptr VCNUsableComponent::Copy() const                          
{                                                                              
    VCNUsableComponent* otherComponent = new VCNUsableComponent();         
                                                                               
    otherComponent->mIsSelectable = mIsSelectable;
    otherComponent->mUseDistance = mUseDistance;
                                                                               
    return std::shared_ptr<VCNUsableComponent>(otherComponent);              
}                                                                              
                                                               
                                                               
//**CodeRegion
void VCNUsableComponent::UseComponent()
{

}




//**EndCodeRegion
