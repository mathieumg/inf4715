#pragma once                                                                                         
                                                                                                     
                                                                                                     
// DO NOT DECLARE MORE CODE REGIONS. USE THE ONES ALREADY IN THE FILE                                
                                                                                                     
#include "VCNLua\LuaTrigger.h"                                                                    
#include "SoundEmitterComponent.h"                                               
//**CodeRegion
//**EndCodeRegion
                                                                                                     
class VCNSoundEmitterStateComponent :   public VCNSoundEmitterComponent                        
{                                                                                                    
public:                                                                                              
    VCNSoundEmitterStateComponent();                                                                          
                                                                                                     
    virtual void Initialise(const Parameters& params) override;                                      
    virtual void Update(VCNFloat dt) override;                                                       
                                                                                                     
    virtual bool SetAttribute( const VCNString& attributeName, const VCNString& attributeValue );    
                                                                                                     
    virtual VCNIComponent::Ptr Copy() const;                                                         
                                                                                                     
    inline const VCNString& GetStateName() const { return mStateName; } 
    inline void SetStateName(const VCNString& val) { mStateName = val; } 
                                                                                               
    inline const VCNString& GetStateValue() const { return mStateValue; } 
    inline void SetStateValue(const VCNString& val) { mStateValue = val; } 
                                                                                               
protected:                                                                                         
    VCNString mStateName;                                               
    VCNString mStateValue;                                               
                                                                                                   
//**CodeRegion
public:
	virtual void Play();
//**EndCodeRegion
};                                                                                                 
                                                                                                   
template class VCNComponentBase<VCNSoundEmitterStateComponent>;                                             
