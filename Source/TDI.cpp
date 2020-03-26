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

/*Historigrama futura implementacion del calculo de valor humbral optimo*/
int Get_Historigrama(unsigned int* historigrama, C_Image a) {
	C_Image::IndexT row, col;
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
std::map<long, std::map<C_Image::IndexT, C_Image::IndexT>> Get_minimos(C_Image a) {
	std::map<long, std::map<C_Image::IndexT, C_Image::IndexT>> map;
	C_Image::IndexT row, col;

	for (row = a.FirstRow(); row <= a.LastRow(); row++)
		for (col = a.FirstCol(); col <= a.LastCol(); col++)
			map[a(row, col)][row] = col;

	return map;
}



C_Image Watersheed(C_Image a) {
	return a;
}

int main(int argc, char **argv)
{
	C_Image::IndexT row, col;
	C_Image a;
	float umbral = 0;
	auto minimo = 0;
	auto aux = 0;

	a.ReadBMP("MisEjemplos/Dados.bmp");
	a.Grey();
	minimo = Get_minimos(a).begin() -> first;

	for (row = a.FirstRow(); row <= a.LastRow(); row++)
		for (col = a.FirstCol(); col <= a.LastCol(); col++) {
			aux = a(row, col);
			if (aux <= minimo) a(row, col) = 255;
		}

	a.WriteBMP("MisEjemplos/Dados_Negative.bmp");

	return 0;
}