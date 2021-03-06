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
vector<vec2> calcRegularPolygon(int n, vec2 center, double r, double fi) {
	vector<vec2> result;
	for (int i = 0; i < n; ++i) {
		double angle = -i * 2.0 * _SPOB_PI/n + fi;
		result.push_back(center + r * vec2(sin(angle), cos(angle)));
	}
	return result;
}

//-----------------------------------------------------------------------------
vector<vec2> placePolyOnEdge(const vector<vec2>& poly, int edge) {
	vec2 a = poly[edge], b = poly[edge+1 == poly.size() ? 0 : edge+1];
	space2 l = makeLine2(a, b);

	vector<vec2> result;
	for (auto& i : poly)
		result.push_back(l.to(i));

	return result;
}

//-----------------------------------------------------------------------------
vector<vec2> rotatePoly(const vector<vec2>& poly, int edge) {
	vector<vec2> result;
	for (int i = edge; i < poly.size(); ++i)
		result.push_back(poly[i]);

	for (int i = 0; i < edge; ++i)
		result.push_back(poly[i]);

	return result;
}

//-----------------------------------------------------------------------------
vector<vec2> sumPolygons(const vector<vec2>& poly1, const vector<vec2>& poly2, int edge1, int edge2) {
	auto poly2r = rotatePoly(placePolyOnEdge(poly2, edge2), edge2);

	vector<vec2> result;
	for (int i = 0; i <= edge1; ++i)
		result.push_back(poly1[i]);

	vec2 a = poly1[edge1], b = poly1[edge1+1 == poly1.size() ? 0 : edge1+1];
	space2 line = makeLine2(a, b);

	for (int i = poly2r.size()-1; i > 1; --i)
		result.push_back(line.from(poly2r[i]));

	for (int i = edge1+1; i < poly1.size(); ++i)
		result.push_back(poly1[i]);

	return result;
}

//-----------------------------------------------------------------------------
space2 calcViewPort(const vec2& min, const vec2& max) {
	space2 result;
	result.pos = min;
	result.i = vec2(max.x-min.x, 0);
	result.j = vec2(0, max.y-min.y);
	return result;
}

//-----------------------------------------------------------------------------
space2 increaseViewportBorderByMinAxis(const space2& viewport, double percent) {
	space2 result = viewport;
	double minLen = std::min(result.i.length(), result.j.length());
	double muli = (result.i.length() + minLen * 2 * percent)/result.i.length();
	double mulj = (result.j.length() + minLen * 2 * percent)/result.j.length();
	result.pos -= result.fromDir(vec2((muli-1)/2.0, (mulj-1)/2.0));
	result.i *= muli;
	result.j *= mulj;
	return result;
}

//-----------------------------------------------------------------------------
std::pair<crd2, vec2> calcViewPortAndSize(vec2 min, vec2 max, int maxSize) {
	auto viewport = calcViewPort(min, max);
	double coef = viewport.i.length() / viewport.j.length();

	vec2 size;
	if (coef > 1)
		size = vec2(maxSize, maxSize / coef);
	else
		size = vec2(maxSize * coef, maxSize);

	return { viewport, size };
}


//=============================================================================
//=============================================================================
//=============================================================================

Image::Image(const vec2& size, const space2& screen_tr, int layers) : screen_tr(screen_tr), alpha(255) {
	for (int i = 0; i < layers; i++)
		imgs.push_back(make_shared<ImageDrawing_aa>(size));
	for (auto& img : imgs)
		img->clear(Transparent);
	rect.a = vec2(0, 0);
	rect.b = size;
}

//-----------------------------------------------------------------------------
void Image::draw_line(const vec2& a1, const vec2& b1, int layer) {
	vec2 a = screen_tr.from(a1);
	vec2 b = screen_tr.from(b1);
	line2 l = makeLine2(a, b);
	vec2 start, end;
	if (intersect(rect, l, start, end)) {
		double t1 = l.to(start);
		double t2 = l.to(end);
		t1 = min(max(0.0, t1), 1.0);
		t2 = min(max(0.0, t2), 1.0);
		if (fabs(t1 - t2) > 0.0001)
			imgs[layer]->drawLine(l.from(t1), l.from(t2));
	}
}

