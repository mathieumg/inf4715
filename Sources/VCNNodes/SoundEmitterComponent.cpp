#include "Precompiled.h"                                     
#include "SoundEmitterComponent.h"                               
                                                               
// DO NOT DECLARE MORE CODE REGIONS. USE THE ONES ALREADY IN THE FILE 
                                                               
//**CodeRegion
#include "VCNWwise\WwiseAudioCore.h"
//**EndCodeRegion
                                                               
VCNSoundEmitterComponent::VCNSoundEmitterComponent()                    
{                                                              
    // Default values                                          
    mIsEnabled = false;                   
    mVolume = 1;                   
    mMinDistance = 0.01;                   
    mMaxDistance = 1000;                   
    mStartOnLoad = false;                   
//**CodeRegion
//**EndCodeRegion
}                                                              
                                                               
void VCNSoundEmitterComponent::Initialise(const Parameters& params)
{                                                              
//**CodeRegion

// 	VCNAudioCore* audioCore = VCNWwiseAudioCore::GetInstance();
// 	if (mIs3D)
// 	{
// 		mResID = audioCore->Create3DSoundEmitter(GetOwner()->GetTag() + VCNTXT("_soundEmitter"), mSoundName, GetOwner()->GetWorldTranslation(), mMinDistance, mMaxDistance, mLoop, mVolume, (mIsStream ? Playback_Stream : Playback_Sound));
// 	}
// 	else
// 	{
// 		mResID = audioCore->Create2DSoundEmitter(GetOwner()->GetTag() + VCNTXT("_soundEmitter"), mSoundName, mLoop, mVolume, (mIsStream ? Playback_Stream : Playback_Sound));
// 	}
// 
// 	if (mResID != kInvalidResID && mStartOnLoad && mIsEnabled)
// 	{
// 		audioCore->Play(mResID, mLoop);
// 	}


//**EndCodeRegion
}                                                              
                                                               
void VCNSoundEmitterComponent::Update(VCNFloat dt)                 
{                                                              
//**CodeRegion
//**EndCodeRegion
}                                                              
                                                               
bool VCNSoundEmitterComponent::SetAttribute( const VCNString& attributeName, const VCNString& attributeValue )
{                                                              
    if (attributeName == L"IsEnabled")                       
    {                                                                      
        mIsEnabled = (attributeValue != L"0");                
        return true;                                                       
    }                                                                      
    else if (attributeName == L"Volume")                       
    {                                                                      
        mVolume = (float)_wtof(attributeValue.c_str());     
        return true;                                                       
    }                                                                      
    else if (attributeName == L"MinDistance")                       
    {                                                                      
        mMinDistance = (float)_wtof(attributeValue.c_str());     
        return true;                                                       
    }                                                                      
    else if (attributeName == L"MaxDistance")                       
    {                                                                      
        mMaxDistance = (float)_wtof(attributeValue.c_str());     
        return true;                                                       
    }                                                                      
    else if (attributeName == L"StartOnLoad")                       
    {                                                                      
        mStartOnLoad = (attributeValue != L"0");                
        return true;                                                       
    }                                                                      
    return false;                                              
}                                                              
                                                               
VCNIComponent::Ptr VCNSoundEmitterComponent::Copy() const                          
{                                                                              
    VCNSoundEmitterComponent* otherComponent = new VCNSoundEmitterComponent();         
                                                                               
    otherComponent->mIsEnabled = mIsEnabled;
    otherComponent->mVolume = mVolume;
    otherComponent->mMinDistance = mMinDistance;
    otherComponent->mMaxDistance = mMaxDistance;
    otherComponent->mStartOnLoad = mStartOnLoad;
                                                                               
    return std::shared_ptr<VCNSoundEmitterComponent>(otherComponent);              
}                                                                              
                                                               
                                                               
//**CodeRegion

void VCNSoundEmitterComponent::Play()
{
}


//**EndCodeRegion
