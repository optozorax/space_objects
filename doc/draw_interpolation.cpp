#define GLM_FORCE_XYZW_ONLY

#include <iostream>

#include <spob/spob2glm.h>
#include <Eigen/Dense>

using Eigen::MatrixXd;
using Eigen::VectorXd;

#include "interpolation.h"
#include "draw.h"
#include "spline.h"
#include "interpolation.h"
//#include "mba.hpp"

void draw_pythagoras_tree(const space2& space, function<void(const vector<vec2>&)> draw_poly, function<void(const vector<vec2>&)> draw_triangle, std::function<bool(vec2)> isTerminate, int depth = 0) {
	// Выходим из рекурсии, если одна из осей (аналогично и сторона квадрата) имеет длину меньше, чем 2
	if (depth > 20 || isTerminate(space.i))
		return;

	// Задаем координаты квадрата
	vector<vec2> p = placePolyOnEdge(calcRegularPolygon(5, vec2(0), 1, 0), 0);

	// Высчитываем координаты прямоугольного треугольника, который лежит своей гипотенузой на оси X, с углом alpha при основании
	double alpha = spob::deg2rad(45);
	vec2 tr_a(0, 0), tr_b(1, 0), tr_c(cos(alpha), 0);
	tr_c = rotate(tr_c, vec2(0), alpha);

	// Преобразуем квадрат из текущих координат к координатам переданного пространства
	p = fromMas(space, p);

	// Рисуем квадрат
	draw_poly(p);
	/*draw_line(a, b);
	draw_line(b, c);
	draw_line(c, d);
	draw_line(d, a);*/
	
	// Строим пространство, которое находится на верхней стороне квадрата
	space2 tr_line;
	if (depth == 0)
		tr_line = makeLine2(p[4], p[3]);
	else
		tr_line = makeLine2(p[3], p[2]);

	// Переводим координаты треугольника к этому пространству
	tr_a = tr_line.from(tr_a);
	tr_b = tr_line.from(tr_b);
	tr_c = tr_line.from(tr_c);

	draw_triangle({tr_a, tr_b, tr_c});

	// Строим пространства, которые находятся на обоих катетах этого треугольника
	space2 l1 = makeLine2(tr_a, tr_c);
	space2 l2 = makeLine2(tr_c, tr_b);

	// Рекурсивно строим дерево в этих пространствах
	draw_pythagoras_tree(l1, draw_poly, draw_triangle, isTerminate, depth+1);
	draw_pythagoras_tree(l2, draw_poly, draw_triangle, isTerminate, depth+1);
}

/*inline crd2 interpolateSpline(const spline2& spline, const space2& a, const space2& b, double pos, int type = 0) {
	crd2 result;
	double alpha = getAngle(vec2(0), a.i);
	double beta = getAngle(vec2(0), b.i);

	if (type == 0) type = (getClockwiseDistance(alpha, beta) < _SPOB_PI) + 1;

	auto calc_angle = [&] (double pos) -> double {
		auto cpos = spline.value(pos);
		vec2 cpos1(cpos.x, cpos.y);
		auto dpos = spline.value(pos, 1);
		auto dnew = rotate(vec2(dpos.x, dpos.y), cpos1, -90);
		return getAngle(cpos1, dnew);
	};

	auto calc_mx = [&] (double pos) -> double {
		auto dpos = spline.value(pos, 1);
		dpos.z = 0;
		return spob::vec2(dpos.x, dpos.y).length();
	};

	double startAngle = calc_angle(1);
	double currentAngle = calc_angle(1-pos);
	double endAngle = calc_angle(0);
	double anglePos = getClockwiseDistance(startAngle, currentAngle)/getClockwiseDistance(startAngle, endAngle);

	double startMx = calc_mx(1);
	double currentMx = calc_mx(1-pos);
	double endMx = calc_mx(0);
	double mxPos = (startMx/currentMx);

	spob::space2 start;
	glm::vec3 vpos = spline.value(1);
	start.pos = vec2(vpos.x, vpos.y);
	glm::vec3 dpos = spline.value(1, 1);
	start.i = vec2(dpos.x, dpos.y)/startMx * a.i.length();
	start.j = rotate(start.i, vec2(0), spob::deg2rad(90));

	spob::space2 current;
	vpos = spline.value(1-pos);
	current.pos = vec2(vpos.x, vpos.y);
	dpos = spline.value(1-pos, 1);
	current.i = vec2(dpos.x, dpos.y)/startMx * a.i.length();
	current.j = rotate(current.i, vec2(0), spob::deg2rad(90));

	double an1 = getSlopeAngle(a);
	double an2 = getSlopeAngle(start);
	double an3 = getClockwiseDistance(an1, an2);
	current = rotate(current, current.pos, an3);

	return current;
}*/

