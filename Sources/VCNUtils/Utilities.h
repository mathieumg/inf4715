///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
/// 
/// @brief Various utilities
///

#ifndef UTILITIES_H
#define UTILITIES_H

#pragma once

// Engine includes
#include "VCNUtils/Types.h"
#include "VCNUtils/Vector.h"

// System includes
#include <cstdlib>
#include <string>
#include <vector>
#include <sstream>


/////////////////////////////////////////////////////////////
///
/// Functor that deletes pointers.
///
struct DeletePointer
{
  template <class T> void operator ()(T*& p) const
  {
    delete p;
    p = NULL;
  }
};

/// Utility package
namespace VCN
{
  /// Common constants
  static const float EPSILON_VCN( _R(0.00000001) );
  static const float PI_VCN( _R(3.1415926535897932384626433832795) );
  static const float PI_2_VCN( _R(1.5707963267948966192313216916398) );
  static const float PI_4_VCN( _R(0.785398163397448) );

  /// Vector I  
  static Vector3 UnitI = Vector3( 1.0, 0.0, 0.0 );

  /// Vector J
  static Vector3 UnitJ = Vector3( 0.0, 1.0, 0.0 );

  /// Vector K
  static Vector3 UnitK = Vector3( 0.0, 0.0, 1.0 );

  /// Convert radians to degrees
  const float RAD_TO_DEG_VCN( const float AngleRad );

  /// Convert degrees to radians
  const float DEG_TO_RAD_VCN( const float AngleDeg );

  /// Convert kilometers/hours to meters/seconds
  const float KMH_TO_MS( const float kmh );

  /// Convert meters/seconds to kilometers/hours
  const float MS_TO_KMH( const float ms );

  /// Returns the sign of the number (0.f, 1.f or -1.f)
  float Sign( float nombre );

  /// Returns the absolute number
  float Abs( float value );

  /// Returns true if the number if ~0
  const bool IsZero(float value);

  /// Returns the nearest power of tow for @a value
  const unsigned int NearestPowerOfTwo(unsigned int value);

  /// Checks if the value is in interval.
  const bool IsBetween( float Valeur, float BorneMin, float BorneMax );

  /// Checks if point is in square.
  const bool IsInSquare( float x, float xMin, float xMax, float y, float yMin, float yMax );

	/// Checks if the flags is contained
	bool HasFlag(std::size_t flagSet, std::size_t toCheck);

  /// Returns biggest aspect ratio of rectangle.
  float AspectRatio(float a, float b);

  /// Checks if the file exists on disk.
  const VCNBool FileExists(const VCNString& filepath);

  /************************************************************************/
  /*                  TEMPLATE FUNCTIONS                                  */
  /************************************************************************/

  ////////////////////////////////////////////////////////////////////////
  ///
  ///  Clamp value @a x between @a min and @a max.
  ///
  ///  @param [in]       x : value to clamp.
  ///  @param [in]       min : lower bound
  ///  @param [in]       max : upper bound
  ///
  ///  @return clamped value
  ///
  ////////////////////////////////////////////////////////////////////////
  template <typename T>
  T Clamp(T x, T min, T max)
  {
    return (x < min) ? min : (x > max) ? max : x;
  }

  template <typename T>
  const T Lerp(T t, T a, T b) 
  {
    return ( a + t * (b - a) );
  }

  ////////////////////////////////////////////////////////////////////////
  ///
  ///  Returns a random number between min and max.
  ///
  ///  @param [in]       min : minimum generated number
  ///  @param [in]       max : maximum generated number
  ///
  ///  @return random number between [min, max]
  ///
  ///  @remarks T must be a number.
  ///
  ////////////////////////////////////////////////////////////////////////
  template <typename T>
  const T Random(T min, T max)
  {
    return( ( rand() * ( max - min ) / RAND_MAX ) + min );
  }

  /// Returns a random vector with all components in the same random range
  template <typename T>
  const Vector3D<T> RandomVector3(T min, T max)
  {
    return Vector3D<T>( Random(min, max), Random(min, max), Random(min, max) );
  }

  /// Returns a random vector with different ranges for all components
  template <typename T>
  const Vector3D<T> RandomVector3(T xmin, T xmax, T ymin, T ymax, T zmin, T zmax)
  {
    return Vector3D<T>( Random(xmin, xmax), Random(ymin, ymax), Random(zmin, zmax) );
  }

