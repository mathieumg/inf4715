///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
///
/// @brief World core implementation
/// 

#include "Precompiled.h"
#include "World.h"

// Project includes
#include "MediaManager.h"
#include "InputKeys.h"
#include "ParticleSystem.h"
#include "Skybox.h"
#include "SplashScreen.h"
#include "Sun.h"
#include "ProceduralTerrain.h"

// Engine includes
#include "VCNAnim/Anim.h"
#include "VCNAnim/AnimCore.h"
#include "VCNNodes/NodeCore.h"
#include "VCNNodes/RenderNode.h"
#include "VCNNodes/ComponentFactory.h"
#include "VCNNodes/TestComponent.h"
#include "VCNPhysic/PhysicCore.h"
#include "VCNRenderer/LightingCore.h"
#include "VCNRenderer/PointLight.h"
#include "VCNRenderer/RenderCore.h"
#include "VCNResources/ResourceCore.h"
#include "VCNUtils/Chrono.h"
#include "VCNUtils/Macros.h"
#include "VCNUtils/Quaternion.h"
#include "VCNUtils/Utilities.h"
#include "VCNRenderer/AnimationEvents.h"
#include "AIRobot.h"
#include <vector>

VCN_TYPE( World, VCNCore<World> );


///////////////////////////////////////////////////////////////////////
World::World()
{

}

///////////////////////////////////////////////////////////////////////
World::~World()
{

}

///////////////////////////////////////////////////////////////////////
VCNBool World::Initialize()
{
	if ( !BaseCore::Initialize() )
		return false;

	// Create always existing nodes
	CreateWorld();

	CreateToolbar();

	// World successfully created!
	return true;
}

///////////////////////////////////////////////////////////////////////
VCNBool World::Uninitialize()
{
	return BaseCore::Uninitialize();
}

///////////////////////////////////////////////////////////////////////
void World::Clear()
{
}

///////////////////////////////////////////////////////////////////////
void World::CreateWorld()
{
	// Set physic constants
	VCNPhysicCore::GetInstance()->SetGravity( Vector3(0.0f, -9.05f, 0.0f) );

	CreateSun();
	CreateSkyBox();
	CreateRooms();
	CreateObjects();
}

///////////////////////////////////////////////////////////////////////
void World::CreateSkyBox()
{
	ShowLoading( VCNTXT("Loading skybox...") );
	VCNNodeCore* nodeCore = VCNNodeCore::GetInstance();
	Skybox* skybox = nodeCore->CreateNode<Skybox>();

	// It is important that the skybox stay at the beginning of the scene graph.
	nodeCore->GetRootNode()->AttachChild( skybox->GetNodeID() );
}

///////////////////////////////////////////////////////////////////////
void World::CreateSun()
{
	ShowLoading( VCNTXT("Loading sun...") );
	VCNNodeCore* nodeCore = VCNNodeCore::GetInstance();
	Sun* sun = nodeCore->CreateNode<Sun>();
	nodeCore->GetRootNode()->AttachChild( sun->GetNodeID() );
}


///////////////////////////////////////////////////////////////////////
VCNBool World::Process(const float elapsedTime)
{  
	const VCNNodeID barrelID = VCNNodeCore::GetInstance()->GetNodeByName( VCNTXT("Barrel") );
	VCNNode* barrel = VCNNodeCore::GetInstance()->GetNode( barrelID );
	if ( barrel )
	{
		// Update transformation of actor
		VCNPhysicActor* physicActor = barrel->GetProperty( VCNTXT("PhysicActor") ).Cast<VCNPhysicActor*>();
		const Matrix4& physicTransformation = physicActor->GetTransform();

		barrel->SetRotation( physicTransformation.GetRotation() );
		barrel->SetTranslation( physicTransformation.GetTranslation() );
	}

	// Update both robots in the maze
// 	const VCNNodeID robot1ID = VCNNodeCore::GetInstance()->GetNodeByName(VCNTXT("Sphere002"));
// 	VCNNode* robot1 = VCNNodeCore::GetInstance()->GetNode( robot1ID );
// 	if (robot1)
// 	{
// 		AIRobot* r1 = robot1->GetProperty(VCNTXT("Robot1")).Cast<AIRobot*>();
// 		r1->Update(elapsedTime);
// 	}
// 
// 	const VCNNodeID robot2ID = VCNNodeCore::GetInstance()->GetNodeByName(VCNTXT("Sphere003"));
// 	VCNNode* robot2 = VCNNodeCore::GetInstance()->GetNode(robot2ID);
// 	
// 	if (robot2)
// 	{
// 		AIRobot* r2 = robot2->GetProperty(VCNTXT("Robot2")).Cast<AIRobot*>();
// 		r2->Update(elapsedTime);
// 	}

	return true;
}


