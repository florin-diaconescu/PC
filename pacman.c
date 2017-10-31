#include <stdio.h>
#include <stdlib.h>

void Table(int *x, int *y, int N, int M, char *direction, int xobs[], int yobs[]){
    int i, j;
    for (i = 0; i <= N + 1; i++){
        for (j = 0; j <= M + 1; j++){
            if ((i == 0) || (j == 0) || (i == N + 1) || (j == M + 1)) printf("*");
            else if ((i == *y) && (j == *x)) printf("%c", *direction);
			else if ((i == yobs[1]) && (j == xobs[1])) printf("x");
			else if ((i == yobs[2]) && (j == xobs[2])) printf("x");
			else if ((i == yobs[3]) && (j == xobs[3])) printf("x");
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

void MoveObstacle(int x, int y, int *xobs, int *yobs){
	int Length, minLength = 35700, i, j, x2, y2;

	for (i = 0; i < 3; i++){
		for (j = 0; j < 3; j++) {
			Length = abs(x - (*xobs + 1 - j)) + abs(y - (*yobs + i - 1));
			if (Length < minLength){
				 minLength = Length;
				 x2 = (*xobs + 1 - j);
				 y2 = (*yobs + i - 1);
			}
		}
	} 	

	*xobs = x2;
	*yobs = y2;
}

int main()
{
	int N, M, x, y, P, i, xobs[4] = {0}, yobs[4] = {0}, flag = 0, turn = 1;
    char action = 'a', direction = 'v', type[4];
    
    scanf("%d %d %d %d %d", &N, &M, &x, &y, &P);

	if (P > 0){
		 for (i = 1; i <= P; i++) 
			scanf ("%d %d %c", &yobs[i], &xobs[i], &type[i]);
	}
	
    while(action){
        if ((action >= 'a') && (action <= 'z')){
			//turn++;
			Table(&x, &y, N, M, &direction, xobs, yobs); 
		}

		scanf("%c", &action);
		
        if (action == 'q') break;
        else if (action == 'w') MoveUp(&y, &direction);
        else if (action == 's') MoveDown(&y, N, &direction);
        else if (action == 'a') MoveLeft(&x, &direction);
        else if (action == 'd') MoveRight(&x, M, &direction);

		//if (turn % 2 == 1){
			for (i = 1; i <= P; i++){
				if (type[i] == 'm') 
					MoveObstacle(x, y, &xobs[i], &yobs[i]); 
			}
		//}

        if ((x == M) && (y == N)) {
			Table(&x, &y, N, M, &direction, xobs, yobs);
            printf("GAME COMPLETED\n");
            break;
        }
 
		for (i = 1; i <= P; i++){
			if ((x == xobs[i]) && (y == yobs[i])){
				printf("GAME OVER\n");
				flag = 1;
				break;
			}
	    }

		if (flag == 1) break;
	}  

	return 0;
}
