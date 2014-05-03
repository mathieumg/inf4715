#pragma once                                                                                         
                                                                                                     
                                                                                                     
// DO NOT DECLARE MORE CODE REGIONS. USE THE ONES ALREADY IN THE FILE                                
                                                                                                     
#include "VCNLua\LuaTrigger.h"                                                                    
#include "ComponentBase.h"                                                                     
//**CodeRegion
//**EndCodeRegion
                                                                                                     
class VCNCircularWallRotatingComponent :   public VCNComponentBase<VCNCircularWallRotatingComponent>,                    
                            public VCNIUpdatable                                                 
{                                                                                                    
public:                                                                                              
    VCNCircularWallRotatingComponent();                                                                          
                                                                                                     
    virtual void Initialise(const Parameters& params) override;                                      
    virtual void Update(VCNFloat dt) override;                                                       
                                                                                                     
    virtual bool SetAttribute( const VCNString& attributeName, const VCNString& attributeValue );    
                                                                                                     
    virtual VCNIComponent::Ptr Copy() const;                                                         
                                                                                                     
    inline const double& GetSpeed() const { return mSpeed; } 
    inline void SetSpeed(const double& val) { mSpeed = val; } 
                                                                                               
protected:                                                                                         
    double mSpeed;                                               
                                                                                                   
//**CodeRegion
//**EndCodeRegion
};                                                                                                 
                                                                                                   
template class VCNComponentBase<VCNCircularWallRotatingComponent>;                                             
