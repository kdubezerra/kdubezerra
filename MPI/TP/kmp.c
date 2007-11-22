#include <stdio.h>
#include <stdlib.h>

#define XSIZE 100
#define XL 10000000000

void preKmp(char *x, unsigned long m, unsigned long kmpNext[]) {
   unsigned long i, j;

   i = 0;
   j = kmpNext[0] = -1;
   while (i < m) {
      while (j > -1 && x[i] != x[j])
         j = kmpNext[j];
      i++;
      j++;
      if (x[i] == x[j])
         kmpNext[i] = kmpNext[j];
      else
         kmpNext[i] = j;
   }
}


void KMP(char *x, unsigned long m, char *y, unsigned long n) {
   unsigned long i, j, kmpNext[XSIZE];

   /* Preprocessing */
   preKmp(x, m, kmpNext);

   /* Searching */
   i = j = 0;
   while (j < n) {
      while (i > -1 && x[i] != y[j])
         i = kmpNext[i];
      i++;
      j++;
      if (i >= m) {
         printf("%d\n",j - i);
         i = kmpNext[i];
      }
   }
}

unsigned long main (unsigned long argc, char** argv) {
	char lalphabet[] = {'a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y', 'z'};
	unsigned long i;
	
	
	if (argc != 3) {
		printf ("Uso: %s <padrão> <comprimento do padrão>\n", argv[0]);
	}
	
	char *pattern = argv[1];
	unsigned long m = atoi (argv[2]);
	char *text = argv[3];
	unsigned long n = atoi (argv[4]);
	
	text = calloc (XL, sizeof(char));
	for (i = 0; i < XL; i++) {
		text[i] = lalphabet[rand() % 26];
	}
	text[XL-1] = '\0';
	
	printf ("É AGORA\n");
		
	KMP(pattern, m, text, XL);
	
	//printf ("%s\n", text + 85454910);
	
	
}
