
///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
///
/// @brief Sound Event
///

#ifndef __SOUND_EVENT_H__
#define __SOUND_EVENT_H__

#pragma once

#include "Event.h"
#include "VCNNodes\GameEvent.h"

class VCN2DSoundEmitter;
class VCN3DSoundEmitter;

////////////////////////////////////////////////////////////////////////
/// @class SoundEvent
/// @brief Sound Event class.
///
/// @author Marc-Alexandre Gauvreau
/// @date 14/02/2010
////////////////////////////////////////////////////////////////////////
class SoundEvent : public GameEvent
{
public:
  SoundEvent(VCN2DSoundEmitter* soundEmitter, bool repeatable = true);
  SoundEvent(VCN3DSoundEmitter* soundEmitter, bool repeatable = true);
  virtual ~SoundEvent();
  virtual void CallActions();

protected:
  VCN2DSoundEmitter* m2DSoundEmitter;
  VCN3DSoundEmitter* m3DSoundEmitter;
};

#endif // __SOUND_EVENT_H__

//////////////////////////////////////////////////////////////////////////
///
//////////////////////////////////////////////////////////////////////////
