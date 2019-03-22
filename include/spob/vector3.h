#pragma once

#include <cmath>
#include <exception>

#include <spob/common.h>

namespace spob
{

	//-------------------------------------------------------------------------
	class vec3
	{
	public:
		double x, y, z;

		vec3();
		explicit vec3(const double& a);
		vec3(const double& x, const double& y, const double& z);

		vec3& negate(void);
		vec3& normalize(void);

		double length(void) const;
		double lengthSqr(void) const;

		//---------------------------------------------------------------------
		vec3& operator+=(const vec3& a);
		vec3& operator-=(const vec3& a);
		vec3& operator*=(const double& a);
		vec3& operator/=(const double& a);
	};

	//-------------------------------------------------------------------------
	vec3 operator-(const vec3& a);
	vec3 operator+(const vec3& a, const vec3& b);
	vec3 operator-(const vec3& a, const vec3& b);
	vec3 operator*(const vec3& a, const double& k);
	vec3 operator/(const vec3& a, const double& k);
	vec3 operator*(const double& k, const vec3& a);
	vec3 operator/(const double& k, const vec3& a);

	//-------------------------------------------------------------------------
	vec3 cross(const vec3& i, const vec3& j);
	double dot(const vec3& a, const vec3& b);
	double cosine(const vec3& a, const vec3& b);
	double project(const vec3& a, const vec3& to);
	vec3 projection(const vec3& a, const vec3& to);
	double distance(const vec3& a, const vec3& b);

	//-------------------------------------------------------------------------
	bool isPerpendicular(const vec3& a, const vec3& b);
	bool isCollinear(const vec3& a, const vec3& b);
	bool isCoDirectional(const vec3& a, const vec3& b);
	bool isAntiDirectional(const vec3& a, const vec3& b);

