/* File Name: master.cpp
 * Date: Nov. 07, 2005
 * Principal author: Vikram Malik		
 * Team: Cerro Porteno
 * Project Name: Speedy, Parallel Text Search Using MPI
 * Developers: Vikram Malik, John Moe, Ajit Marathe, Vinayak Patil
 */

/* Copyright (c) 2005, Vikram Malik, John Moe, Ajit Marathe, Vinayak Patil of Team
 * Cerro Porteno of University of Minnesota Duluth (CS8621)
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 * Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 * Redistributions in binary form must reproduce the above copyright notice, this
 * list of conditions and the following disclaimer in the documentation and/or
 * other materials provided with the distribution.
 * Neither the name of the University of Minnesota Duluth nor the names of its contributors may
 * be used to endorse or promote products derived from this software without
 * specific prior written permission.
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef _MASTER_H_
#define _MASTER_H_

/*Includes*/
#include <pthread.h>
//#include "mpi.h"
#include "common.h"
#include "index.h"
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/dir.h>
#include <dirent.h>
#include <fstream>
#include <iostream>

/*Macro Definitions*/
#define MAX_FILES 1000
#define ROOT if(myid==0)
#define SLAVES if(myid != 0)
#define TERMINATION_TAG  2
#define EXIT_PROGRAM -1

/*Debug checks*/
#define DEBUG 0  /*set DEBUG to 1 to enable the debugging printf's*/
#define TRACEIN if(DEBUG){
#define TRACEOUT }

/*Globals and externs*/
#ifdef MPI_VERSION
MPI_Status status;	
MPI_Request request;
#endif
struct file_n_size
{
	char fname[50];
	char fpath[300];
	unsigned long size;
};
extern struct file_n_size **file_array;

extern double indexBuildingTime;
extern double indexing_start_time;
extern double indexingStartTime;

/* Function Pointers for different configurations of indexing*/
extern void (*initialize)(void);
extern void (*cleanup) (void);
extern void (*parse) (char*, int, int);
extern void (*findphrase) (char**, int, Match* &, unsigned long &);


/*Structure passed to each root thread*/
struct thread_id
{
	int id;
	int slaves;
	int threads;
};

struct query
{
	char findwhat[500];
};

/*Function Prototypes*/
void sort_by_filesize(struct file_n_size**, int);
int getQueries(struct query queries[]);
void distribute_files(struct file_n_size **, int, int);
void dir_scan (struct file_n_size**, char*);
char* get_corpus_path();
void generate_results(struct file_n_size **,int);
void* readfile(void*);
void distributor_threads2(struct thread_id **data, int no_of_threads, int no_of_slaves);
void process_files2(int, int);

#endif
