#include "draw.h"

//-----------------------------------------------------------------------------
void draw_logo(void) {
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

	//-------------------------------------------------------------------------
	// Сетка и координаты
	img.set_pen(1.5/80.0, setAlpha(Gray, 192));
	img.draw_grid(standard);
	img.set_pen(1/80.0, Black);
	draw_rect(standard);
	img.set_pen(2.5/80.0, Gray);
	img.draw_crd(standard);

	//-------------------------------------------------------------------------
	// Линия 1
	img.set_pen(2.5/80.0, Gray);
	line2 l1 = makeLine2(vec2(-3, 1), vec2(-2, 1.3));
	l1.orthonormalize();
	img.draw_line2(l1);

	// Проекция на линию 1
	img.set_pen(1/80.0, setAlpha(Black, 128));
	img.draw_project(vec2(-4, 2), l1);

	//-------------------------------------------------------------------------
	// Линия 2
	img.set_pen(2.5/80.0, Gray);
	line2 l2 = makeLine2(vec2(4, 0.2), vec2(3.5, 0.5));
	img.draw_line2(l2);

	// Проекция на линию 2
	img.set_pen(1/80.0, setAlpha(Black, 128));
	img.draw_project(vec2(1.5, 0.3), l2);

	//-------------------------------------------------------------------------
	// Пересечение линий
	img.set_pen(1.5/80.0, img.clr);
	img.draw_intersect(l1, l2);

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

	//-------------------------------------------------------------------------
	// Проекция точки на координаты
	vec2 p(-0.9, 0.7);
	img.draw_circle(p, 3/80.0, Black);
	img.set_pen(1/80.0, Black);
	img.draw_dashed_line(p, vec2(p.x, 0), 10/80.0, 3/80.0);
	img.draw_dashed_line(p, vec2(0, p.y), 10/80.0, 3/80.0);

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
	draw_logo();
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