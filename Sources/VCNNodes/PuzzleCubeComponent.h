#pragma once                                                                                         
                                                                                                     
                                                                                                     
// DO NOT DECLARE MORE CODE REGIONS. USE THE ONES ALREADY IN THE FILE                                
                                                                                                     
#include "VCNLua\LuaTrigger.h"                                                                    
#include "ComponentBase.h"                                                                     
//**CodeRegion
enum VCNPuzzleCubeState
{
	Not_Pushing,
	Pushing
};

//**EndCodeRegion
                                                                                                     
class VCNPuzzleCubeComponent :   public VCNComponentBase<VCNPuzzleCubeComponent>,                    
                            public VCNIUpdatable                                                 
{                                                                                                    
public:                                                                                              
    VCNPuzzleCubeComponent();                                                                          
                                                                                                     
    virtual void Initialise(const Parameters& params) override;                                      
    virtual void Update(VCNFloat dt) override;                                                       
                                                                                                     
    virtual bool SetAttribute( const VCNString& attributeName, const VCNString& attributeValue );    
                                                                                                     
    virtual VCNIComponent::Ptr Copy() const;                                                         
                                                                                                     
    inline const int& GetPuzzlePiece() const { return mPuzzlePiece; } 
    inline void SetPuzzlePiece(const int& val) { mPuzzlePiece = val; } 
                                                                                               
    inline const float& GetPushForce() const { return mPushForce; } 
                                                                                               
    inline const bool& GetLockOnAxis() const { return mLockOnAxis; } 
    inline void SetLockOnAxis(const bool& val) { mLockOnAxis = val; } 
                                                                                               
protected:                                                                                         
    int mPuzzlePiece;                                               
    float mPushForce;                                               
    bool mLockOnAxis;                                               
                                                                                                   
//**CodeRegion
public:
	void OnCharacterHit(const Vector3& hit);

private:
	VCNPuzzleCubeState mPushingState;
	VCNPuzzleCubeState mLastPushingState;
	Vector3 mPushDirection;
//**EndCodeRegion
};                                                                                                 
                                                                                                   
template class VCNComponentBase<VCNPuzzleCubeComponent>;                                             
