#define GLM_FORCE_XYZW_ONLY

#include <iostream>
#include <time.h>

#include <spob/spob2glm.h>
#include <Eigen/Dense>

using Eigen::MatrixXd;
using Eigen::VectorXd;

#include "interpolation.h"
#include "draw.h"
#include "spline.h"
#include "interpolation.h"
//#include "mba.hpp"

typedef function<void(const std::vector<vec2>&)> DrawFunction;
typedef function<bool(const vec2&, int)> TerminateFunction;

int n = 5, m = 2;

void empty_draw(const std::vector<vec2>& poly) {}

//-----------------------------------------------------------------------------
std::pair<space2, space2> getFractalSpaces(space2 space, DrawFunction draw_poly = empty_draw, DrawFunction draw_triangle = empty_draw) {
	// Задаем координаты квадрата
	vector<vec2> p = placePolyOnEdge(calcRegularPolygon(n, vec2(0), 1, 0), 0);

	// Высчитываем координаты прямоугольного треугольника, который лежит своей гипотенузой на оси X, с углом alpha при основании
	double alpha = spob::deg2rad(18);
	vec2 tr_a(0, 0), tr_b(1, 0), tr_c(cos(alpha), 0);
	tr_c = rotate(tr_c, vec2(0), alpha);

	// Рисуем квадрат
	draw_poly(fromMas(space, p));

	// Строим пространство, которое находится на верхней стороне квадрата
	space2 tr_line = makeLine2(p[m+1], p[m]);

	// Переводим координаты треугольника к этому пространству
	tr_a = tr_line.from(tr_a);
	tr_b = tr_line.from(tr_b);
	tr_c = tr_line.from(tr_c);

	draw_triangle(fromMas(space, std::vector<vec2>{tr_a, tr_b, tr_c}));

	// Строим пространства, которые находятся на обоих катетах этого треугольника
	space2 l1 = makeLine2(tr_a, tr_c);
	space2 l2 = makeLine2(tr_c, tr_b);

	//l1.j *= 0.8;
	//l2.j *= 0.8;

	return {l1, l2};
}

//-----------------------------------------------------------------------------
void draw_pythagoras_tree(space2 space, DrawFunction draw_poly, DrawFunction draw_triangle, TerminateFunction isTerminate, int depth = 0) {
	// Выходим из рекурсии, если одна из осей (аналогично и сторона квадрата) имеет длину меньше, чем 2
	if (isTerminate(space.i, depth))
		return;

	auto sp = getFractalSpaces(space, draw_poly, draw_triangle);

	// Рекурсивно строим дерево в этих пространствах
	draw_pythagoras_tree(space.from(sp.first), draw_poly, draw_triangle, isTerminate, depth+1);
	draw_pythagoras_tree(space.from(sp.second), draw_poly, draw_triangle, isTerminate, depth+1);
}

//-----------------------------------------------------------------------------
std::pair<vec2, vec2> calcBoundingBox(void) {
	bool isInitialized = false;
	vec2 min, max;
	draw_pythagoras_tree(getStandardCrd2(), [&] (const vector<vec2>& poly) {
		if (!isInitialized) {
			isInitialized = true;
			min = poly[0];
			max = poly[1];
		}
		for (auto& i : poly) {
			if (i.x < min.x) min.x = i.x;
			if (i.y < min.y) min.y = i.y;
			if (i.x > max.x) max.x = i.x;
			if (i.y > max.y) max.y = i.y;
		}
	}, [&] (const vector<vec2>& poly) {}, [&] (vec2 i, int depth) -> bool {
		return depth > 500 || i.length() < 0.05;
	});
	return {min, max};
}

