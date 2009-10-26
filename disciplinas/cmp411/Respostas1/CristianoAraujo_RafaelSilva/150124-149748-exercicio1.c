/* Cristiano Werner Araújo - 150124	
   Rafael Hansen da Silva  - 149748
*/

#include <pthread.h>
#include <stdio.h>
#define N_THREADS 5


void *funcao(void *threadid)
{
   long tid;
   tid = (long)threadid;
   //cout<<"Sou a Thread : "<<tid<<endl;
   printf("sou a thread %ld!\n", tid);
   pthread_exit(NULL);

}

int main(void)
{

pthread_t threads_array[N_THREADS];

long num_th;
int rc;
for(num_th = 0;num_th<N_THREADS;num_th++)
	{
	rc = pthread_create(&threads_array[num_th], NULL, funcao, (void *)num_th);
	if(rc)
		{
		printf("erro: %i",rc);		
		//cout<<"ERRO : codigo de retorno eh:"<<rc<<endl;
	
		}
	}
}
