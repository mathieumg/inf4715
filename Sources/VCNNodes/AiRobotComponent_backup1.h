#pragma once                                                                                         
                                                                                                     
                                                                                                     
// DO NOT DECLARE MORE CODE REGIONS. USE THE ONES ALREADY IN THE FILE                                
                                                                                                     
#include "VCNLua\LuaTrigger.h"                                                                    
#include "ComponentBase.h"                                                                     
//**CodeRegion
#include "VCNNodes/Node.h"
#include "VCNNodes/NodeCore.h"
#include "VCNPhysX/PhysXRaycast.h"
#include <vector>

enum Directions{UP,DOWN,LEFT,RIGHT,INVALID};
struct dir
{
	bool UP;
	bool DOWN;
	bool LEFT;
	bool RIGHT;
};

//**EndCodeRegion
                                                                                                     
class VCNAiRobotComponent :   public VCNComponentBase<VCNAiRobotComponent>,                    
                            public VCNIUpdatable                                                 
{                                                                                                    
public:                                                                                              
    VCNAiRobotComponent();                                                                          
                                                                                                     
    virtual void Initialise(const Parameters& params) override;                                      
    virtual void Update(VCNFloat dt) override;                                                       
                                                                                                     
    virtual bool SetAttribute( const VCNString& attributeName, const VCNString& attributeValue );    
                                                                                                     
    virtual VCNIComponent::Ptr Copy() const;                                                         
                                                                                                     
                                                                                               
                                                                                               
                                                                                               
                                                                                               
                                                                                               
                                                                                               
                                                                                               
    inline const LuaTrigger& GetTriggerAwarenessChange() const { return mTriggerAwarenessChange; } 
    inline void SetTriggerAwarenessChange(const LuaTrigger& val) { mTriggerAwarenessChange = val; } 
                                                                                               
protected:                                                                                         
    int mGridPosX;                                               
    int mGridPosY;                                               
    float mMoveSpeed;                                               
    float mFieldViewAngle;                                               
    float mDeltaAngle;                                               
    float mDistMargin;                                               
    VCNString mCharacterNodeName;                                               
    LuaTrigger mTriggerAwarenessChange;                                               
                                                                                                   
//**CodeRegion
private:
	std::vector<std::vector<dir>>	mMaze;
	Vector2i						mGridTarget;
	Vector2f						mWorldTarget;
	Vector3							mMazeCenter;
	Directions						mLastDirection;
	bool							mCharacterNodeInitialized;
	VCNNode*						mCharacter;
	PhysXRaycast*					mRayCast;
	VCNFloat						mTotalTime;

public:
	// Determine where the robot is going
	void FindNewTarget();
	// Convert grid position to world position
	Vector2f GridToWorld(Vector2i gridPos);
	// Convert world to grid
	//Vector2i WorldToGrid(Vector2f worldPos);
	// Check if position is in radius of another one
	bool isCloseEnough();
	// Choose new direction
	Directions ChooseNewDirection();
	// Check field of view for enemy
	bool CheckForEnemy();
	// Move to target
	void MoveToTarget(float elapsedTime);
	// Inverse de last direction since we already moved
	Directions GetLastDirection(Directions lastDirection);

	static VCNFloat					mAwarenessLevel;


//**EndCodeRegion
};                                                                                                 
                                                                                                   
template class VCNComponentBase<VCNAiRobotComponent>;                                             
