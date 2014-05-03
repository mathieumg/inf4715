///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
/// 
/// @brief Matrix 4x4 implementation using row major
///

#ifndef __MATRIX_H__
#define __MATRIX_H__

#pragma once

#include "VCNUtils/Macros.h"
#include "VCNUtils/Types.h"
#include "VCNUtils/Vector.h"

//////////////////////////////////////////////////////////////////////////
///
///  @class Matrix4T
///
///  @brief Matrix 4x4
///
//////////////////////////////////////////////////////////////////////////
template <typename T>
class Matrix4T
{
public:

  /**
  * @name Constructors
  * @{
  */

  /// Construct a identity matrix.
  Matrix4T<T>()
    : m11(1), m12(0), m13(0), m14(0)
    , m21(0), m22(1), m23(0), m24(0)
    , m31(0), m32(0), m33(1), m34(0)
    , m41(0), m42(0), m43(0), m44(1)
  {
  }

  /// Construct a matrix by specifying every components.
  Matrix4T<T>(
    T  m0, T  m1, T  m2, T  m3,
    T  m4, T  m5, T  m6, T  m7,
    T  m8, T  m9, T m10, T m11,
    T m12, T m13, T m14, T m15)
    : m11(m0),  m12(m1),  m13(m2),  m14(m3)
    , m21(m4),  m22(m5),  m23(m6),  m24(m7)
    , m31(m8),  m32(m9),  m33(m10), m34(m11)
    , m41(m12), m42(m13), m43(m14), m44(m15)
  {
  }

  /// Construct matrix from raw array.
  explicit Matrix4T<T>(const T* elements)
  {
    memcpy(m, elements, sizeof(T)*16);
  }

  /// Copy constructor
  Matrix4T<T>(const Matrix4T<T> &mat)
    : m11(mat.m11),  m12(mat.m12),  m13(mat.m13),  m14(mat.m14)
    , m21(mat.m21),  m22(mat.m22),  m23(mat.m23),  m24(mat.m24)
    , m31(mat.m31),  m32(mat.m32),  m33(mat.m33),  m34(mat.m34)
    , m41(mat.m41),  m42(mat.m42),  m43(mat.m43),  m44(mat.m44)
  {
  }

  /** @} */

  /**
  * @name Operators
  * @{
  */

  /// Returns M + B
  Matrix4T<T> operator + (const Matrix4T<T> &B) const
  {
    return Matrix4T( 
      m[ 0] + B.m[ 0], m[ 1] + B.m[ 1], m[ 2] + B.m[ 2], m[ 3] + B.m[ 3],
      m[ 4] + B.m[ 4], m[ 5] + B.m[ 5], m[ 6] + B.m[ 6], m[ 7] + B.m[ 7],
      m[ 8] + B.m[ 8], m[ 9] + B.m[ 9], m[10] + B.m[10], m[11] + B.m[11],
      m[12] + B.m[12], m[13] + B.m[13], m[14] + B.m[14], m[15] + B.m[15]);
  }

  /// Returns M - B
  Matrix4T<T> operator - (const Matrix4T<T> &B) const
  {
    return Matrix4T( 
      m[ 0] - B.m[ 0], m[ 1] - B.m[ 1], m[ 2] - B.m[ 2], m[ 3] - B.m[ 3],
      m[ 4] - B.m[ 4], m[ 5] - B.m[ 5], m[ 6] - B.m[ 6], m[ 7] - B.m[ 7],
      m[ 8] - B.m[ 8], m[ 9] - B.m[ 9], m[10] - B.m[10], m[11] - B.m[11],
      m[12] - B.m[12], m[13] - B.m[13], m[14] - B.m[14], m[15] - B.m[15]);
  }

