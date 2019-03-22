#pragma once

#include <cmath>
#include <exception>

#include <spob/common.h>

namespace spob
{

	//-------------------------------------------------------------------------
	class vec2
	{
	public:
		double x, y;

		vec2();
		explicit vec2(const double& a);
		vec2(const double& x, const double& y);

		vec2& negate(void);
		vec2& normalize(void);

		double length(void) const;
		double lengthSqr(void) const;

		//---------------------------------------------------------------------
		vec2& operator+=(const vec2& a);
		vec2& operator-=(const vec2& a);
		vec2& operator*=(const double& a);
		vec2& operator/=(const double& a);
	};

	//-------------------------------------------------------------------------
	vec2 operator-(const vec2& a);
	vec2 operator+(const vec2& a, const vec2& b);
	vec2 operator-(const vec2& a, const vec2& b);
	vec2 operator*(const vec2& a, const double& k);
	vec2 operator/(const vec2& a, const double& k);
	vec2 operator*(const double& k, const vec2& a);
	vec2 operator/(const double& k, const vec2& a);

	//-------------------------------------------------------------------------
	vec2 cross(const vec2& i);
	double dot(const vec2& a, const vec2& b);
	double cosine(const vec2& a, const vec2& b);
	double project(const vec2& a, const vec2& to);
	vec2 projection(const vec2& a, const vec2& to);
	double distance(const vec2& a, const vec2& b);

	//-------------------------------------------------------------------------
	bool isPerpendicular(const vec2& a, const vec2& b);
	bool isCollinear(const vec2& a, const vec2& b);
	bool isCoDirectional(const vec2& a, const vec2& b);
	bool isAntiDirectional(const vec2& a, const vec2& b);

	//-------------------------------------------------------------------------
	vec2 cartesian2polar(const vec2& a);
	vec2 polar2cartesian(const vec2& a);

//=============================================================================
//=============================================================================
//=============================================================================

//-----------------------------------------------------------------------------
inline vec2::vec2() : vec2(0) {}

//-----------------------------------------------------------------------------
inline vec2::vec2(const double& a) : x(a), y(a) {}

//-----------------------------------------------------------------------------
inline vec2::vec2(const double& x, const double& y) : x(x), y(y) {}

//-----------------------------------------------------------------------------
inline vec2& vec2::negate(void) {
	x = -x;
	y = -y;
	return *this;
}

//-----------------------------------------------------------------------------
inline vec2& vec2::normalize(void) {
	double length1 = length();
	if (length1 != 0) {
		x /= length1;
		y /= length1;
	}
	return *this;
}

//-----------------------------------------------------------------------------
inline double vec2::length(void) const {
	return std::sqrt(x*x + y*y);
}

//-----------------------------------------------------------------------------
inline double vec2::lengthSqr(void) const {
	return x*x + y*y;
}

//-----------------------------------------------------------------------------
inline vec2& vec2::operator+=(const vec2& a) {
	x += a.x;
	y += a.y;
	return *this;
}

//-----------------------------------------------------------------------------
inline vec2& vec2::operator-=(const vec2& a) {
	x -= a.x;
	y -= a.y;
	return *this;
}

//-----------------------------------------------------------------------------
inline vec2& vec2::operator*=(const double& a) {
	x *= a;
	y *= a;
	return *this;
}

//-----------------------------------------------------------------------------
inline vec2& vec2::operator/=(const double& a) {
	#ifdef _DEBUG
	if (a == 0)
		throw std::exception("Divide by zero");
	#endif
	x /= a;
	y /= a;
	return *this;
}

//-----------------------------------------------------------------------------
inline vec2 operator-(const vec2& a) {
	return vec2(-a.x, -a.y);
}

//-----------------------------------------------------------------------------
inline vec2 operator+(const vec2& a, const vec2& b) {
	return vec2(a.x + b.x, a.y + b.y);
}

//-----------------------------------------------------------------------------
inline vec2 operator-(const vec2& a, const vec2& b) {
	return vec2(a.x - b.x, a.y - b.y);
}

//-----------------------------------------------------------------------------
inline vec2 operator*(const vec2& a, const double& k) {
	return vec2(a.x * k, a.y * k);
}

//-----------------------------------------------------------------------------
inline vec2 operator/(const vec2& a, const double& k) {
	#ifdef _DEBUG
	if (k == 0)
		throw std::exception("Divide by zero");
	#endif
	return vec2(a.x / k, a.y / k);
}

//-----------------------------------------------------------------------------
inline vec2 operator*(const double& k, const vec2& a) {
	return vec2(a.x * k, a.y * k);
}

//-----------------------------------------------------------------------------
inline vec2 operator/(const double& k, const vec2& a) {
	#ifdef _DEBUG
	if (a.x == 0 || a.y == 0)
		throw std::exception("Divide by zero");
	#endif
	return vec2(k / a.x, k/ a.y);
}

//-----------------------------------------------------------------------------
inline vec2 cross(const vec2& i) {
	return vec2(-i.y, i.x);
}

//-----------------------------------------------------------------------------
inline double dot(const vec2& a, const vec2& b) {
	return a.x * b.x + a.y * b.y;
}

//-----------------------------------------------------------------------------
inline double cosine(const vec2& a, const vec2& b) {
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
inline double project(const vec2& a, const vec2& to) {
	#ifdef _DEBUG
	if (to.length() == 0)
		throw std::exception("Projection to null vector");
	#endif
	return dot(a, to) / dot(to, to);
}

//-----------------------------------------------------------------------------
inline vec2 projection(const vec2& a, const vec2& to) {
	return to * project(a, to);
}

//-----------------------------------------------------------------------------
inline double distance(const vec2& a, const vec2& b) {
	return (b - a).length();
}

//-----------------------------------------------------------------------------
inline bool isPerpendicular(const vec2& a, const vec2& b) {
	return std::fabs(cosine(a, b)) < _SPOB_IS_NEAR_PRECISION;
}

//-----------------------------------------------------------------------------
inline bool isCollinear(const vec2& a, const vec2& b) {
	return std::fabs(std::fabs(cosine(a, b)) - 1) < _SPOB_IS_NEAR_PRECISION;
}

//-----------------------------------------------------------------------------
inline bool isCoDirectional(const vec2& a, const vec2& b) {
	return (cosine(a, b) > 0) && isCollinear(a, b);
}

//-----------------------------------------------------------------------------
inline bool isAntiDirectional(const vec2& a, const vec2& b) {
	return (cosine(a, b) < 0) && isCollinear(a, b);
}

//-----------------------------------------------------------------------------
inline vec2 cartesian2polar(const vec2& cartesian) {
	const double& x = cartesian.x;
	const double& y = cartesian.y;
	return vec2(std::atan2(y, x), std::sqrt(x*x + y*y));
}

//-----------------------------------------------------------------------------
inline vec2 polar2cartesian(const vec2& spheric) {
	const double& phi = spheric.x;
	const double& r = spheric.y;
	return vec2(r * std::cos(phi), r * std::sin(phi));
}

};