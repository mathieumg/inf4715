#include "Precompiled.h"                                     
#include "EmptyComponent.h"                               
                                                               
// DO NOT DECLARE MORE CODE REGIONS. USE THE ONES ALREADY IN THE FILE 
                                                               
//**CodeRegion
//**EndCodeRegion
                                                               
VCNEmptyComponent::VCNEmptyComponent()                    
{                                                              
    // Default values                                          
//**CodeRegion
//**EndCodeRegion
}                                                              
                                                               
void VCNEmptyComponent::Initialise(const Parameters& params)
{                                                              
//**CodeRegion
//**EndCodeRegion
}                                                              
                                                               
void VCNEmptyComponent::Update(VCNFloat dt)                 
{                                                              
//**CodeRegion
//**EndCodeRegion
}                                                              
                                                               
bool VCNEmptyComponent::SetAttribute( const VCNString& attributeName, const VCNString& attributeValue )
{                                                              
    return false;                                              
}                                                              
                                                               
VCNIComponent::Ptr VCNEmptyComponent::Copy() const                          
{                                                                              
    VCNEmptyComponent* otherComponent = new VCNEmptyComponent();         
                                                                               
                                                                               
    return std::shared_ptr<VCNEmptyComponent>(otherComponent);              
}                                                                              
                                                               
                                                               
//**CodeRegion
//**EndCodeRegion