//-----------------------------------------------------------------------------
void Image::draw_dashed_line(const vec2& a, const vec2& b, double size, double space, int layer) {
	line2 line = makeLine2(a, b);
	line.orthonormalize();
	double length = distance(a, b), i = 0;
	vec2 last = a;
	while (i < length) {
		if (i + size > length)
			i = length - size;
		draw_line(last, line.from(i + size), layer);
		last = line.from(i + size + space);
		i += size + space;
	}
}

//-----------------------------------------------------------------------------
void Image::draw_crd(const crd2& crd, int layer) {
	Color current_clr = clr;
	double arrow_size = std::max(crd.i.length(), crd.j.length()) * 0.1;
	double arrow_angle = spob::deg2rad(30);
	set_pen(thick, Red, layer);
	draw_arrow(crd.pos, crd.pos + crd.i, arrow_angle, arrow_size, layer);
	set_pen(thick, Blue, layer);
	draw_arrow(crd.pos, crd.pos + crd.j, arrow_angle, arrow_size, layer);
	set_pen(thick, Black, layer);
	draw_circle(crd.pos, thick, Black, layer);
	set_pen(thick, current_clr, layer);
}

//-----------------------------------------------------------------------------
void Image::draw_arrow(const vec2& a, const vec2& b, double angle, double size, int layer) {
	draw_line(a, b, layer);

	line2 line = makeLine2(b, a);

	line.normalize();

	vec2 first(size, 0);
	vec2 second(size, 0);

	first = rotate(first, vec2(0), angle);
	second = rotate(second, vec2(0), -angle);

	space2 space_of_line(line);
	draw_line(b, space_of_line.from(first), layer);
	draw_line(b, space_of_line.from(second), layer);
}

//-----------------------------------------------------------------------------
void Image::draw_inf_line(const line2& line, int layer) {
	vec2 a, b;
	if (intersect(rect, screen_tr.from(line), a, b))
		draw_line(screen_tr.to(a), screen_tr.to(b), layer);
}

//-----------------------------------------------------------------------------
void Image::draw_line2(const line2& line, int layer) {
	double current_thick = thick;
	Color current_clr = clr;
	double arrow_size = std::max(line.i.length(), line.j.length()) * 0.1;
	double arrow_angle = spob::deg2rad(30);

	set_pen(current_thick/1.5, setAlpha(Gray, 128), layer);
	draw_inf_line(line, layer);

	Color clr = setLightness(Gray,  getLightness(Gray) / 1.1);
	
	set_pen(current_thick / 2.0, clr, layer);
	double t1, t2;
	if (intersect(rect, line, t1, t2) == INTER_OVERLAPS) {
		for (int i = t1; i < t2; i++) {
			//if (i != 0 && i != 1)
				//draw_line(line.from(i) + line.j * 0.03, line.from(i) - line.j * 0.03);
		}
	}

	set_pen(current_thick, Green, layer);
	draw_arrow(line.pos, line.from(1), arrow_angle, arrow_size, layer);

	draw_circle(line.pos, thick, Black, layer);

	set_pen(current_thick, current_clr, layer);
}

//-----------------------------------------------------------------------------
void Image::draw_grid(const space2& space, int layer) {
	double current_thick = thick;
	Color current_clr = clr;

	auto draw_one_dim_grid = [&] (line2 l) {
		auto offset_l = [] (const line2& l, double a) -> line2 {
			line2 lcopy = l;
			lcopy.pos += lcopy.j * a;
			return lcopy;
		};

		auto is_line_border = [&] (const line2& l, double a) -> bool {
			double t1, t2;
			return intersect(rect, screen_tr.from(offset_l(l, a)), t1, t2);
		};

		vec2 screenCenter = screen_tr.to(vec2(0.5, 0.5));
		double dist = space2(l).to(screenCenter).y;

		double coef = 2;
		set_pen(current_thick / coef, setAlpha(current_clr, getAlpha(current_clr) / coef), layer);
		auto draw_line = [&] (double offset) {
			if (offset == 0) {
				set_pen(current_thick, current_clr, layer);
				draw_inf_line(offset_l(l, offset), layer);
				set_pen(current_thick / coef, setAlpha(current_clr, getAlpha(current_clr) / coef), layer);
			} else {
				draw_inf_line(offset_l(l, offset), layer);
			}
		};

		if (is_line_border(l, int(dist)))
			draw_line(int(dist));

		int i;
		
		i = dist;
		while (is_line_border(l, i+1)) {
			i++;
			draw_line(i);
		}

		i = dist;
		while (is_line_border(l, i-1)) {
			i--;
			draw_line(i);
		}

		coef = 2.0 * coef;
		set_pen(current_thick / coef, setAlpha(current_clr, getAlpha(current_clr) / coef), layer);

		if (is_line_border(l, int(dist * 5)/5.0))
			draw_line(int(dist * 5)/5.0);

		i = dist * 5;
		while (is_line_border(l, (i+1)/5.0)) {
			i++;
			if (i % 5 != 0)
				draw_line(i/5.0);
		}

		i = dist * 5;
		while (is_line_border(l, (i-1)/5.0)) {
			i--;
			if (i % 5 != 0)
				draw_line(i/5.0);
		}
	};

	draw_one_dim_grid(space);
	draw_one_dim_grid(space2(space.j, space.i, space.pos));

	set_pen(current_thick, current_clr, layer);
}

