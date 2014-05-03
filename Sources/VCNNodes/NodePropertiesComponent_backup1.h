#pragma once                                                                                         
                                                                                                     
                                                                                                     
// DO NOT DECLARE MORE CODE REGIONS. USE THE ONES ALREADY IN THE FILE                                
                                                                                                     
#include "VCNLua\LuaTrigger.h"                                                                    
#include "ComponentBase.h"                                                                     
//**CodeRegion
//**EndCodeRegion
                                                                                                     
class VCNNodePropertiesComponent :   public VCNComponentBase<VCNNodePropertiesComponent>,                    
                            public VCNIUpdatable                                                 
{                                                                                                    
public:                                                                                              
    VCNNodePropertiesComponent();                                                                          
                                                                                                     
    virtual void Initialise(const Parameters& params) override;                                      
    virtual void Update(VCNFloat dt) override;                                                       
                                                                                                     
    virtual bool SetAttribute( const VCNString& attributeName, const VCNString& attributeValue );    
                                                                                                     
    virtual VCNIComponent::Ptr Copy() const;                                                         
                                                                                                     
    inline const bool& GetIsNodeSelectable() const { return mIsNodeSelectable; } 
    inline void SetIsNodeSelectable(const bool& val) { mIsNodeSelectable = val; } 
                                                                                               
    inline const bool& GetIsNodeActive() const { return mIsNodeActive; } 
    inline void SetIsNodeActive(const bool& val) { mIsNodeActive = val; } 
                                                                                               
    inline const VCNString& GetNodeTag() const { return mNodeTag; } 
    inline void SetNodeTag(const VCNString& val) { mNodeTag = val; } 
                                                                                               
    inline const VCNString& GetReadableName() const { return mReadableName; } 
                                                                                               
protected:                                                                                         
    bool mIsNodeSelectable;                                               
    bool mIsNodeActive;                                               
    VCNString mNodeTag;                                               
    VCNString mReadableName;                                               
                                                                                                   
//**CodeRegion
//**EndCodeRegion
};                                                                                                 
                                                                                                   
template class VCNComponentBase<VCNNodePropertiesComponent>;                                             
