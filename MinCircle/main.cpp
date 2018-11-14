#include <stdio.h>
#include <vector>
#include <utility>
#include <iostream>
#include <fstream>
#include <cstdio>
#include "random.hpp"
#include "Heuristic.h"
#include "RandomicIncremental.h"

// get base random alias which is auto seeded and has static API and internal state
using Random = effolkronium::random_static;

int main() 
{
	/** Cria container de 1000 pontos aleatórios [0,1000] */
	std::vector<Point> points;
	for (int i = 0; i < 1000; i++) 
	{
		auto x = Random::get(0.f, 1000.f);
		auto y = Random::get(0.f, 1000.f);
		points.push_back(Point(x, y));
	}

	/** Classe responsável por executar o algoritmo heuristico */
	std::cout << "Início do Algoritmo Heuristico" << std::endl;
	Heuristic heuristicAlgo(points);
	heuristicAlgo.execute();
	auto minCenterRadius = heuristicAlgo.getMinCenterRadius();
	std::cout << "Center: (" << minCenterRadius.first.getX() << "," << minCenterRadius.first.getY() << ")" << std::endl;
	std::cout << "Radius: " << minCenterRadius.second << std::endl;
	std::cout << "Término do Algoritmo Heuristico" << std::endl;

	/** Classe responsável por executar o algoritmo randomico incremental */
	std::cout << "Início do Algoritmo Randomico Incremental" << std::endl;
	RandomicIncremental riAlgo(points);
	riAlgo.execute();
	auto minCircle = riAlgo.getMinCircle();
	for (auto point : minCircle)
	{
		std::cout << "point: " << point.getX() << ", " << point.getY() << std::endl;
	}
	std::cout << "Término do Algoritmo Randomico Incremental" << std::endl;


	/* Calcula centro e raio para o circulo do Algoritmo Randomico Incremental */
	Point riCenter;
	double riRadius;
	if (minCircle.size() == 2)
	{
		double x = (minCircle[0].getX() + minCircle[1].getX()) / 2;
		double y = (minCircle[0].getY() + minCircle[1].getY()) / 2;
		riCenter = Point(x, y);
		double xx = (minCircle[0].getX() - minCircle[1].getX()) * (minCircle[0].getX() - minCircle[1].getX());
		double yy = (minCircle[0].getY() - minCircle[1].getY()) * (minCircle[0].getY() - minCircle[1].getY());
		riRadius = std::sqrt(xx + yy) / 2;
	}
	else
	{
		double xx = (minCircle[0].getX() - minCircle[1].getX()) * (minCircle[0].getX() - minCircle[1].getX());
		double yy = (minCircle[0].getY() - minCircle[1].getY()) * (minCircle[0].getY() - minCircle[1].getY());
		double a = std::sqrt(xx + yy) / 2;

		xx = (minCircle[1].getX() - minCircle[2].getX()) * (minCircle[1].getX() - minCircle[2].getX());
		yy = (minCircle[1].getY() - minCircle[2].getY()) * (minCircle[1].getY() - minCircle[2].getY());
		double b = std::sqrt(xx + yy) / 2;

		xx = (minCircle[2].getX() - minCircle[0].getX()) * (minCircle[2].getX() - minCircle[0].getX());
		yy = (minCircle[2].getY() - minCircle[0].getY()) * (minCircle[2].getY() - minCircle[0].getY());
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
				double x = (minCircle[0].getX() + minCircle[1].getX()) / 2;
				double y = (minCircle[0].getY() + minCircle[1].getY()) / 2;
				riCenter = Point(x, y);
				double xx = (minCircle[0].getX() - minCircle[1].getX()) * (minCircle[0].getX() - minCircle[1].getX());
				double yy = (minCircle[0].getY() - minCircle[1].getY()) * (minCircle[0].getY() - minCircle[1].getY());
				riRadius = std::sqrt(xx + yy) / 2;
			}
			if (b > a && b > c)
			{
				double x = (minCircle[1].getX() + minCircle[2].getX()) / 2;
				double y = (minCircle[1].getY() + minCircle[2].getY()) / 2;
				riCenter = Point(x, y);
				double xx = (minCircle[1].getX() - minCircle[2].getX()) * (minCircle[1].getX() - minCircle[2].getX());
				double yy = (minCircle[1].getY() - minCircle[2].getY()) * (minCircle[1].getY() - minCircle[2].getY());
				riRadius = std::sqrt(xx + yy) / 2;
			}
			if (c > a && c > b)
			{
				double x = (minCircle[2].getX() + minCircle[0].getX()) / 2;
				double y = (minCircle[2].getY() + minCircle[0].getY()) / 2;
				riCenter = Point(x, y);
				double xx = (minCircle[2].getX() - minCircle[0].getX()) * (minCircle[2].getX() - minCircle[0].getX());
				double yy = (minCircle[2].getY() - minCircle[0].getY()) * (minCircle[2].getY() - minCircle[0].getY());
				riRadius = std::sqrt(xx + yy) / 2;
			}
		}
		/* sem angulo obtuso */
		else
		{
			double x = (minCircle[0].getX() + minCircle[1].getX() + minCircle[2].getX()) / 3;
			double y = (minCircle[0].getY() + minCircle[1].getY() + minCircle[2].getY()) / 3;
			riCenter = Point(x, y);

			double xx = (riCenter.getX() - minCircle[1].getX()) * (riCenter.getX() - minCircle[1].getX());
			double yy = (riCenter.getY() - minCircle[1].getY()) * (riCenter.getY() - minCircle[1].getY());
			riRadius = std::sqrt(xx + yy) / 2;
		}
	}

	std::cout << "Escrevendo pontos em disco" << std::endl;
	std::ofstream file;
	file.open("points.txt");
	for (auto point : points)
	{
		file << point.getX() << "," << point.getY() << std::endl;
	}
	file.close();

	file.open("circlecoord.txt");
	file << "HEURISTIC:" << std::endl << "CENTER: (" << minCenterRadius.first.getX() << ","
		<< minCenterRadius.first.getY() << ")" << std::endl
		<<"RADIUS: " << minCenterRadius.second << std::endl << std::endl;

	file << "RI:" << std::endl << "CENTER: (" << riCenter.getX() << ","
		<< riCenter.getY() << ")" << std::endl
		<< "RADIUS: " << riRadius << std::endl;
	for (auto point : minCircle)
	{
		file << "POINT: " << point.getX() << ", " << point.getY() << std::endl;
	}

	int counterRI = 0;
	/* TESTE PARA ALGORITMO RANDOMICO INCREMENTAL */
	for (auto point : points)
	{
		double xx = (point.getX() - riCenter.getX()) * (point.getX() - riCenter.getX());
		double yy = (point.getY() - riCenter.getY()) * (point.getY() - riCenter.getY());
		double d = std::sqrt(xx + yy);

		if (d > riRadius)
			counterRI++;
	}

	int counterH = 0;
	/* TESTE PARA ALGORITMO HEURISTICO */
	for (auto point : points)
	{
		double xx = (point.getX() - minCenterRadius.first.getX()) * (point.getX() - minCenterRadius.first.getX());
		double yy = (point.getY() - minCenterRadius.first.getY()) * (point.getY() - minCenterRadius.first.getY());
		double d = std::sqrt(xx + yy);

		if (d > minCenterRadius.second)
			counterH++;
	}

	/* Cálculo de erro */
	double errorNum = std::fabs((minCenterRadius.second*minCenterRadius.second)*std::_Pi - (riRadius * riRadius*std::_Pi));
	double error = errorNum / (riRadius * riRadius*std::_Pi);
	std::cout << "Error: " << error << std::endl;

	std::cout << "Fim" << std::endl;

	/** Tá aqui apenas pra impedir do console fechar imediatamente */
	std::getchar();

	return 0;
}