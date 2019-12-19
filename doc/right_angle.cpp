#include <iostream>
#include <fstream>
#include <vector>
#include <complex>

#include <spob/spob.h>
#include <spob/polar2.h>
#include "draw.h"

using namespace spob;
using namespace std;

void draw_right_angle(Image& image, const vec2& a, const vec2& b, const vec2& c) {
	image.set_pen(0.02, Black);

	image.draw_arrow(a, b - (b - a) * 0.03, spob::deg2rad(30), 0.07);
	image.draw_arrow(b, c - (c - b) * 0.03, spob::deg2rad(30), 0.07);

	disk2 disk(b, a);
	double r = 0.15;
	double end = getRightAngle(a, b, c);

	image.set_pen(0.01, Black);

	double count = 100;
	vec2 last = disk.from(polar2(r, 0));
	for (int i = 0; i <= count; i++) {
		double t = i / count;
		auto current = disk.from(polar2(r, end * t));
		image.draw_line(last, current);
		last = current;
	}

	image.draw_circle(a, 0.03, Black);
	image.draw_circle(b, 0.03, Black);
	image.draw_circle(c, 0.03, Black);

	image.set_pen(0.01, Black);
	image.imgs[0]->setTextStyle(TextStyle(20, "Consolas", TEXT_NONE));
	image.imgs[0]->drawText(image.screen_tr.from(a) + vec2(-17, 5), "a");
	image.imgs[0]->drawText(image.screen_tr.from(b) + vec2(-19, 5), "b");
	image.imgs[0]->drawText(image.screen_tr.from(c) + vec2(5, 9), "c");
}

int main() {
	auto [viewport, size] = calcViewPortAndSize(vec2(-0.5, -0.5), vec2(2, 2), 500);
	Image image(size, getStandardCrd2());
	image.setViewPort(viewport);

	image.alpha = 255;
	image.clear(twg::White);

	draw_right_angle(image, vec2(0), vec2(0, 1), vec2(sqrt(2)/2, 1 + sqrt(2)/2));

	vec2 offset(1.5, 0);
	draw_right_angle(image, offset + vec2(0), offset + vec2(0, 1), offset + vec2(-sqrt(2) / 2, 1 - sqrt(2) / 2));

	image.save("rigth_angle.png");

	return 0;
}