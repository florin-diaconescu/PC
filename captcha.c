//Diaconescu Florin, 312CB, florin.diaconescu

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "bmp_header.h"
#define W 255

/*functie pentru incarcarea headerelor fisierului BMP trimis ca argument
si initializarea matricii de pixeli ai imaginii*/
unsigned char **LoadBMP(char *image_name, FileHeader *fileh, InfoHeader *infoh){
	//deschid fisierul de intrare si verific realizarea operatiei
	FILE *img = fopen(image_name, "rb");
	if (!img) exit(1);

	/*i si j sunt contoare pentru parcurgerea matricii, padding este
	folosit pentru a sari cu fseek pentru padding, daca latimea
	nu este multiplu de 4, iar Map este matricea de pixeli propriu-zisa*/
	int i, j, padding;
	unsigned char **Map;

	//citesc informatiile din headere
	fread(fileh, sizeof(FileHeader), 1, img);
	fread(infoh, sizeof(InfoHeader), 1, img);
	
	/*aloc dinamic matricea, verificand daca operatia s-a realizat cu
	succes, in caz contrar eliberand memoria alocata pana atunci*/
	Map = (unsigned char **)malloc(infoh->height * sizeof(unsigned char *));	
	if (!Map) exit(1);

	for (i = 0; i < infoh->height; i++){
		Map[i] = (unsigned char *)malloc(3 * infoh->width * sizeof(unsigned char));
		if (!Map[i]){
			for (j = 0; j < i; j++)
				free(Map[j]);
			free(Map);
			fclose(img);
			exit(1);
		}
	}

	//sar peste imageDataOffset pentru a prelucra pixelii imaginii
	fseek(img, fileh->imageDataOffset, SEEK_SET);

	padding = infoh->width % 4;

	/*citesc matricea linie cu linie, in sens invers, pentru ca la
	prelucrare sa o pot parcurge in sens "normal"*/
	for (i = infoh->height - 1; i >= 0; i--){
		fread(Map[i], 1, 3 * infoh->width, img);
		fseek(img, padding, SEEK_CUR);
	}

	//inchid fisierul de intrare si returnez matricea de pixli
	fclose(img);
	return Map;
}

/*functie pentru scrierea unei imagini in format BMP, pe baza headerelor, 
a matricii de pixeli si a padding-ului*/
void DumpBMP(char *task_name, FileHeader *fileh, InfoHeader *infoh, unsigned char **Map){
	//deschid fisierul de intrare si verific realizarea operatiei
	FILE *out = fopen(task_name, "wb");
	if (!out) exit(1);

	/*i este un contor folosit pentru parcurgerea matricii, padding este
	folosit pentru a vedea cati bytes de padding trebuie adaugati matricii
	de pixeli, iar p este un pointer folosit pentru inserarea de 0-uri
	pentru padding, care este alocat dinamic, operatie verificata ulterior*/
	int i, padding;
	unsigned char *p = calloc(3, 1);
	if (!p) exit(1);

	//scriu informatiile din headere
	fwrite(fileh, sizeof(FileHeader), 1, out);
	fwrite(infoh, sizeof(InfoHeader), 1, out);
	
	padding = infoh->width % 4;
	
	/*parcurg matricea si afisez pixelii in ordine inversa, intrucat citirea
	a fost facuta tot invers, pentru a fi mai usor de urmarit, insa acum
	trebuie intoarsa din nou, caracteristica a imaginilor BMP*/
	for (i = infoh->height - 1; i >= 0; i--){
		fwrite(Map[i], 1, 3 * infoh->width, out);
		fwrite(p, 1, padding, out);
	}

	//eliberez memoria pentru p si inchid fisierul de iesire
	free(p);
	fclose(out);
}

/*functie care returneaza 1 in cazul in care pixelul trimis ca argument este
colorat si 0 in cazul in care este alb*/
int coloredPixel(unsigned char **Map, int i, int j){
	if ((Map[i][j] != W) || (Map[i][j + 1] != W) || (Map[i][j + 2] != W))
		return 1;
	else return 0;
}

