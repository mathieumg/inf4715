///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
///
/// @brief Debugging graphs implementation
///
/// @TODO Could be moved to VCN UI or utility module
///

#include "Precompiled.h"
#include "DebugGraph.h"

#include "VCNCore/CoreTimer.h"
#include "VCNCore/System.h"
#include "VCNUtils/Chrono.h"
#include "VCNRenderer/RenderCore.h"

VCN_TYPE( DebugGraph, VCNWidget );

///////////////////////////////////////////////////////////////////////
DebugGraph::DebugGraph()
{
  VCNRenderCore* renderer = VCNRenderCore::GetInstance();

  // Create a default rendering font
  if( !renderer->CreateFont( VCNTXT("Arial"), 0, false, 8, &mFont) )
  {
    VCN_ASSERT_FAIL( _T("Could not create debug font.") );
  }

  // Get screen information to determine where the debug view will be placed
  const VCNPoint& viewportSize = renderer->GetResolution();
  const float width = 0.18f;
  const float height = 0.20f;
  const float left = 0.75f * viewportSize.x;
  const float right = left + width * viewportSize.x;
  const float top = 0.02f * viewportSize.y;
  const float bottom = top + height * viewportSize.y;

  const VCNColor bgColor(0.65f, 0.65f, 0.65f);
  VT_SCREEN_STRUCT vertices[] =
  {    
    { left,  top,    1, 1, VCNColor( bgColor.R, bgColor.G, bgColor.B, 0.55f ).ToARGB()},
    { right, top,    1, 1, VCNColor( bgColor.R, bgColor.G, bgColor.B, 0.55f ).ToARGB()},
    { left,  bottom, 1, 1, VCNColor( bgColor.R, bgColor.G, bgColor.B, 0.55f ).ToARGB()},
    { right, bottom, 1, 1, VCNColor( bgColor.R, bgColor.G, bgColor.B, 0.55f ).ToARGB()},
  };

  // Create the vertex buffer that will be used to draw the graph background.
  mBackgroundVertexBufferID = renderer->CreateCache(VT_SCREEN, vertices, sizeof(vertices));
}


///////////////////////////////////////////////////////////////////////
DebugGraph::~DebugGraph()
{
}


///////////////////////////////////////////////////////////////////////
void DebugGraph::Update(const float elapsedTime)
{
  // Update graph value
  //
  if ( mHUDGraphValueQueue.size() > VCNCoreTimer::GetInstance()->GetFPS() * 6 )
  {
    mHUDGraphValueQueue.pop_front();
  }

  HUDGraphValue stats;
  stats.time = VCNCoreTimer::GetInstance()->GetTotalElapsed();
  stats.frameTime = static_cast<VCNFloat32>( VCNCoreTimer::GetInstance()->GetFrameElapsedTime() );
  stats.DMPS = VCNSystem::GetInstance()->GetDMPS();

  mHUDGraphValueQueue.push_back( stats );
}


