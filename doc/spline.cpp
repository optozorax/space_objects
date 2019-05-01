#include "spline.h"

//-----------------------------------------------------------------------------
slae_line::slae_line(int size, double value) : values(size+1, 0), non_zero_numbers(0) {
	values.front() = value;
}

//-----------------------------------------------------------------------------
void slae_line::set(double value) {
	values[0] = value;
}

//-----------------------------------------------------------------------------
void slae_line::set(int i, double value) {
	assert(i+1 < values.size());
	if (value == 0) {
		if (values[i+1] != 0) non_zero_numbers--;
	} else {
		if (values[i+1] == 0) non_zero_numbers++;
	}
	values[i+1] = value;
}

//-----------------------------------------------------------------------------
double slae_line::get(void) const {
	return values[0];
}

//-----------------------------------------------------------------------------
double slae_line::get(int i) const {
	assert(i+1 < values.size());
	return values[i+1];
}

//-----------------------------------------------------------------------------
bool slae_line::is_number(void) const {
	return non_zero_numbers == 0;
}

//-----------------------------------------------------------------------------
int slae_line::size(void) const {
	return values.size() - 1;
}

//-----------------------------------------------------------------------------
slae_line operator+(const slae_line& a, const slae_line& b) { 
	assert(a.size() == b.size());
	slae_line result(a.size());
	result.set(a.get() + b.get());
	for (int i = 0; i < a.size(); i++) {
		result.set(i, a.get(i) + b.get(i));
	}
	return result;
}

//-----------------------------------------------------------------------------
slae_line operator-(const slae_line& a, const slae_line& b) {
	assert(a.size() == b.size());
	slae_line result(a.size());
	result.set(a.get() - b.get());
	for (int i = 0; i < a.size(); i++) {
		result.set(i, a.get(i) - b.get(i));
	}
	return result;
}

//-----------------------------------------------------------------------------
slae_line operator*(const slae_line& a, const slae_line& b) {
	assert((a.size() == b.size()) && (a.is_number() || b.is_number()));
	slae_line result(a.size());
	double mul = 0;
	if (a.is_number() && b.is_number()) {
		result.set(a.get() * b.get());
		return result;
	} else if (a.is_number()) {
		result = slae_line(b);
		mul = a.get();
	} else if (b.is_number()) {
		result = slae_line(a);
		mul = b.get();
	}

	result.set(result.get() * mul);
	for (int i = 0; i < result.size(); i++)
		result.set(i, result.get(i) * mul);

	return result;
}

//-----------------------------------------------------------------------------
slae_line operator/(const slae_line& a, const slae_line& b) {
	assert((a.size() == b.size()) && (a.is_number() || b.is_number()));
	slae_line result(a.size());
	double mul = 0;
	if (a.is_number() && b.is_number()) {
		result.set(a.get() * b.get());
		return result;
	} else if (a.is_number()) {
		result = slae_line(b);
		mul = 1.0/a.get();
	} else if (b.is_number()) {
		result = slae_line(a);
		mul = b.get();
	}

	result.set(result.get() / mul);
	for (int i = 0; i < result.size(); i++)
		result.set(i, result.get(i) / mul);

	return result;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
vec3c to_slae_line(const glm::vec3& v, int size) {
	return vec3c(slae_line(size, v.x), slae_line(size, v.y), slae_line(size, v.z));
}

//-----------------------------------------------------------------------------
mat3c to_slae_line(const glm::mat3& p, int size) {
	return mat3c(to_slae_line(p[0], size), to_slae_line(p[1], size), to_slae_line(p[2], size));
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
slae_solver::slae_solver(int size) : A(size, size), b(size), i(0), size(size) {}

//-----------------------------------------------------------------------------
void slae_solver::process(const slae_line& line) {
	assert(size == line.size() && i < size);
	b(i) = -line.get();
	for (int j = 0; j < line.size(); j++)
		A(i, j) = line.get(j);

	i++;
}

//-----------------------------------------------------------------------------
void slae_solver::process(const vec3c& vec) {
	assert(vec.z.is_number());
	process(vec.x);
	process(vec.y);
} // vec.z не учитывается!!!

//-----------------------------------------------------------------------------
std::vector<double> slae_solver::solve(void) {
	VectorXd x = A.colPivHouseholderQr().solve(b);
	std::vector<double> result(size);
	for (int j = 0; j < size; j++) result[j] = x(j);
	double precision = (A*x-b).norm();
	assert(precision/(2.0*size) < 0.01);
	return result;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
polynom_spline2_crd_interpolation::polynom_spline2_crd_interpolation(int n) : n(n), coefs(2*n, 0) {}

//-----------------------------------------------------------------------------
glm::vec3 polynom_spline2_crd_interpolation::value(double t, int d) const {
	auto dcoefs = calcDCoefs(d);

	glm::vec3 res(0, 0, d == 0);
	double tmul = 1;
	for (int i = d; i < n; ++i) {
		int pos = n-i-1;
		res.x += dcoefs[pos] * coefs[pos] * tmul;
		res.y += dcoefs[pos] * coefs[n + pos] * tmul;
		tmul *= t;
	}
	return res;
}

//-----------------------------------------------------------------------------
void polynom_spline2_crd_interpolation::mul(const glm::mat3& p) {
	auto pc = to_slae_line(p, 2*n);
	vec3c f(slae_line(2*n, 0), slae_line(2*n, 0), slae_line(2*n, 0));
	f.z.set(n-1, 1);
	for (int i = 0; i < n; ++i) {
		f.x.set(i, coefs[i]);
		f.y.set(i, coefs[n+i]);
	}
	f = pc * f;
	std::vector<double> newcoefs(2*n);
	for (int i = 0; i < n; i++) {
		newcoefs[i] = f.x.get(i);
		newcoefs[n+i] = f.y.get(i);
	}
	coefs = newcoefs;
}

//-----------------------------------------------------------------------------
void polynom_spline2_crd_interpolation::solve_coefs(const glm::mat3& p, glm::vec3 at_zero) {
	auto pc = to_slae_line(p, 2*n);
	auto at_zero_c = to_slae_line(at_zero, 2*n);

	slae_solver solver(2*n);
	solver.process(unknown_value(0) - at_zero_c);
	solver.process(unknown_value(1) - pc*unknown_value(0));
	for (int i = 1; i < n-1; ++i)
		solver.process(unknown_value(1, i) - pc*unknown_value(0, i));

	coefs = solver.solve();
}

//-----------------------------------------------------------------------------
std::vector<double> polynom_spline2_crd_interpolation::calcDCoefs(int d) const {
	// Строим коэффициенты производной
	std::vector<double> dcoefs(n+1, 1);
	dcoefs.back() = 0;
	for (int i = 0; i < d; ++i) {
		double last = dcoefs.back();
		for (int j = n; j > 0; --j) {
			double temp = dcoefs[j-1];
			int number = n+1-j;
			dcoefs[j-1] = last * number;
			last = temp;
		}
	}

	return dcoefs;
}

//-----------------------------------------------------------------------------
vec3c polynom_spline2_crd_interpolation::unknown_value(double t, int d) const {
	auto dcoefs = calcDCoefs(d);

	// Считаем сам вектор с учетом производной
	vec3c result(slae_line(2*n, 0), slae_line(2*n, 0), slae_line(2*n, 0));
	result.z.set(d == 0);
	double tmul = 1;
	for (int i = d; i < n; ++i) {
		int pos = n-i-1;
		result.x.set(pos, dcoefs[pos] * tmul);
		result.y.set(n + pos, dcoefs[pos] * tmul);
		tmul *= t;
	}

	return result;
}