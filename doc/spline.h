#pragma once

// Чтобы не было проблем с деструктором в union'е xyz, rgb.
#define GLM_FORCE_XYZW_ONLY

#include <glm/glm.hpp>
#include <vector>
#include <cassert>

#include <Eigen/Dense>

using Eigen::MatrixXd;
using Eigen::VectorXd;

//-----------------------------------------------------------------------------
/** Класс линии в СЛАУ. Её можно складывать с другими линиями в СЛАУ, а так же умножать на числа. В итоге можно проводить вычисление матрицы СЛАУ без её предвательного расчета, просто написав расчет в настолько общем виде, насколько это треубется. 

По большей части содержит в себе коэффициенты при неизвестных переменных, количество которых задается в size. Так же одно число специально выделено на правую часть - число без неизвестных, число, которому должны быть равны вся сумма неизвестных помноженных на коэффициенты.

Внимание! То, что возвращает метод get() надо брать с обратным знаком, когда решаете СЛАУ.

*/
class slae_line
{
public:
	slae_line(int size, double value = 0);

	void set(double value); // Ставит значение в правую часть
	void set(int i, double value); /// Ставит значение в строку СЛАУ

	double get(void) const; /// Возвращает значение правую часть
	double get(int i) const; /// Возвращает значение из строки СЛАУ

	bool is_number(void) const; /// Если в данной строке СЛАУ отсутствуют неизвестные, то её можно считать за число, и на неё можно умножать без боязни получить квадраты неизвестных

	int size(void) const; /// Возвращает число неизвестных
private:
	std::vector<double> values;
	int non_zero_numbers;
};

// Операции над строкой СЛАУ. Доступны основные арифметические операции.
slae_line operator+(const slae_line& a, const slae_line& b);
slae_line operator-(const slae_line& a, const slae_line& b);
slae_line operator*(const slae_line& a, const slae_line& b);
slae_line operator/(const slae_line& a, const slae_line& b);

//-----------------------------------------------------------------------------
// Создание своих типов матриц и векторов на основе строки СЛАУ, чтобы максимально высокоуровнево записывать условия.
typedef glm::vec<3, slae_line, glm::defaultp> vec3c;
typedef glm::mat<3, 3, slae_line, glm::defaultp> mat3c;

vec3c to_slae_line(const glm::vec3& v, int size);
mat3c to_slae_line(const glm::mat3& p, int size);

//-----------------------------------------------------------------------------
/** Класс решателя СЛАУ, который получает строки slae_line. */
class slae_solver
{
public:
	slae_solver(int size);

	void process(const slae_line& line);
	void process(const vec3c& vec); // vec.z не учитывается!!!

	std::vector<double> solve(void);
private:
	int i, size;
	MatrixXd A;
	VectorXd b;
};

//-----------------------------------------------------------------------------
/** Класс сплайна на основе полиномов n степени для интерполяции двух систем координат. */
class polynom_spline2_crd_interpolation
{
public:
	polynom_spline2_crd_interpolation(int n);

	glm::vec3 value(double t, int d = 0) const; /// Получить значение полинома в точке t. Так же если задана величина производной, можно получить значение производной степени d в этой точке.

	void mul(const glm::mat3& p); /// Применить преобразование координат p к этому полиному. После преобразования значения в value будут выдаваться такие, как будто к ним применили это преобразование координат.

	void solve_coefs(const glm::mat3& p, glm::vec3 at_zero); /// Найти такие коэффициенты полинома, чтобы в точке t=0 он был равен at_zero, а в t=1 был равен p*at_zero, при этом чтобы такое же равенство сохранилось для всех производных вплоть до n-1 порядка
private:
	std::vector<double> calcDCoefs(int d) const; /// Находит какие коэффициенты стоят перед каждым элементом полинома при взятии производной d-ой степени.
	vec3c fvalue(double t) const; /// Возвращает значение полинома в виде строки СЛАУ, где записаны строки всех неизвестных.
	vec3c unknown_value(double t, int d = 0) const; /// Возвращает значение полинома в точке t, в виде строки СЛАУ, то есть там записаны коэффициенты с учетом того, что они ещё неизвестны. Так же можно получить производную, задав d отличное от нуля.

	int n;
	std::vector<double> coefs;
};