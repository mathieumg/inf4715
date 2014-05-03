///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
/// 

#include "Precompiled.h"

#include "Constants.h"
#include "Matrix.h"
#include "Quaternion.h"
#include "Vector.h"
#include "Assert.h"

//-------------------------------------------------------------
/// Constructor 
//-------------------------------------------------------------
VCNQuat::VCNQuat() 
  : x(0.0f)
  , y(0.0f)
  , z(0.0f)
  , w(1.0f)
{ 
}

//-------------------------------------------------------------
/// Constructor 
//-------------------------------------------------------------
VCNQuat::VCNQuat( VCNFloat _x, VCNFloat _y, VCNFloat _z, VCNFloat _w )
  : x(_x)
  , y(_y)
  , z(_z)
  , w(_w)
{
}

///////////////////////////////////////////////////////////////////////
VCNQuat::VCNQuat(const Matrix4& mat)
{
  SetFromMatrix(mat);
}


//-------------------------------------------------------------
/// Destructor
//-------------------------------------------------------------
VCNQuat::~VCNQuat()
{
}


//-------------------------------------------------------------
/// Get the magnitude of a quaternion
//-------------------------------------------------------------
const float VCNQuat::GetMagnitude() const
{
  return sqrtf( x*x + y*y + z*z + w*w );
}


//-------------------------------------------------------------
/// Normalize the quaternion
//-------------------------------------------------------------
void VCNQuat::Normalize() 
{
  float m = sqrtf( x*x + y*y + z*z + w*w );

  if( m != 0 ) 
  { 
    x /= m;
    y /= m;
    z /= m;
    w /= m;
  }
}


//-------------------------------------------------------------
/// Conjugate the quaternion
//-------------------------------------------------------------
void VCNQuat::Conjugate() 
{
  x = -x;
  y = -y;
  z = -z;
}


//-------------------------------------------------------------
/// Rotate the quaternion
//-------------------------------------------------------------
void VCNQuat::Rotate( const VCNQuat &q1, const VCNQuat &q2 ) 
{ 
  VCNQuat t = q1 * q2 * (~q1); 
  x = t.x;
  y = t.y;
  z = t.z;
  w = t.w; 
}


//-------------------------------------------------------------
/// Rotate the quaternion
//-------------------------------------------------------------
Vector3 VCNQuat::Rotate( const Vector3 &v ) 
{ 
  VCNQuat t(x, y, z, w);
  VCNQuat r = t*v*(~t);
  return Vector3(r.x, r.y, r.z); 
}


//-------------------------------------------------------------
// Get a rotation matrix out of the quaternion
//-------------------------------------------------------------
void VCNQuat::GetMatrix(Matrix4& m) const
{
  VCN_ASSERT_MSG( GetMagnitude() > 0.9999f && GetMagnitude() < 1.0001f, "Quaternion is not normalized");		

  float wx, wy, wz, xx, yy, yz, xy, xz, zz, x2, y2, z2;

  // Set matrix to identity
  // m.SetIdentity();
  m.m14 = m.m24 = m.m34 = 0.0f;
  m.m44 = 1.0f;

  x2 = x + x; 
  y2 = y + y; 
  z2 = z + z;

  xx = x * x2;   
  xy = x * y2;  
  xz = x * z2;

  yy = y * y2;   
  yz = y * z2;   
  zz = z * z2;

  wx = w * x2;   
  wy = w * y2;   
  wz = w * z2;

  m.m11 = 1.0f - (yy + zz);
  m.m12 = xy - wz;
  m.m13 = xz + wy;

  m.m21 = xy + wz;
  m.m22 = 1.0f - (xx + zz);
  m.m23 = yz - wx;

  m.m31 = xz - wy;
  m.m32 = yz + wx;
  m.m33 = 1.0f - (xx + yy);
}


//-------------------------------------------------------------
/// Construct quaternion from Euler angles
//-------------------------------------------------------------
void VCNQuat::SetFromEuler( VCNFloat pitch, VCNFloat yaw, VCNFloat roll ) 
{
  float cX, cY, cZ, sX, sY, sZ, cYcZ, sYsZ, cYsZ, sYcZ;

  pitch *= 0.5f;
  yaw   *= 0.5f;
  roll  *= 0.5f;

  cX = cosf( pitch );
  cY = cosf( yaw );
  cZ = cosf( roll );

  sX = sinf( pitch );
  sY = sinf( yaw );
  sZ = sinf( roll );

  cYcZ = cY * cZ;
  sYsZ = sY * sZ;
  cYsZ = cY * sZ;
  sYcZ = sY * cZ;

  w = cX * cYcZ + sX * sYsZ;
  x = sX * cYcZ - cX * sYsZ;
  y = cX * sYcZ + sX * cYsZ;
  z = cX * cYsZ - sX * sYcZ;
}