  /// Returns M * B
  Matrix4T<T> operator * (const Matrix4T<T> &B) const
  {
    return Matrix4T( 
      m[ 0] * B.m[ 0] + m[ 1] * B.m[ 4] + m[ 2] * B.m[ 8] + m[ 3] * B.m[12],
      m[ 0] * B.m[ 1] + m[ 1] * B.m[ 5] + m[ 2] * B.m[ 9] + m[ 3] * B.m[13],
      m[ 0] * B.m[ 2] + m[ 1] * B.m[ 6] + m[ 2] * B.m[10] + m[ 3] * B.m[14],
      m[ 0] * B.m[ 3] + m[ 1] * B.m[ 7] + m[ 2] * B.m[11] + m[ 3] * B.m[15],
      m[ 4] * B.m[ 0] + m[ 5] * B.m[ 4] + m[ 6] * B.m[ 8] + m[ 7] * B.m[12],
      m[ 4] * B.m[ 1] + m[ 5] * B.m[ 5] + m[ 6] * B.m[ 9] + m[ 7] * B.m[13],
      m[ 4] * B.m[ 2] + m[ 5] * B.m[ 6] + m[ 6] * B.m[10] + m[ 7] * B.m[14],
      m[ 4] * B.m[ 3] + m[ 5] * B.m[ 7] + m[ 6] * B.m[11] + m[ 7] * B.m[15],
      m[ 8] * B.m[ 0] + m[ 9] * B.m[ 4] + m[10] * B.m[ 8] + m[11] * B.m[12],
      m[ 8] * B.m[ 1] + m[ 9] * B.m[ 5] + m[10] * B.m[ 9] + m[11] * B.m[13],
      m[ 8] * B.m[ 2] + m[ 9] * B.m[ 6] + m[10] * B.m[10] + m[11] * B.m[14],
      m[ 8] * B.m[ 3] + m[ 9] * B.m[ 7] + m[10] * B.m[11] + m[11] * B.m[15],
      m[12] * B.m[ 0] + m[13] * B.m[ 4] + m[14] * B.m[ 8] + m[15] * B.m[12],
      m[12] * B.m[ 1] + m[13] * B.m[ 5] + m[14] * B.m[ 9] + m[15] * B.m[13],
      m[12] * B.m[ 2] + m[13] * B.m[ 6] + m[14] * B.m[10] + m[15] * B.m[14],
      m[12] * B.m[ 3] + m[13] * B.m[ 7] + m[14] * B.m[11] + m[15] * B.m[15]);
  }

  /// Returns M += B
  Matrix4T<T> &operator +=(const Matrix4T<T> &B)
  {
    for(int i = 0; i < 16; ++i)
      m[i] += B.m[i];
    return *this;
  }

  /// Returns M -= B
  Matrix4T<T> &operator -=(const Matrix4T<T> &B)
  {
    for(int i = 0; i < 16; ++i)
      m[i] -= B.m[i];
    return *this;
  }

  /// Returns M *= f
  Matrix4T<T> &operator *=(const T f)
  {
    for(int i = 0; i < 16; ++i)
      m[i] *= f;
    return *this;
  }

  /// Returns M *= B
  Matrix4T<T>& operator*=(const Matrix4T<T> &B)
  {
    Set( 
      m[ 0] * B.m[ 0] + m[ 1] * B.m[ 4] + m[ 2] * B.m[ 8] + m[ 3] * B.m[12],
      m[ 0] * B.m[ 1] + m[ 1] * B.m[ 5] + m[ 2] * B.m[ 9] + m[ 3] * B.m[13],
      m[ 0] * B.m[ 2] + m[ 1] * B.m[ 6] + m[ 2] * B.m[10] + m[ 3] * B.m[14],
      m[ 0] * B.m[ 3] + m[ 1] * B.m[ 7] + m[ 2] * B.m[11] + m[ 3] * B.m[15],
      m[ 4] * B.m[ 0] + m[ 5] * B.m[ 4] + m[ 6] * B.m[ 8] + m[ 7] * B.m[12],
      m[ 4] * B.m[ 1] + m[ 5] * B.m[ 5] + m[ 6] * B.m[ 9] + m[ 7] * B.m[13],
      m[ 4] * B.m[ 2] + m[ 5] * B.m[ 6] + m[ 6] * B.m[10] + m[ 7] * B.m[14],
      m[ 4] * B.m[ 3] + m[ 5] * B.m[ 7] + m[ 6] * B.m[11] + m[ 7] * B.m[15],
      m[ 8] * B.m[ 0] + m[ 9] * B.m[ 4] + m[10] * B.m[ 8] + m[11] * B.m[12],
      m[ 8] * B.m[ 1] + m[ 9] * B.m[ 5] + m[10] * B.m[ 9] + m[11] * B.m[13],
      m[ 8] * B.m[ 2] + m[ 9] * B.m[ 6] + m[10] * B.m[10] + m[11] * B.m[14],
      m[ 8] * B.m[ 3] + m[ 9] * B.m[ 7] + m[10] * B.m[11] + m[11] * B.m[15],
      m[12] * B.m[ 0] + m[13] * B.m[ 4] + m[14] * B.m[ 8] + m[15] * B.m[12],
      m[12] * B.m[ 1] + m[13] * B.m[ 5] + m[14] * B.m[ 9] + m[15] * B.m[13],
      m[12] * B.m[ 2] + m[13] * B.m[ 6] + m[14] * B.m[10] + m[15] * B.m[14],
      m[12] * B.m[ 3] + m[13] * B.m[ 7] + m[14] * B.m[11] + m[15] * B.m[15]);
    return *this;
  }

