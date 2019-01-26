#pragma once

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

};