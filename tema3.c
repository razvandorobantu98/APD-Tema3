//Dorobantu Razvan-Florin, 335CB

#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

char type[2];
int width, height;
int max_value;
unsigned char **matrix_R, **matrix_G, **matrix_B, **matrix_BW;
unsigned char **new_R, **new_G, **new_B, **new_BW;
int start_H, end_H;

//functie care citeste imaginea
void ReadImage(char file[]) {
	FILE *fid;
	fid = fopen(file, "r");

	fscanf(fid, "%s\n", type);
	fscanf(fid, "# Created by GIMP version 2.10.14 PNM plug-in\n");
	fscanf(fid, "%d %d\n", &width, &height);
	fscanf(fid, "%d\n", &max_value);

	//daca imaginea e color
	if(type[0] == 'P' && type[1] == '6') {

		matrix_R = calloc(height, sizeof(unsigned char *));
		matrix_G = calloc(height, sizeof(unsigned char *));
		matrix_B = calloc(height, sizeof(unsigned char *));
		for(int i = 0; i < height; i++) {
			matrix_R[i] = calloc(width, sizeof(unsigned char));
			matrix_G[i] = calloc(width, sizeof(unsigned char));
			matrix_B[i] = calloc(width, sizeof(unsigned char));
		}
		for(int i = 0; i < height; i++) {
			for(int j = 0; j < width; j++) {
				fscanf(fid, "%c", &matrix_R[i][j]);
				fscanf(fid, "%c", &matrix_G[i][j]);
				fscanf(fid, "%c", &matrix_B[i][j]);
			}
		}
	}
	//daca imaginea e alb-negru
	else if(type[0] == 'P' && type[1] == '5') {
		
		matrix_BW = calloc(height, sizeof(unsigned char *));
		for(int i = 0; i < height; i++) {
			matrix_BW[i] = calloc(width, sizeof(unsigned char));
		}
		for(int i = 0; i < height; i++) {
			for(int j = 0; j < width; j++) {
				fscanf(fid, "%c", &matrix_BW[i][j]);
			}
		}
	}

	fclose(fid);
}

//functie care scrie imaginea
void PrintImage(char file[])
{
	FILE *fid;
	fid = fopen(file, "w");
	
	//daca imaginea e color
	if(type[0] == 'P' && type[1] == '6') {
		fprintf(fid, "%s\n", type);
		fprintf(fid, "%d %d\n", width, height);
		fprintf(fid, "%d\n", max_value);
		for(int i = 0; i < height; i++) {
			for(int j = 0; j < width; j++) {
				fprintf(fid, "%c", matrix_R[i][j]);
				fprintf(fid, "%c", matrix_G[i][j]);
				fprintf(fid, "%c", matrix_B[i][j]);
			}
		}
	}
	//daca imaginea e alb-negru
	else if(type[0] == 'P' && type[1] == '5') {
		fprintf(fid, "%s\n", type);
		fprintf(fid, "%d %d\n", width, height);
		fprintf(fid, "%d\n", max_value);
		for(int i = 0; i < height; i++) {
			for(int j = 0; j < width; j++) {
				fprintf(fid, "%c", matrix_BW[i][j]);
			}
		}
	}

	fclose(fid);
}

