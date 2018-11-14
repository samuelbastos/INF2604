#pragma once
#include <stdio.h>
#include <vector>
#include <utility>
#include <iostream>
#include <cstdio>
#include "random.hpp"
#include "Point.h"

using Random = effolkronium::random_static;

class Heuristic
{
public:
	Heuristic(std::vector<Point> points);
	~Heuristic();

	void execute();
	inline std::pair<Point, float> getMinCenterRadius() { return _minCenterRadius; }

private:

	std::vector<Point> _points;
	std::pair<Point, float> _minCenterRadius;
};

