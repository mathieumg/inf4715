///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
/// 

#ifndef SINGLETON_H
#define SINGLETON_H

#pragma once

#define SINGLETON_DECLARATION(_CLASS)   \
  friend class Singleton<_CLASS>;

template <typename T>
class Singleton
{
public:

  /// Create the instance if not already created.
  static T& CreateInstance()
  {
    return GetInstance();
  }

  /// Returns the unique instance
  static T& GetInstance()
  {
    if (!mInstance) {
      mInstance = new T();
      atexit( Release );
    }
    return *mInstance;
  }
  
  /// Release the unique instance
  static void Release()
  {
    delete mInstance;
    mInstance = 0;
  }

protected:
  Singleton() {}
  ~Singleton() {}
  
private:
  Singleton(Singleton const&); // copy ctor hidden
  Singleton & operator=(Singleton const&); // assign op hidden

  static T* mInstance;
};

template <typename T> T* Singleton<T>::mInstance = 0;

#endif  /* _SINGLETON_H */