/*urmatoarele functii verifica, pentru pixelul trimis ca argument, daca este
coltul din stanga sus al unei "matrici" de 5x5 caracteristica unei anumite
cifre, bazandu-se pe foarte mult pattern-matching*/

//functie care returneaza 1 daca detecteaza cifra "0" si 0 in caz contrar
int check0(unsigned char **Map, int i, int j){
	if ((coloredPixel(Map, i, j)) &&
	(coloredPixel(Map, i, j + 3)) &&
	(coloredPixel(Map, i, j + 12)) &&
	(coloredPixel(Map, i + 1, j)) &&
	(!coloredPixel(Map, i + 2, j + 3)) &&
	(coloredPixel(Map, i + 4, j)) &&
	(coloredPixel(Map, i + 4, j + 12)))
		return 1;

	return 0;
}

//functie care returneaza 1 daca detecteaza cifra "1" si 0 in caz contrar
int check1(unsigned char **Map, int i, int j){
	if ((!coloredPixel(Map, i, j)) &&
	(!coloredPixel(Map, i, j + 4)) &&
	(!coloredPixel(Map, i, j + 9)) &&
	(coloredPixel(Map, i, j + 12)) &&
	(!coloredPixel(Map, i, j + 15)) &&
	(coloredPixel(Map, i + 1, j + 12)) &&
	(!coloredPixel(Map, i + 2, j + 6)) &&
	(coloredPixel(Map, i + 2, j + 12)) &&
	(coloredPixel(Map, i + 3, j + 12)) &&
	(!coloredPixel(Map, i + 4, j))&&
	(coloredPixel(Map, i + 4, j + 12)))
			return 1;

	return 0;
}

//functie care returneaza 1 daca detecteaza cifra "2" si 0 in caz contrar
int check2(unsigned char **Map, int i, int j){
	if ((coloredPixel(Map, i, j)) &&
	(coloredPixel(Map, i, j + 3)) &&
  	(coloredPixel(Map, i, j + 12)) &&
	(!coloredPixel(Map, i + 1, j)) &&
	(coloredPixel(Map, i + 2, j + 6)) &&
	(coloredPixel(Map, i + 3, j)) &&
	(coloredPixel(Map, i + 4, j + 12)))
		return 1;

	return 0;
}

//functie care returneaza 1 daca detecteaza cifra "3" si 0 in caz contrar
int check3(unsigned char **Map, int i, int j){
	if ((coloredPixel(Map, i, j)) &&
	(coloredPixel(Map, i, j + 3)) &&
	(coloredPixel(Map, i, j + 12)) &&
	(!coloredPixel(Map, i + 1, j)) &&
	(coloredPixel(Map, i + 2, j)) &&
	(coloredPixel(Map, i + 2, j + 6)) &&
	(coloredPixel(Map, i + 2, j + 9)) &&
  	(coloredPixel(Map, i + 4, j + 12)))
		return 1;

	return 0;
}

//functie care returneaza 1 daca detecteaza cifra "4" si 0 in caz contrar
int check4(unsigned char **Map, int i, int j){
	if ((coloredPixel(Map, i, j)) &&
	(!coloredPixel(Map, i, j + 3)) &&
	(coloredPixel(Map, i, j + 12)) &&
	(coloredPixel(Map, i + 1, j)) &&
  	(coloredPixel(Map, i + 1, j + 12)) &&
  	(coloredPixel(Map, i + 2, j)) &&
	(coloredPixel(Map, i + 2, j + 6)) &&
  	(coloredPixel(Map, i + 2, j + 9)) &&
  	(coloredPixel(Map, i + 2, j + 12)) &&
  	(coloredPixel(Map, i + 3, j + 12)) &&
  	(coloredPixel(Map, i + 4, j + 12)))
		return 1;

	return 0;
}

//functie care returneaza 1 daca detecteaza cifra "5" si 0 in caz contrar
int check5(unsigned char **Map, int i, int j){
	if ((coloredPixel(Map, i, j)) &&
	(coloredPixel(Map, i, j + 3)) &&
 	(coloredPixel(Map, i, j + 12)) &&	
	(coloredPixel(Map, i + 1, j)) &&
	(!coloredPixel(Map, i + 1, j + 12)) &&
	(!coloredPixel(Map, i + 3, j)) &&
	(coloredPixel(Map, i + 4, j + 12)))
		return 1;

	return 0;
}

