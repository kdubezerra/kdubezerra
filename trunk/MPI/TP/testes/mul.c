#include <stdio.h>
#include <sys/time.h>

int main () {
	struct timeval start, end;
	long total_usec;
	float a = 1234.3452;
	float b = 523.2345;
	float c;
	int i,j;

	gettimeofday(&start, NULL);
	for (i = 0 ; i < 32768 ; i++)
		for (j = 0; j < 32768 ; j++)
			c = a * b;
	gettimeofday(&end, NULL);

	total_usec = (end.tv_sec - start.tv_sec)*1e6 + (end.tv_usec - start.tv_usec);
	
	printf ("Tempo: %ld usec\n", total_usec);

}

