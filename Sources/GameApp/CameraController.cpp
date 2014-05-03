///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
///
/// @brief Gameplay controller implementation.
///

#include "Precompiled.h"
#include "CameraController.h"

// Project includes
#include "Camera.h"
#include "Config.h"
#include "InputManager.h"
#include "StateMachine.h"
#include "Terrain.h"
#include "World.h"

// Engine includes
#include "VCNUtils/Macros.h"
#include "VCNUtils/Utilities.h"
#include "VCNRenderer/XformCore.h"
#include "VCNPhysic/PhysicCore.h"
#include "VCNPhysX/PhysXRaycast.h"
#include "VCNUtils/Vector.h"
#include "characterkinematic/PxControllerObstacles.h"
#include "VCNPhysX/PhysxController.h"
#include "characterkinematic/PxControllerManager.h"
#include "VCNNodes/PhysicsComponent.h"
#include "VCNNodes/UsableComponent.h"


//////////////////////////////////////////////////////////////////////////
///
///  Constructs the gameplay controller.
///
///  This function doesn't return a value
//
//////////////////////////////////////////////////////////////////////////
CameraController::CameraController(Camera& camera)
: mCamera(camera)
, mPreviousCursorPosition()
, mFreeLook(true)
, mInterpolatedCameraPosition(16)
, mJumpForce(30.0f)
, mMovingState(Idle)
, mCanMove(true)
{
	VCNNodeCore* nodeCore = VCNNodeCore::GetInstance();
	VCNNode* nodeController = nodeCore->GetNode(nodeCore->GetNodeByName(VCNTXT("Character")));
	VCN_ASSERT(nodeController);
	VCNPhysicController* controller = nodeController->GetProperty(VCNTXT("PhysicController")).Cast<VCNPhysicController*>();
	VCN_ASSERT(controller);

	mPhysxController = (VCNPhysxController*)controller;
	mCharacterNode = nodeController;
	mGravity = mPhysxController->getController()->getScene()->getGravity().y;

    mRaycast = new PhysXRaycast(mPhysxController->getController()->getScene());

	//controller->;
	//PxU32 collisionFlags = controller->move(const PxVec3& disp, PxF32 minDist, PxF32 elapsedTime, const PxControllerFilters& filters, const PxObstacleContext* obstacles = NULL);

	// Update transformation of actor
	/*VCNPhysicActor* physicActor = barrel->GetProperty(VCNTXT("PhysicActor")).Cast<VCNPhysicActor*>();
	const Matrix4& physicTransformation = physicActor->GetTransform();*/
	//barrel->SetRotation(physicTransformation.GetRotation());
	//barrel->SetTranslation(physicTransformation.GetTranslation());
}

//////////////////////////////////////////////////////////////////////////
///  Destructs the camera controller.
///
///  This function doesn't return a value
//////////////////////////////////////////////////////////////////////////
CameraController::~CameraController(void)
{
}

////////////////////////////////////////////////////////////////////////
/// Updates gameplay according to user inputs.
///
/// @param[in] elapsedTime: elapsed time between calls.
///
/// @return nothing
////////////////////////////////////////////////////////////////////////
void CameraController::Update(float elapsedTime)
{
	const bool playState = StateMachine::GetInstance().IsStateActive(GS_PLAY);

	// No character controller, there is a problem, or it is not yet initialized
	if (!mPhysxController)
		return;

	if (playState)
	{
		ReadInputs(elapsedTime);
		UpdateView();
	}
}


