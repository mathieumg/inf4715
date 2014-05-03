#pragma once                                                                                         
                                                                                                     
                                                                                                     
// DO NOT DECLARE MORE CODE REGIONS. USE THE ONES ALREADY IN THE FILE                                
                                                                                                     
#include "VCNLua\LuaTrigger.h"                                                                    
#include "UsableComponent.h"                                               
//**CodeRegion
class VCNPhysicsComponent;

enum DoorMovingState
{
	DOOR_CLOSED = 0,
	DOOR_OPENED = 1,
	DOOR_OPENING = 2,
	DOOR_CLOSING = 3
};

//**EndCodeRegion
                                                                                                     
class VCNDoorComponent :   public VCNUsableComponent                        
{                                                                                                    
public:                                                                                              
    VCNDoorComponent();                                                                          
                                                                                                     
    virtual void Initialise(const Parameters& params) override;                                      
    virtual void Update(VCNFloat dt) override;                                                       
                                                                                                     
    virtual bool SetAttribute( const VCNString& attributeName, const VCNString& attributeValue );    
                                                                                                     
    virtual VCNIComponent::Ptr Copy() const;                                                         
                                                                                                     
    inline const float& GetOpeningSpeed() const { return mOpeningSpeed; } 
    inline void SetOpeningSpeed(const float& val) { mOpeningSpeed = val; } 
                                                                                               
    inline const float& GetClosingSpeed() const { return mClosingSpeed; } 
    inline void SetClosingSpeed(const float& val) { mClosingSpeed = val; } 
                                                                                               
                                                                                               
    inline const LuaTrigger& GetOpenValidationScript() const { return mOpenValidationScript; } 
    inline void SetOpenValidationScript(const LuaTrigger& val) { mOpenValidationScript = val; } 
                                                                                               
    inline const LuaTrigger& GetOnOpenTrigger() const { return mOnOpenTrigger; } 
    inline void SetOnOpenTrigger(const LuaTrigger& val) { mOnOpenTrigger = val; } 
                                                                                               
    inline const LuaTrigger& GetOnCloseTrigger() const { return mOnCloseTrigger; } 
    inline void SetOnCloseTrigger(const LuaTrigger& val) { mOnCloseTrigger = val; } 
                                                                                               
protected:                                                                                         
    float mOpeningSpeed;                                               
    float mClosingSpeed;                                               
    int mStartingMovingState;                                               
    LuaTrigger mOpenValidationScript;                                               
    LuaTrigger mOnOpenTrigger;                                               
    LuaTrigger mOnCloseTrigger;                                               
                                                                                                   
//**CodeRegion
protected:
	void Open();
	void Close();

	

	VCNPhysicsComponent* GetPhysicsComponent();

	virtual void UpdateClosing(float dt);
	virtual void UpdateOpening(float dt);

	void SetupPhysicsComponentOnNode(VCNNode* node);

public:
	virtual void UseComponent();
	inline void SetIsBeingPointed(bool val) { mIsBeingPointed = val; }

protected:
	DoorMovingState mMovingState;
	bool mIsBeingPointed;
	
	//**EndCodeRegion
};                                                                                                 
                                                                                                   
template class VCNComponentBase<VCNDoorComponent>;                                             
