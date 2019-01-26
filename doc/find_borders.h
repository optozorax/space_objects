#pragma once

#include <vector>
#include <spob/spob.h>

namespace spob
{

	//-------------------------------------------------------------------------
	/** Класс для нахождения ограничивающего прямоугольника для точек, а так же, позволяющий далее эти точки нарисовать на изображении с максимальным размером maxSize и с заданным отступом от краев изображения border. */
	class FindBorders : public space2
	{
	public:
		FindBorders(int maxSize, int border);

		/** Сюда передается точка для обработки. */
		void process(vec2 point);

		/** Сюда передается массив точек для обработки. */
		void process(const std::vector<vec2>& points);

		/** После обработки всех точек обязательно надо завершить обработку этой функцией, потому что там высчитываются необходимые значения. */
		void finish(void);

		/** Возвращает итоговый размер изображения. */
		vec2 getCalculatedSize(void) const;

		void offsetToEqualSize(void);

		/** Определяет, находится ли pos внутри рассчитанных границ. */
		bool isInside(vec2 pos) const;

		/** Если раньше считалось, что пиксели начинаются с левого верхнего угла, то после этой функции они будут считаться с левого нижнего угла. */
		void invertY(void);

		/** Прямой доступ к переменным min и max. */
		vec2 min, max;
	private:
		int m_maxSize, m_border;
		double m_scale;
		vec2 m_offset;
		vec2 m_size;
	};

};