///////////////////////////////////////////////////////////////////////
void World::CreateObjects()
{
}


///////////////////////////////////////////////////////////////////////
const bool World::OnKeyUp(VCNUInt8 keycode, VCNUInt32 modifiers)
{
	if ( keycode == KEYPAD_0 )
	{
		Clear();
		return true;
	}
	else if ( keycode == KEY_SPACE )
	{
		const VCNNodeID barrelID = VCNNodeCore::GetInstance()->GetNodeByName( VCNTXT("Barrel") );
		const VCNNode* barrel = VCNNodeCore::GetInstance()->GetNode( barrelID );
		if ( barrel )
		{ 
			// Give a random impulsion to the barrel!
			VCNPhysicActor* physicActor = barrel->GetProperty( VCNTXT("PhysicActor") ).Cast<VCNPhysicActor*>();
			physicActor->AddForce( VCN::RandomVector3(-400.0f, 400.0f) );
		}

		return true;
	}

	return false;
}


///////////////////////////////////////////////////////////////////////
void World::CreateRooms()
{
	// Create the terrain and add it to the scene.
	ShowLoading( VCNTXT("Load rooms...") );
	VCNNodeCore* nodeCore = VCNNodeCore::GetInstance();
	VCNNode* scene = nodeCore->GetRootNode();
	
	// Create room
// 	VCNNode* room1 = MEDIA()->CreateInstance( VCNTXT("Room1") );
// 	scene->AttachChild( room1->GetNodeID() );
// 	VCNPhysicCore::GetInstance()->CreateActor( room1 );

//  	VCNNode* crate = MEDIA()->CreateInstance( VCNTXT("Crate") );
//  	scene->AttachChild( crate->GetNodeID() );

	VCNNode* defaultScene = MEDIA()->CreateInstance( VCNTXT("DefaultScene") );
	scene->AttachChild( defaultScene->GetNodeID() );
	defaultScene->InitializeAllComponentsRecursive(); // TODO: Find a better way to do this

	///////////////////////////////////////////////////////////////////////
	// Create a barrel
	//
	VCNNode* const barrel = MEDIA()->CreateInstance( VCNTXT("Barrel") );
	barrel->SetTranslation( -5.4f, 2.2f, 1.5f );
	scene->AttachChild( barrel->GetNodeID() );
	VCNNodeCore::GetInstance()->PushName( VCNTXT("Barrel"), barrel->GetNodeID() );
	barrel->AddProperty( VCNTXT("Dynamic") );
	VCNPhysicActor* barrelPhysicActor = VCNPhysicCore::GetInstance()->CreateActor( barrel );
	VCN_ASSERT( barrelPhysicActor );
    barrel->SetSelectable(true);
	barrel->AddProperty( VCNTXT("PhysicActor"), barrelPhysicActor );

	/*VCNNode* const zombie = MEDIA()->CreateInstance( VCNTXT("Zombie") );
	zombie->SetTranslation( 1.0f, 0.41f, 10.0f );
	zombie->SetScale( 0.060f );
	scene->AttachChild( zombie->GetNodeID() );*/

	/*LoopAnimation evt;
	evt.Name = "Idle1";
	evt.TransitionTime = 0.5f;
	evt.RestartTime = 0.5f;
	zombie->FireDown(evt);*/

	///////////////////////////////////////////////////////////////////////
	// Create the character controller
	//
// 	VCNNode* const character = MEDIA()->CreateInstance(VCNTXT("Zombie"));
// 	character->SetTranslation(1.0f, 0.41f, 8.0f);
// 	character->SetScale(0.060f);
// 	scene->AttachChild(character->GetNodeID());
// 	VCNNodeCore::GetInstance()->PushName(VCNTXT("Character"), character->GetNodeID());
// 	VCNPhysicController* characterPhysicController = VCNPhysicCore::GetInstance()->CreateCharacterController(character);
// 	VCN_ASSERT(characterPhysicController);
// 	character->AddProperty(VCNTXT("PhysicController"), characterPhysicController);

	///////////////////////////////////////////////////////////////////////
	// Create the map representing the level with robots
	//	
	// naming convention
	// u : up
	// d : down
	// l : left
	// r : right
	//
	// Vertical direction goes first in the name
	//
	// NOTE : There is nothing in the middle of the circle to prevent robots from going there,
	//		  so there is only 3 layers.
	//
	
// 
// 	// 1 direction possible
// 	dir u = { true, false, false, false };
// 	dir d = { false, true, false, false };
// 	dir l = { false, false, true, false };
// 	dir r = { false, false, false, true };
// 	// 2 directions possible
// 	dir ul = { true, false, true, false };
// 	dir ur = { true, false, false, true };
// 	dir dr = { false, true, false, true };
// 	dir dl = { false, true, true, false };
// 	dir ud = { true, true, false, false };
// 	dir lr = { false, false, true, true };
// 	// 3 directions possible
// 	dir ulr = { true, false, true, true };
// 	dir udl = { true, true, true, false };
// 	dir udr = { true, true, false, true };
// 	dir dlr = { false, true, true, true };
// 	// 4 dir
// 	dir udlr = { true, true, true, true };
// 
// 	dir l1[] = { ur, lr, ulr, l, r, ulr, ulr, l, r, u, r, ulr, ulr, l, ur, ul };
// 	dir l2[] = { dl, ur, udlr, lr, ul, d, dr, ur, ul, dr, ul, d, udr, ul, ud, dr };
// 	dir l3[] = { lr, dl, dr, l, dr, lr, lr, dl, dr, l, dr, lr, dl, dr, dl, r };
// 	
// 	std::vector<std::vector<dir>> maze(3); // (3, std::vector<int>(10));
// 	std::vector<dir> innerCircle(l1, l1 + sizeof(l1) / sizeof(l1[0]));
// 	std::vector<dir> midCircle(l2, l2 + sizeof(l2) / sizeof(l2[0]));
// 	std::vector<dir> extCircle(l3, l3 + sizeof(l3) / sizeof(l3[0]));
// 	maze[0] = innerCircle;
// 	maze[1] = midCircle;
// 	maze[2] = extCircle;

	///////////////////////////////////////////////////////////////////////
	// Create the robots controlled by AI
	//
	//VCNNode* const robot1 = MEDIA()->CreateInstance(VCNTXT("Zombie"));
	//VCNNode* const robot2 = MEDIA()->CreateInstance(VCNTXT("Zombie"));
	//robot1->SetTranslation(3.0f, 0.41f, 8.0f);
	//robot2->SetTranslation(5.0f, 0.41f, 8.0f);
	//robot1->SetScale(0.030f);
	//robot2->SetScale(0.030f);
	//scene->AttachChild(robot1->GetNodeID());
	//scene->AttachChild(robot2->GetNodeID());
// 	const VCNNodeID NodeIdR1 = VCNNodeCore::GetInstance()->GetNodeByName(VCNTXT("Sphere002"));
// 	const VCNNodeID NodeIdR2 = VCNNodeCore::GetInstance()->GetNodeByName(VCNTXT("Sphere003"));
// 	VCNNode* robot1 = VCNNodeCore::GetInstance()->GetNode(NodeIdR1);
// 	VCNNode* robot2 = VCNNodeCore::GetInstance()->GetNode(NodeIdR2);
// 	//VCNNodeCore::GetInstance()->PushName(VCNTXT("Robot1"), robot1->GetNodeID());
// 	//VCNNodeCore::GetInstance()->PushName(VCNTXT("Robot2"), robot2->GetNodeID());
// 	AIRobot* r1 = new AIRobot(robot1->GetNodeID(), maze, Vector2i(6, 2));
// 	AIRobot* r2 = new AIRobot(robot2->GetNodeID(), maze, Vector2i(11, 1));
// 	VCN_ASSERT(r1);
// 	VCN_ASSERT(r2);
// 	robot1->AddProperty(VCNTXT("Robot1"), r1);
// 	robot2->AddProperty(VCNTXT("Robot2"), r2);

	///////////////////////////////////////////////////////////////////////
	// Create a lantern
	//
	VCNNode* const lantern = MEDIA()->CreateInstance( VCNTXT("Lantern") );
	lantern->SetTranslation( -0.54f, 3.18f, -3.054f );
	scene->AttachChild( lantern->GetNodeID() );

	///////////////////////////////////////////////////////////////////////
	// Create a point light
	//
// 	VCNPointLight* light = VCNLightingCore::GetInstance()->CreateLight( LT_OMNI, VCNTXT("PointLight") )->Cast<VCNPointLight>();
// 	light->SetTranslation( 0.0f, 0.1f, 0.0f );
// 	light->SetConstantAttenuation( 0.24f );
// 	light->SetLinearAttenuation( 0.3305f );
// 	light->SetQuadAttenuation( 0.01f );
// 	light->SetRange( 1.0f );
// 	light->SetMaxRange( 16.0f );
// 	lantern->AttachChild( light->GetNodeID() );
// 	VCNNodeCore::GetInstance()->PushName( VCNTXT("MainLight"), light->GetNodeID() );
	
	///////////////////////////////////////////////////////////////////////
	// Create a particle emitter
	//
	ParticleSystem* lanternParticles = VCNNodeCore::GetInstance()->CreateNode<ParticleSystem>();
	lanternParticles->CreateFireEmitter( lantern->GetWorldTranslation() + Vector3(0, 0.021f, 0) );
	scene->AttachChild( lanternParticles->GetNodeID() );
	
	// Create physic components
#ifdef DEBUG
	VCNPhysicCore::GetInstance()->CreateGroundPlane();
#endif
}