//////////////////////////////////////////////////////////////////////////
void CameraController::ReadInputs(const VCNFloat elapsedTime)
{
	InputManager& inputManager = InputManager::GetInstance();
    CameraMovingState newMovingState = Idle;
    boolean moving = false;

	//float distance = kSpeed * elapsedTime;

	// Check for player boost speed
	//if (inputManager.IsKeyPressed(KEY_SHIFT)){ distance *= 4; }

	//// Update the player position based on movement 
	//if (inputManager.IsKeyPressed(KEY_UP) || inputManager.IsKeyPressed(KEY_W))    { movedDistance += mCamera.GetDirection() * distance; }
	//if (inputManager.IsKeyPressed(KEY_DOWN) || inputManager.IsKeyPressed(KEY_S))  { movedDistance += mCamera.GetDirection() * -distance; }
	//if (inputManager.IsKeyPressed(KEY_LEFT) || inputManager.IsKeyPressed(KEY_A))  { movedDistance += mCamera.GetStrafeDirection() * -distance; }
	//if (inputManager.IsKeyPressed(KEY_RIGHT) || inputManager.IsKeyPressed(KEY_D)) { movedDistance += mCamera.GetStrafeDirection() * distance; }

	if (inputManager.IsKeyPressed(KEY_SPACE)) 
	{ 
		startJump(); 
	}

	const PxControllerFilters filters(0, NULL, NULL);

	PxVec3 disp;
	const PxF32 heightDelta = mPhysxController->getJump().getHeight(elapsedTime);
	float dy;
	if (heightDelta != 0.0f)
	{
		dy = heightDelta;
	}
	else
	{
		dy = mGravity * elapsedTime;
	}

	PxVec3 targetKeyDisplacement(0);

	Vector3 dir = mCamera.GetDirection();
	PxVec3 forward = PxVec3(dir.x, dir.y, dir.z);

	forward.y = 0;
	forward.normalize();
	PxVec3 up = PxVec3(0, 1, 0);
	PxVec3 right = forward.cross(up);

	Vector3 movedDistance(Vector3::Zero);
	const Vector3 currentCameraPosition = mCamera.GetViewerPosition();

	const float kSpeed = 7.0f;

	// Update the player position based on movement 
	if (mPhysxController->getJump().mJump)
	{
		targetKeyDisplacement = mOldTargetKeyDisplacement;
	}
	else
	{
        if (mCanMove) {
            if (inputManager.IsKeyPressed(KEY_UP) || inputManager.IsKeyPressed(KEY_W))
            {
                moving = true;
                targetKeyDisplacement += forward;
            }

            if (inputManager.IsKeyPressed(KEY_DOWN) || inputManager.IsKeyPressed(KEY_S))
            {
                moving = true;
                targetKeyDisplacement -= forward;
            }

            if (inputManager.IsKeyPressed(KEY_LEFT) || inputManager.IsKeyPressed(KEY_A))
            {
                moving = true;
                targetKeyDisplacement += right;
            }

            if (inputManager.IsKeyPressed(KEY_RIGHT) || inputManager.IsKeyPressed(KEY_D))
            {
                moving = true;
                targetKeyDisplacement -= right;
            }
        }

		float mRunningSpeed = 1.5 * kSpeed;
		float mWalkingSpeed = kSpeed;

        if (moving)
        {
            newMovingState = Walking;
        }
        else
        {
            newMovingState = Idle;
        }
        
        if (inputManager.IsKeyPressed(KEY_SHIFT))
        {
            newMovingState = Running;
        }

		targetKeyDisplacement *= (inputManager.IsKeyPressed(KEY_SHIFT)) ? mRunningSpeed : mWalkingSpeed;
		targetKeyDisplacement *= elapsedTime;
		mOldTargetKeyDisplacement = targetKeyDisplacement;
	}

	disp = targetKeyDisplacement;
	disp.y = dy;

//    static PxObstacleContext* obstacleContext = mPhysxController->getManager()->createObstacleContext();
	const PxU32 flags = mPhysxController->getController()->move(disp, 0.001f, elapsedTime, filters, NULL/*obstacleContext*/);

    CameraData newPositionData;
    if (disp.magnitude() > 0)
    {
        newPositionData.eventType = Moving;
        newPositionData.newPosition = mCamera.GetViewerPosition();
        newPositionData.newLookAt = mCamera.GetFocusPosition(); // mCamera.GetFocusPosition() or mCamera.GetDirection()

        Notify(newPositionData);
    }

	if (flags & PxControllerFlag::eCOLLISION_DOWN)
	{
		mPhysxController->getJump().stopJump();
	}
    /*else if(flags & PxControllerFlag::eCOLLISION_SIDES)
    {
        for(PxU32 i = 0; i < obstacleContext->getNbObstacles(); ++i)
        {
            const PxObstacle* obstacle = obstacleContext->getObstacle(i);
            if(obstacle->mUserData != nullptr)
            {
                VCNNode* node = (VCNNode*)obstacle->mUserData;
                if(node->GetTag() == VCNTXT("TetrisPiece"))
                {
                    VCNPhysicsComponent* physics = node->GetComponent<VCNPhysicsComponent>();
                    if(physics != nullptr)
                    {
                        VCNPhysicActor* actor = physics->GetPhysicsActor();
                        actor->AddForce(Vector3(disp.x, disp.y, disp.z));
                    }
                }
            }
        }
    }*/
	//mInterpolatedCameraPosition.In(movedDistance);

	// Free look
	if (mFreeLook)
	{
		if (inputManager.IsMouseCaptured())
		{
			int deltaX = 0;
			int deltaY = 0;
			inputManager.GetMouseMovements(deltaX, deltaY);

			if (deltaX != 0 || deltaY != 0)
			{
				mCamera.Rotate(-deltaX, deltaY);
			}
		}
		else
		{
			// Remove free look if the mouse is no more captured.
			mFreeLook = false;
		}
	}

	//mCamera.SetViewerPosition(currentCameraPosition + mInterpolatedCameraPosition.Out());
	PxExtendedVec3 v = mPhysxController->getController()->getPosition();
	//PxExtendedVec3 h = mPhysxController->getController()->
	Vector3 p = Vector3(v.x, v.y, v.z);
	mCharacterNode->SetTranslation(p);
	p.y += 1.5f;
	mCamera.SetViewerPosition(p);

	//mCharacterNode->SetRotation(mCamera.);
	

	// Bound camera position
	BoundCamera();

    if (newMovingState != mMovingState)
    {
        static CameraData camData;
        switch (newMovingState)
        {
            case Idle:
                camData.eventType = MovingStop;
                break;

            case Walking:
                camData.eventType = WalkingStart;
                break;

            case Running:
                camData.eventType = RunningStart;
                break;
        }
        mMovingState = newMovingState;
        Notify(camData);
    }
}

