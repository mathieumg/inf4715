#pragma once                                                                                         
                                                                                                     
                                                                                                     
// DO NOT DECLARE MORE CODE REGIONS. USE THE ONES ALREADY IN THE FILE                                
                                                                                                     
#include "VCNLua\LuaTrigger.h"                                                                    
#include "DoorComponent.h"                                               
//**CodeRegion
//**EndCodeRegion
                                                                                                     
class VCNRotatingDoorComponent :   public VCNDoorComponent                        
{                                                                                                    
public:                                                                                              
    VCNRotatingDoorComponent();                                                                          
                                                                                                     
    virtual void Initialise(const Parameters& params) override;                                      
    virtual void Update(VCNFloat dt) override;                                                       
                                                                                                     
    virtual bool SetAttribute( const VCNString& attributeName, const VCNString& attributeValue );    
                                                                                                     
    virtual VCNIComponent::Ptr Copy() const;                                                         
                                                                                                     
    inline const bool& GetRotateClockwise() const { return mRotateClockwise; } 
                                                                                               
    inline const float& GetMaxRotationAngle() const { return mMaxRotationAngle; } 
                                                                                               
protected:                                                                                         
    bool mRotateClockwise;                                               
    float mMaxRotationAngle;                                               
                                                                                                   
//**CodeRegion
protected:
	void ApplyBaseRotation();

	virtual void UpdateClosing( float dt );

	virtual void UpdateOpening( float dt );



protected:
	float mRotationAngle;
	float mOpenedRotationAngle;
	float mClosedRotationAngle;
//**EndCodeRegion
};                                                                                                 
                                                                                                   
template class VCNComponentBase<VCNRotatingDoorComponent>;                                             
