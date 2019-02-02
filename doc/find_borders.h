#pragma once

#include <vector>
#include <spob/spob.h>

namespace spob
{

	//-------------------------------------------------------------------------
	/** Класс для нахождения ограничивающего прямоугольника для точек. */
	class BorderFinder2
	{
	public:
		BorderFinder();

		void process(vec2 point);
		void process(const std::vector<vec2>& points);

		bool isInside(vec2 pos) const;

		vec2 min, max;
	};

	//-------------------------------------------------------------------------
	/** Класс для преобразования координат из тех, что даны, к координатам изображения. Обладатем теми же методами и свойствами, что и классы, от которых он наследуется. */
	class BorderFinderForImage : public space2, public BorderFinder2
	{
	public:
		BorderFinderForImage(int maxSize, int border, bool isDimensionsMustBeEqual = false, bool isYMustBeFlipped = true);
		
		/** После обработки всех точек обязательно надо завершить обработку этой функцией, потому что там высчитываются необходимые значения. */
		void finish(void);

		/** Возвращает итоговый размер изображения. */
		vec2 getCalculatedSize(void) const;

		/** 
			Метод to() преобразует координаты из передаваемых в process к координатам изображения.
			Метод from() преобразует координаты из координат изображения к координатам, которые передавались в process.
		*/
	private:
		int m_maxSize, m_border;
		double m_scale;
		vec2 m_offset;
		vec2 m_size;
		bool m_isDimensionsMustBeEqual;
		bool m_isYMustBeFlipped;
	};

};