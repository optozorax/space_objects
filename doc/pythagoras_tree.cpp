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

	// Задаем координаты квадрата
	vec2 a(0, 0), b(1, 0), c(1, 1), d(0, 1);

	// Высчитываем координаты прямоугольного треугольника, который лежит своей гипотенузой на оси X, с углом alpha при основании
	double alpha = spob::deg2rad(45);
	vec2 tr_a(0, 0), tr_b(1, 0), tr_c(cos(alpha), 0);
	tr_c = rotate(tr_c, vec2(0), alpha);

	// Преобразуем квадрат из текущих координат к координатам переданного пространства
	a = space.from(a);
	b = space.from(b);
	c = space.from(c);
	d = space.from(d);

	// Рисуем квадрат
	draw_line(a, b);
	draw_line(b, c);
	draw_line(c, d);
	draw_line(d, a);
	
	// Строим пространство, которое находится на верхней стороне квадрата
	space2 tr_line = makeLine2(d, c);

	// Переводим координаты треугольника к этому пространству
	tr_a = tr_line.from(tr_a);
	tr_b = tr_line.from(tr_b);
	tr_c = tr_line.from(tr_c);

	// Строим пространства, которые находятся на обоих катетах этого треугольника
	space2 l1 = makeLine2(tr_a, tr_c);
	space2 l2 = makeLine2(tr_c, tr_b);

	// Рекурсивно строим дерево в этих пространствах
	draw_pythagoras_tree(l1);
	draw_pythagoras_tree(l2);
}

int main() {
//int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	img.clear(White);
	img.setPen(Pen(1, Black));

	draw_pythagoras_tree(makeLine2(vec2(400, 410), vec2(300, 410)));

	saveToBmp(&img, L"pythagoras_tree.bmp");
}