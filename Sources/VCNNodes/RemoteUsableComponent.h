#pragma once                                                                                         
                                                                                                     
                                                                                                     
// DO NOT DECLARE MORE CODE REGIONS. USE THE ONES ALREADY IN THE FILE                                
                                                                                                     
#include "VCNLua\LuaTrigger.h"                                                                    
#include "UsableComponent.h"                                               
//**CodeRegion
//**EndCodeRegion
                                                                                                     
class VCNRemoteUsableComponent :   public VCNUsableComponent                        
{                                                                                                    
public:                                                                                              
    VCNRemoteUsableComponent();                                                                          
                                                                                                     
    virtual void Initialise(const Parameters& params) override;                                      
    virtual void Update(VCNFloat dt) override;                                                       
                                                                                                     
    virtual bool SetAttribute( const VCNString& attributeName, const VCNString& attributeValue );    
                                                                                                     
    virtual VCNIComponent::Ptr Copy() const;                                                         
                                                                                                     
    inline const VCNString& GetRemoteNodeName() const { return mRemoteNodeName; } 
                                                                                               
protected:                                                                                         
    VCNString mRemoteNodeName;                                               
                                                                                                   
//**CodeRegion
protected:
	virtual void UseComponent();

protected:
	VCNNodeID mRootNodeID;
//**EndCodeRegion
};                                                                                                 
                                                                                                   
template class VCNComponentBase<VCNRemoteUsableComponent>;                                             
