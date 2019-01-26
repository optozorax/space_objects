#include "find_borders.h"

namespace spob 
{

//-----------------------------------------------------------------------------
FindBorders::FindBorders(int maxSize, int border) : m_maxSize(maxSize), m_border(border), space2(getStandardCrd2()) {
	double inf = std::numeric_limits<double>::infinity();
	min = vec2(inf, inf);
	max = vec2(-inf, -inf);
}

//-----------------------------------------------------------------------------
void FindBorders::process(vec2 point) {
	if (point.x > max.x) max.x = point.x;
	if (point.y > max.y) max.y = point.y;
	if (point.x < min.x) min.x = point.x;
	if (point.y < min.y) min.y = point.y;
}

//-----------------------------------------------------------------------------
void FindBorders::process(const std::vector<vec2>& points) {
	for (const auto& i : points)
		process(i);
}

//-----------------------------------------------------------------------------
void FindBorders::finish(void) {
	if (max.x - min.x > max.y - min.y) {
		m_size.x = m_maxSize;
		m_size.y = (max.y - min.y)/(max.x - min.x) * m_size.x;

		m_scale = m_maxSize/(max.x - min.x);
	} else {
		m_size.y = m_maxSize;
		m_size.x = (max.x - min.x)/(max.y - min.y) * m_size.y;

		m_scale = m_maxSize/(max.y - min.y);
	}
	m_offset = -min;
	m_size += vec2(m_border) * 2;

	i /= m_scale;
	j /= m_scale;
	pos = min - m_border * (i + j);

	auto crd = spob::invert(*this);
	i = crd.i;
	j = crd.j;
	pos = crd.pos;
}

//-----------------------------------------------------------------------------
void FindBorders::invertY(void) {
	space2 standard_corner = getStandardCrd2();
	standard_corner.pos = vec2(0, m_size.y);
	standard_corner.j.negate();
	auto crd = standard_corner.to(*this);
	i = crd.i;
	j = crd.j;
	pos = crd.pos;
}

//-----------------------------------------------------------------------------
vec2 FindBorders::getCalculatedSize(void) const {
	return m_size;
}

//-----------------------------------------------------------------------------
void FindBorders::offsetToEqualSize(void) {
	pos += (vec2(m_maxSize + m_border) - m_size)/2.0;
	m_size = vec2(m_maxSize + m_border);
}

//-----------------------------------------------------------------------------
bool FindBorders::isInside(vec2 pos) const {
	return (pos.x > min.x) && (pos.y > min.y) && (pos.x < max.x) && (pos.y < max.y);
}

};