ostream& operator<<(ostream& out, const spob::vec2& v) {
	out << "(" << v.x << ", " << v.y << ")";
	return out;
}

ostream& operator<<(ostream& out, const glm::vec3& v) {
	out << "(" << v.x << ", " << v.y << ", " << v.z << ")";
	return out;
}

vec2 getPos(const VectorXd& coefs, double t) {
	return vec2(coefs[0]*t*t + coefs[1]*t + coefs[2], coefs[3]*t*t + coefs[4]*t + coefs[5]);
}

VectorXd mul(const VectorXd& coefs, glm::mat3 m) {
	vec2 i(m[0][0], m[0][1]);
	vec2 j(m[1][0], m[1][1]);
	vec2 p(m[2][0], m[2][1]);
	VectorXd res(6);
	res[0] = coefs[0]*i.x + coefs[3]*j.x;
	res[1] = coefs[1]*i.x + coefs[4]*j.x;
	res[2] = coefs[2]*i.x + coefs[5]*j.x + p.x;

	res[3] = coefs[0]*i.y + coefs[3]*j.y;
	res[4] = coefs[1]*i.y + coefs[4]*j.y;
	res[5] = coefs[2]*i.y + coefs[5]*j.y + p.y;

	return res;
}

VectorXd findCoefs(glm::mat3 m) {
	vec2 i(m[0][0], m[0][1]);
	vec2 j(m[1][0], m[1][1]);
	vec2 p(m[2][0], m[2][1]);
	MatrixXd A(6, 6);
	A << 
		1, 1, 1-i.x, 0, 0, -j.x,
		0, 0, -i.y, 1, 1, 1-j.y,
		//1, 1, 1, 0, 0, 0,
		//0, 0, 0, 1, 1, 1,
		2, 1-i.x, 0, 0, -j.x, 0, 
		0, -i.y, 0, 2, 1-j.y, 0,
		0, 0, 1, 0, 0, 0,
		0, 0, 0, 0, 0, 1;
		//1-i.x, 0, 0, -j.x, 0, 0,
		//-i.y, 0, 0, 1-j.y, 0, 0;
	//cout << A << endl;
	VectorXd b(6);
	b << p.x, p.y, p.x, p.y, 0, 0;
	//cout << b << endl;
	VectorXd x = A.fullPivLu().solve(b);
	//cout << x << endl;

	cout << m * glm::vec3(0, 0, 1) << endl;
	cout << getPos(x, 0) << endl;
	cout << getPos(x, 1) << endl;

	//auto newX = mul(x, m);
	//cout << getPos(newX, 0) << endl;
	//cout << getPos(newX, 1) << endl;

	//cout << endl << newX << endl;

	return x;
}

