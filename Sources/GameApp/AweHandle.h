///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
///
/// @brief Wraps some Awesomium C objects using handles and ref counting
///

#ifndef AWEHANDLE_H
#define AWEHANDLE_H

// Engine includes
#include "VCNUtils/Assert.h"

// System includes
#include "Awesomium/awesomium_capi.h"

// TODO: Move to VCNUI
template<typename T, void(*Destructor)(T*)>
class AweHandle
{
public:

#if defined( DEBUG )
#define AWE_HANDLE_CONTRACT \
  virtual void __macro_contract__() override {}
#else
#define AWE_HANDLE_CONTRACT
#endif
#define AWE_HANDLE_CONSTRUCTS(CLSS)       \
  AWE_HANDLE_CONTRACT                     \
  CLSS(const CLSS& s)                     \
  : Base(s.handle, s.refCount, s.managed) \
  {                                       \
  }                                       \
  CLSS& operator=(const CLSS& s)          \
  {                                       \
    this->handle   = s.handle;            \
    this->refCount = s.refCount;          \
    this->managed  = s.managed;           \
    AddRef();                             \
    return *this;                         \
  }                                     

#if defined( DEBUG )
  // Ensure the derived class uses the macro AWE_HANDLE_CONSTRUCTS to handle constructs.
  // Add:
  //    AWE_HANDLE_CONSTRUCTS( DerivedHandleClassName )
  // to your derived handle class definition
  // Validation is only done in debug builds
  virtual void __macro_contract__() = 0;
#endif

  typedef AweHandle<T, Destructor> Base;

  ~AweHandle()
  {
    if ( managed )
    {
      if ( Release() == 0 )
      {
        Destructor( handle );
        delete refCount;
      }
    }
  }

  operator T* ()
  {
    return handle;
  }

  operator const T* () const
  {
    return handle;
  }

protected:

  AweHandle(const bool m = true)
    : handle(0)
    , refCount(0)
    , managed(m)
  {
    CreateRef();
  }

  AweHandle(T* h, int* r, bool m)
    : handle(h)
    , refCount(r)
    , managed(m)
  {
    AddRef();
  }

  void CreateRef()
  {
    refCount = new int(1);
  }

  void AddRef()
  {
    VCN_ASSERT( refCount );
    (*refCount)++;
  }

  const int Release()
  {
    VCN_ASSERT( refCount );
    (*refCount)--;
    return *refCount;
  }

  T*    handle;
  int*  refCount;
  bool  managed;
};

class AweString : public AweHandle<awe_string, awe_string_destroy>
{
public:

  AWE_HANDLE_CONSTRUCTS( AweString );

  AweString()
    : Base( false )
  {
    handle = const_cast<awe_string*>( awe_string_empty() );
  }

  AweString(const char* cstr)
    : Base()
  {
    handle = awe_string_create_from_ascii( cstr, strlen(cstr) );
  }

  AweString& operator=(const char* cstr)
  {
    handle = awe_string_create_from_ascii( cstr, strlen(cstr) );
    managed = true;
    return *this;
  }

  ~AweString()
  {
  }
};

class AweValue : public AweHandle<awe_jsvalue, awe_jsvalue_destroy>
{
public:

  AWE_HANDLE_CONSTRUCTS( AweValue );

  AweValue(const char* cstr)
  {
    str = cstr;
    handle = awe_jsvalue_create_string_value( str );
  }

  AweString str;
};

class AweValueArray : public AweHandle<awe_jsarray, awe_jsarray_destroy>
{
public:

  AWE_HANDLE_CONSTRUCTS( AweValueArray );

  AweValueArray(const std::vector<AweValue>& v)
  {
	  BuildFromArray(v);
  }

  AweValueArray(const AweValue& arg1)
  {
	  std::vector<AweValue> v;
	  v.push_back(arg1);
      BuildFromArray(v); 
  }

private:

	void BuildFromArray(const std::vector<AweValue>& v)
	{
		for (auto it = v.begin(); it != v.end(); it++)
		{
			args.push_back( *it );
		}
		handle = awe_jsarray_create( &args[0], args.size() ); 
	}

  std::vector<const awe_jsvalue*> args;
};

#endif // AWEHANDLE_H