void World::CreateToolbar()
{
	std::stringstream ss;
	const VCNUInt32 barWidth = 250;
	const VCNUInt32 barHeight = 325;

	mBar = TwNewBar("WorldBar");

	const VCNPoint& screenDimension = VCNRenderCore::GetInstance()->GetResolution();

	ss <<
		"WorldBar "
		"label='[World Tool]' "
		"color='150 100 150 50' "
		"position='40 " << screenDimension.y - barHeight - 130 << "' "
		"size='" << barWidth << " " << barHeight << "' "
		"valuesWidth=100 "
		"refresh=1.0"
		<< " iconified=true "
		;

	TwDefine(ss.str().c_str());

	TwAddVarCB(mBar, "LConstAtt", TW_TYPE_FLOAT, SetLConstAtt,  GetLConstAtt,  this, "label='Const. Att.' group='Light'");
	TwAddVarCB(mBar, "LLinearAtt",TW_TYPE_FLOAT, SetLLinearAtt, GetLLinearAtt, this, "label='Linear Att.' group='Light'");
	TwAddVarCB(mBar, "LQuadAtt",  TW_TYPE_FLOAT, SetLQuadAtt,   GetLQuadAtt,   this, "label='Quad. Att.' group='Light'");
	TwAddVarCB(mBar, "LRange",    TW_TYPE_FLOAT, SetLRange,     GetLRange,     this, "label='Range' group='Light'");
	TwAddVarCB(mBar, "LMaxRange", TW_TYPE_FLOAT, SetLMaxRange,  GetLMaxRange,  this, "label='Max Range' group='Light'");
}

