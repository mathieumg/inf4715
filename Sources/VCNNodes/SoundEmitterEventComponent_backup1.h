#pragma once                                                                                         
                                                                                                     
                                                                                                     
// DO NOT DECLARE MORE CODE REGIONS. USE THE ONES ALREADY IN THE FILE                                
                                                                                                     
#include "VCNLua\LuaTrigger.h"                                                                    
#include "SoundEmitterComponent.h"                                               
//**CodeRegion
//**EndCodeRegion
                                                                                                     
class VCNSoundEmitterEventComponent :   public VCNSoundEmitterComponent                        
{                                                                                                    
public:                                                                                              
    VCNSoundEmitterEventComponent();                                                                          
                                                                                                     
    virtual void Initialise(const Parameters& params) override;                                      
    virtual void Update(VCNFloat dt) override;                                                       
                                                                                                     
    virtual bool SetAttribute( const VCNString& attributeName, const VCNString& attributeValue );    
                                                                                                     
    virtual VCNIComponent::Ptr Copy() const;                                                         
                                                                                                     
    inline const VCNString& GetEventName() const { return mEventName; } 
    inline void SetEventName(const VCNString& val) { mEventName = val; } 
                                                                                               
protected:                                                                                         
    VCNString mEventName;                                               
                                                                                                   
//**CodeRegion
	VCNUInt mGameObjectID;

public:
	virtual void Play();
//**EndCodeRegion
};                                                                                                 
                                                                                                   
template class VCNComponentBase<VCNSoundEmitterEventComponent>;                                             
