#include <stdio.h>
#include <stdlib.h>

void Table(int *x, int *y, int N, int M, char *direction, int xObs[], int yObs[]){
    int i, j;
    for (i = 0; i <= N + 1; i++){
        for (j = 0; j <= M + 1; j++){
            if ((i == 0) || (j == 0) || (i == N + 1) || (j == M + 1)) printf("*");
            else if ((i == *y) && (j == *x)) printf("%c", *direction);
			else if ((i == yObs[1]) && (j == xObs[1])) printf("x");
			else if ((i == yObs[2]) && (j == xObs[2])) printf("x");
			else if ((i == yObs[3]) && (j == xObs[3])) printf("x");
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

void MoveObstacle(int x, int y, int *xObs, int *yObs){
	int Length, minLength = 35700, i, j, x2, y2;

	for (i = 0; i < 3; i++){
		for (j = 0; j < 3; j++) {
			Length = abs(x - (*xObs + 1 - j)) + abs(y - (*yObs + i - 1));
			if (Length < minLength){
				 minLength = Length;
				 x2 = (*xObs + 1 - j);
				 y2 = (*yObs + i - 1);
			}
		}
	} 	

	*xObs = x2;
	*yObs = y2;
}

int main()
{
	int N, M, x, y, P, i, xObs[4] = {0}, yObs[4] = {0};
	int flag = 0, turn = 0, flagObs = 0;
    char action = 'a', direction = 'v', type[4];
    
    scanf("%d %d %d %d %d", &N, &M, &x, &y, &P);

	if (P > 0){
		 for (i = 1; i <= P; i++) 
			scanf ("%d %d %c", &yObs[i], &xObs[i], &type[i]);
	}
	
    while(action){
        if ((action >= 'a') && (action <= 'z'))
			Table(&x, &y, N, M, &direction, xObs, yObs); 

		scanf("%c", &action);
		
        if (action == 'q') break;
        else if (action == 'w') MoveUp(&y, &direction);
        else if (action == 's') MoveDown(&y, N, &direction);
        else if (action == 'a') MoveLeft(&x, &direction);
        else if (action == 'd') MoveRight(&x, M, &direction);

		if ((action >= 'a') && (action <= 'z')){
			flagObs = 0;
			turn++;
		}

		if ((turn % 2 == 0) && (turn != 0) && (!flagObs)){
			for (i = 1; i <= P; i++){
				if (type[i] == 'm') 
					MoveObstacle(x, y, &xObs[i], &yObs[i]); 
			}
			flagObs = 1;
		}

        if ((x == M) && (y == N)) {
			Table(&x, &y, N, M, &direction, xObs, yObs);
            printf("GAME COMPLETED\n");
            break;
        }
 
		for (i = 1; i <= P; i++){
			if ((x == xObs[i]) && (y == yObs[i])){
				printf("GAME OVER\n");
				flag = 1;
				break;
			}
	    }

		if (flag == 1) break;

	}  
	
	return 0;
}
