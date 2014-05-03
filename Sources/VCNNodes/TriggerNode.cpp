///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
///
/// @brief Trigger Implementation
///

#include "Precompiled.h"
#include "TriggerNode.h"

//#include "InputManager.h"

#include "VCNUtils/Ray.h"
#include "VCNUtils/Utilities.h"
#include "VCNRenderer/RenderCore.h"

//////////////////////////////////////////////////////////////////////////
TriggerNode::TriggerNode( VCNNodeID nodeID )
: VCNNode(nodeID)
, mShowAABB(false)
{
  mDebugLineColor.R = VCN::Random(0.2f, 1.0f);
  mDebugLineColor.G = VCN::Random(0.2f, 1.0f);
  mDebugLineColor.B = VCN::Random(0.2f, 1.0f);
  mDebugLineColor.A = 1.0f;
}

//////////////////////////////////////////////////////////////////////////
TriggerNode::~TriggerNode()
{
}

//////////////////////////////////////////////////////////////////////////
void TriggerNode::AttachEvent( GameEvent* effect, CollisionFlag how /*= CF_TRUE*/ )
{
  mEvents.push_back( EventTrigger(how, EventPointer(effect)) );
}

//////////////////////////////////////////////////////////////////////////
void TriggerNode::AttachEvent( EventPointer effect, CollisionFlag how /*= CF_TRUE*/ )
{
  mEvents.push_back( EventTrigger(how, effect) );
}

//////////////////////////////////////////////////////////////////////////
void TriggerNode::Activate(CollisionFlag cf /*= CF_TRUE*/)
{
  std::for_each(mEvents.begin(), mEvents.end(), [cf](EventList::value_type& triggerEvent)
  {
    if ((triggerEvent.how & cf) == cf)
    {
      triggerEvent.event->CallActions();
    }
  });
}

//////////////////////////////////////////////////////////////////////////
const VCNBool TriggerNode::Test( const Vector3& position )
{
  return mBBox.Intersects( position );
}

//////////////////////////////////////////////////////////////////////////
const VCNBool TriggerNode::Test( const Vector3& from, const Vector3& to, CollisionFlag& flag )
{
  flag = CF_NO;

  if ( !mBBox.Intersects(from) && mBBox.Intersects(to) )
  {
    flag = CF_ENTER;
  }
  else if ( mBBox.Intersects(from) && !mBBox.Intersects(to) )
  {
    flag = CF_LEAVE;
  }

  return flag != CF_NO;
}

//////////////////////////////////////////////////////////////////////////
void TriggerNode::Render() const
{
#ifdef DEBUG
	VCNRenderCore* renderer = VCNRenderCore::GetInstance();

	const Vector3 min = mBBox.vcMin, max = mBBox.vcMax;

	renderer->DrawLine(min, Vector3(max.x, min.y, min.z), mDebugLineColor);
	renderer->DrawLine(min, Vector3(min.x, max.y, min.z), mDebugLineColor);
	renderer->DrawLine(Vector3(max.x, min.y, min.z), Vector3(max.x, max.y, min.z), mDebugLineColor);
	renderer->DrawLine(Vector3(min.x, max.y, min.z), Vector3(max.x, max.y, min.z), mDebugLineColor);

	renderer->DrawLine(Vector3(min.x, min.y, max.z), Vector3(max.x, min.y, max.z), mDebugLineColor);
	renderer->DrawLine(Vector3(min.x, min.y, max.z), Vector3(min.x, max.y, max.z), mDebugLineColor);
	renderer->DrawLine(Vector3(max.x, min.y, max.z), Vector3(max.x, max.y, max.z), mDebugLineColor);
	renderer->DrawLine(Vector3(min.x, max.y, max.z), Vector3(max.x, max.y, max.z), mDebugLineColor);

	renderer->DrawLine(Vector3(min.x, min.y, min.z), Vector3(min.x, min.y, max.z), mDebugLineColor);
	renderer->DrawLine(Vector3(min.x, max.y, min.z), Vector3(min.x, max.y, max.z), mDebugLineColor);
	renderer->DrawLine(Vector3(max.x, min.y, min.z), Vector3(max.x, min.y, max.z), mDebugLineColor);
	renderer->DrawLine(Vector3(max.x, max.y, min.z), Vector3(max.x, max.y, max.z), mDebugLineColor);
#endif
  VCNNode::Render();
}

//////////////////////////////////////////////////////////////////////////
void TriggerNode::Process(const float elapsedTime)
{
  //mShowAABB = InputManager::GetInstance().IsKeyPressed(KEY_V);
}

//////////////////////////////////////////////////////////////////////////
void TriggerNode::ResetEvents()
{
  std::for_each(mEvents.begin(), mEvents.end(), [](EventList::value_type& triggerEvent)
  {
    triggerEvent.event->SetActivation(false);
  });
}
