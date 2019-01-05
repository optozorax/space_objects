#define _USE_MATH_DEFINES

#include <cmath>
#include <spob/spob.h>
#include <twg/image/image_drawing.h>
using namespace twg;

using namespace spob;
ImageDrawing_aa img(Point_i(300, 300));
void draw_line(vec2 a, vec2 b) { // Некая сторонняя функция, которая рисует линию из точки a в точку b
	img.drawLine(a, b);
}

void draw_anything(const space2& tr) {
	vec2 a(-1, -1), b(-1, 1), c(1, 1), d(1, -1);

	draw_line(tr.from(a), tr.from(b));
	draw_line(tr.from(b), tr.from(c));
	draw_line(tr.from(c), tr.from(d));
	draw_line(tr.from(d), tr.from(a));

	draw_line(tr.from(a), tr.from(c));
}

//int main() {
int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	img.clear(White);
	img.setPen(Pen(1.5, Black));

	vec2 center(150, 150);

	space2 tr = getStandardCrd2();
	tr.pos = center + vec2(130, 0);
	tr.j.negate();
	tr.i *= 15;
	tr.j *= 15;

	int count = 4 + 2 * 4;
	for (int i = 0; i < count; ++i) {
		double angle = i * 2.0 * M_PI / double(count);
		draw_anything(rotate(tr, center, angle));
	}

	saveToBmp(&img, L"symmetry.bmp");
}