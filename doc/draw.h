#include <functional>
#include <vector>
#include <memory>

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

space2 calcViewPort(const vec2& min, const vec2& max);
space2 increaseViewportBorderByMinAxis(const space2& viewport, double percent);
std::pair<crd2, vec2> calcViewPortAndSize(vec2 min, vec2 max, int maxSize);


//-----------------------------------------------------------------------------
class Image
{
public:
	Image(const vec2& size, const space2& screen_tr, int layers = 1);

	void draw_line(const vec2& a, const vec2& b, int layer = 0);
	void draw_dashed_line(const vec2& a, const vec2& b, double size, double space, int layer = 0);
	void draw_inf_line(const line2& line, int layer = 0);
	void draw_arrow(const vec2& a, const vec2& b, double angle, double size, int layer = 0);
	void draw_crd(const crd2& crd, int layer = 0);
	void draw_line2(const line2& line, int layer = 0);
	void draw_grid(const space2& space, int layer = 0);
	void draw_polygon(const std::vector<vec2>& polygon, int layer = 0);
	void draw_polyfill(const std::vector<vec2>& polygon, Color clr, int layer = 0);
	void draw_circle(const vec2& pos, double r, Color clr, int layer = 0);

	void draw_project(const vec2& pos, const line2& line, int layer = 0);
	void draw_intersect(const line2& l1, const line2& l2, int layer = 0);

	void set_pen(double thick, Color clr, int layer = 0);

	void set_alpha(int alpha);

	void setViewPort(const space2& view);

	void clear(Color clr);

	void combine_layers(void);
	void save(std::string file);

	double fromThick(double a);

	std::vector<shared_ptr<ImageDrawing_aa>> imgs;
	rect2 rect;
	double thick;
	int alpha;
	Color clr;
	space2 screen_tr;
};