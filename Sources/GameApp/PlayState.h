///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
///
/// @brief Interface of the main gaming state.
///

#ifndef PLAYSTATE_H
#define PLAYSTATE_H

#pragma once

// Project includes
#include "Camera.h"
#include "GameSmartTypes.h"
#include "GameState.h"
#include "ItemStore.h"
#include "Inventory.h"
#include "HUD.h"

using namespace patterns;
struct CameraData;
class VCNNode;

// Engine includes
#include "VCNScripting/ScriptingCore.h"
#include "VCNAudio/AudioCore.h"
#include "VCNCore/CoreTimer.h"

// Forward declarations

///
/// Keep tracks of the game state. When this state is active,
/// the player is playing. New games our created by this state.
///
class PlayState : public GameState, public Observer<InventoryData>, public Observer<HUDAction>, public Observer<CameraData>, public Observer<ItemStoreData>
{

public:

  PlayState();
  virtual ~PlayState();
  
public:

  /// Initialize state for a new game
  void NewGame();

  /// Pre render the current frame (pre effects)
  virtual void PreRender() const override;
  
  /// Render the game
  virtual void Render() const override;

  /// Updates the state
  virtual void Update(float elapsedTime) override;
  
  /// Returns the camera.
  Camera& GetCamera() { return mCamera; }
  const Camera& GetCamera() const { return mCamera; }

  /// Called when a new game is requested.
  virtual void OnNewGame();

  /// Called when the item store is updated.
  virtual void UpdateObserver(ItemStoreData& hint);

  /// Called when the inventory is updated.
  virtual void UpdateObserver(InventoryData& hint);

  /// Called when the HUD executes an action.
  virtual void UpdateObserver(HUDAction& hint);

  virtual void setEquippedItem(const std::string itemId);

  // Fade the scren to a certain percentage.
  virtual void fadeScreen(VCNInt percentage, VCNInt duration, const std::string& callbackName);

  /// Called when the camera data is updated.
  virtual void UpdateObserver(CameraData& hint);

protected:

// GameState Interface

  /// Initialize the main game state
  virtual VCNBool Initialize() override;

  /// Release resources used by the main game state
  virtual VCNBool Uninitialize() override;

  /// Resume play
  virtual void OnLeave() override;

  /// Pause play
  virtual void OnEnter() override;
  
  /// Called when the player press a key.
  virtual const bool OnKeyPress(VCNUInt8 keycode) override;

  /// Handle left mouse button press.
  virtual void OnLeftMouseButtonDown(MouseEventArgs& args) override;

private:
  
  /// Initialize game fonts
  VCNBool InitializeFonts();
  
  /// Renders the game HUD
  void RenderHUD() const;
  
// Data members

  Camera                      mCamera;
  CameraControllerPointer     mCameraController;
  VCNUInt                     mHUDFont;
  bool                        mNewGame;
  HUDPointer                  mHUD;
  DebugGraphPointer           mDebugGraph;
  ItemStore*                  mItemStore;
  Inventory*				  mInventory;
  std::string                 mCurrentItem;
  std::string                 mCurrentActionVerb;
  VCNScriptingCore*			  mScripting;
  VCNAudioCore*               mAudio;

  VCNNode*                    mSelectedNode;

  // Game rendering variables
  VCNBool                     mFreeLook;
  VCNBool                     mShowHUD;
  VCNBool                     mShowDebugHud;
  VCNFloat                    mCurrentFade;

  // Trigger for fade in fade out
  TimeTrigger                 mFadeTrigger;
};

#endif /* PLAYSTATE_H */
