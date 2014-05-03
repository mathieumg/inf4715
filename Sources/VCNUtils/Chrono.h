///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
/// 
/// @brief Implementation of a chronometer.
///
/// @remarks Use macro for simplification
///

#ifndef __CHRONO_H__
#define __CHRONO_H__

#pragma once

#include <Windows.h>

#include "VCNUtils/Macros.h"
#include "VCNUtils/Utilities.h"

#ifndef FINAL

/// Create a unique chronometer in scope
#define CHRONO(_tag) Chrono _chrono##__COUNTER__(_tag)

/// Create a unique chronometer in scope that reports source file and line location
#define CHRONO_INFO(_tag) INFO(_tag); Chrono _chrono##__COUNTER__(_tag)

/// Same as CHRONO, but has another for more signification
#define CHRONO_THIS_METHOD() Chrono _chrono_method##__COUNTER__(__FUNCTION__)

/// Same as CHRONO, but report the average time at the end.
#define CHRONO_AVERAGE(_tag)                                                            \
  static ChronoAverage _chronoAverage(_tag);                                            \
  ChronoAverage::ChronoReport MACRO_CONCAT(_chrono_average_, __COUNTER__)(_tag, &_chronoAverage, false, false)

/// Same as CHRONO_AVERAGE, but ONLY report the average time at the end
#define CHRONO_AVERAGE_METHOD()                                                                            \
  static ChronoAverage _chronoAverageMethod(__FUNCTION__);                                                 \
  ChronoAverage::ChronoReport MACRO_CONCAT(_chrono_average_method, __COUNTER__)(__FUNCTION__, &_chronoAverageMethod, false, false)

/// Same as CHRONO_AVERAGE, but ONLY report the average time at the end
#define CHRONO_TOTAL_METHOD()                                                                        \
  static ChronoTotal _chronoTotalMethod(__FUNCTION__);                                               \
  ChronoTotal::ChronoReport MACRO_CONCAT(_chrono_total_method, __COUNTER__)(__FUNCTION__, &_chronoTotalMethod, false, false)

#else

#define CHRONO(_tag)
#define CHRONO_INFO(_tag)
#define CHRONO_THIS_METHOD()
#define CHRONO_AVERAGE(_tag)
#define CHRONO_AVERAGE_METHOD()
#define CHRONO_TOTAL_METHOD()

#endif

////////////////////////////////////////////////////////////////////////
///  @class Chrono
///  @brief The Chrono class permits the client to optimize methods by 
///         evaluating their running time.
///
///  @remarks <pre>
///            The chronometer is useful when used in scope.
///            e.g. {
///                   Chrono c("test"); 
///                   for(int i = 0; i < 1000000; i++) ;
///                 }
///            this code will return the running time of the code between {}
///            </pre>
////////////////////////////////////////////////////////////////////////
class Chrono
{  
public:

  // Ctor(s) / Dtor

  ////////////////////////////////////////////////////////////////////////
  ///
  ///  @brief Construct a chronometer.
  ///
  ///  @param [in]       Tag                Tag name of the meter for identification.
  ///                                       tag name must not exceed 15 characters.
  ///  @param [in]       Suspend [=false]   Suspend the meter at construction. If so, 
  ///                                       you need to start it manually.
  ///  @param [in]       Report  [=true]    Indicate to report to debugger at destruction
  ///
  ///  This function doesn't return a value
  ////////////////////////////////////////////////////////////////////////
  Chrono(const char* Tag, bool Suspend = false, bool Report = true)
    : mBegClock(0)
    , mEndClock(0)
    , mDiff(0)
    , mReport(Report)
  {
    strncpy(mTag, Tag, STATIC_ARRAY_COUNT(mTag));

    if ( !Suspend )
      Start();
  }

  ////////////////////////////////////////////////////////////////////////
  ///  @brief Stop and destruct the meter.
  ///
  ///  This function doesn't return a value
  ////////////////////////////////////////////////////////////////////////
  virtual ~Chrono()
  {
    Stop();
  }

  // Attributes

  ////////////////////////////////////////////////////////////////////////
  ///  @brief Return the tag name of the meter.
  ///
  ///  @return the tag name internal pointer.
  ////////////////////////////////////////////////////////////////////////
  const char* GetTagName() const { return mTag; }