//-------------------------------------------------------------
/// Converts quaternion into Euler angles
//-------------------------------------------------------------
void VCNQuat::GetEulers( VCNFloat* pitch, VCNFloat* yaw, VCNFloat* roll ) const
{
  double   r11, r21, r31, r32, r33;
  double   q00, q11, q22, q33;
  double   tmp;

  q00 = w * w;
  q11 = x * x;
  q22 = y * y;
  q33 = z * z;

  r11 = q00 + q11 - q22 - q33;
  r21 = 2 * (x*y + w*z);
  r31 = 2 * (x*z - w*y);
  r32 = 2 * (y*z + w*x);
  r33 = q00 - q11 - q22 + q33;

  tmp = fabs(r31);
  if(tmp > 0.999999)
  {
    double r12 = 2 * (x*y - w*z);
    double r13 = 2 * (x*z + w*y);

    *pitch = 0.0f;
    *yaw   = (float)-((VCNPI/2) * r31/tmp);
    *roll  = (float)atan2(-r12, -r31*r13);
  }
  else
  {
    *pitch = (float)atan2(r32, r33);
    *yaw   = (float)asin(-r31);
    *roll  = (float)atan2(r21, r11);
  }
}


//-------------------------------------------------------------
/// Divides a quaternion by a scalar value
//-------------------------------------------------------------
void VCNQuat::operator /= ( const VCNFloat f ) 
{
  w /= f;
  x /= f;
  y /= f;
  z /= f; 
}


//-------------------------------------------------------------
/// Divides a quaternion by a scalar value
//-------------------------------------------------------------
VCNQuat VCNQuat::operator / ( const VCNFloat f ) 
{
  return VCNQuat( x/f, y/f, z/f, w/f );
}


//-------------------------------------------------------------
/// Multiplies a quaternion by a scalar value
//-------------------------------------------------------------
void VCNQuat::operator *= ( const VCNFloat f ) 
{
  w *= f;  x *= f;  y *= f;  z *= f; 
}


//-------------------------------------------------------------
/// Multiplies a quaternion by a scalar value
//-------------------------------------------------------------
VCNQuat VCNQuat::operator * ( const VCNFloat f ) const
{
  return VCNQuat( x*f, y*f, z*f, w*f );
}


//-------------------------------------------------------------
/// Add two quaternions
//-------------------------------------------------------------
void VCNQuat::operator += ( const VCNQuat &q ) 
{
  w += q.w;
  x += q.x;
  y += q.y;
  z += q.z; 
}

//-------------------------------------------------------------
/// Add two quaternions
//-------------------------------------------------------------
VCNQuat VCNQuat::operator + ( const VCNQuat &q ) const 
{
  return VCNQuat( x+q.x, y+q.y, z+q.z, w+q.w );
}

//-------------------------------------------------------------
/// Subtract two quaternions
//-------------------------------------------------------------
VCNQuat VCNQuat::operator -( const VCNQuat &q ) const
{
  VCNQuat ret;
  ret.x = x - q.x;
  ret.y = y - q.y;
  ret.z = z - q.z;
  ret.w = w - q.w;
  return ret;
}
//-------------------------------------------------------------
/// Multiplies two quaternions
//-------------------------------------------------------------
void VCNQuat::operator *= ( const VCNQuat &q ) 
{
  float _w = w*q.w - x*q.x - y*q.y - z*q.z;
  float _x = w*q.x + x*q.w + y*q.z - z*q.y;
  float _y = w*q.y + y*q.w + z*q.x - x*q.z;
  float _z = w*q.z + z*q.w + x*q.y - y*q.x;

  x = _x;
  y = _y;
  z = _z;
  w = _w;
}


//-------------------------------------------------------------
/// Multiplies two quaternions
//-------------------------------------------------------------
VCNQuat VCNQuat::operator * ( const VCNQuat &q ) const
{
  VCNQuat qResult;

  qResult.w = w*q.w - x*q.x - y*q.y - z*q.z;
  qResult.x = w*q.x + x*q.w + y*q.z - z*q.y;
  qResult.y = w*q.y + y*q.w + z*q.x - x*q.z;
  qResult.z = w*q.z + z*q.w + x*q.y - y*q.x;

  return qResult;
}


