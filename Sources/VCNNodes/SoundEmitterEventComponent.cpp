#include "Precompiled.h"                                     
#include "SoundEmitterEventComponent.h"                               
                                                               
// DO NOT DECLARE MORE CODE REGIONS. USE THE ONES ALREADY IN THE FILE 
                                                               
//**CodeRegion
#include "VCNAudio\AudioCore.h"
//**EndCodeRegion
                                                               
VCNSoundEmitterEventComponent::VCNSoundEmitterEventComponent()                    
{                                                              
    // Default values                                          
    mEventName = L"";                   
//**CodeRegion
	mGameObjectID = 0;
//**EndCodeRegion
}                                                              
                                                               
void VCNSoundEmitterEventComponent::Initialise(const Parameters& params)
{                                                              
    VCNSoundEmitterComponent::Initialise(params); 
//**CodeRegion
	// Create a GameObject for this node
	VCNAudioCore* audioCore = VCNAudioCore::GetInstance();
	mGameObjectID = audioCore->GetUniqueGameObjectID();
	audioCore->RegisterGameObj(mGameObjectID, GetOwner()->GetTag());
	audioCore->SetAttenuationScalingFactor(mGameObjectID, mMaxDistance/100.0f);
//**EndCodeRegion
}                                                              
                                                               
void VCNSoundEmitterEventComponent::Update(VCNFloat dt)                 
{                                                              
    VCNSoundEmitterComponent::Update(dt);    
//**CodeRegion
	VCNAudioCore* audioCore = VCNAudioCore::GetInstance();
	// ** Hack, give an orientation
	audioCore->SetPosition(mGameObjectID, GetOwner()->GetWorldTranslation(), Vector3(0.0f, -1.0f, 0.0f));
//**EndCodeRegion
}                                                              
                                                               
bool VCNSoundEmitterEventComponent::SetAttribute( const VCNString& attributeName, const VCNString& attributeValue )
{                                                              
    if (attributeName == L"EventName")                       
    {                                                                      
        mEventName = attributeValue;                           
        return true;                                                       
    }                                                                      
    VCNSoundEmitterComponent::SetAttribute(attributeName, attributeValue); 
    return false;                                              
}                                                              
                                                               
VCNIComponent::Ptr VCNSoundEmitterEventComponent::Copy() const                          
{                                                                              
    VCNSoundEmitterEventComponent* otherComponent = new VCNSoundEmitterEventComponent();         
                                                                               
    otherComponent->mEventName = VCNString(mEventName);
    otherComponent->mIsEnabled = mIsEnabled;
    otherComponent->mVolume = mVolume;
    otherComponent->mMinDistance = mMinDistance;
    otherComponent->mMaxDistance = mMaxDistance;
    otherComponent->mStartOnLoad = mStartOnLoad;
                                                                               
    return std::shared_ptr<VCNSoundEmitterEventComponent>(otherComponent);              
}                                                                              
                                                               
                                                               
//**CodeRegion

void VCNSoundEmitterEventComponent::Play()
{
	VCNAudioCore* audioCore = VCNAudioCore::GetInstance();
	audioCore->PostEvent(mEventName, mGameObjectID);
}

//**EndCodeRegion
