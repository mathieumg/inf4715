///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
/// 
/// @brief Defines a skeleton for a game.  It is a Core module, but it
///        is the mother of core modules, as it will contain a list of
///        all core modules in the game and update them as necessary.
///

#ifndef VICUNA_GAMEBASE
#define VICUNA_GAMEBASE

// Engine includes
#include "VCNCore/Core.h"

// System includes
#include <list>

class VCNGameBase : public VCNAtom
{
  VCN_CLASS;

public:

  /// Default constructor
  VCNGameBase();

  /// Default destructor
  virtual ~VCNGameBase();

  /// Adds a core module
  inline void AddCoreModule(IVCNCore* mod);

  /// Prepare the game to quit
  void Quit();

  /// Returns true if the user request to quit the processing
  const VCNBool IsQuitting() const { return mRequestQuit; }

  /// Initialize the game.
  virtual VCNBool Initialize();

  /// Process a single frame.
  virtual VCNBool Process(const float elapsedTime);

  /// Render the current frame.
  virtual const VCNBool Render() const;

  /// Uninitialize the game.
  virtual VCNBool Uninitialize();

  /// Prepares the upcoming frame
  virtual void PrepareFrame() const = 0;
  
  /// Render a single frame.
  virtual void RenderFrame() const = 0;

protected:

  /// Called to create game cores.
  virtual const VCNBool CreateModules() = 0;

  /// Initialize modules
  virtual const VCNBool InitModules();

  /// Process modules
  virtual VCNBool ProcessModules(const float elapsedTime);

  /// Release modules
  virtual const VCNBool ReleaseModules();

// Data members

  // Make things easier for ourselves
  typedef std::list<IVCNCore*> CoreList;

  CoreList  mCoreModules;
  VCNBool   mRequestQuit;
};

///////////////////////////////////////////////////////////////////////
inline void VCNGameBase::AddCoreModule(IVCNCore* core)
{
  mCoreModules.push_back(core);
}

#endif