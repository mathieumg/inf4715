#include "Precompiled.h"                                     
#include "TestLua.h"                               
                                                               
// DO NOT DECLARE MORE CODE REGIONS. USE THE ONES ALREADY IN THE FILE 
                                                               
//**CodeRegion
//**EndCodeRegion
                                                               
VCNTestLua::VCNTestLua()                    
{                                                              
    // Default values                                          
    mTrigger1 = LuaTrigger(L"", L"");                   
    mTrigger2 = LuaTrigger(L"", L"");                   
//**CodeRegion
//**EndCodeRegion
}                                                              
                                                               
void VCNTestLua::Initialise(const Parameters& params)
{                                                              
//**CodeRegion
//**EndCodeRegion
}                                                              
                                                               
void VCNTestLua::Update(VCNFloat dt)                 
{                                                              
//**CodeRegion
//**EndCodeRegion
}                                                              
                                                               
bool VCNTestLua::SetAttribute( const VCNString& attributeName, const VCNString& attributeValue )
{                                                              
    if (attributeName == L"Trigger1")                       
    {                                                                      
        std::stringstream ss;                                              
        std::string str( attributeValue.begin(), attributeValue.end() );   
        ss << str;                                                         
        ss >> mTrigger1;                                      
        return true;                                                       
    }                                                                      
    else if (attributeName == L"Trigger2")                       
    {                                                                      
        std::stringstream ss;                                              
        std::string str( attributeValue.begin(), attributeValue.end() );   
        ss << str;                                                         
        ss >> mTrigger2;                                      
        return true;                                                       
    }                                                                      
    return false;                                              
}                                                              
                                                               
VCNIComponent::Ptr VCNTestLua::Copy() const                          
{                                                                              
    VCNTestLua* otherComponent = new VCNTestLua();         
                                                                               
    otherComponent->mTrigger1 = LuaTrigger(mTrigger1);
    otherComponent->mTrigger2 = LuaTrigger(mTrigger2);
                                                                               
    return std::shared_ptr<VCNTestLua>(otherComponent);              
}                                                                              
                                                               
                                                               
//**CodeRegion
//**EndCodeRegion
