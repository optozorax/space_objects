#include <iostream>

#include <spob/spob.h>
#include "draw.h"
#include <prtl_vis/scene_reader.h>
#include <fstream>
#include <vector>
#include <prtl/portal_space.h>
#include <prtl_vis/animation.h>

using namespace spob;
using namespace scene;

struct moving_crd2 {
	space2 crd;
	vec2 localSpeed;

	moving_crd2(space2 crd, vec2 localSpeed = vec2(0)) : crd(crd), localSpeed(localSpeed) {
	}

	vec2 getGlobalSpeed(void) const {
		return crd.fromDir(localSpeed);
	}

	void setGlobalSpeed(const vec2& globalSpeed) {
		localSpeed = crd.toDir(globalSpeed);
	}

	moving_crd2 to(const moving_crd2& a) const {
		moving_crd2 result(crd.to(a.crd));
		result.setGlobalSpeed(crd.toDir(a.getGlobalSpeed() - getGlobalSpeed()));
		return result;
	}

	moving_crd2 from(const moving_crd2& a) const {
		moving_crd2 result(crd.from(a.crd));
		result.setGlobalSpeed(getGlobalSpeed());
		return result;
	}

	moving_crd2 fromJNegative(const moving_crd2& a) const {
		auto crd_copy = crd;
		crd_copy.j.negate();
		moving_crd2 result(crd_copy.from(a.crd));
		result.setGlobalSpeed(crd_copy.fromDir(a.getGlobalSpeed()) + getGlobalSpeed());
		return result;
	}

	void move(double time) {
		crd.pos += getGlobalSpeed() * time;
	}
};

struct polygon {
	moving_crd2 m;
	double start, end;

	vec2 getStart(void) const {
		return line2(m.crd).from(start);
	}

	vec2 getEnd(void) const {
		return line2(m.crd).from(end);
	}
};

struct portal {
	moving_crd2 m1, m2;
	double start, end;
};

moving_crd2 teleport(const moving_crd2& input, const moving_crd2& output, const moving_crd2& object) {
	return output.fromJNegative(input.to(object));
}

class drawer {
public:
	drawer(vec2 size, crd2 viewport) : image(size, getStandardCrd2()) {
		gif.start(size, "a.gif");
		image.setViewPort(viewport);
		clear();

		scene.frames.push_back({});
		add_room();
	}

	void clear(void) {
		image.alpha = 255;
		image.clear(twg::White);
		image.set_pen(1.5 / 80.0, setAlpha(Gray, 192));
		image.draw_grid(getStandardCrd2(), 0);
	}

	void draw(const polygon& poly, const twg::Color& clr) {
		image.alpha = 128;
		image.set_pen(0.03, clr);
		auto center = (poly.getStart() + poly.getEnd()) / 2.0;
		//image.draw_arrow(center, center + poly.m.getGlobalSpeed(), spob::deg2rad(45), 0.035);
		//image.draw_arrow(center, center + poly.m.crd.j, spob::deg2rad(45), 0.035);
		//image.draw_arrow(center, center + poly.m.crd.i, spob::deg2rad(45), 0.035);
		image.draw_line(poly.getStart(), poly.getEnd());
		image.draw_circle(poly.getStart(), 0.04, clr);
		image.draw_circle(poly.getEnd(), 0.04, clr);
	}

	vec3 to(const vec2& a) const {
		return vec3(0, a.x, a.y);
	}

	crd3 to(const crd2& b) const {
		crd3 a;
		a.i = vec3(1, 0, 0);
		a.j = to(b.i);
		a.k = to(b.j);
		a.pos = to(b.pos);
		return a;
	}

	vec3 to(const twg::Color& clr) const {
		return vec3(getRed(clr) / 255.0, getGreen(clr) / 255.0, getBlue(clr) / 255.0);
	}

