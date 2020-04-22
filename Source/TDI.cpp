#include <stdio.h>
#include <stdlib.h>  
#include <iostream>
#include <vector>
#include <map>
#include <thread>

#include <math.h>

#include <C_General.hpp>
#include <C_Matrix.hpp>
#include <C_Image.hpp>

int Test(int argc, char **argv);
int GREY_SCALE = 256;
C_Image img(0, 0, 0, 0, 0);
C_Image a;
C_Matrix a_sobel;
typedef long IndexT;
typedef double ElementT;

struct Point{
	IndexT x = -1;
	IndexT y = -1;
	double sob;
};

int Get_Limite() {
	int histograma[256] = {};
	IndexT row, col;

	/*Definicion del historigrama*/
	for (int i = 0; i < GREY_SCALE; i++) histograma[i] = 0;

	for (row = a.FirstRow(); row <= a.LastRow(); row++)
		for (col = a.FirstCol(); col <= a.LastCol(); col++)
			histograma[(int)a(row, col)]++;

	//Busqueda de punto de equilibrio de histograma
	//Pendiente de implementacion algoritmo de OTSU, posible mejora
	int medio = 128;
	int pesoIz = histograma[0];
	int pesoDe = histograma[255];
	int ini = 0;
	int fin = 255;

	while (ini <= fin) {
		if (pesoDe > pesoIz) {
			pesoDe -= histograma[fin--];
			if (((ini + fin) / 2) < medio) {
				pesoDe += histograma[medio];
				pesoIz -= histograma[medio--];
			}
		}
		else if (pesoIz >= pesoDe) {
			pesoIz -= histograma[ini++];
			if (((ini + fin) / 2) >= medio) {
				pesoIz += histograma[medio + 1];
				pesoDe -= histograma[medio + 1];
				medio++;
			}
		}
	}
	return medio;
}

void Sobel(int limite) {
	IndexT row, col;
	a_sobel = a;

	auto aux = 0;
	double gx = 0;
	double gy = 0;
	double g = 0;

	for (row = a.FirstRow()+1; row <= a.LastRow()-1; row++)
		for (col = a.FirstCol()+1; col <= a.LastCol()-1; col++) {
			gx = (a(row - 1, col + 1) + (2 * a(row, col + 1)) + (a(row, col))) - (a(row - 1, col - 1) + (2 * a(row, col - 1)) + (a(row + 1, col - 1)));
			gy = (a(row + 1, col - 1) + (2 * a(row, col - 1)) + (a(row - 1, col + 1))) - (a(row - 1, col -1) + (2 * a(row - 1, col)) + (a(row - 1, col + 1)));
			g = sqrt(pow(gx,2) + pow (gx, 2));
			a_sobel(row, col) = (g < 0) ? 0 : ((g > 255)? 255 : g);
			//a_sobel(row, col) = g;
		}

	a.Free();
}

void FloodFillRecursivo(IndexT x, IndexT y, long vecino, long seeds, long color) {
	if (img.In(x, y)) {
		if (img(x, y) != color) vecino--;
		if (img(x, y) == color)
		{
			img(x, y) = seeds;
			FloodFillRecursivo(x + 1, y, vecino, seeds, color);
			FloodFillRecursivo(x, y + 1, vecino, seeds, color);
			FloodFillRecursivo(x - 1, y, vecino, seeds, color);
			FloodFillRecursivo(x, y - 1, vecino, seeds, color);
		}
	}
}