///////////////////////////////////////////////////////////////////////
void DebugGraph::Draw() const
{
  VCNRenderCore* renderer = VCNRenderCore::GetInstance();

  const VCNColor kBorderColor(0.8f, 0.8f, 0.8f);
  const VCNPoint& viewportSize = renderer->GetResolution();
  const float width = 0.18f;
  const float height = 0.20f;
  const float left = 0.75f * viewportSize.x;
  const float right = left + width * viewportSize.x;
  const float top = 0.02f * viewportSize.y;
  const float bottom = top + height * viewportSize.y;

  // Draw background
  renderer->RenderScreenCache(mBackgroundVertexBufferID);
  // Draw borders
  renderer->DrawScreenLine( Vector2(left-1, top-1), Vector2(right+1, top-1), kBorderColor );
  renderer->DrawScreenLine( Vector2(right+1, top-1), Vector2(right+1, bottom+1), kBorderColor );
  renderer->DrawScreenLine( Vector2(right+1, bottom+1), Vector2(left-1, bottom+1), kBorderColor );
  renderer->DrawScreenLine( Vector2(left-1, bottom+1), Vector2(left-1, top-1), kBorderColor );

  // Draw legends
  renderer->RenderText( mFont, right + 5.0f, bottom - 8.0f, 250, 250, 250, 0, VCNTXT("t (s)") );

  if ( mHUDGraphValueQueue.size() > 2 )
  {
    struct GraphFPSMinMax
    {
      const bool operator()(const HUDGraphValue& l, const HUDGraphValue& r) const
      {
        return l.frameTime < r.frameTime;
      }
    };

    struct GraphDMPSMinMax
    {
      const bool operator()(const HUDGraphValue& l, const HUDGraphValue& r) const
      {
        return l.DMPS < r.DMPS;
      }
    };

    const VCNColor kFPSLineColor(0.1f, 1.0f, 0.1f);
    const VCNColor kDMPSLineColor(1.0f, 0.2f, 0.2f);
    const VCNFloat32 timeMin = mHUDGraphValueQueue.front().time;
    const VCNFloat32 timeMax = mHUDGraphValueQueue.back().time;
    const VCNFloat32 timeLength = timeMax - timeMin;
    auto fpsMinMax = std::minmax_element(mHUDGraphValueQueue.begin(), mHUDGraphValueQueue.end(), GraphFPSMinMax());
    auto dmpsMinMax = std::minmax_element(mHUDGraphValueQueue.begin(), mHUDGraphValueQueue.end(), GraphDMPSMinMax());
    const VCNFloat32 fpsMin = fpsMinMax.first->frameTime / 2.0f;
    const VCNFloat32 fpsMax = fpsMinMax.second->frameTime * 2.0f;
    const VCNFloat32 dmpsMin = dmpsMinMax.first->DMPS / 2.0f;
    const VCNFloat32 dmpsMax = dmpsMinMax.second->DMPS * 2.0f + VCN::EPSILON_VCN;

    VCNFloat32 x = left + ((mHUDGraphValueQueue.front().time - timeMin) / timeLength) * width * viewportSize.x;
    VCNFloat32 y = bottom - ((mHUDGraphValueQueue.front().frameTime - fpsMin) / fpsMax) * height * viewportSize.y;
    x = std::min( right, x );
    y = std::max( top, y );
    Vector2 lastFPSPt(x, y);

    y = bottom - ((mHUDGraphValueQueue.front().DMPS - dmpsMin) / dmpsMax) * height * viewportSize.y;
    y = std::max( top, y );
    Vector2 lastDMPSPt(x, y);

    for (HUDGraphValueQueue::const_iterator it = mHUDGraphValueQueue.begin() + 1, end = mHUDGraphValueQueue.end(); it != end; ++it)
    {
      x = left + ((it->time - timeMin) / timeLength) * width * viewportSize.x;
      y = bottom - ((it->frameTime - fpsMin) / fpsMax) * height * viewportSize.y;
      x = std::min( right, x );
      y = std::max( top, y );

      renderer->DrawScreenLine( lastFPSPt, Vector2(x, y), kFPSLineColor );
      lastFPSPt.Set(x, y);

      y = bottom - ((it->DMPS - dmpsMin) / dmpsMax) * height * viewportSize.y;
      y = std::max( top, y );
      renderer->DrawScreenLine( lastDMPSPt, Vector2(x, y), kDMPSLineColor );
      lastDMPSPt.Set(x, y);
    }

    // Draw time axis markers
    //
    const VCNColor kLblTxtColor = VCNColor::FromInteger(250u, 250u, 255u);
    VCNRect timeLabelRect;
    renderer->RenderText( mFont, left - 8.0f, bottom + 6.0f, kLblTxtColor, &timeLabelRect, 
      VCNTXT("%.1f"), mHUDGraphValueQueue.front().time );

    for (int i = 1, markerCount = 10; i < markerCount; ++i)
    {
      float x = i / static_cast<float>( markerCount ) * width * viewportSize.x + left;
      if ( x > timeLabelRect.right + 10.0f )
      {
        float time = timeMin + i / static_cast<float>( markerCount ) * timeLength;
        renderer->DrawScreenLine( Vector2(x, bottom + 5), Vector2(x, bottom - 5), kBorderColor );
        renderer->RenderText( mFont, x - 8.0f, bottom + 6.0f, kLblTxtColor, &timeLabelRect, VCNTXT("%.1f"), time );
      }
    }
    renderer->RenderText( mFont, right - 5.0f, bottom + 6.0f, kLblTxtColor, 0, 
      VCNTXT("%.1f"), mHUDGraphValueQueue.back().time );


    renderer->FlushScreenLines();
  }
}
