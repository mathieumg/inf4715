#include "Precompiled.h"                                     
#include "AiRobotComponent.h"                               
                                                               
// DO NOT DECLARE MORE CODE REGIONS. USE THE ONES ALREADY IN THE FILE 
                                                               
//**CodeRegion
#include "VCNPhysic/PhysicCore.h"
#include "characterkinematic/PxControllerObstacles.h"
#include "VCNPhysX/PhysxController.h"
#include "characterkinematic/PxControllerManager.h"
#include "PhysicsComponent.h"

VCNFloat VCNAiRobotComponent::mAwarenessLevel = 0.0f;

//**EndCodeRegion
                                                               
VCNAiRobotComponent::VCNAiRobotComponent()                    
{                                                              
    // Default values                                          
    mGridPosX = 0;                   
    mGridPosY = 0;                   
    mMoveSpeed = 3;                   
    mFieldViewAngle = 10;                   
    mDeltaAngle = 22.5;                   
    mDistMargin = 5;                   
    mCharacterNodeName = L"w";                   
    mTriggerAwarenessChange = LuaTrigger(L"", L"");                   
//**CodeRegion
	mDistMargin = 0.1f;
	mCharacterNodeInitialized = false;
//**EndCodeRegion
}                                                              
                                                               
void VCNAiRobotComponent::Initialise(const Parameters& params)
{                                                              
//**CodeRegion

	// Initialization of the random seed
	srand(time(NULL));

	// TODO: Find another place to put this
	// 1 direction possible
	dir u = { true, false, false, false };
	dir d = { false, true, false, false };
	dir l = { false, false, true, false };
	dir r = { false, false, false, true };
	// 2 directions possible
	dir ul = { true, false, true, false };
	dir ur = { true, false, false, true };
	dir dr = { false, true, false, true };
	dir dl = { false, true, true, false };
	dir ud = { true, true, false, false };
	dir lr = { false, false, true, true };
	// 3 directions possible
	dir ulr = { true, false, true, true };
	dir udl = { true, true, true, false };
	dir udr = { true, true, false, true };
	dir dlr = { false, true, true, true };
	// 4 dir
	dir udlr = { true, true, true, true };

	//           0   1     2    3   4    5    6    7   8   9   10   11   12  13   14   15
	dir l1[] = { ur, lr,  ulr,  l,  r, ulr, ulr,   l,  r, ul,   r, ulr, ulr,  l,  ur,  ul };
	dir l2[] = { dl, ur, udlr, lr, ul,   d,  dr, ulr, ul, dr,  ul,   d, udr, ul, udr, dlr };
	dir l3[] = { lr, dl,   dr,  l, dr,  lr,  lr,  dl, dr, lr, dlr,  lr,  dl, dr, dlr,  lr };

	//std::vector<std::vector<dir>> maze(3); // (3, std::vector<int>(10));
	mMaze = std::vector<std::vector<dir>>(3);
	std::vector<dir> innerCircle(l1, l1 + sizeof(l1) / sizeof(l1[0]));
	std::vector<dir> midCircle(l2, l2 + sizeof(l2) / sizeof(l2[0]));
	std::vector<dir> extCircle(l3, l3 + sizeof(l3) / sizeof(l3[0]));
	mMaze[0] = innerCircle;
	mMaze[1] = midCircle;
	mMaze[2] = extCircle;

	mLastDirection = INVALID;
	mGridTarget = Vector2i(mGridPosX, mGridPosY);

	// TODO: Change this when we can refer nodes in the editor
	const VCNNodeID mMazeNodeId = VCNNodeCore::GetInstance()->GetNodeByName(VCNTXT("Cylinder002"));
	VCNNode* mMazeNode = VCNNodeCore::GetInstance()->GetNode(mMazeNodeId);
	GetOwner()->AddProperty(VCNTXT("Dynamic"));
	//GetOwner()->SetActive(true);
	//GetOwner()->SetIsDynamic(true);
	//GetOwner()->SetIsKinematic(true);
	//GetOwner()->SetHasGravity(false);
	mMazeCenter = mMazeNode->GetWorldTranslation();
	mWorldTarget = GridToWorld(mGridTarget);
	GetOwner()->SetTranslation(Vector3(mWorldTarget.x,5.0f,mWorldTarget.y));

	mTotalTime = 0.0f;

	mTriggerAwarenessChange.SetTriggerName("robotai");
	mTriggerAwarenessChange.SetTriggerType("awarenessChange");
	
//**EndCodeRegion
}                                                              
                                                               
