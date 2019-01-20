#define _USE_MATH_DEFINES

#include <cmath>
#include <spob/spob.h>
#include <twg/image/image_drawing.h>
using namespace twg;

using namespace spob;
ImageDrawing_aa img(Point_i(700, 420));
void draw_line(vec2 a, vec2 b) { // Некая сторонняя функция, которая рисует линию из точки a в точку b
	img.drawLine(a, b);
}

void draw_pythagoras_tree(const space2& space) {
	// Выходим из рекурсии, если одна из осей имеет длину меньше, чем 2. Это означает, что сейчас будет рисоваться квадрат с длиной стороны меньше, чем 2.
	if (space.i.length() < 2)
		return;

	vec2 a(0, 0), b(0, 1), c(1, 1), d(1, 0);

	// Рисуем квадрат
	draw_line(space.from(a), space.from(b));
	draw_line(space.from(b), space.from(c));
	draw_line(space.from(c), space.from(d));
	draw_line(space.from(d), space.from(a));

	// Находим вершину треугольника на верхней части квадрата
	vec2 triangle_vertex = rotate(b + vec2(sqrt(2.0)/2.0, 0), b, 45.0 / 180.0 * M_PI);

	// Рекурсивно рисуем дерево для каждой стороны этого треугольника на дереве пифагора
	draw_pythagoras_tree(makeLine2(space.from(b), space.from(triangle_vertex)));
	draw_pythagoras_tree(makeLine2(space.from(triangle_vertex), space.from(c)));
}

//int main() {
int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	img.clear(White);
	img.setPen(Pen(1, Black));

	draw_pythagoras_tree(makeLine2(vec2(400, 410), vec2(300, 410)));

	saveToBmp(&img, L"pythagoras_tree.bmp");
}