///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
/// 
/// @brief 2D/3D/4D Vector implementation 
///

#ifndef VECTOR_H
#define VECTOR_H

#pragma once

#include <limits>
#include <cmath>
#include <cassert>
#include "iosfwd"

#define VECTOR_EPSILON 1.19209E-005

#ifdef _DEBUG
#define VECTOR_ASSERT assert
#else
#define VECTOR_ASSERT(args)
#endif

////////////////////////////////////////////////////////////////////////
/// @class Vector2D
///
/// @brief Generic 2D vector.
////////////////////////////////////////////////////////////////////////
template<class T>
class Vector2D
{
public:

	union
	{
		struct { T x, y; };  ///< individual components
		struct { T i, j; };  ///< individual components
		struct { T s, t; };  ///< individual components
		struct { T u, v; };  ///< individual components
		T vec[2];            ///< Components
	};

	static const Vector2D<T> Zero;

	explicit Vector2D()
		: x(0), y(0)
	{
	}

	/// Ctor
	explicit Vector2D(T _x, T _y)
		: x(_x), y(_y)
	{
	}

	/// Copy ctor
	Vector2D(const Vector2D& v)
		: x(v.x), y(v.y)
	{
	}

	/// Conversion ctor
	template<typename T2>
	Vector2D(const Vector2D<T2>& u)
		: x(T(u.x)), y(T(u.y))
	{
	}


	/// Copy assignment operator
	Vector2D& operator=(const Vector2D& v)
	{
		x = v.x;
		y = v.y;
		return *this;
	}

	const bool Equals(const Vector2D& v, const T eps = VECTOR_EPSILON) const
	{
		if ( fabs(x - v.x) >= eps || fabs(y - v.y) >= eps )
			return false;

		return true;
	}

	bool operator== (const Vector2D& v) const
	{
		return Equals(v);
	}

	// Returns the internal index
	T& operator[](const int i)
	{
		return vec[i];
	}

	// Returns the internal index
	const T operator[](const int i) const
	{
		return vec[i];
	}

	/// Returns the angle with the X axis
	T Angle() const
	{
		return ::atan2(x, y);
	}

	/// Rotates the vector counter-clockwise around Z.
	Vector2D Rotate(const T angle) const
	{
		const T cosAngle = cos(angle);
		const T sinAngle = sin(angle);
		return Vector2D(x * cosAngle + y * -sinAngle, x * sinAngle + y * cosAngle);
	}

	/// Rotates 90 degrees counter clockwise around Z.
	const Vector2D Rotate90CCW() const
	{
		return Vector2D(-y, x);
	}

	/// Rotates 90 degrees clockwise around Z.
	const Vector2D Rotate90CW() const
	{
		return Vector2D(y, -x);
	}

	/// Sets the vector components.
	Vector2D& Set(const T x, const T y)
	{
		this->x = x;
		this->y = y;
		return *this;
	}

	static Vector2D Lerp(float t, const Vector2D& v1, const Vector2D& v2)
	{
		return Vector2D(
			VCN::Lerp(t, v1.x, v2.x),
			VCN::Lerp(t, v1.y, v2.y)
		);
	}

};



///
/// Generic 3D vector.
///
template<typename T>
class Vector3D
{
public:

	union
	{
		struct { T x, y, z; };  ///< individual components
		struct { T i, j, k; };  ///< individual components
		struct { T s, t, r; };  ///< individual components
		struct { T r, g, b; };  ///< individual components
		T vec[3];               ///< Components
	};

	static const Vector3D<T> Zero;

	/**
	* @name Constructors
	* @{
	*/

	/// Construct default vector with every component at 0.
	explicit Vector3D()
		: x(0), y(0), z(0)
	{
	}

	explicit Vector3D(const T array[3])
		: x(array[0]), y(array[1]), z(array[2])
	{
	}

	explicit Vector3D(const int k)
		: x(T(k)), y(T(k)), z(T(k))
	{
	}

	/// Constructor
	explicit Vector3D(T _x, T _y, T _z)
		: x(_x), y(_y), z(_z)
	{
	}

	/// Copy Constructor
	Vector3D(const Vector3D& v)
		: x(v.x), y(v.y), z(v.z)
	{
	}

