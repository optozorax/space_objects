#pragma once

#include <spob/common.h>
#include <spob/vector2.h>
#include <spob/vector3.h>
#include <spob/coordinates2.h>

namespace spob
{	

	class crd3;
	class space3;
	class plane3;
	class line3;

	//-------------------------------------------------------------------------
	class crd3
	{
	public:
		vec3 i, j, k, pos;

		crd3(); // make standard coord system or null?
		crd3(const vec3& i, const vec3& j, const vec3& k, const vec3& pos);

		//---------------------------------------------------------------------
		void invert(void); // invert only normal k
		void normalize(void);
		void orthogonalize(void);
		void orthonormalize(void);

		void place(const vec3& to);
		void move(const vec3& offset);

		//---------------------------------------------------------------------
		bool isRight(void) const; // k == cross(i, j), or isCoDirectional(k, cross(i, j))
		bool isOrthogonal(void) const; // is all axes orthogonal
		bool isOrthonormal(void) const;	// is all axes orthonormal
	};

	crd3 getStandardCrd3(void);

	//---------------------------------------------------------------------
	class space3 : public crd3
	{
	public:
		space3();
		space3(const crd3& crd);
		space3(const vec3& i, const vec3& j, const vec3& k, const vec3& pos);
		space3& operator=(const crd3& crd);

		vec3 to(const vec3& o) const; // o in absolute
		vec3 toDir(const vec3& d) const;
		vec3 from(const vec3& o) const; // o in current
		vec3 fromDir(const vec3& d) const;

		crd3 to(const crd3& crd) const;
		crd3 from(const crd3& crd) const;
	};

	// crd.to() == invert(crd).from()
	crd3 invert(const crd3& crd);

	//---------------------------------------------------------------------
	class plane3 : public crd3
	{
	public:
		plane3();
		plane3(const crd3& crd); // warning: crd can be not orthogonal
		plane3(const vec3& i, const vec3& j, const vec3& pos); // k = cross(i, j)
		plane3(const vec3& i, const vec3& j, const vec3& k, const vec3& pos); 
		plane3& operator=(const crd3& crd);

		vec2 to(const vec3& o) const; // o in absolute
		vec2 toDir(const vec3& d) const;
		vec3 from(const vec2& o) const; // o in current
		vec3 fromDir(const vec2& d) const;

		line2 to(const line3& o) const;
		line3 from(const line2& o) const;

		vec3 project(const vec3& o) const;
		vec3 projectDir(const vec3& d) const;
	};

	plane3 makePlane3(const vec3& pos, const vec3& toI, const vec3& toJ); // i = toI - pos, j = toJ - pos

	//---------------------------------------------------------------------
	class line3 : public crd3
	{
	public:
		line3();
		line3(const crd3& crd); // warning: crd can be not orthogonal
		line3(const vec3& i, const vec3& pos); // j are random but |j| = |i|, dot(i, j) = 0, k = cross(i, j), |k| = |i|
		line3(const vec3& i, const vec3& j, const vec3& k, const vec3& pos); 
		line3& operator=(const crd3& crd);

		double to(const vec3& o) const; // o in absolute
		double toDir(const vec3& d) const;
		vec3 from(const double& o) const; // o in current
		vec3 fromDir(const double& d) const;

		vec3 project(const vec3& o) const;
		vec3 projectDir(const vec3& d) const;
	};

	line3 makeLine3(const vec3& pos, const vec3& toI); // i = toI - pos

	//-------------------------------------------------------------------------
	// Relative location checks
	bool isPerpendicular(const plane3& a, const plane3& b);
	bool isPerpendicular(const plane3& plane, const line3& line);
	bool isPerpendicular(const plane3& plane, const vec3& d);
	bool isPerpendicular(const line3& a, const line3& b);
	bool isPerpendicular(const line3& line, const vec3& d);
	bool isIntersect(const plane3& a, const plane3& b);
	bool isIntersect(const plane3& plane, const line3& line);
	bool isIntersect(const line3& a, const line3& b);
	bool isContain(const plane3& a, const plane3& b);
	bool isContain(const plane3& plane, const line3& line);
	bool isContain(const plane3& plane, const vec3& o);
	bool isContain(const line3& a, const line3& b);
	bool isContain(const line3& line, const vec3& o);
	bool isParallel(const plane3& a, const plane3& b);
	bool isParallel(const plane3& plane, const line3& line);
	bool isParallel(const plane3& plane, const vec3& d);
	bool isParallel(const line3& a, const line3& b);
	bool isParallel(const line3& line, const vec3& d);
	bool isSkew(const line3& a, const line3& b);

	//-------------------------------------------------------------------------
	// Distance between objects
	// If objects intersect, then distance = 0
	double distance(const plane3& a, const plane3& b);
	double distance(const plane3& plane, const line3& line);
	double distance(const plane3& plane, const vec3& o);
	double distance(const line3& a, const line3& b);
	double distance(const line3& line, const vec3& o);

