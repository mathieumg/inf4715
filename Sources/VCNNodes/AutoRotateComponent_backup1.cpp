#include "Precompiled.h"                                     
#include "AutoRotateComponent.h"                               
                                                               
// DO NOT DECLARE MORE CODE REGIONS. USE THE ONES ALREADY IN THE FILE 
                                                               
//**CodeRegion
//**EndCodeRegion
                                                               
VCNAutoRotateComponent::VCNAutoRotateComponent()                    
{                                                              
    // Default values                                          
    mSpeedFactor = 1;                   
    mRotationAxis = Vector3(0.0f, 0.0f, 0.0f);                   
    mRotationPosition = 0;                   
//**CodeRegion
//**EndCodeRegion
}                                                              
                                                               
void VCNAutoRotateComponent::Initialise(const Parameters& params)
{                                                              
//**CodeRegion
//**EndCodeRegion
}                                                              
                                                               
void VCNAutoRotateComponent::Update(VCNFloat dt)                 
{                                                              
//**CodeRegion
	mRotationPosition += dt * mSpeedFactor;
	VCNQuat q = VCNQuat::FromEuler(mRotationAxis.x * mRotationPosition, mRotationAxis.y * mRotationPosition, mRotationAxis.z * mRotationPosition);
	q.Normalize();
	GetOwner()->SetRotation(q);

//**EndCodeRegion
}                                                              
                                                               
bool VCNAutoRotateComponent::SetAttribute( const VCNString& attributeName, const VCNString& attributeValue )
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
                                                               
VCNIComponent::Ptr VCNAutoRotateComponent::Copy() const                          
{                                                                              
    VCNAutoRotateComponent* otherComponent = new VCNAutoRotateComponent();         
                                                                               
    otherComponent->mSpeedFactor = mSpeedFactor;
    otherComponent->mRotationAxis = Vector3(mRotationAxis);
    otherComponent->mRotationPosition = mRotationPosition;
                                                                               
    return std::shared_ptr<VCNAutoRotateComponent>(otherComponent);              
}                                                                              
                                                               
                                                               
//**CodeRegion
//**EndCodeRegion
