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
glm::mat3 interpolate(const glm::mat3& P, double n) {
	n = 1;
	double a = P[0][0], b = P[1][0], c = P[2][0];
	double d = P[0][0], x = P[1][0], y = P[2][0];
	double z = std::sqrt(a*a-2*x*a+x*x+4*b*d);
	double h = std::pow(a+x-z, n);
	double g = std::pow(a+x+z, n);
	double t = std::pow(2.0, 1-n);
	double p = std::pow(2.0, 2+n);
	double u = std::pow(2.0, n);
	double k = std::pow(2.0, -n);
	double j = std::pow(2.0, 2-n);
	double i = std::pow(2.0, n+1);

	glm::vec3 v1(
		-(t*(a*h-x*h-z*h-a*g+x*g-z*g))/z,
		-(j*(d*h-d*g))/z,
		0
	);

	glm::vec3 v2(
		-(k*(4*b*h-4*b*g))/z, 
		(t*(a*h-x*h+z*h-a*g+x*g+z*g))/z, 
		0
	);

	glm::vec3 v3(
		(2*c*x*x*h+2*a*c*h+4*b*c*d*h-2*a*c*x*h-2*c*x*h-2*a*b*y*h+4*b*y*h-2*b*x*y*h-2*b*z*y*h-2*c*z*h+2*c*x*z*h-2*c*x*x*g-2*a*c*g-4*b*c*d*g+2*a*c*x*g+2*c*x*g-2*c*z*g+2*c*x*z*g+2*a*b*g*y-4*b*g*y+2*b*x*g*y-2*b*z*g*y+p*b*z*y+p*c*z-p*c*x*z)/(-u*z*a+u*x*z*a+u*z-u*b*d*z-u*x*z), 
		(2*(-a*c*d*h+2*c*d*h-c*d*x*h+a*a*y*h-a*y*h+2*b*d*y*h-a*x*y*h+x*y*h+a*z*y*h-z*y*h-c*d*z*h+a*c*d*g-2*c*d*g+c*d*x*g-c*d*z*g-a*a*g*y+a*g*y-2*b*d*g*y+a*x*g*y-x*g*y+a*z*g*y-z*g*y+i*z*y-i*a*z*y+i*c*d*z))/(-u*z*a+u*x*z*a+u*z-u*b*d*z-u*x*z), 
		4
	);

	glm::mat3 result(v1, v2, v3);

	result /= 4;

	return result;
}

}