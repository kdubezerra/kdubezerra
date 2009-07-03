#include <stdio.h>
#include <omp.h>

#define num_passos 20000000
int main()
{
	double pi = 0;
	int i;
#pragma omp parallel for reduction (+:pi)
	for (i = 0; i < num_passos; i++)
	{
		pi += 4.0 / (4.0 * i + 1.0);
		pi -= 4.0 / (4.0 * i + 3.0);
	}
	printf("O valor de pi é %f\n", pi);
}
