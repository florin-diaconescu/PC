#include <stdio.h>

int main()
{
	unsigned int N, i, j;
	char C;

	scanf("%u %c", &N, &C);
	
	/*prin aceasta instructiune repetitiva, afisez caracterul citit
	de la tastatura, daca este deasupra diagonalei principale, sau 
	spatiu, in caz contrar*/
	for (i = 0; i < N; i++) {
		for (j = 0; j < N; j++) {
			if (i <= j) printf ("%c", C);
				else printf(" ");
			/*afisez spatiu dupa caracter doar daca nu este ultimul
			de pe linie*/
			if (j < (N - 1)) printf(" ");
		}
		printf("\n");
	}

	return 0;
}
