#include "Precompiled.h"                                     
#include "SoundEmitterSwitchComponent.h"                               
                                                               
// DO NOT DECLARE MORE CODE REGIONS. USE THE ONES ALREADY IN THE FILE 
                                                               
//**CodeRegion
#include "VCNAudio\AudioCore.h"
//**EndCodeRegion
                                                               
VCNSoundEmitterSwitchComponent::VCNSoundEmitterSwitchComponent()                    
{                                                              
    // Default values                                          
    mSwitchName = L"";                   
    mSwitchValue = L"";                   
//**CodeRegion
//**EndCodeRegion
}                                                              
                                                               
void VCNSoundEmitterSwitchComponent::Initialise(const Parameters& params)
{                                                              
    VCNSoundEmitterComponent::Initialise(params); 
//**CodeRegion
	// Create a GameObject for this node
	VCNAudioCore* audioCore = VCNAudioCore::GetInstance();
	mGameObjectID = audioCore->GetUniqueGameObjectID();
	audioCore->RegisterGameObj(mGameObjectID, GetOwner()->GetTag());
//**EndCodeRegion
}                                                              
                                                               
void VCNSoundEmitterSwitchComponent::Update(VCNFloat dt)                 
{                                                              
    VCNSoundEmitterComponent::Update(dt);    
//**CodeRegion
	VCNAudioCore* audioCore = VCNAudioCore::GetInstance();
	// ** Hack, give an orientation
	audioCore->SetPosition(mGameObjectID, GetOwner()->GetWorldTranslation(), Vector3(0.0f, -1.0f, 0.0f));
//**EndCodeRegion
}                                                              
                                                               
bool VCNSoundEmitterSwitchComponent::SetAttribute( const VCNString& attributeName, const VCNString& attributeValue )
{                                                              
    if (attributeName == L"SwitchName")                       
    {                                                                      
        mSwitchName = attributeValue;                           
        return true;                                                       
    }                                                                      
    else if (attributeName == L"SwitchValue")                       
    {                                                                      
        mSwitchValue = attributeValue;                           
        return true;                                                       
    }                                                                      
    VCNSoundEmitterComponent::SetAttribute(attributeName, attributeValue); 
    return false;                                              
}                                                              
                                                               
VCNIComponent::Ptr VCNSoundEmitterSwitchComponent::Copy() const                          
{                                                                              
    VCNSoundEmitterSwitchComponent* otherComponent = new VCNSoundEmitterSwitchComponent();         
                                                                               
    otherComponent->mSwitchName = VCNString(mSwitchName);
    otherComponent->mSwitchValue = VCNString(mSwitchValue);
    otherComponent->mIsEnabled = mIsEnabled;
    otherComponent->mVolume = mVolume;
    otherComponent->mMinDistance = mMinDistance;
    otherComponent->mMaxDistance = mMaxDistance;
    otherComponent->mStartOnLoad = mStartOnLoad;
                                                                               
    return std::shared_ptr<VCNSoundEmitterSwitchComponent>(otherComponent);              
}                                                                              
                                                               
                                                               
//**CodeRegion
void VCNSoundEmitterSwitchComponent::Play()
{
	VCNAudioCore* audioCore = VCNAudioCore::GetInstance();
	audioCore->SetSwitch(mSwitchName, mSwitchValue, mGameObjectID);
}

//**EndCodeRegion
