#include <algorithm>
#include "draw.h"

//-----------------------------------------------------------------------------
Inter intersect(const rect2& rect, const line2& line,
			   double& t1, double& t2) {
	double xmin = (rect.a.x - line.pos.x)/line.i.x;
	double xmax = (rect.b.x - line.pos.x)/line.i.x;

	double ymin = (rect.a.y - line.pos.y)/line.i.y;
	double ymax = (rect.b.y - line.pos.y)/line.i.y;

	if (xmin > xmax) std::swap(xmin, xmax);
	if (ymin > ymax) std::swap(ymin, ymax);

	t1 = std::max(xmin, ymin);
	t2 = std::min(xmax, ymax);

	if (t2 > t1)
		return INTER_OVERLAPS;
	else
		return INTER_ABSENT;
}

//-----------------------------------------------------------------------------
Inter intersect(const rect2& rect, const line2& line,
			   vec2& a, vec2& b) {
	double t1, t2;
	if (intersect(rect, line, t1, t2) == INTER_OVERLAPS) {
		a = line.from(t1);
		b = line.from(t2);
		return INTER_OVERLAPS;
	} else
		return INTER_ABSENT;
}

//-----------------------------------------------------------------------------
double findBorder(const std::function<bool(double)>& f) {
	// Функция f имеет вид: f([0; c]) = true; f([c; inf]) = false. Данная функция ищет c.
	double a = 0;
	double b = 1;
	while (fabs(b - a) > 0.0001)  {
		if (f(b)) {
			b *= 2;
		} else {
			double c = (a + b) / 2.0;
			if (f(c))
				a = c;
			else
				b = c;
		}
	}

	return (a + b)/2.0;
}

//=============================================================================
//=============================================================================
//=============================================================================

Image::Image(const vec2& size, const space2& screen_tr) : img(size), screen_tr(screen_tr) {
	img.clear(White);
	rect.a = screen_tr.to(vec2(0, 0));
	rect.b = screen_tr.to(size);
}

//-----------------------------------------------------------------------------
void Image::draw_line(const vec2& a1, const vec2& b1) {
	vec2 a = screen_tr.from(a1);
	vec2 b = screen_tr.from(b1);
	img.drawLine(a, b);
}

//-----------------------------------------------------------------------------
void Image::draw_dashed_line(const vec2& a, const vec2& b, double size, double space) {
	line2 line = makeLine2(a, b);
	line.orthonormalize();
	double length = distance(a, b), i = 0;
	vec2 last = a;
	while (i < length) {
		if (i + size > length)
			i = length - size;
		draw_line(last, line.from(i + size));
		last = line.from(i + size + space);
		i += size + space;
	}
}

//-----------------------------------------------------------------------------
void Image::draw_crd(const crd2& crd) {
	Color current_clr = clr;
	double arrow_size = std::max(crd.i.length(), crd.j.length()) * 0.1;
	double arrow_angle = deg2rad(30);
	set_pen(thick, Red);
	draw_arrow(crd.pos, crd.pos + crd.i, arrow_angle, arrow_size);
	set_pen(thick, Blue);
	draw_arrow(crd.pos, crd.pos + crd.j, arrow_angle, arrow_size);
	set_pen(thick, current_clr);
	draw_circle(crd.pos, thick, Black);
}

//-----------------------------------------------------------------------------
void Image::draw_arrow(const vec2& a, const vec2& b, double angle, double size) {
	draw_line(a, b);

	line2 line = makeLine2(b, a);

	line.normalize();

	vec2 first(size, 0);
	vec2 second(size, 0);

	first = rotate(first, vec2(0), angle);
	second = rotate(second, vec2(0), -angle);

	space2 space_of_line(line);
	draw_line(b, space_of_line.from(first));
	draw_line(b, space_of_line.from(second));
}

//-----------------------------------------------------------------------------
void Image::draw_inf_line(const line2& line) {
	vec2 a, b;
	if (intersect(rect, line, a, b))
		draw_line(a, b);
}

