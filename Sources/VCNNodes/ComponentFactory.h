#pragma once                                                                                         
                                                                                                     
#include "ComponentBase.h"                                                                         
#include <string>                                                                                    
                                                                                                     
class ComponentFactory                                                                               
{                                                                                                    
                                                                                                     
public:                                                                                              
                                                                                                     
    // Method to build a component from a component name.                                            
    // Can return NULL if class is not found. Otherwise, you are responsible to delete the memory    
    static VCNIComponent* CreateNewComponent(const VCNString& componentName);                        
                                                                                                     
//**CodeRegion

//**EndCodeRegion
                                                                                                     
};                                                                                                   