  /// Checks if two matrix are equal
  const bool operator == (const Matrix4T<T> &B) const
  {
    return memcmp(m, B.m, sizeof(T)*16) == 0;
  }

  /// Checks if two matrix are not equal
  const bool operator != (const Matrix4T<T> &B) const
  {
    return memcmp(m, B.m, sizeof(T)*16) != 0;
  }

  /// Returns m = B
  Matrix4T& operator=(const Matrix4T<T> &B)
  {
    m[ 0] = B.m[ 0]; m[ 1] = B.m[ 1]; m[ 2] = B.m[ 2]; m[ 3] = B.m[ 3];
    m[ 4] = B.m[ 4]; m[ 5] = B.m[ 5]; m[ 6] = B.m[ 6]; m[ 7] = B.m[ 7];
    m[ 8] = B.m[ 8]; m[ 9] = B.m[ 9]; m[10] = B.m[10]; m[11] = B.m[11];
    m[12] = B.m[12]; m[13] = B.m[13]; m[14] = B.m[14]; m[15] = B.m[15];
    return *this;
  }

  /// Return const raw array
  operator const T*() const { return m; }

  /// Return raw array
  operator       T*()       { return m; }

  /// Returns v = t * M
  friend const Vector3D<T> operator * (const Vector3D<T> &t, const Matrix4T<T> &mat)
  {
    return Vector3D<T>(   
      mat.m[ 0]*t.x + mat.m[ 4]*t.y + mat.m[ 8]*t.z + mat.m[12],
      mat.m[ 1]*t.x + mat.m[ 5]*t.y + mat.m[ 9]*t.z + mat.m[13],
      mat.m[ 2]*t.x + mat.m[ 6]*t.y + mat.m[10]*t.z + mat.m[14]);
  }

  /// Returns t *= M
  friend void operator *=(Vector3D<T> &t, const Matrix4T<T> &mat)
  {
    t.Set(  
      mat.m[ 0]*t[0] + mat.m[ 4]*t[1] + mat.m[ 8]*t[2] + mat.m[12],
      mat.m[ 1]*t[0] + mat.m[ 5]*t[1] + mat.m[ 9]*t[2] + mat.m[13],
      mat.m[ 2]*t[0] + mat.m[ 6]*t[1] + mat.m[10]*t[2] + mat.m[14]);
  }

  /// Returns v = M * t
  friend const Vector4D<T> operator * (const Matrix4T<T> &mat, const Vector4D<T> &t)
  {
    return Vector4D<T>(  
      mat.m[ 0]*t.x + mat.m[ 4]*t.y + mat.m[ 8]*t.z + mat.m[12]*t.w,
      mat.m[ 1]*t.x + mat.m[ 5]*t.y + mat.m[ 9]*t.z + mat.m[13]*t.w,
      mat.m[ 2]*t.x + mat.m[ 6]*t.y + mat.m[10]*t.z + mat.m[14]*t.w,
      mat.m[ 3]*t.x + mat.m[ 7]*t.y + mat.m[11]*t.z + mat.m[15]*t.w);
  }