//-----------------------------------------------------------------------------
void Image::draw_line2(const line2& line) {
	double current_thick = thick;
	Color current_clr = clr;
	double arrow_size = std::max(line.i.length(), line.j.length()) * 0.1;
	double arrow_angle = deg2rad(30);

	set_pen(current_thick/1.5, setAlpha(Gray, 128));
	draw_inf_line(line);

	Color clr = setLightness(Gray,  getLightness(Gray) / 1.1);
	
	set_pen(current_thick / 2.0, clr);
	double t1, t2;
	if (intersect(rect, line, t1, t2) == INTER_OVERLAPS) {
		for (int i = t1; i < t2; i++) {
			//if (i != 0 && i != 1)
				//draw_line(line.from(i) + line.j * 0.03, line.from(i) - line.j * 0.03);
		}
	}

	set_pen(current_thick, Green);
	draw_arrow(line.pos, line.from(1), arrow_angle, arrow_size);

	draw_circle(line.pos, thick, Black);

	set_pen(current_thick, current_clr);
}

//-----------------------------------------------------------------------------
void Image::draw_grid(const space2& space) {
	double current_thick = thick;
	double current_clr = clr;

	auto draw_one_dim_grid = [&] (line2 l) {
		auto offset_l = [&l] (const line2& l, double a) -> line2 {
			line2 lcopy = l;
			lcopy.pos += lcopy.j * a;
			return lcopy;
		};

		auto is_line_border = [&] (const line2& l, double a) -> bool {
			double t1, t2;
			return intersect(rect, offset_l(l, a), t1, t2);
		};

		l.j.negate();
		double lmin = findBorder(std::bind(is_line_border, l, std::placeholders::_1));
		l.j.negate();
		double lmax = findBorder(std::bind(is_line_border, l, std::placeholders::_1));

		const double coef = 2;
		set_pen(current_thick / coef, setAlpha(current_clr, getAlpha(current_clr) / coef));
		for (int i = -lmin; i < lmax; i++) {
			if (i == 0) {
				set_pen(current_thick, current_clr);
				draw_inf_line(offset_l(l, i));
				set_pen(current_thick / coef, setAlpha(current_clr, getAlpha(current_clr) / coef));
			} else {
				draw_inf_line(offset_l(l, i));
			}
		}

		const double coef2 = 2 * coef;
		set_pen(current_thick / coef2, setAlpha(current_clr, getAlpha(current_clr) / coef2));
		for (int i = -lmin * 5; i < lmax * 5; i++) {
			if (i % 5 != 0)
				draw_inf_line(offset_l(l, i/5.0));
		}
	};

	draw_one_dim_grid(space);
	draw_one_dim_grid(space2(space.j, space.i, space.pos));

	set_pen(current_thick, current_clr);
}

//-----------------------------------------------------------------------------
void Image::draw_polygon(const std::vector<vec2>& polygon) {
	for (int i = 0; i < polygon.size()-1; ++i)
		draw_line(polygon[i], polygon[i+1]);
	draw_line(polygon.back(), polygon.front());
}

//-----------------------------------------------------------------------------
void Image::draw_circle(const vec2& pos1, double r1, Color clr) {
	vec2 pos = screen_tr.from(pos1);
	double r = screen_tr.fromDir(vec2(r1, 0)).x;
	img.setBrush(clr);
	img.drawPolygon(computeEllipse(vec2(r)).move(pos));
}

//-----------------------------------------------------------------------------
void Image::draw_project(const vec2& pos, const line2& line) {
	draw_circle(pos, thick * 2.0, clr);
	draw_dashed_line(pos, line.project(pos), 10/80.0, 3/80.0);
	draw_circle(line.project(pos), thick, clr);
}

//-----------------------------------------------------------------------------
void Image::draw_intersect(const line2& l1, const line2& l2) {
	vec2 common;
	intersect(l1, l2, common);
	draw_circle(common, thick * 2, Red);
}

//-----------------------------------------------------------------------------
void Image::set_pen(double thick1_, Color clr1) {
	thick = thick1_;
	double thick1 = screen_tr.fromDir(vec2(thick1_, 0)).x;
	clr = clr1;
	img.setPen(Pen(thick1, clr));
}

//-----------------------------------------------------------------------------
void Image::save(std::string file) {
	saveToBmp(&img, std::wstring(file.begin(), file.end()), true);
}