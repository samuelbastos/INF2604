#include <iostream>
#include <set>
#include "Point.h"
#include <fstream>

int main()
{
	/** Cria container de 1000 pontos aleatórios [0,1000] */
	std::vector<Point> points = Point::generateCloud(1000);
	std::vector<Point> pointsConst = points;

	/* Ordena os pontos em X */
	std::sort(points.begin(), points.end(), [](Point& p1, Point& p2) {
		return (p1.getX() < p2.getX());
	});

	/* Insere pontos do fecho inicial */
	std::vector<Point> ch;
	ch.push_back(points[0]);
	ch.push_back(points[1]);
	ch.push_back(points[2]);
	
	for (unsigned int k = 3; k < points.size(); k++)
	{
		Point p = points[k];
		float eprev, enext;
		int rightTan = 0, leftTan = 0;
		int n = ch.size(); 
		eprev = Point::isLeft(ch[0], ch[1], p);

		// Isso é aqui é O(numero de pontos no fecho até o momento)
		// Encontra os pontos de retas tangentes
		// http://geomalgorithms.com/a15-_tangents.html
		for (unsigned int i = 1; i < n; i++)
		{
			// Se chegar ao último ponto do fecho, o ponto seguinte é o primeiro ponto
			int next = (i == n - 1) ? 0 : i + 1;
			enext = Point::isLeft(ch[i], ch[next], p); 

			if ((eprev <= 0) && (enext > 0))
			{
				bool below = Point::isLeft(p, ch[i], ch[rightTan]) < 0;
				if (!below) rightTan = i;
			}
			else if ((eprev > 0) && (enext <= 0))
			{
				bool above = Point::isLeft(p, ch[i], ch[leftTan]) > 0;
				if (!above) leftTan = i;
			}

			eprev = enext;
		}

		// Encontra indice dos pontos que não pertencem mais ao fecho
		std::vector<int> toRemoveIndexes;
		for (unsigned int j = 0; j < n; j++)
		{
			auto isLeft = Point::isLeft(ch[rightTan], ch[leftTan], ch[j]);
			if (isLeft > 0)
				toRemoveIndexes.push_back(j);
		}

		// Remove os pontos que não pertencem mais ao fecho
		std::sort(toRemoveIndexes.begin(), toRemoveIndexes.end());
		if (toRemoveIndexes.size() > 0)
		{
			for (int r = 0; r < toRemoveIndexes.size(); r++)
			{
				int ri = toRemoveIndexes[r] - r;
				ch.erase(ch.begin() + ri);
			}
		}
		
		ch.push_back(p);
	}

	std::ofstream file;
	file.open("points-and-convexhull.txt");

	for (auto point : pointsConst)
	{
		file << point.getIndex() << " " << point.getX() << " " << point.getY() << " " << std::endl;
	}

	file << std::endl;

	for (auto c : ch)
	{
		file << c.getIndex() << std::endl;
	}

	file.close();

	return 0;
}