  ////////////////////////////////////////////////////////////////////////
  ///  @brief Returns the elapsed time
  ///
  ///  @return difference between start and end
  ////////////////////////////////////////////////////////////////////////
  const float GetTime() const { return mDiff; }

  // Operations

  ////////////////////////////////////////////////////////////////////////
  ///  @brief Start the meter.
  ///
  ///  This function doesn't return a value
  ////////////////////////////////////////////////////////////////////////
  void Start()
  {
    mEndClock = 0;
    // Query computer's frequency
    // TODO: use VCNTimer to remove dependency to Windows here
    QueryPerformanceFrequency((LARGE_INTEGER*)&mPerfFrequency);

    // Make the last frame time equal to the current real time
    QueryPerformanceCounter((LARGE_INTEGER*)&mBegClock);
  }

  ////////////////////////////////////////////////////////////////////////
  ///  @brief Stop the meter and output the running time.
  ///
  ///  @return the time difference.
  ////////////////////////////////////////////////////////////////////////
  const float Stop()
  {
    ASSERT(mBegClock != 0 && "Chronometer hasn't been started");

    QueryPerformanceCounter((LARGE_INTEGER*)&mEndClock);

    // Compute the amount of time that have elapsed since last update
    mDiff = float(mEndClock - mBegClock) / float(mPerfFrequency);

    if (mReport)
    {
      char TimeDiffStr[256];
      int Result = sprintf(TimeDiffStr, "[Chrono] : %s : %s\n", mTag, VCN::Eng(mDiff));
      if( Result == -1 )
        return 0; // Failed

      VCNLog << TimeDiffStr;
    }

    return mDiff;
  }

private:

  // Data members

  _int64    mBegClock;        ///< Start time
  _int64    mEndClock;        ///< End time
  _int64    mPerfFrequency;   ///< Chronometer frequency.
  char      mTag[64];         ///< Chronometer identifier.
  float     mDiff;            ///< Cached elapsed time
  bool      mReport;          ///< Indicate to report to debugger
};



////////////////////////////////////////////////////////////////////////
///
///  @class ChronoAverage
///
///  @brief This class serves as a chronometer that gets report from other
///         chronometer to compute the average time.
///
///  @remarks to be used with the macro CHRONO_AVERAGE
///
////////////////////////////////////////////////////////////////////////
class ChronoAverage
{
public:

  ////////////////////////////////////////////////////////////////////////
  ///  @brief Default constructor
  ///
  ///  @param [in]       Tag : Name tag
  ///
  ///  This function doesn't return a value
  ////////////////////////////////////////////////////////////////////////
  ChronoAverage(const char* tag)
    : mHits(0)
    , mTotalTime(0)
  {
    strncpy(mTag, tag, STATIC_ARRAY_COUNT(mTag));
  }

  ////////////////////////////////////////////////////////////////////////
  ///  @brief Default destructor
  ///
  ///  Report compute average time.
  ///
  ///  This function doesn't return a value
  ////////////////////////////////////////////////////////////////////////
  ~ChronoAverage()
  {
    char timeStr[256];
    int Result = sprintf_s(timeStr, STATIC_ARRAY_COUNT(timeStr), 
      "[Chrono Average] : %s : %ss\n", mTag, VCN::Eng(mTotalTime / mHits, 3, 0));
    if( Result > 0 )
    {
      VCNLog << timeStr;
    }
  }

  ////////////////////////////////////////////////////////////////////////
  ///  @brief Gets called when another chronometer object gets destructed.
  ///
  ///  @param [in]       time : time to add to the total time
  ///
  ///  This function doesn't return a value
  ////////////////////////////////////////////////////////////////////////
  void Report(float time)
  {
    mHits++;
    mTotalTime += time;
  }

  ////////////////////////////////////////////////////////////////////////
  ///
  ///  @class ChronoReport
  ///
  ///  @brief Inner class to ChronoAverage to specify a chronometer that reports
  ///         himself to ChronoAverage.
  ///
  ////////////////////////////////////////////////////////////////////////
  class ChronoReport : public Chrono
  {
  public:

    /// Default constructor
    ChronoReport(const char* Tag, ChronoAverage* reportTo, bool Suspend, bool Report)
      : Chrono(Tag, Suspend, Report)
      , mReportee(reportTo)
    {
      ASSERT(mReportee);
    }

