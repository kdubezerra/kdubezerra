/* File Name: master_routines.cpp
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

#include "mpi.h"
#include "master.h"
#include "index.h"
#include "common.h"
#include "/usr/local/include/zlib.h"

int total_files=0;
static int file_counter = 0;
pthread_mutex_t mutex;
extern struct file_n_size **file_array;
char *chunks[1000];



double rnd_start;
double rnd_end;
double total_rnd_time;
int ct_root=1;
double indexingStartTime = -1;

/*Function definitions*/

/* This functions sorts the array of file_n_size structure pointers, 
 * to yield a list of files sorted in the descending order of their 
 * size
 */
void sort_by_filesize(struct file_n_size **base_addr, int no_of_files)
{
	int i,j;
	i=j=0;
	
	struct file_n_size **temp;
	temp = (struct file_n_size **) malloc(sizeof(struct file_n_size*));

	for(i=0; i<no_of_files; i++)
	{
		for(j=i+1; j<no_of_files; j++)
		{
			if(base_addr[i]->size < base_addr[j]->size)
			{	*temp = base_addr[i];
				base_addr[i] = base_addr[j];
				base_addr[j] = *temp;
			}
		}
	}

	TRACEIN
	for(i=0; i<no_of_files; i++)
	{
		printf(" sort by size: fname:%s size:%d fpath:%s\n", base_addr[i]->fname, base_addr[i]->size, base_addr[i]->fpath);
	}
	TRACEOUT
}

/* This function reads the path of the corpus from the runit.sh script 
 * and returns it to the root
 */
char* get_corpus_path()
{
		FILE* runit;
		char  *temp1, *temp2;
		char store[100];
		temp1 = (char*) malloc(sizeof(char) * 300);
		temp2 = (char*) malloc(sizeof(char) * 300);
		if ( !(runit = fopen("runit.sh", "r")))
		{
			printf("Runit.sh not found. Exiting.");
			exit(0);
		}
		while(!strstr(fgets(temp1, 300, runit), "#corpus_path")) {};
		temp2 = strstr(temp1, "/");
		strstr(temp2,"\n")[0] = '\0'; /*get rid of the trailing newline*/
		fclose(runit);
		return(temp2);
}


/* This is a recursive function that scans the directory containing the corpus, 
 * as specified by the user in runit.sh. It builds a list of files present in 
 * the directory. If a subdirectory is found, it scans all the files 
 * within the subdirectory before proceeding furthur with the next file.
 * This function requires the corpus name specified in runit.sh to end 
 * with a '/'
 */
void dir_scan (struct file_n_size** array, char* directory)
{
	DIR* dir_p;
	dirent* dir_entry_p;
	char *filename, *path;
	struct stat stat_p;
	int j =0;

	filename = (char*) malloc(sizeof(char) * 50);
	path = (char*) malloc(sizeof(char) * 300);

   TRACEIN
	printf("DIRECTORY received in dir_scan is %s\n", directory);
   TRACEOUT

	if (!(dir_p = (DIR*) opendir(directory)))
	{
		printf("Failed to open directory %s\n", directory);
	}
	strcpy(path,"");
        while((dir_entry_p = (dirent*) readdir( (DIR*) dir_p)) != NULL)
	{
		strcpy(filename, dir_entry_p->d_name);
 		if( (strcmp(filename, ".") == 0) || (strcmp(filename, "..") == 0) ) continue;
		strcat(path, directory);
		strcat(path, filename);
		strcat(path, "/");
		j =  stat(path, &stat_p);
		if(S_ISDIR(stat_p.st_mode))
		{
			dir_scan(array, path);
			strcpy(path, "");
			continue;
		}
		array[total_files] = (struct file_n_size*) malloc(sizeof(struct file_n_size));
		strcpy( array[total_files]->fname, (const char*)filename);
		strcpy( array[total_files]->fpath, (const char*)directory);
		array[total_files]->size = stat_p.st_size;
		TRACEIN
		printf("dir_scan fpath-%s; size- %d; stat=%d; stat-error= %s\n", array[total_files]->fpath, array[total_files]->size, j, strerror(errno)); 
		printf("total_files = %d\n", total_files);
		TRACEOUT
		/*reset path*/
		strcpy(path, "");
		total_files++;
	}
	closedir((DIR*) dir_p);
	free(filename);
	free(path);
}/*end of the scan function*/



