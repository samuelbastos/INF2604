#include <iostream>
#include <fstream>
#include <set>
#include "Point.h"
#define _USE_MATH_DEFINES
#include <math.h>

class Edge
{
public:
	inline Edge(unsigned int p1, unsigned int p2) { v1 = p1; v2 = p2; }
	inline Edge() {}
	~Edge() = default;
	inline std::pair<unsigned int, unsigned int> getVs()
	{
		return std::pair<unsigned int, unsigned int>(v1, v2);
	}

private:

	unsigned int t1;
	unsigned int t2;
	unsigned int v1;
	unsigned int v2;
};

class Triangle
{
public:

	inline Triangle(int id, unsigned int v11, unsigned int v22,
		unsigned int v33)
	{
		index = id;
		v1 = v11;
		v2 = v22;
		v3 = v33;
	}

	~Triangle() = default;

	inline unsigned int getV1() { return v1; }
	inline unsigned int getV2() { return v2; }
	inline unsigned int getV3() { return v3; }
	inline std::vector<unsigned int> getVs()
	{ return std::vector<unsigned int>({ v1,v2,v3 }); }
	inline unsigned int getNT1() { return nt1; }
	inline unsigned int getNT2() { return nt2; }
	inline unsigned int getNT3() { return nt3; }
	inline void setNT1(unsigned int nt) { nt1 = nt; }
	inline void setNT2(unsigned int nt) { nt1 = nt; }
	inline void setNT3(unsigned int nt) { nt1 = nt; }
	inline unsigned int getId() { return index; }
	inline Edge getE1() { return e1; }
	inline Edge getE2() { return e2; }
	inline Edge getE3() { return e3; }
	inline void setE1(Edge e) { e1 = e; }
	inline void setE2(Edge e) { e2 = e; }
	inline void setE3(Edge e) { e3 = e; }
	inline void setNewVs(unsigned int v11, unsigned int v22, unsigned int v33)
	{
		v1 = v11;
		v2 = v22;
		v3 = v33;
	}

private:

	int index;
	unsigned int v1;
	unsigned int v2;
	unsigned int v3;
	unsigned int nt1;
	unsigned int nt2;
	unsigned int nt3;
	Edge e1;
	Edge e2;
	Edge e3;
};

/*
 * Método faz uma triangulação do pontos utilizando um método incremental
 * @param points : vértices da triangulação (vector é modificado)
 * @return : Vector de triangulos
 */
