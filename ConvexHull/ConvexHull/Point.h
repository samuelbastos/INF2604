#pragma once
#include <math.h>
#include "random.hpp"

using Random = effolkronium::random_static;

class Point
{
public:
	Point(int iindex, float xx, float yy);
	~Point();

	inline void set(float xx, float yy) { x = xx; y = yy; }
	inline float getX() const { return x; }
	inline float getY() const { return y; }
	inline int getIndex() const { return index; }

	inline static float distance(Point p1, Point p2)
	{
		float xx = (p1.x - p2.x) * (p1.x - p2.x);
		float yy = (p1.y - p2.y) * (p1.y - p2.y);
		return sqrtf((float)xx + (float)yy);
	}

	inline static std::vector<Point> generateCloud(int qtd)
	{
		std::vector<Point> points;
		for (int i = 0; i < qtd; i++)
		{
			auto x = Random::get(0.f, 1000.f);
			auto y = Random::get(0.f, 1000.f);
			points.push_back(Point(i, x, y));
		}

		return points;
	}

	// isLeft(): test if a point is Left|On|Right of an infinite line.
	//    Input:  three points P0, P1, and P2
	//    Return: >0 for P2 left of the line through P0 and P1
	//            =0 for P2 on the line
	//            <0 for P2 right of the lin
	inline static int isLeft(Point p0, Point p1, Point p2)
	{
		return (p1.x - p0.x)*(p2.y - p0.y) - (p2.x - p0.x)*(p1.y - p0.y);
	}

private:

	int index;
	float x;
	float y;
};

inline bool operator<(const Point& p1, const Point& p2)
{
	return p1.getX() < p2.getX();
}
