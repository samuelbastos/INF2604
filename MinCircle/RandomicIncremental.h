#pragma once
#include <stdio.h>
#include <vector>
#include <utility>
#include <iostream>
#include <cstdio>
#include "random.hpp"
#include "Point.h"

using Random = effolkronium::random_static;

class RandomicIncremental
{
public:
	RandomicIncremental(std::vector<Point> points);
	~RandomicIncremental();

	void execute();
	inline std::vector<Point> getMinCircle() { return _minCircle; }

private:

	bool contains(std::vector<Point> circle, Point p);
	void randomSwap();
	std::vector<Point> minCircle();
	std::vector<Point> minCircleWithPoint(std::vector<Point> points, Point q);
	std::vector<Point> minCircleWith2Points(std::vector<Point> points, Point q1, Point q2);

	std::vector<Point> _points;
	std::vector<Point> _minCircle;
};

