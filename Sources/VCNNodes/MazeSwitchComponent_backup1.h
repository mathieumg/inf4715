#pragma once                                                                                         
                                                                                                     
                                                                                                     
// DO NOT DECLARE MORE CODE REGIONS. USE THE ONES ALREADY IN THE FILE                                
                                                                                                     
#include "VCNLua\LuaTrigger.h"                                                                    
#include "UsableComponent.h"                                               
//**CodeRegion
#include "NodeCore.h"
//**EndCodeRegion
                                                                                                     
class VCNMazeSwitchComponent :   public VCNUsableComponent                        
{                                                                                                    
public:                                                                                              
    VCNMazeSwitchComponent();                                                                          
                                                                                                     
    virtual void Initialise(const Parameters& params) override;                                      
    virtual void Update(VCNFloat dt) override;                                                       
                                                                                                     
    virtual bool SetAttribute( const VCNString& attributeName, const VCNString& attributeValue );    
                                                                                                     
    virtual VCNIComponent::Ptr Copy() const;                                                         
                                                                                                     
    inline const VCNString& GetRemoteNodeName() const { return mRemoteNodeName; } 
                                                                                               
    inline const LuaTrigger& GetToggleSwitch() const { return mToggleSwitch; } 
    inline void SetToggleSwitch(const LuaTrigger& val) { mToggleSwitch = val; } 
                                                                                               
protected:                                                                                         
    VCNString mRemoteNodeName;                                               
    LuaTrigger mToggleSwitch;                                               
                                                                                                   
//**CodeRegion
public:
	virtual void UseComponent();
protected:
	VCNNodeID mRootNodeID;
//**EndCodeRegion
};                                                                                                 
                                                                                                   
template class VCNComponentBase<VCNMazeSwitchComponent>;                                             
