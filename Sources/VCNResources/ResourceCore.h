///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
/// 
/// The Vicuna Resource System Core module.
///

#ifndef VICUNA_RESOURCE_CORE
#define VICUNA_RESOURCE_CORE

#pragma once

#include "Resource.h"

#include "VCNCore/Core.h"
#include "VCNNodes/Node.h"

class VCNResource;

class VCNResourceCore : public VCNCore<VCNResourceCore>
{
  VCN_CLASS;

  static const VCNString NO_RESOURCE_STRING;

public:
  VCNResourceCore();
  virtual ~VCNResourceCore();

  // Core module overrides
  virtual VCNBool Initialize();
  virtual VCNBool Uninitialize();
  virtual VCNBool Process(const float elapsedTime);

  // Called to load a resource from its name
  template <class T>
  T* GetResource( const VCNString& resourceName );

  // Called to load a resource from its ID
  template <class T>
  T* GetResource( VCNResID resID );

  // Get the ID of a resource
  VCNResID GetResourceID( const VCNString& resourceName );
  VCNResID GetResourceID( VCNResource* resource );
  const VCNString& GetResourceName( const VCNResID resourceID );

  // Returns the next free ID
  VCNResID GetFreeResourceID();

  // Called to directly add a resource already in memory with a unique generated name
  VCNResID AddResource( VCNResource* resource );

  // Called to directly add a resource already in memory
  VCNResID AddResource( const VCNString& resourceName, VCNResource* resource );

  // Removes a resource without deleting it
  VCNResource* RemoveResource( const VCNResID resourceID );
  VCNResource* RemoveResource( const VCNString& resourceName );

  // Removes and deletes the resource
  VCNBool DeleteResource( const VCNResID resourceID, VCNBool delayed = false );
  VCNBool DeleteResource( const VCNString& resourceName, VCNBool delayed = false );

  // If anyone needs
  const VCNUInt GetResourceCount() const;
    
  // Called to empty the resource core
  virtual void ReleaseAll();

protected:

  // Internal function that finds a resource and checks type with internal RTTI
  VCNResource* GetResource( VCNResID resID, const VCNTypeInfo* classID );

  /// Delete pending resources
  void DeletePending();

protected:

  // Handy type for an iterator onto our name map
  typedef std::map<VCNString,VCNResID>::iterator ResourceNameIterator;

  // Map names to resource IDs
  std::map<VCNString,VCNResID> mResourceNameMap;

  // Our vector of all the resources 
  // (a resource ID is an index into this vector)
  std::vector<VCNResource*> mResources;

  // ID's that aren't being used
  std::vector<VCNResID> mFreeIDs;

  // Resources pending for delayed deletion
  std::vector<VCNResource*> mDelayedDeletion;
};

//-------------------------------------------------------------
inline const VCNUInt VCNResourceCore::GetResourceCount() const
{
  return (VCNUInt)mResources.size();
}

//-------------------------------------------------------------
inline VCNResource* VCNResourceCore::RemoveResource( const VCNString& resourceName )
{
  return RemoveResource( GetResourceID(resourceName) );
}

//-------------------------------------------------------------
inline VCNBool VCNResourceCore::DeleteResource( const VCNString& resourceName, VCNBool delayed )
{
  return DeleteResource( GetResourceID(resourceName), delayed );
}

//-------------------------------------------------------------
template <class T>
inline T* VCNResourceCore::GetResource( const VCNString& resourceName )
{
  return (T*)GetResource( GetResourceID(resourceName), &T::VCNTYPE );
}

//-------------------------------------------------------------
template <class T>
inline T* VCNResourceCore::GetResource( VCNResID resID )
{
  return (T*)GetResource( resID, &T::VCNTYPE );
}

//-------------------------------------------------------------
/// Internal function that finds a resource and checks type
/// with internal RTTI
//-------------------------------------------------------------
inline VCNResource* VCNResourceCore::GetResource( VCNResID resID, const VCNTypeInfo* classID )
{
  VCNResource* resource = NULL;

  // Make sure it's not an invalid request
  if( resID < mResources.size() )
  {
    // Get the resource
    resource = mResources[resID];

    // Make sure the type of the resource is correct
    VCN_ASSERT( resource->GetRealType()->IsA( classID ) );
  }

  return resource;
}

#endif
