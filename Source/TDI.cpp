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

C_Matrix Flood(C_Image a, C_Matrix mat, IndexT x, IndexT y, float umbral) {
	if (!mat.In(x, y)) return mat;
	if (mat(x, y) > 0) return mat;
	if (a(x, y) <= umbral) {
		Flood(a, mat, x - 1, y - 1, umbral);
		/*Flood(a, mat, x - 1, y, umbral + 1);
		Flood(a, mat, x - 1, y + 1,umbral + 1);
		Flood(a, mat, x, y - 1,umbral + 1);
		Flood(a, mat, x, y + 1,umbral + 1);
		Flood(a, mat, x + 1, y - 1,umbral + 1);
		Flood(a, mat, x + 1, y,umbral + 1);
		Flood(a, mat, x + 1, y + 1, umbral + 1);*/
	}
	mat(x, y) = 255;
	return mat;
}

int main(int argc, char **argv)
{
	IndexT row, col;
	C_Image a;
	C_Matrix mat(0,0,0,0,0);
	std::map<long, std::map<IndexT, IndexT>> map;

	auto aux = 0;

	a.ReadBMP("MisEjemplos/Dados.bmp");
	a.Grey();

	map = Get_Minimos(a);
	mat.Resize(a.FirstRow(),a.LastRow(),a.FirstCol(),a.LastCol(),0);

	Flood(a,
		mat,
		(map.begin()->second).begin()->first,
		(map.begin()->second).begin()->second,
		(map.begin()->first));

	a.WriteBMP("MisEjemplos/Dados_Negative1.bmp");

	return 0;
}