std::vector<Triangle> triangulate(std::vector<Point>& points)
{
	std::ofstream file;
	file.open("debug.txt");

	/* Ordena os pontos em X */
	std::sort(points.begin(), points.end(), [](Point& p1, Point& p2) {
		return (p1.getX() < p2.getX());
	});

	std::vector<Triangle> triangles;

	/* Insere pontos do fecho inicial */
	std::vector<Point> ch;

	ch.push_back(points[0]);
	ch.push_back(points[1]);
	ch.push_back(points[2]);

	triangles.push_back(Triangle(triangles.size(), ch[0].getIndex(),
		ch[1].getIndex(), ch[2].getIndex()));

	for (int k = 3; k < points.size(); k++)
	{
		Point p = points[k];
		float eprev, enext;
		int rightTan = 0, leftTan = 0;
		int n = ch.size();
		eprev = Point::isLeft(ch[0], ch[1], p);

		// Isso é aqui é O(numero de pontos no fecho até o momento)
		// Encontra os pontos de retas tangentes
		// http://geomalgorithms.com/a15-_tangents.html
		for (int i = 1; i < n; i++)
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

		// Encontrar os pontos que são visiveis a pk para
		// conectá-los.
		std::vector<int> toConnect;
		for (int j = 0; j < n; j++)
		{
			auto isLeft = Point::isLeft(ch[rightTan], ch[leftTan], ch[j]);
			if (isLeft > 0)
				toConnect.push_back(j);
		}

		if (toConnect.size() == 0) // Apenas conecta com os pontos de tangencia
		{
			auto p0 = ch[rightTan].getIndex();
			auto p1 = ch[leftTan].getIndex();
			auto p2 = p.getIndex();
			triangles.push_back(Triangle(triangles.size(), p0, p1, p2));

			file << "ToConnect == 0" << std::endl;
			file << "point: " << p.getIndex() << std::endl;
			file << "right: " << p0 << std::endl;
			file << "left: " << p1 << std::endl;
			for (int i = 0; i < ch.size(); i++)
			{
				file << "ch[" << i << "] = " << ch[i].getIndex() << std::endl;
			}

			file << std::endl << std::endl;
		}
		if (toConnect.size() == 1) // Só tem um ponto a direita dos pontos de tangencia
		{
			auto right = ch[rightTan].getIndex();
			auto left = ch[leftTan].getIndex();
			auto x = ch[toConnect[0]].getIndex();

			triangles.push_back(Triangle(triangles.size(), x, p.getIndex(), right));
			triangles.push_back(Triangle(triangles.size(), x, p.getIndex(), left));

			file << "ToConnect == 1" << std::endl;
			file << "point: " << p.getIndex() << std::endl;
			file << "right: " << right << std::endl;
			file << "left: " << left << std::endl;
			for (int i = 0; i < ch.size(); i++)
			{
				file << "ch[" << i << "] = " << ch[i].getIndex() << std::endl;
			}
			for (int i = 0; i < toConnect.size(); i++)
			{
				file << "toConnect[" << i << "] = " << ch[toConnect[i]].getIndex() << std::endl;
			}
			file << std::endl << std::endl;
		}
		if (toConnect.size() > 1) // Mais de um ponto entre a direita dos pontos de tangencia
		{
			auto right = ch[rightTan].getIndex();
			auto left = ch[leftTan].getIndex();
			auto last = ch[toConnect[toConnect.size() - 1]].getIndex();
			auto first = ch[toConnect[0]].getIndex();
			file << "ToConnect = 2" << std::endl;
			file << "point: " << p.getIndex() << std::endl;
			file << "right: " << right << std::endl;
			file << "left: " << left << std::endl;
			file << "first: " << first << std::endl;
			file << "last: " << last << std::endl;
			for (int i = 0; i < ch.size(); i++)
			{
				file << "ch[" << i << "] = " << ch[i].getIndex() << std::endl;
			}
			for (int i = 0; i < toConnect.size(); i++)
			{
				file << "toConnect[" << i << "] = " << ch[toConnect[i]].getIndex() << std::endl;
			}
			file << std::endl << std::endl;

			triangles.push_back(Triangle(triangles.size(), p.getIndex(), left, first)); // primeiro triangulo
			triangles.push_back(Triangle(triangles.size(), p.getIndex(), right, last));  // ultimo triangulo
			
			// triangulos intermediários
			for (int i = 1; i < toConnect.size() - 1; i++)
			{
				auto p0 = ch[toConnect[i]].getIndex();
				auto p1 = p.getIndex();
				auto p2 = ch[toConnect[i + 1]].getIndex();
				triangles.push_back(Triangle(triangles.size(), p0, p1, p2));
			}
			std::cout << "oi" << std::endl;
		}
		
		// Encontra indice dos pontos que não pertencem mais ao fecho 
		std::vector<int> toRemoveIndexes;
		for (int j = 0; j < n; j++)
		{
			auto isLeft = Point::isLeft(ch[rightTan], ch[leftTan], ch[j]);
			if (isLeft > 0)
				toRemoveIndexes.push_back(j);
		}

		// Remove os pontos que não pertencem mais ao fecho
		// importante pra não correr risco de ligar com pontos interiores
		// e também pra deixar o algoritmo mais rápido
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
	return triangles;
}

/*
 * Método escreve a malha de triangulos em um arquivo OFF
 * @param pointsConst : vértices da triangulação (vector que não foi ordenado por X)
 * @param triangles : triangulos a serem escritos no arquivo OFF
 * @return : null
 */
void writeOff(std::vector<Triangle> triangles, std::vector<Point> pointsConst, std::string name)
{
	std::ofstream file;
	file.open(name);

	// Escreve o cabeçalho do OFF
	file << "OFF" << std::endl;
	file << pointsConst.size() << " " << triangles.size()
		<< " " << 0 << std::endl;

	// Escreve os pontos no OFF
	for (unsigned int i = 0; i < pointsConst.size(); i++)
	{
		file << pointsConst[i].getX() << " " << pointsConst[i].getY()
			<< " " << 0.f << std::endl;
	}

	// Escreve os triangulos no OFF 
	for (auto t : triangles)
	{
		file << 3 << " " << t.getV1() << " " << t.getV2() << " " << t.getV3() << std::endl;
	}
}

/*
 * Método determina os pares de triangulos (indices) com aresta em comum
 * @param triangles : triangulos a serem vasculhados
 * @return : pares de indices de triangulos
 */
std::vector<std::pair<unsigned int, unsigned int>> sameEdgeTs(std::vector<Triangle> triangles)
{
	std::vector<std::pair<unsigned int, unsigned int>> v;

	for (int i = 0; i < triangles.size(); i++)
	{
		for (int j = 0; j < triangles.size(); j++)
		{
			auto f = triangles[i].getVs();
			auto s = triangles[j].getVs();

			int equalPoints = 0;

			for (auto ff : f)
				for (auto ss : s)
					if (ff == ss && triangles[i].getId()
						!= triangles[j].getId())
						equalPoints++;

			if (equalPoints == 2)
			{
				// checa se ja foi adicionado o par no vetor, adiciona caso não
				bool add = true;
				for (int ii = 0; ii < v.size(); ii++)
				{
					bool one = v[ii].first == i && v[ii].second == j;
					bool two = v[ii].first == j && v[ii].second == i;
					if (one || two) add = false;
				}
				if(add) v.push_back(std::pair<unsigned int, unsigned int>(i, j));
			}

			//std::cout << equalPoints << std::endl;
		}
	}

	return v;
}

/*
 * Método determina se um quad é convexo
 * @param points : vértices do quad
 * @return : bool que indica se o quad é convexo ou não
 */

bool isQuadConvex(std::vector<Point> points)
{
	/* Ordena os pontos em X */
	std::sort(points.begin(), points.end(), [](Point& p1, Point& p2) {
		return (p1.getX() < p2.getX());
	});

	/* Insere pontos do fecho inicial */
	std::vector<Point> ch;
	ch.push_back(points[0]);
	ch.push_back(points[1]);
	ch.push_back(points[2]);

	for (int k = 3; k < points.size(); k++)
	{
		Point p = points[k];
		float eprev, enext;
		int rightTan = 0, leftTan = 0;
		int n = ch.size();
		eprev = Point::isLeft(ch[0], ch[1], p);

		// Isso é aqui é O(numero de pontos no fecho até o momento)
		// Encontra os pontos de retas tangentes
		// http://geomalgorithms.com/a15-_tangents.html
		for (int i = 1; i < n; i++)
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
		for (int j = 0; j < n; j++)
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

	return ch.size() == points.size();
}

// https://www.calculator.net/triangle-calculator.html
/*
 * Método determina se uma aresta é legal
 * @param pointsConst : pontos da triangulacao
 * @param triangles : triangulos
 * @param i1 e i2 : indice dos triangulos que possuem a mesma aresta
 * @return : bool que indica se a aresta é legal ou não
 */
bool isLegal(std::vector<Point> pointsConst, std::vector<Triangle>& triangles, unsigned int i1, unsigned int i2)
{
	auto t1 = triangles[i1].getVs();
	auto t2 = triangles[i2].getVs();

	// Ângulos de T1
	auto a1 = Point::distance(pointsConst[t1[0]], pointsConst[t1[1]]);
	auto b1 = Point::distance(pointsConst[t1[1]], pointsConst[t1[2]]);
	auto c1 = Point::distance(pointsConst[t1[2]], pointsConst[t1[0]]);

	float angleA1 = acosf((b1*b1 + c1*c1 - a1*a1) / (2*b1*c1)); // Lei dos cossenos
	float angleB1 = asinf(b1*sinf(angleA1) / a1);				// Lei do senos
	float angleC1 = M_PI - angleA1 - angleB1;					// Soma dos angulos = PI
 	float sum = angleA1 + angleB1 + angleC1;
	
	// Ângulos de T2
	auto a2 = Point::distance(pointsConst[t2[0]], pointsConst[t2[1]]);
	auto b2 = Point::distance(pointsConst[t2[1]], pointsConst[t2[2]]);
	auto c2 = Point::distance(pointsConst[t2[2]], pointsConst[t2[0]]);

	float angleA2 = acosf((b2*b2 + c2 * c2 - a2 * a2) / (2 * b2*c2)); // Lei dos cossenos
	float angleB2 = asinf(b2*sinf(angleA2) / a2);					  // Lei do senos
	float angleC2 = M_PI - angleA2 - angleB2;			              // Soma dos angulos = PI
	sum = angleA2 + angleB2 + angleC2;

	std::vector<float> angles1;
	angles1.push_back(angleA1);
	angles1.push_back(angleB1);
	angles1.push_back(angleC1);
	angles1.push_back(angleA2);
	angles1.push_back(angleB2);
	angles1.push_back(angleC2);
	std::sort(angles1.begin(), angles1.end());

	std::vector<unsigned int> eqvs;

	// pega os indices da aresta em comum
	for (int i = 0; i < t1.size(); i++)
	{
		for (int j = 0; j < t2.size(); j++)
		{
			if (t1[i] == t2[j])
				eqvs.push_back(t1[i]);
		}
	}
	
	// descobre ponto oposto de T1
	int a = -1;
	for (int i = 0; i < t1.size(); i++)
	{
		if (t1[i] != eqvs[0] && t1[i] != eqvs[1])
			a = t1[i];
	}

	// descobre ponto oposto de T2
	int d = -1;
	for (int i = 0; i < t2.size(); i++)
	{
		if (t2[i] != eqvs[0] && t2[i] != eqvs[1])
			d = t2[i];
	}

	auto b = eqvs[0];
	auto c = eqvs[1];

	// Ângulos de T1
	a1 = Point::distance(pointsConst[a], pointsConst[c]);
	b1 = Point::distance(pointsConst[c], pointsConst[d]);
	c1 = Point::distance(pointsConst[d], pointsConst[a]);

	angleA1 = acosf((b1*b1 + c1 * c1 - a1 * a1) / (2 * b1*c1)); // Lei dos cossenos
	angleB1 = asinf(b1*sinf(angleA1) / a1);				        // Lei do senos
	angleC1 = M_PI - angleA1 - angleB1;					        // Soma dos angulos = PI
	sum = angleA1 + angleB1 + angleC1;

	// Ângulos de T2
	a2 = Point::distance(pointsConst[a], pointsConst[b]);
	b2 = Point::distance(pointsConst[b], pointsConst[d]);
	c2 = Point::distance(pointsConst[d], pointsConst[a]);

	angleA2 = acosf((b2*b2 + c2 * c2 - a2 * a2) / (2 * b2*c2)); // Lei dos cossenos
	angleB2 = asinf(b2*sinf(angleA2) / a2);					    // Lei do senos
	angleC2 = M_PI - angleA2 - angleB2;			                // Soma dos angulos = PI
	sum = angleA2 + angleB2 + angleC2;

	std::vector<float> angles2;
	angles2.push_back(angleA1);
	angles2.push_back(angleB1);
	angles2.push_back(angleC1);
	angles2.push_back(angleA2);
	angles2.push_back(angleB2);
	angles2.push_back(angleC2);
	std::sort(angles2.begin(), angles2.end());

	// Aresta é ilegal, precisa mudar os triangulos
	if (angles1[0] < angles2[0])
	{
		triangles[i1].setNewVs(a, c, d);
		triangles[i2].setNewVs(a, b, d);
		return false;
	}

	return true;
}

int main()
{
	/** Cria container de 1000 pontos aleatórios [0,1000] */
	std::vector<Point> points = Point::generateCloud(20);
	std::vector<Point> pointsConst = points;

	auto triangles = triangulate(points);
	writeOff(triangles, pointsConst, "triangles.off");

	bool end = false;
	while (!end)
	{
		auto trianglePairs = sameEdgeTs(triangles);
		std::vector<bool> inversible;

		for (int i = 0; i < trianglePairs.size(); i++)
		{
			auto t1 = triangles[trianglePairs[i].first].getVs();
			auto t2 = triangles[trianglePairs[i].second].getVs();
			std::vector<Point> quad;

			// adiciona os vértices do primeiro triangulo
			for (int j = 0; j < 3; j++)
				quad.push_back(pointsConst[t1[j]]);

			// adiciona os vértices do segundo triangulo (sem repetir)
			for (int j = 0; j < 3; j++)
			{
				bool add = true;
				for (int k = 0; k < quad.size(); k++)
				{
					if (quad[k].getIndex() == pointsConst[t2[j]].getIndex())
						add = false;
				}

				if (add) quad.push_back(pointsConst[t2[j]]);
			}

			// se o quadrilátero é convexo pode ter aresta invertida
			bool convexity = isQuadConvex(quad);
			inversible.push_back(convexity);
			//std::cout << convexity << std::endl;
		}

		int finish = 0;
		for (int i = 0; i < inversible.size(); i++)
		{
			if (inversible[i]) // se for inversivel, verifica se é uma aresta legal
			{
				auto legalE = isLegal(pointsConst, triangles, trianglePairs[i].first, trianglePairs[i].second);
				if (!legalE) { finish++; break; }
			}
		}

		// Não ocorreu nenhuma inversão, convergiu
		if (finish == 0) end = true;
	}

	writeOff(triangles, pointsConst, "delaunay.off");

	return 0;
}