  /// Returns the next power of two of @value
  template <typename T>
  const T NextPow2(T value)
  {
    int level = 1;
    T next = 1;
    while (next < value)
    {
      next = 1 << level;
      ++level;
    }

    return next;
  }

  ///////////////////////////////////////////////////////////////////////
  template<typename T>
  const bool AreEqual(const T v1, const T v2, const T eps = EPSILON_VCN)
  {
    return fabs(v2 - v1) < eps;
  }

  ///////////////////////////////////////////////////////////////////////
  template<typename T>
  const bool IsCloseEnough(T f1, T f2)
  {
    return IsCloseEnough(f1, f2, EPSILON_VCN);
  }

  ///////////////////////////////////////////////////////////////////////
  template<typename T>
  const bool IsCloseEnough(T f1, T f2, T c)
  {
    // Determines whether the two floating-point values f1 and f2 are
    // close enough together that they can be considered equal.

    return fabs((f1 - f2) / ((f2 == 0.0) ? 1.0 : f2)) < c;
  }

  /// Remove unwanted characters from a string.
  const std::string RemoveUnwantedCharacters(const std::string& str, const std::string& chars);

  ////////////////////////////////////////////////////////////////////////
  ///
  /// Cast anything to a string if possible.
  ///
  template<typename T>
  const std::string ToString(const T& valeur, std::ios_base& (*modifier)(std::ios_base&) = 0)
  {
    std::ostringstream oss;

    // Apply modifier if any.
    if (modifier)
      oss << modifier;

    oss << valeur;
    return oss.str();
  }

  ////////////////////////////////////////////////////////////////////////
  ///
  /// Try to convert a string to a type.
  ///
  template<typename T>
  const bool ToType(const std::string& chaine, T& valeur, std::ios_base& (*modifier)(std::ios_base&) = 0)
  {
    std::istringstream iss(chaine);

    // Apply modifier
    if (modifier)
      iss >> modifier;

    // Read value
    T temp;
    iss >> temp;

    // Make sure we successfully read the value
    const bool success = ( (!iss.fail()) && (iss.peek() == EOF) );

    if (success)
      valeur = temp;

    return success;
  }

  /// Prints floating-point numbers using engineering notation and SI prefixes 
  char *Eng(double value, int digits = 3, int numeric = 1);

  /// Print an error message
  void PrintError(const std::string& message);

	/// Strip the class name of a typeid
	std::string StripClassAndStruct(const std::string& tostrip);

	/// Get the type name of T
	template<typename T>
	const std::string& ExtractTypeName()
	{
		static std::string name = StripClassAndStruct(typeid(T).name());
		return name;
	}

  ////////////////////////////////////////////////////////////////////////
  ///
  /// Loop element if the predicate is true.
  /// 
  /// @param first   [IN]    first element in range
  /// @param last    [IN]    last element in range
  /// @param pred    [IN]    predicate to test condition
  /// @param f       [IN]    functor
  ///
  /// @return functor
  ///
  template< class InputIterator, class Function, class Predicate >
  Function for_each_if(InputIterator first, InputIterator last, Predicate pred, Function f)
  {
    for( ; first != last; ++first)
    {
      if( pred(*first) )
        f(*first);
    }
    return f;
  };

  ////////////////////////////////////////////////////////////////////////
  ///
  /// Only execute loop functor on element which equals T.
  /// 
  /// @param first   [IN]    first element in range
  /// @param last    [IN]    last element in range
  /// @param value   [IN]    value determining if the functor must be called.
  /// @param f       [IN]    functor
  ///
  /// @return functor
  ///
  template< class InputIterator, class Function, class T>
  Function for_each_equal(InputIterator first, InputIterator last, const T& value, Function f)
  {
    for( ; first != last; ++first)
    {
      if( *first == value )
        f(*first);
    }
    return f;
  };

  ////////////////////////////////////////////////////////////////////////
  ///
  /// Loop all element, but stop if the functor returns true.
  /// 
  /// @param first   [IN]    first element in range
  /// @param last    [IN]    last element in range
  /// @param f       [IN]    functor, return true to stop
  ///
  /// @return true if the function break
  ///
  template< class InputIterator, class Function >
  const bool for_each_break(InputIterator first, InputIterator last, Function f)
  {
    for( ; first != last; ++first)
    {
      if ( f(*first) )
        return true;
    }
    return false;
  };

	inline bool HasFlag(std::size_t flagSet, std::size_t toCheck)
	{
		return (flagSet & toCheck) == toCheck;
	}


}; // namespace VCN

#endif // __UTILITIES_H__