	//-------------------------------------------------------------------------
	// Rotating around axis
	vec3 rotate(const vec3& o, const line3& around, double angle);
	vec3 rotateDir(const vec3& d, const line3& around, double angle);
	crd3 rotate(const crd3& crd, const line3& around, double angle);

	// Rotating using Euler angles
	vec3 rotate(const vec3& o, const vec3& angles);
	crd3 rotate(const crd3& crd, const vec3& angles);

	//-------------------------------------------------------------------------
	// Find intersection
	// 3D lines
	Inter intersect(const line3& a, const line3& b,
					double& t1, double& t2);
	Inter intersect(const line3& a, const line3& b,
					vec3& pos);

	// 3D plane and line
	Inter intersect(const plane3& plane, const line3& line,
					vec2& pos, double& t);
	Inter intersect(const plane3& plane, const line3& line,
					vec3& pos);

	// 3D planes
	Inter intersect(const plane3& plane1, const plane3& plane2,
					line2& l1, line2& l2);
	Inter intersect(const plane3& plane1, const plane3& plane2,
					line3& line);

//=============================================================================
//=============================================================================
//=============================================================================

//-----------------------------------------------------------------------------
inline crd3::crd3() : i(1, 0, 0), j(0, 1, 0), k(0, 0, 1), pos(0) {
}

//-----------------------------------------------------------------------------
inline crd3::crd3(const vec3& i, const vec3& j, const vec3& k, const vec3& pos) : i(i), j(j), k(k), pos(pos) {
}

//-----------------------------------------------------------------------------
inline void crd3::invert(void) {
	k.negate();
}

//-----------------------------------------------------------------------------
inline void crd3::place(const vec3& to) {
	pos = to;
}

//-----------------------------------------------------------------------------
inline void crd3::move(const vec3& offset) {
	pos += offset;
}

//-----------------------------------------------------------------------------
inline crd3 getStandardCrd3(void) {
	return crd3(vec3(1, 0, 0), vec3(0, 1, 0), vec3(0, 0, 1), vec3(0, 0, 0));
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
inline space3::space3() : crd3() {
}

//-----------------------------------------------------------------------------
inline space3::space3(const crd3& crd) {
	operator=(crd);
}

//-----------------------------------------------------------------------------
inline space3::space3(const vec3& i, const vec3& j, const vec3& k, const vec3& pos) : crd3(i, j, k, pos) {
}

//-----------------------------------------------------------------------------
inline space3& space3::operator=(const crd3& crd) {
	i = crd.i;
	j = crd.j;
	k = crd.k;
	pos = crd.pos;
	return *this;
}

//-----------------------------------------------------------------------------
inline vec3 space3::to(const vec3& o) const {
	return toDir(o - pos);
}

//-----------------------------------------------------------------------------
inline vec3 space3::from(const vec3& o) const {
	return pos + i*o.x + j*o.y + k*o.z;
}

//-----------------------------------------------------------------------------
inline vec3 space3::fromDir(const vec3& d) const {
	return from(d) - pos;
}

//-----------------------------------------------------------------------------
inline crd3 invert(const crd3& crd) {
	return space3(crd).to(getStandardCrd3());
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
inline plane3::plane3() : crd3() {
}

//-----------------------------------------------------------------------------
inline plane3::plane3(const crd3& crd) {
	operator=(crd);
}

//-----------------------------------------------------------------------------
inline plane3::plane3(const vec3& i, const vec3& j, const vec3& pos) : crd3(i, j, cross(i, j), pos) {
}

//-----------------------------------------------------------------------------
inline plane3::plane3(const vec3& i, const vec3& j, const vec3& k, const vec3& pos) : crd3(i, j, k, pos) {
} 

//-----------------------------------------------------------------------------
inline plane3& plane3::operator=(const crd3& crd) {
	i = crd.i;
	j = crd.j;
	k = crd.k;
	pos = crd.pos;
	return *this;
}

//-----------------------------------------------------------------------------
inline vec2 plane3::to(const vec3& o) const {
	return toDir(o - pos);
}

//-----------------------------------------------------------------------------
inline vec3 plane3::from(const vec2& o) const {
	return pos + i*o.x + j*o.y;
}

//-----------------------------------------------------------------------------
inline vec3 plane3::fromDir(const vec2& d) const {
	return from(d) - pos;
}

//-----------------------------------------------------------------------------
inline line2 plane3::to(const line3& o) const {
	return line2(to(o.pos), toDir(o.i));
}

//-----------------------------------------------------------------------------
inline line3 plane3::from(const line2& o) const {
	return line3(from(o.pos), fromDir(o.i));
}

//-----------------------------------------------------------------------------
inline vec3 plane3::project(const vec3& o) const {
	return from(to(o));
}

//-----------------------------------------------------------------------------
inline vec3 plane3::projectDir(const vec3& d) const {
	return fromDir(toDir(d));
}

//-----------------------------------------------------------------------------
inline plane3 makePlane3(const vec3& pos, const vec3& toI, const vec3& toJ) {
	return plane3(toI - pos, toJ - pos, pos);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
inline line3::line3() : crd3() {
}

//-----------------------------------------------------------------------------
inline line3::line3(const crd3& crd) {
	operator=(crd);
}

//-----------------------------------------------------------------------------
inline line3::line3(const vec3& i, const vec3& j, const vec3& k, const vec3& pos) : crd3(i, j, k, pos) {
} 

//-----------------------------------------------------------------------------
inline line3& line3::operator=(const crd3& crd) {
	i = crd.i;
	j = crd.j;
	k = crd.k;
	pos = crd.pos;
	return *this;
}

//-----------------------------------------------------------------------------
inline double line3::to(const vec3& o) const {
	return toDir(o - pos);
}

//-----------------------------------------------------------------------------
inline vec3 line3::from(const double& o) const {
	return pos + i*o;
}

//-----------------------------------------------------------------------------
inline vec3 line3::fromDir(const double& d) const {
	return from(d) - pos;
}

//-----------------------------------------------------------------------------
inline vec3 line3::project(const vec3& o) const {
	return from(to(o));
}

//-----------------------------------------------------------------------------
inline vec3 line3::projectDir(const vec3& d) const {
	return fromDir(toDir(d));
}

//-----------------------------------------------------------------------------
inline line3 makeLine3(const vec3& pos, const vec3& toI) {
	return line3(toI - pos, pos);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
inline bool isPerpendicular(const plane3& a, const plane3& b) {
	return isPerpendicular(a.k, b.k);
}

//-----------------------------------------------------------------------------
inline bool isPerpendicular(const plane3& plane, const line3& line) {
	return isPerpendicular(plane.k, line.i);
}

//-----------------------------------------------------------------------------
inline bool isPerpendicular(const plane3& plane, const vec3& d) {
	return isCollinear(plane.k, d);
}

//-----------------------------------------------------------------------------
inline bool isPerpendicular(const line3& a, const line3& b) {
	return isPerpendicular(a.i, b.i);
}

//-----------------------------------------------------------------------------
inline bool isPerpendicular(const line3& line, const vec3& d) {
	return isPerpendicular(line.i, d);
}

//-----------------------------------------------------------------------------
inline bool isIntersect(const plane3& a, const plane3& b) {
	return !isParallel(a, b);
}

//-----------------------------------------------------------------------------
inline bool isIntersect(const plane3& plane, const line3& line) {
	return !isParallel(plane, line);
}

//-----------------------------------------------------------------------------
inline bool isIntersect(const line3& a, const line3& b) {
	const double precision = 0.0001;
	return !isParallel(a, b) && distance(plane3(a.i, b.i, a.pos), plane3(a.i, b.i, b.pos)) < precision;
}

//-----------------------------------------------------------------------------
inline bool isContain(const plane3& a, const plane3& b) {
	return isParallel(a, b) && isContain(a, b.pos);
}

//-----------------------------------------------------------------------------
inline bool isContain(const plane3& plane, const line3& line) {
	return isParallel(plane, line) && isContain(plane, line.pos);
}

//-----------------------------------------------------------------------------
inline bool isContain(const plane3& plane, const vec3& o) {
	return isParallel(plane, plane.pos - o) || isNear(plane.pos, o);
}

//-----------------------------------------------------------------------------
inline bool isContain(const line3& a, const line3& b) {
	return isParallel(a, b) && isContain(a, b.pos);
}

//-----------------------------------------------------------------------------
inline bool isContain(const line3& line, const vec3& o) {
	return isCollinear(line.i, line.pos - o) || isNear(line.pos, o);
}

//-----------------------------------------------------------------------------
inline bool isParallel(const plane3& a, const plane3& b) {
	return isCollinear(a.k, b.k);
}

//-----------------------------------------------------------------------------
inline bool isParallel(const plane3& plane, const line3& line) {
	return isPerpendicular(plane.k, line.i);
}

//-----------------------------------------------------------------------------
inline bool isParallel(const plane3& plane, const vec3& d) {
	return isPerpendicular(plane.k, d);
}

//-----------------------------------------------------------------------------
inline bool isParallel(const line3& a, const line3& b) {
	return isCollinear(a.i, b.i);
}

//-----------------------------------------------------------------------------
inline bool isParallel(const line3& line, const vec3& d) {
	return isCollinear(line.i, d);
}

//-----------------------------------------------------------------------------
inline bool isSkew(const line3& a, const line3& b) {
	return !isIntersect(a, b) && !isParallel(a, b);
}

};