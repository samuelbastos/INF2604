#pragma once

class Point
{
public:
	inline Point(double xx, double yy) { x = xx; y = yy; }
	inline Point() {}
	inline double getX() { return x; }
	inline double getY() { return y; }
	inline void setX(double v) { x = v; }
	inline void setY(double v) { y = v; }

private:
	double x;
	double y;
};