/* This function is called by the root, and generates the result
 * files for each query based on the results returned by the slave
 * processors
 */
void generate_results(struct file_n_size **file_array, int numprocs)
{
	char* pDocs; 
	char query[10000]; //TODO: Make Dynamic!
	int	temp[2]; 
   FILE * fp;
   double totalParallelTime = 0;
   double totalSequentialTime = 0;
   
   if(!(fp = fopen("INPUT", "r")))
   {
   	printf("******Failed to open query file*******");
   	exit(0);
   }
   int q=0; //query number
   int qlen = 0;
   while(fgets(query, sizeof(query), fp)!= NULL)
	{
		strstr(query,"\n")[0] = '\0';/*get rid of the \n at end of each string*/
		int qlen = strlen(query)+1;
		MPI_Bcast(&qlen,1,MPI_INT,0,MPI_COMM_WORLD);
		MPI_Bcast(query,qlen,MPI_CHAR,0,MPI_COMM_WORLD);
		double startTime = MPI_Wtime();
		char fileName[100];
		sprintf(fileName,"results/results.%d",q); 
		FILE* outfp	= fopen(fileName,"a+");
		int totalMatches = 0;
		for	(int i=1; i	< numprocs; i++) 
		{
			//START_TIMING
				MPI_Recv(temp,2,MPI_INT,-1,1,MPI_COMM_WORLD,&status);
			//END_TIMING
			totalMatches += temp[0];
			pDocs =	new	char[temp[1]];
			//START_TIMING
				MPI_Recv(pDocs,temp[1],MPI_CHAR,status.MPI_SOURCE,2,MPI_COMM_WORLD,&status);
			//END_TIMING
			double ioStart = MPI_Wtime();
			fwrite(pDocs,temp[1],1,outfp);
			double ioEnd = MPI_Wtime();
			startTime += (ioEnd - ioStart);
			delete [] pDocs;
		}
		double endTime = MPI_Wtime();
		double seqTime = 0;
		MPI_Reduce(&seqTime,&seqTime,1,MPI_DOUBLE,MPI_SUM,0,MPI_COMM_WORLD);
		cout << "Total sequential time for query # " << q << ": " << seqTime << endl;
		cout << "Total parallel time for query # " << q << ":   " << (endTime - startTime) << endl; 
		cout << "Total results for query # " << q << ":         " << totalMatches << endl;
		cout << endl;
      totalParallelTime += (endTime - startTime);
      totalSequentialTime += seqTime;
		fclose(outfp);
		q++;
	}
	cout << "Total sequential time for all queries: " << totalSequentialTime << endl;
	cout << "Total parallel time for all queries  : " << totalParallelTime << endl;
	cout << endl;
	qlen = 0;
	MPI_Bcast(&qlen,1,MPI_INT,0,MPI_COMM_WORLD);
	fclose(fp);
}/*End of generate_result function*/



