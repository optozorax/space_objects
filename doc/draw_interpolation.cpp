#define GLM_FORCE_XYZW_ONLY

#include <iostream>
#include <sstream>
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

typedef function<void(const std::vector<vec2>&, int)> DrawFunction;
typedef function<bool(const vec2&, int)> TerminateFunction;

int n = 4, m = 2;
int angle = 45;
bool isFirst = true;

int maxDepth = 100;
int insideCount = 1;
double min_len = 5.3;
bool isExperiment = false;

double posAnimation = 0;

void empty_draw(const std::vector<vec2>& poly, int a) {}

//-----------------------------------------------------------------------------
std::pair<space2, space2> getFractalSpaces(space2 space, DrawFunction draw_poly = empty_draw, DrawFunction draw_triangle = empty_draw, int depth = 0) {
	// Задаем координаты квадрата
	vector<vec2> p = placePolyOnEdge(calcRegularPolygon(n, vec2(0), 1, 0), 0);

	// Высчитываем координаты прямоугольного треугольника, который лежит своей гипотенузой на оси X, с углом alpha при основании
	double alpha = spob::deg2rad(angle);
	vec2 tr_a(0, 0), tr_b(1, 0), tr_c(cos(alpha), 0);
	tr_c = rotate(tr_c, vec2(0), alpha);

	// Рисуем квадрат
	draw_poly(fromMas(space, p), depth);

	// Строим пространство, которое находится на верхней стороне квадрата
	space2 tr_line = makeLine2(p[m+1], p[m]);

	// Переводим координаты треугольника к этому пространству
	tr_a = tr_line.from(tr_a);
	tr_b = tr_line.from(tr_b);
	tr_c = tr_line.from(tr_c);

	draw_triangle(fromMas(space, std::vector<vec2>{tr_a, tr_b, tr_c}), depth);

	// Строим пространства, которые находятся на обоих катетах этого треугольника
	space2 l1 = makeLine2(tr_a, tr_c);
	space2 l2 = makeLine2(tr_c, tr_b);

	/*l1.j *= 0.8;
	l2.j *= 0.8;*/
	/*l1 = rotate(l1, l1.pos, spob::deg2rad(-50));
	l2 = rotate(l2, l2.pos + l2.i, spob::deg2rad(50));*/

	return {l1, l2};
}

//-----------------------------------------------------------------------------
void draw_pythagoras_tree(space2 space, DrawFunction draw_poly, DrawFunction draw_triangle, TerminateFunction isTerminate, int depth = 0) {
	// Выходим из рекурсии, если одна из осей (аналогично и сторона квадрата) имеет длину меньше, чем 2
	if (isTerminate(space.i, depth))
		return;

	auto sp = getFractalSpaces(space, draw_poly, draw_triangle, depth);

	// Рекурсивно строим дерево в этих пространствах
	draw_pythagoras_tree(space.from(sp.first), draw_poly, draw_triangle, isTerminate, depth+1);
	draw_pythagoras_tree(space.from(sp.second), draw_poly, draw_triangle, isTerminate, depth+1);
}

//-----------------------------------------------------------------------------
std::pair<vec2, vec2> calcBoundingBox(void) {
	bool isInitialized = false;
	vec2 min, max;
	draw_pythagoras_tree(getStandardCrd2(), [&] (const vector<vec2>& poly, int depth) {
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
	}, [&] (const vector<vec2>& poly, int depth) {}, [&] (vec2 i, int depth) -> bool {
		return depth > 500 || i.length() < 0.01;
	});
	return {min, max};
}

