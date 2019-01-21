#define _USE_MATH_DEFINES

#include <cmath>
#include <spob/spob.h>
#include <twg/image/image_drawing.h>
using namespace twg;

using namespace spob;
ImageDrawing_aa img(Point_i(700, 420));
void draw_line(vec2 a, vec2 b) { // Некая сторонняя функция, которая рисует линию из точки a в точку b
	img.drawLine(a + vec2(0.5, 0.5), b + vec2(0.5, 0.5));
}

void draw_pythagoras_tree(const space2& space) {
	// Выходим из рекурсии, если одна из осей имеет длину меньше, чем 2. Это означает, что сейчас будет рисоваться квадрат с длиной стороны меньше, чем 2.
	if (space.i.length() < 2)
		return;

	vec2 a(0, 0), b(0, 1), c(1, 1), d(1, 0);

	// Считаем правильный многоугольник
	const int n = 5;
	const int m = 1;
	std::vector<vec2> poly;
	for (int i = 0; i < n; ++i) {
		double angle = 2.0 * M_PI/n * i;
		poly.push_back(vec2(cos(angle), sin(angle)));
	}

	// Преобразуем координаты так, чтобы он своим первым ребром находится на оси X
	space2 poly_line = makeLine2(poly[0], poly[1]);
	for (auto& i : poly) i = poly_line.to(i);

	for (int i = 0; i < poly.size() - 1; i++)
		draw_line(space.from(poly[i]), space.from(poly[i+1]));
	draw_line(space.from(poly.back()), space.from(poly.front()));

	// Строим прямогоульный треугольник, который лежит своей гипотенузой на оси OX, с углом alpha
	double alpha = 45.0 / 180.0 * M_PI;
	vec2 triangle_a = vec2(0), triangle_b = vec2(1, 0);
	vec2 triangle_c = rotate(vec2(cos(alpha), 0), vec2(0), alpha);

	// Преобразуем координаты треугольника к координатам m-й стороны многоугольника
	space2 triangle_line = makeLine2((m == n) ? poly[0] : poly[m+1], poly[m]);
	triangle_a = triangle_line.from(triangle_a);
	triangle_b = triangle_line.from(triangle_b);
	triangle_c = triangle_line.from(triangle_c);

	// Рекурсивно рисуем дерево для каждой стороны этого треугольника на дереве пифагора
	draw_pythagoras_tree(makeLine2(space.from(triangle_a), space.from(triangle_c)));
	draw_pythagoras_tree(makeLine2(space.from(triangle_c), space.from(triangle_b)));
}

int main() {
//int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	img.clear(White);
	img.setPen(Pen(1, setAlpha(Black, 128)));

	draw_pythagoras_tree(makeLine2(vec2(500, 235), vec2(350, 235)));

	saveToBmp(&img, L"pythagoras_polygon_tree.bmp");
}