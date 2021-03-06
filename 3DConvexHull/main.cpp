#include <iostream>
#include "Vector3D.h"
#include <map>
#include <string>
#include "random.hpp"
using Random = effolkronium::random_static;

class Triangle
{
public:

	inline Triangle(unsigned int v11, unsigned int v22,
		unsigned int v33)
	{
		v1 = v11;
		v2 = v22;
		v3 = v33;
	}

	~Triangle() = default;

	inline unsigned int getV1() { return v1; }
	inline unsigned int getV2() { return v2; }
	inline unsigned int getV3() { return v3; }

private:

	unsigned int v1;
	unsigned int v2;
	unsigned int v3;
};

class Edge
{
public:

	inline Edge(unsigned int v11, unsigned v22)
	{ v1 = v11; v2 = v22; }
	~Edge() = default;

	inline unsigned int getV1() { return v1; }
	inline unsigned int getV2() { return v2; }

private:

	unsigned int v1;
	unsigned v2;
};

// To find orientation of ordered triplet (p1, p2, p3). 
// The function returns following values 
// 0 --> p, q and r are colinear 
// 1 --> Clockwise 
// 2 --> Counterclockwise 
// https://www.geeksforgeeks.org/orientation-3-ordered-points/
int orientation(Point3Dd p1, Point3Dd p2, Point3Dd p3)
{
	// See 10th slides from following link for derivation 
	// of the formula 
	double val = (p2.y - p1.y) * (p3.x - p2.x) -
		(p2.x - p1.x) * (p3.y - p2.y);

	if (val == 0.0) return 0;  // colinear 

	return (val > 0.0) ? 1 : 2; // clock or counterclock wise 
}

void reverseTriangle(Triangle& t)
{
	t = Triangle(t.getV1(), t.getV3(), t.getV2());
}

void writeOFF(std::string path, std::vector<Point3Dd> points, std::vector<Triangle> hull)
{
	std::ofstream file;
	file.open(path);

	// Escreve o cabeçalho do OFF
	file << "OFF" << std::endl;
	file << points.size() << " " << hull.size()
		<< " " << 0 << std::endl;

	// Escreve os pontos no OFF
	for (auto point : points)
	{
		file << point.x << " " << point.y <<
			" " << point.z << std::endl;
	}

	// Escreve os triangulos no OFF 
	for (auto f : hull)
	{
		file << 3 << " " << f.getV1() << " " << f.getV2() << " " << f.getV3() << std::endl;
	}

	file.close();
}

void writeXYZ(std::string path, std::vector<Point3Dd> points)
{
	std::ofstream file;
	file.open(path);

	// Escreve os pontos no OFF
	for (auto point : points)
	{
		file << point.x << " " << point.y <<
			" " << point.z << std::endl;
	}

	file.close();
}

// https://en.wikipedia.org/wiki/Tetrahedron
double calculateTetrahedronVolume(std::vector<Point3Dd> points, Triangle t, Point3Dd d)
{
	Point3Dd a = points[t.getV1()];
	Point3Dd b = points[t.getV2()];
	Point3Dd c = points[t.getV3()];
	Point3Dd ad = a - d;
	Point3Dd bd = b - d;
	Point3Dd cd = c - d;
	Point3Dd bdxcd;
	bdxcd.cross(bd, cd);
	return ad.dot(bdxcd)/6.0;
}

bool hasEdge(std::vector<Edge> edges, Edge searchEdge)
{
	bool result = false;

	for (auto e : edges)
	{
		bool nonInverse = e.getV1() == searchEdge.getV1()
			&& e.getV2() == searchEdge.getV2();
		bool inverse = e.getV2() == searchEdge.getV1()
			&& e.getV1() == searchEdge.getV2();

		if (nonInverse || inverse)
			result = true;
	}

	return result;
}


int getEdgeIndex(std::vector<Edge> edges, Edge searchEdge)
{
	int result = -1;

	for (unsigned int i = 0; i < edges.size(); i++)
	{
		bool nonInverse = edges[i].getV1() == searchEdge.getV1()
			&& edges[i].getV2() == searchEdge.getV2();
		bool inverse = edges[i].getV2() == searchEdge.getV1()
			&& edges[i].getV1() == searchEdge.getV2();

		if (nonInverse || inverse)
			result = i;
	}

	return result;
}

std::vector<Edge> getCornerEdges(std::vector<Triangle> triangles)
{
	std::vector<Edge> edges;
	std::vector<int> edgeCounter;
	std::vector<Edge> result;

	for (auto& t : triangles)
	{
		Edge one(t.getV1(), t.getV2());
		Edge two(t.getV2(), t.getV3());
		Edge three(t.getV3(), t.getV1());

		// Vertice 1
		if (!hasEdge(edges, one))
		{
			edges.push_back(one);
			edgeCounter.push_back(1);
		}
		else
		{
			int index = getEdgeIndex(edges, one);
			edgeCounter[index]++;
		}

		// Vertice 2
		if (!hasEdge(edges, two))
		{
			edges.push_back(two);
			edgeCounter.push_back(1);
		}
		else
		{
			int index = getEdgeIndex(edges, two);
			edgeCounter[index]++;
		}

		// Vertice 3
		if (!hasEdge(edges, three))
		{
			edges.push_back(three);
			edgeCounter.push_back(1);
		}
		else
		{
			int index = getEdgeIndex(edges, three);
			edgeCounter[index]++;
		}
	}

	// As únicas arestas que são as de interesse são as que possuem
	// única ocorrência
	for (unsigned int i = 0; i < edges.size(); i++)
	{
		if (edgeCounter[i] == 1)
			result.push_back(edges[i]);
	}

	return result;
}

