///
/// @brief AI controlling a robot in a maze
///

#include "Precompiled.h"
#include "AIRobot.h"
#include <time.h> /* time */
#include <stdlib.h> /* srand, rand */



AIRobot::AIRobot(const VCNNodeID& nodeId, std::vector<std::vector<dir>> maze, Vector2i pos)
	: mNodeId(nodeId),
	mGridPos(pos),
	mGridTarget(pos),
	mMaze(maze),
	mMoveSpeed(3.0f),
	//mOnWorldTarget(true),
	mFieldViewAngle(10.0f),
	mDeltaAngle(22.5f),
	mDistMargin(5.0f)
{
	const VCNNodeID mMazeNodeId = VCNNodeCore::GetInstance()->GetNodeByName(VCNTXT("Cylinder002"));
	VCNNode* mMazeNode = VCNNodeCore::GetInstance()->GetNode(mMazeNodeId);
	
	mNode = VCNNodeCore::GetInstance()->GetNode(mNodeId);
	mWorldTarget = GridToWorld(mGridTarget);
	mNode->SetTranslation(Vector3(mWorldTarget.x,0,mWorldTarget.y));

	mMazeCenter = mMazeNode->GetWorldTranslation();
	// Initialization of the random seed
	srand(time(NULL));
}

AIRobot::~AIRobot()
{
	
}

void AIRobot::FindNewTarget()
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
		if (mGridPos[0] == 0)
		{
			mGridTarget[0] = mMaze[mGridPos[1]].size() - 1;
		}
		else
		{
			mGridTarget[0] = mGridPos[0] - 1;
		}

		break;

	case RIGHT:
		// If we're on the edge, wrap around
		if (mGridPos[0] == mMaze[mGridPos[1]].size() - 1)
		{
			mGridTarget[0] = 0;
		}
		else
		{
			mGridTarget[0] = mGridPos[0] + 1;
		}

		break;

	default :
		throw std::invalid_argument("Something is wrong with the chosen direction");
		break;
	}


}
void AIRobot::Update(float elapsedTime) 
{
	// Step 1 : check for enemy
	// TODO : Finish this part and decide what to do
	CheckForEnemy();

	// Step 2 : check if we reached target
	if (isCloseEnough())
	{
		mGridPos = mGridTarget;
		FindNewTarget();
	}
	
	// Step 3 : move towards target
	MoveToTarget(elapsedTime);

}

void AIRobot::MoveToTarget(float elapsedTime)
{
	// We assume here that mWorldTarget is OK
	Vector3 curPos = mNode->GetLocalTranslation();
	mWorldTarget = GridToWorld(mGridTarget);
	Vector2f dir(mWorldTarget.x - curPos.x, mWorldTarget.y - curPos.z);
	
	Vector3 v(dir.x,0,dir.y);
	v.Normalize();
	v *= mMoveSpeed;
	v *= elapsedTime;
	v.y = 0;

	mNode->SetTranslation(curPos+v);
	
}

bool AIRobot::CheckForEnemy()
{
	return false;
}

Vector2f AIRobot::GridToWorld(Vector2i gridPos)
{
	int x = gridPos[0];
	int y = gridPos[1];

	float curAng = (mDeltaAngle / 2.0f) + (x*mDeltaAngle);
	
	int r;
	switch (y)
	{
	case 0 :
		r = 5;
		break;

	case 1 :
		r = 7;
		break;

	case 2 :
		r = 9;
		break;

	default:
		throw std::invalid_argument("Something is wrong with the y received in GridToWorld");
		break;
	}

	return Vector2f(mMazeCenter.x + r*cos(curAng), mMazeCenter.z + r*sin(curAng));
}

Directions AIRobot::ChooseNewDirection()
{
	std::vector<Directions> possibleDirections;

	int x = mGridPos[0];
	int y = mGridPos[1];

	dir curElem = mMaze[y][x];


	if (curElem.UP)
		possibleDirections.push_back(UP);
	if (curElem.DOWN)
		possibleDirections.push_back(DOWN);
	if (curElem.LEFT)
		possibleDirections.push_back(LEFT);
	if (curElem.RIGHT)
		possibleDirections.push_back(RIGHT);

	// Remove last direction from possibilities if we have only one and there is at least an other direction to go
	// This avoid going back where we came from unless necessary
	if (mLastDirection != NULL && (possibleDirections.size() >= 2))
	{
		std::vector<Directions>::iterator position = std::find(possibleDirections.begin(), possibleDirections.end(), mLastDirection);
		if (position != possibleDirections.end()) // == vector.end() means the element was not found
			possibleDirections.erase(position);
	}

	// Random selection of element in the possible directions
	int r = rand() % possibleDirections.size();

	return possibleDirections[r];
}

//Vector2i AIRobot::WorldToGrid(Vector2f worldPos){
//	return Vector2i(0, 0);
//}

bool AIRobot::isCloseEnough()
{
	mWorldTarget = GridToWorld(mGridTarget);
	Vector3 realPos(mNode->GetLocalTranslation());
	float delX = (realPos.x - mWorldTarget.x);
	float delZ = (realPos.z - mWorldTarget.y);
	return ((delX*delX + delZ*delZ) <= mDistMargin);
}