//functie care returneaza 1 daca detecteaza cifra "6" si 0 in caz contrar
int check6(unsigned char **Map, int i, int j){
	if ((coloredPixel(Map, i, j)) &&
	(coloredPixel(Map, i, j + 3)) &&
 	(coloredPixel(Map, i, j + 12)) &&	
	(coloredPixel(Map, i + 1, j)) &&
	(!coloredPixel(Map, i + 1, j + 12)) &&
	(coloredPixel(Map, i + 3, j)) &&
	(coloredPixel(Map, i + 4, j + 12)))
		return 1;

	return 0;
}

//functie care returneaza 1 daca detecteaza cifra "7" si 0 in caz contrar
int check7(unsigned char **Map, int i, int j){
	if ((coloredPixel(Map, i, j)) &&
  	(coloredPixel(Map, i, j + 3)) &&
	(coloredPixel(Map, i, j + 9)) &&
	(coloredPixel(Map, i, j + 12)) &&
	(!coloredPixel(Map, i + 1, j)) &&
	(!coloredPixel(Map, i + 2, j + 9)) &&
	(coloredPixel(Map, i + 2, j + 12)) &&
	(coloredPixel(Map, i + 4, j + 12)))
		return 1;

	return 0;
}

//functie care returneaza 1 daca detecteaza cifra "8" si 0 in caz contrar
int check8(unsigned char **Map, int i, int j){
	if ((coloredPixel(Map, i, j)) &&
	(coloredPixel(Map, i, j + 3)) &&
	(coloredPixel(Map, i, j + 12)) &&
	(coloredPixel(Map, i + 1, j)) &&
	(coloredPixel(Map, i + 2, j)) &&
	(coloredPixel(Map, i + 2, j + 3)) &&
	(coloredPixel(Map, i + 2, j + 12)) &&
	(coloredPixel(Map, i + 3, j)) &&
	(coloredPixel(Map, i + 4, j + 12)))
		return 1;

	return 0;
}

//functie care returneaza 1 daca detecteaza cifra "9" si 0 in caz contrar
int check9(unsigned char **Map, int i, int j){
	if ((coloredPixel(Map, i, j)) &&
	(coloredPixel(Map, i, j + 3)) &&
	(coloredPixel(Map, i, j + 12)) &&
	(coloredPixel(Map, i + 1, j)) &&
	(coloredPixel(Map, i + 2, j)) &&
	(coloredPixel(Map, i + 2, j + 3)) &&
	(coloredPixel(Map, i + 2, j + 12)) &&
	(!coloredPixel(Map, i + 3, j)) &&
	(coloredPixel(Map, i + 4, j + 12)))
		return 1;

	return 0;
}

void solveCaptcha(char *task_name, InfoHeader *infoh, unsigned char **Map){
	/*deschiderea fisierului text de iesire si verificarea realizarii
	cu succes a operatiei*/
	FILE *out = fopen(task_name, "w");
	if (!out) exit(1);

	int i, j;

	/*parcurg matricea de pixeli mai intai pe coloana si apoi pe linie,
	avand grija sa nu depasesc limitele alocate pentru matrice si,
	verificand pe rand daca vreun pixel este coltul din stanga sus
	al unei "matrice" 5x5 caracteristica unei cifre, si in caz pozitiv
	afiseaza acea cifra si iese din al doilea for, pentru ca nu pot fi
	doua cifre incepand pe aceeasi coloana*/
	for (j = 0; j < 3 * infoh->width - 14; j += 3){
		for (i = 0; i < infoh->height - 4; i++){
			if (check0(Map, i, j)) {
				fprintf(out, "0");
				break;
			}
			else if (check1(Map, i, j)){
				fprintf(out, "1");
				break;
			}
			else if (check2(Map, i, j)){
				fprintf(out, "2");
				break;
			}
			else if (check3(Map, i, j)){
				fprintf(out, "3");
				break;
			}
			else if (check4(Map, i, j)){
				fprintf(out, "4");
				break;
			}
			else if (check5(Map, i, j)){
				fprintf(out, "5");
				break;
			}
			else if (check6(Map, i, j)){
				fprintf(out, "6");
				break;
			}
			else if (check7(Map, i, j)){
				fprintf(out, "7");
				break;
			}
			else if (check8(Map, i, j)){
				fprintf(out, "8");
				break;
			}
			else if (check9(Map, i, j)){
				fprintf(out, "9");
				break;
			}
		}	
	}

	//inchiderea fisierului de iesire
	fclose(out);
}

