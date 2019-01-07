#include <spob/coordinates3.h>

namespace spob
{

//-----------------------------------------------------------------------------
crd3::crd3() : i(1, 0, 0), j(0, 1, 0), k(0, 0, 1), pos(0) {
}

//-----------------------------------------------------------------------------
crd3::crd3(const vec3& i, const vec3& j, const vec3& k, const vec3& pos) : i(i), j(j), k(k), pos(pos) {
}

//-----------------------------------------------------------------------------
void crd3::invert(void) {
	k.negate();
}

//-----------------------------------------------------------------------------
void crd3::normalize(void) {
	i.normalize();
	j.normalize();
	k.normalize();
}

//-----------------------------------------------------------------------------
void crd3::orthogonalize(void) {
	i = i;
	j = j - projection(j, i);
	k = k - projection(k, i) - projection(k, j);
}

//-----------------------------------------------------------------------------
void crd3::orthonormalize(void) {
	orthogonalize();
	normalize();
}

//-----------------------------------------------------------------------------
void crd3::place(const vec3& to) {
	pos = to;
}

//-----------------------------------------------------------------------------
void crd3::move(const vec3& offset) {
	pos += offset;
}

//-----------------------------------------------------------------------------
bool crd3::isRight(void) const {
	// @TODO test with non-orthonormalized crd
	crd3 copy = *this;
	copy.orthonormalize();
	return isCoDirectional(copy.k, cross(copy.i, copy.j));
}

//-----------------------------------------------------------------------------
bool crd3::isOrthogonal(void) const {
	return isPerpendicular(i, j) &&
		   isPerpendicular(i, k) &&
		   isPerpendicular(j, k);
}

//-----------------------------------------------------------------------------
bool crd3::isOrthonormal(void) const {
	return isOrthogonal() && 
		   isNear(i, vec3(i).normalize()) &&
		   isNear(j, vec3(j).normalize()) &&
		   isNear(k, vec3(k).normalize());
}

//-----------------------------------------------------------------------------
crd3 getStandardCrd3(void) {
	return crd3(vec3(1, 0, 0), vec3(0, 1, 0), vec3(0, 0, 1), vec3(0, 0, 0));
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
space3::space3() : crd3() {
}

//-----------------------------------------------------------------------------
space3::space3(const crd3& crd) {
	operator=(crd);
}

//-----------------------------------------------------------------------------
space3::space3(const vec3& i, const vec3& j, const vec3& k, const vec3& pos) : crd3(i, j, k, pos) {
}

//-----------------------------------------------------------------------------
space3& space3::operator=(const crd3& crd) {
	i = crd.i;
	j = crd.j;
	k = crd.k;
	pos = crd.pos;
	return *this;
}

//-----------------------------------------------------------------------------
vec3 space3::to(const vec3& o) const {
	return toDir(o - pos);
}

//-----------------------------------------------------------------------------
vec3 space3::toDir(const vec3& o) const {
	// Решается векторное уравнение i*result.x + j*result.y + k*result.z = o
	vec3 result;
	double a = k.y*j.z - j.y*k.z;
	double b = i.y*k.z - k.y*i.z;
	double c = j.y*i.z - i.y*j.z;
	double down = i.x*a + j.x*b + k.x*c;
	double d = j.y*o.z - o.y*j.z;
	double e = o.y*k.z - k.y*o.z;
	double f = i.y*o.z - o.y*i.z;
	double g = o.y*j.z - j.y*o.z;
	double h = k.y*i.z - i.y*k.z;
	result.x = (j.x*e + k.x*d + o.x*a)/down;
	result.y = -(i.x*e + k.x*f + o.x*h)/down;
	result.z = (i.x*g + j.x*f + o.x*c)/down;
	return result;
}

//-----------------------------------------------------------------------------
vec3 space3::from(const vec3& o) const {
	return pos + i*o.x + j*o.y + k*o.z;
}

//-----------------------------------------------------------------------------
vec3 space3::fromDir(const vec3& d) const {
	return from(d) - pos;
}

//-----------------------------------------------------------------------------
crd3 space3::to(const crd3& crd) const {
	crd3 result;
	result.i = toDir(crd.i);
	result.j = toDir(crd.j);
	result.k = toDir(crd.k);
	result.pos = to(crd.pos);
	return result;
}

//-----------------------------------------------------------------------------
crd3 space3::from(const crd3& crd) const {
	crd3 result;
	result.i = fromDir(crd.i);
	result.j = fromDir(crd.j);
	result.k = fromDir(crd.k);
	result.pos = from(crd.pos);
	return result;
}

//-----------------------------------------------------------------------------
crd3 invert(const crd3& crd) {
	return space3(crd).to(getStandardCrd3());
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
plane3::plane3() : crd3() {
}

//-----------------------------------------------------------------------------
plane3::plane3(const crd3& crd) {
	operator=(crd);
}

//-----------------------------------------------------------------------------
plane3::plane3(const vec3& i, const vec3& j, const vec3& pos) : crd3(i, j, cross(i, j), pos) {
}

//-----------------------------------------------------------------------------
plane3::plane3(const vec3& i, const vec3& j, const vec3& k, const vec3& pos) : crd3(i, j, k, pos) {
} 

//-----------------------------------------------------------------------------
plane3& plane3::operator=(const crd3& crd) {
	i = crd.i;
	j = crd.j;
	k = crd.k;
	pos = crd.pos;
	return *this;
}

//-----------------------------------------------------------------------------
vec2 plane3::to(const vec3& o) const {
	return toDir(o - pos);
}

//-----------------------------------------------------------------------------
vec2 plane3::toDir(const vec3& o) const {
	// Решается векторное уравнение i*result.x + j*result.y + k*result.z = o, но координата z игнорируется
	vec2 result;
	double a = k.y*j.z - j.y*k.z;
	double b = i.y*k.z - k.y*i.z;
	double c = j.y*i.z - i.y*j.z;
	double down = i.x*a + j.x*b + k.x*c;
	double d = j.y*o.z - o.y*j.z;
	double e = o.y*k.z - k.y*o.z;
	double f = i.y*o.z - o.y*i.z;
	double h = k.y*i.z - i.y*k.z;
	result.x = (j.x*e + k.x*d + o.x*a)/down;
	result.y = -(i.x*e + k.x*f + o.x*h)/down;
	return result;
}

//-----------------------------------------------------------------------------
vec3 plane3::from(const vec2& o) const {
	return pos + i*o.x + j*o.y;
}

//-----------------------------------------------------------------------------
vec3 plane3::fromDir(const vec2& d) const {
	return from(d) - pos;
}

//-----------------------------------------------------------------------------
line2 plane3::to(const line3& o) const {
	return line2(to(o.pos), toDir(o.i));
}

//-----------------------------------------------------------------------------
line3 plane3::from(const line2& o) const {
	return line3(from(o.pos), fromDir(o.i));
}

//-----------------------------------------------------------------------------
vec3 plane3::project(const vec3& o) const {
	return from(to(o));
}

//-----------------------------------------------------------------------------
vec3 plane3::projectDir(const vec3& d) const {
	return fromDir(toDir(d));
}

//-----------------------------------------------------------------------------
plane3 makePlane3(const vec3& pos, const vec3& toI, const vec3& toJ) {
	return plane3(toI - pos, toJ - pos, pos);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
line3::line3() : crd3() {
}

//-----------------------------------------------------------------------------
line3::line3(const crd3& crd) {
	operator=(crd);
}

//-----------------------------------------------------------------------------
line3::line3(const vec3& i1, const vec3& pos1) {
	i = i1;
	pos = pos1;
	if (i.z != 0)
		j = vec3(1, 1, -(i.x + i.y) / i.z);
	else if (i.y != 0)
		j = vec3(1, -(i.x + i.z) / i.y, 1);
	else
		j = vec3(-(i.y + i.z) / i.x, 1, 1);
	j.normalize();
	j *= i.length();
	k = cross(i, j);
	k.normalize();
	k *= i.length();
}

//-----------------------------------------------------------------------------
line3::line3(const vec3& i, const vec3& j, const vec3& k, const vec3& pos) : crd3(i, j, k, pos) {
} 

//-----------------------------------------------------------------------------
line3& line3::operator=(const crd3& crd) {
	i = crd.i;
	j = crd.j;
	k = crd.k;
	pos = crd.pos;
	return *this;
}

//-----------------------------------------------------------------------------
double line3::to(const vec3& o) const {
	return toDir(o - pos);
}

//-----------------------------------------------------------------------------
double line3::toDir(const vec3& o) const {
	// Решается векторное уравнение i*result.x + j*result.y + k*result.z = o, но координаты y и z игнорируется
	vec2 result;
	double a = k.y*j.z - j.y*k.z;
	double b = i.y*k.z - k.y*i.z;
	double c = j.y*i.z - i.y*j.z;
	double down = i.x*a + j.x*b + k.x*c;
	double d = j.y*o.z - o.y*j.z;
	double e = o.y*k.z - k.y*o.z;
	return (j.x*e + k.x*d + o.x*a)/down;
}

//-----------------------------------------------------------------------------
vec3 line3::from(const double& o) const {
	return pos + i*o;
}

//-----------------------------------------------------------------------------
vec3 line3::fromDir(const double& d) const {
	return from(d) - pos;
}

//-----------------------------------------------------------------------------
vec3 line3::project(const vec3& o) const {
	return from(to(o));
}

//-----------------------------------------------------------------------------
vec3 line3::projectDir(const vec3& d) const {
	return fromDir(toDir(d));
}

//-----------------------------------------------------------------------------
line3 makeLine3(const vec3& pos, const vec3& toI) {
	return line3(toI - pos, pos);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
bool isPerpendicular(const plane3& a, const plane3& b) {
	return isPerpendicular(a.k, b.k);
}

//-----------------------------------------------------------------------------
bool isPerpendicular(const plane3& plane, const line3& line) {
	return isPerpendicular(plane.k, line.i);
}

//-----------------------------------------------------------------------------
bool isPerpendicular(const plane3& plane, const vec3& d) {
	return isCollinear(plane.k, d);
}

//-----------------------------------------------------------------------------
bool isPerpendicular(const line3& a, const line3& b) {
	return isPerpendicular(a.i, b.i);
}

//-----------------------------------------------------------------------------
bool isPerpendicular(const line3& line, const vec3& d) {
	return isPerpendicular(line.i, d);
}

//-----------------------------------------------------------------------------
bool isIntersect(const plane3& a, const plane3& b) {
	return !isParallel(a, b);
}

//-----------------------------------------------------------------------------
bool isIntersect(const plane3& plane, const line3& line) {
	return !isParallel(plane, line);
}

//-----------------------------------------------------------------------------
bool isIntersect(const line3& a, const line3& b) {
	const double precision = 0.0001;
	return !isParallel(a, b) && distance(plane3(a.i, b.i, a.pos), plane3(a.i, b.i, b.pos)) < precision;
}

//-----------------------------------------------------------------------------
bool isContain(const plane3& a, const plane3& b) {
	return isParallel(a, b) && isContain(a, b.pos);
}

//-----------------------------------------------------------------------------
bool isContain(const plane3& plane, const line3& line) {
	return isParallel(plane, line) && isContain(plane, line.pos);
}

//-----------------------------------------------------------------------------
bool isContain(const plane3& plane, const vec3& o) {
	return isParallel(plane, plane.pos - o) || isNear(plane.pos, o);
}

//-----------------------------------------------------------------------------
bool isContain(const line3& a, const line3& b) {
	return isParallel(a, b) && isContain(a, b.pos);
}

//-----------------------------------------------------------------------------
bool isContain(const line3& line, const vec3& o) {
	return isCollinear(line.i, line.pos - o) || isNear(line.pos, o);
}

//-----------------------------------------------------------------------------
bool isParallel(const plane3& a, const plane3& b) {
	return isCollinear(a.k, b.k);
}

//-----------------------------------------------------------------------------
bool isParallel(const plane3& plane, const line3& line) {
	return isPerpendicular(plane.k, line.i);
}

//-----------------------------------------------------------------------------
bool isParallel(const plane3& plane, const vec3& d) {
	return isPerpendicular(plane.k, d);
}

//-----------------------------------------------------------------------------
bool isParallel(const line3& a, const line3& b) {
	return isCollinear(a.i, b.i);
}

//-----------------------------------------------------------------------------
bool isParallel(const line3& line, const vec3& d) {
	return isCollinear(line.i, d);
}

//-----------------------------------------------------------------------------
bool isSkew(const line3& a, const line3& b) {
	return !isIntersect(a, b) && !isParallel(a, b);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
double distance(const plane3& a, const plane3& b) {
	#ifdef _DEBUG
	if (!a.isOrthogonal())
		throw std::exception();
	#endif
	if (isIntersect(a, b))
		return 0;
	else
		return distance(a, b.pos);
}

//-----------------------------------------------------------------------------
double distance(const plane3& plane, const line3& line) {
	#ifdef _DEBUG
	if (!plane.isOrthogonal())
		throw std::exception();
	#endif
	if (isIntersect(plane, line))
		return 0;
	else
		return distance(plane, line.pos);	
}

//-----------------------------------------------------------------------------
double distance(const plane3& plane, const vec3& o) {
	#ifdef _DEBUG
	if (!plane.isOrthogonal())
		throw std::exception();
	#endif
	if (isContain(plane, o))
		return 0;
	else
		return distance(o, plane.project(o));
}

//-----------------------------------------------------------------------------
double distance(const line3& a, const line3& b) {
	#ifdef _DEBUG
	if (!a.isOrthogonal())
		throw std::exception();
	#endif
	if (isIntersect(a, b))
		return 0;
	else
		if (isParallel(a, b))
			return distance(b.pos, a.project(b.pos));
		else
			return distance(plane3(a.i, b.i, a.pos), plane3(a.i, b.i, b.pos));
}

//-----------------------------------------------------------------------------
double distance(const line3& line, const vec3& o) {
	#ifdef _DEBUG
	if (!line.isOrthogonal())
		throw std::exception();
	#endif
	if (isContain(line, o))
		return 0;
	else
		return distance(o, line.project(o));
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
vec3 rotate(const vec3& o, const line3& around, double angle) {
	space3 space(around);
	vec3 pos = space.to(o);
	vec2 pos2(pos.y, pos.z);
	pos2 = rotate(pos2, vec2(0), angle);
	pos.y = pos2.x;
	pos.z = pos2.y;
	return space.from(pos);
}

//-----------------------------------------------------------------------------
vec3 rotateDir(const vec3& d, const line3& around, double angle) {
	return rotate(d + around.pos, around, angle) - around.pos;
}

//-----------------------------------------------------------------------------
crd3 rotate(const crd3& crd, const line3& around, double angle) {
	crd3 result;
	result.i = rotateDir(crd.i, around, angle);
	result.j = rotateDir(crd.j, around, angle);
	result.k = rotateDir(crd.k, around, angle);
	result.pos = rotate(crd.pos, around, angle);
	return result;
}

//-----------------------------------------------------------------------------
inline void rotate2(double& x, double &y, const double& angle) {
	double x1 = std::cos(angle)*x - std::sin(angle)*y;
	double y1 = std::sin(angle)*x + std::cos(angle)*y;
	x = x1;
	y = y1;
}

//-----------------------------------------------------------------------------
vec3 rotate(const vec3& o, const vec3& angles) {
	vec3 result = o;
	rotate2(result.x, result.y, angles.x);
	rotate2(result.x, result.z, angles.z);
	rotate2(result.y, result.z, -angles.y);
	return result;
}

//-----------------------------------------------------------------------------
crd3 rotate(const crd3& crd, const vec3& angles) {
	crd3 result;
	result.pos = crd.pos;
	result.i = rotate(crd.i, angles);
	result.j = rotate(crd.j, angles);
	result.k = rotate(crd.k, angles);
	return result;
}

};