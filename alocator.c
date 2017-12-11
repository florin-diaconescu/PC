//Diaconescu Florin, 312CB, florin.diaconescu

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

/* folosesc mai multe variabile globale, cu nume sugestive, care semnifica
dimensiunea arenei, variabila separata pentru indicele de start, un contor
al numarului de blocuri si suma sectiunilor de date alocate de utilizator.
Arena este tocmai arena folosita de alocator */
int Arena_size, startIndex, numBlocks, sumAlloc;
unsigned char *Arena;

/* functia de INITIALIZE, in care se seteaza dimensiunea arenei la parametrul
size, iar zona arenei este alocata dinamica prin intermediul unui apel calloc */
void INITIALIZE(int size){
	Arena_size = size;
	Arena = calloc(size, sizeof(unsigned char));
}

/*functia de FINALIZE elibereaza zona alocata dinamic in INITIALIZE, iar 
contorul blocurilor este resetat */
void FINALIZE(){
	free(Arena);
	numBlocks = 0;
}

//functia de DUMP afiseaza termenii arenei in formatul specificat in enunt
void DUMP(){
	int i, j, k = 0;

	//tratez separat cazul in care dimensiunea arenei este 0
	if (!Arena_size) return;

	/* prin urmatoarele bucle se realizeaza afisarea propriu-zisa a termenilor,
	folosindu-ma de 3 contoare, i si j pentru afisarea pana la cel mai mare 
	termen multiplu de 16, iar k pentru a afisa termenii ramasi */
	for (i = 0; i < Arena_size - 16; i += 16){
		printf("%08X\t", i);

		for (j = 0; j < 16; j++, k++){
			printf("%02X ", *(Arena + k));
			if (j == 7) 
				printf(" ");
		}
		printf("\n");
	}

	printf("%08X\t", i);
	for(;k < Arena_size; k++){
		printf("%02X ", *(Arena + k));
		if (k % 8 == 7)
			printf(" ");
	}
	printf("\n");
}

//functie pentru alocarea blocurilor de dimensiunea size
int ALLOC(int size){
	/* i este un contor, currentBlock memoreaza indicele de start al blocului
	curent, nextBlock pe cel al blocului urmator*/
	int i, currentBlock, nextBlock = 0, currentIndex;

	/* daca arena este goala, verific daca este suficient spatiu pentru a insera 
	blocul, iar daca nu, modific gestiunea corespunzator */
	if (numBlocks == 0){
		if (Arena_size < size + 12){
			printf("0\n");
			return 0;
		}

		*((int *)(Arena)) = 0;
		*((int *)(Arena + 4)) = 0;
		*((int *)(Arena + 8)) = size;
		startIndex = 0;
		
		sumAlloc += size;
		numBlocks++;
		printf("12\n");
		return (12);
		}

	//verific daca este loc suficient intre primul termen al arenei si primul bloc
	if (startIndex >= size + 12){
		*((int *)(Arena)) = startIndex;
		*((int *)(Arena + 4)) = 0;
		*((int *)(Arena + 8)) = size;
		startIndex = 0;

		sumAlloc += size;
		numBlocks++;
		printf("12\n");
		return (12);
	}

	//initializez variabila currentIndex cu indicele de start
	currentIndex = startIndex;

	for (i = 0; i < numBlocks; i++){
		currentBlock = *((int *)(Arena + currentIndex));
		nextBlock = nextBlock + *((int *)(Arena + currentIndex + 8)) + 12;
		
		/* daca indicele blocului curent este 0, inseamna ca este ultimul, deci
		verific daca este spatiu pentru a insera blocul de dimensiune size, iar 
		in caz contrar afisez 0 */
		if (!currentBlock){
			if (Arena_size - nextBlock < size + 12){
				printf("0\n");
				return 0;
			}

			else{
				*((int *)(Arena + currentIndex)) = nextBlock;
				*((int *)(Arena + nextBlock)) = currentBlock;
				*((int *)(Arena + nextBlock + 4)) = currentIndex;
				*((int *)(Arena + nextBlock + 8)) = size;

				sumAlloc += size;
				numBlocks++; 
				printf("%d\n", nextBlock + 12);
				return (nextBlock + 12);
			}
		}

		/* verific daca este loc suficient pentru a insera blocul intre 2 blocuri
		vecine */
		if (currentBlock - nextBlock >= size + 12){
			*((int *)(Arena + currentIndex)) = nextBlock;
			*((int *)(Arena + nextBlock)) = currentBlock;
			*((int *)(Arena + nextBlock + 4)) = currentIndex;
			*((int *)(Arena + nextBlock + 8)) = size;

			sumAlloc += size;
			numBlocks++;
			printf("%d\n", nextBlock + 12);
			return (nextBlock + 12);
		}	

		/* daca nu s-a reusit alocarea pana acum, se actualizeaza currentIndex,
		pentru urmatoarea apelare a for-ului */
		currentIndex = nextBlock;
	}

	return 0;	
}

