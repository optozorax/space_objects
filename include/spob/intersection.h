#pragma once

#include <spob/vector2.h>
#include <spob/vector3.h>
#include <spob/coordinates2.h>
#include <spob/coordinates3.h>

namespace spob
{
	
	//-------------------------------------------------------------------------
	/** Тип пересечения двух некоторых объектов. */
	enum Inter
	{
		INTER_ABSENT, // Объекты не имеют общих точек
		INTER_OVERLAPS, // Объекты пересекаются по некоторому множеству точек
		INTER_COINCIDES // Один объект находится внутри другого
	};

	//-------------------------------------------------------------------------
	// 2D lines
	Inter intersect(const line2& a, const line2& b,
					double& t1, double& t2);
	Inter intersect(const line2& a, const line2& b,
					vec2& pos);

	//-------------------------------------------------------------------------
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