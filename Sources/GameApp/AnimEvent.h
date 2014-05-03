///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
///
/// @brief Animation event interface
///

#ifndef ANIM_EVENT_H
#define ANIM_EVENT_H

#pragma once

#if 0

// Project includes
#include "Event.h"

// Engine includes
#include "VCNUtils/Types.h"

class VCNAnimationController;
class VCNAnimCore;

///
/// Anim Event class.
///
class AnimEvent : public Event
{
public:
  AnimEvent(const VCNNodeID& nodeId, const VCNResID& id, 
    const VCNTChar* sound ,bool open, bool repeatable = true);
  virtual ~AnimEvent();
  virtual void CallActions();
protected:
  VCNNodeID         mNodeId;
  VCNResID          mId;
  const VCNTChar*   mSound;
  bool              mOpen;
};

#endif

#endif // ANIM_EVENT_H
