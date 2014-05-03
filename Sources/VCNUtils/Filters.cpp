///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
/// 
/// @brief Implementation of various filters
///

#include "Precompiled.h"
#include "Filters.h"

#include <algorithm>

///////////////////////////////////////////////////////////////////////
VCNCappedAverageTimeFilter::VCNCappedAverageTimeFilter(const float maxtime)
  : VCNLowPassFilter(32)
  , mValue(0)
  , mMaxTime(maxtime)
{

}

///////////////////////////////////////////////////////////////////////
const float VCNCappedAverageTimeFilter::In(const float v)
{
  mBuffer.push_back( VCNLowPassFilter::In( v ) );

  float total = 0.0f;
  std::for_each(mBuffer.begin(), mBuffer.end(), [&total](const float v) { total += v; });

  while ( total > mMaxTime && !mBuffer.empty() )
  {
    total -= mBuffer.front();
    mBuffer.pop_front();
  }

  if ( mBuffer.empty() )
    return 0.0f;

  mValue = total / mBuffer.size();
  return mValue;
}
