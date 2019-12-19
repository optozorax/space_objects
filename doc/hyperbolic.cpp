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

struct complex_matrix22 {
	complex<double> a, b, c, d;
} to_circle_m = { 1, -1i, 1, 1i };

complex_matrix22 to_circle_end = { 1i, 1, 1, 1i };

vec2 to_circle(const vec2& pos) {
	auto z = to(pos);
	return to((to_circle_m.a * z + to_circle_m.b) / (to_circle_m.c * z + to_circle_m.d));
	return pos;
}

vec2 calc_coords(const matrix22& m, const vec2& pos) {
	complex<double> z = to(pos);
	return to((m.a * z + m.b) / (m.c * z + m.d));
}

double sqr(const double& a) {
	return a * a;
}

class graph {
public:
	int find_add(const vec2& a, bool isAdd) {
		for (int i = 0; i < points.size(); i++) {
			if (isNear(points[i], a))
				return i;
		}
		if (isAdd) {
			points.push_back(a);
			return points.size()-1;
		} else {
			return -1;
		}
		
	}

	void add(const vec2& a, const vec2& b) {
		int ai = find_add(a, true);
		int bi = find_add(b, true);

		for (auto& i : edges) {
			if ((i.first == ai && i.second == bi) || (i.first == bi && i.second == ai)) {
				return;
			}
		}

		edges.push_back({ ai, bi });
	}

	void calc(void) {
		double a = 1.0 / 4.0 * (1.0 + sqrt(5.0)) * (2.0 + sqrt(2 * sqrt(5.0) - 2.0));

		matrix22 e{ 1, 0, 0, 1 };
		matrix22 r{ 1, 1, -1, 1 };
		matrix22 t{ 0, a, -1, 0 };

		function<void(const vec2&, const matrix22&, const vec2&, int)> draw_squares;
		draw_squares = [&](const vec2& pos, const matrix22& m, const vec2& last, int depth) {
			vec2 next = calc_coords(m, pos);
			add(last, next);
			if (depth > 8)
				return;
			draw_squares(pos, m * t, next, depth + 1);
			draw_squares(pos, m * r * t, next, depth + 1);
			draw_squares(pos, m * r * r * t, next, depth + 1);
			draw_squares(pos, m * r * r * r * t, next, depth + 1);
		};

		vec2 pos(0, 1);
		draw_squares(pos, e, pos, 0);
	}

	void draw(Image& image) const {
		auto draw_line = [&](const vec2& a, const vec2& b) {
			double count = 10;
			image.set_pen(0.01, Blue);
			if (fabs(a.x - b.x) < 0.0001) {
				vec2 last = a;
				for (int i = 0; i <= count; i++) {
					double t = i / count;
					vec2 current = a + (b - a) * t;
					image.draw_line(to_circle(last), to_circle(current));
					last = current;
				}
			}
			else {
				// Линия - окружность (p, 0) с радиусом r в модели верхней полуплоскости
				double p = (b.lengthSqr() - a.lengthSqr()) / (2 * b.x - 2 * a.x);
				double r = sqrt(sqr(a.x - p) + sqr(a.y));
				vec2 center(p, 0);

				double start = getRightAngle(center + vec2(r, 0), center, a);
				double end = getRightAngle(center + vec2(r, 0), center, b);

				vec2 last = a;
				for (int i = 0; i <= count; i++) {
					double t = i / count;
					double angle = start + (end - start) * t;
					vec2 current(cos(angle), sin(angle));
					current *= r;
					current += center;

					image.draw_line(to_circle(last), to_circle(current));
					last = current;
				}
			}
		};

		auto draw = [&](const vec2& pos) {
			image.draw_circle(to_circle(pos), 0.01, Black);
		};

		for (auto& i : edges) {
			draw_line(points[i.first], points[i.second]);
		}

		for (auto& i : points) {
			draw(i);
		}
	}

	vector<vec2> points;
	vector<pair<int, int>> edges;
};

int main() {
	auto [viewport, size] = calcViewPortAndSize(vec2(-3, -1.1), vec2(3, 3), 1000);
	Image image(size, getStandardCrd2());
	image.setViewPort(viewport);

	ImageGif gif;
	gif.start(size, "hyperbolic.gif");

	graph g;
	g.calc();

	double count = 100;
	double overpercent = 0.2;
	for (double i = -count * overpercent; i <= count + count * overpercent; i++) {
		image.alpha = 255;
		image.clear(twg::White);
		image.set_pen(1.5 / 80.0, setAlpha(Gray, 192));
		image.draw_grid(getStandardCrd2(), 0);

		cout << i << endl;
		double t = i / count;

		bool maxSleep = false;
		if (t > 1) {
			maxSleep = (t - 1) == overpercent;
		}
		if (t < 0) {
			maxSleep = (-t) == overpercent;
		}

		if (t < 0) t = 0;
		if (t > 1) t = 1;
		t = sin((t-0.5) * _SPOB_PI)/2 + 0.5;

		complex_matrix22 ec = { 1i, 0, 0, 1i };
		to_circle_m.a = ec.a + (to_circle_end.a - ec.a) * t;
		to_circle_m.b = ec.b + (to_circle_end.b - ec.b) * t;
		to_circle_m.c = ec.c + (to_circle_end.c - ec.c) * t;
		to_circle_m.d = ec.d + (to_circle_end.d - ec.d) * t;
	
		g.draw(image);

		gif.process(*image.imgs[0], maxSleep ? 90 : 3);
	}
	gif.end();

	return 0;
}