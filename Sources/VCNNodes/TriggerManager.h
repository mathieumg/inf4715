
///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
///
/// @brief Trigger Manager
///

#ifndef __TRIGGER_MANAGER_H__
#define __TRIGGER_MANAGER_H__

#pragma once

// Engine includes
#include "VCNUtils/Singleton.h"
#include "VCNUtils/Types.h"
#include "VCNUtils/Vector.h"
#include "GameEvent.h"

class Event;
class TriggerNode;
class VCNAabb;

///
/// Class managing the game's multiple triggers.
///
class TriggerManager : public Singleton<TriggerManager>
{
  SINGLETON_DECLARATION( TriggerManager );
public:

  /// Default dtor
  ~TriggerManager();

  /// Create a trigger into space
  TriggerNode* CreateTrigger( const VCNString& triggerName, const VCNAabb& box );

  /// Create a keypress trigger
  TriggerNode* CreateTrigger( const VCNString& triggerName, const VCNUInt8 keycode );

  /// Remove a trigger
  void RemoveTrigger( const VCNString& triggerName );

  /// Attach an event to a trigger
  void AttachEvent( const VCNString& triggerName, GameEvent* effect );

  /// Check if the position is contains in a trigger.
  const VCNBool Test(const Vector3& position);

  /// Check if we entered or leaved a trigger zone.
  const VCNBool Test(const Vector3& from, const Vector3& to);

  /// Check if trigger is activated conditionally with player's position
  const VCNBool Track(const Vector3& from, const Vector3& to);

  /// Check if trigger is activated conditionally with keyboard button
  const VCNBool Track(const VCNUInt8& keycode);

  /// Reset repeatable state of trigger events
  void ResetEvents();

private:

  /// Default ctor (prevent copy)
  TriggerManager();

  typedef std::map<VCNString, TriggerNode*> TriggerList;
  typedef TriggerList::iterator             TriggerListIterator;
  
  TriggerList mTriggerList;
};

#endif // __TRIGGER_MANAGER_H__

//////////////////////////////////////////////////////////////////////////
///
//////////////////////////////////////////////////////////////////////////