//-----------------------------------------------------------------------------
void draw_animation(void) {
	crd2 standard = getStandardCrd2();
	vector<vec2> square = placePolyOnEdge(calcRegularPolygon(4, vec2(0), 1, 0), 0);

	//-------------------------------------------------------------------------
	// Считаем ограничивающий прямоугольник у фрактала и систему координат, которрая будет идеально смотреть на фрактал вместе с границами
	auto bbox = calcBoundingBox();
	auto viewport = calcViewPort(bbox.first, bbox.second);
	viewport = increaseViewportBorderByMinAxis(viewport, 0.1);
	double coef = viewport.i.length() / viewport.j.length();

	vec2 size;
	double sz = 500;
	if (coef > 1)
		size = vec2(sz, sz / coef);
	else
		size = vec2(sz * coef, sz);

	//-------------------------------------------------------------------------
	// Инициализируем все изображения
	ImageGif gif;
	ImageGif gif2;
	Image img(size, viewport, maxDepth+5);
	Image img2(size, viewport, maxDepth+5);

	img.setViewPort(viewport);
	img2.setViewPort(viewport);

	string t = std::to_string(time(0));
	stringstream sout;
	sout << "p3_" << n << "." << m << "_" << angle;
	gif.start(img.imgs[0]->size(), sout.str() + ".gif");
	gif2.start(img2.imgs[0]->size(), sout.str() + "_explanation.gif");

	//-------------------------------------------------------------------------
	// Инициализируем системы координат и интерполяцию
	auto sp = getFractalSpaces(standard);
	//auto space = sp.second, another = sp.first;
	space2 space, another;
	if (isFirst) {
		space = sp.first;
		another = sp.second;
	} else {
		space = sp.second;
		another = sp.first;
	}

	auto start = standard;
	for (int i = 0; i < insideCount; i++)
		start = space.from(start);
	auto end = space.from(start);
	MatrixPowerInterpolator interpolator(start, end);
	//SplineInterpolator2 interpolator(5, start, end);

	//-------------------------------------------------------------------------
	// Основной цикл рисования
	double count = 60;
	for (int i = 0; i <= count; i += (isExperiment) ? 60 : 1) {
	//int i = 0; {
		cout << i << endl;
		posAnimation = i/count;

		// Сетка и координаты на изображении-объяснении
		img.clear(White);
		//img.clear(Black);
		img.set_pen(0.5/80.0, Black, 0);

		//img2.clear(White);
		img2.clear(White);
		img2.set_pen(1.5/80.0, setAlpha(Gray, 192), 0);
		img2.draw_grid(standard, 0);
		img2.set_pen(1.5/80.0, setAlpha(Gray, 192), maxDepth+2);
		img2.draw_crd(standard, maxDepth+2);
		img2.set_pen(1/80.0, Black, 0);

		// Интерполированная система координат
		space2 c = interpolator.interpolate(posAnimation);
		//space2 c = interpolate(start, end, posAnimation);
		//space2 c = interpolateCircular(start, end, posAnimation, 1.12, 2, true);
		space2 d = c.from(viewport);
		img.setViewPort(d);

		// Сам процесс рисования фрактала
		static double max_len = img.screen_tr.fromDir(vec2(0, 1)).length();
		double start_min_len = std::min(min_len + 1, min_len * 2);

		auto draw = [&] (const vector<vec2>& poly, int depth) {
			double len = distance(img.screen_tr.from(poly[1]), img.screen_tr.from(poly[0]));
			double pos = (len-min_len)/(max_len-min_len);

			double alphapos = 1;
			if (len < start_min_len) alphapos = std::min((len - min_len)/(start_min_len-min_len), 1.0); 

			//Color start = rgb(0x15, 0x57, 0x99), end = rgb(0x15, 0x99, 0x57);
			//Color start = Miku, end = Red;
			Color start = getColorBetween(0.1, Miku, White), end = Blue;
			//Color start = getColorBetween(0.5, Red, Black), end = Blue;
			//Color start = White, end = Blue;
			//Color start = Black, end = Blue;
			//Color start = Gray, end = getColorBetween(0.3, Miku, Black);
			//Color start = Gray, end = Red;
			//Color start = getColorBetween(0.5, getColorBetween(0.5, Green, Black), getColorBetween(0.5, Yellow, Black)), end = Red;
			Color clr = setAlpha(getColorBetween(
				sqrt(sqrt(pos)), 
				//pos,
				end, start), 
				//sqrt(sqrt(alphapos)) 
				alphapos
				* 128);
			img.imgs[depth+1]->setBrush(clr);
			img2.imgs[depth+1]->setBrush(clr);

			{
				auto p1 = fromMas(img.screen_tr, poly);
				Polygon_d p2;
				for (auto& i : p1) p2.array.push_back(i);
				img.imgs[depth+1]->drawPolygon(p2);
			}
			{
				auto p1 = fromMas(img2.screen_tr, poly);
				Polygon_d p2;
				for (auto& i : p1) p2.array.push_back(i);
				img2.imgs[depth+1]->drawPolygon(p2);
			}
		};

		//#ifndef _DEBUG
		draw_pythagoras_tree(standard, draw, draw, [&] (vec2 i, int depth) -> bool {
			return depth > maxDepth || img.screen_tr.fromDir(i).length() < min_len;
		});
		//#endif

		// Рисуем все текущие системы координат
		img2.draw_crd(start, maxDepth+2);
		img2.draw_crd(end, maxDepth+2);
		img2.draw_crd(c, maxDepth+2);

		// Рисуем viewport
		img2.set_pen(2.5/80.0, Black, maxDepth+2);
		img2.draw_polygon(fromMas(d, square), maxDepth+2);

		img2.set_pen(0.5/80.0, Gray, maxDepth+2);

		if (!isExperiment) {
			space2 startcopy = standard;
			double startStandardLength = startcopy.fromDir(standard.i).length();
			double countSplines = 50;
			for (int i = 0; i < countSplines; i++) {
				double countLines = 10;
				for (int j = 0; j < countLines; j++) {
					auto p1 = interpolator.interpolate(j/countLines);
					auto p2 = interpolator.interpolate((j+1)/countLines);

					//auto p1 = interpolate(start, end, j/countLines);
					//auto p2 = interpolate(start, end, (j+1)/countLines);

					//auto p1 = interpolateCircular(start, end, j/countLines, 1.12, 2, true);
					//auto p2 = interpolateCircular(start, end, (j+1)/countLines, 1.12, 2, true);

					img2.set_pen(4/80.0 * startcopy.fromDir(p1.i).length() / startStandardLength, Black, maxDepth+2);
					img2.draw_line(startcopy.from(p1.pos), startcopy.from(p2.pos), maxDepth+2);
				}
				startcopy = space2(end).from(space2(start).to(startcopy));
			}
		}

		img.combine_layers();
		img2.combine_layers();

		gif.process(*img.imgs[0], 2);
		gif2.process(*img2.imgs[0], 2);
	}

	gif.end();
	gif2.end();
}