	/// Copy Constructor conversion
	template<class T2>
	Vector3D(const Vector3D<T2>& u)
		: x(T(u.x)), y(T(u.y)), z(T(u.z))
	{
	}

	/// Constructs a vector from two points.
	Vector3D(const Vector3D& from, const Vector3D& to)
		: x(to.x - from.x), y(to.y - from.y), z(to.z - from.z)
	{
	}

	/** @} */

	/**
	* @name Operators
	* @{
	*/

	/// Copy assignment operator
	Vector3D& operator=(const Vector3D& v)
	{
		x = v.x;
		y = v.y;
		z = v.z;
		return *this;
	}

	// Returns the internal index
	T& operator[](const int i)
	{
		return vec[i];
	}

	// Returns the internal index
	const T operator[](const int i) const
	{
		return vec[i];
	}

	/// Returns the vector data buffer.
	operator const T*() const
	{
		return (const T*)this->vec;
	}

	/// Returns writable vector buffer.
	operator T*()
	{
		return (T*)this->vec;
	}

	/// Multiplies a vector and a scalar together (i.e Vector(1,1,1) * 2.42)
	const Vector3D operator*(const T k) const
	{
		return Vector3D(x * k, y * k, z * k);
	}

	/// Multiplies a vector and a scalar together (i.e 2.42 * Vector(1,1,1))
	friend Vector3D operator*(const T k, const Vector3D& v)
	{
		return Vector3D(v.x * k, v.y * k, v.z * k);
	}

	/// Multiplies a scalar with self.
	const Vector3D& operator*=(const T k)
	{
		x *= k;
		y *= k;
		z *= k;
		return *this;
	}

	/// Dot product operator
	const T operator*(const Vector3D& v) const
	{
		return x * v.x + y * v.y + z * v.z;
	}

	/// Adds two vector together.
	const Vector3D operator+(const Vector3D& v) const
	{
		return Vector3D(x + v.x, y + v.y, z + v.z);
	}

	/// Adds a vector to self.
	const Vector3D& operator+=(const Vector3D& v)
	{
		x += v.x;
		y += v.y;
		z += v.z;
		return *this;
	}

	/// Subtracts two vector together
	const Vector3D operator-(const Vector3D& v) const
	{
		return Vector3D(x - v.x, y - v.y, z - v.z);
	}

	/// Subtracts two vector together
	const Vector3D& operator-=(const Vector3D& v)
	{
		x -= v.x;
		y -= v.y;
		z -= v.z;
		return *this;
	}

	/// Divides self with a scalar.
	const Vector3D operator/(const T k) const
	{
		VECTOR_ASSERT( k != 0 );
		return Vector3D( x / k, y / k, z / k);
	}

	template<typename U>
	const Vector3D operator/(const U k) const
	{
		VECTOR_ASSERT( k != 0 );
		return Vector3D( x / T(k), y / T(k), z / T(k));
	}

	/// Divide an vector with self.
	const Vector3D& operator/=(const T k)
	{
		VECTOR_ASSERT( k != 0 );
		x /= k;
		y /= k;
		z /= k;
		return *this;
	}

	/** @} */

	/// Set vector components.
	Vector3D& Set(const T inX, const T inY, const T inZ)
	{
		x = inX;
		y = inY;
		z = inZ;
		return *this;
	}

	/// Returns spherical coordinates of the vector.
	void GetSphericalCoordinates(T& radius, T& phi, T& theta) const
	{
		radius = this->Length();
		theta = theta();
		phi = phi();
	}

	/// Constructs a vector based on spherical coordinates
	void SetSphericalCoordinates(const T radius, const T phi, const T theta)
	{
		x = radius * sin(phi) * cos(theta);
		y = radius * sin(phi) * sin(theta);
		z = radius * cos(phi);
	}

	/// Returns the elevation angle
	T Phi() const
	{
		Vector2D<T> xy(x, y);
		Vector2D<T> zr (z, xy.Length());
		return zr.angle();
	}

	/// Returns the xy rotation
	T Theta() const
	{
		Vector2D<T> xy(x, y);
		return xy.angle();
	}

	/// Returns a 2D vector from X and Y
	const Vector2D<T> ToXY() const
	{
		return Vector2D<T>(x, y);
	}