///////////////////////////////////////////////////////////////////////
void TW_CALL World::SetLConstAtt(const void *value, void *clientData)
{
	VCNNodeID mainLightID = VCNNodeCore::GetInstance()->GetNodeByName( VCNTXT("MainLight") );
	if ( VCNPointLight* light = VCNNodeCore::GetInstance()->GetNode( mainLightID )->TryCast<VCNPointLight>() )
	{
		const float* val = (const float*)value;
		light->SetConstantAttenuation( *val );
	}
}

///////////////////////////////////////////////////////////////////////
void TW_CALL World::SetLLinearAtt(const void *value, void *clientData)
{
	VCNNodeID mainLightID = VCNNodeCore::GetInstance()->GetNodeByName( VCNTXT("MainLight") );
	if ( VCNPointLight* light = VCNNodeCore::GetInstance()->GetNode( mainLightID )->TryCast<VCNPointLight>() )
	{
		const float* val = (const float*)value;
		light->SetLinearAttenuation( *val );
	}
}

///////////////////////////////////////////////////////////////////////
void TW_CALL World::SetLQuadAtt(const void *value, void *clientData)
{
	VCNNodeID mainLightID = VCNNodeCore::GetInstance()->GetNodeByName( VCNTXT("MainLight") );
	if ( VCNPointLight* light = VCNNodeCore::GetInstance()->GetNode( mainLightID )->TryCast<VCNPointLight>() )
	{
		const float* val = (const float*)value;
		light->SetQuadAttenuation( *val );
	}
}

