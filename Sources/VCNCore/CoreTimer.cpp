///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
/// 

#include "Precompiled.h"
#include "CoreTimer.h"

VCN_TYPE( VCNCoreTimer, VCNCore<VCNCoreTimer> ) ;

////////////////////////////////////////////////////////////////////////
/// Default constructor. 
////////////////////////////////////////////////////////////////////////
VCNCoreTimer::VCNCoreTimer() 
: mFrameElapsedTime( 0.0 )
, mTotalElapsed( 0.0 )
, mFactor( 1.0 )
, mFrameCount(0)
, mFPS(0)
, mOneSecond(0)
, mAverageFrameTime(1.0f)
{
}


////////////////////////////////////////////////////////////////////////
/// Destructor. 
////////////////////////////////////////////////////////////////////////
VCNCoreTimer::~VCNCoreTimer()
{
}


///////////////////////////////////////////////////////////////////////
VCNBool VCNCoreTimer::Initialize()
{
  if ( !BaseCore::Initialize() )
    return false;

  Reset();

  return true;
}


///////////////////////////////////////////////////////////////////////
VCNBool VCNCoreTimer::Uninitialize()
{
  Reset();

  return BaseCore::Uninitialize();
}

////////////////////////////////////////////////////////////////////////
///
/// Add a trigger to be processed.
///
/// @param  name    Unique name of the trigger
/// @param  trigger Trigger properties
/// @param  replace Replace trigger if currently exists
///
////////////////////////////////////////////////////////////////////////
void VCNCoreTimer::AddTrigger( const VCNString& name, const TimeTrigger& trigger, bool replace /*= false */ )
{
  TriggerList::iterator fItr = mTriggers.find(name);
  if ( replace || fItr == mTriggers.end() )
  {
    mTriggers[name] = trigger;
  }
  else
  {
    VCN_ASSERT( !"Trigger can't be replaced" );
  }
}


////////////////////////////////////////////////////////////////////////
/// 
/// Removes the trigger described by name. 
///
/// @param  name  name of the trigger to be removed. 
///
////////////////////////////////////////////////////////////////////////
void VCNCoreTimer::RemoveTrigger( const VCNString& name )
{
  TriggerList::iterator fItr = mTriggers.find(name);
  if ( fItr != mTriggers.end() )
  {
    mTriggers.erase(fItr);
  }
}


////////////////////////////////////////////////////////////////////////
/// 
/// Check for triggers that has expired and needs to be ran.
///
/// @return always true.
///
////////////////////////////////////////////////////////////////////////
VCNBool VCNCoreTimer::Process(const float elapsedTime)
{
  TriggerList::iterator itr = mTriggers.begin(), 
                        end = mTriggers.end();
  while ( itr != end )
  {
    itr->second.mElapsedTime += elapsedTime;
    if (itr->second.mDelay <= itr->second.mElapsedTime)
    {
      (itr->second.mFuncPtr)(itr->second.mFuncData, itr->second.mElapsedTime, &itr->second.mRepeat);
      itr->second.mElapsedTime = 0;

      if( !itr->second.mRepeat )
      {
        mTriggers.erase( itr++ );
      }
      else 
      {
        ++itr;
      }
    }
    else
    {
      ++itr;
    }
  }

  return true;
}


////////////////////////////////////////////////////////////////////////
///
/// Resets this object. Useful when the core timer has been initialized too early and the next 
/// Process call is far away.
///
////////////////////////////////////////////////////////////////////////
void VCNCoreTimer::Reset()
{
  mFrameElapsedTime = 0;
  mTotalElapsed = 0;
  mFrameCount = 0;
  mOneSecond = 0;
}


///////////////////////////////////////////////////////////////////////
///
/// Updates the frame timer info, like current FPS, elapsed time, etc.
/// 
/// @param frameTime [IN] elapsed time since last frame.
///
void VCNCoreTimer::Tick(const VCNFloat64 frameTime)
{
  // Low-pass filter on elapsed time.
  mFrameElapsedTime = frameTime;
  mTotalElapsed += mFrameElapsedTime;

  // Calculate FPS
  ++mFrameCount;
  mOneSecond += mFrameElapsedTime;
  if (mOneSecond >= 1.0f)
  {
    mFPS = mFrameCount;

    mFrameCount = 0;
    mOneSecond -= 1.0f;
  }

  mAverageFrameTime.In( static_cast<float>(frameTime) );
}
