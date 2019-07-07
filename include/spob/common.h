#pragma once

#include <vector>

namespace spob {

	#define _SPOB_PI 3.141592653589793238462643383279502884197169399375105820974944592307816406286208998628034825342117067982148086513282306647093844609550582231725359408128481117450284102701938521105559644622948954930381964428810975 

	#define _SPOB_IS_NEAR_PRECISION 0.0001

	/** Тип пересечения двух некоторых объектов. */
	enum Inter
	{
		INTER_ABSENT, // Объекты не имеют общих точек
		INTER_OVERLAPS, // Объекты пересекаются по некоторому множеству точек
		INTER_COINCIDES // Один объект находится внутри другого
	};

	//-------------------------------------------------------------------------
	inline double deg2rad(double a) { return a / 180.0 * _SPOB_PI; }
	inline double rad2deg(double r) { return r * 180.0 / _SPOB_PI; }

	//-------------------------------------------------------------------------
	template<class A, class B>
	bool isNear(const A& a, const B& b) {
		return distance(a, b) < _SPOB_IS_NEAR_PRECISION;
	}

	//-------------------------------------------------------------------------
	// Функции для массового преобразования
	#define mas_func(a) \
	template<class C, class T> \
	auto a##Mas(const C& c, const std::vector<T>& mas) { \
		std::vector<decltype(c.a(mas.front()))> result; \
		result.reserve(mas.size()); \
		for (const auto& i : mas) \
			result.push_back(c.a(i)); \
		return result; \
	}

	mas_func(to)
	mas_func(toDir)
	mas_func(from)
	mas_func(fromDir)

};