//-----------------------------------------------------------------------------
void draw_logo(void) {
	/*// Coordinates of data points.
	std::vector<double> coo = {0.0, 0.0, 1.0};

	// Data values.
	std::vector<double> val = {0.2, 0.0, 0.0, -0.2, -1.0, 1.0};

	// Bounding box containing the data points.
	mba::point<2> lo = {-0.1, -0.1};
	mba::point<2> hi = { 1.1,  1.1};

	// Initial grid size.
	mba::index<2> grid = {3, 3};

	// Algorithm setup.
	mba::MBA<2> interp(lo, hi, grid, coo, val);

	// Get interpolated value at arbitrary location.
	double w = interp(mba::point<2>{0.3, 0.7});*/


	ImageGif gif;
	ImageGif gif2;

	vec2 size(500, 500 * 1.25);
	vec2 size2(750, 500);

	crd2 standard = getStandardCrd2();
	/*space2 screen = standard;
	screen = rotate(screen, vec2(0, 0), spob::deg2rad(0));
	screen.pos = size / 2.0 + vec2(0.5, 0.5);
	screen.i *= 80;
	screen.j *= 80;
	screen.j.negate();

	screen.pos = screen.from(vec2(4.5, -1.6));*/

	crd2 screen = standard;
	screen.pos = vec2(-5, -3);
	double mx = 10;
	screen.i *= mx * 1.5;
	screen.j *= mx;


	//-------------------------------------------------------------------------
	Image img(size, screen);
	Image img2(size2, screen);
	Image imgStatic(size2, screen);

	img.setViewPort(screen);
	img2.setViewPort(screen);
	imgStatic.setViewPort(screen);

	/*imgStatic.thick = 0.01;
	space2 standard_copy = standard;
	standard_copy.move({-0.2, -0.2});
	standard_copy.i *= 2;
	standard_copy.j *= 2;
	imgStatic.setViewPort(standard_copy);
	imgStatic.set_pen(1.5/80.0, setAlpha(Gray, 192));
	imgStatic.draw_grid(standard);
	imgStatic.set_pen(1/80.0, Black);
	imgStatic.draw_crd(standard);
	imgStatic.save("test.png");
	system("test.png");*/

	vector<vec2> square = placePolyOnEdge(calcRegularPolygon(4, vec2(0), 1, 0), 0);

	double alpha = spob::deg2rad(45);
	vector<vec2> sq = placePolyOnEdge(calcRegularPolygon(5, vec2(0), 1, 0), 0);

	imgStatic.draw_polygon(sq);
	vector<vec2> tr = {vec2(0, 0), vec2(1, 0), rotate(vec2(cos(alpha), 0), vec2(0), -alpha)};

	auto poly = sumPolygons(sq, tr, 2, 0);

	imgStatic.draw_polygon(poly);

	space2 line = makeLine2(poly[4], poly[3]);
	poly = toMas(line, poly);
	imgStatic.draw_polygon(poly);

	cout << "line.to({0, 0}): " << line.to({0, 0}) << endl;
	cout << "line.from({0, 0}): " << line.from({0, 0}) << endl;

	auto P = getFromMatrix(line);
	//spline2 spline(8);
	//spline.solve_coefs(P, {0, 0, 1});
	//auto coefs = findCoefs(P);

	SplineInterpolator interpolator(8, standard, line);

	cout << spob::rad2deg(getSlopeAngle(line)) << endl;
	vec2 offset = cartesian2polar(getOffset(line));
	cout.precision(16);
	cout << offset.x << " " << offset.y << endl;
	offset = cartesian2polar(line.to(vec2(0, 0)));
	cout << offset.x << " " << offset.y << endl;

	poly = toMas(line, poly);
	imgStatic.draw_polygon(poly);

	gif.start(img.img.size(), "interpolation.gif");
	gif2.start(img2.img.size(), "interpolation2.gif");

	//for (int angle = 0; angle < 360; angle += 5) {
	int angle = 45; {
	/*crd2 a = rotate(standard, vec2(0, 0), spob::deg2rad(angle));
	a.move(vec2(-5, 2));
	a.i *= 3;
	a.j *= 3;*/
	crd2 a = line;

	// Сетка и координаты
	//imgStatic.img.clear(White);
	imgStatic.set_pen(1.5/80.0, setAlpha(Gray, 192));
	imgStatic.draw_grid(standard);
	imgStatic.set_pen(1/80.0, Black);
	imgStatic.draw_crd(standard);
	imgStatic.draw_crd(a);
	imgStatic.set_pen(2.5/80.0, Gray);

	imgStatic.draw_polygon(poly);
	imgStatic.draw_polygon(fromMas(line, poly));

	double count = 60;
	for (int i = 0; i < count; ++i) {
		cout << i << endl;

		double pos = i/count;

		// Интерполированная система координат
		space2 c = interpolator.interpolate(pos);
		//space2 c = interpolateSpline(spline, a, standard, pos, 1);
		//space2 c = interpolateCircular(a, standard, pos, 12.45, 1, true);
		//crd2 c = interpolate(a, standard, pos);

		/*double viewPortScale = 2;
		space2 d = c;
		d.pos += c.fromDir({-viewPortScale , -viewPortScale });
		d.i *= 1 + viewPortScale * 2;
		d.j *= 1 + viewPortScale * 2;*/
		double xoffset = 3;
		space2 d = c;
		d.pos += c.fromDir({-2 , -3});
		d.i *= 1 + 2 * xoffset;
		d.j *= (1 + 2 * xoffset) * 1.25;
		img.setViewPort(d);

		// Сетка и координаты
		img.img.clear(White);
		//img.set_pen(1.5/80.0, setAlpha(Gray, 192));
		//img.draw_grid(standard);
		//img.set_pen(1/80.0, Black);
		//img.draw_crd(standard);
		//img.draw_crd(a);
		//img.draw_crd(c);
		//img.set_pen(2.5/80.0, Gray);
		img.set_pen(0.5/80.0, Black);

		double max_len = 170;
		double start_min_len = 10;
		double min_len = 5.1;

		auto draw = [&] (const vector<vec2>& poly) {
			double len = distance(img.screen_tr.from(poly[1]), img.screen_tr.from(poly[0]));
			double pos = (len-min_len)/(max_len-min_len);

			double alphapos = 1;
			if (len < start_min_len) alphapos = (len - min_len)/(start_min_len-min_len); 

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

		img2.img.clear(White);
		img2.set_pen(1.5/80.0, setAlpha(Gray, 192));
		img2.draw_grid(standard);
		img2.set_pen(1/80.0, Black);

		#ifndef _DEBUG
		draw_pythagoras_tree(makeLine2(poly.back(), poly.front()), draw, draw, [&] (vec2 i) -> bool {
			return img.screen_tr.fromDir(i).length() < min_len;
		});
		#endif

		img2.draw_crd(standard);
		img2.draw_crd(a);
		img2.draw_crd(c);

		img2.set_pen(2.5/80.0, Gray);
		img2.draw_polygon(fromMas(d, square));

		img2.set_pen(0.5/80.0, Gray);

		imgStatic.draw_crd(c);

		/*auto splinecopy = spline;
		for (int i = 0; i < 10; i++) {
			img2.set_pen(2/80.0, getColorBetween(i/10.0, Bitcoin, Green));
			double countLine = 20;
			for (int j = 0; j < countLine; j++) {
				auto p1 = splinecopy.value(j/countLine);
				auto p2 = splinecopy.value((j+1)/countLine);
				img2.draw_line(vec2(p1.x, p1.y), vec2(p2.x, p2.y));
			}
			splinecopy.mul(P);
		}*/

		gif.process(img.img, 2);
		gif2.process(img2.img, 2);
	}
	//gif.process(imgStatic.img, 16);
	}
	gif.end();
	gif2.end();

	imgStatic.save("interpolation.png");
}

//-----------------------------------------------------------------------------
int main() {
	draw_logo();
}