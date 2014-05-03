#pragma once                                                                                         
                                                                                                     
                                                                                                     
// DO NOT DECLARE MORE CODE REGIONS. USE THE ONES ALREADY IN THE FILE                                
                                                                                                     
#include "VCNLua\LuaTrigger.h"                                                                    
#include "ComponentBase.h"                                                                     
//**CodeRegion
//**EndCodeRegion
                                                                                                     
class VCNSoundEmitterComponent :   public VCNComponentBase<VCNSoundEmitterComponent>,                    
                            public VCNIUpdatable                                                 
{                                                                                                    
public:                                                                                              
    VCNSoundEmitterComponent();                                                                          
                                                                                                     
    virtual void Initialise(const Parameters& params) override;                                      
    virtual void Update(VCNFloat dt) override;                                                       
                                                                                                     
    virtual bool SetAttribute( const VCNString& attributeName, const VCNString& attributeValue );    
                                                                                                     
    virtual VCNIComponent::Ptr Copy() const;                                                         
                                                                                                     
    inline const bool& GetIsEnabled() const { return mIsEnabled; } 
    inline void SetIsEnabled(const bool& val) { mIsEnabled = val; } 
                                                                                               
    inline const float& GetVolume() const { return mVolume; } 
                                                                                               
    inline const float& GetMinDistance() const { return mMinDistance; } 
                                                                                               
    inline const float& GetMaxDistance() const { return mMaxDistance; } 
                                                                                               
    inline const bool& GetStartOnLoad() const { return mStartOnLoad; } 
                                                                                               
protected:                                                                                         
    bool mIsEnabled;                                               
    float mVolume;                                               
    float mMinDistance;                                               
    float mMaxDistance;                                               
    bool mStartOnLoad;                                               
                                                                                                   
//**CodeRegion
public:
	virtual void Play();
//**EndCodeRegion
};                                                                                                 
                                                                                                   
template class VCNComponentBase<VCNSoundEmitterComponent>;                                             
