#include <cmath>
#include <iostream>
#include "draw.h"
#include "find_borders.h"

//-----------------------------------------------------------------------------
void draw_explanation(int n, int m) {
	std::string add = std::to_string(n) + "_" + std::to_string(m) + "_";

	int counter = 0;
	double sz = 300;
	double border = 20;

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
		img.save(add + "explanation1.bmp");

		img.draw_crd(poly_line);
		img.save(add + "explanation2.bmp");
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

		img.save(add + "explanation5.bmp");
	}

	for (auto& i : poly) i = poly_line.to(i);

	space2 triangle_line = makeLine2((m == n) ? poly[0] : poly[m+1], poly[m]);

	triangle = fromMas(triangle_line, triangle);

	//-------------------------------------------------------------------------
	{
		FindBorders brd(sz, border);
		brd.process(poly);
		brd.process(crd_mas);

		brd.finish();

		vec2 size = brd.getCalculatedSize();
		brd.invertY();

		Image img(size, brd);
		draw_crd_grid(img);
		img.draw_polygon(poly);
		img.save(add + "explanation3.bmp");
	}

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

		img.draw_crd(triangle_line);
		img.save(add + "explanation4.bmp");

		img.img.clear(White);
		draw_crd_grid(img);
		img.set_pen(img.thick, setAlpha(img.clr, 64));
		img.draw_polygon(poly);
		img.set_pen(img.thick, Black);
		img.draw_polygon(triangle);
		img.save(add + "explanation6.bmp");
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
		img.save(add + "explanation7.bmp");

		img.set_pen(img.thick / 1.5, img.clr);
		img.draw_polygon(poly1);
		img.draw_polygon(poly2);
		img.save(add + "explanation8.bmp");
	}
}

void draw_animation(int n, int m, bool isOnlyLastFrame, int maxSize, bool isDrawGrid = false) {
	std::vector<std::vector<space2>> to_draw(15);
	
	// Вычисляем координаты правильного многоугольника
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
	FindBorders brd(maxSize, 20);
	for (auto& i : to_draw) {
		for (auto& j : i) {
			brd.process(fromMas(j, poly));
		}
	}
	brd.finish();
	brd.invertY();

	auto draw_save = [&] (int i) {
		Image img(brd.getCalculatedSize(), brd);

		if (isDrawGrid) {
			img.set_pen(1.5/80.0, setAlpha(Gray, 192));
			img.draw_grid(getStandardCrd2());
			img.set_pen(2.5/80.0, Gray);
			img.draw_crd(getStandardCrd2());
		}
		img.set_pen(0.5/80.0, Black);

		img.thick /= 2;
		img.set_pen(img.thick, Black);
		for (int j = 0; j < i; j++) {
			for (auto& k : to_draw[j]) {
				img.draw_polygon(fromMas(k, poly));
			}
		}

		img.set_pen(img.thick*2, Black);
		for (auto& k : to_draw[i]) {
			img.draw_polygon(fromMas(k, poly));
		}

		img.save("anim/n" + std::to_string(n) + "_m" + std::to_string(m) + "_a" + std::to_string(int(alpha * 180.0 / pi)) + "_N" + std::to_string(i) + ".bmp");
	};

	if (isOnlyLastFrame) {
		draw_save(to_draw.size() - 1);
	} else {
		for (int i = 0; i < to_draw.size(); i++)
			draw_save(i);
	}
}

