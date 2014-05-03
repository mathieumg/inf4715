#pragma once                                                                                         
                                                                                                     
                                                                                                     
// DO NOT DECLARE MORE CODE REGIONS. USE THE ONES ALREADY IN THE FILE                                
                                                                                                     
#include "VCNLua\LuaTrigger.h"                                                                    
#include "ComponentBase.h"                                                                     
//**CodeRegion
//**EndCodeRegion
                                                                                                     
class VCNCharacterStartingPointComponent :   public VCNComponentBase<VCNCharacterStartingPointComponent>,                    
                            public VCNIUpdatable                                                 
{                                                                                                    
public:                                                                                              
    VCNCharacterStartingPointComponent();                                                                          
                                                                                                     
    virtual void Initialise(const Parameters& params) override;                                      
    virtual void Update(VCNFloat dt) override;                                                       
                                                                                                     
    virtual bool SetAttribute( const VCNString& attributeName, const VCNString& attributeValue );    
                                                                                                     
    virtual VCNIComponent::Ptr Copy() const;                                                         
                                                                                                     
    inline const bool& GetIsEnabled() const { return mIsEnabled; } 
    inline void SetIsEnabled(const bool& val) { mIsEnabled = val; } 
                                                                                               
protected:                                                                                         
    bool mIsEnabled;                                               
                                                                                                   
//**CodeRegion
//**EndCodeRegion
};                                                                                                 
                                                                                                   
template class VCNComponentBase<VCNCharacterStartingPointComponent>;                                             