  /// Returns v = t * M
  friend const Vector4D<T> operator * (const Vector4D<T> &t, const Matrix4T<T> &mat)
  {
    return Vector4D<T>(   
      mat.m[ 0]*t.x + mat.m[ 4]*t.y + mat.m[ 8]*t.z + mat.m[12]*t.w,
      mat.m[ 1]*t.x + mat.m[ 5]*t.y + mat.m[ 9]*t.z + mat.m[13]*t.w,
      mat.m[ 2]*t.x + mat.m[ 6]*t.y + mat.m[10]*t.z + mat.m[14]*t.w,
      mat.m[ 3]*t.x + mat.m[ 7]*t.y + mat.m[11]*t.z + mat.m[15]*t.w);
  }

  /** @} */

  /// Return raw data
  const T* GetData() const { return m; }

  /// Computes and returns the matrix's determinant.
  const T GetDeterminant() const
  {
    T det;
    det =  m[0] * m[5] * m[10];
    det += m[4] * m[9] * m[2];
    det += m[8] * m[1] * m[6];
    det -= m[8] * m[5] * m[2];
    det -= m[4] * m[1] * m[10];
    det -= m[0] * m[9] * m[6];
    return det;
  }

  /// Returns the inverse of the matrix
  Matrix4T<T> GetInverse() const
  {
    Matrix4T<T> copy = *this;
    copy.SetInverse();
    return copy;
  }

  Matrix4T<T> GetTranspose() const
  {
    Matrix4T<T> copy = *this;
    copy.SetTranspose();
    return copy;
  }

  const Vector3D<T> TransformCoord(const Vector3D<T>& v) const
  {
    return Vector3D<T>(  
      m[ 0]*v.x + m[ 4]*v.y + m[ 8]*v.z + m[12],
      m[ 1]*v.x + m[ 5]*v.y + m[ 9]*v.z + m[13],
      m[ 2]*v.x + m[ 6]*v.y + m[10]*v.z + m[14]);
  }

  const Vector3D<T> TransformNormal(const Vector3D<T>& v) const
  {
    return Vector3D<T>(  
      _11*v.x + _21*v.y + _31*v.z,
      _12*v.x + _22*v.y + _32*v.z,
      _13*v.x + _23*v.y + _33*v.z);
  }

  const Vector3D<T> GetTranslation() const 
  {
    return Vector3D<T>(m[12], m[13], m[14]);
  }

  /// Returns the frame x axis
  const Vector3D<T> GetXAxis() const
  {
    return Vector3D<T>(m11, m12, m13);
  }

  /// Returns the frame y axis
  const Vector3D<T> GetYAxis() const
  {
    return Vector3D<T>(m21, m22, m23);
  }

  /// Returns the frame z axis
  const Vector3D<T> GetZAxis() const
  {
    return Vector3D<T>(m31, m32, m33);
  }

  /// Returns the 3x3 matrix with a normalized frame
  Matrix4T GetRotation() const
  {
    Matrix4T<T> result;

    Vector3D<T>* xAxis = (Vector3D<T>*)&result.m11;
    Vector3D<T>* yAxis = (Vector3D<T>*)&result.m21;
    Vector3D<T>* zAxis = (Vector3D<T>*)&result.m31;

    *xAxis = GetXAxis();
    *yAxis = GetYAxis();
    *zAxis = GetZAxis();

    xAxis->Normalize();
    yAxis->Normalize();
    zAxis->Normalize();
        
    return result;
  }

  /// Construct a look at matrix
  Matrix4T<T> &SetLookAt(T viewX, T viewY, T viewZ, T focusX, T focusY, T focusZ, T upX, T upY, T upZ)
  {
    return SetLookAt( 
      Vector3D<T>(viewX , viewY , viewZ ),
      Vector3D<T>(focusX, focusY, focusZ),
      Vector3D<T>(upX   , upY   , upZ   ));
  }