void VCNAiRobotComponent::Update(VCNFloat dt)                 
{                                                              
//**CodeRegion

	mDistMargin = 0.1f;

	
	VCNAiRobotComponent::mAwarenessLevel -= (dt / 25);
	if (VCNAiRobotComponent::mAwarenessLevel < 0.0f)
	{
		VCNAiRobotComponent::mAwarenessLevel = 0.0f;
	}

	// Step 1 : check for enemyyo

	// TODO : Finish this part and decide what to do
	if (CheckForEnemy())
	{
		mTotalTime += dt;
		if (mTotalTime >= 1.0f)
		{
			VCNAiRobotComponent::mAwarenessLevel += 0.25;

			mTotalTime -= 1.0f;
		}
	}
	else
	{
		mTotalTime = 0.0f;
	}

	// update awareness level of in the UI
	// CACHE THIS TO OPTIMIZE PERFORMANCE AND DONT CALL TRIGGER EACH TICK
	mTriggerAwarenessChange.Trigger((int) (VCNAiRobotComponent::mAwarenessLevel * 100));
	if (VCNAiRobotComponent::mAwarenessLevel >= 1.0f)
	{
		VCNAiRobotComponent::mAwarenessLevel = 0.0f;
	}

	// Step 2 : check if we reached target

	if (isCloseEnough())
	{
		// We reached the target, update gridpos
		mGridPosX = mGridTarget.x;
		mGridPosY = mGridTarget.y;
		// Find a new target
		FindNewTarget();
	}

	// Step 3 : move towards target
	MoveToTarget(dt);

	VCNPhysicsComponent* c = GetOwner()->GetComponent<VCNPhysicsComponent>();
	VCNPhysicActor* pActor = c->GetPhysicsActor();
	pActor->SetKinematicTarget(GetOwner()->GetWorldTransformation());

//**EndCodeRegion
}                                                              
                                                               
bool VCNAiRobotComponent::SetAttribute( const VCNString& attributeName, const VCNString& attributeValue )
{                                                              
    if (attributeName == L"GridPosX")                       
    {                                                                      
        mGridPosX = (int)_wtoi(attributeValue.c_str());       
        return true;                                                       
    }                                                                      
    else if (attributeName == L"GridPosY")                       
    {                                                                      
        mGridPosY = (int)_wtoi(attributeValue.c_str());       
        return true;                                                       
    }                                                                      
    else if (attributeName == L"MoveSpeed")                       
    {                                                                      
        mMoveSpeed = (float)_wtof(attributeValue.c_str());     
        return true;                                                       
    }                                                                      
    else if (attributeName == L"FieldViewAngle")                       
    {                                                                      
        mFieldViewAngle = (float)_wtof(attributeValue.c_str());     
        return true;                                                       
    }                                                                      
    else if (attributeName == L"DeltaAngle")                       
    {                                                                      
        mDeltaAngle = (float)_wtof(attributeValue.c_str());     
        return true;                                                       
    }                                                                      
    else if (attributeName == L"DistMargin")                       
    {                                                                      
        mDistMargin = (float)_wtof(attributeValue.c_str());     
        return true;                                                       
    }                                                                      
    else if (attributeName == L"CharacterNodeName")                       
    {                                                                      
        mCharacterNodeName = attributeValue;                           
        return true;                                                       
    }                                                                      
    else if (attributeName == L"TriggerAwarenessChange")                       
    {                                                                      
        std::stringstream ss;                                              
        std::string str( attributeValue.begin(), attributeValue.end() );   
        ss << str;                                                         
        ss >> mTriggerAwarenessChange;                                      
        return true;                                                       
    }                                                                      
    return false;                                              
}                                                              
                                                               
