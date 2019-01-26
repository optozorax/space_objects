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

void draw_arrow(vec2 a, vec2 b, double angle, double size) { // Стрелка начинается в точке a и кончается в точке b, стрелка должна находиться в точке b
	draw_line(a, b);

	// Создаем систему координат, которая начинается в точке b, и её вектор i направлен к точке a
	line2 line = makeLine2(b, a);

	// Нормализуем эту систему координат, потому что вектор i равен длине вектора (b-a)
	line.normalize();

	// Создаем две точки краев стрелки
	vec2 first(size, 0);
	vec2 second(size, 0);

	// Один край вращаем по часовой стрелке, а другой против
	first = rotate(first, vec2(0), angle);
	second = rotate(second, vec2(0), -angle);

	// Преобразуем эти края из системы координат линии в абсолютные координаты и рисуем это
	space2 space_of_line(line);
	draw_line(b, space_of_line.from(first));
	draw_line(b, space_of_line.from(second));
}

//int main() {
int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	img.clear(White);
	img.setPen(Pen(1.5, Black));

	vec2 center(150, 150);

	vec2 a(50, 0); a += center;
	vec2 b(130, 0); b += center;

	// Рисование стрелок по всем направлениям
	int count = 4 + 3 * 4;
	for (int i = 0; i < count; ++i) {
		double angle = deg2rad(360.0 / count * i);
		draw_arrow(rotate(a, center, angle), rotate(b, center, angle), deg2rad(30), 10)
	}

	saveToBmp(&img, L"arrow.bmp");
}
