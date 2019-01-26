#pragma once

#include <spob/vector2.h>

namespace spob
{	

	class crd2;
	class space2;
	class line2;

	//-------------------------------------------------------------------------
	class crd2
	{
	public:
		vec2 i, j, pos;

		crd2(); // make standard coord system or null?
		crd2(const vec2& i, const vec2& j, const vec2& pos);

		//---------------------------------------------------------------------
		void invert(void); // invert only normal j
		void normalize(void);
		void orthogonalize(void);
		void orthonormalize(void);

		void place(const vec2& to);
		void move(const vec2& offset);
		
		//---------------------------------------------------------------------
		bool isRight(void) const; // j == cross(i)
		bool isOrthogonal(void) const;
		bool isOrthonormal(void) const;
	};

	crd2 getStandardCrd2(void);

	//-------------------------------------------------------------------------
	class space2 : public crd2
	{
	public:
		space2();
		space2(const crd2& crd);
		space2(const vec2& i, const vec2& j, const vec2& pos);
		space2& operator=(const crd2& crd);

		vec2 to(const vec2& o) const; // o in absolute
		vec2 toDir(const vec2& d) const;
		vec2 from(const vec2& o) const; // o in current
		vec2 fromDir(const vec2& d) const;

		crd2 to(const crd2& crd) const;
		crd2 from(const crd2& crd) const;
	};

	// crd.to() == invert(crd).from()
	crd2 invert(const crd2& crd);

	//-------------------------------------------------------------------------
	class line2 : public crd2
	{
	public:
		line2();
		line2(const crd2& crd); // warning: crd can be not orthogonal
		line2(const vec2& i, const vec2& pos); // j = cross(i)
		line2(const vec2& i, const vec2& j, const vec2& pos); 
		line2& operator=(const crd2& crd);

		double to(const vec2& o) const; // o in absolute
		double toDir(const vec2& d) const;
		vec2 from(const double& o) const; // o in current
		vec2 fromDir(const double& d) const;

		vec2 project(const vec2& o) const;
		vec2 projectDir(const vec2& d) const;
	};

	line2 makeLine2(const vec2& pos, const vec2& toI); // i = toI - pos

	//-------------------------------------------------------------------------
	// Relative location checks
	bool isPerpendicular(const line2& a, const line2& b);
	bool isPerpendicular(const line2& line, const vec2& d);
	bool isIntersect(const line2& a, const line2& b);
	bool isContain(const line2& a, const line2& b);
	bool isContain(const line2& line, const vec2& o);
	bool isParallel(const line2& a, const line2& b);
	bool isParallel(const line2& line, const vec2& d);

	//-------------------------------------------------------------------------
	// Distance between objects
	// If objects intersect, then distance = 0
	double distance(const line2& a, const line2& b);
	double distance(const line2& line, const vec2& o);

	//-------------------------------------------------------------------------
	// Rotating around point
	vec2 rotate(const vec2& o, const vec2& around, double angle);
	vec2 rotateDir(const vec2& d, const vec2& around, double angle);
	crd2 rotate(const crd2& crd, const vec2& around, double angle);

