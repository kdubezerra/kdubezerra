#include "heap.h"

heap_v* heap_create (int n, void *func/*int (*func) (void*, void*)*/) {
	heap_v* new_heap = malloc ( sizeof (heap_v) );
	
	new_heap->last = -1;
	new_heap->vect = calloc ( n, sizeof (void *) );
 	new_heap->f = func;
}

void heap_append (void* data, heap_v* heap) {
	heap->last++;
	heap->vect[heap->last] = data;	
}

void heapify_up (heap_v* heap) {
	int index = heap->last;
	int parent = (int) ((index + 1) / 2) - 1;
	
// 	printf ("O pai do %d é o %d\n", index, parent);
	
	while (parent >= 0) {
// 		printf ("vai chamar a função\n");
		if ( heap->f ( heap->vect[index] , heap->vect[parent] ) ) {
// 			printf ("chamou a função\n");
			void* temp = heap->vect[parent];
			heap->vect[parent] = heap->vect[index];
			heap->vect[index] = temp;
			
			index = parent;
			parent = (int) ((index + 1) / 2) - 1;
		}
		else
			break;		
	}	
	
}

void heapify_down (heap_v* heap) {
	int index = 0;
	int left = 1;
	int right = 2;
	int bigger = 0;
	int last = heap->last;
	
	while (1) {
		bigger = index;
		if (left <= last && heap->f (heap->vect[left], heap->vect[bigger]))
			bigger = left;
		if (right <= last && heap->f (heap->vect[right], heap->vect[bigger]))
			bigger = right;
		
		if (index != bigger) {
			void* temp = heap->vect[index];
			heap->vect[index] = heap->vect[bigger];
			heap->vect[bigger] = temp;
		}
		else {
			break;
		}
		
		index = bigger;
		
		left = 2 * index + 1;
		right = 2 * index + 2;
	}
	
}

void heap_insert (void* data, heap_v* heap) {
	
	heap_append (data, heap);
	heapify_up (heap);
	
}

void* heap_extractMax (heap_v* heap) {
	void *data;
	
	if (heap->last < 0)
		return NULL;	
	
	data = heap->vect[0];
	
	heap->vect[0] = heap->vect[heap->last];
	heap->last--;
	
	heapify_down (heap);
	
	return data;		
}

int maior (void* _a, void* _b) {
	
// 	printf ("Estou dentro da função\n");
	
	int a = *((int*) _a);
	int b = *((int*) _b);
	
// 	printf ("Fiz os casts\n");
	
	if (a > b)
		return 1;
	else
		return 0;
}

int main () {
	int i, *novo;
	
	heap_v* my_heap = heap_create(16, &maior);
	
	for (i = 0; i < 16; i++) {
		novo = malloc(sizeof(int));
		*novo = rand() % 256;
		printf ("Inserindo o novo elemento %d\n", *novo);
		heap_insert((void*) novo, my_heap);
	}
	
	heap_extractMax (my_heap);
	heap_extractMax (my_heap);
	heap_extractMax (my_heap);
	
	sweep(my_heap);
	
}


void sweep (heap_v* heap) {
	int i;
	for (i = 0; i <= heap->last; i++) {
		printf ("Elemento\t%d\t=\t%d\n",i, *((int*) heap->vect[i]));
	}
}