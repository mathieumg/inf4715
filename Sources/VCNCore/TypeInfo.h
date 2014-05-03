///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
/// 
/// @brief The heart of the Vicuna RTTI system
///

#ifndef TYPE_INFO_H
#define TYPE_INFO_H

#include "VCNUtils/Types.h"

//
// Compilation Type ID Generation
//

typedef unsigned long type_id;
static const type_id INVALID_TYPE_ID = static_cast<type_id>(-1);

/// This function returns the next type id found at run-time.
/// This function really needs to be inlined for the 
/// static data type to be unique.
inline const type_id _next_type_id()
{
  static type_id id = 0;
  return ++id;
}

template<typename T>
inline const type_id GetTypeUniqueID()
{
	static type_id id = _next_type_id();
	return id;
}

///
/// This class is responsible to uniquely identify a class. Just like RTTI but in a simpler fashion.
///
template<typename T> 
class TypeID
{
public:

  TypeID()
		: id( GetTypeUniqueID<T>() )
  {
  }

  /// Returns implicitly the type ID
  operator const type_id() const
  {
    return id;
  }

  /// Returns the type id implicitly.
  const type_id operator()() const
  {
    return id;
  }

  /// Returns the type id explicitly.
  const type_id get() const
  {
    return id;
  }

  /// Creates a default instance of type id.
  T* create() const
  {
    return new T();
  }

  /// Creates an instance of type id with one argument.
  template<typename Arg1> T* create(Arg1 arg1) const
  {
    return new T(arg1);
  }

  /// Creates an instance of type id with two arguments.
  template<typename Arg1, typename Arg2> T* create(Arg1 arg1, Arg2 arg2) const
  {
    return new T(arg1, arg2);
  }

  /// Creates an instance of type id with three arguments.
  template<typename Arg1, typename Arg2, typename Arg3> T* create(Arg1 arg1, Arg2 arg2, Arg3 arg3) const
  {
    return new T(arg1, arg2, arg3);
  }

  // NOTE: Add more create(...) variant here...

private:

  type_id id;
};

class VCNTypeInfo
{
public:

  /// Constructs a type info descriptor
  VCNTypeInfo(type_id type, VCNTypeInfo* parent)
  {
    mTypeID = type;
    mParent = parent;
  }

  /// Returns the type ID.
  const type_id GetTypeID() const { return mTypeID; }
  
  /// Returns the type parent info
  const VCNTypeInfo* GetParent() const { return mParent; }

  // Searches upward the class tree
  const bool IsA( const VCNTypeInfo* searchType ) const
  {
    const VCNTypeInfo* currentType = this;
    while( currentType )
    {
      if( currentType == searchType )
        return true;
      else
        currentType = currentType->GetParent();
    }

    return false;
  }

  /// Checks if is of type
  const bool IsA(const type_id typeID) const 
  {
    const VCNTypeInfo* currentType = this;
    while( currentType )
    {
      if( currentType->GetTypeID() == typeID )
        return true;
      else
        currentType = currentType->GetParent();
    }

    return false;
  }

private:
  type_id      mTypeID;
  VCNTypeInfo* mParent;
};

/// Declares the class type info internal members.
/// Usage in header files (.h): VCN_CLASS;
#define VCN_CLASS                                                       \
  public:                                                               \
    static const char* VCNCLASSNAME;                                    \
    static VCNTypeInfo VCNTYPE;                                         \
		static const VCNTypeInfo* Type() { return &VCNTYPE; }               \
    virtual const VCNTypeInfo* GetRealType() const { return &VCNTYPE; } \
    virtual const char* GetTypeName() const { return VCNCLASSNAME; } 

/// Defines base type info.
/// Usage in object files (.cpp): VCN_BASETYPE( VCNAtom );
#define VCN_BASETYPE( clss )                          \
  const char* clss::VCNCLASSNAME = #clss;             \
  VCNTypeInfo clss::VCNTYPE( TypeID<clss>(), NULL );

/// Defines concrete type info.
/// Usage in object files (.cpp) : VCN_TYPE( VCNRenderNode, VCNNode ) ;
#define VCN_TYPE( clss, parclss )                                 \
  const char* clss::VCNCLASSNAME = #clss;                         \
  VCNTypeInfo clss::VCNTYPE( TypeID<clss>(), &parclss::VCNTYPE );

#endif
