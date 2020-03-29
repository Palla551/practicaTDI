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
C_Matrix mat(0, 0, 0, 0, 0);
C_Image a;
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

/*Calculo de los puntos minimos de la imagen*/
map<ElementT, map<IndexT,IndexT>> Get_Minimos(C_Image a) {
	map<ElementT, map<IndexT, IndexT>> map;
	IndexT row, col;

	for (row = a.FirstRow(); row <= a.LastRow(); row++)
		for (col = a.FirstCol(); col <= a.LastCol(); col++)
			map[a(row, col)][row] = col;

	return map;
}

void Binarizacion() {
	IndexT row, col;
	auto aux = 0;
	//Ajustar limite || REVISAR
	aux = Get_Limite();

	//Binarizacion de la imagen
	for (row = a.FirstRow(); row <= a.LastRow(); row++)
		for (col = a.FirstCol(); col <= a.LastCol(); col++)
			if (a(row, col) > aux) a(row, col) = 255;
			else a(row, col) = 0;

}

/*Algoritmo de llenado, version lenta sin limites automaticos ni marcadores
Posible mejora, introduccion de hilos para agilizar el calculo de forma paralela*/

void FloodHilos(double x, double y, double limite, int numHilo, bool hilos = false) {
	if (mat.In(x,y) && !(mat(x, y) > 0)) {
		IndexT row, col;
		ElementT aux = 0;

		//SE
		if(numHilo == 0 || !hilos)
		for (row = x; row <= a.LastRow() - 1; row++)
			for (col = y; col <= a.LastCol() - 1; col++) {
				if (!mat.In(x - 1, y - 1)) break;

				aux = a(row - 1, col - 1);
				if (a(row, col) - aux > limite) {
					mat(row, col) = 255;
					break;
				}
			}

		//SW
		if (numHilo == 1 || !hilos)
		for (row = x; row <= a.LastRow() - 1; row++)
			for (col = y; col >= a.FirstCol() + 1; col--) {
				if (!mat.In(x - 1, y + 1)) break;
				
				aux = a(row - 1, col + 1);
				if (a(row, col) - aux > limite) {
					mat(row, col) = 255;
					break;
				}
			}

		//NE
		if (numHilo == 2 || !hilos)
		for (row = x; row >= a.FirstRow() + 1; row--)
			for (col = y; col <= a.LastCol() - 1; col++) {
				if (!mat.In(x + 1, y - 1)) break;
				
				aux = a(row + 1, col - 1);
				if (a(row, col) - aux > limite) {
					mat(row, col) = 255;
					break;
				}
			}
		//NW
		if (numHilo == 3 || !hilos)
		for (row = x; row >= a.FirstRow() + 1; row--)
			for (col = y; col >= a.FirstCol() + 1; col--) {
				if (!mat.In(x + 1, y + 1)) break;
				
				aux = a(row + 1, col + 1);
				if (a(row, col) - aux > limite) {
					mat(row, col) = 255;
					break;
				}
			}
	}
}

void Flood(bool hilos) {
	thread hilo0, hilo1, hilo2, hilo3;
	map<ElementT, std::map<IndexT, IndexT>> map;

	Binarizacion();
	map = Get_Minimos(a);

	//METODO DE LLENADO HILOS REVISAR WARNINGS DE CONVERSION DE TIPO
	if (hilos)
		for (auto x : map) {
			for (auto y : x.second) {
				hilo0 = thread(FloodHilos, y.first, y.second, 10, 0, true);
				hilo1 = thread(FloodHilos, y.first, y.second, 10, 1, true);
				hilo2 = thread(FloodHilos, y.first, y.second, 10, 2, true);
				hilo3 = thread(FloodHilos, y.first, y.second, 10, 3, true);

				hilo0.join();
				hilo1.join();
				hilo2.join();
				hilo3.join();
			}
		}

	else
		for (auto x : map)
			for (auto y : x.second)
				FloodHilos(y.first, y.second, 10, -1);
}

int main(int argc, char **argv)
{
	a.ReadBMP("MisEjemplos/Ajedrez_Gris.bmp");
	mat.Resize(a.FirstRow(),a.LastRow(),a.FirstCol(),a.LastCol(),0);
	a.Grey();

	Flood(false);
	
	C_Image b(mat);
	b.WriteBMP("MisEjemplos/Ajedrez_Gris1.bmp");

	return 0;
}