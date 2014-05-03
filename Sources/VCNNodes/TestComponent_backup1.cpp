#include "Precompiled.h"                                     
#include "TestComponent.h"                               
                                                               
// DO NOT DECLARE MORE CODE REGIONS. USE THE ONES ALREADY IN THE FILE 
                                                               
//**CodeRegion
//**EndCodeRegion
                                                               
VCNTestComponent::VCNTestComponent()                    
{                                                              
    // Default values                                          
    mParam1 = 2;                   
    mParam2 = 0.0f;                   
    mAlloVect2 = Vector2(0.0f, 0.0f);                   
    mPageContent = L"this is a page content";                   
//**CodeRegion
//**EndCodeRegion
}                                                              
                                                               
void VCNTestComponent::Initialise(const Parameters& params)
{                                                              
//**CodeRegion
//**EndCodeRegion
}                                                              
                                                               
void VCNTestComponent::Update(VCNFloat dt)                 
{                                                              
//**CodeRegion
//**EndCodeRegion
}                                                              
                                                               
bool VCNTestComponent::SetAttribute( const VCNString& attributeName, const VCNString& attributeValue )
{                                                              
    if (attributeName == L"Param1")                       
    {                                                                      
        mParam1 = (int)_wtoi(attributeValue.c_str());       
        return true;                                                       
    }                                                                      
    else if (attributeName == L"Param2")                       
    {                                                                      
        mParam2 = (float)_wtof(attributeValue.c_str());     
        return true;                                                       
    }                                                                      
    else if (attributeName == L"AlloVect2")                       
    {                                                                      
        std::stringstream ss;                                              
        std::string str( attributeValue.begin(), attributeValue.end() );   
        ss << str;                                                         
        ss >> mAlloVect2;                                      
        return true;                                                       
    }                                                                      
    else if (attributeName == L"PageContent")                       
    {                                                                      
        mPageContent = attributeValue;                           
        return true;                                                       
    }                                                                      
    return false;                                              
}                                                              
                                                               
VCNIComponent::Ptr VCNTestComponent::Copy() const                          
{                                                                              
    VCNTestComponent* otherComponent = new VCNTestComponent();         
                                                                               
    otherComponent->mParam1 = mParam1;
    otherComponent->mParam2 = mParam2;
    otherComponent->mAlloVect2 = Vector2(mAlloVect2);
    otherComponent->mPageContent = VCNString(mPageContent);
                                                                               
    return std::shared_ptr<VCNTestComponent>(otherComponent);              
}                                                                              
                                                               
                                                               
//**CodeRegion
//**EndCodeRegion