  /// Construct a look at matrix for a LH system
  Matrix4T<T> &SetLookAt(const Vector3D<T> &viewPosition, const Vector3D<T> &viewFocus, const Vector3D<T> &upVector)
  {
    const Vector3D<T> zaxis = (viewFocus - viewPosition).Normalized();
    const Vector3D<T> xaxis = upVector.CrossProduct(zaxis).Normalized();
    const Vector3D<T> yaxis = zaxis.CrossProduct(xaxis);

    m[0] = xaxis.x; m[1] = yaxis.x; m[ 2] = zaxis.x;
    m[4] = xaxis.y; m[5] = yaxis.y; m[ 6] = zaxis.y;
    m[8] = xaxis.z; m[9] = yaxis.z; m[10] = zaxis.z;
    
    m[3]  = 0.f;
    m[7]  = 0.f;
    m[11] = 0.f;

    m[12] = -DotProduct(xaxis, viewPosition);
    m[13] = -DotProduct(yaxis, viewPosition);
    m[14] = -DotProduct(zaxis, viewPosition);
    m[15] = 1.0f;
    
    return *this;
  }

  /// Set a perspective matrix for a LH system
  Matrix4T<T>& SetPerspective(T fovDegrees, T aspect, T zNear, T zFar)
  {
    SetIdentity();

    const float radians = fovDegrees * 0.017453f;
    const float yScale = 1.0f / tan(radians * 0.5f);
    const float xScale = yScale / aspect;
    const float Q  = zFar / (zFar - zNear);

    m[ 0] = xScale;
    m[ 5] = yScale;
    m[10] = Q;
    m[11] = 1.0f;
    m[14] = -zNear * Q;
    m[15] = 0.0f;

    return *this;
  }

  /// Set a orthographic matrix for a LH system
  Matrix4T<T>& SetOrthogonality(T width, T height, T _near, T _far)
  {
    SetIdentity();

    m11 =  2.0f / width;
    m22 =  2.0f / height;
    m33 =  1.0f / (_far-_near);

    m43 =  -_near / (_far-_near);
    m44 =  1.0f;

    return *this;
  }

  /// SetInverse the matrix.
  const bool SetInverse()
  {
    int i, j, k, swap;
    T temp[16], t;

    memcpy(temp, m, 16*sizeof(T));
    SetIdentity();

    for (i = 0; i < 4; i++)
    {
      swap = i;
      for (j = i + 1; j < 4; j++)
      {
        if (fabs(temp[j*4 + i]) >  fabs(temp[i*4 + i]))
        {
          swap = j;
        }
      }

      if (swap != i)
      {
        for (k = 0; k < 4; k++)
        {
          t                = temp[i*4 + k];
          temp[i*4 + k]    = temp[swap*4 + k];
          temp[swap*4 + k] = t;

          t             = m[i*4 + k];
          m[i*4 + k]    = m[swap*4 + k];
          m[swap*4 + k] = t;
        }
      }

      if(!temp[i*4 + i])
        return false;

      t = temp[i*4 + i];

      for (k = 0; k < 4; k++)
      {
        temp[i*4 + k] /= t;
        m[i*4 + k]     = m[i*4 + k] / t;
      }

      for (j = 0; j < 4; j++)
      {
        if (j != i)
        {
          t = temp[j*4 + i];
          for (k = 0; k < 4; k++)
          {
            temp[j*4 + k] -= temp[i*4 + k] * t;
            m[j*4 + k]     = m[j*4 + k] - m[i*4 + k] * t;
          }
        }
      }
    }
    return true;
  }

  /// Set a scaling matrix.
  void BuildScale(const Vector3D<T>& axis)
  {
    SetIdentity();
    m[ 0] = axis.x;
    m[ 5] = axis.y;
    m[10] = axis.z;
  }

  /// Set a scaling matrix.
  void BuildScale(T x, T y, T z)
  {
    SetIdentity();
    m[ 0] = x;
    m[ 5] = y;
    m[10] = z;
  }

