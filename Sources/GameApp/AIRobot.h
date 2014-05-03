///
/// @brief AI controlling a robot in a maze
///

#ifndef AIROBOT_H
#define AIROBOT_H

#pragma once
#include "VCNNodes/Node.h"
#include "VCNNodes/NodeCore.h"
#include <vector>

enum Directions{UP,DOWN,LEFT,RIGHT};

struct dir
{
	bool UP;
	bool DOWN;
	bool LEFT;
	bool RIGHT;
};

class AIRobot
{
public:
	
	AIRobot(const VCNNodeID& nodeId, std::vector<std::vector<dir>> maze, Vector2i pos);
	virtual ~AIRobot();

	// Determine where the robot is going
	void FindNewTarget();
	// Update robot's pos and angle
	void Update(float elapsedTime);
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
	 
private:
  VCNNodeID							mNodeId;
  std::vector<std::vector<dir>>		mMaze;

  Vector2i							mGridPos;
  Vector2i							mGridTarget;
  //Vector2f							mWorldPos;
  Vector2f							mWorldTarget;
  Vector3							mMazeCenter;

  Directions						mLastDirection;

  //bool								mOnWorldTarget;
  
  // Angle from the entrance
  const float						mDeltaAngle;
  // Allowed distance from target point
  const int							mDistMargin;

  float								mLookAngle;
  const float						mFieldViewAngle;
  VCNNode*							mNode;
  const float						mMoveSpeed;
};

#endif // AIROBOT_H
