#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "bmp_header.h"

unsigned char **LoadBMP(char *image_name, FileHeader *fileh, InfoHeader *infoh){
	FILE *img = fopen(image_name, "rb");
	if (!img) exit(1);

	int i, j, padding;
	unsigned char **Map;

	fread(fileh, sizeof(FileHeader), 1, img);
	fread(infoh, sizeof(InfoHeader), 1, img);
	
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

	fseek(img, fileh->imageDataOffset, SEEK_SET);

	padding = infoh->width % 4;

	for (i = infoh->height - 1; i >= 0; i--){
		fread(Map[i], 1, 3 * infoh->width, img);
		if (padding) fseek(img, padding, SEEK_CUR);
	}

	fclose(img);
	return Map;
}

void DumpBMP(char *task_name, FileHeader *fileh, InfoHeader *infoh, unsigned char **Map){
	FILE *out = fopen(task_name, "wb");
	if (!out) exit(1);

	int i, padding;
	unsigned char *p = calloc(3, 1);
	if (!p) exit(1);

	fwrite(fileh, sizeof(FileHeader), 1, out);
	fwrite(infoh, sizeof(InfoHeader), 1, out);
	
	padding = infoh->width % 4;
	
	for (i = infoh->height - 1; i >= 0; i--){
		fwrite(Map[i], 1, 3 * infoh->width, out);
		if (padding) fwrite(p, 1, padding, out);
	}

	free(p);
	fclose(out);
}

void solveCaptcha(char *task_name, FileHeader *fileh, InfoHeader *infoh, unsigned char **Map){
	FILE *out = fopen(task_name, "w");
	if (!out) exit(1);
	
	int i, j, padding;

	padding = infoh->width % 4;

	for (j = 0; j < 3 * infoh->width; j += 3){
		for (i = 0; i < infoh->height - 4; i++){
			if (((Map[i][j] != 255) || (Map[i][j + 1] != 255) || (Map[i][j + 2] != 255)) &&
			((Map[i + 4][j + 12] != 255) || (Map[i + 4][j + 13] != 255) || (Map[i + 4][j + 14] != 255))){
				if ((Map[i][j + 3] != 255) || (Map[i][j + 4] != 255) || (Map[i][j + 5] != 255)){
					if ((Map[i + 1][j] != 255) || (Map[i + 1][j + 1] != 255) || (Map[i + 1][j + 2] != 255)){
						if ((Map[i + 2][j + 3] != 255) || (Map[i + 2][j + 4] != 255) || (Map[i + 2][j + 5] != 255)){
							if ((Map[i + 3][j] != 255) || (Map[i + 3][j + 1] != 255) || (Map[i + 3][j + 2] != 255)){
								if ((Map[i + 2][j + 12] != 255) || (Map[i + 2][j + 13] != 255) || (Map[i + 2][j + 14] != 255)){
									fprintf(out, "8");
								}
								else fprintf(out, "6");
							}
							else{//5 sau 9
								if ((Map[i + 2][j + 12] != 255) || (Map[i + 2][j + 13] != 255) || (Map[i + 2][j + 14] != 255)){
									fprintf(out, "5");
								}
								else fprintf(out, "9");
							}
						}
						else fprintf(out, "0");
					}
					//daca pixelul de sub e 0, poate sa fie 2, 3 sau 7
					else{
						if ((Map[i + 3][j] != 255) || (Map[i + 3][j + 1] != 255) || (Map[i + 3][j + 2] != 255)){
							fprintf(out, "2");
						}
						else {//3 sau 7
							if ((Map[i + 2][j] != 255) || (Map[i + 2][j + 1] != 255) || (Map[i + 2][j + 2] != 255)){
								fprintf(out, "3");
							}
							else fprintf(out, "7");
						}
					}
				}
				if ((Map[i + 2][j + 9] != 255) || (Map[i + 2][j + 10] != 255) || (Map[i + 3][j + 11] != 255)) 
					fprintf(out, "4");
			}
			if ((j >3) && ((Map[i][j] != 255) || (Map[i][j + 1] != 255) || (Map[i][j + 2] != 255)) &&
			((Map[i + 4][j] != 255) || (Map[i + 4][j + 1] != 255) || (Map[i + 4][j + 2] != 255)) &&
			((Map[i][j + 3] == 255) && (Map[i][j + 4] == 255) && (Map[i][j + 5] == 255)) &&
			((Map[i][j - 3] == 255) && (Map[i][j - 2] == 255) && (Map[i][j - 1] == 255)))
				fprintf(out,"1");
		}
	}
	
	fclose(out);
}

int main()
{
	FILE *input = fopen("input.txt", "r");
	if (!input) return 0;
	
	int i, j;
	char *image_name = malloc(30);
	char *task1_name = malloc(30);
	char *task2_name = malloc(30);
	unsigned char **Map;
	unsigned char b, g, r;
	if (!image_name) return 0;

	
	FileHeader *fileh = malloc(sizeof(FileHeader));
	if (!fileh) return 0;

	InfoHeader *infoh = malloc(sizeof(InfoHeader));
	if (!infoh) return 0;

	fscanf(input, "%s", image_name);
	fscanf(input, "%hhu %hhu %hhu", &b, &g, &r);
	
	Map = LoadBMP(image_name, fileh, infoh);

	for (i = 0; i < infoh->height; i++){
		for (j = 0; j < 3 * infoh->width; j += 3){
			if ((Map[i][j] != 255) || (Map[i][j + 1] != 255) || (Map[i][j + 2] != 255)){
					Map[i][j] = b;
					Map[i][j + 1] = g;
					Map[i][j + 2] = r;
			}
		}
	}

	memcpy(task1_name, image_name, strlen(image_name) - 4);
	sprintf(task1_name, "%s_task1.bmp", task1_name);

	memcpy(task2_name, image_name, strlen(image_name) - 4);
	sprintf(task2_name, "%s_task2.txt", task2_name);

	DumpBMP(task1_name, fileh, infoh, Map);

	solveCaptcha(task2_name, fileh, infoh, Map);

	free(image_name);
	free(task1_name);
	free(task2_name);
	for (i = 0; i < infoh->height; i++)
		free(Map[i]);
	free(Map);
	free(infoh);
	free(fileh);
	fclose(input);

	return 0;
}