	void draw(const portal& portal, const twg::Color& clr) {
		draw({ portal.m1, portal.start, portal.end }, clr);
		draw({ portal.m2, portal.start, portal.end }, clr);

		crd3 a = to(portal.m1.crd);
		crd3 b = to(portal.m2.crd);
		b.k.negate();

		b.i *= 0.8;
		if (clr == Orange)
			b.i *= 0.8;

		if (clr == Dollar)
			b.i *= 0.8 * 0.8;
		
		if (clr == Yellow)
			b.i *= 0.8 * 0.8 * 0.8 * 0.8;

		if (clr == Pink) {
			b.pos.y += 0.0001;
			b.i *= 0.8 * 0.8 * 0.8 * 0.8 * 0.8 * 0.8 * 0.8;
		}

		b.i /= b.i.length();

		vector<vec2> poly;
		poly.emplace_back(-0.5, portal.start);
		poly.emplace_back(0.5, portal.start);
		poly.emplace_back(0.5, portal.end);
		poly.emplace_back(-0.5, portal.end);

		double h = 0.03;
		vector<vec2> sideBorder;
		sideBorder.emplace_back(-0.5 - h, portal.start);
		sideBorder.emplace_back(-0.5, portal.start);
		sideBorder.emplace_back(-0.5, portal.end);
		sideBorder.emplace_back(-0.5 - h, portal.end);

		vector<vec2> sideBorder2;
		sideBorder2.emplace_back(0.5 + h, portal.start);
		sideBorder2.emplace_back(0.5, portal.start);
		sideBorder2.emplace_back(0.5, portal.end);
		sideBorder2.emplace_back(0.5 + h, portal.end);

		scene.frames.back().portals.push_back({a, b, poly, to(clr), to(clr)});
		scene.frames.back().colored_polygons.push_back({ a, sideBorder, to(Bitcoin) });
		scene.frames.back().colored_polygons.push_back({ a, sideBorder2, to(Bitcoin) });
		scene.frames.back().colored_polygons.push_back({ b, sideBorder, to(Aqua) });
		scene.frames.back().colored_polygons.push_back({ b, sideBorder2, to(Aqua) });
	}

	void add_room(void) {
		using namespace prtl;
		PortalSpace sp;
		MaterialMap polyMm;
		addRoom(sp, polyMm, vec3(0), 10);
		sp2frame(sp, polyMm, polyMm, true, scene.frames.back(), "aoeuaoeuo", false);
	}

	void frame() {
		gif.process(*image.imgs[0], 3);
		static int i = 0;
		image.save("anim/" + to_string(i) + ".png");
		i++;

		vector<vec2> poly2;
		poly2.emplace_back(0, -0.1);
		poly2.emplace_back(0, 0.1);
		poly2.emplace_back(1, 0.1);
		poly2.emplace_back(1, -0.1);

		/*scene.frames.back().colored_polygons.push_back({ crd3(vec3(1, 0, 0), vec3(0, 1, 0), vec3(0, 0, 1), vec3(0)), poly2, vec3(1, 0, 0) });
		scene.frames.back().colored_polygons.push_back({ crd3(vec3(0, 1, 0), vec3(1, 0, 0), vec3(0, 0, 1), vec3(0)), poly2, vec3(0, 1, 0) });
		scene.frames.back().colored_polygons.push_back({ crd3(vec3(0, 0, 1), vec3(0, 1, 0), vec3(1, 0, 0), vec3(0)), poly2, vec3(0, 0, 1) });*/

		scene.frames.push_back({});
		add_room();
	}

	void save(const Scene& scene, string name) const {
		ofstream fout(name);
		fout << unparse(scene);
		fout.close();
	}

	void save() {
		gif.end();

		scene.cam_rotate_around = vec3(0, 0, 0);
		//scene.cam_spheric_pos = vec3(0, 1.32, 1.8);
		scene.cam_spheric_pos = vec3(2.06, 1.39, 1.41);
		
		save(scene, "portal_in_portal_full.json");
	}

	Image image;
	ImageGif gif;
	Scene scene;
};

using namespace std;

bool isSame1(const crd2& a, const crd2& b) {
	return distance(a.pos, b.pos) + distance(a.i, b.i) + distance(a.j, b.j) < 0.01;
}

