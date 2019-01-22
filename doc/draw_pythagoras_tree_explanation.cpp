#include <cmath>
#include "draw.h"
#include "find_borders.h"

//-----------------------------------------------------------------------------
void draw_logo(void) {
	int counter = 0;
	double sz = 300;
	double border = 20;

	const int n = 5;
	const int m = 2;
	std::vector<vec2> poly;
	for (int i = 0; i < n; ++i) {
		double angle = 2.0 * pi/n * i;
		poly.push_back(vec2(cos(angle), sin(angle)));
	}

	space2 poly_line = makeLine2(poly[0], poly[1]);

	// Строим прямогоульный треугольник, который лежит своей гипотенузой на оси OX, с углом alpha
	double alpha = 45.0 / 180.0 * pi;
	std::vector<vec2> triangle = {vec2(0), vec2(1, 0), rotate(vec2(cos(alpha), 0), vec2(0), alpha)};

	auto standard = getStandardCrd2();
	std::vector<vec2> crd_mas = {vec2(0, 0), vec2(0, 1), vec2(1, 0)};

	auto draw_crd_grid = [&standard] (Image& img) {
		img.set_pen(1.5/80.0, setAlpha(Gray, 192));
		img.draw_grid(standard);
		img.set_pen(2.5/80.0, Gray);
		img.draw_crd(standard);
		img.set_pen(1/80.0, Black);
	};

	//-------------------------------------------------------------------------
	{
		FindBorders brd(sz, border);
		brd.process(poly);
		brd.process(crd_mas);
		brd.process(fromMas(poly_line, crd_mas));

		brd.finish();

		vec2 size = brd.getCalculatedSize();
		brd.invertY();

		Image img(size, brd);
		draw_crd_grid(img);
		img.draw_polygon(poly);
		img.save("explanation1.bmp");

		img.draw_crd(poly_line);
		img.save("explanation2.bmp");
	}

	//-------------------------------------------------------------------------
	{
		FindBorders brd(sz, border);
		brd.process(triangle);
		brd.process(crd_mas);

		brd.finish();

		vec2 size = brd.getCalculatedSize();
		brd.invertY();

		Image img(size, brd);

		draw_crd_grid(img);

		img.draw_polygon(triangle);

		img.save("explanation5.bmp");
	}

	for (auto& i : poly) i = poly_line.to(i);

	space2 triangle_line = makeLine2((m == n) ? poly[0] : poly[m+1], poly[m]);

	triangle = fromMas(triangle_line, triangle);

	//-------------------------------------------------------------------------
	{
		FindBorders brd(sz, border);
		brd.process(poly);
		brd.process(crd_mas);
		brd.process(fromMas(triangle_line, crd_mas));

		brd.finish();

		vec2 size = brd.getCalculatedSize();
		brd.invertY();

		Image img(size, brd);
		draw_crd_grid(img);
		img.draw_polygon(poly);
		img.save("explanation3.bmp");

		img.draw_crd(triangle_line);
		img.save("explanation4.bmp");

		img.img.clear(White);
		draw_crd_grid(img);
		img.set_pen(img.thick, setAlpha(img.clr, 64));
		img.draw_polygon(poly);
		img.set_pen(img.thick, Black);
		img.draw_polygon(triangle);
		img.save("explanation6.bmp");
	}

	space2 l1 = makeLine2(triangle[0], triangle[2]);
	space2 l2 = makeLine2(triangle[2], triangle[1]);

	auto poly1 = fromMas(l1, poly);
	auto poly2 = fromMas(l2, poly);

	//-------------------------------------------------------------------------
	{
		FindBorders brd(sz, border);
		brd.process(poly);
		brd.process(poly1);
		brd.process(poly2);
		brd.process(crd_mas);
		brd.process(fromMas(l1, crd_mas));
		brd.process(fromMas(l2, crd_mas));

		brd.finish();

		vec2 size = brd.getCalculatedSize();
		brd.invertY();

		Image img(size, brd);
		draw_crd_grid(img);
		img.set_pen(img.thick, setAlpha(img.clr, 64));
		img.draw_polygon(poly);
		img.draw_polygon(triangle);
		img.set_pen(img.thick, Black);
		img.set_pen(img.thick * 1.5, img.clr);
		img.draw_crd(l1);
		img.draw_crd(l2);
		img.save("explanation7.bmp");

		img.set_pen(img.thick / 1.5, img.clr);
		img.draw_polygon(poly1);
		img.draw_polygon(poly2);
		img.save("explanation8.bmp");
	}
}

