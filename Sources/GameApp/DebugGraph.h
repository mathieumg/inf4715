///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
///
/// @brief Debugging graph visualizer interface
///

#ifndef DEBUGGRAPH_H
#define DEBUGGRAPH_H

#pragma once

#include "VCNUI/Widget.h"

class IDebugGraphValue
{
public:

  virtual const VCNFloat32 ReadValue() const = 0;
};

class DebugGraphFPSValue : public IDebugGraphValue
{
};

struct HUDGraphValue
{
  VCNFloat32  time;
  VCNFloat32  frameTime;
  VCNFloat32  DMPS;
};

///
/// This class is responsible to display in-time value like FPS, DMPS, time in 
/// a simple graph.
///
class DebugGraph : public VCNWidget
{
  VCN_CLASS;

public:

  /// Default constructor
  DebugGraph();

  /// Destructor
  virtual ~DebugGraph();

//
// VCNWidget overrides
//

  /// Probes various value to show in the graph.
  virtual void Update(const float elapsedTime) override;

  /// Draws the graph.
  virtual void Draw() const override;

protected:

private:

  typedef std::deque<HUDGraphValue> HUDGraphValueQueue;

  HUDGraphValueQueue  mHUDGraphValueQueue;
  VCNUInt             mFont;
  VCNResID            mBackgroundVertexBufferID;

};


#endif // DEBUGGRAPH_H