bool CameraController::canJump(VCNPhysxController* actor)
{
	PxControllerState cctState;
	actor->getController()->getState(cctState);
	return (cctState.collisionFlags & PxControllerFlag::eCOLLISION_DOWN) != 0;
}

void CameraController::startJump()
{
	if (canJump(mPhysxController))
	{
		mPhysxController->jump(mJumpForce);
        static CameraData camData;
        camData.eventType = Jumping;
        Notify(camData);
	}
}

//////////////////////////////////////////////////////////////////////////
VCNBool CameraController::IsFreeLook()
{
    return mFreeLook;
}

//////////////////////////////////////////////////////////////////////////
void CameraController::EnterFreeLook()
{
	InputManager& inputManager = InputManager::GetInstance();
	if (!inputManager.IsMouseCaptured())
	{
		inputManager.GetCursorPosition(mPreviousCursorPosition);
		inputManager.SetMouseCaptured(true);
	}

    mFreeLook = true;
}

//////////////////////////////////////////////////////////////////////////
void CameraController::LeaveFreeLook()
{
	InputManager& inputManager = InputManager::GetInstance();
	if (inputManager.IsMouseCaptured())
	{
		inputManager.SetMouseCaptured(false);
		inputManager.SetCursorPosition(mPreviousCursorPosition);
	}

    mFreeLook = false;
}

//////////////////////////////////////////////////////////////////////////
void CameraController::UpdateView()
{
	// Place camera
	mCamera.Update();

	// Update the physic debugger camera if any
	// TODO: Use observer pattern
	VCNPhysicCore* PhysicCore = VCNPhysicCore::GetInstance();
	if (PhysicCore)
	{
		PhysicCore->UpdateDebugCamera(mCamera.GetViewerPosition(), mCamera.GetFocusPosition(), mCamera.GetUpDirection());
	}

	// Update view
	VCNXformCore::GetInstance()->SetViewLookAt(mCamera.GetViewMatrix());

    
}

