#pragma once                                                                                         
                                                                                                     
                                                                                                     
// DO NOT DECLARE MORE CODE REGIONS. USE THE ONES ALREADY IN THE FILE                                
                                                                                                     
#include "VCNLua\LuaTrigger.h"                                                                    
#include "UsableComponent.h"                                               
//**CodeRegion
class VCNPhysicsComponent;
enum BridgeMovingState
{
	BRIDGE_CLOSED = 0,
	BRIDGE_OPENING = 1,
	BRIDGE_OPENED = 2,
};
//**EndCodeRegion
                                                                                                     
class VCNSlidingBridgeComponent :   public VCNUsableComponent                        
{                                                                                                    
public:                                                                                              
    VCNSlidingBridgeComponent();                                                                          
                                                                                                     
    virtual void Initialise(const Parameters& params) override;                                      
    virtual void Update(VCNFloat dt) override;                                                       
                                                                                                     
    virtual bool SetAttribute( const VCNString& attributeName, const VCNString& attributeValue );    
                                                                                                     
    virtual VCNIComponent::Ptr Copy() const;                                                         
                                                                                                     
    inline const Vector3& GetTravelDistance() const { return mTravelDistance; } 
    inline void SetTravelDistance(const Vector3& val) { mTravelDistance = val; } 
                                                                                               
    inline const float& GetslidingSpeed() const { return mslidingSpeed; } 
    inline void SetslidingSpeed(const float& val) { mslidingSpeed = val; } 
                                                                                               
                                                                                               
    inline const LuaTrigger& GetslideBridge() const { return mslideBridge; } 
    inline void SetslideBridge(const LuaTrigger& val) { mslideBridge = val; } 
                                                                                               
protected:                                                                                         
    Vector3 mTravelDistance;                                               
    float mslidingSpeed;                                               
    int mStartingMovingState;                                               
    LuaTrigger mslideBridge;                                               
                                                                                                   
//**CodeRegion
protected:

	VCNPhysicsComponent* GetPhysicsComponent();

	virtual void UpdateOpening(float dt);

	void SetupPhysicsComponentOnNode(VCNNode* node);

	virtual void UseComponent();


	Vector3 mCurrentPosition;
	Vector3 mTargetPosition;

	BridgeMovingState mMovingState;
	
//**EndCodeRegion
};                                                                                                 
                                                                                                   
template class VCNComponentBase<VCNSlidingBridgeComponent>;                                             