//-----------------------------------------------------------------------------
void draw_logo(void) {
	crd2 standard = getStandardCrd2();
	vector<vec2> square = placePolyOnEdge(calcRegularPolygon(4, vec2(0), 1, 0), 0);

	//-------------------------------------------------------------------------
	// Считаем ограничивающий прямоугольник у фрактала и систему координат, которрая будет идеально смотреть на фрактал вместе с границами
	auto bbox = calcBoundingBox();
	auto viewport = calcViewPort(bbox.first, bbox.second);
	viewport = increaseViewportBorderByMinAxis(viewport, 0.1);
	double coef = viewport.i.length() / viewport.j.length();

	vec2 size(1000, 1000 / coef);

	//-------------------------------------------------------------------------
	// Инициализируем все изображения
	ImageGif gif;
	ImageGif gif2;
	Image img(size, viewport);
	Image img2(size, viewport);

	img.setViewPort(viewport);
	img2.setViewPort(viewport);

	string t = std::to_string(time(0));
	gif.start(img.img.size(), "g_" + t + ".gif");
	gif2.start(img2.img.size(), "g_" + t + "_explanation.gif");

	//-------------------------------------------------------------------------
	// Инициализируем системы координат и интерполяцию
	auto sp = getFractalSpaces(standard);
	//auto space = sp.second, another = sp.first;
	auto space = sp.first, another = sp.second;

	auto start = space.from(space.from(space.from(space)));
	auto end = space.from(start);
	MatrixPowerInterpolator interpolator(start, end);
	//SplineInterpolator2 interpolator(5, start, end);

	//-------------------------------------------------------------------------
	// Основной цикл рисования
	double count = 60;
	for (int i = 0; i < count; ++i) {
	//int i = 0; {
		cout << i << endl;
		double pos = i/count;

		// Сетка и координаты на изображении-объяснении
		img.img.clear(White);
		img.set_pen(0.5/80.0, Black);
		img2.img.clear(White);
		img2.set_pen(1.5/80.0, setAlpha(Gray, 192));
		img2.draw_grid(standard);
		img2.draw_crd(standard);
		img2.set_pen(1/80.0, Black);

		// Интерполированная система координат
		space2 c = interpolator.interpolate(pos);
		space2 d = c.from(viewport);
		img.setViewPort(d);

		// Сам процесс рисования фрактала
		double max_len = img.screen_tr.fromDir(vec2(0, 1)).length();
		double min_len = 0.1;
		double start_min_len = min_len + 3;

		auto draw = [&] (const vector<vec2>& poly) {
			double len = distance(img.screen_tr.from(poly[1]), img.screen_tr.from(poly[0]));
			double pos = std::min((len-min_len)/(max_len-min_len), 1.0);

			double alphapos = 1;
			if (len < start_min_len) alphapos = std::min((len - min_len)/(start_min_len-min_len), 1.0); 

			Color clr = setAlpha(getColorBetween(sqrt(sqrt(pos)), Red, Miku), 
								 //sqrt(sqrt(alphapos)) 
								 alphapos
								 * 128);
			img.img.setBrush(clr);
			img2.img.setBrush(clr);

			{
				auto p1 = fromMas(img.screen_tr, poly);
				Polygon_d p2;
				for (auto& i : p1) p2.array.push_back(i);
				img.img.drawPolygon(p2);
			}
			{
				auto p1 = fromMas(img2.screen_tr, poly);
				Polygon_d p2;
				for (auto& i : p1) p2.array.push_back(i);
				img2.img.drawPolygon(p2);
			}
		};

		#ifndef _DEBUG
		draw_pythagoras_tree(standard, draw, draw, [&] (vec2 i, int depth) -> bool {
			return depth > 100 || img.screen_tr.fromDir(i).length() < min_len;
		});
		#endif

		// Рисуем все текущие системы координат
		img2.draw_crd(start);
		img2.draw_crd(end);
		img2.draw_crd(c);

		// Рисуем viewport
		img2.set_pen(2.5/80.0, Gray);
		img2.draw_polygon(fromMas(d, square));

		img2.set_pen(0.5/80.0, Gray);

		space2 startcopy = standard;
		double countSplines = 20;
		for (int i = 0; i < countSplines; i++) {
			img2.set_pen(2/80.0, getColorBetween(i/countSplines, Bitcoin, Green));
			double countLines = 10;
			for (int j = 0; j < countLines; j++) {
				auto p1 = interpolator.interpolate(j/countLines);
				auto p2 = interpolator.interpolate((j+1)/countLines);
				img2.draw_line(startcopy.from(p1.pos), startcopy.from(p2.pos));
			}
			startcopy = space2(end).from(space2(start).to(startcopy));
		}

		gif.process(img.img, 2);
		gif2.process(img2.img, 2);
	}

	gif.end();
	gif2.end();
}

//-----------------------------------------------------------------------------
int main() {
	draw_logo();
}