/*root threads' start function*/
void* readfile(void* arg)
{
	FILE* fp;
	char *file_path;
	struct thread_id *temp;
	int fileid=0, val=0;
	int threadid, slave_count, thread_count, tag, loop, remaining;
	int root_arr[2];
	char* chunk;	
	MPI_Status thread_status;	

	temp = (struct thread_id *)arg;
	threadid = temp->id;
	slave_count = temp->slaves;
	thread_count = temp->threads;

	tag = threadid%thread_count;
	
	TRACEIN
	printf("\n I am TID %d, tag=%d\n", threadid, tag);
	fflush(stdin);
   TRACEOUT
   
	while(file_counter < total_files)
	{

		/*counter will need mutex..*/
		pthread_mutex_lock(&mutex);
			if(file_counter>=total_files) {
			  pthread_mutex_unlock(&mutex);
			  break;
			}
			fileid = file_counter;
			file_counter++;
		pthread_mutex_unlock(&mutex);
		
		TRACEIN
		//printf("\n\n*****THREAD ID IS %d******total files- %d, file_counter-%d\n", tid, total_files, fileid);
		//printf("Thread %d is waiting for recv from slave\n", tid);
		TRACEOUT
		
		
		
		chunk = (char*) malloc(sizeof(char) * file_array[fileid]->size);
		CHECK_ALLOC(chunk,"File chunk");
		file_path = (char*) malloc(sizeof(char) * 300);
		CHECK_ALLOC(file_path,"File_path");
		
		/*construct the path and open the file*/
		strcpy(file_path, file_array[fileid]->fpath);
		strcat(file_path, file_array[fileid]->fname);
		if(!(fp = fopen(file_path, "r")))
		{
			printf("\n\n*********Failed to open hello file %s********\n\n", file_path);
		}
	
		/* About to send the file to slave so start the read and distrbute timer*/
      if (indexing_start_time < 0) {
        indexing_start_time = MPI_Wtime();
      }
		if( (threadid==1) && (ct_root==1))
		{
			rnd_start = MPI_Wtime();
			TRACEIN
			printf("\n\n **** rnd_start is  %f\n\n", rnd_start);
			TRACEOUT
		//	indexing_start_time = MPI_Wtime();
			
		}
			
		/* About to send the file to slave so start the read and distrbute timer*/
/*		if((threadid == 1) && (counting_root == 1))
		{
			RND_START
			counting_root++;
			indexing_start_time = MPI_Wtime();
		}
*/
		int read = fread(chunk, 1, file_array[fileid]->size, fp);
      MPI_Recv(&val, 1, MPI_INT, MPI_ANY_SOURCE, tag, MPI_COMM_WORLD, &thread_status);
      root_arr[0] = file_array[fileid]->size;
      root_arr[1] = fileid;
      MPI_Send(root_arr, 2, MPI_INT, thread_status.MPI_SOURCE, tag, MPI_COMM_WORLD);
                        
                        		
      MPI_Send(chunk, file_array[fileid]->size, MPI_CHAR, thread_status.MPI_SOURCE, tag, MPI_COMM_WORLD);

		if( (threadid==1) && (ct_root==1))
		{
			rnd_end = MPI_Wtime();
			TRACEIN
			printf("\n\n **** rnd_end is  %f\n\n", rnd_end);
			TRACEOUT
			total_rnd_time = rnd_end-rnd_start;
			TRACEIN
			printf("\n\n **** rnd_diff is  %f\n\n", total_rnd_time);
			TRACEOUT
			ct_root++;
		}

		TRACEIN
		printf("***Thread %d sends file (no-%d) %s, totalFiles=%d, slave-%d, tag-%d , total_done-%d\n\n", 
			threadid, fileid, file_array[fileid]->fname, total_files, thread_status.MPI_SOURCE, tag, file_counter);
		fflush(stdin);
		TRACEOUT
		free(chunk);
		free(file_path);
		fclose(fp);
	}

	/* All the files have been sent now, lets send the termination message. If number 
	 * of slave threads is greater than or is not an exact multiple of, the number the 
	 * slaves, their might be some slaves root threads that were communicating with 
	 * one more slave that others. So that has to be taken care of here
	 */
	
	/* loop is the count of no of threads each root thread was communicating with. 
	 * So it has to terminate these
	 */
	loop = slave_count/thread_count;
	for(int i=0; i<loop; i++)
	{
		MPI_Recv(&val, 1, MPI_INT, MPI_ANY_SOURCE, tag, MPI_COMM_WORLD, &thread_status);
		root_arr[0] = -51;
		root_arr[1] = 0;
		MPI_Send(root_arr, 2, MPI_INT, thread_status.MPI_SOURCE, tag, MPI_COMM_WORLD);
		TRACEIN
		printf("Sent termination message to slave %d\n", thread_status.MPI_SOURCE);
		fflush(stdin);
		TRACEOUT
	}

	/* remaining is the count of root threads that had to deal with one more 
	 * slave that the other root threads. So it terminates those here
	 */
	remaining = (slave_count%thread_count);
	if(threadid<=remaining)
	{
		MPI_Recv(&val, 1, MPI_INT, MPI_ANY_SOURCE, tag, MPI_COMM_WORLD, &thread_status);
		val = -51;
		MPI_Send(&val, 1, MPI_INT, thread_status.MPI_SOURCE, tag, MPI_COMM_WORLD);
		TRACEIN
		printf("Sent termination message to slave %d\n", thread_status.MPI_SOURCE);
		fflush(stdin);
		TRACEOUT
	}
	return((void*)NULL);
}

