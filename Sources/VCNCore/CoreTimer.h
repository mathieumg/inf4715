///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
/// 

#ifndef  VICUNA_BASE_TIMER
#define  VICUNA_BASE_TIMER

#pragma once

#include "Core.h"

#include "VCNUtils/Filters.h"
#include <functional>

class VCNCoreTimer;

///
/// Time triggers can be used as normal timer that calls a callback function
/// when the timer has reach the specified time.
///
class TimeTrigger
{
  friend class VCNCoreTimer;

public:

  //typedef void (*TriggerFunc)(void* clientData, float elapsedTime, bool* repeat);
  typedef std::function<void(void*, float, bool*)> TriggerFunc;

  TimeTrigger(float delay = 0.0f, TriggerFunc func = 0, void* data = 0, bool repeat = false)
    : mDelay(delay)
    , mFuncPtr(func)
    , mFuncData(data)
    , mRepeat(repeat)
    , mElapsedTime(0)
  {
  }

  void Set(float delay, TriggerFunc func, void* data = 0, bool repeat = false)
  {
    mDelay = delay;
    mFuncPtr = func;
    mFuncData = data;
    mRepeat = repeat;
  }

protected:
private:

  float       mDelay;
  void*       mFuncData;
  TriggerFunc mFuncPtr;
  bool        mRepeat;
  float       mElapsedTime;
};

///
/// This abstract class is responsible for time manipulation.
///
class VCNCoreTimer : public VCNCore<VCNCoreTimer>
{
  VCN_CLASS;

public:

  /// Default constructor
  VCNCoreTimer();

  /// Destructor
  virtual ~VCNCoreTimer();

  /*
  TODO:
  GetFixedFPS();
  GetVariableFPS();
  GetFixedElapsedTime()
  GetUpdateElapsedTime()
  GetFrameElapsedTime()
  GetFrameElapsedTimeCPU()
  GetFrameElapsedTimeGPU()
  */

  /// Returns the elapsed total time (unpaused) from the beginning of the application.
  const float GetTotalElapsed() const { return (float)(mTotalElapsed*mFactor); }
  
  /// This time might be variable frame to frame, use carefully.
  const float GetFrameElapsedTime() const { return (float)(mFrameElapsedTime*mFactor); }

  /// Returns the average time frame in the last second.
  const float GetAverageFrameTimePerSecond() const { return mAverageFrameTime.Out(); }

  /// Returns the FPS of the running application.
  const VCNUInt32 GetFPS() const { return mFPS; }

  /// Returns the application time scale (1.0f by default)
  const float GetScale(void) const { return (float)mFactor; }

  /// Sets the application time scale
  void SetScale(float fac = 1.0f) { mFactor = (double)fac; }

  /// Add a time trigger. Triggers are processed each frame.
  void AddTrigger(const VCNString& name, const TimeTrigger& trigger, bool replace = false);

  /// Remove a time trigger.
  void RemoveTrigger(const VCNString& name);

  /// Resets the core timer.
  virtual void Reset();
  
  /// Process a single frame
  virtual void Tick(const VCNFloat64 frameTime);

  /// Initialize the timer
  virtual VCNBool Initialize() override;

  /// Uninitialize the timer
  virtual VCNBool Uninitialize() override;

  /// Processes time triggers.
  virtual VCNBool Process(const float elapsedTime) override;

protected:
  
  typedef std::map<VCNString, TimeTrigger>  TriggerList;

  VCNFloat64                  mFrameElapsedTime;  // Time elapsed since previous frame
  VCNFloat64                  mTotalElapsed;      // Time elapsed since start of app
  VCNFloat64                  mFactor;            // Change core timer speed
  VCNUInt32                   mFrameCount;        // Frame counting variable
  VCNUInt32                   mFPS;               // Last FPS calculated
  VCNFloat64                  mOneSecond;         // One second timer
  TriggerList                 mTriggers;          // User timers
  VCNCappedAverageTimeFilter  mAverageFrameTime;  // Average frame time per seconds
};

#endif
