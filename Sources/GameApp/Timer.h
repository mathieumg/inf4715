#ifndef TIMER_H
#define TIMER_H

///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
///
/// @brief Timer interface
///

#pragma once

class Timer
{
public:
   // 'running' is initially false.  A timer needs to be explicitly started
   // using 'start' or 'restart'
   Timer();

   /// Check if the timer is running
   bool IsRunning() const { return mRunning; }

   /// Return the elapsed seconds
   double GetTime() const;

   /// Start the timer
   void Start();

   /// Restart the timer
   void Restart();

   /// Stop the timer
   void Stop();

   /// Display the timer state.
   void Check(std::ostream& os = std::cout);

private:

   /// Get the elapsed time on the last batch (start/stop)
   double GetElapsedTime() const;

   friend std::ostream & operator<<(std::ostream& os, const Timer& t);

// Data members

   bool     mRunning;
   double   mAccumulatedTime; ///> Accumulated time in seconds

#if defined(_WIN32)
   LARGE_INTEGER  mFrequency;
   LARGE_INTEGER  mStartTime;    ///< Start time of the timer.
#elif defined(LINUX)
   timeval  mStartTime;
#else
   #error OS Not supported!
#endif
};

class DelayTimer : private Timer
{
public:
   DelayTimer();

   DelayTimer(double delay);

   void Set(double delay);

   bool Check() const;

private:
   double mDelay;
};

#endif // TIMER_H
