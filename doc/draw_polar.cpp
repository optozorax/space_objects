#include <iostream>
#include <fstream>
#include <vector>
#include <complex>

#include <spob/spob.h>
#include <spob/polar2.h>
#include "draw.h"

using namespace spob;
using namespace std;

int main() {
	auto [viewport, size] = calcViewPortAndSize(vec2(-3, -1.1), vec2(3, 3), 1000);
	Image image(size, getStandardCrd2());
	image.setViewPort(viewport);

	image.alpha = 255;
	image.clear(twg::White);
	//image.set_pen(1.5 / 80.0, setAlpha(Gray, 192));
	//image.draw_grid(getStandardCrd2(), 0);

	disk2 disk(vec2(0.5, 1), vec2(3, 0.1));

	auto a = disk.tr.pos;
	auto b = disk.from(polar2(1, 0));
	image.set_pen(0.01, Gray);

	double count1 = 30;
	for (int j = 0; j <= count1; j++) {
		double r = j * 0.2;

		if (r == 1) {
			image.set_pen(0.02, Black);
		} else if (fmod(r, 1.0) == 0) {
			image.set_pen(0.01, setAlpha(Black, 128));
		} else {
			image.set_pen(0.002, setAlpha(Gray, 32));
		}

		double count = 100;
		vec2 last = disk.from(polar2(r, 0));
		for (int i = 0; i <= count; i++) {
			double al = i / count * 2.0 * _SPOB_PI;
			vec2 current = disk.from(polar2(r, al));
			image.draw_line(last, current);
			last = current;
		}
	}

	image.set_pen(0.01, Gray);
	double countArrow = 12 * 5;
	for (int i = 0; i < countArrow; i++) {
		double al = _SPOB_PI * 2.0 * i / countArrow;

		if (i == 0) {
			image.set_pen(0.02, Black);
		}
		else if (i % 5 == 0) {
			image.set_pen(0.01, setAlpha(Black, 128));
		}
		else {
			image.set_pen(0.002, setAlpha(Gray, 32));
		}

		image.draw_line(a, disk.from(polar2(5, al)));
	}

	image.set_pen(0.02, Black);
	image.draw_arrow(a, disk.from(polar2(1.5, 0)), spob::deg2rad(45), 0.1);

	image.draw_circle(a, 0.03, Black);

	image.save("polar.png");

	return 0;
}