//functia care aplica un filtru
void Transform(float K[3][3]) {

	//daca imaginea e color
	if(type[0] == 'P' && type[1] == '6') {
		
		for(int i = start_H; i < end_H; i++) {
			for(int j = 0; j < width; j++) {

				//calculeaza noile valoari ale pixelului
				float val_R = 0, val_G = 0, val_B = 0;

				if(i - 1 >= 0 && j - 1 >= 0) {
					val_R += matrix_R[i - 1][j - 1] * K[2][2];
					val_G += matrix_G[i - 1][j - 1] * K[2][2];
					val_B += matrix_B[i - 1][j - 1] * K[2][2];
				}
				if(i - 1 >= 0) {
					val_R += matrix_R[i - 1][j] * K[2][1];
					val_G += matrix_G[i - 1][j] * K[2][1];
					val_B += matrix_B[i - 1][j] * K[2][1];
				}
				if(i - 1 >= 0 && j + 1 < width) {
					val_R += matrix_R[i - 1][j + 1] * K[2][0];
					val_G += matrix_G[i - 1][j + 1] * K[2][0];
					val_B += matrix_B[i - 1][j + 1] * K[2][0];
				}
				if(j - 1 >= 0) {
					val_R += matrix_R[i][j - 1] * K[1][2];
					val_G += matrix_G[i][j - 1] * K[1][2];
					val_B += matrix_B[i][j - 1] * K[1][2];
				}
				val_R += matrix_R[i][j] * K[1][1];
				val_G += matrix_G[i][j] * K[1][1];
				val_B += matrix_B[i][j] * K[1][1];
				if(j + 1 < width) {
					val_R += matrix_R[i][j + 1] * K[1][0];
					val_G += matrix_G[i][j + 1] * K[1][0];
					val_B += matrix_B[i][j + 1] * K[1][0];
				}
				if(i + 1 < height && j - 1 >= 0) {
					val_R += matrix_R[i + 1][j - 1] * K[0][2];
					val_G += matrix_G[i + 1][j - 1] * K[0][2];
					val_B += matrix_B[i + 1][j - 1] * K[0][2];
				}
				if(i + 1 < height) {
					val_R += matrix_R[i + 1][j] * K[0][1];
					val_G += matrix_G[i + 1][j] * K[0][1];
					val_B += matrix_B[i + 1][j] * K[0][1];
				}
				if(i + 1 < height && j + 1 < width) {
					val_R += matrix_R[i + 1][j + 1] * K[0][0];
					val_G += matrix_G[i + 1][j + 1] * K[0][0];
					val_B += matrix_B[i + 1][j + 1] * K[0][0];
				}

				//verifica sa nu depaseasca limitele
				if(val_R > 255)
					val_R = 255;
				else if(val_R < 0)
					val_R = 0;
				if(val_G > 255)
					val_G = 255;
				else if(val_G < 0)
					val_G = 0;
				if(val_B > 255)
					val_B = 255;
				else if(val_B < 0)
					val_B = 0;

				//retine valorile pixelilor in matrici auxiliare
				new_R[i][j] = (unsigned char)val_R;
				new_G[i][j] = (unsigned char)val_G;
				new_B[i][j] = (unsigned char)val_B;
			}
		}

		//scrie rezultatul in matricea originala
		for(int i = 0; i < height; i++) {
			for(int j = 0; j < width; j++) {
				matrix_R[i][j] = new_R[i][j];
				matrix_G[i][j] = new_G[i][j];
				matrix_B[i][j] = new_B[i][j];
			}
		}
	}
	//daca imaginea e alb-negru
	else if(type[0] == 'P' && type[1] == '5') {
		
		for(int i = start_H; i < end_H; i++) {
			for(int j = 0; j < width; j++) {

				float val_BW = 0;

				//calculeaza noile valoari ale pixelului
				if(i - 1 >= 0 && j - 1 >= 0) {
					val_BW += matrix_BW[i - 1][j - 1] * K[2][2];
				}
				if(i - 1 >= 0) {
					val_BW += matrix_BW[i - 1][j] * K[2][1];
				}
				if(i - 1 >= 0 && j + 1 < width) {
					val_BW += matrix_BW[i - 1][j + 1] * K[2][0];
				}
				if(j - 1 >= 0) {
					val_BW += matrix_BW[i][j - 1] * K[1][2];
				}
				val_BW += matrix_BW[i][j] * K[1][1];
				if(j + 1 < width) {
					val_BW += matrix_BW[i][j + 1] * K[1][0];
				}
				if(i + 1 < height && j - 1 >= 0) {
					val_BW += matrix_BW[i + 1][j - 1] * K[0][2];
				}
				if(i + 1 < height) {
					val_BW += matrix_BW[i + 1][j] * K[0][1];
				}
				if(i + 1 < height && j + 1 < width) {
					val_BW += matrix_BW[i + 1][j + 1] * K[0][0];
				}

				//verifica sa nu depaseasca limiteles
				if(val_BW > 255)
					val_BW = 255;
				else if(val_BW < 0)
					val_BW = 0;

				//retine valorile pixelilor in matrici auxiliare
				new_BW[i][j] = (unsigned char)val_BW;
			}
		}

		//scrie rezultatul in matricea originala
		for(int i = 0; i < height; i++) {
			for(int j = 0; j < width; j++) {
				matrix_BW[i][j] = new_BW[i][j];
			}
		}
	}

}

