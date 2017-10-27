#include <stdio.h>

void Table(int *x, int *y, int N, int M, char *direction){
    int i, j;
    for (i = 0; i <= N + 1; i++){
        for (j = 0; j <= M + 1; j++){
            if ((i == 0) || (j == 0) || (i == N + 1) || (j == M + 1)) printf("*");
            else if ((i == *y) && (j == *x)) printf("%c", *direction);
            else printf(" ");
        }
        printf("\n");
    }
}

void MoveUp(int *y, char *direction){
    if (*y - 1 > 0){
        *y = *y - 1;
        *direction = '^';        
    }
}

void MoveDown(int *y, int N, char *direction){
    if (*y + 1 < N + 1){
        *y = *y + 1;
        *direction = 'v';
    } 
}

void MoveLeft(int *x, char *direction){
    if (*x - 1 > 0){
        *x = *x - 1;
        *direction = '<';
    }
}

void MoveRight(int *x, int M, char *direction){
    if (*x + 1 < M + 1){
        *x = *x + 1;
        *direction = '>';
    }
}

int main()
{
    int N, M, x, y, P;
    char action = 'a', direction = 'v';
    
    scanf("%d %d %d %d %d", &N, &M, &x, &y, &P);
   
    while(action){
        if ((action >= 'a') && (action <= 'z'))
			Table(&x, &y, N, M, &direction); 
        scanf("%c", &action);
        if (action == 'q') break;
        if (action == 'w') MoveUp(&y, &direction);
        else if (action == 's') MoveDown(&y, N, &direction);
        else if (action == 'a') MoveLeft(&x, &direction);
        else if (action == 'd') MoveRight(&x, M, &direction);
        if ((x == M) && (y == N)) {
			Table(&x, &y, N, M, &direction);
            printf("GAME COMPLETED\n");
            break;
        } 
    } 
   
	return 0;
}
