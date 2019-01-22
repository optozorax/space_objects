#pragma once

#include <vector>

#include <spob/vector2.h>
#include <spob/vector3.h>
#include <spob/coordinates2.h>
#include <spob/coordinates3.h>
#include <spob/intersection.h>

//-----------------------------------------------------------------------------
// Функции для массового преобразования
#define mas_func(a) \
template<class C, class T> \
auto a##Mas(const C& c, const std::vector<T>& mas) { \
	std::vector<decltype(c.a##(mas.front()))> result; \
	result.reserve(mas.size()); \
	for (const auto& i : mas) \
		result.push_back(c.a##(i)); \
	return result; \
}

mas_func(to)
mas_func(toDir)
mas_func(from)
mas_func(fromDir)