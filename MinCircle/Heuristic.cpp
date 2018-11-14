#include "Heuristic.h"



Heuristic::Heuristic(std::vector<Point> points) :
	_points(points)
{
}

Heuristic::~Heuristic()
{
}

void Heuristic::execute()
{
	time_t init;
	time_t end;
	double seconds;
	time(&init);

	/** Achar pares {pxmin, pxmax} e {pymin, pymax} */
	Point pxmin, pxmax, pymin, pymax;
	double xmin = 2000, xmax = -1, ymin = 2000, ymax = -1;
	for (auto point : _points)
	{
		if (point.getX() < xmin) {
			pxmin = point;
			xmin = point.getX();
		}
		if (point.getX() > xmax) {
			pxmax = point;
			xmax = point.getX();
		}
		if (point.getY() < ymin) {
			pymin = point;
			ymin = point.getX();
		}
		if (point.getY() > ymax) {
			pymax = point;
			ymax = point.getX();
		}
	}

	/** Calcula qual dos pares é o que possui maior distancia entre os pontos */
	double xx = (pxmin.getX() - pxmax.getX()) * (pxmin.getX() - pxmax.getX());
	double yy = (pxmin.getY() - pxmax.getY()) * (pxmin.getY() - pxmax.getY());
	double xDistance = std::sqrt(xx + yy);

	xx = (pymin.getX() - pymax.getX()) * (pymin.getX() - pymax.getX());
	yy = (pymin.getY() - pymax.getY()) * (pymin.getY() - pymax.getY());
	double yDistance = std::sqrt(xx + yy);

	/** Salva a maior distancia em d e qual o par mais distante em mdpair */
	double d = (xDistance > yDistance) ? xDistance : yDistance;
	std::pair<Point, Point> mdpair = (xDistance > yDistance) ?
		std::pair<Point, Point>(pxmin, pxmax) : std::pair<Point, Point>(pymin, pymax);

	/* Calcula o centro e o raio do circulo */
	double cx = (mdpair.first.getX() + mdpair.second.getX()) / 2;
	double cy = (mdpair.first.getY() + mdpair.second.getY()) / 2;
	double r = d / 2;
	Point c = Point(cx, cy);

	for (auto point : _points)
	{
		double xx = (c.getX() - point.getX()) * (c.getX() - point.getX());
		double yy = (c.getY() - point.getY()) * (c.getY() - point.getY());
		double d = std::sqrt(xx + yy);

		if (d > r)
		{
			double xnorm = (point.getX() - c.getX()) / d;
			double ynorm = (point.getY() - c.getY()) / d;
			double constantCenter = (d - r) / 2;
			double constantR = (d + r) / 2;
			xnorm *= constantCenter;
			ynorm *= constantCenter;
			double cx = c.getX() + xnorm;
			double cy = c.getY() + ynorm;
			c = Point(cx, cy);
			r = constantR;
		}
	}
	
	_minCenterRadius = std::pair<Point, double>(c, r);

	time(&end);
	seconds = difftime(end, init);
	std::cout << seconds / 60 << std::endl;
}