  /// Set a rotation matrix.
  void BuildRotationAxis(float angle, const Vector3D<T>& axis)
  {
    float   
      sinAngle = sin(angle),
      cosAngle = cos(angle),
      oneMinusCosAngle = 1.0f - cosAngle;

    SetIdentity();

    m[ 0] = (axis[0])*(axis[0]) + cosAngle*(1-(axis[0])*(axis[0]));
    m[ 4] = (axis[0])*(axis[1])*(oneMinusCosAngle) - sinAngle*axis[2];
    m[ 8] = (axis[0])*(axis[2])*(oneMinusCosAngle) + sinAngle*axis[1];

    m[ 1] = (axis[0])*(axis[1])*(oneMinusCosAngle) + sinAngle*axis[2];
    m[ 5] = (axis[1])*(axis[1]) + cosAngle*(1-(axis[1])*(axis[1]));
    m[ 9] = (axis[1])*(axis[2])*(oneMinusCosAngle) - sinAngle*axis[0];

    m[ 2] = (axis[0])*(axis[2])*(oneMinusCosAngle) - sinAngle*axis[1];
    m[ 6] = (axis[1])*(axis[2])*(oneMinusCosAngle) + sinAngle*axis[0];
    m[10] = (axis[2])*(axis[2]) + cosAngle*(1-(axis[2])*(axis[2]));
  }

  /// Set matrix from raw data.
  void Set(const T* elements)
  {
    memcpy(m, elements, sizeof(T)*16);
  }

  /// Set matrix by specifying every components.
  void Set(
    T  m0, T  m1, T  m2, T  m3,
    T  m4, T  m5, T  m6, T  m7,
    T  m8, T  m9, T m10, T m11,
    T m12, T m13, T m14, T m15)
  {
    m[ 0] =  m0; m[ 1] =  m1; m[ 2] =  m2; m[ 3] =  m3;
    m[ 4] =  m4; m[ 5] =  m5; m[ 6] =  m6; m[ 7] =  m7;
    m[ 8] =  m8; m[ 9] =  m9; m[10] = m10; m[11] = m11;
    m[12] = m12; m[13] = m13; m[14] = m14; m[15] = m15;
  }

  /// Set matrix from another matrix.
  void Set(const Matrix4T<T> &matrix)
  {
    memcpy(m, matrix.m, sizeof(T) * 16);
  }

  /// Transpose matrix.
  Matrix4T& SetTranspose()
  {
    T temp = 0;
    temp  = m[4];  
    m[4]  = m[1];
    m[1]  = temp;
    
    temp  = m[8];
    m[8]  = m[2];  
    m[2]  = temp;
    
    temp  = m[12]; 
    m[12] = m[3];
    m[3]  = temp;  
    
    temp  = m[9];
    m[9]  = m[6];  
    m[6]  = temp;
    
    temp  = m[13]; 
    m[13] = m[7];
    m[7]  = temp;  
    
    temp  = m[14];
    m[14] = m[11]; 
    m[11] = temp;

    return *this;
  }

  /// Apply identity matrix to myself.
  void SetIdentity()
  {
    memset(m, 0, sizeof(m));
    m[0] = m[5] = m[10] = m[15] = 1.0f;
  }

  /// SetInverse and transpose matrix.
  void SetInverseTranspose()
  {
    if(!SetInverse())
      return;
    SetTranspose();
  }

  /// Set matrix to null.
  void SetZero()
  {
    memset(m, 0, sizeof(T)*16);
  }

  /// Set matrix from rotation around X
  void SetRotationX(const T angle)
  {
    SetIdentity();
    m[ 5] =  cos(angle);
    m[ 6] =  sin(angle);
    m[ 9] = -m[6];
    m[10] =  m[5];
  }

  /// Set matrix from rotation around Y
  void SetRotationY(const T angle)
  {
    SetIdentity();
    m[ 0] =  cos(angle);
    m[ 2] =  sin(angle);
    m[ 8] = -m[2];
    m[10] =  m[0];
  }

  /// Set matrix from rotation around Z
  void SetRotationZ(const T angle)
  {
    SetIdentity();
    m[0] =  cos(angle);
    m[1] =  sin(angle);
    m[4] = -m[1];
    m[5] =  m[0];
  }