//-----------------------------------------------------------------------------
double gauss_kernel(double x) {
	// x in [-1, 1], 
	x *= 3;
	return std::exp(-x*x/2.0);
}

//-----------------------------------------------------------------------------
void draw_interpolation() {
	crd2 standard = getStandardCrd2();

	// Начальные расчеты
	space2 a = standard;
	space2 b = standard; b.move(vec2(3, 3)); b = rotate(b, b.pos, spob::deg2rad(70)); b.i *= 1.1; b.j *= 1.1;
	space2 c = standard; c.move(vec2(5, 4)); c = rotate(c, c.pos, spob::deg2rad(-15)); c.i *= 0.3; c.j *= 0.3;

	glm::mat3 A = getFromMatrix(a);
	glm::mat3 B = getFromMatrix(b);
	glm::mat3 C = getFromMatrix(c);

	std::vector<double> points = {0, 1, 2};
	SplineInterpolator3 splinea(points, 0, 5);
	SplineInterpolator3 splineb(points, 1, 5);
	SplineInterpolator3 splinec(points, 2, 5);

	//MatrixPowerInterpolator interpolator(a, b);

	//-------------------------------------------------------------------------
	// Цикл расчетов
	double count = 60;
	std::vector<space2> counts;
	for (int i = 0; i <= 2*count; i++) {
		double t = i/count;

		// Подход на основе полиномов Лагранжа и возведении матрицы в степень
		/*double ta =  (t*t-3*t+2)/2.0; // 0,1 ; 1,0 ; 2,0
		double tb = 2*t-t*t; // 0,0 ; 1,1 ; 2,0
		double tc = (t*t-t)/2.0; // 0,0 ; 1,0 ; 2,1
		glm::mat3 P = pow(C, tc) * pow(B, tb) * pow(A, ta);*/

		// Подход на основе ядерных функций и возведении матрицы в степень
		/*double ta =  gauss_kernel(t); // 0,1 ; 1,0 ; 2,0
		double tb = gauss_kernel(t-1); // 0,0 ; 1,1 ; 2,0
		double tc = gauss_kernel(t-2); // 0,0 ; 1,0 ; 2,1
		glm::mat3 P = pow(C, tc) * pow(B, tb) * pow(A, ta);*/

		double ta = splinea.interpolate(t);
		double tb = splineb.interpolate(t);
		double tc = splinec.interpolate(t);
		glm::mat3 P = pow(C, tc) * pow(B, tb) * pow(A, ta);
		counts.push_back(getToCrd(P));

		//counts.push_back(interpolate(a, b, t));
		//counts.push_back(interpolateCircular(a, b, t, 5, 2));
		//counts.push_back(interpolator.interpolate(t));
	}

	//-------------------------------------------------------------------------
	// Расчет окна видимости и размера изображения
	space2 viewport = standard;
	viewport.pos -= viewport.fromDir(vec2(1, 1)/2.0);
	viewport.j *= 7;
	viewport.i *= 10;

	double coef = viewport.i.length() / viewport.j.length();
	double sz = 1000;
	vec2 size;
	if (coef > 1)
		size = vec2(sz, sz / coef);
	else
		size = vec2(sz * coef, sz);

	//-------------------------------------------------------------------------
	// Инициализируем все изображения
	ImageGif gif;
	Image img(size, viewport);
	img.setViewPort(viewport);

	gif.start(img.imgs[0]->size(), "interpolation.gif");

	//-------------------------------------------------------------------------
	// Цикл рисования
	for (int i = 0; i < counts.size(); i++) {
		//img.setViewPort(counts[i]);

		// Рисуем сетку
		img.clear(White);
		img.set_pen(1.5/80.0, setAlpha(Gray, 192));
		img.draw_grid(standard);
		img.set_pen(1.5/80.0, setAlpha(Gray, 192));
		img.draw_crd(standard);

		img.set_pen(1/80.0, Black);
		vec2 lastPos = a.pos;
		for (auto& j : counts) {
			img.set_alpha(16);
			img.draw_crd(j);
			img.set_alpha(255);
			img.draw_line(lastPos, j.pos);
			lastPos = j.pos;
		}
		for (auto j : counts) {
			j = c.from(j);

			img.set_alpha(16);
			img.draw_crd(j);
			img.set_alpha(255);
			img.draw_line(lastPos, j.pos);
			lastPos = j.pos;
		}
		for (auto j : counts) {
			j = c.from(c.from(j));

			img.set_alpha(16);
			img.draw_crd(j);
			img.set_alpha(255);
			img.draw_line(lastPos, j.pos);
			lastPos = j.pos;
		}
		img.set_alpha(255);

		// Рисуем все текущие системы координат
		img.set_pen(3/80.0, Black);
		img.draw_crd(a);
		img.draw_crd(b);
		img.draw_crd(c);
		img.draw_crd(counts[i]);

		gif.process(*img.imgs[0], 2);
	}

	gif.end();
}

//-----------------------------------------------------------------------------
int main() {
	//draw_animation();
	draw_interpolation();
}