void draw_animation(void) {
	std::vector<std::vector<space2>> to_draw(15);
	
	// Вычисляем координаты правильного многоугольника
	const int n = 5;
	const int m = 2;
	std::vector<vec2> poly;
	for (int i = 0; i < n; ++i) {
		double angle = 2.0 * pi/n * i;
		poly.push_back(vec2(cos(angle), sin(angle)));
	}
	space2 poly_line = makeLine2(poly[0], poly[1]);
	poly = toMas(poly_line, poly);

	// Вычисляем координаты прямоугольного треугольника
	double alpha = 45.0 / 180.0 * pi;
	std::vector<vec2> triangle = {
		vec2(0), 
		vec2(1, 0), 
		rotate(vec2(cos(alpha), 0), vec2(0), alpha)
	};

	// Создаем все системы координат, где будут лежать следующие многоугольники
	to_draw[0].push_back(getStandardCrd2());
	for (int i = 0; i < to_draw.size() - 1; i++) {
		for (auto& j : to_draw[i]) {
			space2 triangle_line = makeLine2((m == n) ? poly[0] : poly[m+1], poly[m]);
			auto triangle1 = fromMas(j, fromMas(triangle_line, triangle));

			// Рекурсивно рисуем дерево для каждой стороны этого треугольника на дереве пифагора
			space2 l1 = makeLine2(triangle1[0], triangle1[2]);
			space2 l2 = makeLine2(triangle1[2], triangle1[1]);
			to_draw[i+1].push_back(l1);
			to_draw[i+1].push_back(l2);
		}
	}

	// Вычисляем границы всех многоугольников
	FindBorders brd(500, 20);
	for (auto& i : to_draw) {
		for (auto& j : i) {
			brd.process(fromMas(j, poly));
		}
	}
	brd.finish();
	brd.invertY();

	// Рисуем каждый кадр
	for (int i = 0; i < to_draw.size(); i++) {
		Image img(brd.getCalculatedSize(), brd);

		img.set_pen(1.5/80.0, setAlpha(Gray, 192));
		img.draw_grid(getStandardCrd2());
		img.set_pen(2.5/80.0, Gray);
		img.draw_crd(getStandardCrd2());
		img.set_pen(1/80.0, Black);

		img.thick /= 2;

		img.set_pen(img.thick, setAlpha(Black, 64));
		for (int j = 0; j < i; j++) {
			for (auto& k : to_draw[j]) {
				img.draw_polygon(fromMas(k, poly));
			}
		}

		img.set_pen(img.thick, Black);
		for (auto& k : to_draw[i]) {
			img.draw_polygon(fromMas(k, poly));
		}

		img.save("anim/n" + std::to_string(n) + "_m" + std::to_string(m) + "_a" + std::to_string(int(alpha * 180.0 / pi)) + "_N" + std::to_string(i) + ".bmp");
	}
}

//-----------------------------------------------------------------------------
int main() {
	draw_logo();
	draw_animation();
}

/*

1. double angle = 2*pi/n * i;
   poly[i] = vec2(cos(angle), sin(angle));
2. space2 poly_line = makeLine2(poly[0], poly[1]);
3. poly = fromMas(poly_line, poly);
4. space2 triangle_line = makeLine2(poly[1], poly[2]);
5. std::vector<vec2> triangle = {
       vec2(0, 0), 
       vec2(1, 0), 
       rotate(vec2(cos(alpha), 0), vec2(0), alpha)
   };
6. triangle = fromMas(triangle_line, triangle);
7. space2 l1 = makeLine2(triangle[0], triangle[2]);
   space2 l2 = makeLine2(triangle[2], triangle[1]);
8. recursion(l1);
   resursion(l2);

*/