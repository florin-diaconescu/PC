#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

int Arena_size, startIndex, numBlocks, sumAlloc;
unsigned char *Arena;

void changeValue(int index, int value){
	int *curent = (int*)(Arena + index);
	*curent = value;
}

void INITIALIZE(int size){
	Arena_size = size;
	Arena = calloc(size, sizeof(unsigned char));
}

void FINALIZE(){
	free(Arena);
	numBlocks = 0;
}

void DUMP(){
	int i, j, k = 0;

	if (!Arena_size) return;

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

int ALLOC(int size){
	int i, j, currentBlock, nextBlock, currentIndex;

	if (numBlocks == 0){
		if (Arena_size < size + 12){
			printf("0\n");
			return 0;
		}

		*((int *)(Arena)) = 0;
		*((int *)(Arena + 4)) = 0;
		*((int *)(Arena + 8)) = size;
		startIndex = 0;
		
		numBlocks++;
		printf("12\n");
		return (12);
		}

	if (startIndex >= size + 12){
		*((int *)(Arena)) = startIndex;
		*((int *)(Arena + 4)) = 0;
		*((int *)(Arena + 8)) = size;
		*((int *)(*((int *)(Arena + i)))) = 0;
		startIndex = 0;

		numBlocks++;
		printf("12\n");
		return (12);
	}

	currentIndex = startIndex;

	for (i = 0; i < numBlocks; i++){
		currentBlock = *((int *)(Arena + currentIndex));
		nextBlock = nextBlock + *((int *)(Arena + currentIndex + 8)) + 12;
		//printf("Current block: %d\nNext block: %d\n", currentBlock, nextBlock);
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
				numBlocks++; 
				printf("%d\n", nextBlock + 12);
				return (nextBlock + 12);
			}
		}
//* MODIFICA ASTA!!! *//
		if (currentBlock - nextBlock >= size + 12){
			*((int *)(nextBlock)) = *((int *)(Arena + i));
			*((int *)(nextBlock + 4)) = i;
			*((int *)(nextBlock + 8)) = size + 12;
			*((int *)(currentBlock + 4)) = nextBlock;
			*((int *)(i)) = nextBlock;
			numBlocks++;
			printf("%d\n", nextBlock + 12);
			return (nextBlock + 12);
		}	
//* MODIFICA ASTA!!! *//

		currentIndex = nextBlock;
		//printf("Current index: %d\n", currentIndex);
	}
	
}

int FREE(int index){
	int size, nextIndex, previousIndex;
	
	nextIndex = *((int *)(Arena + index - 12));
	previousIndex = *((int *)(Arena + index - 8));
	size = *((int *)(Arena + index - 4));

	if (numBlocks == 1){
		memset(Arena + index - 4, 0, size);
		numBlocks --;
		return size;
	}

	memset(Arena + index - 8, 0, 4);
	memset(Arena + index - 8, 0, 4);
	memset(Arena + index - 4, 0, size);
	*((int *)(Arena + previousIndex)) = nextIndex;

	if (nextIndex)
		*((int *)(Arena + nextIndex - 4)) = previousIndex;
	
	return size;
}

void FILL(int index, int size, int value){
	int i, currentIndex, subSize, sizeSum = 0;

	if (sumAlloc > size)
		memset(Arena + index, value, size);

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
        sumAlloc += ALLOC(size);

    } else if (strcmp(cmd_name, "FREE") == 0) {
        char* index_str = strtok(NULL, delims);
        if (!index_str) {
            goto invalid_command;
        }
        int index = atoi(index_str);
        sumAlloc -= FREE(index);

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

    } else if (strcmp(cmd_name, "ALLOCALIGNED") == 0) {
        char* size_str = strtok(NULL, delims);
        if (!size_str) {
            goto invalid_command;
        }
        int size = atoi(size_str);
        char* align_str = strtok(NULL, delims);
        if (!align_str) {
            goto invalid_command;
        }
        int align = atoi(align_str);
        // TODO - ALLOCALIGNED

    } else if (strcmp(cmd_name, "REALLOC") == 0) {
        printf("Found cmd REALLOC\n");
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
        // TODO - REALLOC

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
