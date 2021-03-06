#include <iostream>
#include <fstream>
#include <array>
#include "vector2.h"
#include "triangle.h"
#include "delaunay.h"

//nv nt
//0 x0 y0
//1 x1 x1
//. . .
//0 v0 v1 v2
//. . .
//
//E salvar o grafo dual no formato :
//nv nt
//0 x0 y0
//. . .
//0 v0 v1 v2 o0 o1 o2
//. . .

// Gera random float
float RandomFloat(float a, float b) {
	const float random = ((float)rand()) / (float)RAND_MAX;
	const float diff = b - a;
	const float r = random * diff;
	return a + r;
}

// Classe simples que representa o conjunto dos pontos opostos de um triangulo
template <class T>
class OppositePoints
{
public: 
	using VertexType = Vector2<T>;
	OppositePoints(std::vector<VertexType> v) : ops(v) {}
	std::vector<VertexType> ops;
};

int main()
{
	// Gera nuvem de pontos aleatórios
	std::vector<Vector2<float>> points;
	int numberPoints = 1000;
	for (int i = 0; i < numberPoints; ++i) 
	{
		points.push_back(Vector2<float>(i, RandomFloat(0, 1000), RandomFloat(0, 1000)));
	}

	// Faz uma triangulação de Delaunay da nuvem de pontos (input)
	Delaunay<float> triangulation;
	const std::vector<Triangle<float>> triangles = triangulation.triangulate(points);

	std::vector<OppositePoints<float>> opsVector;

	// Bruteforce : cada triangulo olha todos os triangulos do poligono
	for (unsigned int i = 0; i < triangles.size(); i++)
	{
		std::vector<Vector2<float>> opps;
		for (unsigned int j = 0; j < triangles.size(); j++)
		{
			int isOppT = 0;
			int eq1 = -1, eq2 = -1;
			std::vector<int> edgeVs;
			std::vector<Vector2<float>> pi;
			std::vector<Vector2<float>> pj;
			pi.push_back(triangles[i].p1);
			pi.push_back(triangles[i].p2);
			pi.push_back(triangles[i].p3);
			pj.push_back(triangles[j].p1);
			pj.push_back(triangles[j].p2);
			pj.push_back(triangles[j].p3);

			// Vai salvar indice dos vértices que são comuns a dois triangulos
			for (unsigned int ii = 0; ii < pi.size(); ii++)
			{
				for (unsigned int jj = 0; jj < pj.size(); jj++)
				{
					if (pi[ii].index == pj[jj].index) {
						isOppT++;
						edgeVs.push_back(jj);
					}
				}
			}

			// isOppT == 2 signfica que possui dois vertices em comum, acha o oposto 
			// É garantido que um triangulo possui pelo menos um vértice oposto, caso
			// contrário o polígono seria um triangulo apenas
			if (isOppT == 2)
			{
				int opv;
				if (edgeVs[0] + edgeVs[1] == 1)
					opv = 2;
				else if (edgeVs[0] + edgeVs[1] == 2)
					opv = 1;
				else if (edgeVs[0] + edgeVs[1] == 3)
					opv = 0;

				opps.push_back(pj[opv]);
			}
		}

		// Guarda os vertices opostos do triangulo corrente i
		opsVector.push_back(OppositePoints<float>(opps));
	}

	std::ofstream file;
	file.open("incident.txt");
	for (auto point : points)
	{
		// file << point.first << "," << point.second << std::endl;
		file << point.index << " " << point.x << " " << point.y << std::endl;
	}

	file << std::endl;

	for (auto triangle : triangles)
	{
		file << triangle.p1.index << " " << triangle.p2.index << " " <<
			triangle.p3.index << std::endl;
	}

	file.close();

	std::ofstream fileD;
	fileD.open("dualgraph.txt");
	for (auto point : points)
	{
		fileD << point.index << " " << point.x << " " << point.y << std::endl;
	}

	fileD << std::endl;

	for (unsigned int i = 0; i < triangles.size(); i++)
	{
		fileD << triangles[i].p1.index << " " << triangles[i].p2.index << " " <<
			triangles[i].p3.index << " ";

		for (unsigned int j = 0; j < opsVector[i].ops.size(); j++)
			fileD << opsVector[i].ops[j].index << " ";
		for (unsigned int j = 0; j < (3 - opsVector[i].ops.size()); j++)
			fileD << -1 << " ";

		fileD << std::endl;
	}

	fileD.close();

	return 0;
}