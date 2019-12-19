#pragma once

#include <spob/common.h>
#include <spob/vector2.h>
#include <spob/coordinates2.h>

namespace spob
{	

	class polar2;
	class polar_space2;
	class disk2;
	class circle2;

	//-------------------------------------------------------------------------
	struct polar2 {
		polar2() : r(0), al(0) {}
		polar2(double r, double al) : r(r), al(al) {}

		double r;
		double al;

		void rotateClockwise(double angle) { al -= angle; }
		void rotateAntiClockwise(double angle) { al += angle; }
	};

	//-------------------------------------------------------------------------
	class polar_space2
	{
	public:
		space2 tr;

		polar_space2() : polar_space2(vec2(0), vec2(1, 0)) {}
		polar_space2(const vec2& pos, const double& r) : polar_space2(pos, pos + vec2(0, r)) {}
		polar_space2(const vec2& pos, const vec2& noonDir) : tr(makeLine2(pos, noonDir)) {
			tr.normalize();
		}

		void place(const vec2& to) { tr.place(to); }
		void move(const vec2& offset) { tr.move(offset); }
	protected:
		polar2 toStandard(const vec2& a) const {
			return polar2(a.length(), getRightAngle(vec2(0, 1), vec2(0), a));
		}
		vec2 fromStandard(const polar2& o) const {
			return vec2(cos(o.al), sin(o.al)) * o.r;
		}
	};

	//-------------------------------------------------------------------------
	class disk2 : public polar_space2
	{
	public:
		disk2() : polar_space2() {}
		disk2(const vec2& pos, const double& r) : polar_space2(pos, r) {}
		disk2(const vec2& pos, const vec2& noon) : polar_space2(pos, noon) {}

		polar2 to(const vec2& o) const {
			return toStandard(tr.to(o));
		}
		polar2 toDir(const vec2& d) const {
			return toStandard(tr.toDir(d));
		}
		vec2 from(const polar2& o) const {
			return tr.from(fromStandard(o));
		}
		vec2 fromDir(const polar2& d) const {
			return tr.fromDir(fromStandard(d));
		}
	};

	class circle2 : public polar_space2
	{
	public:
		circle2() : polar_space2() {}
		circle2(const vec2& pos, const double& r) : polar_space2(pos, r) {}
		circle2(const vec2& pos, const vec2& noon) : polar_space2(pos, noon) {}

		double to(const vec2& o) const {
			return toStandard(tr.to(o)).al;
		}
		double toDir(const vec2& d) const {
			return toStandard(tr.toDir(d)).al;
		}
		vec2 from(const double& al) const {
			return tr.from(fromStandard(polar2(1, al)));
		}
		vec2 fromDir(const double& al) const {
			return tr.fromDir(fromStandard(polar2(1, al)));
		}

		vec2 project(const vec2& o) const {
			return from(to(o));
		}
	};

	//-------------------------------------------------------------------------
	// Distance between objects
	// If objects intersect, then distance = 0
	inline double distance(const circle2& circle, const vec2& o) {
		return distance(circle.project(o), o);
	}

	//-------------------------------------------------------------------------
	// Rotating around point
	inline polar_space2 rotate(const polar_space2& polar, const vec2& around, double angle) {
		auto result = polar;
		result.tr = rotate(polar.tr, around, angle);
		return result;
	}

};