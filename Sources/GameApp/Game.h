///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
///
/// @brief Game interface
///

#ifndef GAME_H
#define GAME_H

#pragma once

#include <AntTweakBar.h>

// Project includes
#include "Timer.h"
#include "Camera.h"
#include "CameraController.h"
#include "SoundManager.h"
#include "MediaManager.h"
#include "StateMachine.h"

// Engine includes
#include "VCNCore/GameBase.h"


///
/// This class serve as the main game instance. Almost all entry points 
/// are directed to this class.
///
class Game : public VCNGameBase, public Singleton<Game>
{
  VCN_CLASS;
  SINGLETON_DECLARATION( Game );

public:

  /// Default constructor
  Game();

  /// Default destructor
  virtual ~Game();

  /// Create game modules.
  virtual const VCNBool CreateModules() override;

  /// Initialize the game.
  virtual VCNBool Initialize() override;
  
  /// Uninitialize the game before quitting.
  virtual VCNBool Uninitialize() override;

  /// Process a single step of the game.
  virtual VCNBool Process(const float elapsedTime) override;

  /// Prepares the upcoming frame
  virtual void PrepareFrame() const override;
  
  /// Renders the main scene
  virtual void RenderFrame() const override;
  
protected:

private:

  /// Load game sounds
  void LoadSounds();

  /// Load game 3D assets
  void LoadData();
    
  /// Load state machine
  void LoadStates();
    
  /// Creates a new game state.
  void NewGame();

// Data members
 
};

#endif // GAME_H