// Pega hull sem as faces visiveis
std::vector<Triangle> getHullWoVisibleFaces(std::vector<Triangle> actualHull,
			std::vector<unsigned int> occludedIndexes)
{
	std::vector<Triangle> newHull;

	for (unsigned int i = 0; i < occludedIndexes.size(); i++)
	{
		newHull.push_back(actualHull[occludedIndexes[i]]);
	}

	return newHull;
}

Point3Dd circumcenter(const Point3Dd& a, const Point3Dd& b, const Point3Dd& c)
{
	Point3Dd ac = c - a;
	Point3Dd ab = b - a;

	Point3Dd abXac;
	abXac.cross(ab, ac);

	Point3Dd abXacXab;
	abXacXab.cross(abXac, ab);

	Point3Dd acXabXac;
	acXabXac.cross(ac, abXac);

	Point3Dd num = abXacXab* (ac.sqrLength()) + acXabXac * (ab.sqrLength());
	double dem = 2.0 * (abXac.sqrLength());
	Point3Dd cc = num / dem;
	Point3Dd res = a + cc;
	return res;
}

 std::vector<Point3Dd> generateCloud(int qtd)
{
	std::vector<Point3Dd> points;
	for (int i = 0; i < qtd; i++)
	{
		auto x = Random::get(0.0, 1000.0);
		auto y = Random::get(0.0, 1000.0);
		auto z = Random::get(0.0, 1000.0);
		points.push_back(Point3Dd(i, x, y, z));
	}

	return points;
}

 void reorientFaces(std::vector<Point3Dd> points, std::vector<Triangle>& hull)
 {
	 double x = (points[0].x + points[1].x + points[2].x + points[3].x) / 4.0;
	 double y = (points[0].y + points[1].y + points[2].y + points[3].y) / 4.0;
	 double z = (points[0].z + points[1].z + points[2].z + points[3].z) / 4.0;

	 for (auto& face : hull)
	 {
		 auto e1 = points[face.getV2()] - points[face.getV1()];
		 auto e2 = points[face.getV3()] - points[face.getV1()];
		 Point3Dd normal;
		 normal.cross(e1, e2);

		 double a = normal.x;
		 double b = normal.y;
		 double c = normal.z;
		 double x0 = points[face.getV2()].x;
		 double y0 = points[face.getV2()].y;
		 double z0 = points[face.getV2()].z;

		 double d = -((a * x0) + (b * y0) + (c * z0));

		 // se positivo  -> normal pra dentro
		 // negativo -> normal pra fora
		 double result = a * x + b * y + c * z + d;
		 if (result > 0.0)
			 reverseTriangle(face);
	 }
 }

int main()
{
	// Gera nuvem de pontos
	std::vector<Point3Dd> points = generateCloud(100);
	
	// Triangulos iniciais
	Triangle t1(0, 1, 2);
	Triangle t2(0, 1, 3);
	Triangle t3(1, 2, 3);
	Triangle t4(2, 0, 3);

	std::vector<Triangle> hull({t1,t2,t3,t4});

	// Inverte orientação dos triangulos caso seja CW
	for (auto& triangle : hull)
	{
		if (orientation(points[triangle.getV1()], points[triangle.getV2()],
			points[triangle.getV3()]) == 1)
		{
			reverseTriangle(triangle);
		}
	}

	reorientFaces(points, hull);
	writeOFF("initial.off", points, hull);

	// Algoritmo
	// http://www.bowdoin.edu/~ltoma/teaching/cs3250-CompGeom/spring17/Lectures/cg-hull3d.pdf
	for (unsigned int i = 4; i < points.size(); i++)
	{
		std::vector<Triangle> visibleTs;
		std::vector<unsigned int> visibleTsIndexes;
		std::vector<unsigned int> occludedTsIndexes;

		for (unsigned int j = 0; j < hull.size(); j++)
		{
			// *** add to visible triangles list 

			// Calcula volume, se menor que zero é triangulo visivel!
			double volume = calculateTetrahedronVolume(points, hull[j], points[i]);
			if (volume < 0.0)
			{
				visibleTs.push_back(hull[j]);
				visibleTsIndexes.push_back(j);
			}
			else
			{
				occludedTsIndexes.push_back(j);
			}
		}

		if (visibleTs.size() > 0)
		{
			// *** find border edge of all visible faces
			// *** for each border edge e construct  a face (e,pi) and add to HULL
			// *** for each visible face f: delete from H

			// Pega os edges de corner dos triangulos visiveis
			auto cornerEdges = getCornerEdges(visibleTs);

			// Remove faces visiveis e adiciona novas faces
			std::vector<Triangle> newHull = getHullWoVisibleFaces(hull, occludedTsIndexes);
			for (auto edge : cornerEdges)
			{
				Triangle t(edge.getV1(), edge.getV2(), i);
				newHull.push_back(t);
			}

			hull = newHull;
			reorientFaces(points, hull);
			if (i < 10)
			{
				std::string path = "hull-"  + std::to_string(i) +  ".off";
				writeOFF(path, points, hull);
			}
		}
		// *** else : discard pi (pi must be inside H)
	}

	// Escreve no arquivo
	writeOFF("hull.off", points, hull);
	writeXYZ("hull-points.xyz", points);
	return 0;
}