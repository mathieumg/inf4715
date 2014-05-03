///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
/// 

#ifndef VCNQUAT_H
#define VCNQUAT_H

#pragma once

#include "Matrix.h"

//-------------------------------------------------------------
/// Our basic quaternion class
//-------------------------------------------------------------
class VCNQuat
{
public:
  VCNFloat x, y, z, w;

  /// Default constructor
  VCNQuat();

  /// Constructs a quaternion with set values
  VCNQuat(VCNFloat _x, VCNFloat _y, VCNFloat _z, VCNFloat _w);

  /// Construct from a matrix
  VCNQuat(const Matrix4& mat);

  // Destructs the quaternion
  ~VCNQuat();

  /// Constructs a quaternion from Euler angles
  static const VCNQuat FromEuler( VCNFloat pitch, VCNFloat yaw, VCNFloat roll );

  /// Constructs a quaternion from a spherical interpolate
  static const VCNQuat Slerp( const VCNQuat& a, const VCNQuat& b, const float u );

  /// Returns Euler angles
  void GetEulers( VCNFloat* pitch, VCNFloat* yaw, VCNFloat* roll ) const;

  /// Returns the corresponding matrix in a left handed system
  void GetMatrix(Matrix4& m) const;

  /// Returns the quaternion magnitude
  const VCNFloat GetMagnitude() const;

  /// Sets the identity
  void SetIdentity();

  /// Sets the quaternion using Euler angles
  void SetFromEuler( VCNFloat pitch, VCNFloat yaw, VCNFloat roll);

  /// Sets from a left handed matrix
  void SetFromMatrix(const Matrix4& mat);

  /// Sets two vectors
  void SetFromDirection(const Vector3& dir, const Vector3& up = Vector3(0, 1, 0));

  /// Normalize the quaternion
  void Normalize();

  /// Conjugate the quaternion
  void Conjugate();

  /// Computes the dot product with another quaternion
  VCNFloat DotProduct( const VCNQuat& q ) const;

  /// Rotate the quaternion
  void Rotate( const VCNQuat &q1, const VCNQuat &q2 );

  /// Rotates the quaternion around v
  Vector3 Rotate( const Vector3 &v );

// Operators

  void    operator /=( const VCNFloat f );
  VCNQuat operator /( const VCNFloat f );

  void    operator *=( const VCNFloat f );
  VCNQuat operator *( const VCNFloat f ) const;

  VCNQuat operator *( const Vector3 &v ) const;

  VCNQuat operator *( const VCNQuat &q ) const;
  void    operator *=( const VCNQuat &q );

  void    operator +=( const VCNQuat &q );
  VCNQuat operator +( const VCNQuat &q ) const;

  VCNQuat operator -( const VCNQuat &q ) const;

  VCNQuat operator~() const { return VCNQuat(-x, -y, -z, w); }

  operator const Matrix4() const;
};

//-------------------------------------------------------------
inline VCNFloat VCNQuat::DotProduct( const VCNQuat& q ) const
{
  return( x * q.x + y*q.y + z*q.z + w*q.w );
}

#endif // VCNQUAT_H