VCNIComponent::Ptr VCNAiRobotComponent::Copy() const                          
{                                                                              
    VCNAiRobotComponent* otherComponent = new VCNAiRobotComponent();         
                                                                               
    otherComponent->mGridPosX = mGridPosX;
    otherComponent->mGridPosY = mGridPosY;
    otherComponent->mMoveSpeed = mMoveSpeed;
    otherComponent->mFieldViewAngle = mFieldViewAngle;
    otherComponent->mDeltaAngle = mDeltaAngle;
    otherComponent->mDistMargin = mDistMargin;
    otherComponent->mCharacterNodeName = VCNString(mCharacterNodeName);
    otherComponent->mTriggerAwarenessChange = LuaTrigger(mTriggerAwarenessChange);
                                                                               
    return std::shared_ptr<VCNAiRobotComponent>(otherComponent);              
}                                                                              
                                                               
                                                               
//**CodeRegion



void VCNAiRobotComponent::FindNewTarget()
{
	// This method updates the mGridTarget position according to the direction chosen by ChooseNewDirection()
	// The rest is handled by the update method

	Directions d = ChooseNewDirection();
	// We already considered the last dir in ChooseNewDirection
	mLastDirection = d;
	switch (d) 
	{
	case UP:
		mGridTarget[1]++;
		break;

	case DOWN:
		mGridTarget[1]--;
		break;

	case LEFT:
		// If we're on the edge, wrap around
		if (mGridPosX == 0)
		{
			mGridTarget[0] = mMaze[mGridPosY].size() - 1;
		}
		else
		{
			mGridTarget[0] = mGridPosX - 1;
		}

		break;

	case RIGHT:
		// If we're on the edge, wrap around
		if (mGridPosX == mMaze[mGridPosY].size() - 1)
		{
			mGridTarget[0] = 0;
		}
		else
		{
			mGridTarget[0] = mGridPosX + 1;
		}

		break;

	default :
		throw std::invalid_argument("Something is wrong with the chosen direction");
		break;
	}
}

Vector2f VCNAiRobotComponent::GridToWorld( Vector2i gridPos )
{

	float x = (float)gridPos[0];
	int y = gridPos[1];

	//float curAng = fmod((90.0f + (mDeltaAngle / 2.0f) + (x*mDeltaAngle)),360.0f);
	float curAng = (90.0f - (mDeltaAngle / 2.0f) - (x*mDeltaAngle));

	float r;
	static const float mazeRadius = 32.0f; // A valider
	switch (y)
	{
	case 0 :
		//r = (5.0f / 10.0f) * mazeRadius;
		r = (mazeRadius / 10.0f) * 5.0f;
		break;

	case 1 :
		//r = (7.0f / 10.0f) * mazeRadius;
		r = (mazeRadius / 10.0f) * 7.0f;

		break;

	case 2 :
		//r = (9.0f / 10.0f) * mazeRadius;
		r = (mazeRadius / 10.0f) * 9.0f;

		break;

	default:
		throw std::invalid_argument("Something is wrong with the y received in GridToWorld");
		break;
	}

	float cosAngle = cos(VCN::DEG_TO_RAD_VCN(curAng));
	float sinAngle = sin(VCN::DEG_TO_RAD_VCN(curAng));

	Vector2f ans = Vector2f(mMazeCenter.x + (r*cosAngle), mMazeCenter.z + (r*sinAngle));
	return ans;
	//return Vector2f(mMazeCenter.x, mMazeCenter.z + 5.0f);
}

