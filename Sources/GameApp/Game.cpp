///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
///
/// @brief Game entry point.x Game logic starts here.
///

#include "Precompiled.h"
#include "Game.h"

// Project includes
#include "Config.h"
#include "ConfigLoader.h"
#include "Display.h"
#include "MediaManager.h"
#include "InputManager.h"
#include "PlayState.h"
#include "World.h"

// Engine includes
#include "VCNAnim/AnimCore.h"
#include "VCNCore/CoreTimer.h"
#include "VCND3D9/D3D9.h"
#include "VCND3D9/D3D9EffectCore.h"
#include "VCND3D9/D3D9MaterialCore.h"
#include "VCND3D9/D3D9ShaderCore.h"
#include "VCNFMODSound/FMODAudioCore.h"
#include "VCNNodes/ComponentCore.h"
#include "VCNParticles/ParticleCore.h"
#include "VCNPhysX/PhysxCore.h"
#include "VCNRenderer/GPUProfiling.h"
#include "VCNRenderer/LightingCore.h"
#include "VCNRenderer/XformCore.h"
#include "VCNResources/ResourceCore.h"
#include "VCNUtils/Chrono.h"
#include "VCNWindows/Windows.h"
#include "VCNWwise/WwiseAudioCore.h"
#include "VCNLua/LuaCore.h"

VCN_TYPE( Game, VCNGameBase );

///////////////////////////////////////////////////////////////////////
Game::Game()
{
}


///////////////////////////////////////////////////////////////////////
Game::~Game()
{
}


///////////////////////////////////////////////////////////////////////
///
/// Creates all the modules needed by this game. 
/// NOTE: Order is important
///
const VCNBool Game::CreateModules()
{
	// System cores
	AddCoreModule( CreateCore<VCNCoreTimer>() );
	AddCoreModule( CreateCore<VCNWindows>() );
	AddCoreModule( CreateCore<VCNResourceCore>() );
	AddCoreModule( CreateCore<VCND3D9>() );
	AddCoreModule( CreateCore<VCNDXShaderCore>() );
	AddCoreModule( CreateCore<VCNDXMaterialCore>() );
	AddCoreModule( CreateCore<VCND3D9EffectCore>() );
	AddCoreModule( CreateCore<VCNAnimCore>() );
	//AddCoreModule( CreateCore<VCNFMODAudioCore>() );
	AddCoreModule( CreateCore<VCNWwiseAudioCore>() );
	AddCoreModule( CreateCore<VCNLightingCore>() );
	AddCoreModule( CreateCore<VCNParticleCore>() );
	AddCoreModule( CreateCore<VCNXformCore>() );
	AddCoreModule( CreateCore<VCNComponentCore>() );
	AddCoreModule( CreateCore<VCNNodeCore>() );
	AddCoreModule( CreateCore<VCNPhysxCore>() );
	AddCoreModule( CreateCore<VCNLuaCore>() );
	
	// Gameplay cores
	AddCoreModule( CreateCore<MediaManager>() ); // RENAME MediaManager
	AddCoreModule( CreateCore<World>() );     // RENAME WorldManager

	// Nothing to report
	return true;
}


//////////////////////////////////////////////////////////////////////////
VCNBool Game::Initialize()
{
	// Initialize engine
	const bool success = VCNGameBase::Initialize();
	if ( !success )
		return false;

	LoadData();
	LoadSounds(); 
	LoadStates();
	
	// Reset core timer so loading time doesn't impact what fallows
	VCNCoreTimer::GetInstance()->Reset();

	return success;
}

//////////////////////////////////////////////////////////////////////////
VCNBool Game::Process(const float elapsedTime)
{  
	InputManager::GetInstance().Update( elapsedTime );

	// Run service processes first
	if ( VCNGameBase::Process( elapsedTime ) )
	{    
		// Update game managers  
		SoundManager::GetInstance().Update( elapsedTime );

		// Process the entire scene
		VCNNodeCore::GetInstance()->GetRootNode()->Process( elapsedTime );

		return true;
	}

	return false;
}


///////////////////////////////////////////////////////////////////////
void Game::PrepareFrame() const
{
	GPU_PROFILE_BLOCK_NAME( L"PrepareFrame" );

	StateMachine::GetInstance().PreRender();
}


//////////////////////////////////////////////////////////////////////////
void Game::RenderFrame() const 
{ 
	GPU_PROFILE_BLOCK_NAME( L"RenderFrame" );

	// Notify the state machine to render its current state
	StateMachine::GetInstance().Render();
}


///////////////////////////////////////////////////////////////////////
///
/// Release game resources
///
VCNBool Game::Uninitialize()
{
	// Uninitialize the state machine and its child state
	StateMachine::GetInstance().Uninitialize();

	// Release singleton managers
	StateMachine::Release();
	SoundManager::Release();

	return VCNGameBase::Uninitialize();
}


//////////////////////////////////////////////////////////////////////////
void Game::LoadSounds()
{
	SoundManager::GetInstance().LoadSounds();
}


//////////////////////////////////////////////////////////////////////////
void Game::LoadData()
{
	// CCW culling for now
	VCNRenderCore::GetInstance()->SetBackfaceCulling( RS_CULL_CCW );

	// Get the screen dimensions (used for aspect calculations)
	const VCNPoint point = VCNRenderCore::GetInstance()->GetResolution();
	const float aspect = point.x / static_cast<float>(point.y);

	// Set some default projections
	VCNXformCore::GetInstance()->SetClippingPlanes( 0.5f, 512.0f );
	VCNXformCore::GetInstance()->SetProjectionType( PT_PERSPECTIVE );
	VCNXformCore::GetInstance()->SetPerspectiveMatrix( 46.8f, aspect );
}


///////////////////////////////////////////////////////////////////////
void Game::LoadStates()
{
	// Initialize the state machine and its child state
	// A New game will be create after this call.
	StateMachine::GetInstance().Initialize( GS_MAIN_MENU );
}


///////////////////////////////////////////////////////////////////////
void Game::NewGame()
{
}
