#include "Precompiled.h"                                     
#include "TestBert.h"                               
                                                               
// DO NOT DECLARE MORE CODE REGIONS. USE THE ONES ALREADY IN THE FILE 
                                                               
//**CodeRegion
//**EndCodeRegion
                                                               
VCNTestBert::VCNTestBert()                    
{                                                              
    // Default values                                          
    mSpeedFactor = 1;                   
    mRotationAxis = Vector3(0.0f, 0.0f, 0.0f);                   
    mRotationPosition = 0;                   
//**CodeRegion
//**EndCodeRegion
}                                                              
                                                               
void VCNTestBert::Initialise(const Parameters& params)
{                                                              
//**CodeRegion
//**EndCodeRegion
}                                                              
                                                               
void VCNTestBert::Update(VCNFloat dt)                 
{                                                              
//**CodeRegion
//**EndCodeRegion
}                                                              
                                                               
bool VCNTestBert::SetAttribute( const VCNString& attributeName, const VCNString& attributeValue )
{                                                              
    if (attributeName == L"SpeedFactor")                       
    {                                                                      
        mSpeedFactor = _wtof(attributeValue.c_str());            
        return true;                                                       
    }                                                                      
    else if (attributeName == L"RotationAxis")                       
    {                                                                      
        std::stringstream ss;                                              
        std::string str( attributeValue.begin(), attributeValue.end() );   
        ss << str;                                                         
        ss >> mRotationAxis;                                      
        return true;                                                       
    }                                                                      
    else if (attributeName == L"RotationPosition")                       
    {                                                                      
        mRotationPosition = _wtof(attributeValue.c_str());            
        return true;                                                       
    }                                                                      
    return false;                                              
}                                                              
                                                               
VCNIComponent::Ptr VCNTestBert::Copy() const                          
{                                                                              
    VCNTestBert* otherComponent = new VCNTestBert();         
                                                                               
    otherComponent->mSpeedFactor = mSpeedFactor;
    otherComponent->mRotationAxis = Vector3(mRotationAxis);
    otherComponent->mRotationPosition = mRotationPosition;
                                                                               
    return std::shared_ptr<VCNTestBert>(otherComponent);              
}                                                                              
                                                               
                                                               
//**CodeRegion
//**EndCodeRegion
