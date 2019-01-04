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

};