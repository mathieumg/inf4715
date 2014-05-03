#pragma once                                                                                         
                                                                                                     
                                                                                                     
// DO NOT DECLARE MORE CODE REGIONS. USE THE ONES ALREADY IN THE FILE                                
                                                                                                     
#include "VCNLua\LuaTrigger.h"                                                                    
#include "DoorComponent.h"                                               
//**CodeRegion
class VCNNode;
#include "VCNUtils\Types.h"
//**EndCodeRegion
                                                                                                     
class VCNSlidingDoorComponent :   public VCNDoorComponent                        
{                                                                                                    
public:                                                                                              
    VCNSlidingDoorComponent();                                                                          
                                                                                                     
    virtual void Initialise(const Parameters& params) override;                                      
    virtual void Update(VCNFloat dt) override;                                                       
                                                                                                     
    virtual bool SetAttribute( const VCNString& attributeName, const VCNString& attributeValue );    
                                                                                                     
    virtual VCNIComponent::Ptr Copy() const;                                                         
                                                                                                     
    inline const Vector3& GetTravelDistance() const { return mTravelDistance; } 
    inline void SetTravelDistance(const Vector3& val) { mTravelDistance = val; } 
                                                                                               
    inline const bool& GetIsLeftPart() const { return mIsLeftPart; } 
    inline void SetIsLeftPart(const bool& val) { mIsLeftPart = val; } 
                                                                                               
    inline const VCNString& GetOtherPartName() const { return mOtherPartName; } 
    inline void SetOtherPartName(const VCNString& val) { mOtherPartName = val; } 
                                                                                               
protected:                                                                                         
    Vector3 mTravelDistance;                                               
    bool mIsLeftPart;                                               
    VCNString mOtherPartName;                                               
                                                                                                   
//**CodeRegion
protected:
	void ComputeTargetPosition();

	virtual void UpdateClosing( float dt );

	virtual void UpdateOpening( float dt );

	VCNNode* GetOtherPartNode();

protected:
	Vector3 mTargetPositionLeft;
	Vector3 mTargetPositionRight;

	Vector3 mCurrentLeftPosition;
	Vector3 mCurrentRightPosition;

	Vector3 mStartingLeftPosition;
	Vector3 mStartingRightPosition;

	VCNNodeID mOtherNodeID;
//**EndCodeRegion
};                                                                                                 
                                                                                                   
template class VCNComponentBase<VCNSlidingDoorComponent>;                                             
