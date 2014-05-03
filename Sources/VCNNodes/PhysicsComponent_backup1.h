#pragma once                                                                                         
                                                                                                     
                                                                                                     
// DO NOT DECLARE MORE CODE REGIONS. USE THE ONES ALREADY IN THE FILE                                
                                                                                                     
#include "VCNLua\LuaTrigger.h"                                                                    
#include "ComponentBase.h"                                                                     
//**CodeRegion
#include "VCNPhysic\PhysicActor.h"
//**EndCodeRegion
                                                                                                     
class VCNPhysicsComponent :   public VCNComponentBase<VCNPhysicsComponent>,                    
                            public VCNIUpdatable                                                 
{                                                                                                    
public:                                                                                              
    VCNPhysicsComponent();                                                                          
                                                                                                     
    virtual void Initialise(const Parameters& params) override;                                      
    virtual void Update(VCNFloat dt) override;                                                       
                                                                                                     
    virtual bool SetAttribute( const VCNString& attributeName, const VCNString& attributeValue );    
                                                                                                     
    virtual VCNIComponent::Ptr Copy() const;                                                         
                                                                                                     
    inline const bool& GetActive() const { return mActive; } 
    inline void SetActive(const bool& val) { mActive = val; } 
                                                                                               
    inline const bool& GetIsDynamic() const { return mIsDynamic; } 
    inline void SetIsDynamic(const bool& val) { mIsDynamic = val; } 
                                                                                               
    inline const bool& GetHasGravity() const { return mHasGravity; } 
    inline void SetHasGravity(const bool& val) { mHasGravity = val; } 
                                                                                               
    inline const bool& GetIsKinematic() const { return mIsKinematic; } 
    inline void SetIsKinematic(const bool& val) { mIsKinematic = val; } 
                                                                                               
    inline const bool& GetForceHollow() const { return mForceHollow; } 
                                                                                               
protected:                                                                                         
    bool mActive;                                               
    bool mIsDynamic;                                               
    bool mHasGravity;                                               
    bool mIsKinematic;                                               
    bool mForceHollow;                                               
                                                                                                   
//**CodeRegion
	VCNPhysicActor* mPhysicsActor;

public:

	void DestroyPhysicsActor();
public:
	inline VCNPhysicActor* GetPhysicsActor() { return mPhysicsActor; }
//**EndCodeRegion
};                                                                                                 
                                                                                                   
template class VCNComponentBase<VCNPhysicsComponent>;                                             
