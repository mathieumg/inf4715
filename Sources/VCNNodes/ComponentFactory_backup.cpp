#include "Precompiled.h"                                                                                           
#include "ComponentFactory.h"                                                                                      
#include <algorithm>                                                                                                 
                                                                                                                     
#include "SoundEmitterComponent.h"                                                                        
#include "SoundEmitterEventComponent.h"                                                                        
#include "SoundEmitterStateComponent.h"                                                                        
#include "SoundEmitterSwitchComponent.h"                                                                        
#include "PhysicsTriggerComponent.h"                                                                        
#include "TestLua.h"                                                                        
#include "AutoRotateComponent.h"                                                                        
#include "TestBert.h"                                                                        
#include "PhysicsComponent.h"                                                                        
#include "CircularWallRotatingComponent.h"                                                                        
#include "NodePropertiesComponent.h"                                                                        
#include "PuzzleCubeComponent.h"                                                                        
#include "AiRobotComponent.h"                                                                        
#include "CharacterStartingPointComponent.h"                                                                        
#include "UsableComponent.h"                                                                        
#include "SlidingBridgeComponent.h"                                                                        
#include "DoorComponent.h"                                                                        
#include "SlidingDoorComponent.h"                                                                        
#include "RotatingDoorComponent.h"                                                                        
#include "RemoteUsableComponent.h"                                                                        
#include "MazeSwitchComponent.h"                                                                        
#include "TestComponent.h"                                                                        
#include "EmptyComponent.h"                                                                        
                                                                                                                     
VCNIComponent* ComponentFactory::CreateNewComponent( const VCNString& componentName )                                
{                                                                                                                    
    VCNString lowerComponentName = componentName;                                                                    
    std::transform(lowerComponentName.begin(), lowerComponentName.end(), lowerComponentName.begin(), ::tolower);     
                                                                                                                     
    if (lowerComponentName == L"soundemittercomponent")                                              
    {                                                                                                            
        return new VCNSoundEmitterComponent();                                                              
    }                                                                                                            
    if (lowerComponentName == L"soundemittereventcomponent")                                              
    {                                                                                                            
        return new VCNSoundEmitterEventComponent();                                                              
    }                                                                                                            
    if (lowerComponentName == L"soundemitterstatecomponent")                                              
    {                                                                                                            
        return new VCNSoundEmitterStateComponent();                                                              
    }                                                                                                            
    if (lowerComponentName == L"soundemitterswitchcomponent")                                              
    {                                                                                                            
        return new VCNSoundEmitterSwitchComponent();                                                              
    }                                                                                                            
    if (lowerComponentName == L"physicstriggercomponent")                                              
    {                                                                                                            
        return new VCNPhysicsTriggerComponent();                                                              
    }                                                                                                            
    if (lowerComponentName == L"testlua")                                              
    {                                                                                                            
        return new VCNTestLua();                                                              
    }                                                                                                            
    if (lowerComponentName == L"autorotatecomponent")                                              
    {                                                                                                            
        return new VCNAutoRotateComponent();                                                              
    }                                                                                                            
    if (lowerComponentName == L"testbert")                                              
    {                                                                                                            
        return new VCNTestBert();                                                              
    }                                                                                                            
    if (lowerComponentName == L"physicscomponent")                                              
    {                                                                                                            
        return new VCNPhysicsComponent();                                                              
    }                                                                                                            
    if (lowerComponentName == L"circularwallrotatingcomponent")                                              
    {                                                                                                            
        return new VCNCircularWallRotatingComponent();                                                              
    }                                                                                                            
    if (lowerComponentName == L"nodepropertiescomponent")                                              
    {                                                                                                            
        return new VCNNodePropertiesComponent();                                                              
    }                                                                                                            
    if (lowerComponentName == L"puzzlecubecomponent")                                              
    {                                                                                                            
        return new VCNPuzzleCubeComponent();                                                              
    }                                                                                                            
    if (lowerComponentName == L"airobotcomponent")                                              
    {                                                                                                            
        return new VCNAiRobotComponent();                                                              
    }                                                                                                            
    if (lowerComponentName == L"characterstartingpointcomponent")                                              
    {                                                                                                            
        return new VCNCharacterStartingPointComponent();                                                              
    }                                                                                                            
    if (lowerComponentName == L"usablecomponent")                                              
    {                                                                                                            
        return new VCNUsableComponent();                                                              
    }                                                                                                            
    if (lowerComponentName == L"slidingbridgecomponent")                                              
    {                                                                                                            
        return new VCNSlidingBridgeComponent();                                                              
    }                                                                                                            
    if (lowerComponentName == L"doorcomponent")                                              
    {                                                                                                            
        return new VCNDoorComponent();                                                              
    }                                                                                                            
    if (lowerComponentName == L"slidingdoorcomponent")                                              
    {                                                                                                            
        return new VCNSlidingDoorComponent();                                                              
    }                                                                                                            
    if (lowerComponentName == L"rotatingdoorcomponent")                                              
    {                                                                                                            
        return new VCNRotatingDoorComponent();                                                              
    }                                                                                                            
    if (lowerComponentName == L"remoteusablecomponent")                                              
    {                                                                                                            
        return new VCNRemoteUsableComponent();                                                              
    }                                                                                                            
    if (lowerComponentName == L"mazeswitchcomponent")                                              
    {                                                                                                            
        return new VCNMazeSwitchComponent();                                                              
    }                                                                                                            
    if (lowerComponentName == L"testcomponent")                                              
    {                                                                                                            
        return new VCNTestComponent();                                                              
    }                                                                                                            
    if (lowerComponentName == L"emptycomponent")                                              
    {                                                                                                            
        return new VCNEmptyComponent();                                                              
    }                                                                                                            
                                                                                                                     
    return NULL;                                                                                                     
}                                                                                                                    
                                                                                                                     
//**CodeRegion
//**EndCodeRegion
                                                                                                                     
