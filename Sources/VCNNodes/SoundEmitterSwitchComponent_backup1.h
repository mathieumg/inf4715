#pragma once                                                                                         
                                                                                                     
                                                                                                     
// DO NOT DECLARE MORE CODE REGIONS. USE THE ONES ALREADY IN THE FILE                                
                                                                                                     
#include "VCNLua\LuaTrigger.h"                                                                    
#include "SoundEmitterComponent.h"                                               
//**CodeRegion
//**EndCodeRegion
                                                                                                     
class VCNSoundEmitterSwitchComponent :   public VCNSoundEmitterComponent                        
{                                                                                                    
public:                                                                                              
    VCNSoundEmitterSwitchComponent();                                                                          
                                                                                                     
    virtual void Initialise(const Parameters& params) override;                                      
    virtual void Update(VCNFloat dt) override;                                                       
                                                                                                     
    virtual bool SetAttribute( const VCNString& attributeName, const VCNString& attributeValue );    
                                                                                                     
    virtual VCNIComponent::Ptr Copy() const;                                                         
                                                                                                     
    inline const VCNString& GetSwitchName() const { return mSwitchName; } 
    inline void SetSwitchName(const VCNString& val) { mSwitchName = val; } 
                                                                                               
    inline const VCNString& GetSwitchValue() const { return mSwitchValue; } 
    inline void SetSwitchValue(const VCNString& val) { mSwitchValue = val; } 
                                                                                               
protected:                                                                                         
    VCNString mSwitchName;                                               
    VCNString mSwitchValue;                                               
                                                                                                   
//**CodeRegion
	VCNUInt mGameObjectID;

public:
	virtual void Play();
//**EndCodeRegion
};                                                                                                 
                                                                                                   
template class VCNComponentBase<VCNSoundEmitterSwitchComponent>;                                             
