#include <stdio.h>

int main()
{
	unsigned int N, i, j;
	char C;

	scanf("%u %c", &N, &C);
	
	for (i = 0; i < N; i++) {
		for (j = 0; j < N; j++) {
			if (i <= j) printf ("%c", C);
				else printf(" ");
			if (j < (N - 1)) printf(" ");
		}
		printf("\n");
	}

	return 0;
}
