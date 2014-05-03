/*
* (C) Copyright Christopher Diggins 2005
* (C) Copyright Pablo Aguilar 2005
* (C) Copyright Kevlin Henney 2001
*
* Distributed under the Boost Software License, Version 1.0. (See
* accompanying file LICENSE_1_0.txt or copy at
* http://www.boost.org/LICENSE_1_0.txt)  
*/

#ifndef VCNANY_H
#define VCNANY_H

#include <stdexcept>
#include <typeinfo>
#include <algorithm>

struct bad_any_cast : std::bad_cast {
  bad_any_cast(const std::type_info& src, const std::type_info& dest)
    : from(src.name()), to(dest.name())
  { }
  virtual const char* what() { 
    return "bad cast"; 
  }
  const char* from;
  const char* to;
};

namespace any_detail
{
  // function pointer table
  struct fxn_ptr_table {
    const std::type_info& (*get_type)();
    void (*static_delete)(void**);
    void (*clone)(void* const*, void**);
    void (*move)(void* const*,void**);
  };

  // static functions for small value-types 
  template<bool is_small>
  struct fxns
  {
    template<typename T>
    struct type {
      static const std::type_info& get_type() { 
        return typeid(T); 
      }
      static void static_delete(void** x) { 
        reinterpret_cast<T*>(x)->~T(); 
      }
      static void clone(void* const* src, void** dest) { 
        new(dest) T(*reinterpret_cast<T const*>(src)); 
      }
      static void move(void* const* src, void** dest) { 
        reinterpret_cast<T*>(dest)->~T(); 
        *reinterpret_cast<T*>(dest) = *reinterpret_cast<T const*>(src); 
      }
    };
  };

  // static functions for big value-types (bigger than a void*)
  template<>
  struct fxns<false>
  {
    template<typename T>
    struct type {
      static const std::type_info& get_type() { 
        return typeid(T); 
      }
      static void static_delete(void** x) { 
        delete(*reinterpret_cast<T**>(x)); 
      }
      static void clone(void* const* src, void** dest) { 
        *dest = new T(**reinterpret_cast<T* const*>(src)); 
      }
      static void move(void* const* src, void** dest) { 
        (*reinterpret_cast<T**>(dest))->~T(); 
        **reinterpret_cast<T**>(dest) = **reinterpret_cast<T* const*>(src); 
      }
    };
  };

  template<typename T>
  struct get_table 
  {
    static const bool is_small = sizeof(T) <= sizeof(void*);

    static fxn_ptr_table* get()
    {
      static fxn_ptr_table static_table = {
        fxns<is_small>::template type<T>::get_type
        , fxns<is_small>::template type<T>::static_delete
        , fxns<is_small>::template type<T>::clone
        , fxns<is_small>::template type<T>::move
      };
      return &static_table;
    }
  };

  struct empty {
  };
} // namespace any_detail

struct VCNAny
{
  // structors
  template <typename T>
  VCNAny(const T& x) {
    table = any_detail::get_table<T>::get();
    if (sizeof(T) <= sizeof(void*)) {
      new(&object) T(x);
    }
    else {        
      object = new T(x);
    }
  }

  VCNAny() {
    table = any_detail::get_table<any_detail::empty>::get();
    object = NULL;
  }

  VCNAny(const VCNAny& x) {
    table = any_detail::get_table<any_detail::empty>::get();
    assign(x);
  }

  ~VCNAny() {
    table->static_delete(&object);
  }

  // assignment
  VCNAny& assign(const VCNAny& x) {
    // are we copying between the same type?
    if (table == x.table) {
      // if so, we can avoid reallocation
      table->move(&x.object, &object);
    }
    else {
      reset();
      x.table->clone(&x.object, &object);				          
      table = x.table;
    }
    return *this;
  }

  template <typename T>
  VCNAny& assign(const T& x)
  {
    // are we copying between the same type?
    any_detail::fxn_ptr_table* x_table = any_detail::get_table<T>::get();
    if (table == x_table) {
      // if so, we can avoid deallocating and resuse memory 
      if (sizeof(T) <= sizeof(void*)) {
        // create copy on-top of object pointer itself
        new(&object) T(x);
      }
      else {
        // create copy on-top of old version
        new(object) T(x);
      }
    }
    else {        
      reset();
      if (sizeof(T) <= sizeof(void*)) {
        // create copy on-top of object pointer itself
        new(&object) T(x);
        // update table pointer 
        table = x_table;
      }
      else {
        object = new T(x);          
        table = x_table;
      }
    }
    return *this;
  }

  // assignment operator 
  template<typename T>
  VCNAny& operator=(T const& x) {
    return assign(x);
  }

  // utility functions
  VCNAny& swap(VCNAny& x) {
    std::swap(table, x.table);
    std::swap(object, x.object);
  }

  const std::type_info& get_type() const {
    return table->get_type();
  }

  template<typename T>
  const T& Cast() const {
    if (get_type() != typeid(T)) {
      throw bad_any_cast(get_type(), typeid(T));
    }
    if (sizeof(T) <= sizeof(void*)) {
      return *reinterpret_cast<T const*>(&object);
    }
    else {
      return *reinterpret_cast<T const*>(object);
    }
  }

  // implicit casting is disabled by default for compatibility with boost::any 
#ifdef ANY_IMPLICIT_CASTING
  // automatic casting operator
  template<typename T>
  operator T() const {
    return Cast<T>();
  }
#endif // implicit casting

  bool empty() const {
    //return object == NULL;
    return table == any_detail::get_table<any_detail::empty>::get();
  }

  void reset()
  {
    if (empty()) return; 
    table->static_delete(&object);
    table = any_detail::get_table<any_detail::empty>::get();
    object = NULL;
  }

  // fields
  any_detail::fxn_ptr_table* table;	  
  void* object;
};

// boost::any-like casting
template<typename T>
T* any_cast(VCNAny* this_) {
  if (this_->get_type() != typeid(T)) {
    throw bad_any_cast(this_->get_type(), typeid(T));
  }
  if (sizeof(T) <= sizeof(void*)) {
    return reinterpret_cast<T*>(&this_->object);
  }
  else {
    return reinterpret_cast<T*>(this_->object);
  }
}

template<typename T>
T const* any_cast(VCNAny const* this_) {
  return any_cast<T>(const_cast<VCNAny*>(this_));
}

template<typename T>
T const& any_cast(VCNAny const& this_){
  return *any_cast<T>(const_cast<VCNAny*>(&this_));
}

#endif // VCNANY_H