  void BuildRotationFromDirection(const Vector3D<T> &dir, const Vector3D<T>& up = Vector3D<T>(0, 1, 0))
  {
    // Cross vectors (left hand system)
    const Vector3 vRight = CrossProduct(up, dir);
    const Vector3 vUp = CrossProduct(dir, vRight);        

    // Create view matrix
    Set(
      vRight.x,     vRight.y,     vRight.z,     0.0f,
      vUp.x,        vUp.y,        vUp.z,        0.0f,
      dir.x,        dir.y,        dir.z,        0.0f,
      0.0f,         0.0f,         0.0f,         1.0f
    );
  }

  /// Set matrix from rotation around XYZ
  void BuildRotation(const Vector3D<T> &t)
  {
    float   
      cosX = cos(t[0]), sinX = sin(t[0]),
      cosY = cos(t[1]), sinY = sin(t[1]),
      cosZ = cos(t[2]), sinZ = sin(t[2]);

    Set(
      cosY * cosZ + sinX * sinY * sinZ,   -cosX * sinZ,    sinX * cosY * sinZ - sinY * cosZ,  0,
      cosY * sinZ - sinX * sinY * cosZ,    cosX * cosZ,   -sinY * sinZ - sinX * cosY * cosZ,  0,
      cosX * sinY,                         sinX,           cosX * cosY,                       0,
      0,                                   0,              0,                                 1);
  }

  /// Set matrix from rotation around XYZ
  void BuildRotation(T x, T y, T z)
  {
    float   
      cosX = cos(x), sinX = sin(x),
      cosY = cos(y), sinY = sin(y),
      cosZ = cos(z), sinZ = sin(z);

    Set(
      cosY * cosZ + sinX * sinY * sinZ,   -cosX * sinZ,    sinX * cosY * sinZ - sinY * cosZ,  0,
      cosY * sinZ - sinX * sinY * cosZ,    cosX * cosZ,   -sinY * sinZ - sinX * cosY * cosZ,  0,
      cosX * sinY,                         sinX,           cosX * cosY,                       0,
      0,                                   0,              0,                                 1);
  }

  /// Set translation matrix
  void BuildTranslation(const Vector3D<T>& t)
  {
    SetIdentity();
    m[12] =  t.x;
    m[13] =  t.y;
    m[14] =  t.z;
  }

  /// Set translation matrix
  void BuildTranslation(const T x, const T y, const T z)
  {
    SetIdentity();
    m[12] =  x;
    m[13] =  y;
    m[14] =  z;
  }

  void SetTranslation(const Vector3D<T>& t)
  {
    m[12] =  t.x;
    m[13] =  t.y;
    m[14] =  t.z;
  }

  /// Set translation matrix
  void SetTranslation(const T x, const T y, const T z)
  {
    m[12] =  x;
    m[13] =  y;
    m[14] =  z;
  }

  /// Trace matrix data to output window
  void Trace() const
  {
    TRACE(
      L"[%+.3f %+.3f %+.3f %+.3f\n"
      L" %+.3f %+.3f %+.3f %+.3f\n"
      L" %+.3f %+.3f %+.3f %+.3f\n"
      L" %+.3f %+.3f %+.3f %+.3f]\n",
      m[ 0], m[ 1], m[ 2], m[ 3],
      m[ 4], m[ 5], m[ 6], m[ 7],
      m[ 8], m[ 9], m[10], m[11],
      m[12], m[13], m[14], m[15]);
  }

// Data members

  union {

    // Representation used to debug
    //  rows[0] < X axis
    //  rows[1] < Y axis
    //  rows[2] < Z axis
    //  rows[3] < Translation
    struct {
      Vector4D<T> rows[4];
    };

    // Direct3D representation
    struct {
      T _11, _12, _13, _14;
      T _21, _22, _23, _24;
      T _31, _32, _33, _34;
      T _41, _42, _43, _44;
    };

    // Internal representation
    struct {
      T m11, m12, m13, m14;
      T m21, m22, m23, m24;
      T m31, m32, m33, m34;
      T m41, m42, m43, m44;
    };

    /// Raw array
    T     m[16];
  };
};

typedef Matrix4T<VCNFloat>  Matrix4;
typedef Matrix4T<float>     Matrix4f;
typedef Matrix4T<double>    Matrix4d;

#endif /* __MATRIX_H__ */