	/// Returns a 2D vector from X and Z
	const Vector2D<T> ToXZ() const
	{
		return Vector2D<T>(x, z);
	}

	/// Returns a 2D vector from Y and Z
	const Vector2D<T> ToYZ() const
	{
		return Vector2D<T>(y, z);
	}

	/// Return the dot product between self . B
	const T DotProduct(const Vector3D& v) const
	{
		return x * v.x + y * v.y + z * v.z;
	}

	/// Multiplies per components (useful for scale vectors)
	Vector3D MulComponents(const Vector3D& v) const
	{
		return Vector3D(x * v.x, y * v.y, z * v.z);
	}

	/// Return the dot product between self . B
	friend const T DotProduct(const Vector3D& A, const Vector3D& B)
	{
		return A.x * B.x + A.y * B.y + A.z * B.z;
	}

	/// Returns the cross product of two vectors
	friend const Vector3D CrossProduct(const Vector3D& A, const Vector3D& B)
	{
		return Vector3D(A.y * B.z - A.z * B.y, A.z * B.x - A.x * B.z, A.x * B.y - A.y * B.x);
	}

	/// Compute the cross product of myself with B. (A x B)
	const Vector3D CrossProduct(const Vector3D& B) const
	{
		return ::CrossProduct(*this, B);
	}

	/// Computes the rotation vector of the myself around an given axis.
	/// Angle in radians
	Vector3D Rotate(const T angle, const Vector3D& axe) const
	{
		const T cosAngle = cos(angle);
		const T sinAngle = sin(angle);
		const T unMoinsCosAngle = 1 - cosAngle;

		return Vector3D(
			// X
			(cosAngle + unMoinsCosAngle * x * x) * x +
			(unMoinsCosAngle * x * y - z * sinAngle) * y +
			(unMoinsCosAngle * x * z + y * sinAngle) * z,
			// Y
			(unMoinsCosAngle * x * y + z * sinAngle) * x +
			(cosAngle + unMoinsCosAngle * y * y) * y +
			(unMoinsCosAngle * y * z - x * sinAngle) * z,
			// Z
			(unMoinsCosAngle * x * z - y * sinAngle) * x +
			(unMoinsCosAngle * y * z + x * sinAngle) * y +
			(cosAngle + unMoinsCosAngle * z * z) * z);
	}

	/// Normalize vector
	void Normalize()
	{
		const T n = Length() + std::numeric_limits<T>::min();
		x /= n;
		y /= n;
		z /= n;
	}

	/// Return normalized vector
	const Vector3D Normalized() const
	{
		const T n = Length() + std::numeric_limits<T>::min();
		return Vector3D( x / n, y / n, z / n );
	}

	/// Return Length
	const T Length() const
	{
		const T sl = SquareLength();
		return sqrt( sl );
	}

	/// Returns square Length
	const T SquareLength() const
	{
		return x*x + y*y + z*z;
	}

	/// Checks if vectors are almost equals
	const bool Equals(const Vector3D& v, const T eps = VECTOR_EPSILON) const
	{
		if ( fabs(x - v.x) >= eps || fabs(y - v.y) >= eps || fabs(z - v.z) >= eps )
			return false;

		return true;
	}

	bool operator== (const Vector3D& v) const
	{
		return Equals(v);
	}

	/// Returns the smallest arc angle formed between the two vectors.
	const T AngleWith( const Vector3D& v ) 
	{
		return (T)acos( ((*this) * v) / (this->Length()*v.Length()) );
	}

	/// Check if vector is null
	const bool IsNull() const
	{
		return Equals(Zero);
	}

	static Vector3D Lerp(float t, const Vector3D& v1, const Vector3D& v2)
	{
		return Vector3D(
			VCN::Lerp(t, v1.x, v2.x),
			VCN::Lerp(t, v1.y, v2.y),
			VCN::Lerp(t, v1.z, v2.z)
			);
	}
};




////////////////////////////////////////////////////////////////////////
/// @class Vector4D
///
/// @brief Generic 4D.
////////////////////////////////////////////////////////////////////////
template<class T>
class Vector4D
{
public:

	union
	{
		struct { T x, y, z, w; };  ///< individual components
		struct { T r, g, b, a; };  ///< individual components
		T vec[4];                  ///< Components
	};

