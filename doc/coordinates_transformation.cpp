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

void draw_sin(const space2& tr) {
	// Рисуем оси координат
	draw_line(tr.from(vec2(-1, 0)), tr.from(vec2(1, 0)));
	draw_line(tr.from(vec2(0, -1)), tr.from(vec2(0, 1)));

	// Рисуем график функции sin(x) в пределах [-2, 2]
	vec2 last(-2, std::sin(-2));
	for (double x = -2; x <= 2; x += 0.01) {
		vec2 current(x, std::sin(x));
		draw_line(tr.from(last), tr.from(current));
		last = current;
	}
}

//int main() {
int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	img.clear(White);
	img.setPen(Pen(1.5, Black));

	space2 tr = getStandardCrd2();
	tr.pos = vec2(150, 150); // Смещаем в центр экрана
	tr.j.negate(); // Делается это потому что на экране ось Y направлена вниз
	tr.i *= 60; // Увеличиваем ось X в 60 раз
	tr.j *= 60;

	draw_sin(tr);

	saveToBmp(&img, L"coordinates_transformation.bmp");
}
