#include <iostream>
#include <functional>
#include <algorithm>
#include <string>

#include <spob/spob.h>
#include <twg/image/image_drawing.h>

#undef max
#undef min

using namespace spob;
using namespace twg;

struct rect2 { vec2 a, b; };

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
bool intersect(const rect2& rect, const line2& line,
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

//-----------------------------------------------------------------------------
class Image
{
public:
	Image(const vec2& size, const space2& screen_tr);

	void draw_line(const vec2& a, const vec2& b);
	void draw_dashed_line(const vec2& a, const vec2& b, double size, double space);
	void draw_inf_line(const line2& line);
	void draw_arrow(const vec2& a, const vec2& b, double angle, double size);
	void draw_crd(const crd2& crd);
	void draw_line2(const line2& line);
	void draw_grid(const space2& space);
	//void draw_path(const std::vector<vec2>& path);
	void draw_circle(const vec2& pos, double r, Color clr);

	void draw_project(const vec2& pos, const line2& line);
	void draw_intersect(const line2& l1, const line2& l2);

	void set_pen(double thick, Color clr);

	void save(std::string file);

	ImageDrawing_aa img;
	rect2 rect;
	double thick;
	Color clr;
	space2 screen_tr;
};

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
/*void Image::draw_path(const std::vector<vec2>& path) {
	for (int i = 0; i < path.size()-1; ++i)
		draw_line(path[i], path[i+1]);
	draw_line(path[path.size() - 1], path[0]);
}*/

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

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void draw_coords(void) {
	int counter = 0;
	vec2 size(900, 450);

	crd2 standard = getStandardCrd2();
	space2 screen = standard;
	screen = rotate(screen, vec2(0, 0), deg2rad(0));
	screen.pos = size / 2.0 + vec2(0.5, 0.5);
	screen.i *= 80;
	screen.j *= 80;
	screen.j.negate();

	//-------------------------------------------------------------------------
	Image img(size, screen);

	auto draw_rect = [&] (const space2& space) {
		auto old_screen_tr = img.screen_tr;
		img.screen_tr = img.screen_tr.from(space);

		vec2 a(0, 0), b(1, 0), c(1, 1), d(0, 1);
		img.draw_line(a, b);
		img.draw_line(b, c);
		img.draw_line(c, d);
		img.draw_line(d, a);

		double old_thick = img.thick;
		img.set_pen(img.thick * 3, img.clr);
		double start = 0.1;
		double h = 0.4;
		vec2 a1(start, 0), a2(0.5, 1), a3(1 - start, 0), a4(start + (0.5 - start) * (h+0.05), h+0.05), a5(1 - start - (0.5 - start) * (h-0.05), h-0.05);
		img.draw_line(a1, a2);
		img.draw_line(a2, a3);
		img.draw_line(a4, a5);
		img.set_pen(old_thick, img.clr);

		img.screen_tr = old_screen_tr;
	};

	//img.save(std::to_string(counter++)+".bmp");img.img.clear(Transparent);
	//-------------------------------------------------------------------------
	// Сетка и координаты
	img.set_pen(1.5/80.0, setAlpha(Gray, 192));
	img.draw_grid(standard);
	img.set_pen(1/80.0, Black);
	draw_rect(standard);
	img.set_pen(2.5/80.0, Gray);
	img.draw_crd(standard);

	//img.save(std::to_string(counter++)+".bmp");img.img.clear(Transparent);
	//-------------------------------------------------------------------------
	// Линия 1
	img.set_pen(2.5/80.0, Gray);
	line2 l1 = makeLine2(vec2(-3, 1), vec2(-2, 1.3));
	l1.orthonormalize();
	img.draw_line2(l1);

	// Проекция на линию 1
	img.set_pen(1/80.0, setAlpha(Black, 128));
	img.draw_project(vec2(-4, 2), l1);

	//img.save(std::to_string(counter++)+".bmp");img.img.clear(Transparent);
	//-------------------------------------------------------------------------
	// Линия 2
	img.set_pen(2.5/80.0, Gray);
	line2 l2 = makeLine2(vec2(4, 0.2), vec2(3.5, 0.5));
	img.draw_line2(l2);

	// Проекция на линию 2
	img.set_pen(1/80.0, setAlpha(Black, 128));
	img.draw_project(vec2(1.5, 0.3), l2);

	//img.save(std::to_string(counter++)+".bmp");img.img.clear(Transparent);
	//-------------------------------------------------------------------------
	// Пересечение линий
	img.set_pen(1.5/80.0, img.clr);
	img.draw_intersect(l1, l2);

	//img.save(std::to_string(counter++)+".bmp");img.img.clear(Transparent);
	//-------------------------------------------------------------------------
	// Координаты 1
	auto crd_1 = rotate(standard, vec2(0, 0), deg2rad(30));
	crd_1.pos = vec2(2.2, -1.6);
	crd_1.pos += crd_1.i;
	crd_1.i.negate();
	img.set_pen(1/80.0, Black);
	draw_rect(crd_1);
	img.set_pen(1.5/80.0, Black);
	img.draw_crd(crd_1);

	//img.save(std::to_string(counter++)+".bmp");img.img.clear(Transparent);
	//-------------------------------------------------------------------------
	// Координаты 2
	auto crd_2 = rotate(standard, vec2(0, 0), deg2rad(-49));
	crd_2.i += crd_2.j;
	crd_2.pos = vec2(-4.77, -1.2);
	crd_2.i *= 1.5;
	crd_2.j *= 1.5;
	img.set_pen(1/80.0, Black);
	draw_rect(crd_2);
	img.set_pen(1.5/80.0, Black);
	img.draw_crd(crd_2);

	//img.save(std::to_string(counter++)+".bmp");img.img.clear(Transparent);
	//-------------------------------------------------------------------------
	// Проекция точки на координаты
	vec2 p(-0.9, 0.7);
	img.draw_circle(p, 3/80.0, Black);
	img.set_pen(1/80.0, Black);
	img.draw_dashed_line(p, vec2(p.x, 0), 10/80.0, 3/80.0);
	img.draw_dashed_line(p, vec2(0, p.y), 10/80.0, 3/80.0);

	//img.save(std::to_string(counter++)+".bmp");img.img.clear(Transparent);
	//-------------------------------------------------------------------------
	// Текст
	ImageDrawing_win imgw(&img.img);
	imgw.setPen(Pen(1, Black));
	double font_size = 50.0/80.0 * img.screen_tr.i.length();
	double offset = 10.0/80.0 * img.screen_tr.i.length();
	imgw.setTextStyle(TextStyle(font_size, L"Ubuntu Mono", TEXT_NONE));
	std::wstring text = L"space objects";
	auto text_size = imgw.getTextSize(text);
	imgw.drawText(vec2((size.x - text_size.x)/2.0, size.y - text_size.y - offset), text);

	img.save("logo.bmp");
}

//-----------------------------------------------------------------------------
void draw_pythagoras_tree(Image& img, const space2& space) {
	if (space.i.length() < 2)
		return;

	vec2 a(0, 0), b(0, 1), c(1, 1), d(1, 0);

	img.draw_line(space.from(a), space.from(b));
	img.draw_line(space.from(b), space.from(c));
	img.draw_line(space.from(c), space.from(d));
	img.draw_line(space.from(d), space.from(a));

	double alpha = deg2rad(45);
	vec2 triangle_vertex = rotate(b + vec2(cos(alpha), 0), b, alpha);
	//vec2 triangle_vertex = b + vec2(0.5, 0.5);

	draw_pythagoras_tree(img, makeLine2(space.from(b), space.from(triangle_vertex)));
	draw_pythagoras_tree(img, makeLine2(space.from(triangle_vertex), space.from(c)));
}

//-----------------------------------------------------------------------------
int main() {
	draw_coords();
	/*Image img(vec2(700, 420));
	img.set_pen(1, Black);
	space2 space = makeLine2(vec2(400, 410), vec2(300, 410));
	space.pos += vec2(0.5, 0.5);

	vec2 a(0, 0), b(0, 1), c(1, 1), d(1, 0);

	img.set_pen(0.5, setAlpha(Gray, 64));
	img.draw_line(space.from(a), space.from(b));
	img.draw_line(space.from(b), space.from(c));
	img.draw_line(space.from(c), space.from(d));
	img.draw_line(space.from(d), space.from(a));

	vec2 triangle_vertex = rotate(b + vec2(sqrt(2.0)/2.0, 0), b, deg2rad(45));

	space2 space1 = makeLine2(space.from(b), space.from(triangle_vertex));
	space2 space_2 = makeLine2(space.from(triangle_vertex), space.from(c));
	draw_pythagoras_tree(img, space1);

	img.set_pen(1, Black);
	draw_pythagoras_tree(img, space_2);

	img.set_pen(2, Black);
	img.draw_crd(space_2);
	img.save("pythagoras_tree_1.bmp");

	img.img.clear(White);
	img.set_pen(1, Black);
	draw_pythagoras_tree(img, space);
	img.set_pen(2, Black);
	img.draw_crd(space);
	img.save("pythagoras_tree.bmp");*/
}