bool VCNAiRobotComponent::isCloseEnough()
{
	mWorldTarget = GridToWorld(mGridTarget);
	Vector3 realPos(GetOwner()->GetWorldTranslation());
	float delX = (mWorldTarget.x - realPos.x);
	float delZ = (mWorldTarget.y - realPos.z);
	return (((delX*delX) + (delZ*delZ)) <= (mDistMargin*mDistMargin));
}

Directions VCNAiRobotComponent::ChooseNewDirection()
{
	std::vector<Directions> possibleDirections;

	int x = mGridPosX;
	int y = mGridPosY;

	dir curElem = mMaze[y][x];


	if (curElem.UP)
	{
		possibleDirections.push_back(UP);

	}
	if (curElem.DOWN)
	{
		possibleDirections.push_back(DOWN);

	}
	if (curElem.LEFT)
	{
		possibleDirections.push_back(LEFT);

	}
	if (curElem.RIGHT)
	{
		possibleDirections.push_back(RIGHT);

	}

	// Remove last direction from possibilities if we have only one and there is at least an other direction to go
	// This avoid going back where we came from unless necessary

	Directions dirToAvoid = GetLastDirection(mLastDirection);

	if (dirToAvoid != INVALID && (possibleDirections.size() >= 2))
	{
		std::vector<Directions>::iterator position = std::find(possibleDirections.begin(), possibleDirections.end(), dirToAvoid);
		if (position != possibleDirections.end()) // == vector.end() means the element was not found
			possibleDirections.erase(position);
	}

	// Random selection of element in the possible directions
	int r = rand() % possibleDirections.size();

	return possibleDirections[r];
}

Directions VCNAiRobotComponent::GetLastDirection(Directions lastDirection)
{
	Directions ret = INVALID;
	switch (lastDirection)
	{
	case UP :
	Directions ret;
		ret = DOWN;
		break;
	case DOWN:
		ret = UP;
		break;
	case RIGHT:
		ret = LEFT;
		break;
	case LEFT:
		ret = RIGHT;
		break;
	default:
		//ret = INVALID;
		break;
	}
	//if (lastDirection == UP)
	//{
	//	return DOWN;
	//}
	//if (lastDirection == DOWN)
	//{
	//	return UP;
	//}
	//if (lastDirection == LEFT)
	//{
	//	return RIGHT;
	//}
	//if (lastDirection == RIGHT)
	//{
	//	return LEFT;
	//}
	return ret;
}

