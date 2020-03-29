#include <stdio.h>
#include <stdlib.h>  
#include <iostream>
#include <vector>
#include <map>

#include <math.h>

#include <C_General.hpp>
#include <C_Matrix.hpp>
#include <C_Image.hpp>

int Test(int argc, char **argv);
int GREY_SCALE = 256;
C_Matrix mat(0, 0, 0, 0, 0);
typedef long IndexT;
typedef double ElementT;

int Get_Limite(C_Image a) {
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
std::map<ElementT, std::map<IndexT,IndexT>> Get_Minimos(C_Image a) {
	std::map<ElementT, std::map<IndexT, IndexT>> map;
	IndexT row, col;

	for (row = a.FirstRow(); row <= a.LastRow(); row++)
		for (col = a.FirstCol(); col <= a.LastCol(); col++)
			map[a(row, col)][row] = col;

	return map;
}

/*Algoritmo de llenado, version lenta sin limites automaticos ni marcadores
Posible mejora, introduccion de hilos para agilizar el calculo de forma paralela*/
void Flood(C_Image a, IndexT x, IndexT y, ElementT limite) {
	if (mat.In(x,y) && !(mat(x, y) > 0)) {
		IndexT row, col;
		C_Matrix::ElementT aux = 0;
		//SE
		for (row = x; row <= a.LastRow() - 1; row++)
			for (col = y; col <= a.LastCol() - 1; col++) {
				if (!mat.In(x - 1, y - 1)) break;
				aux = a(row - 1, col - 1);
				if (a(row, col) - aux > limite) {
					mat(row, col) = 255;
					break;
				}
			}
		
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

int main(int argc, char **argv)
{
	IndexT row, col;
	C_Image a;
	C_Image destino;
	std::map<ElementT, std::map<IndexT, IndexT>> map;
	auto aux = 0;

	a.ReadBMP("MisEjemplos/Aguadulce_Gris.bmp");
	a.Grey();

	//Ajustar limite || REVISAR
	aux = Get_Limite(a);
	
	//Binarizacion de la imagen
	for (row = a.FirstRow(); row <= a.LastRow(); row++)
		for (col = a.FirstCol(); col <= a.LastCol(); col++)
			if (a(row, col) > aux) a(row, col) = 255;
			else a(row, col) = 0;

	//CALCULO DE PUNTOS MINIMOS MEDIANTE FIFO
	map = Get_Minimos(a);
	mat.Resize(a.FirstRow(),a.LastRow(),a.FirstCol(),a.LastCol(),0);
	
	//METODO DE LLENADO
	for (auto x : map) {
		for (auto y : x.second) {
			Flood(a, y.first, y.second, 10);
		}
	}

	C_Image b(mat);
	b.WriteBMP("MisEjemplos/Aguadulce_Gris1.bmp");

	return 0;
}