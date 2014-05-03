///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
/// 

#include "Precompiled.h"
#include "ResourceCore.h"

#include "Resource.h"

#include "VCNUtils/StringUtils.h"

VCN_TYPE( VCNResourceCore, VCNCore<VCNResourceCore> ) ;

const VCNString VCNResourceCore::NO_RESOURCE_STRING = VCNTXT("NO_RESOURCE");

//-------------------------------------------------------------
VCNResourceCore::VCNResourceCore()
{
}

//-------------------------------------------------------------
VCNResourceCore::~VCNResourceCore()
{
  ReleaseAll();
}

//-------------------------------------------------------------
/// Called at startup
//-------------------------------------------------------------
void VCNResourceCore::ReleaseAll()
{
  // Empty the name map
  mResourceNameMap.clear();

  // Delete all the resources
  for( VCNUInt i=0; i<mResources.size(); i++ )
  {
    if( mResources[i] )
      delete mResources[i];
  }
  mResources.clear();
  mFreeIDs.clear();
}

//-------------------------------------------------------------
/// Called at startup
//-------------------------------------------------------------
VCNBool VCNResourceCore::Initialize() 
{ 
  return BaseCore::Initialize();
}

//////////////////////////////////////////////////////////////////////////
VCNBool VCNResourceCore::Uninitialize()
{
  DeletePending();
  ReleaseAll();

  return BaseCore::Uninitialize();
}

//////////////////////////////////////////////////////////////////////////
void VCNResourceCore::DeletePending()
{
  // Delete everything that was pending
  while( !mDelayedDeletion.empty() )
  {
    VCNResource* resource = mDelayedDeletion.back();
    delete resource;
    mDelayedDeletion.pop_back();
  }
}

//-------------------------------------------------------------
/// Called every frame
//-------------------------------------------------------------
VCNBool VCNResourceCore::Process(const float elapsedTime) 
{
  DeletePending();

  return true; 
}


//-------------------------------------------------------------
/// Get the ID of a resource by its name
//-------------------------------------------------------------
VCNResID VCNResourceCore::GetResourceID( const VCNString& resourceName )
{
  VCNResID resID = kInvalidResID;

  if( !resourceName.empty() )
  {
    // First lets try to see if we already loaded it
    ResourceNameIterator iter = mResourceNameMap.find( resourceName );

    // If the item was found in the map, then simply return it
    if( iter != mResourceNameMap.end() )
    {
      // Take down the ID
      resID = (*iter).second;
    }
  }

  return resID;
}

//-------------------------------------------------------------
/// Get the ID of a resource by pointer
//-------------------------------------------------------------
VCNResID VCNResourceCore::GetResourceID( VCNResource* resource )
{
  VCNResID resID = kInvalidResID;

  if( resource )
  {
    for( VCNResID i=0; i<mResources.size() && resID==kInvalidResID; i++ )
    {
      if( mResources[i] == resource )
        resID = i;
    }  
  }

  return resID;
}

//-------------------------------------------------------------
/// Called to directly add a resource already in memory
//-------------------------------------------------------------
VCNResID VCNResourceCore::AddResource( const VCNString& resourceName, VCNResource* resource )
{
  // Try to see if we can find it
  VCNResID resID = GetResourceID(resourceName);

  // If it's not in there, then add it
  if( resID == kInvalidResID )
  {
    // Get the ID for our resource and remove it from the availability vector
    resID = GetFreeResourceID();
    mFreeIDs.pop_back();

    // Now set up our resource in its spot
    mResources[resID] = resource;
    mResourceNameMap[resourceName] = resID;
    resource->SetResourceID( resID );
    resource->SetName( resourceName );
  }
  else
  {
    VCN_ASSERT_FAIL( _T("Resource collision [%s]!  Same name used twice..."), resourceName.c_str() );
  }

  return resID;
}



///////////////////////////////////////////////////////////////////////
VCNResID VCNResourceCore::AddResource(VCNResource* resource)
{
  static VCNUInt32 sNextID = 0;
  const VCNString uniqueName = StringBuilder() << resource->GetTypeName() << "_" << sNextID++;
  return AddResource( uniqueName, resource );
}



//-------------------------------------------------------------
// Removes the resource and then deletes it to free the mem.
//-------------------------------------------------------------
VCNBool VCNResourceCore::DeleteResource( const VCNResID resourceID, VCNBool delayed )
{
  VCNResource* resource = RemoveResource( resourceID );

  if( resource )
  {
    if( delayed )
      mDelayedDeletion.push_back( resource );
    else
      delete resource;
  }

  return (resource != NULL);
}

//-------------------------------------------------------------
/// Removes the resource (does not free memory)
//-------------------------------------------------------------
VCNResource* VCNResourceCore::RemoveResource( const VCNResID resourceID )
{
  // Get the resource
  VCNResource* resource = GetResource<VCNResource>( resourceID );
  if( resource )
  {
    // Erase the resource from the map
    mResourceNameMap.erase( resource->GetName() );

    // Clear that resource in the resource vector
    mResources[resourceID] = NULL;

    // Add it to the list of free IDs
    mFreeIDs.push_back(resourceID);
  }

  // Return the resource's pointer
  return resource;
}

//-------------------------------------------------------------
/// Slow as molasses, not recommended.
//-------------------------------------------------------------
const VCNString& VCNResourceCore::GetResourceName( const VCNResID resourceID )
{
  // Have an iterator on the first element of the map
  ResourceNameIterator iter = mResourceNameMap.begin();
  while( iter != mResourceNameMap.end() )
  {
    // If we found'em, return'em
    if( (*iter).second == resourceID )
      return (*iter).first;

    ++iter;
  }

  return NO_RESOURCE_STRING;
}

//-------------------------------------------------------------
/// Return the first free resource ID
//-------------------------------------------------------------
VCNResID VCNResourceCore::GetFreeResourceID()
{
  VCNResID resourceID = kInvalidResID;

  if( mFreeIDs.empty() )
  {
    // Create an empty spot and return it
    resourceID = mResources.size();
    mResources.push_back(NULL);
    mFreeIDs.push_back( resourceID );
  }
  else
  {
    resourceID = mFreeIDs.back();
  }

  return resourceID;
}

