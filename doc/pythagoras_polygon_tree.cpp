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
	// Выходим из рекурсии, если одна из осей (аналогично и сторона квадрата) имеет длину меньше, чем 2
	if (space.i.length() < 2)
		return;

	// Считаем правильный многоугольник
	const int n = 5;
	const int m = 1;
	std::vector<vec2> poly;
	for (int i = 0; i < n+1; ++i) {
		double angle = spob::deg2rad(360.0/n * i);
		poly.push_back(vec2(cos(angle), sin(angle)));
	}

	// Преобразуем координаты так, чтобы он своим первым ребром находится на оси X
	space2 poly_line = makeLine2(poly[0], poly[1]);
	poly = toMas(poly_line, poly);

	// Высчитываем координаты прямоугольного треугольника, который лежит своей гипотенузой на оси X, с углом alpha при основании
	double alpha = spob::deg2rad(45);
	std::vector<vec2> triangle = {
		vec2(0, 0), 
		vec2(1, 0), 
		rotate(vec2(cos(alpha), 0), vec2(0), alpha)
	};

	// Преобразуем многоугольник из текущих координат к координатам переданного пространства
	poly = fromMas(space, poly);

	// Рисуем многоугольник
	for (int i = 0; i < poly.size() - 1; i++)
		draw_line(poly[i], poly[i+1]);
	
	// Строим пространство, которая находится на m-й стороне многоугольника
	space2 tr_line = makeLine2(poly[m+1], poly[m]);

	// Переводим координаты треугольника к этому пространству
	triangle = fromMas(tr_line, triangle);

	// Строим пространства, которые находятся на обоих катетах этого треугольника
	space2 l1 = makeLine2(triangle[0], triangle[2]);
	space2 l2 = makeLine2(triangle[2], triangle[1]);

	// Рекурсивно строим дерево в этих пространствах
	draw_pythagoras_tree(l1);
	draw_pythagoras_tree(l2);
}

int main() {
//int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	img.clear(White);
	img.setPen(Pen(1, setAlpha(Black, 128)));

	draw_pythagoras_tree(makeLine2(vec2(500, 235), vec2(350, 235)));

	saveToBmp(&img, L"pythagoras_polygon_tree.bmp");
}