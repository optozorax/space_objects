#include <exception>
#include <spob/intersection.h>

namespace spob
{

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