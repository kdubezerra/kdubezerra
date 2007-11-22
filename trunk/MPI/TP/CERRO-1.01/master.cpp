/* File Name: master.cpp
 * Date: Nov. 07, 2005
 * Author: Vikram Malik
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


/* This file implements the root processor, and all the communication
 * file distributin, and receiving and reducing the results that goes
 * on between the root and the slave processors.
 *
 * The root begins with scanning the specified number of directories 
 * (specified as an integer argument to the program) and buils a list
 * of all the files, that would need to be indexed, in the descending 
 * order of their sizes.
 * Next it implements a centralized load balancer. The root waits for
 * a signal from the slave indicating that its idle and needs work. It 
 * then reads the next file from its list of files, and sends it to the 
 * requesting slave. Once all the files have been sent to the slaves,
 * and are indexed, the root sends a termination signal to the slaves 
 * indicating the same so that the slaves do not send any more request
 * signals indicating their idle state back to the root.
 */

#include "mpi.h"
#include "master.h"
#include "index.h"
#include "common.h"
#include "search.h"

using namespace std;

/*Globals and externs*/
#define NO_OF_THREADS numprocs-1
extern int total_files;
extern char *chunks[1000];
struct file_n_size **file_array;

int no_of_threads, no_of_slaves;

/* Function Pointers for different configurations of indexing*/
void (*initialize)(void);
void (*cleanup) (void);
void (*parse) (char*, int, int);
void (*findphrase) (char**, int, Match* &, unsigned long &);

/* Function Pointers for different configurations of load-balancer*/
void (*distribute) (struct thread_id **, int, int );
void (*process) (int, int);


/* Timing variables*/
/* These macros and variables calculate the total wallclock time for the program*/
double proc_started, proc_finished, total_time;
#define START_TIMING proc_started  = MPI_Wtime();
#define END_TIMING   proc_finished = MPI_Wtime(); \
					 total_time    += proc_finished - proc_started;

extern double total_rnd_time;

double indexBuildingTime = 0;
double indexing_start_time= -1;


