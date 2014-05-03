///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
/// 
/// @brief Atom base class interface 
///

#ifndef VCNATOM_H
#define VCNATOM_H

#pragma once

// Engine includes
#include "VCNCore/Exception.h"
#include "VCNCore/TypeInfo.h"
#include "VCNUtils/Any.h"
#include "VCNUtils/Assert.h"

// System includes
#include <map>

///
/// The Vicuna Atom is the base element for any class that we
/// want to use in the engine.  It will add basic engine
/// features to any class the derives from it.
///
class VCNAtom
{
  VCN_CLASS;

public:

// Predefined types

  /// Defines property IDs to be string. (Could be ints if performance is needed)
  typedef VCNString PropertyID;

  /// Defines the property set data type.
  typedef std::map<PropertyID, VCNAny>  PropertyList;

// Ctor(s) / Dtor

  /// Default constructor
  VCNAtom();

  /// Default destructor
  virtual ~VCNAtom();

// Interface

  /// Checks if instance is of type T
  template<typename T> const bool Is() const
  {
    const VCNTypeInfo* typeInfo = GetRealType();

    return typeInfo->IsA( &T::VCNTYPE );
  }

  /// Allows downward casting by checking against the down most type.
  /// i.e. :
  ///     VCNNode* node = Factory::Create();
  ///     VCNRenderNode* renderNode = node->Cast<VCNRenderNode>();
  ///     VCN_ASSERT( renderNode != NULL );
  template<typename T> T* Cast() 
  { 
    return const_cast<T*>( static_cast<const VCNAtom*>(this)->Cast<T>() ); 
  }

  /// Downcast the object to is const version.
  template<typename T> const T* Cast() const
  {
#if !defined( FINAL )
    const VCNTypeInfo* currType = GetRealType();

    if( currType->IsA( &T::VCNTYPE ) )
      return static_cast<const T*>( this );
    else
    {
      VCN_ASSERT_FAIL( "Can't cast object to requested type." );
      return NULL;
    }
#else
    // In final builds we just want to return the casted pointer 
    // as we should have tested our game properly.
    // Otherwise, if in this scenario the casting can be null, use TryCast()
    return static_cast<const T*>( this );
#endif
  }

  /// Downcast the object to T and returns NULL if it fails
  template<typename T> T* TryCast() 
  { 
    return const_cast<T*>( static_cast<const VCNAtom*>(this)->TryCast<T>() ); 
  }
  template<typename T> const T* TryCast() const
  {
    const VCNTypeInfo* currType = GetRealType();

    if( currType->IsA( &T::VCNTYPE ) )
      return static_cast<const T*>( this );
    else
    {
      return NULL;
    }
  }

// Property interface

  /// Adds a new metadata to the set.
  const VCNAny& AddProperty(const PropertyID& propertyID, const VCNAny& value = 0);

  /// Removes a metadata from the set.
  void RemoveProperty(const PropertyID& propertyID);

  /// Checks if a given property exists.
  const bool HasProperty(const PropertyID& propertyID) const;

  /// Sets the property value, value must have been added previously
  const VCNAny& SetProperty(const PropertyID& propertyID, const VCNAny& value);

  /// Returns the value of a given property.
  /// If the property doesn't exists, a dummy value if returned.
  const VCNAny& GetProperty(const PropertyID& propertyID) const;

  /// Returns the size of all the properties
  const VCNUInt GetPropertyCount() const { return mProperties.size(); }

private:

  /// Dynamic property list
  PropertyList mProperties;

};

#endif
