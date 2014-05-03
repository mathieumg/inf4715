///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
///
/// @brief World core interface
///

#ifndef WORLD_H
#define WORLD_H

#pragma once

#include "InputController.h"

#include "VCNCore/Core.h"
#include "VCNUtils/Constants.h"
#include "VCNNodes/NodeCore.h"

// Forward declarations

///
/// The world core is responsible to store and keep track of all game world entities.
/// It also facilitate the access to all world entities for lookup and processing.
/// This core doesn't managed the lifetime of entities, but is responsible to
/// registered them to corresponding systems.
///
class World : public VCNCore<World>, InputController
{
	VCN_CLASS;

public:
	
	World();
	virtual ~World();

// Constants

// Predefined types
	
public:

// Accessors

// Interface

	/// Clears the world.
	void Clear();
	
// VCNCore interface

	/// Initializes the world core.
	virtual VCNBool Initialize() override;

	/// Releases resources and unregister entities if needed.
	virtual VCNBool Uninitialize() override;

	/// Process world entities
	virtual VCNBool Process(const float elapsedTime) override;

private:

	/// Create bases nodes for a new world.
	void CreateWorld();

	/// Creates a skybox
	void CreateSkyBox();

	/// Creates a sun
	void CreateSun();

	/// Create various test objects
	void CreateObjects();

	/// Create rooms
	void CreateRooms();

	/// Creates a tweaking toolbar
	void CreateToolbar();

// InputController interface

	// Called when the user release a key
	virtual const bool OnKeyUp(VCNUInt8 keycode, VCNUInt32 modifiers) override;

private: // Tweak bar callbacks

	static void TW_CALL SetLConstAtt(const void *value, void *clientData);
	static void TW_CALL SetLLinearAtt(const void *value, void *clientData);
	static void TW_CALL SetLQuadAtt(const void *value, void *clientData);
	static void TW_CALL SetLRange(const void *value, void *clientData);
	static void TW_CALL SetLMaxRange(const void *value, void *clientData);
	static void TW_CALL GetLConstAtt(void *value, void *clientData);
	static void TW_CALL GetLLinearAtt(void *value, void *clientData);
	static void TW_CALL GetLQuadAtt(void *value, void *clientData);
	static void TW_CALL GetLRange(void *value, void *clientData);
	static void TW_CALL GetLMaxRange(void *value, void *clientData);

	
private: // Data members

	TwBar* mBar;
};

// Quick access to the world singleton
inline World* WORLD() { return World::GetInstance(); }

#endif // WORLD_H