//-----------------------------------------------------------------------------
void Image::draw_polygon(const std::vector<vec2>& polygon, int layer) {
	for (int i = 0; i < polygon.size()-1; ++i)
		draw_line(polygon[i], polygon[i+1], layer);
	draw_line(polygon.back(), polygon.front(), layer);
}

//-----------------------------------------------------------------------------
void Image::draw_polyfill(const std::vector<vec2>& polygon, Color clr, int layer) {
	Polygon_d poly;
	for (auto& i : polygon)
		poly.array.push_back(screen_tr.from(i));
	imgs[layer]->setBrush(setAlpha(clr, alpha));
	imgs[layer]->drawPolygon(poly);
}

//-----------------------------------------------------------------------------
void Image::draw_circle(const vec2& pos1, double r1, Color clr, int layer) {
	vec2 pos = screen_tr.from(pos1);
	double r = screen_tr.fromDir(vec2(r1, 0)).x;
	imgs[layer]->setBrush(setAlpha(clr, alpha));
	imgs[layer]->drawPolygon(computeEllipse(vec2(r)).move(pos));
}

//-----------------------------------------------------------------------------
void Image::draw_project(const vec2& pos, const line2& line, int layer) {
	draw_circle(pos, thick * 2.0, clr, layer);
	draw_dashed_line(pos, line.project(pos), 10/80.0, 3/80.0, layer);
	draw_circle(line.project(pos), thick, clr, layer);
}

//-----------------------------------------------------------------------------
void Image::draw_intersect(const line2& l1, const line2& l2, int layer) {
	vec2 common;
	intersect(l1, l2, common);
	draw_circle(common, thick * 2, Red, layer);
}

//-----------------------------------------------------------------------------
void Image::set_pen(double thick1_, Color clr1, int layer) {
	thick = thick1_;
	double thick1 = fromThick(thick1_);
	clr = clr1;
	imgs[layer]->setPen(Pen(thick1, setAlpha(clr1, alpha)));
}

//-----------------------------------------------------------------------------
void Image::set_alpha(int _alpha) {
	alpha = _alpha;
	set_pen(thick, clr);
}

//-----------------------------------------------------------------------------
void Image::save(std::string file) {
	saveToPng(&(*imgs[0]), file, true);
}

//-----------------------------------------------------------------------------
void Image::clear(Color clr) {
	for (auto& i : imgs)
		i->clear(Transparent);
	imgs[0]->clear(clr);
}

//-----------------------------------------------------------------------------
void Image::combine_layers(void) {
	for (int i = 1; i < imgs.size(); i++)
		imgs[i]->drawTo(&(*imgs[0]), Point_i(0, 0), Point_i(0, 0), imgs[0]->size(), imgs[0]->size());
}

//-----------------------------------------------------------------------------
void Image::setViewPort(const space2& view) {
	space2 standard = getStandardCrd2();
	standard.i *= imgs[0]->width();
	standard.j *= imgs[0]->height();
	standard.pos.y += imgs[0]->height();
	standard.j.negate();

	vec2 a(0.5, 0.5);

	vec2 pos = standard.from(view.to(a));

	screen_tr = invert(combine(invert(standard), view));

	pos = screen_tr.from(a);

	pos = space2(invert(screen_tr)).from(a);
}

//-----------------------------------------------------------------------------
double Image::fromThick(double a) {
	return screen_tr.fromDir(vec2(a, 0)).x;
}