void CameraController::OnMouseMotion(MouseEventArgs& args)
{
    InputManager& inputManager = InputManager::GetInstance();
	const bool playState = StateMachine::GetInstance().IsStateActive(GS_PLAY);
	
    if (playState && mFreeLook && mCanMove)
	{
		static CameraData camData;

		VCNNode* node = NULL;
		float maxDistance = 10.0f;

		// First test to see if we can find a UsableComponent
		bool result = mRaycast->raycast(100000.0f, mCamera.GetDirection(), mCamera.GetViewerPosition(), node);
		if(result)
		{
			VCNUsableComponent* usableComponent = node->GetComponent<VCNUsableComponent>();
			if (usableComponent != NULL)
			{
				maxDistance = usableComponent->GetUseDistance();
			}
		}

		VCNNode* node2 = NULL;
		bool result2 = mRaycast->raycast(maxDistance, mCamera.GetDirection(), mCamera.GetViewerPosition(), node2);

		// Undo effect on selected if it exists
		if(camData.selected != NULL)
		{
			VCNUsableComponent* usableComponent = camData.selected->GetComponent<VCNUsableComponent>();
			if (usableComponent != NULL)
			{
				usableComponent->SetIsBeingPointed(false);
			}
			camData.selected->SetSelected(false);
		}

		if(result2)
		{
			// Set effect on new selection
			VCNUsableComponent* usableComponent = node2->GetComponent<VCNUsableComponent>();
			if (usableComponent != NULL)
			{
				usableComponent->SetIsBeingPointed(true);
			}
		    node2->SetSelected(true);
		}

		if (camData.selected != node2)
		{
			if(node2 && node2->IsSelectable())
			{
				camData.selected = node2;
			}
			else
			{
				camData.selected = NULL;
			}
			camData.eventType = PointingObject;
			Notify(camData);
		}
	}
}

//////////////////////////////////////////////////////////////////////////
void CameraController::OnKeyDown(VCNUInt8 keycode, VCNUInt32 modifiers)
{
}

//////////////////////////////////////////////////////////////////////////
const bool CameraController::OnKeyUp(VCNUInt8 keycode, VCNUInt32 modifiers)
{
	if (!StateMachine::GetInstance().IsStateActive(GS_PLAY))
		return false;

    /*
	// Check if the user wants to recover the mouse cursor
	if (keycode == KEY_ESCAPE)
	{
		mFreeLook = false;
		LeaveFreeLook();

		return true;
	}
    */


	return false;
}

//////////////////////////////////////////////////////////////////////////
void CameraController::OnRightMouseButtonDown(MouseEventArgs& args)
{
}

///////////////////////////////////////////////////////////////////////
void CameraController::OnRightMouseButtonUp(MouseEventArgs& args)
{
    /*
    mFreeLook = !mFreeLook;
	if (mFreeLook)
	{
		EnterFreeLook();
	}
	else
	{
		LeaveFreeLook();
	}
    */
}

//////////////////////////////////////////////////////////////////////////
void CameraController::OnMouseWheel(int xPos, int yPos, int fwKeys, short zDelta)
{
	// The wheel rotation (zDelta) is a multiple of WHEEL_DELTA, which is set at 120
	// A positive value indicates that the wheel was rotated forward, away from the user; 
	// A negative value indicates that the wheel was rotated backward, toward the user.
	if (zDelta < 0 && mCamera.GetViewerPosition().y < 50 ||
		zDelta > 0 && mCamera.GetViewerPosition().y > 10)
	{
		zDelta = VCN::Clamp<short>(zDelta, -120, 120);
		mCamera.Move(static_cast<VCNFloat>(zDelta / 10));
		BoundCamera();
	}
}

//////////////////////////////////////////////////////////////////////////
void CameraController::teleport(double posX, double posY, double posZ)
{
	const PxExtendedVec3 newPosition1 = PxExtendedVec3(posX, posY, posZ);
	const Vector3 newPosition2 = Vector3(posX, posY, posZ);
	mPhysxController->getController()->setPosition(newPosition1);
	mCamera.SetViewerPosition(newPosition2);
}

//////////////////////////////////////////////////////////////////////////
void CameraController::setMobility(VCNBool enabled)
{
    mCanMove = enabled;
}

///////////////////////////////////////////////////////////////////////
void CameraController::BoundCamera()
{
	Vector3 camDir = mCamera.GetDirection();
	Vector3 camPos = mCamera.GetViewerPosition();

	camPos.x = VCN::Clamp(camPos.x, -480.0f, 480.0f);
	camPos.z = VCN::Clamp(camPos.z, -480.0f, 480.0f);
	camPos.y = VCN::Clamp(camPos.y, 0.0f, 100.0f);

	mCamera.SetViewerPosition(camPos);
	mCamera.SetFocusPosition(camPos + camDir);
}

///////////////////////////////////////////////////////////////////////