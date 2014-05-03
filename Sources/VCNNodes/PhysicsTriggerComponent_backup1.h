#pragma once                                                                                         
                                                                                                     
                                                                                                     
// DO NOT DECLARE MORE CODE REGIONS. USE THE ONES ALREADY IN THE FILE                                
                                                                                                     
#include "VCNLua\LuaTrigger.h"                                                                    
#include "ComponentBase.h"                                                                     
//**CodeRegion
#include "TriggerNode.h"
//**EndCodeRegion
                                                                                                     
class VCNPhysicsTriggerComponent :   public VCNComponentBase<VCNPhysicsTriggerComponent>,                    
                            public VCNIUpdatable                                                 
{                                                                                                    
public:                                                                                              
    VCNPhysicsTriggerComponent();                                                                          
                                                                                                     
    virtual void Initialise(const Parameters& params) override;                                      
    virtual void Update(VCNFloat dt) override;                                                       
                                                                                                     
    virtual bool SetAttribute( const VCNString& attributeName, const VCNString& attributeValue );    
                                                                                                     
    virtual VCNIComponent::Ptr Copy() const;                                                         
                                                                                                     
    inline const VCNString& GetNameOfObjectToWatch() const { return mNameOfObjectToWatch; } 
    inline void SetNameOfObjectToWatch(const VCNString& val) { mNameOfObjectToWatch = val; } 
                                                                                               
    inline const LuaTrigger& GetOnEnter() const { return mOnEnter; } 
    inline void SetOnEnter(const LuaTrigger& val) { mOnEnter = val; } 
                                                                                               
    inline const LuaTrigger& GetOnExit() const { return mOnExit; } 
    inline void SetOnExit(const LuaTrigger& val) { mOnExit = val; } 
                                                                                               
protected:                                                                                         
    VCNString mNameOfObjectToWatch;                                               
    LuaTrigger mOnEnter;                                               
    LuaTrigger mOnExit;                                               
                                                                                                   
//**CodeRegion

	VCNNodeID mObjectToWatchID;
	TriggerNode* mTriggerNode;
	Vector3 mPreviousPosition;
//**EndCodeRegion
};                                                                                                 
                                                                                                   
template class VCNComponentBase<VCNPhysicsTriggerComponent>;                                             
