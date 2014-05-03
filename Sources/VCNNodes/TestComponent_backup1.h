#pragma once                                                                                         
                                                                                                     
                                                                                                     
// DO NOT DECLARE MORE CODE REGIONS. USE THE ONES ALREADY IN THE FILE                                
                                                                                                     
#include "VCNLua\LuaTrigger.h"                                                                    
#include "ComponentBase.h"                                                                     
//**CodeRegion
//**EndCodeRegion
                                                                                                     
class VCNTestComponent :   public VCNComponentBase<VCNTestComponent>,                    
                            public VCNIUpdatable                                                 
{                                                                                                    
public:                                                                                              
    VCNTestComponent();                                                                          
                                                                                                     
    virtual void Initialise(const Parameters& params) override;                                      
    virtual void Update(VCNFloat dt) override;                                                       
                                                                                                     
    virtual bool SetAttribute( const VCNString& attributeName, const VCNString& attributeValue );    
                                                                                                     
    virtual VCNIComponent::Ptr Copy() const;                                                         
                                                                                                     
    inline void SetParam1(const int& val) { mParam1 = val; } 
                                                                                               
                                                                                               
    inline const Vector2& GetAlloVect2() const { return mAlloVect2; } 
    inline void SetAlloVect2(const Vector2& val) { mAlloVect2 = val; } 
                                                                                               
    inline const VCNString& GetPageContent() const { return mPageContent; } 
                                                                                               
protected:                                                                                         
    int mParam1;                                               
    float mParam2;                                               
    Vector2 mAlloVect2;                                               
    VCNString mPageContent;                                               
                                                                                                   
//**CodeRegion
//**EndCodeRegion
};                                                                                                 
                                                                                                   
template class VCNComponentBase<VCNTestComponent>;                                             
