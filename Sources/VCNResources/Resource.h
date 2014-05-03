///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
/// 
/// The Vicuna Resource base class.
///

#ifndef VICUNA_RESOURCE
#define VICUNA_RESOURCE

#pragma once

#include "VCNCore/Atom.h"
#include "VCNUtils/Constants.h"

class VCNResource : public VCNAtom
{
  VCN_CLASS;

public:

  virtual ~VCNResource();

  // Returns the ID of the resource
  VCNResID GetResourceID() const;
  void     SetResourceID( const VCNResID resID );

  // Accessors to resource version
  VCNFloat GetVersion() const;
  void     SetVersion( const VCNFloat v );

  // Accessors to resource name
  VCNString GetName() const;
  void      SetName( const VCNString& name );

  // Release the resource, detach self from manager
  virtual void Release();

protected:

  VCNResource();

  VCNResID  mResID;
  VCNFloat  mResVersion;
  VCNString mResName;
  
};

//-------------------------------------------------------------
inline VCNResID VCNResource::GetResourceID() const
{
  return mResID;
}

//-------------------------------------------------------------
inline void VCNResource::SetResourceID( const VCNResID resID )
{
  mResID = resID;
}

//-------------------------------------------------------------
inline VCNFloat VCNResource::GetVersion() const
{
  return mResVersion;
}

//-------------------------------------------------------------
inline void VCNResource::SetVersion( const VCNFloat v )
{
  mResVersion = v;
}

//-------------------------------------------------------------
inline VCNString VCNResource::GetName() const
{
  return mResName;
}

//-------------------------------------------------------------
inline void VCNResource::SetName( const VCNString& name )
{
  mResName = name;
}


#endif