void UnGzipIfNeeded(char* & data, int & size)
{
  if (data[0] != 31 || data[1] != 139) {
    return;
    }
  int newLen = data[size-4] | data[size-3]<<8 | data[size-2] << 16 | data[size-1] << 24;
  char* newBuf = (char*)malloc(newLen);
  z_stream strm;
  
  strm.next_in = (unsigned char*)data;
  strm.avail_in = size;
  
  strm.next_out = (unsigned char*)newBuf;
  strm.avail_out = newLen;
      
  strm.zalloc = NULL;
  strm.zfree = NULL;
  strm.opaque = NULL;
 
  inflateInit2 (&strm, 31); //31 = gzip 
  inflate(&strm,Z_FINISH);
  inflateEnd(&strm);
 
  free (data);
  data = newBuf;
  size = newLen;
}

/* This function is called by the slaves to receive the files (work) sent to 
 * them by the root processor. These files are the parsed and indexed
 */
void process_files2(int slaveid, int thread_count)
{
	/* Slaves send a signal for work to the root, receive a message from root in response, 
	 * check if its the termination message. If it is, theyr terminate, else procedd with
	 * indexing the chunk. Once done, they again send a signal back to root to get more 
	 * work
	 */
	int slave_arr[2], val;
	double indexBuildingStartTime;
	MPI_Status slave_status;
	int tag = slaveid%thread_count;
	TRACEIN
	printf("I am slave %d, tag-%d\n", slaveid, tag);
	TRACEOUT
	while(1)
	{
		val = 1;
        MPI_Send(&val, 1, MPI_INT, 0, tag, MPI_COMM_WORLD);
		MPI_Recv(slave_arr, 2, MPI_INT, 0, tag, MPI_COMM_WORLD, &slave_status);
		if(slave_arr[0]==-51) 
		{
			TRACEIN
			printf("Received termination message from root \n");	
			fflush(stdin);
			TRACEOUT
			break;
		}
		chunks[ slave_arr[1] ] = (char*) malloc(sizeof(char)*slave_arr[0]);
        MPI_Recv(chunks[ slave_arr[1] ], slave_arr[0], MPI_CHAR, 0, tag, MPI_COMM_WORLD, &slave_status);
		
		TRACEIN
		printf("Slave %d received file from root\n", slaveid);
		fflush(stdin);
		TRACEOUT
		  UnGzipIfNeeded(chunks[ slave_arr[1] ],slave_arr[0]);
        indexBuildingStartTime = MPI_Wtime();
        if (indexingStartTime < 0) {
          indexingStartTime = MPI_Wtime();
        }
        (*parse)(chunks[ slave_arr[1] ], slave_arr[0], slave_arr[1]);
        indexBuildingTime += (MPI_Wtime() - indexBuildingStartTime); 
	}
}



/* This function creates no_of_threads root threads for the load balancer*/
void distributor_threads2(struct thread_id **data, int no_of_threads, int no_of_slaves)
{
	pthread_t threads[no_of_threads];
	int ret;
	for(int si=0; si<no_of_threads; si++)
	{
		data[si] = (struct thread_id *) malloc(sizeof(struct thread_id));		
		data[si]->id = si+1; //thread id's start at base 1.
		data[si]->threads = no_of_threads;
		data[si]->slaves = no_of_slaves;
		ret = pthread_create(&threads[si], NULL, readfile, (void*) data[si]);
	}
	for(int si=0; si<no_of_threads; si++)
	{
		ret = pthread_join(threads[si], (void**) NULL);
	}
}


/*Function definitions end*/