int main53() {
	double s2 = sqrt(2.0) / 2.0;
	double s3a = sqrt(3.0) / 2.0;
	double s3b = 0.5;
	moving_crd2 a(crd2(vec2(0, 1), vec2(1, 0), vec2(0)), vec2(0));
	moving_crd2 b(crd2(vec2(s2, -s2), vec2(s2, s2), vec2(s2, 0)), vec2(0));
	//moving_crd2 b(crd2(vec2(s3a, -s3b), vec2(s3b, s3a), vec2(s2, 0)), vec2(0));
	b.setGlobalSpeed(vec2(-1, 0));

	auto v = calcViewPortAndSize(vec2(-2, -2), vec2(2, 2), 500);
	drawer d(v.second, v.first);

	double time = -1;
	double step = 1.0 / 20.0;

	a.move(time);
	b.move(time);
	for (; time < 0; time += step) {
		d.clear();
		d.draw({ a, b, -1, 1 }, twg::Blue);
		d.frame();

		a.move(step);
		b.move(step);
	}

	time = 0;

	auto c = teleport(a, b, b);
	for (; time < 1; time += step) {
		double p0 = -1;
		double p1 = -1 + time;
		double p2 = 1;

		d.clear();
		d.draw({ a, b, p1, p2 }, twg::Blue);
		d.draw({ a, c, p0, p1 }, twg::Orange);
		d.frame();

		a.move(step * s2);
		b.move(step * s2);
		c.move(step * s2);
	}

	time = 0;

	double orangeSpeed = fabs(c.getGlobalSpeed().x);
	auto c2 = teleport(a, c, b);
	double greenSpeed = fabs(c2.getGlobalSpeed().x);
	auto c4 = teleport(a, c2, c);
	auto c5 = teleport(a, c4, c2);

	double lastSize1 = 0, lastSize2 = 0;

	for (; time <= 1; time += step) {
		double p0 = -1;
		double p1 = -time * orangeSpeed * s2;
		double p2 = 0;
		double p3 = time * greenSpeed;
		double p4 = time;
		double p5 = 1;

		d.clear();
		d.draw({ a, b, p4, p5 }, twg::Blue);
		d.draw({ a, c, p0, p1 }, twg::Orange);
		d.draw({ a, c2, p3, p4 }, twg::Dollar);
		d.draw({ a, c4, p1, p2 }, twg::Yellow);

		cout << (p4 - p5) - lastSize1 << "," << (p3 - p4) - lastSize2 << endl;

		lastSize1 = p4 - p5;
		lastSize2 = p3 - p4;

		d.draw({ a, c5, p2, p3 }, twg::Pink);

		/*vector<moving_crd2> mas = {a, b};
		vector<moving_crd2> newMas;
		for (int count = 0; count < 4; count++) {
			cout << "count: " << count << endl;
			for (auto& i : newMas) mas.push_back(i);
			newMas.clear();
			for (int i = 0; i < mas.size(); i++) {
				cout << "i: " << i << endl;
				for (int j = 0; j < mas.size(); j++) {
					if (count == 3) 
						cout << "j: " << j << endl;
					for (int k = 0; k < mas.size(); k++) {
						if (newMas.size() > 500)
							goto to_new;
						auto result = teleport(mas[i], mas[j], mas[k]);
						if (isSame1(mas[i].crd, mas[j].crd)) goto end;
						for (auto& elem : mas) if (isSame1(result.crd, elem.crd)) goto end;
						for (auto& elem : newMas) if (isSame1(result.crd, elem.crd)) goto end;
						newMas.push_back(result);
					end:;
					}
				}
			}
		to_new:;
		}

		to_draw:

		std::cout << mas.size() << std::endl;

		for (auto& i : mas) {
			d.draw({ i, p2, p3 }, twg::Pink);
		}*/

		d.frame();

		a.move(step * s2);
		b.move(step * s2);
		c.move(step * s2);
		c2.move(step * s2);
		c4.move(step * s2);
	}

	d.save();
	system("glut.exe portal_in_portal_full.json");

	return 0;
}