	static const Vector4D<T> Zero;

	explicit Vector4D()
		: x(0), y(0), z(0), w(0)
	{
	}

	explicit Vector4D(const T _x, const T _y, const T _z, const T _w)
		: x(_x), y(_y), z(_z), w(_w)
	{
	}

	Vector4D(const Vector4D& v)
		: x(v.x), y(v.y), z(v.z), w(v.w)
	{
	}

	template<class T2>
	Vector4D(const Vector4D<T2>& u)
		: x(T(u.x)), y(T(u.y)), z(T(u.z)), w(T(u.w))
	{
	}

	Vector4D& operator=(const Vector4D& v)
	{
		x = v.x;
		y = v.y;
		z = v.z;
		w = v.w;
		return *this;
	}

	// Returns the internal index
	T& operator[](const int i)
	{
		return vec[i];
	}

	// Returns the internal index
	const T operator[](const int i) const
	{
		return vec[i];
	}

	/// Divide an vector with self.
	const Vector4D& operator/=(const T k)
	{
		VECTOR_ASSERT( k != 0 );
		x /= k;
		y /= k;
		z /= k;
		w /= k;
		return *this;
	}

	/// Checks if vectors are almost equals
	const bool Equals(const Vector4D& v, const T eps = VECTOR_EPSILON) const
	{
		if ( fabs(x - v.x) >= eps || fabs(y - v.y) >= eps || fabs(z - v.z) >= eps || fabs(w - v.w) >= eps )
			return false;

		return true;
	}

	bool operator== (const Vector4D& v) const
	{
		return Equals(v);
	}

	static Vector4D Lerp(float t, const Vector4D& v1, const Vector4D& v2)
	{
		return Vector3D(
			VCN::Lerp(t, v1.x, v2.x),
			VCN::Lerp(t, v1.y, v2.y),
			VCN::Lerp(t, v1.z, v2.z),
			VCN::Lerp(t, v1.w, v2.w)
			);
	}
};


//
// Various vector type conversion functions.
//

/// Cast a tuple x, y, z to an another tuple x, y, z of different type
template<typename R, typename V> inline const R V2V(const V& v) 
{
	return R(v.x, v.y, v.z); 
}

// 
// Defines useful typedef for quicker to use syntax
//

typedef Vector2D<float>         Vector2;
typedef Vector3D<float>         Vector3;
typedef Vector4D<float>         Vector4;

typedef Vector2D<float>         Vector2f;
typedef Vector3D<float>         Vector3f;
typedef Vector4D<float>         Vector4f;

typedef Vector2D<double>        Vector2d;
typedef Vector3D<double>        Vector3d;
typedef Vector4D<double>        Vector4d;

typedef Vector2D<int>           Vector2i;
typedef Vector3D<int>           Vector3i;
typedef Vector4D<int>           Vector4i;


template<typename T> const Vector2D<T> Vector2D<T>::Zero(0, 0);
template<typename T> const Vector3D<T> Vector3D<T>::Zero(0, 0, 0);
template<typename T> const Vector4D<T> Vector4D<T>::Zero(0, 0, 0, 0);


// For serialisation
template<class T>
std::ostream& operator<<(std::ostream& s, const Vector2D<T>& v)
{
	s << v.x << ' ' << v.y;
	return s;
}

template<class T>
std::istream &operator>>(std::istream &s, Vector2D<T>& v)
{
	s >> v.x >> v.y;
	return s;
}

template<class T>
std::ostream& operator<<(std::ostream& s, const Vector3D<T>& v)
{
	s << v.x << ' ' << v.y << ' ' << v.z;
	return s;
}

template<class T>
std::istream &operator>>(std::istream &s, Vector3D<T>& v)
{
	s >> v.x >> v.y >> v.z;
	return s;
}

template<class T>
std::ostream& operator<<(std::ostream& s, const Vector4D<T>& v)
{
	s << v.x << ' ' << v.y << ' ' << v.z << ' ' << v.w;
	return s;
}

template<class T>
std::istream &operator>>(std::istream &s, Vector4D<T>& v)
{
	s >> v.x >> v.y >> v.z >> v.w;
	return s;
}


#endif // VECTOR_H