/* functia de FREE elibereaza o zona de memorie incepand cu index, modificand
si gestiunea blocurilor vecine */
int FREE(int index){
	/* size memoreaza dimensiunea blocului ce urmeaza a fi eliberat, iar nextIndex
	si, respectiv, previousIndex indicii de start ai blocurilor vecine */
	int size, nextIndex, previousIndex;
	
	nextIndex = *((int *)(Arena + index - 12));
	previousIndex = *((int *)(Arena + index - 8));
	size = *((int *)(Arena + index - 4));

	//daca arena contine un singur bloc, tratez cazul separat
	if (numBlocks == 1){
		memset(Arena + index - 4, 0, size);

		sumAlloc -= size;
		numBlocks --;
		return size;
	}

	//modific gestiunea blocurilor vecine
	*((int *)(Arena + previousIndex)) = nextIndex;
	*((int *)(Arena + nextIndex + 4)) = previousIndex;

	//daca se elibereaza primul bloc, actualizez indicele de start
	if (index == 12) 
		startIndex = nextIndex;

	/* scad din sumAlloc numarul de octeti eliberati, iar printr-un memset
	golesc gestiunea blocului dorit, dar si sectunea de date */
	sumAlloc -= size;
	memset(Arena + index - 12, 0, size + 12);
	numBlocks--;
	return size;
}

//functia de FILL umple maxim size octeti cu valoarea value, incepand cu index
void FILL(int index, int size, int value){
	/* prin subSize memorez numarul de octeti umpluti la fiecare iteratie a 
	buclei, iar in sizeSum memorez numarul total de octeti umpluti */
	int i, currentIndex, subSize, sizeSum = 0;

	/* daca au fost alocati mai multi octeti decat size, umplu octetii printr-un
	memset */
	if (sumAlloc > size)
		memset(Arena + index, value, size);

	/* daca nu, ma asigur ca umplu doar octetii din sectiunea de date, cat timp
	numarul total de octeti umpluti nu depaseste value */
	else{
		currentIndex = startIndex;
		for (i = 0; i < numBlocks; i++){
			subSize = *((int *)(Arena + currentIndex + 8));
			sizeSum += subSize;

			if (sizeSum <= size)
				memset(Arena + currentIndex + 12, value, subSize);
			currentIndex += 12 + subSize;
		}
	}
}

/* scheletul alocatorului, prin care se parseaza datele de la tastatura si se 
intra pe ramura corespunzatoare, apelandu-se functia dorita */
void parse_command(char* cmd)
{
    const char* delims = " \n";

    char* cmd_name = strtok(cmd, delims);
    if (!cmd_name) {
        goto invalid_command;
    }

    if (strcmp(cmd_name, "INITIALIZE") == 0) {
        char* size_str = strtok(NULL, delims);
        if (!size_str) {
            goto invalid_command;
        }
        int size = atoi(size_str);
        INITIALIZE(size);

    } else if (strcmp(cmd_name, "FINALIZE") == 0) {
        FINALIZE();

   } else if (strcmp(cmd_name, "DUMP") == 0) {
        DUMP();

    } else if (strcmp(cmd_name, "ALLOC") == 0) {
        char* size_str = strtok(NULL, delims);
        if (!size_str) {
            goto invalid_command;
        }
        int size = atoi(size_str);
				ALLOC(size);

    } else if (strcmp(cmd_name, "FREE") == 0) {
        char* index_str = strtok(NULL, delims);
        if (!index_str) {
            goto invalid_command;
        }
        int index = atoi(index_str);
        FREE(index);

    } else if (strcmp(cmd_name, "FILL") == 0) {
        char* index_str = strtok(NULL, delims);
        if (!index_str) {
            goto invalid_command;
        }
        int index = atoi(index_str);
        char* size_str = strtok(NULL, delims);
        if (!size_str) {
            goto invalid_command;
        }
        int size = atoi(size_str);
        char* value_str = strtok(NULL, delims);
        if (!value_str) {
            goto invalid_command;
        }
        int value = atoi(value_str);
        FILL(index, size, value);

    } else {
        goto invalid_command;
    }

    return;

invalid_command:
    printf("Invalid command: %s\n", cmd);
    exit(1);
}

int main(void)
{
    ssize_t read;
    char* line = NULL;
    size_t len;

    /* parse input line by line */
    while ((read = getline(&line, &len, stdin)) != -1) {
        /* print every command to stdout */
        printf("%s", line);

        parse_command(line);
    }

    free(line);

    return 0;
}
