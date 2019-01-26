#define _USE_MATH_DEFINES

#include <cmath>
#include <spob/spob.h>
#include <twg/image/image_drawing.h>
using namespace twg;

using namespace spob;
ImageDrawing_aa img(Point_i(400, 400));
void draw_line(vec2 a, vec2 b) { // Некая сторонняя функция, которая рисует линию из точки a в точку b
	img.drawLine(a, b);
}

void draw_3D_cube(const space2& logic_to_img) {
	space3 cam = getStandardCrd3();

	// Делаем так, чтобы камера смотрела осью k в точку (0, 0, 0)
	cam.i.negate();
	std::swap(cam.i, cam.k);
	std::swap(cam.i, cam.j);

	// Задаем положение камеры
	cam.pos = vec3(5, 0, 0);

	line3 vertical(vec3(0, 0, 1), vec3(0));
	line3 horizontal(vec3(0, 1, 0), vec3(0));
	double alpha = deg2rad(30);
	double beta = deg2rad(30);

	// Вращаем её сначала относительно горизонтальной оси, то есть поднимаем её немного вверх. Направление оси k в точку (0, 0, 0) по прежнему остается.
	cam = rotate(cam, horizontal, -beta);
	// Затем вращаем её относительно вертикальной оси, то есть поворачиваем её направо. Направление оси k так же сохраняется.
	cam = rotate(cam, vertical, alpha);

	// Фокусное расстояние
	double focus = 0.1;

	// Получаем положение точки фокуса
	vec3 focus_point = cam.from(vec3(0, 0, -focus)); // Берем точку фокуса отрицательной, чтобы она находилась по обратную сторону от объкта, на который направлена камера, чтобы изображение было не перевернуто, да и чтобы при приближении камеры к объекту на расстояние фокуса, он не искажался слишком сильно.

	// Точки вершин куба
	std::vector<vec3> cube_points = {
		vec3(-1, -1, -1), vec3(-1, -1, 1), vec3(-1, 1, 1), vec3(-1, 1, -1),
		vec3(1, -1, -1),  vec3(1, -1, 1),  vec3(1, 1, 1),  vec3(1, 1, -1),
	};	

	// Ребра куба, они задаются двумя номерами вершины.
	std::vector<std::pair<int, int>> cube_edges = {
		{0, 1}, {1, 2}, {2, 3}, {3, 0},
		{4, 5}, {5, 6}, {6, 7}, {7, 4},
		{0, 4}, {1, 5}, {2, 6}, {3, 7},
	};

	auto project = [&] (vec3 point) -> vec2 {
		// Проецирует point на плоскость камеры через фокус

		/** Прежде чем объяснять что тут происходит расскажу немного об устройстве камеры, глаза:
			1. Свет двигается по прямой. Из-за этого и говорят "лучи света".
			2. В глазу и камере стоит линза, которая фокусирует лучи света таким образом, чтобы они все проходили через одну точку - фокус. 
			3. Далее этот луч света падает на сетчатку. Остается только вычислить координаты падения луча света, чтобы нарисовать его на экране. Конечно, сетчатка глаза искривлена, но вот матрица камеры - нет. И мы моделируем именно камеру. Поэтому у нас луч тоже падает на плоскость.

			Такая система с точкой фокуса реализует перспективную проекцию. Если убрать из системы эту точку, и проецировать всё, просто опуская перпендикуляры, то получится ортогональная проекция.

			Ну и тут мы получаем прямую, проходящую через данную точку и точку фокуса. Далее находим пересечение этой точки и плоскости. Координаты на плоскости и будут искомыми координатами.
		*/
		line3 ray = makeLine3(focus_point, point);
		vec2 pos;
		double t;
		if (intersect(cam, ray, pos, t) == INTER_OVERLAPS)
			return pos;
		else
			return vec2(0);

		// Следующий код реализует ортогональную проекцию: 
		//vec3 pos = cam.to(point);
		//return vec2(pos.x, pos.y)/50;

		// Деление на 50 происходит потому что при проецировании через фокус изображение получается примерно в 50 раз меньше объекта. 50 == distance(cam.pos, vec3(0, 0, 0))/focus.
	};

	// Перебираем все ребра, проецируем их точки на плоскость камеры и рисуем линию ребра на экран
	for (auto& i : cube_edges) {
		vec3 a = cube_points[i.first];
		vec3 b = cube_points[i.second];

		// logic_to_img нужен чтобы преобразовать координаты из "логических" в координаты изображения. В данном примере тут просто имеется смещение и масштабирование
		draw_line(logic_to_img.from(project(a)), logic_to_img.from(project(b)));
	}
}

//int main() {
int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	img.clear(White);
	img.setPen(Pen(1, Black));

	double mx = 5000;
	space2 logic_to_img = getStandardCrd2();
	logic_to_img.i *= mx;
	logic_to_img.j *= mx;
	logic_to_img.j.negate();
	logic_to_img.pos = vec2(200, 200);
	draw_3D_cube(logic_to_img);
	
	saveToBmp(&img, L"3D_cube.bmp");
}