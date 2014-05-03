#include "Precompiled.h"                                     
#include "SoundEmitterStateComponent.h"                               
                                                               
// DO NOT DECLARE MORE CODE REGIONS. USE THE ONES ALREADY IN THE FILE 
                                                               
//**CodeRegion
#include "VCNAudio\AudioCore.h"
//**EndCodeRegion
                                                               
VCNSoundEmitterStateComponent::VCNSoundEmitterStateComponent()                    
{                                                              
    // Default values                                          
    mStateName = L"";                   
    mStateValue = L"";                   
//**CodeRegion
//**EndCodeRegion
}                                                              
                                                               
void VCNSoundEmitterStateComponent::Initialise(const Parameters& params)
{                                                              
    VCNSoundEmitterComponent::Initialise(params); 
//**CodeRegion
//**EndCodeRegion
}                                                              
                                                               
void VCNSoundEmitterStateComponent::Update(VCNFloat dt)                 
{                                                              
    VCNSoundEmitterComponent::Update(dt);    
//**CodeRegion
//**EndCodeRegion
}                                                              
                                                               
bool VCNSoundEmitterStateComponent::SetAttribute( const VCNString& attributeName, const VCNString& attributeValue )
{                                                              
    if (attributeName == L"StateName")                       
    {                                                                      
        mStateName = attributeValue;                           
        return true;                                                       
    }                                                                      
    else if (attributeName == L"StateValue")                       
    {                                                                      
        mStateValue = attributeValue;                           
        return true;                                                       
    }                                                                      
    VCNSoundEmitterComponent::SetAttribute(attributeName, attributeValue); 
    return false;                                              
}                                                              
                                                               
VCNIComponent::Ptr VCNSoundEmitterStateComponent::Copy() const                          
{                                                                              
    VCNSoundEmitterStateComponent* otherComponent = new VCNSoundEmitterStateComponent();         
                                                                               
    otherComponent->mStateName = VCNString(mStateName);
    otherComponent->mStateValue = VCNString(mStateValue);
    otherComponent->mIsEnabled = mIsEnabled;
    otherComponent->mVolume = mVolume;
    otherComponent->mMinDistance = mMinDistance;
    otherComponent->mMaxDistance = mMaxDistance;
    otherComponent->mStartOnLoad = mStartOnLoad;
                                                                               
    return std::shared_ptr<VCNSoundEmitterStateComponent>(otherComponent);              
}                                                                              
                                                               
                                                               
//**CodeRegion

void VCNSoundEmitterStateComponent::Play()
{
	VCNAudioCore* audioCore = VCNAudioCore::GetInstance();
	audioCore->SetState(mStateName, mStateValue);
}


//**EndCodeRegion
