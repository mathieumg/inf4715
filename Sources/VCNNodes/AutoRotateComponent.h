#pragma once                                                                                         
                                                                                                     
                                                                                                     
// DO NOT DECLARE MORE CODE REGIONS. USE THE ONES ALREADY IN THE FILE                                
                                                                                                     
#include "VCNLua\LuaTrigger.h"                                                                    
#include "ComponentBase.h"                                                                     
//**CodeRegion
//**EndCodeRegion
                                                                                                     
class VCNAutoRotateComponent :   public VCNComponentBase<VCNAutoRotateComponent>,                    
                            public VCNIUpdatable                                                 
{                                                                                                    
public:                                                                                              
    VCNAutoRotateComponent();                                                                          
                                                                                                     
    virtual void Initialise(const Parameters& params) override;                                      
    virtual void Update(VCNFloat dt) override;                                                       
                                                                                                     
    virtual bool SetAttribute( const VCNString& attributeName, const VCNString& attributeValue );    
                                                                                                     
    virtual VCNIComponent::Ptr Copy() const;                                                         
                                                                                                     
    inline const double& GetSpeedFactor() const { return mSpeedFactor; } 
    inline void SetSpeedFactor(const double& val) { mSpeedFactor = val; } 
                                                                                               
    inline const Vector3& GetRotationAxis() const { return mRotationAxis; } 
    inline void SetRotationAxis(const Vector3& val) { mRotationAxis = val; } 
                                                                                               
                                                                                               
protected:                                                                                         
    double mSpeedFactor;                                               
    Vector3 mRotationAxis;                                               
    double mRotationPosition;                                               
                                                                                                   
//**CodeRegion
//**EndCodeRegion
};                                                                                                 
                                                                                                   
template class VCNComponentBase<VCNAutoRotateComponent>;                                             
