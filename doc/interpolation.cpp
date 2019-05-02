#include <iostream>
#include "interpolation.h"

#include <spob/spob2glm.h>

namespace spob
{

//-----------------------------------------------------------------------------
double getSlopeAngle(const space2& crd) {
	return getAngle(vec2(0, 0), crd.fromDir(vec2(1, 0)));
}

//-----------------------------------------------------------------------------
vec2 getOffset(const space2& crd) {
	return crd.from(vec2(0, 0));
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
double interpolate(double a, double b, double pos) {
	return a + (b - a) * pos;
}

//-----------------------------------------------------------------------------
double interpolateCircularClockwise(double alpha, double beta, double pos) {
	if (isLessClockwise(alpha, beta)) {
		return interpolate(alpha, beta, pos);
	} else {
		double distance = getClockwiseDistance(alpha, beta);
		return interpolate(alpha, offsetClockwise(alpha, distance), pos);
	}
}

//-----------------------------------------------------------------------------
double interpolateCircularAntiClockwise(double alpha, double beta, double pos) {
	if (isLessAntiClockwise(alpha, beta)) {
		return interpolate(alpha, beta, pos);
	} else {
		double distance = getAntiClockwiseDistance(alpha, beta);
		return interpolate(alpha, offsetAntiClockwise(alpha, distance), pos);
	}
}

//-----------------------------------------------------------------------------
vec2 interpolate(const vec2& a, const vec2& b, double pos) {
	return vec2(interpolate(a.x, b.x, pos), interpolate(a.y, b.y, pos));
}

//-----------------------------------------------------------------------------
vec2 interpolateCircular(const vec2& a, const vec2& b, double pos) {
	return 
		polar2cartesian(
			interpolate(
				cartesian2polar(a), 
				cartesian2polar(b), 
				pos
			)
		);
}

//-----------------------------------------------------------------------------
vec2 interpolateCircularClockwise(const vec2& a, const vec2& b, double pos, double pos2) {
	vec2 ap = cartesian2polar(a);
	vec2 bp = cartesian2polar(b);
	vec2 cp(interpolateCircularClockwise(ap.x, bp.x, pos), interpolate(ap.y, bp.y, pos2 == -1 ? pos : pos2));
	return polar2cartesian(cp);
}

//-----------------------------------------------------------------------------
vec2 interpolateCircularAntiClockwise(const vec2& a, const vec2& b, double pos, double pos2) {
	vec2 ap = cartesian2polar(a);
	vec2 bp = cartesian2polar(b);
	vec2 cp(interpolateCircularAntiClockwise(ap.x, bp.x, pos), interpolate(ap.y, bp.y, pos2 == -1 ? pos : pos2));
	return polar2cartesian(cp);
}

//-----------------------------------------------------------------------------
crd2 interpolate(const crd2& a, const crd2& b, double pos) {
	return crd2(
		interpolate(a.i, b.i, pos), 
		interpolate(a.j, b.j, pos), 
		interpolate(a.pos, b.pos, pos)
	);
}

//-----------------------------------------------------------------------------
vec2 calcCircle(const vec2& p1, const vec2& p2, double r, bool isFirst) {
	#define sqr(a) ((a)*(a))
	double c = 2.0 * (p1.x-p2.x);
	double a = sqr(p1.x)-sqr(p2.x)+sqr(p1.y)-sqr(p2.y);
	double b, ba, bb, bc, d, x, y;
	if (c != 0) {
		a /= c;
		b = -2.0 * (p1.y-p2.y)/c;

		ba = sqr(b) + 1;
		bb = 2*a*b - 2*b*p1.x - 2*p1.y;
		bc = sqr(a) - 2*a*p1.x + sqr(p1.x) - sqr(r) + sqr(p1.y);

		d = sqr(bb) - 4.0 * ba * bc;
		if (d < 0) return (p1 + p2)/2.0;

		if (isFirst)
			y = (-bb+std::sqrt(d))/(2.0*ba);
		else
			y = (-bb-std::sqrt(d))/(2.0*ba);

		x = a + b*y;

		return vec2(x, y);
	} else {
		c = 2.0 * (p1.y-p2.y);
		a /= c;
		b = -2.0 * (p1.x-p2.x)/c;

		ba = sqr(b) + 1;
		bb = 2*a*b - 2*b*p1.y - 2*p1.x;
		bc = sqr(a) - 2*a*p1.y + sqr(p1.x) - sqr(r) + sqr(p1.y);

		d = sqr(bb) - 4.0 * ba * bc;
		if (d < 0) return (p1 + p2)/2.0;

		if (isFirst)
			x = (-bb+std::sqrt(d))/(2.0*ba);
		else
			x = (-bb-std::sqrt(d))/(2.0*ba);

		y = a + b*x;

		return vec2(x, y);
	}
	#undef sqr
}

//-----------------------------------------------------------------------------
crd2 interpolateCircular(const crd2& a, const crd2& b, double pos, double r, int type, bool isInvert) {
	crd2 result;
	double alpha = getAngle(vec2(0), a.i);
	double beta = getAngle(vec2(0), b.i);

	if (type == 0) type = (getClockwiseDistance(alpha, beta) < _SPOB_PI) + 1;

	if (type == 1) {
		vec2 circlePos = calcCircle(a.pos, b.pos, r, false ^ isInvert);

		result.i = interpolateCircularClockwise(a.i, b.i, pos);
		result.j = interpolateCircularClockwise(a.j, b.j, pos);
		result.pos = interpolateCircularClockwise(a.pos - circlePos, b.pos - circlePos, pos) + circlePos;
	} else {
		vec2 circlePos = calcCircle(a.pos, b.pos, r, true ^ isInvert);

		result.i = interpolateCircularAntiClockwise(a.i, b.i, pos);
		result.j = interpolateCircularAntiClockwise(a.j, b.j, pos);
		result.pos = interpolateCircularAntiClockwise(a.pos - circlePos, b.pos - circlePos, pos) + circlePos;
	}
	
	return result;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
SplineInterpolator::SplineInterpolator(int n, const crd2& c1, const crd2& c2) : spline(n), start(c1), end(c2) {
	glm::vec3 at_zero(c1.pos.x, c1.pos.y, 1);
	glm::mat3 p = getFromMatrix(c2) * getToMatrix(c1);
	spline.solve_coefs(p, at_zero);
}

//-----------------------------------------------------------------------------
crd2 SplineInterpolator::interpolate(double pos) const {
	auto calc_mx = [&] (double pos) -> double {
		auto dpos = spline.value(pos, 1);
		dpos.z = 0;
		return spob::vec2(dpos.x, dpos.y).length();
	};

	double startMx = calc_mx(0);

	spob::space2 start1;
	glm::vec3 vpos = spline.value(0);
	start1.pos = vec2(vpos.x, vpos.y);
	glm::vec3 dpos = spline.value(0, 1);
	start1.i = vec2(dpos.x, dpos.y)/startMx * start.i.length();
	start1.j = rotate(start.i, vec2(0), spob::deg2rad(90));

	spob::space2 current;
	vpos = spline.value(pos);
	current.pos = vec2(vpos.x, vpos.y);
	dpos = spline.value(pos, 1);
	current.i = vec2(dpos.x, dpos.y)/startMx * start.i.length();
	current.j = rotate(current.i, vec2(0), spob::deg2rad(90));

	double an1 = getSlopeAngle(start);
	double an2 = getSlopeAngle(start1);
	double an3 = getClockwiseDistance(an1, an2);
	current = rotate(current, current.pos, an3);

	return current;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
SplineInterpolator2::SplineInterpolator2(int n, const crd2& c1, const crd2& c2) : spline(n), start(c1), end(c2) {
	glm::mat3 at_zero(1);
	glm::mat3 p = getFromMatrix(space2(c1).to(c2));
	spline.solve_coefs(p);
}

//-----------------------------------------------------------------------------
crd2 SplineInterpolator2::interpolate(double pos) const {
	return start.from(getToCrd(spline.value(pos)));
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
glm::mat3 pow(const glm::mat3& P, double n) {
	using namespace Eigen;

	assert(n >= 0);

	glm::mat3 result(1);
	for (int i = 0; i < int(n); i++)
		result *= P;

	n -= int(n);

	while (n != 0) {
		Matrix3d A;
		A << 
			P[0][0], P[0][1], P[0][2],
			P[1][0], P[1][1], P[1][2],
			P[2][0], P[2][1], P[2][2];
		if (n > 0.5) {
			A = A.pow(std::fmod(n, 0.5));
			n -= fmod(n, 0.5); 
		} else {
			A = A.pow(n);
			n = 0;
		}

		result *= glm::mat3(
			A(0, 0), A(0, 1), A(0, 2), 
			A(1, 0), A(1, 1), A(1, 2), 
			A(2, 0), A(2, 1), A(2, 2));
	}

	return result;
}

//-----------------------------------------------------------------------------
MatrixPowerInterpolator::MatrixPowerInterpolator(const crd2& c1, const crd2& c2) : start(c1) {
	auto c3 = space2(c1).to(c2);
	P = getFromMatrix(c3);
}

//-----------------------------------------------------------------------------
crd2 MatrixPowerInterpolator::interpolate(double pos) const {
	auto Pp = pow(P, pos);
	auto c3 = getToCrd(Pp);
	return start.from(c3);
}

}