void Smoothing() {
	float K[3][3] = {{1.0 / 9.0, 1.0 / 9.0, 1.0 / 9.0}, 
			{1.0 / 9.0, 1.0 / 9.0, 1.0 / 9.0}, 
			{1.0 / 9.0, 1.0 / 9.0, 1.0 / 9.0}};
	Transform(K);
}

void Gaussian() {
	float K[3][3] = {{1.0 / 16.0, 2.0 / 16.0, 1.0 / 16.0}, 
			{2.0 / 16.0, 4.0 / 16.0, 2.0 / 16.0}, 
			{1.0 / 16.0, 2.0 / 16.0, 1.0 / 16.0}};
	Transform(K);
}

void Sharpen() {
	float K[3][3] = {{0.0 / 3.0, -2.0 / 3.0, 0.0 / 3.0}, 
			{-2.0 / 3.0, 11.0 / 3.0, -2.0 / 3.0}, 
			{0.0 / 3.0, -2.0 / 3.0, 0.0 / 3.0}};
	Transform(K);
}

void Mean() {
	float K[3][3] = {{-1.0, -1.0, -1.0}, 
			{-1.0, 9.0, -1.0}, 
			{-1.0, -1.0, -1.0}};
	Transform(K);
}

void Emboss() {
	float K[3][3] = {{0.0, 1.0, 0.0}, 
			{0.0, 0.0, 0.0}, 
			{0.0, -1.0, 0.0}};
	Transform(K);
}

