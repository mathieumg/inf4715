
///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
///
/// @brief Trigger Manager Implementation
///

#include "Precompiled.h"
#include "TriggerManager.h"

#include "TriggerNode.h"

#include "VCNUtils/Utilities.h"
#include "VCNNodes/NodeCore.h"

////////////////////////////////////////////////////////////////////////
///
/// @fn TriggerManager::TriggerManager()
///
/// Constructor for the TriggerManager class.
///
/// @return TriggerManager
///
////////////////////////////////////////////////////////////////////////
TriggerManager::TriggerManager()
: mTriggerList()
{
}

////////////////////////////////////////////////////////////////////////
///
/// @fn TriggerManager::~TriggerManager()
///
/// Destructor for the TriggerManager class.
///
/// @return void
///
////////////////////////////////////////////////////////////////////////
TriggerManager::~TriggerManager()
{
}

////////////////////////////////////////////////////////////////////////
///
/// @fn TriggerManager::CreateTrigger( const VCNString triggerName, const VCNAabb& box )
///
/// Method creating a new trigger with bounding box activation.
///
/// @param[in] triggerName : the name of the trigger
/// @param[in] box : the bounding box where the event is triggered
///
/// @return void
///
////////////////////////////////////////////////////////////////////////
TriggerNode* TriggerManager::CreateTrigger( const VCNString& triggerName, const VCNAabb& box )
{
  VCNNodeCore* nodeCore = VCNNodeCore::GetInstance();

  // Create the trigger node and attach it to the main scene
  TriggerNode* newTriggerNode = nodeCore->CreateNode<TriggerNode>();
  nodeCore->GetRootNode()->AttachChild( newTriggerNode->GetNodeID() );

  // Set trigger attributes
  newTriggerNode->SetTag( triggerName );
  newTriggerNode->SetBBox( box );

  // Add the new trigger to the managed trigger list
  mTriggerList[triggerName] = newTriggerNode;

  return newTriggerNode;
}

////////////////////////////////////////////////////////////////////////
///
/// @fn TriggerManager::RemoveTrigger( const VCNString triggerName )
///
/// Method removing a trigger.
///
/// @param[in] triggerName : the name of the trigger
///
/// @return void
///
////////////////////////////////////////////////////////////////////////
void TriggerManager::RemoveTrigger( const VCNString& triggerName )
{
  TriggerListIterator fItr = mTriggerList.find( triggerName );
  if ( fItr !=  mTriggerList.end() )
  {
    VCNNodeCore::GetInstance()->GetRootNode()->RemoveChild( fItr->second->GetNodeID() );

    delete fItr->second;
    mTriggerList.erase( fItr );
  }
}

////////////////////////////////////////////////////////////////////////
///
/// @fn TriggerManager::AttachEvent( Event* effect, const VCNString triggerName)
///
/// Method attaching an event to a trigger.
///
/// @param[in] effect : the event to be attached
/// @param[in] triggerName : the name of the trigger
///
/// @return void
///
////////////////////////////////////////////////////////////////////////
void TriggerManager::AttachEvent( const VCNString& triggerName, GameEvent* effect )
{
  TriggerListIterator fItr = mTriggerList.find( triggerName );

  VCN_ASSERT_MSG( fItr != mTriggerList.end(), VCNTXT("Trigger doesn't exists") );

  fItr->second->AttachEvent( effect );
}

//////////////////////////////////////////////////////////////////////////
const VCNBool TriggerManager::Test( const Vector3& position )
{
  VCNBool hit = false;

  std::for_each(mTriggerList.begin(), mTriggerList.end(), [&hit, &position](TriggerList::value_type &triggerPair)
  {
    TriggerNode* node = triggerPair.second;

    hit |= node->Test(position);
  });

  return hit;
}

//////////////////////////////////////////////////////////////////////////
const VCNBool TriggerManager::Test( const Vector3& from, const Vector3& to )
{
  VCNBool hit = false;

  std::for_each(mTriggerList.begin(), mTriggerList.end(), [&hit, &from, &to](TriggerList::value_type &triggerPair)
  {
    TriggerNode* node = triggerPair.second;

    TriggerNode::CollisionFlag flags;
    hit |= node->Test(from, to, flags);
  });

  return hit;
}

//////////////////////////////////////////////////////////////////////////
const VCNBool TriggerManager::Track( const Vector3& from, const Vector3& to )
{
  VCNBool triggered = false;

  std::for_each(mTriggerList.begin(), mTriggerList.end(), [&triggered, &from, &to](TriggerList::value_type &triggerPair)
  {
    TriggerNode* node = triggerPair.second;

    TriggerNode::CollisionFlag flags;
    const VCNBool in = node->Test(from, to, flags);
    if ( in )
    {
      triggered = true;
      node->Activate(flags);
    }
  });

  return triggered;
}

//////////////////////////////////////////////////////////////////////////
void TriggerManager::ResetEvents()
{
  std::for_each(mTriggerList.begin(), mTriggerList.end(), [](TriggerList::value_type &triggerPair)
  {
    TriggerNode* node = triggerPair.second;

    node->ResetEvents();
  });
}