///////////////////////////////////////////////////////////////////////
void TW_CALL World::SetLRange(const void *value, void *clientData)
{
	VCNNodeID mainLightID = VCNNodeCore::GetInstance()->GetNodeByName( VCNTXT("MainLight") );
	if ( VCNPointLight* light = VCNNodeCore::GetInstance()->GetNode( mainLightID )->TryCast<VCNPointLight>() )
	{
		const float* val = (const float*)value;
		light->SetRange( *val );
	}
}

///////////////////////////////////////////////////////////////////////
void TW_CALL World::SetLMaxRange(const void *value, void *clientData)
{
	VCNNodeID mainLightID = VCNNodeCore::GetInstance()->GetNodeByName( VCNTXT("MainLight") );
	if ( VCNPointLight* light = VCNNodeCore::GetInstance()->GetNode( mainLightID )->TryCast<VCNPointLight>() )
	{
		const float* val = (const float*)value;
		light->SetMaxRange( *val );
	}
}

///////////////////////////////////////////////////////////////////////
void TW_CALL World::GetLConstAtt(void *value, void *clientData)
{
	VCNNodeID mainLightID = VCNNodeCore::GetInstance()->GetNodeByName( VCNTXT("MainLight") );
	if ( VCNPointLight* light = VCNNodeCore::GetInstance()->GetNode( mainLightID )->TryCast<VCNPointLight>() )
	{
		float* out = (float*)value;
		*out = light->GetConstantAttenuation();
	}
}

///////////////////////////////////////////////////////////////////////
void TW_CALL World::GetLLinearAtt(void *value, void *clientData)
{
	VCNNodeID mainLightID = VCNNodeCore::GetInstance()->GetNodeByName( VCNTXT("MainLight") );
	if ( VCNPointLight* light = VCNNodeCore::GetInstance()->GetNode( mainLightID )->TryCast<VCNPointLight>() )
	{
		float* out = (float*)value;
		*out = light->GetLinearAttenuation();
	}
}

///////////////////////////////////////////////////////////////////////
void TW_CALL World::GetLQuadAtt(void *value, void *clientData)
{
	VCNNodeID mainLightID = VCNNodeCore::GetInstance()->GetNodeByName( VCNTXT("MainLight") );
	if ( VCNPointLight* light = VCNNodeCore::GetInstance()->GetNode( mainLightID )->TryCast<VCNPointLight>() )
	{
		float* out = (float*)value;
		*out = light->GetQuadAttenuation();
	}
}

///////////////////////////////////////////////////////////////////////
void TW_CALL World::GetLRange(void *value, void *clientData)
{
	VCNNodeID mainLightID = VCNNodeCore::GetInstance()->GetNodeByName( VCNTXT("MainLight") );
	if ( VCNPointLight* light = VCNNodeCore::GetInstance()->GetNode( mainLightID )->TryCast<VCNPointLight>() )
	{
		float* out = (float*)value;
		*out = light->GetRange();
	}
}

///////////////////////////////////////////////////////////////////////
void TW_CALL World::GetLMaxRange(void *value, void *clientData)
{
	VCNNodeID mainLightID = VCNNodeCore::GetInstance()->GetNodeByName( VCNTXT("MainLight") );
	if ( VCNPointLight* light = VCNNodeCore::GetInstance()->GetNode( mainLightID )->TryCast<VCNPointLight>() )
	{
		float* out = (float*)value;
		*out = light->GetMaxRange();
	}
}
