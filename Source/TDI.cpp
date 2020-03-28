#include <stdio.h>
#include <stdlib.h>  
#include <iostream>
#include <algorithm>
#include <vector>
#include <map>

#include <math.h>
#include <fcntl.h>

#include <time.h>

#include <C_General.hpp>
#include <C_Trace.hpp>
#include <C_File.hpp>
#include <C_Arguments.hpp>
#include <C_Matrix.hpp>
#include <C_Image.hpp>

int Test(int argc, char **argv);
int GREY_SCALE = 256;
C_Matrix mat(0, 0, 0, 0, 0);
typedef long IndexT;

/*Historigrama futura implementacion del calculo de valor humbral optimo*/
int Get_Historigrama(unsigned int* historigrama, C_Image a) {
	IndexT row, col;
	int sum = 0;
	float varianza = 0.0;
	float dtipica = 0.0;

	/*Definicion del historigrama*/
	for (int i = 0; i < GREY_SCALE; i++) historigrama[i] = 0;

	for (row = a.FirstRow(); row <= a.LastRow(); row++)
		for (col = a.FirstCol(); col <= a.LastCol(); col++)
			historigrama[(int)a(row, col)]++;
	
	return 0;
}

/*Calculo de los puntos minimos de la imagen*/
std::map<long, std::map<IndexT,IndexT>> Get_Minimos(C_Image a) {
	std::map<long, std::map<IndexT, IndexT>> map;
	IndexT row, col;

	for (row = a.FirstRow(); row <= a.LastRow(); row++)
		for (col = a.FirstCol(); col <= a.LastCol(); col++)
			map[a(row, col)][row] = col;

	return map;
}

void Flood(C_Image a, IndexT x, IndexT y, float vasija) {
	IndexT row, col;
	auto aux = 0;
	auto limite = 100;

	if (!mat(x, y) > 0) {
		//SE
		for (row = x; row <= a.LastRow() - 1; row++)
			for (col = y; col <= a.LastCol() - 1; col++) {
				aux = a(row - 1, col - 1);
				
				if (a(row, col) - aux > 30) {
					mat(row, col) = 255;
					break;
				}

				if (a(row, col) - vasija <= limite) mat(row, col) = 0;
			}

		//SW
		for (row = x; row <= a.LastRow() - 1; row++)
			for (col = y; col >= a.FirstCol() + 1; col--) {
				aux = a(row - 1, col + 1);
				
				if (a(row, col) - aux > 30) {
					mat(row, col) = 255;
					break;
				}
				if (a(row, col) - vasija <= limite) mat(row, col) = 0;
			}

		//NE
		for (row = x; row >= a.FirstCol() + 1; row--)
			for (col = y; col <= a.LastCol() - 1; col++) {
				aux = a(row + 1, col - 1);
				
				if (a(row, col) - aux > 30) {
					mat(row, col) = 255;
					break;
				}
				if (a(row, col) - vasija <= limite) mat(row, col) = 0;
			}
		//NW
		for (row = x; row >= a.FirstCol() + 1; row--)
			for (col = y; col >= a.FirstCol() + 1; col--) {
				aux = a(row + 1, col + 1);
				
				if (a(row, col) - aux > 30) {
					mat(row, col) = 255;
					break;
				}
				if (a(row, col) - vasija <= limite) mat(row, col) = 0;
			}
	}
}

int main(int argc, char **argv)
{
	IndexT row, col;
	C_Image a;
	std::map<long, std::map<IndexT, IndexT>> map;

	auto aux = 0;

	a.ReadBMP("MisEjemplos/Hercules.bmp");
	a.Grey();

	map = Get_Minimos(a);
	mat.Resize(a.FirstRow(),a.LastRow(),a.FirstCol(),a.LastCol(),0);

	for (auto x : map) {
		if (x.first > 100) break;
		for (auto y : x.second) {
			Flood(a, y.first, y.second, x.first);
		}
	}

	C_Image b(mat);

	b.WriteBMP("MisEjemplos/Hercules1.bmp");

	return 0;
}