/* Main function begins*/
int  main(int argc, char* argv[])
{
	double indexBuildingStartTime;
	double searchTime = 0;
	double searchStartTime;
   double indexing_end_time;
   
	int myid, numprocs;
	//struct query allqueries[100];
	char *finished;
	int i, j, k, l, m, p, count, endloop=0;
	i=j=k=l=p=count=0;
	FILE *fp;
	//int total_queries=0
        int index_type, balancer_type;

	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
	MPI_Comm_rank(MPI_COMM_WORLD, &myid);
        START_TIMING

	ROOT
	{
		time_t now = time(NULL);
		struct tm * ltime = localtime (&now);
		printf ("Start Time: %d-%02d-%02d %02d:%02d:%02d\n",
			1900+ltime->tm_year, 
			1 + ltime->tm_mon,
			ltime->tm_mday,
			ltime->tm_hour,
			ltime->tm_min,
			ltime->tm_sec);
		printf ("Processor Count: %d\n",numprocs);
		printf ("\n");                  
	}
 
	TRACEIN
	/*InitTraceFile(myid);*/
	TRACEOUT

	int target_percent = 100;
	if (2 == argc) {
		target_percent = atoi(argv[1]);
	}

	initialize = Initialize2;
	cleanup = Cleanup2;
	parse = Parse2;
	findphrase = FindPhrase2;
	distribute = distributor_threads2;
	process = process_files2;
	
	/*Initialize variables required for indexing*/
	(*initialize)();

	ROOT
	{		
		char *corpus;
		/* Extract the corpus path from runit.sh, and initialize the directory 
		 * names for directories to be indexed 
		 */
		file_array = (struct file_n_size**) malloc(sizeof( struct file_n_size*) * MAX_FILES);
		corpus = (char*) malloc(sizeof(char)*200);
		corpus = get_corpus_path();

		TRACEIN
		printf("\n\n***********CORPUS PATH   %s***********\n\n", corpus);
		TRACEOUT

		/* Scan the mentioned directories to build a list of files, contained in 
		 * the directory, and its subdirectories (if any)
		 */
		dir_scan(file_array, corpus);

		free(corpus);
		
		double totalSize = 0;
		for (int i = 0; i < total_files; i++) {
			totalSize += file_array[i]->size;
		}
		
		cout << "Total Size: " << totalSize << " bytes" << endl;
		cout << "File Count: " << total_files << endl;
      int subsetSize = 0;
      int targetSize = int(double(target_percent) * totalSize / 100);
      //cout << "Target Size: " << targetSize << endl;
      for (int i = 0; i < total_files; i++) {
         subsetSize += file_array[i]->size;
         if (subsetSize >= targetSize) {
            total_files = (i+1);
         	break;
       	}
       }
      if (target_percent < 100) {
      	cout << "** ONLY USING FIRST " << target_percent << " PERCENT OF THE DATA. **" << endl;
      	cout << "   Subset Size: " << subsetSize << " bytes" << endl;
      	cout << "   Subset File Count: " << total_files << endl;
      }
                                                                                                            
		
		/* At this point I have an array of structure pointers 
		 * with names, path and sizes of each of the files in each directory.
		 * Now sort the array structure on file size in descending order
		 */
		sort_by_filesize(file_array, total_files);

		/* Now the ROOT reads the queries to search for from INPUT file.*/			
		//total_queries = getQueries(allqueries);
		
		//TRACEIN
/*		//printf("\n\n Total queries is %d \n\n", total_queries);
		//for(int o=0; o<total_queries; o++)
		//{
		//	printf("\n query is %s", allqueries[o].findwhat);
		//}
		//printf("\n\nI have all the queries, proceeding to threads to read and distribute files now \n\n*******");
		//fflush(stdin);*/
		//TRACEOUT
	}/*End of ROOT block*/



	/* Load-Balancing - This comprises reading all the files that were scanned
	 * by dir_scan(), and then distributing them to the slaves for parsing. I 
	 * create multiple root threads here (15 or the number of slaves, whichever
	 * is greater) to read the files in parallel. Every message sent of received
	 * by a root thread is tagged with message_tag = myid%no_of_root_threads. The
	 * same approach is followed by the slaves to communicate with a particular
	 * root thread. The number 15 is chosen as an upper bound for the number of 
	 * root threads since this gave me the best time, and requesting more than 
	 * 15 threads results in heap overflow with all those threads requesting for 
	 * memory simultaneously. This approach helped us ditribute the load evenly, 
	 * and reduce the time spent in reading and distributing the file by about 
	 * 80 percent
	 */

	no_of_slaves = numprocs-1;
	no_of_threads = (no_of_slaves<25)?no_of_slaves:25; 

	/* To find the read and distribute time, I time only the the root thread
	 * that would be the last to terminate
	 */	
	//last_to_terminate = no_of_slaves%no_of_threads;


	ROOT
	{	
		struct thread_id **data;
		data = (struct thread_id **) malloc(sizeof(struct thread_id *) * no_of_threads);
		(*distribute)(data, no_of_threads, no_of_slaves);
	}
	
	SLAVES
	{
		(*process)(myid, no_of_threads);
	}
	
	ROOT
	{
      indexing_end_time = MPI_Wtime();
      
		TRACEIN
		printf("%%%%%%%%%ALL THREADS TERMINATED!!! MOVING ON%%%%%%%%%%%%%%%%%");
		TRACEOUT
	}
	
	double timeToIndexStart = ( indexingStartTime - proc_started );
	
	if (indexingStartTime < 0){
	  timeToIndexStart = 1000000; //a big number
	}
	MPI_Reduce(&timeToIndexStart,&timeToIndexStart,1,MPI_DOUBLE,MPI_MIN,0,MPI_COMM_WORLD);
	

	/* Indexing is complete, now I start sending the queries to the 
	 * slaves. I send all the queries to every slave, which then 
	 * searches for the queries in its part of the index.
	 */

	ROOT
	{
		generate_results(file_array, numprocs);
	}
	SLAVES
	{			 
		char * buffer = NULL;
		int query_len;
		
		while (1) 
		{
			MPI_Bcast(&query_len,1,MPI_INT,0,MPI_COMM_WORLD);
			if (0 == query_len) {
				break;
			}
			char* this_query = new char[query_len];
         MPI_Bcast(this_query,query_len,MPI_CHAR,0,MPI_COMM_WORLD);
         double startTime = MPI_Wtime();
			Match* pMatches;
			int* temp =	new	int[2];
			unsigned long numMatches;
			searchStartTime	= MPI_Wtime();
			DoQuery(this_query,pMatches,numMatches);
			temp[0]	= numMatches;
			int totLen = 0;
			for (int m=0; m	< numMatches; m++) 
			{
				totLen += (DocumentList[pMatches[m].DocumentId].endbyte	- 
					DocumentList[pMatches[m].DocumentId].startbyte	+ 1);
			}
			char * buffer =	new	char[totLen];
			int	p =	0;
			for	(int m=0; m < numMatches; m++) 
			{
				memcpy(&buffer[p],
					chunks[DocumentList[pMatches[m].DocumentId].fileid] +	
						DocumentList[pMatches[m].DocumentId].startbyte,
					DocumentList[pMatches[m].DocumentId].endbyte	- 
					  DocumentList[pMatches[m].DocumentId].startbyte + 1);
				p +=	(DocumentList[pMatches[m].DocumentId].endbyte -	
					DocumentList[pMatches[m].DocumentId].startbyte + 1);
			} 
			searchTime += (MPI_Wtime() - searchStartTime);
			MPI_Request	req;	
			temp[1]	= totLen;
			MPI_Send(temp,2,MPI_INT,0,1,MPI_COMM_WORLD);
			MPI_Send(buffer,totLen,MPI_CHAR,0,2,MPI_COMM_WORLD);
			double endTime = MPI_Wtime();
			double seqTime = endTime - startTime;
			MPI_Reduce(&seqTime,&seqTime,1,MPI_DOUBLE,MPI_SUM,0,MPI_COMM_WORLD);
			delete [] buffer;
			delete [] pMatches;
			delete [] this_query;
 			this_query = NULL;
		}
	}/*End of slaves block*/

	
	/* Statistics: Now I get the document count, number of words indexed, 
	 * total communication and file read time, total index building time
	 * and total search time.
	 */

	unsigned long totalIndexWords;
	unsigned long totalDocCount;
	double totalIndexBuildingTime;
	double totalSearchTime;
	MPI_Reduce(&g_IndexWords,&totalIndexWords,1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
	MPI_Reduce(&docid,&totalDocCount,1,MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
	MPI_Reduce(&indexBuildingTime, &totalIndexBuildingTime, 1, MPI_DOUBLE,MPI_SUM,0,MPI_COMM_WORLD);
	MPI_Reduce(&searchTime, &totalSearchTime, 1, MPI_DOUBLE,MPI_SUM,0,MPI_COMM_WORLD);
   


	END_TIMING

	/* The master processor now prints the document count, word count, time to 
	 * index all files, time for communication and file-read, and the time to 
	 * search the queries.
	 */
	ROOT
	{
		cout << "Total number of words: " << totalIndexWords << endl;
		cout << "Total number of documents: " << totalDocCount << endl;
		cout << endl;
		cout << "Read and distribute time: " << total_rnd_time << endl;
		cout << "Alternate read and distribute time: " << timeToIndexStart << endl;
		cout << endl;
		cout << "Total sequential time to create the index: " << totalIndexBuildingTime << endl;
      cout << "Total parallel time to create index:       " << (indexing_end_time - indexing_start_time ) << endl;
      cout << endl;
		cout << "Total wall-clock time for the program: " << total_time << endl;
	} 

	(*cleanup)();
	MPI_Finalize();
	return (0);
}/*End of main()*/
