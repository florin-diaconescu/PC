#include <stdio.h>
#include <stdlib.h>

//definesc constanta MAX, pentru lungimea vectorilor obstacolelor mobile 
#define MAX 4

//definesc constanta MAXINT, folosita in MoveObstacle()
#define MAXINT 36700

//Functie folosita pentru afisarea tablei de joc
void Table(int *x, int *y, int N, int M, char *dir, int xObs[], int yObs[]){
    int i, j;

    for (i = 0; i <= N + 1; i++){
        for (j = 0; j <= M + 1; j++){
			//verific daca coordonatele punctului verificat sunt pe margini
            if ((i == 0) || (j == 0) || (i == N + 1) || (j == M + 1)) 
				printf("*");
			//verific daca este verificata pozitia jucatorului
            else if ((i == *y) && (j == *x)) 
				printf("%c", *dir);
			//verific daca afisez un obstacol
			else if ((i == yObs[1]) && (j == xObs[1])) 
				printf("x");
			else if ((i == yObs[2]) && (j == xObs[2])) 
				printf("x");
			else if ((i == yObs[3]) && (j == xObs[3])) 
				printf("x");
            else 
				printf(" ");
        }
        printf("\n");
    }
}

/*Pentru functiile de deplasare, verific daca este posibila, iar
daca nu se depasesc marginile, modific coordonatele personajului*/

//Functie pentru deplasarea in sus (cu 'w') a personajului
void MoveUp(int *y, char *dir){
    if (*y - 1 > 0){
        *y = *y - 1;
        *dir = '^';        
    }
}

//Functie pentru deplasarea in jos (cu 's') a personajului
void MoveDown(int *y, int N, char *dir){
    if (*y + 1 < N + 1){
        *y = *y + 1;
        *dir = 'v';
    } 
}

//Functie pentru deplasarea la stanga (cu 'a') a personajului
void MoveLeft(int *x, char *dir){
    if (*x - 1 > 0){
        *x = *x - 1;
        *dir = '<';
    }
}

//Functie pentru deplasarea la dreapta (cu 'd') a personajului
void MoveRight(int *x, int M, char *dir){
    if (*x + 1 < M + 1){
        *x = *x + 1;
        *dir = '>';
    }
}

//Functie pentru deplasarea obstacolului mobil corespunzator apelarii din main
void MoveObstacle(int x, int y, int *xObs, int *yObs){
	int Length, minLength = MAXINT, i, j, x2, y2;

	/*prin aceasta instructiune repetitiva, se incearca toate combinatiile
	posibile de deplasare a obstacolului mobil, salvandu-se lungimea minima
	dintre cele 8 cazuri, dar si coordonatele punctului nou aferent, in 
	variabilele x2 si y2*/
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
	/*vectorii xObs, yObs si type sunt folositi pentru stocarea
	coordonatelor, respectiv a tipului de obstacol
	flag este folosit pentru iesirea din joc in cazul apasarii 'q'
	flagObs verifica daca deplasarea obstacolului s-a realizat deja
	turn stocheaza numarul de runde
	action reprezinta tasta apasata de personaj
	dir reprezinta caracterul corespunzator deplasarii */
	
	int N, M, x, y, P, i, xObs[MAX] = {0}, yObs[MAX] = {0};
	int flag = 0, turn = 0, flagObs = 0;
    char action = 'a', dir = 'v', type[MAX];
    
    scanf("%d %d %d %d %d", &N, &M, &x, &y, &P);

	//se citesc coordonatele si tipul obstacolelor. daca este cazul
	if (P > 0){
		 for (i = 1; i <= P; i++) 
			scanf ("%d %d %c", &yObs[i], &xObs[i], &type[i]);
	}
	
	//jocul propriu-zis
    while(action){
        if ((action >= 'a') && (action <= 'z'))
			Table(&x, &y, N, M, &dir, xObs, yObs); 

		scanf("%c", &action);
		
		/*se verifica tasta apasata de personaj si se realizeaza 
		actiunea corespunzatoare*/
        if (action == 'q') 
			break;
        else if (action == 'w') 
			MoveUp(&y, &dir);
        else if (action == 's') 
			MoveDown(&y, N, &dir);
        else if (action == 'a') 
			MoveLeft(&x, &dir);
        else if (action == 'd') 
			MoveRight(&x, M, &dir);

		/*se incrementeaza runda jocului, iar obstacolele nu au fost
		mutate deja, fiind o actiune noua a personajului*/
		if ((action >= 'a') && (action <= 'z')){
			flagObs = 0;
			turn++;
		}

		/*daca este o runda para, iar obstacolul nu a fost mutat deja,
		se realizeaza deplasarea obstacolului mobil*/
		if ((turn % 2 == 0) && (turn != 0) && (!flagObs)){
			for (i = 1; i <= P; i++){
				if (type[i] == 'm') 
					MoveObstacle(x, y, &xObs[i], &yObs[i]); 
			}
			flagObs = 1;
		}
		/*daca jucatorul a ajuns in coltul dreapta-jos, afiseaza mesajul
		corespunzator victoriei*/
        if ((x == M) && (y == N)) {
			Table(&x, &y, N, M, &dir, xObs, yObs);
            printf("GAME COMPLETED\n");
            break;
        }
 
		/*daca jucatorul a ajuns intr-o casuta cu un obstacol, afiseaza
		mesajul corespunzator infrangerii*/
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