int main()
{
	/*deschiderea fisierului de input si verificarea realizarii
	cu succes a operatiei*/
	FILE *input = fopen("input.txt", "r");
	if (!input) return 0;
	
	/*i si j sunt folositi pentru parcurgerea matricei, image_name este
	numele imaginii prelucrate la task-urile 1, 2 si 3, taskX_name este 
	numele imaginii folosit la task-ul X (alocate dinamic), Map este
	matricea de pixeli, iar b, g si r culorile folosite la task-ul 1*/
	int i, j;
	char *image_name = malloc(30);
	if (!image_name) return 0;
	char *task1_name = malloc(30);
	if (!task1_name) return 0;
	char *task2_name = malloc(30);
	if (!task2_name) return 0;
	char *task3_name = malloc(30);
	if (!task3_name) return 0;
	unsigned char **Map;
	unsigned char b, g, r;

	//alocarea memoriei pentru headerele imaginii
	FileHeader *fileh = malloc(sizeof(FileHeader));
	if (!fileh) return 0;

	InfoHeader *infoh = malloc(sizeof(InfoHeader));
	if (!infoh) return 0;

	/*citirea din "input.txt" a numelui imaginii folosite la task-urile
	1, 2 si 3, dar si a culorilor folosite la task-ul 1*/
	fscanf(input, "%s", image_name);
	fscanf(input, "%hhu %hhu %hhu", &b, &g, &r);
	
	//incarcarea matricii de pixeli
	Map = LoadBMP(image_name, fileh, infoh);

	/*parcurgerea matricei de pixeli si schimbarea culorii pixelilor
	colorati in culoarea ceruta la task-ul 1*/
	for (i = 0; i < infoh->height; i++){
		for (j = 0; j < 3 * infoh->width; j += 3){
			if (coloredPixel(Map, i, j)){
					Map[i][j] = b;
					Map[i][j + 1] = g;
					Map[i][j + 2] = r;
			}
		}
	}

	/*formarea sirurilor de caractere corespunzatoare numelor fisierelor
	cerute ca output la task-urile 1, 2 si 3, folosind memcpy pentru
	a obtine un sir fara extensia ".bmp", iar mai apoi adaugand, dupa caz,
	caractere pentru a obtine forma ceruta in enunt cu sprintf*/
	memcpy(task1_name, image_name, strlen(image_name) - 4);
	sprintf(task1_name, "%s_task1.bmp", task1_name);

	memcpy(task2_name, image_name, strlen(image_name) - 4);
	sprintf(task2_name, "%s_task2.txt", task2_name);

	memcpy(task3_name, image_name, strlen(image_name) - 4);
	sprintf(task3_name, "%s_task3.bmp", task3_name);

	//scrierea imaginii pentru task-ul 1
	DumpBMP(task1_name, fileh, infoh, Map);

	//rezolvarea Captcha-ului, pentru task-ul 2
	solveCaptcha(task2_name, infoh, Map);

	/*reincarcarea matricii de pixeli (pentru ca inainte s-a alterat
	la task-ul 1, prin schimbarea culorii pixelilor colorati, iar mai apoi
	scrierea imaginii pentru task-ul 3*/
	Map = LoadBMP(image_name, fileh, infoh);
	DumpBMP(task3_name, fileh, infoh, Map);

	//eliberarea memoriei pentru elementele alocate dinamic anterior
	free(image_name);
	free(task1_name);
	free(task2_name);
	free(task3_name);
	for (i = 0; i < infoh->height; i++)
		free(Map[i]);
	free(Map);
	free(infoh);
	free(fileh);
	fclose(input);

	return 0;
}
