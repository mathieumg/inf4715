///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
///
/// @brief A Timer implementation
///

#include "Precompiled.h"
#include "Timer.h"

// Engine include
#include "VCNUtils/Assert.h"

Timer::Timer()
	: mRunning(false)
	, mAccumulatedTime(0)
{
	QueryPerformanceFrequency(&mFrequency);
}

//===========================================================================
// Start a timer.  If it is already running, let it continue running.
// Print an optional message.
void Timer::Start()
{
	// Return immediately if the timer is already running
	if (mRunning)
		return;

	// Set timer status to running and set the start time
	mRunning = true;

#if defined(_WIN32)
	QueryPerformanceCounter(&mStartTime);
#elif defined (LINUX)
	gettimeofday(&mStartTime, 0);
#endif
}

//===========================================================================
// Turn the timer off and start it again from 0.  Print an optional message.
void Timer::Restart()
{
	// Set timer status to running, reset accumulated time, and set start time
	mRunning = true;
	mAccumulatedTime = 0;

#if defined(_WIN32)
	QueryPerformanceCounter(&mStartTime);
#elif defined (LINUX)
	gettimeofday(&mStartTime, 0);
#endif
}

//===========================================================================
// Stop the timer and print an optional message.
void Timer::Stop()
{
	// Compute accumulated running time and set timer status to not running
	if (mRunning) {
		mAccumulatedTime += GetElapsedTime();
	}
	mRunning = false;

}

//===========================================================================
// Print out an optional message followed by the current timer timing.
void Timer::Check(std::ostream& os /*= std::cout*/)
{
	os << "Elapsed time [" << *this << "] seconds\n";

}

//===========================================================================
// Return the total time that the timer has been in the "running"
// state since it was first "started" or last "restarted".  For
// "short" time periods (less than an hour), the actual cpu time
// used is reported instead of the elapsed time.
double Timer::GetElapsedTime() const
{
#if defined(_WIN32)
	LARGE_INTEGER currentTime;
	QueryPerformanceCounter(&currentTime);

	VCN_ASSERT( currentTime.QuadPart > mStartTime.QuadPart );

	return (currentTime.QuadPart - mStartTime.QuadPart) / double(mFrequency.QuadPart);

#elif defined (LINUX)
	struct timeval currentTime;
	gettimeofday(&currentTime, 0);

	const double t1 = (double)mStartTime.tv_sec + (double)mStartTime.tv_usec / (1000 * 1000);
	const double t2 = (double)currentTime.tv_sec + (double)currentTime.tv_usec / (1000 * 1000);
	return t2 - t1;
#endif
}

//===========================================================================
double Timer::GetTime() const
{
	if (mRunning) {
		return mAccumulatedTime + GetElapsedTime();
	}

	// not running
	return mAccumulatedTime;
}

//===========================================================================
// Allow timers to be printed to ostreams using the syntax 'os << t'
// for an ostream 'os' and a timer 't'.  For example, "cout << t" will
// print out the total amount of time 't' has been "running".
inline std::ostream & operator<<(std::ostream& os, const Timer& t)
{
	os << std::setprecision(2) << std::setiosflags(std::ios::fixed)
		<< t.mAccumulatedTime + (t.IsRunning() ? t.GetElapsedTime() : 0);
	return os;
}

//===========================================================================
DelayTimer::DelayTimer()
	: mDelay(0)
{
}

//===========================================================================
DelayTimer::DelayTimer(double delay)
	: mDelay(delay)
{
	Set(delay);
}

//===========================================================================
void DelayTimer::Set(double delay)
{
	mDelay = delay;
	Restart();
}

//===========================================================================
bool DelayTimer::Check() const
{
	if (GetTime() >= mDelay) {
		return true;
	}

	return false;
}