	//-------------------------------------------------------------------------
	vec3 cartesian2spheric(const vec3& a);
	vec3 spheric2cartesian(const vec3& a);
	vec3 cartesian2cylindrical(const vec3& a);
	vec3 cylindrical2cartesian(const vec3& a);

//=============================================================================
//=============================================================================
//=============================================================================

//-----------------------------------------------------------------------------
inline vec3::vec3() : vec3(0) {}

//-----------------------------------------------------------------------------
inline vec3::vec3(const double& a) : x(a), y(a), z(a) {}

//-----------------------------------------------------------------------------
inline vec3::vec3(const double& x, const double& y, const double& z) : x(x), y(y), z(z) {}

//-----------------------------------------------------------------------------
inline vec3& vec3::negate(void) {
	x = -x;
	y = -y;
	z = -z;
	return *this;
}

//-----------------------------------------------------------------------------
inline vec3& vec3::normalize(void) {
	double length1 = length();
	if (length1 != 0) {
		x /= length1;
		y /= length1;
		z /= length1;
	}
	return *this;
}

//-----------------------------------------------------------------------------
inline double vec3::length(void) const {
	return std::sqrt(x*x + y*y + z*z);
}

//-----------------------------------------------------------------------------
inline double vec3::lengthSqr(void) const {
	return x*x + y*y + z*z;
}

//-----------------------------------------------------------------------------
inline vec3& vec3::operator+=(const vec3& a) {
	x += a.x;
	y += a.y;
	z += a.z;
	return *this;
}

//-----------------------------------------------------------------------------
inline vec3& vec3::operator-=(const vec3& a) {
	x -= a.x;
	y -= a.y;
	z -= a.z;
	return *this;
}

//-----------------------------------------------------------------------------
inline vec3& vec3::operator*=(const double& a) {
	x *= a;
	y *= a;
	z *= a;
	return *this;
}

//-----------------------------------------------------------------------------
inline vec3& vec3::operator/=(const double& a) {
	#ifdef _DEBUG
	if (a == 0)
		throw std::exception("Divide by zero");
	#endif
	x /= a;
	y /= a;
	z /= a;
	return *this;
}

//-----------------------------------------------------------------------------
inline vec3 operator-(const vec3& a) {
	return vec3(-a.x, -a.y, -a.z);
}

//-----------------------------------------------------------------------------
inline vec3 operator+(const vec3& a, const vec3& b) {
	return vec3(a.x + b.x, a.y + b.y, a.z + b.z);
}

//-----------------------------------------------------------------------------
inline vec3 operator-(const vec3& a, const vec3& b) {
	return vec3(a.x - b.x, a.y - b.y, a.z - b.z);
}

//-----------------------------------------------------------------------------
inline vec3 operator*(const vec3& a, const double& k) {
	return vec3(a.x * k, a.y * k, a.z * k);
}

//-----------------------------------------------------------------------------
inline vec3 operator/(const vec3& a, const double& k) {
	#ifdef _DEBUG
	if (k == 0)
		throw std::exception("Divide by zero");
	#endif
	return vec3(a.x / k, a.y / k, a.z / k);
}

//-----------------------------------------------------------------------------
inline vec3 operator*(const double& k, const vec3& a) {
	return vec3(a.x * k, a.y * k, a.z * k);
}

//-----------------------------------------------------------------------------
inline vec3 operator/(const double& k, const vec3& a) {
	#ifdef _DEBUG
	if (a.x == 0 || a.y == 0 || a.z == 0)
		throw std::exception("Divide by zero");
	#endif
	return vec3(k / a.x, k / a.y, k / a.z);
}

//-----------------------------------------------------------------------------
inline vec3 cross(const vec3& i, const vec3& j) {
	return vec3(i.y * j.z - i.z * j.y, j.x * i.z - i.x * j.z, i.x * j.y - i.y * j.x);
}

//-----------------------------------------------------------------------------
inline double dot(const vec3& a, const vec3& b) {
	return a.x * b.x + a.y * b.y + a.z * b.z;
}

//-----------------------------------------------------------------------------
inline double cosine(const vec3& a, const vec3& b) {
	double aLen = a.length();
	double bLen = b.length();
	#ifdef _DEBUG
	if (aLen != 0 && bLen != 0) 
		return dot(a, b) / (aLen * bLen);
	else
		throw std::exception("Calc cosine of vector with 0 length.");
	#else
	if (aLen != 0 && bLen != 0) 
		return dot(a, b) / (aLen * bLen);
	else
		return 0;
	#endif
}

//-----------------------------------------------------------------------------
inline double project(const vec3& a, const vec3& to) {
	#ifdef _DEBUG
	if (to.length() == 0)
		throw std::exception("Projection to null vector");
	#endif
	return dot(a, to) / dot(to, to);
}

//-----------------------------------------------------------------------------
inline vec3 projection(const vec3& a, const vec3& to) {
	return to * project(a, to);
}

//-----------------------------------------------------------------------------
inline double distance(const vec3& a, const vec3& b) {
	return (b - a).length();
}

//-----------------------------------------------------------------------------
inline bool isPerpendicular(const vec3& a, const vec3& b) {
	return std::fabs(cosine(a, b)) < _SPOB_IS_NEAR_PRECISION;
}

//-----------------------------------------------------------------------------
inline bool isCollinear(const vec3& a, const vec3& b) {
	return std::fabs(std::fabs(cosine(a, b)) - 1) < _SPOB_IS_NEAR_PRECISION;
}

//-----------------------------------------------------------------------------
inline bool isCoDirectional(const vec3& a, const vec3& b) {
	return (cosine(a, b) > 0) && isCollinear(a, b);
}

//-----------------------------------------------------------------------------
inline bool isAntiDirectional(const vec3& a, const vec3& b) {
	return (cosine(a, b) < 0) && isCollinear(a, b);
}

//-----------------------------------------------------------------------------
inline vec3 cartesian2spheric(const vec3& cartesian) {
	const double& x = cartesian.x;
	const double& y = cartesian.y;
	const double& z = cartesian.z;
	return vec3(
		std::atan2(y, x),
		std::atan2(std::sqrt(x*x + y*y), z),
		std::sqrt(x*x + y*y + z*z)
	);
}

//-----------------------------------------------------------------------------
inline vec3 spheric2cartesian(const vec3& spheric) {
	const double& alpha = spheric.x;
	const double& beta = spheric.y;
	const double& r = spheric.z;
	return vec3(
		r * sin(beta) * cos(alpha), 
		r * sin(beta) * sin(alpha), 
		r * cos(beta)
	);
}

//-----------------------------------------------------------------------------
inline vec3 cartesian2cylindrical(const vec3& cartesian) {
	const double& x = cartesian.x;
	const double& y = cartesian.y;
	const double& z = cartesian.z;
	return vec3(std::atan2(y, x), std::sqrt(x*x + y*y), z);
}

//-----------------------------------------------------------------------------
inline vec3 cylindrical2cartesian(const vec3& cylindrical) {
	const double& phi = cylindrical.x;
	const double& r = cylindrical.y;
	const double& z = cylindrical.z;
	return vec3(r * std::cos(phi), r * std::sin(phi), z);
}

};