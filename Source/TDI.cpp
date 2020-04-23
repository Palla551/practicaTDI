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

struct Point {
	IndexT x = -1;
	IndexT y = -1;
	double sob;
};

void Sobel() {
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

int FloodFill(IndexT x, IndexT y, long vecinos, long seeds, long color)
{
	vector<Point> queue;
	auto vecino = vecinos;
	auto saliente = 0;
	auto cont = 0;
	auto p = Point();
	auto check = Point();
	p.x = x;
	p.y = y;

	queue.push_back(p);
	img(x, y) = seeds;

	while (!queue.empty())
	{
		Point filled = queue.back();
		queue.pop_back();
		cont++;

		// Left
		check.x = filled.x - 1;
		check.y = filled.y;
		if (img.In(check.x, check.y) && (img(check.x, check.y) == color))
		{
		left:
			img(check.x, check.y) = seeds;
			queue.push_back(check);
		}
		else if (img.In(check.x, check.y) && vecino > 0) {
			saliente ++;
			goto left;
		}

		// Right
		check.x = filled.x + 1;
		check.y = filled.y;
		if (img.In(check.x, check.y) && (img(check.x, check.y) == color))
		{
		rigth:
			img(check.x, check.y) = seeds;
			queue.push_back(check);
		}
		else if (img.In(check.x, check.y) && vecino > 0) {
			saliente++;
			goto rigth;
		}

		// Top
		check.x = filled.x;
		check.y = filled.y + 1;
		if (img.In(check.x, check.y) && (img(check.x, check.y) == color))
		{
		top:
			img(check.x, check.y) = seeds;
			queue.push_back(check);
		}
		else if (img.In(check.x, check.y) && vecino > 0) {
			saliente++;
			goto top;
		}

		// Bot
		check.x = filled.x;
		check.y = filled.y - 1;
		if (img.In(check.x, check.y) && (img(check.x, check.y) == color))
		{
		bot:
			img(check.x, check.y) = seeds;
			queue.push_back(check);
		}
		else if (img.In(check.x, check.y) && vecino > 0) {
			saliente++;
			goto bot;
		}

		if (saliente > 0) {
			vecino -= saliente;
			saliente = 0;
		}
	}
	return cont;
}

void Seeds(long umbral, long vecinos, long seeds, long color) {
	IndexT row, col;
	auto a_seeds = seeds;

	for (row = img.FirstRow(); row <= img.LastRow(); row++)
		for (col = img.FirstCol(); col <= img.LastCol(); col++) {
			if (a_sobel(row, col) < umbral) img(row, col) = color;
		}

	int asdf = 0;

	for (row = img.FirstRow(); row <= img.LastRow(); row++)
		for (col = img.FirstCol(); col <= img.LastCol(); col++) {
			if (img(row, col) == color) {
				asdf = FloodFill(row, col, vecinos, a_seeds, color);
				if (asdf > 250) a_seeds++;
				else FloodFill(row, col, vecinos, 0, a_seeds);

				if (a_seeds >= 255) a_seeds = seeds;
			}
		}
	img.WriteBMP("MisEjemplos/AluminaSEEDS.bmp");
	C_Image prueba;
	prueba.ReadBMP("MisEjemplos/AluminaSEEDS.bmp");
	prueba.palette.Read("PaletaSurtida256.txt");
	prueba.WriteBMP("MisEjemplos/AluminaSEEDS.bmp");
	prueba.Free();
}

void WaterShed(long umbral, long vecinos = 0, long seeds = 10, long color = 255) {
	IndexT row, col;

	//Primera parte, generacion de semillas
	Sobel();
	Seeds(umbral, vecinos, seeds, color);

	//Segunda parte, inundacion
	auto limite = umbral;

	while (limite < 255) {
		for (row = img.FirstRow() + 1; row <= img.LastRow() - 1; row++)
			for (col = img.FirstCol() + 1; col <= img.LastCol() - 1; col++) {
				if (img(row, col) > 0)
					for (int i = -1; i <= 1; i++)
						for (int j = -1; j <= 1; j++)
							if (img(row + i, col + j) == 0 && a_sobel(row, col) <= limite)
								img(row + i, col + j) = img(row,col);
			}
		limite++;
	}
}

int main(int argc, char **argv)
{
	C_Image read;
	a.ReadBMP("MisEjemplos/Ajedrez_Gris.bmp");
	read.ReadBMP("MisEjemplos/Ajedrez_Gris.bmp");

	img.Resize(a.FirstRow(),a.LastRow(),a.FirstCol(),a.LastCol(),0);

	a.Grey();

	//read.MedianFilter(a, 3);

	C_Matrix matriz2(-1, 1, -1, 1);
	matriz2.Gaussian((float)0.2);
	matriz2.DivideEscalar(matriz2.Sum());
	a.Convolution(read, matriz2);
	read.Free();

	WaterShed(10, 0);

	img.palette.Read("PaletaSurtida256.txt");

	C_Image sob(a_sobel);
	sob.WriteBMP("MisEjemplos/AluminaSOB.bmp");
	img.WriteBMP("MisEjemplos/AluminaWAT.bmp");

	return 0;
}