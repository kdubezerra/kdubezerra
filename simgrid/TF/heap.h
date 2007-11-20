#ifndef _HEAP_H_
#define _HEAP_H_

#include <stdio.h>
#include <stdlib.h>

typedef struct heap_v_struct {
	void** vect;
	int last;
	int (*f) (void*, void*);
} heap_v;


heap_v* heap_create (int n, void *func/*int (*func) (void*, void*)*/);
void heap_append (void* data, heap_v* heap);
void heapify_up (heap_v* heap);
void heap_insert (void* data, heap_v* heap);
void* heap_extractMax (heap_v* heap);

void sweep (heap_v* heap);

#endif
