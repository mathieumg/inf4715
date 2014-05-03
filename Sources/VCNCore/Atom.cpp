///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
/// 
/// @brief Base atom object implementation
///

#include "Precompiled.h"
#include "Atom.h"

// Engine includes
#include "VCNCore/TypeInfo.h"
#include "VCNUtils/Assert.h"
 
VCN_BASETYPE( VCNAtom );

///////////////////////////////////////////////////////////////////////
///
/// Default constructor.
///
VCNAtom::VCNAtom()
{
}

///////////////////////////////////////////////////////////////////////
///
/// Default destructor
///
VCNAtom::~VCNAtom()
{
}

///////////////////////////////////////////////////////////////////////
///
/// Adds or replaces the property. 
/// @NOTE Make sure the property value can be copied.
/// 
/// @param propertyID [IN] property ID to match
/// @param value       [IN] value associated to the given property ID
///
/// @return the added property
///
const VCNAny& VCNAtom::AddProperty(const PropertyID& propertyID, const VCNAny& value)
{
  return mProperties[propertyID] = value;
}


///////////////////////////////////////////////////////////////////////
const VCNAny& VCNAtom::SetProperty(const PropertyID& propertyID, const VCNAny& value)
{
  PropertyList::iterator fItr = mProperties.find( propertyID );
  VCN_ASSERT( fItr != mProperties.end() );
  fItr->second = value;
  return fItr->second;
}


///////////////////////////////////////////////////////////////////////
///
/// Simply removes the property if any exists.
/// 
/// @param propertyID  [IN] property ID to be removed
///
void VCNAtom::RemoveProperty(const PropertyID& propertyID)
{
  mProperties.erase( propertyID );
}

///////////////////////////////////////////////////////////////////////
///
/// Checks if the property exists in the set.
/// 
/// @param propertyID  [IN] property to check if it exists.
///
/// @return true if it exists, otherwise false is returned.
///
const bool VCNAtom::HasProperty(const PropertyID& propertyID) const
{
  return mProperties.find( propertyID ) != mProperties.end();
}

///////////////////////////////////////////////////////////////////////
///
/// Returns the property value for the requested property.
/// 
/// @param propertyID  [IN] property name
///
/// @return the value or an exception if the property doesn't exists.
///
const VCNAny& VCNAtom::GetProperty(const PropertyID& propertyID) const
{
  // Check first if the property exists, if not then return an exception.
  VCN_ASSERT( HasProperty(propertyID) );

  return mProperties.find( propertyID )->second;
}
