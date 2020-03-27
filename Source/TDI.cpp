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

void Flood(C_Image a, IndexT x, IndexT y, float umbral) {
	IndexT row, col;
	auto aux = 0;

	for (row = x; row <= a.LastRow(); row++)
		for (col = y; col <= a.LastCol(); col++) {
			if (mat(row, col) > 0) continue;

			aux = a(row, col);
			if (aux >= umbral && aux < 255) mat(row, col) = umbral; //255 Cambia por la probavilidad de ser un pixer de fondo
			else mat(row, col) = 0;
		}
}

int main(int argc, char **argv)
{
	IndexT row, col;
	C_Image a;
	std::map<long, std::map<IndexT, IndexT>> map;

	auto aux = 0;

	a.ReadBMP("MisEjemplos/Dados.bmp");
	a.Grey();

	map = Get_Minimos(a);
	mat.Resize(a.FirstRow(),a.LastRow(),a.FirstCol(),a.LastCol(),0);

	Flood(a,(map.begin()->second).begin()->first,
		(map.begin()->second).begin()->second,(map.begin()->first));

	C_Image b(mat);

	b.WriteBMP("MisEjemplos/Dados_Negative1.bmp");

	return 0;
}