void draw_angle_animation(int n, int m) {
	int count = 0;
	for (double angle = 1.0; angle < 90.0; angle+= 0.5) {
		// Вычисляем координаты правильного многоугольника
		const int n = 5;
		const int m = 2;
		std::vector<vec2> poly;
		for (int i = 0; i < n+1; ++i) {
			double angle = 2.0 * pi/n * i;
			poly.push_back(vec2(cos(angle), sin(angle)));
		}
		space2 poly_line = makeLine2(poly[0], poly[1]);
		poly = toMas(poly_line, poly);
		std::vector<vec2> poly_to_draw = poly; poly_to_draw.pop_back();

		// Вычисляем координаты прямоугольного треугольника
		double alpha = spob::deg2rad(angle);
		std::vector<vec2> triangle = {
			vec2(0), 
			vec2(1, 0), 
			rotate(vec2(cos(alpha), 0), vec2(0), alpha)
		};

		//--------------------------------------------------------------------
		// Создаем все системы координат, где будут лежать следующие многоугольники
		FindBorders brd(500, 20);
		std::vector<space2> current_spaces, new_spaces;
		current_spaces.push_back(getStandardCrd2());
		for (int i = 0; i < 1000; i++) {
			for (auto& j : current_spaces) if (j.i.length() > 0.007) {
				brd.process(fromMas(j, poly));

				space2 triangle_line = makeLine2(poly[m+1], poly[m]);
				auto triangle1 = fromMas(j, fromMas(triangle_line, triangle));

				// Рекурсивно рисуем дерево для каждой стороны этого треугольника на дереве пифагора
				space2 l1 = makeLine2(triangle1[0], triangle1[2]);
				space2 l2 = makeLine2(triangle1[2], triangle1[1]);
				new_spaces.push_back(l1);
				new_spaces.push_back(l2);
			}
			std::swap(current_spaces, new_spaces);
			new_spaces.clear();
		}
		brd.finish();
		brd.offsetToEqualSize();
		brd.invertY();

		ImageDrawing_aa img(brd.getCalculatedSize());
		img.clear(Black);
		img.setBrush(setAlpha(Miku, 64));
		std::function<void(const space2&, int)> draw;

		Polygon_d pp;
		std::vector<vec2> p1, triangle1;
		space2 triangle_line, l1, l2;
		draw = [&] (const space2& space, int g) {
			// Выходим из рекурсии, если одна из осей имеет длину меньше, чем 2. Это означает, что сейчас будет рисоваться квадрат с длиной стороны меньше, чем 2.
			double size = brd.fromDir(vec2(space.i.length(), 0)).length();
			if (size < 0.1 || g > 500)
				return;

			pp.array.clear();
			p1 = fromMas(brd, fromMas(space, poly_to_draw));
			for (auto& k : p1) pp.array.push_back(k);
			img.drawPolygon(pp);

			// Преобразуем координаты треугольника к координатам стороны квадрата
			triangle_line = makeLine2(poly[m+1], poly[m]);
			triangle1 = fromMas(triangle_line, triangle);

			// Рекурсивно рисуем дерево для каждой стороны этого треугольника на дереве пифагора
			l1 = makeLine2(triangle1[0], triangle1[2]);
			l2 = makeLine2(triangle1[2], triangle1[1]);
			draw(space.from(l1), g+1);
			draw(space.from(l2), g+1);
		};

		// Рисуем
		draw(getStandardCrd2(), 0);
		count++;
		std::string file = "anim/_n" + std::to_string(n) + "_m" + std::to_string(m) + "_N" + std::to_string(count) + ".bmp";
		saveToBmp(&img, std::wstring(file.begin(), file.end()));

		std::cout << count << std::endl;
	}
}

//-----------------------------------------------------------------------------
int main() {
	draw_explanation(4, 2);
	draw_explanation(5, 2);

	draw_animation(3, 0, true, 1000);
	draw_animation(3, 1, true, 1000);
	draw_animation(4, 0, false, 1000, true);
	draw_animation(4, 1, false, 1000, true);
	draw_animation(5, 1, true, 1000);
	draw_animation(5, 2, true, 1000);
	draw_animation(6, 1, true, 1000);
	draw_animation(10, 3, true, 1000);
	draw_animation(20, 1, true, 1000);
	draw_animation(20, 2, true, 1000);
	draw_animation(20, 3, true, 1000);
	draw_animation(20, 4, true, 1000);
	draw_animation(20, 5, true, 1000);
	draw_animation(20, 6, true, 1000);
	draw_animation(20, 7, true, 1000);
	draw_animation(20, 8, true, 1000);
	draw_animation(20, 9, true, 1000);

	draw_animation(3, 1, false, 500, true);
	draw_animation(4, 0, false, 500, true);
	draw_animation(4, 1, false, 500, true);
	draw_animation(4, 2, false, 500, false);
	draw_animation(5, 2, false, 500, true);

	draw_angle_animation(3, 1);
	draw_angle_animation(4, 0);
	draw_angle_animation(4, 1);
	draw_angle_animation(4, 2);
	draw_angle_animation(5, 1);
	draw_angle_animation(5, 2);
}