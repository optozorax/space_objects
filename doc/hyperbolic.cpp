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

int main() {
	auto [viewport, size] = calcViewPortAndSize(vec2(-1.2, -1.2), vec2(1.2, 1.2), 500);
	Image image(size, getStandardCrd2());
	image.setViewPort(viewport);

	image.alpha = 255;
	image.clear(twg::White);
	image.set_pen(1.5 / 80.0, setAlpha(Gray, 192));
	image.draw_grid(getStandardCrd2(), 0);

	image.draw_circle(vec2(0, 0), 1, Black);

	auto draw = [&](const vec2& pos) {
		image.draw_circle(to_circle(pos), 0.01, White);
	};

	double a = 1.0/4.0 * (1.0 + sqrt(5.0)) * (2.0 + sqrt(2 * sqrt(5.0) - 2.0));
	cout << a << endl;

	matrix22 r{ 1, 1, -1, 1 };
	matrix22 t{ 0, a, -1, 0 };

	function<void(const vec2&, int)> draw_squares;
	draw_squares = [&](const vec2& pos, int depth) {
		draw(pos);
		if (depth > 6)
			return;
		draw_squares(calc_coords(t, pos), depth+1);
		draw_squares(calc_coords(r * t, pos), depth + 1);
		draw_squares(calc_coords(r * r * t, pos), depth + 1);
		draw_squares(calc_coords(r * r * r * t, pos), depth + 1);
	};

	draw_squares(vec2(0, 1), 0);

	image.save("hyperbolic.png");

	return 0;
}