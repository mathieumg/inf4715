///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
/// 
/// @brief Defines core components of the engine. Core components are
///        initialized at startup and updated at every frame.
///

#ifndef VCNCORE_H
#define VCNCORE_H

#pragma once

#include "VCNCore/Atom.h"
#include "VCNCore/TypeInfo.h"
#include "VCNUtils/Assert.h"
#include "VCNUtils/TemplateUtils.h"

///
/// Simple interface used to register cores in the game runtime.
/// NOTE: The templated VCNCore based class can fill this polymorphic behavior.
/// 
class IVCNCore
{
public:

  virtual ~IVCNCore() = 0 {}

  /// Called at startup
  virtual VCNBool Initialize() = 0;

  /// Called when the app closes
  virtual VCNBool Uninitialize() = 0;

  /// Called every frame
  virtual VCNBool Process(const float elapsedTime) = 0;

};

///
/// Base service class for Vicuna cores. The service offer a common way to 
/// gets unique instance of cores and contracts how core gets initialized and 
/// uninitialized.
///
template<typename T>
class VCNCore : public IVCNCore, public VCNAtom, public VCNNonCopyable
{
  VCN_CLASS;

public:

  typedef VCNCore<T> BaseCore;

  /// Checks if the core has been created.
  /// If false is returned there's many chances 
  /// the core will never be created afterward.
  static bool IsInstantiated() { return sInstance != 0; }

  /// Returns the unique core instance.
  static T* GetInstance();

  /// Release the unique instance
  static void Release();

// Interface

  /// Check if the core have been initialized
  bool IsInitialized(void) const;

// IVCNCore overrides

  /// Called at startup
  virtual VCNBool Initialize() override = 0;

  /// Called when the app closes
  virtual VCNBool Uninitialize() override  = 0;

  /// Called every frame
  virtual VCNBool Process(const float elapsedTime) override;
  
protected:

  ///
  /// NOTE: It's best practice to call CreateCore to create a new core.
  ///
  VCNCore()
    : mInitialized(false)
  {
    // ASSERT if created more than once
    VCN_ASSERT( sInstance == nullptr );

    int offset = (int)(T*)1 - (int)(VCNCore<T>*)(T*)1;
    sInstance = (T*)((int)this+offset);

    // Make sure the core gets released when the the application exits.
    atexit( Release );
  }

  /// Reset the static instance
  virtual ~VCNCore()
  {    
    VCN_ASSERT_MSG( !mInitialized, "Core may not have been properly uninitialized." );

    sInstance = 0;
  }  

private:

  static T* sInstance;

  /// Prevent copy and assignment
  VCNCore(VCNCore const&);
  VCNCore& operator=(VCNCore const&);

// Data members

  /// Indicates that the core has been properly initialized.
  VCNBool mInitialized;
};


///////////////////////////////////////////////////////////////////////
///
/// Free function to create cores.
///
/// @return the core instance
///
template<typename T> inline static T* CreateCore()
{
  T* newInstance = new T();
  VCN_ASSERT( newInstance );
  return static_cast<T*>( newInstance );
}


///////////////////////////////////////////////////////////////////////
template<typename T> 
inline T* VCNCore<T>::GetInstance()
{
  VCN_ASSERT_MSG( sInstance, VCNTXT("Core hasn't been initialized") );
  return sInstance;
}


///////////////////////////////////////////////////////////////////////
template<typename T>
inline void VCNCore<T>::Release()
{
  delete sInstance;
  sInstance = 0;
}


///////////////////////////////////////////////////////////////////////
template<typename T>
inline bool VCNCore<T>::IsInitialized(void) const
{
  return mInitialized;
}


///////////////////////////////////////////////////////////////////////
template<typename T>
VCNBool VCNCore<T>::Initialize()
{
  VCN_ASSERT_MSG( !mInitialized, VCNTXT("Core seems to be already initialized!") );

  mInitialized = true; 
  return mInitialized;
}


///////////////////////////////////////////////////////////////////////
template<typename T>
VCNBool VCNCore<T>::Uninitialize()
{
  mInitialized = false; 
  return !mInitialized;
}


///////////////////////////////////////////////////////////////////////
template<typename T>
VCNBool VCNCore<T>::Process(const float elapsedTime)
{
  return true;
}

template <typename T> T* VCNCore<T>::sInstance = NULL;
template <typename T> const char* VCNCore<T>::VCNCLASSNAME( "VCNCore" );
template <typename T> VCNTypeInfo VCNCore<T>::VCNTYPE( TypeID< VCNCore<T> >(), &VCNAtom::VCNTYPE );

#endif
