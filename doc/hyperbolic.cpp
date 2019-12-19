#include <iostream>
#include <fstream>
#include <vector>
#include <complex>

#include <spob/spob.h>
#include "draw.h"

using namespace spob;
using namespace std;

struct matrix22 {
	double a, b, c, d;
};

matrix22 operator*(const matrix22& a, const matrix22& b) {
	return { a.a * b.a + a.b * b.c, a.a * b.b + a.b * b.d, a.c * b.a + a.d * b.c, a.c * b.b + a.d * b.d };
}

ostream& operator<<(ostream& out, const matrix22& a) {
	out << "(" << a.a << ", " << a.b << "; " << a.c << ", " << a.d << ")";
	return out;
}

complex<double> to(const vec2& pos) {
	return complex<double>(pos.x, pos.y);
}

vec2 to(const complex<double>& z) {
	return vec2(z.real(), z.imag());
}

vec2 to_circle(const vec2& pos) {
	auto z = to(pos);
	return to((z - 1i) / (z + 1i));
}

vec2 calc_coords(const matrix22& m, const vec2& pos) {
	complex<double> z = to(pos);
	return to((m.a * z + m.b) / (m.c * z + m.d));
}

double sqr(const double& a) {
	return a * a;
}

int main() {
	auto [viewport, size] = calcViewPortAndSize(vec2(-1.2, -1.2), vec2(1.2, 1.2), 500);
	Image image(size, getStandardCrd2(), 2);
	image.setViewPort(viewport);

	image.alpha = 255;
	image.clear(twg::White);
	image.set_pen(1.5 / 80.0, setAlpha(Gray, 192));
	image.draw_grid(getStandardCrd2(), 0);

	image.draw_circle(vec2(0, 0), 1.03, Black);

	auto draw = [&](const vec2& pos) {
		image.draw_circle(to_circle(pos), 0.01, White, 1);
	};

	auto draw_line = [&](const vec2& a, const vec2& b) {
		// Линия - окружность (p, 0) с радиусом r в модели верхней полуплоскости
		double p = (b.lengthSqr() - a.lengthSqr())/(2*b.x - 2*a.x);
		double r = sqrt(sqr(a.x - p) + sqr(a.y));
		vec2 center(p, 0);

		double start = getRightAngle(center + vec2(r, 0), center, a);
		double end = getRightAngle(center + vec2(r, 0), center, b);

		image.set_pen(0.005, Miku);

		double count = 10;
		vec2 last = a;
		for (int i = 0; i <= count; i++) {
			double t = i / count;
			double angle = start + (end - start) * t;
			vec2 current(cos(angle), sin(angle));
			current *= r;
			current += center;

			image.draw_line(to_circle(last), to_circle(current));
			last = current;
;		}
	};

	double a = 1.0 / 4.0 * (1.0 + sqrt(5.0)) * (2.0 + sqrt(2 * sqrt(5.0) - 2.0));

	matrix22 e{ 1, 0, 0, 1 };
	matrix22 r{ 1, 1, -1, 1 };
	matrix22 t{ 0, a, -1, 0 };

	function<void(const vec2&, const matrix22&, const vec2&, int)> draw_squares;
	draw_squares = [&](const vec2& pos, const matrix22& m, const vec2& last, int depth) {
		vec2 next = calc_coords(m, pos);
		draw(next);
		draw_line(last, next);
		if (depth > 6)
			return;
		draw_squares(pos, m * t, next, depth + 1);
		draw_squares(pos, m * r * t, next, depth + 1);
		draw_squares(pos, m * r * r * t, next, depth + 1);
		draw_squares(pos, m * r * r * r * t, next, depth + 1);
	};

	vec2 pos(0, 1);
	draw_squares(pos, e, pos, 0);

	image.combine_layers();
	image.save("hyperbolic.png");

	return 0;
}