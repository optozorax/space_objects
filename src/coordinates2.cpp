#include <cmath>
#include <spob/coordinates2.h>

namespace spob
{

//-----------------------------------------------------------------------------
bool crd2::isRight(void) const {
	// @TODO test with non-orthonormalized crd
	crd2 copy = *this;
	copy.orthonormalize();
	return isCoDirectional(copy.j, cross(copy.i));
}

//-----------------------------------------------------------------------------
bool crd2::isOrthonormal(void) const {
	return isOrthogonal() && 
		   isNear(i, vec2(i).normalize()) &&
		   isNear(j, vec2(j).normalize());
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

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
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
double line2::toDir(const vec2& o) const {
	// Решается векторное уравнение i*result.x + j*result.y = o, но координата y игнорируется
	vec2 result;
	double down = j.x*i.y - i.x*j.y;
	return -(o.x*j.y - j.x*o.y)/down;
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

};