int main(int argc, char * argv[])
{
	int rank;
    	int nProcesses;
    	MPI_Init(&argc, &argv);
    	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    	MPI_Comm_size(MPI_COMM_WORLD, &nProcesses);
    	MPI_Status stat;

	//procesul 0 citeste din fisieru de input
	if(rank == 0) {
		char *nume_in = argv[1];
		ReadImage(nume_in);
	}

	//procesul 0 face broadcast la dimensiunile si formatul imaginii
	MPI_Bcast(&height, 1, MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Bcast(&width, 1, MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Bcast(&max_value, 1, MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Bcast(&type, 2, MPI_CHAR, 0, MPI_COMM_WORLD);

	//celelalte procese aloca memorie pentru imagine
	if(rank != 0) {
		if(type[0] == 'P' && type[1] == '6') {
			matrix_R = calloc(height, sizeof(unsigned char *));
			matrix_G = calloc(height, sizeof(unsigned char *));
			matrix_B = calloc(height, sizeof(unsigned char *));
		
			for(int i = 0; i < height; i++) {
				matrix_R[i] = calloc(width, sizeof(unsigned char));
				matrix_G[i] = calloc(width, sizeof(unsigned char));
				matrix_B[i] = calloc(width, sizeof(unsigned char));
			}
		}
		else if(type[0] == 'P' && type[1] == '5') {
			matrix_BW = calloc(height, sizeof(unsigned char *));
		
			for(int i = 0; i < height; i++) {
				matrix_BW[i] = calloc(width, sizeof(unsigned char));
			}
		}
	}

	//fiecare proces aloca memorie pentru matricea in care se vor retine noile valori calculate
	if(type[0] == 'P' && type[1] == '6') {
		new_R = calloc(height, sizeof(unsigned char *));
		new_G = calloc(height, sizeof(unsigned char *));
		new_B = calloc(height, sizeof(unsigned char *));
		
		for(int i = 0; i < height; i++) {
			new_R[i] = calloc(width, sizeof(unsigned char));
			new_G[i] = calloc(width, sizeof(unsigned char));
			new_B[i] = calloc(width, sizeof(unsigned char));
		}
	}
	else if(type[0] == 'P' && type[1] == '5') {
		new_BW = calloc(height, sizeof(unsigned char *));
	
		for(int i = 0; i < height; i++) {
			new_BW[i] = calloc(width, sizeof(unsigned char));
		}
	}
	
	//procesul 0 face broadcast la valorile pixelilor din imagine
	if(type[0] == 'P' && type[1] == '6') {
		for(int i = 0; i < height; i++) {
			MPI_Bcast(matrix_R[i], width, MPI_CHAR, 0, MPI_COMM_WORLD);
			MPI_Bcast(matrix_G[i], width, MPI_CHAR, 0, MPI_COMM_WORLD);
			MPI_Bcast(matrix_B[i], width, MPI_CHAR, 0, MPI_COMM_WORLD);
		}
	}
	else if(type[0] == 'P' && type[1] == '5') {
		for(int i = 0; i < height; i++) {
			MPI_Bcast(matrix_BW[i], width, MPI_CHAR, 0, MPI_COMM_WORLD);
		}
	}

	//fiecare proces isi calculeaza ce bucata trebuie sa prelucreze din imagine
	start_H = ceil((double)height / nProcesses) * rank;
	end_H = fmin(ceil((double)height / nProcesses) * (rank + 1), height);
	
	//aplica filtrele
	for(int k = 3; k < argc; k++) {
	
		char *nume_filtru = argv[k];
		if(strcmp(nume_filtru, "smooth") == 0) {
			Smoothing();
		}
		else if(strcmp(nume_filtru, "blur") == 0) {
			Gaussian();
		}
		else if(strcmp(nume_filtru, "sharpen") == 0) {
			Sharpen();
		}
		else if(strcmp(nume_filtru, "mean") == 0) {
			Mean();
		}
		else if(strcmp(nume_filtru, "emboss") == 0) {
			Emboss();
		}

		//fiecare proces face broadcast cu valorile calculate
		if(type[0] == 'P' && type[1] == '6') {	
			for(int i = 0; i < nProcesses; i++)
			{
				int start = ceil((double)height / nProcesses) * i;
				int end = fmin(ceil((double)height / nProcesses) * (i + 1), height);
	
				for(int j = start; j < end; j++) {
					MPI_Bcast(matrix_R[j], width, MPI_CHAR, i, MPI_COMM_WORLD);
					MPI_Bcast(matrix_G[j], width, MPI_CHAR, i, MPI_COMM_WORLD);
					MPI_Bcast(matrix_B[j], width, MPI_CHAR, i, MPI_COMM_WORLD);
				}
			}
		}
		if(type[0] == 'P' && type[1] == '5') {
			for(int i = 0; i < nProcesses; i++)
			{
				int start = ceil((double)height / nProcesses) * i;
				int end = fmin(ceil((double)height / nProcesses) * (i + 1), height);
	
				for(int j = start; j < end; j++) {
					MPI_Bcast(matrix_BW[j], width, MPI_CHAR, i, MPI_COMM_WORLD);
				}
			}
		}
	}

	//procesul 0 scrie in fisierul de output
	if(rank == 0) {
		char *nume_out = argv[2];
		PrintImage(nume_out);
	}

	//fiecare proces elibereaza memoria
	if(type[0] == 'P' && type[1] == '6') {
		for(int i = 0; i < height; i++) {
			free(matrix_R[i]);
			free(matrix_G[i]);
			free(matrix_B[i]);
			free(new_R[i]);
			free(new_G[i]);
			free(new_B[i]);
		}
		free(matrix_R);
		free(matrix_G);
		free(matrix_B);
		free(new_R);
		free(new_G);
		free(new_B);
	}
	else if(type[0] == 'P' && type[1] == '5') {
		for(int i = 0; i < height; i++) {
			free(matrix_BW[i]);
			free(new_BW[i]);
		}
		free(matrix_BW);
		free(new_BW);
	}

	MPI_Finalize();

        return 0;
}
