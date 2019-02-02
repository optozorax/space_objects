class segment2 : public line2
{
public:
	double min, max;

	bool isInside(const double& t) const {return (t >= min) && (t <= max);}
	bool isInside(const vec2& o) const {return isInside(to(o));}
};

Inter intersect(const line2& a, const segment2& b,
				double& t1, double& t2) {
	auto inter = intersect(a, line2(b), t1, t2);
	if (inter != INTER_OVERLAPS) {
		return inter;
	} else {
		if (b.isInside(t2))
			return returned;
		else
			return INTER_ABSENT;
	}
}

Inter intersect(const segment2& a, const segment2& b,
				double& t1, double& t2) {
	auto inter = intersect(line2(a), line2(b), t1, t2);
	if (inter == INTER_ABSENT)
		return inter;
	else if (inter == INTER_OVERLAPS) {
		if (a.isInside(t1) && b.isInside(t2))
			return inter;
		else
			return INTER_ABSENT;
	} else { // inter == INTER_COINCIDES
		vec2 amin = a.from(a.min), amax = a.from(a.max);
		vec2 bmin = b.from(b.min), bmax = b.from(b.max);
		if ((isNear(amin, bmin) && isNear(amax, bmax)) ||
			(isNear(amin, bmax) && isNear(amax, bmin)) ||
			a.isInside(bmin) || a.isInside(bmax)
			)
		if (a.isInside(a.to(b.from(b.min))) || a.isInside(a.to(b.from(b.max))))
			return inter;
		else
			return INTER_ABSENT;
	}
}

Inter intersect(const segment2& a, const segment2& b, 
				segment2& result) {
	// works only when INTER_COINCIDES
	double t1, t2;
	auto inter = intersect(a, b, t1, t2);
	if (inter == INTER_COINCIDES) {

		return inter;
	} else 
		return inter;
}