///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
///
/// @brief Trigger node interface
///

#ifndef __TRIGGERNODE_H__
#define __TRIGGERNODE_H__

#pragma once

#include "Event.h"

#include "VCNNodes/Node.h"
#include "VCNUtils/Aabb.h"
#include "VCNUtils/RenderTypes.h"

#include <memory>
#include "GameEvent.h"

class TriggerManager;

///
/// Class encapsulating the game's triggers.
///
class TriggerNode : private VCNNode
{
  friend class TriggerManager;

public:

  enum CollisionFlag {
    CF_NO      = 0,
    CF_ALL     = 1 << 1,
    CF_ENTER   = 1 << 2,
    CF_LEAVE   = 1 << 3,
  };

// Predefined types
  typedef std::shared_ptr<GameEvent>  EventPointer;
 
  struct EventTrigger
  {
    EventTrigger(CollisionFlag h, EventPointer ptr)
      : how(h)
      , event(ptr)
    {
    }

    CollisionFlag   how;
    EventPointer    event;
  };

  typedef std::vector<EventTrigger> EventList;
  typedef EventList::iterator       EventListIterator;

  /// Default dtor
  virtual ~TriggerNode();

  virtual void Render() const override;
  virtual void Process(const float elapsedTime) override;

  /// Returns the bounding box of the trigger
  const VCNAabb& GetBBox() const { return mBBox; }

  /// Sets the bounding box volume of the trigger
  void SetBBox(const VCNAabb& bbox) { mBBox = bbox; };

  /// Returns the key code of the trigger
  const VCNUInt8  GetKeycode() const { return mKeycode; }

  /// Sets the key code used to launch the trigger (if any)
  void SetKeycode(const VCNUInt8& keycode) { mKeycode = keycode; };

  /// Attach an event that will triggered when the trigger gets active
  /// The effect pointer will get managed buy the trigger
  void AttachEvent( GameEvent* effect, CollisionFlag how = CF_ALL );

  /// Attach a shared pointer event
  void AttachEvent( EventPointer effect, CollisionFlag how = CF_ALL );

  /// Test the trigger for collision
  const VCNBool Test(const Vector3& position);

  /// Test the trigger if we enter or leave
  const VCNBool Test(const Vector3& from, const Vector3& to, CollisionFlag& flag);

  /// Activate trigger events
  void Activate(CollisionFlag cf = CF_ALL);

  /// Reset activation state of events
  void ResetEvents();

private:

  /// Default ctor
  TriggerNode( VCNNodeID nodeID );
  friend class VCNNodeCore;

  VCNAabb   mBBox;
  VCNUInt8     mKeycode;
  EventList mEvents;
  VCNBool   mShowAABB;
  VCNColor  mDebugLineColor;
};

#endif // __TRIGGER_H__

//////////////////////////////////////////////////////////////////////////
///
//////////////////////////////////////////////////////////////////////////
