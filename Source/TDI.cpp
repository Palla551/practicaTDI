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
C_Image mat(0, 0, 0, 0, 0);
C_Image img(0, 0, 0, 0, 0);
C_Image a;
C_Matrix a_sobel;
typedef long IndexT;
typedef double ElementT;

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
	auto gx = 0;
	auto gy = 0;
	auto g = 0;

	for (row = a.FirstRow()+1; row <= a.LastRow()-1; row++)
		for (col = a.FirstCol()+1; col <= a.LastCol()-1; col++) {
			gx = (a(row - 1, col + 1) + (2 * a(row, col + 1)) + (a(row, col))) - (a(row - 1, col - 1) + (2 * a(row, col - 1)) + (a(row + 1, col - 1)));
			gy = (a(row + 1, col - 1) + (2 * a(row, col - 1)) + (a(row - 1, col + 1))) - (a(row - 1, col -1) + (2 * a(row - 1, col)) + (a(row - 1, col + 1)));
			g = sqrt(pow(gx,2) + pow (gx, 2));
			a_sobel(row, col) = (g < limite) ? 0 : ((g >= limite)? 255 : g);
			//a_sobel(row, col) = g;
		}

}

/*Calculo de los puntos minimos de la imagen*/
map<ElementT, map<IndexT,IndexT>> Get_Minimos(C_Image a) {
	map<ElementT, map<IndexT, IndexT>> map;
	IndexT row, col;

	for (row = a.FirstRow(); row <= a.LastRow(); row++)
		for (col = a.FirstCol(); col <= a.LastCol(); col++)
			map[a(row, col)][row] = col;

	return map;
}

void Flood(long x, long y) {
	IndexT row, col;
	ElementT aux = 0;
	auto r = (rand() % 200) + 10;
	auto g = (rand() % 200) + 10;
	auto b = (rand() % 200) + 10;

	//SE
	for (row = x; row <= a.LastRow() - 1; row++)
		for (col = y; col <= a.LastCol() - 1; col++) {
			if (a_sobel(row, col) == 255) break;
			aux = a(row + 1, col + 1);
			if (abs(a(row, col) - aux) < 10 && mat(row, col) == 0) {
				img(row, col) = img.palette(1, C_RED) = r;
				img(row, col) = img.palette(1, C_GREEN) = g;
				img(row, col) = img.palette(1, C_BLUE) = b;

				mat(row, col) = 10;
			}
		}

	//SW
	for (row = x; row <= a.LastRow() - 1; row++)
		for (col = y; col >= a.FirstCol() + 1; col--) {
			if (a_sobel(row, col) == 255) break;
			aux = a(row + 1, col - 1);
			if (abs(a(row, col) - aux) < 10 && mat(row, col) == 0) {
				img(row, col) = img.palette(1, C_RED) = r;
				img(row, col) = img.palette(1, C_GREEN) = g;
				img(row, col) = img.palette(1, C_BLUE) = b;

				mat(row, col) = 10;
			}
		}

	//NE
	for (row = x; row >= a.FirstRow() + 1; row--)
		for (col = y; col <= a.LastCol() - 1; col++) {
			if (a_sobel(row, col) == 255) break;
			aux = a(row - 1, col + 1);
			if (abs(a(row, col) - aux) < 10 && mat(row, col) == 0) {
				img(row, col) = img.palette(1, C_RED) = r;
				img(row, col) = img.palette(1, C_GREEN) = g;
				img(row, col) = img.palette(1, C_BLUE) = b;

				mat(row, col) = 10;
			}
		}
	//NW
	for (row = x; row >= a.FirstRow() + 1; row--)
		for (col = y; col >= a.FirstCol() + 1; col--) {
			if (a_sobel(row, col) == 255) break;
			aux = a(row - 1, col - 1);
			if (abs(a(row, col) - aux) < 10 && mat(row, col) == 0) {
				img(row, col) = img.palette(1, C_RED) = r;
				img(row, col) = img.palette(1, C_GREEN) = g;
				img(row, col) = img.palette(1, C_BLUE) = b;

				mat(row, col) = 10;
			}
		}

}

void WaterSheed() {
	IndexT row, col;
	thread hilo0, hilo1, hilo2, hilo3;
	map<ElementT, std::map<IndexT, IndexT>> map;

	Sobel(Get_Limite());

	for (row = a.FirstRow(); row <= a.LastRow(); row++)
		for (col = a.FirstCol(); col <= a.LastCol(); col++) {
			if (a_sobel(row, col) < 240 && mat(row, col) == 0) {
				Flood(row, col);
			}
		}

}

int main(int argc, char **argv)
{
	C_Image paleta;

	paleta.ReadBMP("MisEjemplos/Dados.bmp");
	a.ReadBMP("MisEjemplos/Alumina.bmp");

	mat.Resize(a.FirstRow(),a.LastRow(),a.FirstCol(),a.LastCol(),0);
	img.Resize(a.FirstRow(),a.LastRow(),a.FirstCol(),a.LastCol(),0);
	img.palette = paleta.palette;
	
	a.Grey();

	WaterSheed();

	//C_Image sob(a_sobel);
	//sob.WriteBMP("MisEjemplos/Alumina_SOB2.bmp");
	img.WriteBMP("MisEjemplos/Alumina_WAT.bmp");

	return 0;
}