//-------------------------------------------------------------
/// Multiplies a vector with a quaternion
//-------------------------------------------------------------
VCNQuat VCNQuat::operator * ( const Vector3 &v ) const 
{
  return VCNQuat(  w*v.x + y*v.z - z*v.y,
          w*v.y + z*v.x - x*v.z,
          w*v.z + x*v.y - y*v.x,
          -(x*v.x + y*v.y + z*v.z) );
}


//-------------------------------------------------------------
/// Spherically linearly interpolate between two unit 
/// quaternions, a and b
//-------------------------------------------------------------
const VCNQuat VCNQuat::Slerp( const VCNQuat& a, const VCNQuat& b, const float u )
{
  VCNQuat c;
  VCNFloat dot = a.DotProduct( b );

  // Don't bother interpolating after a certain amount, LERP and jet
  if( dot > 0.97f )
  {
    // LERP
    c = a + (b-a)*u;

    // return value must be normalized
    c.Normalize();
    
    return c;
  }

  // if (dot < 0), q1 and q2 are more than 90 degrees apart,
  // so we can invert one to reduce spinning
  if( dot < 0 )
  {
    dot = -dot;
    c = b * -1;
  }
  else
  {
    c = b;
  }

  VCNFloat angle = acosf( dot );
  VCNFloat sina = sinf( angle );
  VCNFloat sinat = sinf( angle * u );
  VCNFloat sinaomt = sinf( angle * (1-u) );
  return (a*sinaomt+c*sinat)/sina;
}

void VCNQuat::SetFromMatrix( const Matrix4& mat )
{
  float tr = mat.m11 + mat.m22 + mat.m33, h;
  if(tr >= 0)
  {
    h = sqrt(tr +1);
    w = float(0.5) * h; // HACK: Flip -/+ for LH > RH systems
    h = float(0.5) / h;

    x = (mat.m32 - mat.m23) * h;
    y = (mat.m13 - mat.m31) * h;
    z = (mat.m21 - mat.m12) * h;
  }
  else
  {
    int i = 0; 
    if (mat.m22 > mat.m11)
      i = 1; 
    if (mat.m33 > mat.rows[i][i])
      i = 2; 
    switch (i)
    {
    case 0:
      h = sqrt((mat.m11 - (mat.m22 + mat.m33)) + 1);
      x = float(0.5) * h;
      h = float(0.5) / h;

      y = (mat.m12 + mat.m21) * h; 
      z = (mat.m31 + mat.m13) * h;
      w = (mat.m32 - mat.m23) * h;
      break;
    case 1:
      h = sqrt((mat.m22 - (mat.m33 + mat.m11)) + 1);
      y = float(0.5) * h;
      h = float(0.5) / h;

      z = (mat.m23 + mat.m32) * h;
      x = (mat.m12 + mat.m21) * h;
      w = (mat.m13 - mat.m31) * h;
      break;
    case 2:
      h = sqrt((mat.m33 - (mat.m11 + mat.m22)) + 1);
      z = float(0.5) * h;
      h = float(0.5) / h;

      x = (mat.m31 + mat.m13) * h;
      y = (mat.m23 + mat.m32) * h;
      w = (mat.m21 - mat.m12) * h;
      break;
    default: // Make compiler happy
      x = y = z = w = 0;
      break;
    }
  }

  Normalize();
}

void VCNQuat::SetFromDirection( const Vector3& dir, const Vector3& up )
{
  // Create view matrix
  Matrix4 basis;
  basis.BuildRotationFromDirection(dir, up);

  // Transform to quaternion
  SetFromMatrix( basis );
}

///////////////////////////////////////////////////////////////////////
VCNQuat::operator const Matrix4() const
{
  Matrix4 mat;
  GetMatrix( mat );
  return mat;
}

///////////////////////////////////////////////////////////////////////
const VCNQuat VCNQuat::FromEuler(VCNFloat pitch, VCNFloat yaw, VCNFloat roll)
{
  VCNQuat quat;
  quat.SetFromEuler( pitch, yaw, roll );
  return quat;
}



///////////////////////////////////////////////////////////////////////
void VCNQuat::SetIdentity()
{
  x = y = z = 0;
  w = 1;
}
