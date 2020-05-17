#include <stdio.h>
#include <stdlib.h>  
#include <iostream>
#include <vector>

#include <math.h>

#include <C_General.hpp>
#include <C_Matrix.hpp>
#include <C_Image.hpp>

C_Image img(0, 0, 0, 0, 0);
C_Image a;
C_Image read;
C_Matrix a_sobel;

typedef long IndexT;
typedef double ElementT;

struct Point 
{
	IndexT x = -1;
	IndexT y = -1;
};

void Sobel() {
	IndexT row, col;
	a_sobel = a;

	int mx[3][3] = { {-1, 0, 1}, {-2, 0, 2}, {-1, 0, 1}};
	int my[3][3] = { {-1, -2, -1}, {0, 0, 0}, {1, 2, 1}};
	
	double gx = 0;
	double gy = 0;
	double g = 0;

	for (row = a.FirstRow()+1; row <= a.LastRow()-1; row++)
		for (col = a.FirstCol()+1; col <= a.LastCol()-1; col++) {
			gx = (a(row - 1, col + 1) + (2 * a(row, col + 1)) + (a(row, col + 1))) + (-a(row - 1, col - 1) + (-2 * a(row, col - 1)) + (-a(row + 1, col - 1)));
			gy = (a(row + 1, col - 1) + (2 * a(row + 1, col)) + (a(row + 1, col + 1))) + (-a(row - 1, col -1) + (-2 * a(row - 1, col)) + (-a(row - 1, col + 1)));
			g = sqrt(pow(gx,2) + pow (gx, 2));
			a_sobel(row, col) = (g < 0) ? 0 : ((g > 255)? 255 : g);
			//a_sobel(row, col) = g;
		}

	//for (row = a.FirstRow() + 1; row <= a.LastRow() - 1; row++)
	//	for (col = a.FirstCol() + 1; col <= a.LastCol() - 1; col++) {
	//		for(int x = -1; x < 2; x++)
	//			for (int y = -1; y < 2; y++) {
	//				gx += mx[x + 1][y + 1] * a(row + x, col + y);
	//				gy += my[x + 1][y + 1] * a(row + x, col + y);
	//			}
	//		g = sqrt(pow(gx, 2) + pow(gx, 2));
	//		a_sobel(row, col) = (g < 0) ? 0 : ((g > 255)? 255 : g);
	//		gx = 0;
	//		gy = 0;
	//	}

	//a.Free();

}

int FloodFill(IndexT x, IndexT y, long vecinos, long color_seeds, long previus_color)
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
	img(x, y) = color_seeds;

	while (!queue.empty())
	{
		Point filled = queue.back();
		queue.pop_back();
		cont++;

		// Left
		check.x = filled.x - 1;
		check.y = filled.y;
		if (img.In(check.x, check.y) && (img(check.x, check.y) == previus_color))
		{
		left:
			img(check.x, check.y) = color_seeds;
			queue.push_back(check);
		}
		else if (img.In(check.x, check.y) && vecino > 0) {
			saliente ++;
			goto left;
		}

		// Right
		check.x = filled.x + 1;
		check.y = filled.y;
		if (img.In(check.x, check.y) && (img(check.x, check.y) == previus_color))
		{
		rigth:
			img(check.x, check.y) = color_seeds;
			queue.push_back(check);
		}
		else if (img.In(check.x, check.y) && vecino > 0) {
			saliente++;
			goto rigth;
		}

		// Top
		check.x = filled.x;
		check.y = filled.y + 1;
		if (img.In(check.x, check.y) && (img(check.x, check.y) == previus_color))
		{
		top:
			img(check.x, check.y) = color_seeds;
			queue.push_back(check);
		}
		else if (img.In(check.x, check.y) && vecino > 0) {
			saliente++;
			goto top;
		}

		// Bot
		check.x = filled.x;
		check.y = filled.y - 1;
		if (img.In(check.x, check.y) && (img(check.x, check.y) == previus_color))
		{
		bot:
			img(check.x, check.y) = color_seeds;
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

void Seeds(long umbral, long vecinos, long size_seed, long color_seeds, long previus_color) {
	IndexT row, col;
	auto a_seeds = color_seeds;

	for (row = img.FirstRow(); row <= img.LastRow(); row++)
		for (col = img.FirstCol(); col <= img.LastCol(); col++) {
			if (a_sobel(row, col) < umbral) img(row, col) = previus_color;
		}

	int asdf = 0;
	img.WriteBMP("MisEjemplos/SEEDS_BN.bmp");

	for (row = img.FirstRow(); row <= img.LastRow(); row++)
		for (col = img.FirstCol(); col <= img.LastCol(); col++) {
			if (img(row, col) == previus_color) {
				asdf = FloodFill(row, col, vecinos, a_seeds, previus_color);
				if (asdf > size_seed) a_seeds++;
				else FloodFill(row, col, vecinos, 0, a_seeds);

				if (a_seeds >= 255) a_seeds = color_seeds;
			}
		}
	img.WriteBMP("MisEjemplos/SEEDS_CL.bmp");
	C_Image prueba;
	prueba.ReadBMP("MisEjemplos/SEEDS_CL.bmp");
	prueba.palette.Read("PaletaSurtida256.txt");
	prueba.WriteBMP("MisEjemplos/SEEDS_CL.bmp");
	prueba.Free();
}

void WaterShed(long umbral, long vecinos = 0, long size_seed = 0,long color_seeds = 10, long previus_color = 255) {
	IndexT row, col;

	//Primera parte, generacion de semillas
	Sobel();
	Seeds(umbral, vecinos, size_seed, color_seeds, previus_color);

	//Segunda parte, inundacion
	auto limite = umbral;

	while (limite < 256) {
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

void Resta() {
	IndexT row, col;
	int historigram[256];
	auto max = 0;
	auto aux = 0;

	for (int x = 0; x < 256; x++) historigram[x] = 0;

	for (row = img.FirstRow(); row <= img.LastRow(); row++)
		for (col = img.FirstCol(); col <= img.LastCol(); col++) {
			historigram[(int)img(row, col)]++;
		}

	for (int x = 0; x < 256; x++)
		if (max < historigram[x]) {
			aux = x;
			max = historigram[x];
		}

	for (row = img.FirstRow(); row <= img.LastRow(); row++)
		for (col = img.FirstCol(); col <= img.LastCol(); col++)
			if (img(row, col) == aux) read(row, col) = a(row,col);

}


int main(int argc, char **argv)
{
	a.ReadBMP("MisEjemplos/Valve.bmp");
	read.ReadBMP("MisEjemplos/Valve.bmp");

	img.Resize(a.FirstRow(),a.LastRow(),a.FirstCol(),a.LastCol(),0);

	a.Grey();

	//Aplicacion de filtros
	a.MedianFilter(a, 3);
	//C_Matrix matriz2(-1, 1, -1, 1);
	//matriz2.Gaussian((float)0.4);
	//matriz2.DivideEscalar(matriz2.Sum());
	//a.Convolution(read, matriz2);

	//Aplicacion de algoritmo WaterShed
	WaterShed(25, 0, 100);

	//Aplicacion de emborronado
	Resta();

	//Impresion de las imagenes
	img.palette.Read("PaletaSurtida256.txt");

	C_Image sob(a_sobel);
	sob.WriteBMP("MisEjemplos/SOB.bmp");
	img.WriteBMP("MisEjemplos/WAT.bmp");
	read.WriteBMP("MisEjemplos/SALIDA.bmp");
	return 0;
}