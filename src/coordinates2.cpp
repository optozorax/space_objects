#include <cmath>
#include <spob/coordinates2.h>

namespace spob
{

//-----------------------------------------------------------------------------
crd2::crd2() : i(1, 0), j(0, 1), pos(0) {
}

//-----------------------------------------------------------------------------
crd2::crd2(const vec2& i, const vec2& j, const vec2& pos) : i(i), j(j), pos(pos) {
}

//-----------------------------------------------------------------------------
void crd2::invert(void) {
	j.negate();
}

//-----------------------------------------------------------------------------
void crd2::normalize(void) {
	i.normalize();
	j.normalize();
}

//-----------------------------------------------------------------------------
void crd2::orthogonalize(void) {
	i = i;
	j = j - projection(j, i);
}

//-----------------------------------------------------------------------------
void crd2::orthonormalize(void) {
	orthogonalize();
	normalize();
}

//-----------------------------------------------------------------------------
void crd2::place(const vec2& to) {
	pos = to;
}

//-----------------------------------------------------------------------------
void crd2::move(const vec2& offset) {
	pos += offset;
}

//-----------------------------------------------------------------------------
bool crd2::isRight(void) const {
	// @TODO test with non-orthonormalized crd
	crd2 copy = *this;
	copy.orthonormalize();
	return isCoDirectional(copy.j, cross(copy.i));
}

//-----------------------------------------------------------------------------
bool crd2::isOrthogonal(void) const {
	return isPerpendicular(i, j);
}

//-----------------------------------------------------------------------------
bool crd2::isOrthonormal(void) const {
	return isOrthogonal() && 
		   isNear(i, vec2(i).normalize()) &&
		   isNear(j, vec2(j).normalize());
}

//-----------------------------------------------------------------------------
crd2 getStandardCrd2(void) {
	return crd2(vec2(1, 0), vec2(0, 1), vec2(0, 0));
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
space2::space2() : crd2() {
}

//-----------------------------------------------------------------------------
space2::space2(const crd2& crd) {
	operator=(crd);
}

//-----------------------------------------------------------------------------
space2::space2(const vec2& i, const vec2& j, const vec2& pos) : crd2(i, j, pos) {
}

//-----------------------------------------------------------------------------
space2& space2::operator=(const crd2& crd) {
	i = crd.i;
	j = crd.j;
	pos = crd.pos;
	return *this;
}

//-----------------------------------------------------------------------------
vec2 space2::to(const vec2& o) const {
	return toDir(o - pos);
}

//-----------------------------------------------------------------------------
vec2 space2::toDir(const vec2& o) const {
	// Решается векторное уравнение i*result.x + j*result.y = o
	vec2 result;
	double down = j.x*i.y - i.x*j.y;
	result.x = -(o.x*j.y - j.x*o.y)/down;
	result.y = (o.x*i.y - i.x*o.y)/down;
	return result;
}

//-----------------------------------------------------------------------------
vec2 space2::from(const vec2& o) const {
	return pos + i*o.x + j*o.y;
}

//-----------------------------------------------------------------------------
vec2 space2::fromDir(const vec2& d) const {
	return from(d) - pos;
}

//-----------------------------------------------------------------------------
crd2 space2::to(const crd2& crd) const {
	crd2 result;
	result.i = toDir(crd.i);
	result.j = toDir(crd.j);
	result.pos = to(crd.pos);
	return result;
}

//-----------------------------------------------------------------------------
crd2 space2::from(const crd2& crd) const {
	crd2 result;
	result.i = fromDir(crd.i);
	result.j = fromDir(crd.j);
	result.pos = from(crd.pos);
	return result;	
}

//-----------------------------------------------------------------------------
crd2 invert(const crd2& crd) {
	return space2(crd).to(getStandardCrd2());
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
line2::line2() : crd2() {
}

//-----------------------------------------------------------------------------
line2::line2(const crd2& crd) {
	operator=(crd);
}

//-----------------------------------------------------------------------------
line2::line2(const vec2& i, const vec2& pos) : crd2(i, cross(i), pos) {
}

//-----------------------------------------------------------------------------
line2::line2(const vec2& i, const vec2& j, const vec2& pos) : crd2(i, j, pos) {
}

//-----------------------------------------------------------------------------
line2& line2::operator=(const crd2& crd) {
	i = crd.i;
	j = crd.j;
	pos = crd.pos;
	return *this;
}

//-----------------------------------------------------------------------------
double line2::to(const vec2& o) const {
	return toDir(o - pos);
}

//-----------------------------------------------------------------------------
double line2::toDir(const vec2& o) const {
	// Решается векторное уравнение i*result.x + j*result.y = o, но координата y игнорируется
	vec2 result;
	double down = j.x*i.y - i.x*j.y;
	return -(o.x*j.y - j.x*o.y)/down;
}

//-----------------------------------------------------------------------------
vec2 line2::from(const double& o) const {
	return pos + i*o;
}

//-----------------------------------------------------------------------------
vec2 line2::fromDir(const double& d) const {
	return from(d) - pos;
}

//-----------------------------------------------------------------------------
vec2 line2::project(const vec2& o) const {
	return from(to(o));
}

//-----------------------------------------------------------------------------
vec2 line2::projectDir(const vec2& d) const {
	return fromDir(toDir(d));
}

//-----------------------------------------------------------------------------
line2 makeLine2(const vec2& pos, const vec2& toI) {
	return line2(toI - pos, pos);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
bool isPerpendicular(const line2& a, const line2& b) {
	return isPerpendicular(a.i, b.i);
}

//-----------------------------------------------------------------------------
bool isPerpendicular(const line2& line, const vec2& d) {
	return isPerpendicular(line.i, d);
}

//-----------------------------------------------------------------------------
bool isIntersect(const line2& a, const line2& b) {
	return !isParallel(a, b);
}

//-----------------------------------------------------------------------------
bool isContain(const line2& a, const line2& b) {
	return isParallel(a, b) && isContain(a, b.pos);
}

//-----------------------------------------------------------------------------
bool isContain(const line2& line, const vec2& o) {
	return isCollinear(line.i, o - line.pos) || isNear(line.pos, o);
}

//-----------------------------------------------------------------------------
bool isParallel(const line2& a, const line2& b) {
	return isCollinear(a.i, b.i);
}

//-----------------------------------------------------------------------------
bool isParallel(const line2& line, const vec2& d) {
	return isCollinear(line.i, d);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
double distance(const line2& a, const line2& b) {
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
double distance(const line2& line, const vec2& o) {
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
vec2 rotate(const vec2& o, const vec2& around, double angle) {
	vec2 oa = o - around;
	vec2 result;
	result.x = std::cos(angle)*oa.x - std::sin(angle)*oa.y;
	result.y = std::sin(angle)*oa.x + std::cos(angle)*oa.y;
	result += around;
	return result;
}

//-----------------------------------------------------------------------------
vec2 rotateDir(const vec2& d, const vec2& around, double angle) {
	return rotate(d + around, around, angle) - around;
}

//-----------------------------------------------------------------------------
crd2 rotate(const crd2& crd, const vec2& around, double angle) {
	crd2 copy;
	copy.i = rotateDir(crd.i, around, angle);
	copy.j = rotateDir(crd.j, around, angle);
	copy.pos = rotate(crd.pos, around, angle);
	return copy;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
Inter intersect(const line2& l1, const line2& l2,
				double& t1, double& t2) {
	if (isIntersect(l1, l2)) {
		if (!isContain(l1, l2)) {
			// o_1 + i_1 * t_1 = o_2 + i_2 * t_2
			// i_1 * t_1 - i_2 * t_2 = o_2 - o_1
			// А уравнение ix + jy = o уже решено в методе space2.toDir().
			space2 temp(l1.i, -l2.i, vec2(0, 0));
			auto t = temp.toDir(l2.pos - l1.pos);
			t1 = t.x;
			t2 = t.y;

			return INTER_OVERLAPS;
		} else {
			return INTER_COINCIDES;
		}
	} else
		return INTER_ABSENT;
}

//-----------------------------------------------------------------------------
Inter intersect(const line2& a, const line2& b,
				vec2& pos) {
	double t1 = 0, t2 = 0;
	auto returned = intersect(a, b, t1, t2);
	if (returned == INTER_OVERLAPS)
		pos = a.from(t1);
	return returned;


};