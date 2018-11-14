#include "RandomicIncremental.h"
#include <time.h>

RandomicIncremental::RandomicIncremental(std::vector<Point> points) :
	_points(points)
{
}

RandomicIncremental::~RandomicIncremental()
{
}

void RandomicIncremental::execute()
{
	time_t init;
	time_t end;
	double seconds;
	time(&init);

	auto c = minCircle();

	time(&end);
	seconds = difftime(end, init);
	std::cout << seconds/60 << std::endl;

	_minCircle = c;
}

bool RandomicIncremental::contains(std::vector<Point> circle, Point p)
{
	if (circle.size() == 2)
	{
		double cx = (circle[0].getX() + circle[1].getX()) / 2;
		double cy = (circle[1].getY() + circle[1].getY()) / 2;
		
		double xx = (circle[0].getX() - circle[1].getX()) * (circle[0].getX() - circle[1].getX());
		double yy = (circle[0].getY() - circle[1].getY()) * (circle[0].getY() - circle[1].getY());
		double d = std::sqrt(xx + yy);
		double r = d / 2;

		Point c = Point(cx, cy);

		double pcxx = (p.getX() - c.getX()) *  (p.getX() - c.getX()); // xx de vector CP
		double pcyy = (p.getY() - c.getY()) *  (p.getY() - c.getY()); // yy de vector CP
		double pcd = std::sqrt(pcxx + pcyy); // distance c->p

		if (pcd <= r)
			return true;
		else
			return false;
	}
	if (circle.size() == 3)
	{
		Point center;
		double radius;

		double xx = (circle[0].getX() - circle[1].getX()) * (circle[0].getX() - circle[1].getX());
		double yy = (circle[0].getY() - circle[1].getY()) * (circle[0].getY() - circle[1].getY());
		double a = std::sqrt(xx + yy) / 2;

		xx = (circle[1].getX() - circle[2].getX()) * (circle[1].getX() - circle[2].getX());
		yy = (circle[1].getY() - circle[2].getY()) * (circle[1].getY() - circle[2].getY());
		double b = std::sqrt(xx + yy) / 2;

		xx = (circle[2].getX() - circle[0].getX()) * (circle[2].getX() - circle[0].getX());
		yy = (circle[2].getY() - circle[0].getY()) * (circle[2].getY() - circle[0].getY());
		double c = std::sqrt(xx + yy) / 2;

		bool restOne = a * a + b * b < c*c;
		bool restTwo = b * b + c * c < a*a;
		bool restThree = c * c + a * a < b*b;

		/* tem ângulo obtuso */
		// http://mathworld.wolfram.com/ObtuseTriangle.html
		if (restOne || restTwo || restThree)
		{
			if (a > b && a > c)
			{
				double x = (circle[0].getX() + circle[1].getX()) / 2;
				double y = (circle[0].getY() + circle[1].getY()) / 2;
				center = Point(x, y);
				double xx = (circle[0].getX() - circle[1].getX()) * (circle[0].getX() - circle[1].getX());
				double yy = (circle[0].getY() - circle[1].getY()) * (circle[0].getY() - circle[1].getY());
				radius = std::sqrt(xx + yy) / 2;
			}
			if (b > a && b > c)
			{
				double x = (circle[1].getX() + circle[2].getX()) / 2;
				double y = (circle[1].getY() + circle[2].getY()) / 2;
				center = Point(x, y);
				double xx = (circle[1].getX() - circle[2].getX()) * (circle[1].getX() - circle[2].getX());
				double yy = (circle[1].getY() - circle[2].getY()) * (circle[1].getY() - circle[2].getY());
				radius = std::sqrt(xx + yy) / 2;
			}
			if (c > a && c > b)
			{
				double x = (circle[2].getX() + circle[0].getX()) / 2;
				double y = (circle[2].getY() + circle[0].getY()) / 2;
				center = Point(x, y);
				double xx = (circle[2].getX() - circle[0].getX()) * (circle[2].getX() - circle[0].getX());
				double yy = (circle[2].getY() - circle[0].getY()) * (circle[2].getY() - circle[0].getY());
				radius = std::sqrt(xx + yy) / 2;
			}
		}
		/* sem angulo obtuso */
		else
		{
			double x = (circle[0].getX() + circle[1].getX() + circle[2].getX()) / 3;
			double y = (circle[0].getY() + circle[1].getY() + circle[2].getY()) / 3;
			center = Point(x, y);

			double xx = (center.getX() - circle[1].getX()) * (center.getX() - circle[1].getX());
			double yy = (center.getY() - circle[1].getY()) * (center.getY() - circle[1].getY());
			radius = std::sqrt(xx + yy) / 2;
		}

		double pcxx = (p.getX() - center.getX()) *  (p.getX() - center.getX()); // xx de vector CP
		double pcyy = (p.getY() - center.getY()) *  (p.getY() - center.getY()); // yy de vector CP
		double pcd = std::sqrt(pcxx + pcyy); // distance c->p

		if (pcd <= radius)
			return true;
		else
			return false;
	}

	return false;
}

void RandomicIncremental::randomSwap()
{
	// TODO;
}

std::vector<Point> RandomicIncremental::minCircle()
{
	/* Mudar pra permutação direito */
	Random::shuffle(_points);

	/* C2 */
	std::vector<Point> ci;
	ci.push_back(_points[0]); // p1
	ci.push_back(_points[1]); // p2

	/* Para i = 3,(...),n */
	for (unsigned int i = 2; i < _points.size(); i++)
	{
		//std::cout << "Iter: " << i << std::endl;
		auto pi = _points[i];

		if (!contains(ci, pi))
		{
			std::vector<Point>::const_iterator first = _points.begin();
			std::vector<Point>::const_iterator last = _points.begin() + i;
			std::vector<Point>piMO(first, last);
			ci = minCircleWithPoint(piMO, pi);
		}
	}	

	return ci;
}

std::vector<Point> RandomicIncremental::minCircleWithPoint(std::vector<Point> points, Point q)
{
	std::vector<Point> cj;
	cj.push_back(points[0]);
	cj.push_back(q);

	for (unsigned int j = 1; j < points.size(); j++) {

		auto pj = points[j];
		if (!contains(cj, pj))
		{
			std::vector<Point>::const_iterator first = points.begin();
			std::vector<Point>::const_iterator last = points.begin() + j;
			std::vector<Point>pjMO(first, last);
			cj = minCircleWith2Points(pjMO, pj, q);
		}
	}

	return cj;
}

std::vector<Point> RandomicIncremental::minCircleWith2Points(std::vector<Point> points, Point q1, Point q2)
{
	std::vector<Point> ck;
	ck.push_back(q1);
	ck.push_back(q2);

	for (unsigned int k = 0; k < points.size(); k++) {

		auto pk = points[k];
		if (!contains(ck, pk))
		{
			ck.clear();
			ck.push_back(q1);
			ck.push_back(q2);
			ck.push_back(pk);
		}
	}

	return ck;
}