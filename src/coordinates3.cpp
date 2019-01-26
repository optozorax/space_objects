#include <spob/coordinates3.h>

namespace spob
{

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
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

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
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

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
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

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

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
Inter intersect(const line3& a, const line3& b,
				double& t1, double& t2) {
	if (isIntersect(a, b)) {
		if (!isContain(a, b)) {
			// Работать будет по принципу: создается плоскость, в которой лежат обе эти прямые. Она ортонормализуется, и на неё проецируются прямые, то есть всё сводится к двумерному случаю. И далее вызывается функция для двумерного случая.
			plane3 plane(a.pos, a.i, b.i);
			plane.orthonormalize();
			line2 a2(plane.to(a.pos), plane.toDir(a.i));
			line2 b2(plane.to(b.pos), plane.toDir(b.i));
			intersect(a2, b2, t1, t2);
		} else
			return INTER_COINCIDES;
	} else
		return INTER_ABSENT;
}

//-----------------------------------------------------------------------------
Inter intersect(const line3& a, const line3& b,
				vec3& pos) {
	double t1 = 0, t2 = 0;
	auto returned = intersect(a, b, t1, t2);
	if (returned == INTER_OVERLAPS)
		pos = a.from(t1);
	return returned;
}

//-----------------------------------------------------------------------------
Inter intersect(const plane3& plane, const line3& line,
				vec2& pos, double& t) {
	if (isIntersect(plane, line)) {
		if (!isContain(plane, line)) {
			t = dot(plane.k, plane.pos - line.pos)/dot(plane.k, line.i);
			pos = plane.to(line.from(t));

			return INTER_OVERLAPS;
		} else
			return INTER_COINCIDES;
	} else
		return INTER_ABSENT;
}

//-----------------------------------------------------------------------------
Inter intersect(const plane3& plane, const line3& line,
				vec3& pos) {
	vec2 pos1 = {};
	double t = 0;
	auto returned = intersect(plane, line, pos1, t);
	if (returned == INTER_OVERLAPS)
		pos = plane.from(pos1);
	return returned;
}

//-----------------------------------------------------------------------------
Inter intersect(const plane3& plane1, const plane3& plane2,
				line2& l1, line2& l2) {
	if (isIntersect(plane1, plane2)) {
		if (!isContain(plane1, plane2)) {
			// Работать будет по принципу: у плоскости есть две ортогональные оси - i, j. Если из них образовать прямые, то хотя бы одна из них должна пересекать плоскость, иначе плоскости параллельны. И на основе данных об пересечении этими линиями и строится линия пересечения плоскостей
			vec2 o1, d1, o2, d2;

			vec2 pos1 = {};
			double t1 = 0;
			auto ret1 = intersect(plane2, line3(plane1.i, plane1.j, plane1.k, plane1.pos), pos1, t1);

			vec2 pos2 = {};
			double t2 = 0;
			auto ret2 = intersect(plane2, line3(plane1.j, plane1.i, plane1.k, plane1.pos), pos2, t2);

			pos1 = vec2(t1, 0);
			pos2 = vec2(0, t2);

			if (ret1 == INTER_OVERLAPS && ret2 == INTER_COINCIDES) {
				o1 = vec2(0, 0);
				d1 = vec2(0, 1);
			} else if (ret1 == INTER_COINCIDES && ret2 == INTER_OVERLAPS) {
				o1 = vec2(0, 0);
				d1 = vec2(1, 0);
			} else if (ret1 == INTER_OVERLAPS && ret2 == INTER_ABSENT) {
				o1 = pos1;
				d1 = vec2(0, 1);
			} else if (ret1 == INTER_ABSENT && ret2 == INTER_OVERLAPS) {
				o1 = pos2;
				d1 = vec2(1, 0);
			} else if (ret1 == INTER_OVERLAPS && ret2 == INTER_OVERLAPS) {
				o1 = pos1;
				d1 = (pos2 - pos1).normalize();
			} else {
				throw std::exception();
			}

			o2 = plane2.to(plane1.from(o1));
			d2 = plane2.toDir(plane1.fromDir(d1));
			d2.normalize();

			l1 = line2(d1, o1);
			l2 = line2(d2, o2);
			return INTER_OVERLAPS;
		} else
			return INTER_COINCIDES;
	} else
		return INTER_ABSENT;
}

//-----------------------------------------------------------------------------
Inter intersect(const plane3& plane1, const plane3& plane2,
				line3& line) {
	line2 l1, l2;
	auto returned = intersect(plane1, plane2, l1, l2);
	if (returned == INTER_OVERLAPS)
		line = line3(plane1.fromDir(l1.i), plane1.from(l1.pos));
	return returned;
}

};