bool VCNAiRobotComponent::CheckForEnemy()
{
	// This function only does a ray cast to the character's position,
	// If the robot see's the character we return true

	bool result = false;
	const VCNNodeID charID = VCNNodeCore::GetInstance()->GetNodeByName(mCharacterNodeName);
	VCNNode* character = VCNNodeCore::GetInstance()->GetNode(charID);
	
	

	// DIRTY HACK TO MAKE SURE mCharacter IS INITIALIZED BEFORE RAYCASTING
	if (!mCharacterNodeInitialized && character)
	{
		mCharacter = character;
		mCharacterNodeInitialized = true;
		VCNPhysicController* physicController = character->GetProperty(VCNTXT("PhysicController")).Cast<VCNPhysicController*>();
		VCNPhysxController* physxController = (VCNPhysxController*)physicController;
		mRayCast = new PhysXRaycast(physxController->getController()->getScene());
	}

	if (mCharacterNodeInitialized)
	{
		VCN_ASSERT(character);
		VCNNode* node = NULL;
		//VCNPhysicController* physicController = character->GetProperty(VCNTXT("PhysicController")).Cast<VCNPhysicController*>();
		//VCNPhysxController* physxController = (VCNPhysxController*)physicController;
		//PhysXRaycast* mRaycast = new PhysXRaycast(physxController->getController()->getScene());
		//Vector3f characterPosition = character->GetLocalTranslation();
		Vector3f characterPosition = character->GetWorldTranslation();
		//Vector3f pos = GetOwner()->GetLocalTranslation();
		Vector3f pos = GetOwner()->GetWorldTranslation();
		
		//Vector3f dir = characterPosition - pos;
		//dir.Normalize();
		//bool resultRay = mRaycast->raycast(10000, dir, pos, node, true);
		//
		//if (resultRay)
		//{
		//	// Check if the first node hit is the character
		//	VCNNodeID resultNodeID = node->GetNodeID();
		//	const wchar_t* s1 = node->GetTag().c_str();
		//	//VCNNodeID resultPARENTNodeID = node->GetParent();
		//	//VCNNode* resultGrandParent = VCNNodeCore::GetInstance()->GetNode(resultPARENTNodeID);
		//	//const wchar_t* s2 = resultGrandParent->GetTag().c_str();
		//	//VCNNodeID resultGRANDPARENTNodeID = resultGrandParent->GetParent();

		//	//VCNNodeID characterNodeID = mCharacter->GetNodeID();
		//	//const wchar_t* s3 = mCharacter->GetTag().c_str();

		//	//VCNNodeID characterPARENTNodeID = mCharacter->GetParent();
		//	//VCNNode* CharacterGrandParent = VCNNodeCore::GetInstance()->GetNode(characterPARENTNodeID);
		//	//const wchar_t* s4 = CharacterGrandParent->GetTag().c_str();

		//	//VCNNodeID resultCHARACTERGRANDPARENTNodeID = CharacterGrandParent->GetParent();



 	//		if (_tcscmp(s1, mCharacterNodeName.c_str()) == 0)
		//	{
		//		result = true;
		//	}
		//}

		Vector3f dir = pos - characterPosition;
		dir.Normalize();
		bool resultRay = mRayCast->raycast(10000, dir, characterPosition, node, false);

		if (resultRay)
		{
			// Check if the first node hit is the character
			VCNNodeID resultNodeID = node->GetNodeID();
			const wchar_t* s1 = node->GetTag().c_str();
			//VCNNodeID resultPARENTNodeID = node->GetParent();
			//VCNNode* resultGrandParent = VCNNodeCore::GetInstance()->GetNode(resultPARENTNodeID);
			//const wchar_t* s2 = resultGrandParent->GetTag().c_str();
			//VCNNodeID resultGRANDPARENTNodeID = resultGrandParent->GetParent();

			//VCNNodeID characterNodeID = mCharacter->GetNodeID();
			//const wchar_t* s3 = mCharacter->GetTag().c_str();

			//VCNNodeID characterPARENTNodeID = mCharacter->GetParent();
			//VCNNode* CharacterGrandParent = VCNNodeCore::GetInstance()->GetNode(characterPARENTNodeID);
			//const wchar_t* s4 = CharacterGrandParent->GetTag().c_str();

			//VCNNodeID resultCHARACTERGRANDPARENTNodeID = CharacterGrandParent->GetParent();

			VCNString ownerString = GetOwner()->GetTag();
			const wchar_t*  ownerCStr = ownerString.c_str();


			if (_tcscmp(s1, ownerCStr) == 0)
			{
				result = true;
			}
		}
		


	}
	
	return result;
}

void VCNAiRobotComponent::MoveToTarget( float elapsedTime )
{
	// We assume here that mWorldTarget is OK
	Vector3 curPos = GetOwner()->GetLocalTranslation();
	mWorldTarget = GridToWorld(mGridTarget);
	Vector2f dir(mWorldTarget.x - curPos.x, mWorldTarget.y - curPos.z);

	Vector3 v(dir.x,0,dir.y);
	v.Normalize();
	v *= mMoveSpeed;
	v *= elapsedTime;
	v.y = 0.0f;
	curPos.y = 5.0f;
	GetOwner()->SetTranslation(curPos+v);
}






























//**EndCodeRegion