    /// Default destructor an notify ChronoAverage how much time it took
    ~ChronoReport()
    {
      mReportee->Report(Stop());
    }

  protected:

  private:

    ChronoReport(const ChronoReport&);
    ChronoReport& operator = (const ChronoReport&);

    ChronoAverage* mReportee; ///< Report time to [mReportee]
  };

private:

  ChronoAverage(const ChronoAverage&);
  ChronoAverage& operator = (const ChronoAverage&);

  char    mTag[64];   ///< Chronometer identifier.
  VCNUInt64  mHits;      ///< The number of hits
  VCNFloat mTotalTime; ///< Total time
};

////////////////////////////////////////////////////////////////////////
///
///  @brief This class serves as a chronometer that gets report from other
///         chronometer to compute the total time passed in the defined scope.
///
///  @remarks to be used with the macro CHRONO_TOTAL*
///
////////////////////////////////////////////////////////////////////////
class ChronoTotal
{
public:

  ////////////////////////////////////////////////////////////////////////
  ///  @brief Default constructor
  ///
  ///  @param [in]       Tag : Name tag
  ////////////////////////////////////////////////////////////////////////
  ChronoTotal(const char* tag)
    : mTotalTime(0)
  {
    strncpy(mTag, tag, STATIC_ARRAY_COUNT(mTag));

    mEndClock = 0;
    QueryPerformanceFrequency((LARGE_INTEGER*)&mPerfFrequency);
    QueryPerformanceCounter((LARGE_INTEGER*)&mBegClock);
  }

  ////////////////////////////////////////////////////////////////////////
  ///  @brief Default destructor
  ///
  ///  Report compute average time.
  ////////////////////////////////////////////////////////////////////////
  ~ChronoTotal()
  {
    QueryPerformanceCounter((LARGE_INTEGER*)&mEndClock);

    // Compute the amount of time that have elapsed since last update
    float totalElapsedTime = float(mEndClock - mBegClock) / float(mPerfFrequency);

    char timeStr[256];
    int Result = sprintf_s(timeStr, STATIC_ARRAY_COUNT(timeStr), 
      "[Chrono Total Time] : %s : %ss (%ss per second)\n", 
      mTag, 
      VCNAnsiString(VCN::Eng(mTotalTime, 3, 0)).c_str(),
      VCNAnsiString(VCN::Eng(mTotalTime / totalElapsedTime, 3, 0)).c_str());
    if( Result > 0 )
    {
      VCNLog << timeStr;
    }
  }

  ////////////////////////////////////////////////////////////////////////
  ///  @brief Gets called when another chronometer object gets destructed.
  ///
  ///  @param [in]       time : time to add to the total time
  ///
  ///  This function doesn't return a value
  ////////////////////////////////////////////////////////////////////////
  void Report(float time)
  {
    mTotalTime += time;
  }

  ////////////////////////////////////////////////////////////////////////
  ///
  ///  @class ChronoReport
  ///
  ///  @brief Inner class to ChronoTotal to specify a chronometer that reports
  ///         himself to ChronoTotal.
  ///
  ////////////////////////////////////////////////////////////////////////
  class ChronoReport : public Chrono
  {
  public:

    /// Default constructor
    ChronoReport(const char* Tag, ChronoTotal* reportTo, bool Suspend, bool Report)
      : Chrono(Tag, Suspend, Report)
      , mReportee(reportTo)
    {
      ASSERT(mReportee);
    }

    /// Default destructor an notify ChronoTotal how much time it took
    ~ChronoReport()
    {
      mReportee->Report(Stop());
    }

  protected:

  private:

    ChronoReport(const ChronoReport&);
    ChronoReport& operator = (const ChronoReport&);

    ChronoTotal* mReportee; ///< Report time to [mReportee]
  };

private:

  ChronoTotal(const ChronoTotal&);
  ChronoTotal& operator = (const ChronoTotal&);

  char       mTag[64];   ///< Chronometer identifier.
  VCNFloat   mTotalTime; ///< Total time

  _int64     mBegClock;        ///< Start time
  _int64     mEndClock;        ///< End time
  _int64     mPerfFrequency;   ///< Chronometer frequency.
};

#endif /* __CHRONO_H__ */