	//-------------------------------------------------------------------------
	// Find intersection
	// 2D lines
	Inter intersect(const line2& a, const line2& b,
					double& t1, double& t2);
	Inter intersect(const line2& a, const line2& b,
					vec2& pos);

//=============================================================================
//=============================================================================
//=============================================================================

//-----------------------------------------------------------------------------
inline crd2::crd2() : i(1, 0), j(0, 1), pos(0) {
}

//-----------------------------------------------------------------------------
inline crd2::crd2(const vec2& i, const vec2& j, const vec2& pos) : i(i), j(j), pos(pos) {
}

//-----------------------------------------------------------------------------
inline void crd2::invert(void) {
	j.negate();
}

//-----------------------------------------------------------------------------
inline void crd2::normalize(void) {
	i.normalize();
	j.normalize();
}

//-----------------------------------------------------------------------------
inline void crd2::orthogonalize(void) {
	i = i;
	j = j - projection(j, i);
}

//-----------------------------------------------------------------------------
inline void crd2::orthonormalize(void) {
	orthogonalize();
	normalize();
}

//-----------------------------------------------------------------------------
inline void crd2::place(const vec2& to) {
	pos = to;
}

//-----------------------------------------------------------------------------
inline void crd2::move(const vec2& offset) {
	pos += offset;
}

//-----------------------------------------------------------------------------
inline bool crd2::isOrthogonal(void) const {
	return isPerpendicular(i, j);
}

//-----------------------------------------------------------------------------
inline crd2 getStandardCrd2(void) {
	return crd2(vec2(1, 0), vec2(0, 1), vec2(0, 0));
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
inline space2::space2() : crd2() {
}

//-----------------------------------------------------------------------------
inline space2::space2(const crd2& crd) {
	operator=(crd);
}

//-----------------------------------------------------------------------------
inline space2::space2(const vec2& i, const vec2& j, const vec2& pos) : crd2(i, j, pos) {
}

//-----------------------------------------------------------------------------
inline space2& space2::operator=(const crd2& crd) {
	i = crd.i;
	j = crd.j;
	pos = crd.pos;
	return *this;
}

//-----------------------------------------------------------------------------
inline vec2 space2::to(const vec2& o) const {
	return toDir(o - pos);
}

//-----------------------------------------------------------------------------
inline vec2 space2::from(const vec2& o) const {
	return pos + i*o.x + j*o.y;
}

//-----------------------------------------------------------------------------
inline vec2 space2::fromDir(const vec2& d) const {
	return from(d) - pos;
}

//-----------------------------------------------------------------------------
inline crd2 invert(const crd2& crd) {
	return space2(crd).to(getStandardCrd2());
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
inline line2::line2() : crd2() {
}

//-----------------------------------------------------------------------------
inline line2::line2(const crd2& crd) {
	operator=(crd);
}

//-----------------------------------------------------------------------------
inline line2::line2(const vec2& i, const vec2& pos) : crd2(i, cross(i), pos) {
}

//-----------------------------------------------------------------------------
inline line2::line2(const vec2& i, const vec2& j, const vec2& pos) : crd2(i, j, pos) {
}

//-----------------------------------------------------------------------------
inline line2& line2::operator=(const crd2& crd) {
	i = crd.i;
	j = crd.j;
	pos = crd.pos;
	return *this;
}

//-----------------------------------------------------------------------------
inline double line2::to(const vec2& o) const {
	return toDir(o - pos);
}

//-----------------------------------------------------------------------------
inline vec2 line2::from(const double& o) const {
	return pos + i*o;
}

//-----------------------------------------------------------------------------
inline vec2 line2::fromDir(const double& d) const {
	return from(d) - pos;
}

//-----------------------------------------------------------------------------
inline vec2 line2::project(const vec2& o) const {
	return from(to(o));
}

//-----------------------------------------------------------------------------
inline vec2 line2::projectDir(const vec2& d) const {
	return fromDir(toDir(d));
}

//-----------------------------------------------------------------------------
inline line2 makeLine2(const vec2& pos, const vec2& toI) {
	return line2(toI - pos, pos);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
inline bool isPerpendicular(const line2& a, const line2& b) {
	return isPerpendicular(a.i, b.i);
}

//-----------------------------------------------------------------------------
inline bool isPerpendicular(const line2& line, const vec2& d) {
	return isPerpendicular(line.i, d);
}

//-----------------------------------------------------------------------------
inline bool isIntersect(const line2& a, const line2& b) {
	return !isParallel(a, b);
}

//-----------------------------------------------------------------------------
inline bool isContain(const line2& a, const line2& b) {
	return isParallel(a, b) && isContain(a, b.pos);
}

//-----------------------------------------------------------------------------
inline bool isContain(const line2& line, const vec2& o) {
	return isCollinear(line.i, o - line.pos) || isNear(line.pos, o);
}

//-----------------------------------------------------------------------------
inline bool isParallel(const line2& a, const line2& b) {
	return isCollinear(a.i, b.i);
}

//-----------------------------------------------------------------------------
inline bool isParallel(const line2& line, const vec2& d) {
	return isCollinear(line.i, d);
}

};