void FloodFill(IndexT x, IndexT y, long vecino, long seeds, long color)
{
	vector<Point> queue;
	auto p = Point();
	auto check = Point();
	p.x = x;
	p.y = y;

	queue.push_back(p);

	while (!queue.empty())
	{
		Point filled = queue.back();
		queue.pop_back();

		// Left
		check.x = filled.x - 1;
		check.y = filled.y;
		if (img.In(check.x, check.y) && img(check.x, check.y) == color)
		{
			img(check.x, check.y) = seeds;
			queue.push_back(check);
		}

		// Right
		check.x = filled.x + 1;
		check.y = filled.y;
		if (img.In(check.x, check.y) && img(check.x, check.y) == color)
		{
			img(check.x, check.y) = seeds;
			queue.push_back(check);
		}

		// Top
		check.x = filled.x;
		check.y = filled.y + 1;
		if (img.In(check.x, check.y) && img(check.x, check.y) == color)
		{
			img(check.x, check.y) = seeds;
			queue.push_back(check);
		}

		// Bot
		check.x = filled.x;
		check.y = filled.y - 1;
		if (img.In(check.x, check.y) && img(check.x, check.y) == color)
		{
			img(check.x, check.y) = seeds;
			queue.push_back(check);
		}
	}
}

void Seeds(long umbral, long vecino,long seeds, long color) {
	IndexT row, col;
	auto a_seeds = seeds;
	for (row = img.FirstRow(); row <= img.LastRow(); row++)
		for (col = img.FirstCol(); col <= img.LastCol(); col++) {
			if (a_sobel(row, col) < umbral) img(row, col) = color;
		}

	for (row = img.FirstRow(); row <= img.LastRow(); row++)
		for (col = img.FirstCol(); col <= img.LastCol(); col++) {
			if (img(row, col) == color) {
				FloodFill(row, col, vecino, a_seeds, color);
				a_seeds++;
				if (a_seeds >= 255) a_seeds = seeds;
			}
		}

	
}

void WaterShed(long umbral, long vecino = 0, long seeds = 10, long color = 255) {
	IndexT row, col;

	//Primera parte, generacion de semillas
	Sobel(Get_Limite());
	Seeds(umbral, vecino, seeds, color);

	//Segunda parte, inundacion
	//auto limite = umbral;
	//auto aux = 0;
	//Point pos = Point();

	//while (limite > 0) {
	//	for (row = img.FirstRow() + 1; row <= img.LastRow() - 1; row++)
	//		for (col = img.FirstCol() + 1; col <= img.LastCol() - 1; col++) {
	//			if (a_sobel(row, col) < limite) continue;

	//			if (img(row + 1, col) > 0) {
	//				img(row, col) = img(row + 1, col);
	//				continue;
	//			}
	//			if (img(row, col + 1) > 0) {
	//				img(row, col) = img(row, col + 1);
	//				continue;
	//			}				
	//			if (img(row + 1, col + 1) > 0) {
	//				img(row, col) = img(row + 1, col + 1);
	//				continue;
	//			}
	//			if (img(row - 1, col) > 0) {
	//				img(row, col) = img(row - 1, col);
	//				continue;
	//			}
	//			if (img(row, col - 1) > 0) {
	//				img(row, col) = img(row, col - 1);
	//				continue;
	//			}
	//			if (img(row - 1, col - 1) > 0) {
	//				img(row, col) = img(row - 1, col - 1);
	//				continue;
	//			}
	//			if (img(row + 1, col - 1) > 0) {
	//				img(row, col) = img(row - 1, col - 1);
	//				continue;
	//			}
	//			if (img(row - 1, col + 1) > 0) {
	//				img(row, col) = img(row, col + 1);
	//			}
	//		}
	//	limite--;
	//}
}

int main(int argc, char **argv)
{
	a.ReadBMP("MisEjemplos/Aguadulce_Gris.bmp");

	img.Resize(a.FirstRow(),a.LastRow(),a.FirstCol(),a.LastCol(),0);

	a.Grey();
	a.MedianFilter(a, 3);

	WaterShed(20);

	img.palette.Read("PaletaSurtida256.txt");

	C_Image sob(a_sobel);
	sob.WriteBMP("MisEjemplos/water_coinsSOB.bmp");
	img.WriteBMP("MisEjemplos/water_coinsWAT.bmp");

	return 0;
}