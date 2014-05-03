///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
/// 
/// @brief Interfaces to many filters
///

#ifndef LOWPASS_H
#define LOWPASS_H

#pragma once

// System includes
#include <deque>

///
/// Low-pass filter of value.
///
template< typename T = float >
class VCNLowPassFilter
{
public:

// Interface

  /// Construct the low pass filtering object
  explicit VCNLowPassFilter(const size_t len = 512)
    : mValue(0)
    , mLength(len)
  {
  }

  /// Input a new value that will be filtered
  const T In(const T& v)
  {
    mBuffer.push_back(v);

    if ( (int)mBuffer.size() > mLength )
      mBuffer.pop_front();

    T sum(0);
    for(size_t i = 0, end = mBuffer.size(); i < end; ++i) 
    {
      sum += mBuffer[i];
    }
    mValue = sum / mBuffer.size();
    return mValue;
  }

  /// Returns the filtering value
  inline const T Out() const { return mValue; }

  /// Returns the filtering value when left operand if float
  inline operator const T() const { return mValue; }

private:

// Data members

  std::deque<T> mBuffer;
  T             mValue;
  int           mLength;
};

///
/// Computes the average delta times for a maximum of T time.
///
class VCNCappedAverageTimeFilter : private VCNLowPassFilter<float>
{
public:

  /// Constructs the filter
  explicit VCNCappedAverageTimeFilter(const float maxtime);

  /// Input a new value that will be filtered
  const float In(const float v);

  /// Returns the filtering value
  inline const float Out() const { return mValue; }

  /// Returns the filtering value when left operand if float
  inline operator const float() const { return mValue; }

private:

  // Data members

  std::deque<float> mBuffer;
  float             mValue;
  float             mMaxTime;
};

#endif // __LOWPASS_H__