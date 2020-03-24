#include <stdio.h>
#include <stdlib.h>  
#include <iostream>

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

/*Historigrama de Otsu, Binariazacion de imagen y calculo del valor umbral*/
int Get_Historigrama(unsigned int* historigrama, C_Image a) {
	C_Image::IndexT row, col;
	int sum = 0;

	/*Definicion del historigrama*/
	for (int i = 0; i < GREY_SCALE; i++) historigrama[i] = 0;

	for (row = a.FirstRow(); row <= a.LastRow(); row++)
		for (col = a.FirstCol(); col <= a.LastCol(); col++)
			historigrama[(int)a(row, col)]++;

	/*Calculo de valor umbral mediante varianza --> Prueba*/
	for (int i = 0; i < GREY_SCALE; i++) sum += i*historigrama[i];
	
	return 0;
}

int main(int argc, char **argv)
{
	C_Image::IndexT row, col;
	C_Image a;
	unsigned int* historigrama = (unsigned int*)malloc(sizeof(unsigned int) * GREY_SCALE);
	//Linea de prueba
	a.ReadBMP("Dados.bmp");
	a.Grey();
	
	Get_Historigrama(historigrama, a);

	for (row = a.FirstRow(); row <= a.LastRow(); row++)
		for (col = a.FirstCol(); col <= a.LastCol(); col++)
			if (a(row, col)  < 255) a(row, col) = 0;
			
	a.WriteBMP("Dados_Negative.bmp");

	return 0;
}