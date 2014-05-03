#pragma once                                                                                         
                                                                                                     
                                                                                                     
// DO NOT DECLARE MORE CODE REGIONS. USE THE ONES ALREADY IN THE FILE                                
                                                                                                     
#include "VCNLua\LuaTrigger.h"                                                                    
#include "ComponentBase.h"                                                                     
//**CodeRegion
//**EndCodeRegion
                                                                                                     
class VCNUsableComponent :   public VCNComponentBase<VCNUsableComponent>,                    
                            public VCNIUpdatable                                                 
{                                                                                                    
public:                                                                                              
    VCNUsableComponent();                                                                          
                                                                                                     
    virtual void Initialise(const Parameters& params) override;                                      
    virtual void Update(VCNFloat dt) override;                                                       
                                                                                                     
    virtual bool SetAttribute( const VCNString& attributeName, const VCNString& attributeValue );    
                                                                                                     
    virtual VCNIComponent::Ptr Copy() const;                                                         
                                                                                                     
    inline const bool& GetIsSelectable() const { return mIsSelectable; } 
    inline void SetIsSelectable(const bool& val) { mIsSelectable = val; } 
                                                                                               
    inline const float& GetUseDistance() const { return mUseDistance; } 
    inline void SetUseDistance(const float& val) { mUseDistance = val; } 
                                                                                               
protected:                                                                                         
    bool mIsSelectable;                                               
    float mUseDistance;                                               
                                                                                                   
//**CodeRegion
protected:
	bool mIsBeingPointed;
public:
	virtual void UseComponent();
	inline void SetIsBeingPointed(bool val) { mIsBeingPointed = val; }



	//**EndCodeRegion
};                                                                                                 
                                                                                                   
template class VCNComponentBase<VCNUsableComponent>;                                             
