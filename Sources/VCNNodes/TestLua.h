#pragma once                                                                                         
                                                                                                     
                                                                                                     
// DO NOT DECLARE MORE CODE REGIONS. USE THE ONES ALREADY IN THE FILE                                
                                                                                                     
#include "VCNLua\LuaTrigger.h"                                                                    
#include "ComponentBase.h"                                                                     
//**CodeRegion
//**EndCodeRegion
                                                                                                     
class VCNTestLua :   public VCNComponentBase<VCNTestLua>,                    
                            public VCNIUpdatable                                                 
{                                                                                                    
public:                                                                                              
    VCNTestLua();                                                                          
                                                                                                     
    virtual void Initialise(const Parameters& params) override;                                      
    virtual void Update(VCNFloat dt) override;                                                       
                                                                                                     
    virtual bool SetAttribute( const VCNString& attributeName, const VCNString& attributeValue );    
                                                                                                     
    virtual VCNIComponent::Ptr Copy() const;                                                         
                                                                                                     
    inline const LuaTrigger& GetTrigger1() const { return mTrigger1; } 
    inline void SetTrigger1(const LuaTrigger& val) { mTrigger1 = val; } 
                                                                                               
    inline const LuaTrigger& GetTrigger2() const { return mTrigger2; } 
    inline void SetTrigger2(const LuaTrigger& val) { mTrigger2 = val; } 
                                                                                               
protected:                                                                                         
    LuaTrigger mTrigger1;                                               
    LuaTrigger mTrigger2;                                               
                                                                                                   
//**CodeRegion
//**EndCodeRegion
};                                                                                                 
                                                                                                   
template class VCNComponentBase<VCNTestLua>;                                             
