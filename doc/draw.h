#include <functional>
#include <vector>

#include <spob/spob.h>
#include <twg/image/image_drawing.h>

#undef max
#undef min

using namespace spob;
using namespace twg;
using namespace std;

struct rect2 { vec2 a, b; };

//-----------------------------------------------------------------------------
Inter intersect(const rect2& rect, const line2& line,
			   double& t1, double& t2);
Inter intersect(const rect2& rect, const line2& line,
			   vec2& a, vec2& b);

vector<vec2> calcRegularPolygon(int n, vec2 center, double r, double fi);

vector<vec2> placePolyOnEdge(const vector<vec2>& poly, int edge);

vector<vec2> sumPolygons(const vector<vec2>& poly1, const vector<vec2>& poly2, int edge1, int edge2);

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
	void draw_polygon(const std::vector<vec2>& polygon);
	void draw_circle(const vec2& pos, double r, Color clr);

	void draw_project(const vec2& pos, const line2& line);
	void draw_intersect(const line2& l1, const line2& l2);

	void set_pen(double thick, Color clr);

	void save(std::string file);

	void setViewPort(const space2& view);

	double fromThick(double a);

	ImageDrawing_aa img;
	rect2 rect;
	double thick;
	Color clr;
	space2 screen_tr;
};