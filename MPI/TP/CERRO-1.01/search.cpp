/* File Name: search.cpp
 * Date: Dec 2005
 * Team: Cerro Porteno
 * Project Name: Speedy, Parallel Text Search Using MPI
 * Author: John Moe
 */

/* This file takes a query string in DoQuery and breaks it into 
 * the FindPhrase calls that index?.cpp will handle. It then merges the
 * result lists.  In adition to search terms, queries can contain 
 * phrases, AND, OR and parentheses.  When no operator is specified, AND 
 * is assumed.  DoQuery is not tolerant of malformed queries.
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
#include <pthread.h>

#include "search.h"  // 
#include "common.h"  // Match, etc
#include "master.h"  // findphrase
#include <vector>

using namespace std;

//Merges two sorted match lists returning matches that appear in both lists.
//Used by DoQuery.
void MergeAnd(const Match* firstList, unsigned long firstCount,
              const Match* secondList, unsigned long secondCount,
              Match* &pMatches,unsigned long& MatchCount)
{
   int maxMatches = firstCount > secondCount?firstCount:secondCount;
   pMatches = new Match[maxMatches];
   CHECK_ALLOC(pMatches,"Match[] in MergeAnd");
   int f = 0;
   int s = 0;
   MatchCount = 0;
   while (f!=firstCount && s!=secondCount) {
      if (firstList[f].DocumentId == secondList[s].DocumentId) {
         pMatches[MatchCount++].DocumentId = firstList[f].DocumentId;
         f++;
         s++;
      }
      else if (firstList[f].DocumentId > secondList[s].DocumentId) {
         s++;
      }
      else {
         f++;
      }
   }
}

//Merges two sorted match lists returning matches that appear in either list.
//Used by DoQuery.
void MergeOr(const Match* firstList, unsigned long firstCount,
              const Match* secondList, unsigned long secondCount,
              Match* &pMatches,unsigned long& MatchCount)
{
   int maxMatches = firstCount +  secondCount;
   pMatches = new Match[maxMatches];
   CHECK_ALLOC(pMatches,"Match[] in MergeOr");
   int f = 0;
   int s = 0;
   MatchCount = 0;
   while (f!=firstCount || s!=secondCount) {
      if (f == firstCount) {
         pMatches[MatchCount++].DocumentId = secondList[s].DocumentId;
         s++;
      }
      else if (s == secondCount) {
         pMatches[MatchCount++].DocumentId = firstList[f].DocumentId;
         f++;
      }
      else if (firstList[f].DocumentId == secondList[s].DocumentId) {
         pMatches[MatchCount++].DocumentId = firstList[f].DocumentId;
         f++;
         s++;
      }
      else if (firstList[f].DocumentId > secondList[s].DocumentId) {
         pMatches[MatchCount++].DocumentId = secondList[s].DocumentId;
         s++;
      }
      else {
         pMatches[MatchCount++].DocumentId = firstList[f].DocumentId;
         f++;
      }

   }
}
#define MODE_OR 1
#define MODE_AND 2

void Merge(int mode, Match*& pMatches,  unsigned long&  MatchCount,
                     Match* newMatches, unsigned long  newMatchCount)
{
  Match* prevMatches = pMatches;
  unsigned long prevMatchCount = MatchCount;

  if (MODE_OR == mode) {
    MergeOr(newMatches,newMatchCount,prevMatches,prevMatchCount,
            pMatches,MatchCount);
  }
  else {
    MergeAnd(newMatches,newMatchCount,prevMatches,prevMatchCount,
             pMatches,MatchCount);
  }
  delete [] newMatches;
  delete [] prevMatches;
}

struct ThreadData
{
  pthread_t Thread;
  Match * pMatches;
  unsigned long MatchCount;
  char Mode;
  
  char* Query;
  
  char** Phrase;
  int WordCount;
};

void* SearchThread(void* d)
{
  ThreadData *td = (ThreadData*)d;
  if (td->Phrase) {
    findphrase(td->Phrase,td->WordCount,td->pMatches,td->MatchCount);
  }
  else {
    DoQuery(td->Query,td->pMatches,td->MatchCount);
  }
  return NULL;
}

void DoPhraseSearch(vector<ThreadData*> &tdv, char** phrase, int wc, int mode)
{
  ThreadData* td = new ThreadData;
  td->Mode = mode;
  td->Phrase = new char*[wc];
  td->Query = NULL;
  for (int i=0; i < wc; i++) {
    td->Phrase[i] = new char[strlen(phrase[i])+1];
    strcpy(td->Phrase[i], phrase[i]);
  }
  td->WordCount = wc;
  
  pthread_create(&td->Thread,NULL,SearchThread,td);
  tdv.push_back(td);
}
void DoSubQuerySearch(vector<ThreadData*> &tdv, char* query, int mode)
{
  ThreadData* td = new ThreadData;
  td->Mode = mode;
  td->Phrase = NULL;
  td->Query = query;
  td->WordCount = 0;
    
  pthread_create(&td->Thread,NULL,SearchThread,td);
  tdv.push_back(td);
}
                    

//Handles user queries.
//This function will be called recursively to handle queries with parentheses.
//In initially starts with an empty result set and the mode set to OR.
//The mode is reset to AND after each search term.  Results are combined with
//the existing result set using the current mode.  The mode can be changed by
//including AND or OR in the query.
void DoQuery(const char* query, Match* &pMatches,unsigned long& MatchCount)
{
   vector<ThreadData*> tdv;
   char** phrase = new char*[1000];
   for (int i=0; i < 1000; i++) {
      phrase[i] = new char[1000];
   }

   char *w = phrase[0];
   pMatches = NULL;
   MatchCount = 0;
   char mode = MODE_OR;
   const char* q = query;
   bool bInPhrase = false;
   int wordNumber = 0;

   while (1) {
      if (IsValidChar(*q)) {
         *w++ = tolower(*q++);
         *w = NULL;
         continue;
      }
      //All the rest is for characters that are not valid
      if ('"' == *q) {
         if (bInPhrase) {
            DoPhraseSearch(tdv,phrase,wordNumber+1,mode);
            mode = MODE_AND;

            bInPhrase = false;
            wordNumber = 0;
            w = phrase[0];
         }
         else {
            bInPhrase = true;
            wordNumber = 0;
         }
         q++;
         continue;
      }
      if ('(' == *q) {
         const char* q2 = q+1;
         int depth = 1;
         while(*q2 != ')' || depth != 1) {
            if (*q2 =='(') {
                depth++;
            }
            if (*q2 == ')') {
               depth--;
            }
            q2++;
         }
         char* newQuery = new char[q2-q];
         CHECK_ALLOC(newQuery,"newQuery in DoQuery");
         memcpy(newQuery,q+1,q2-q-1);
         newQuery[q2-q-1] = 0;
         DoSubQuerySearch(tdv,newQuery,mode);
         mode = MODE_AND;
         q = q2;
      }
      if (w != phrase[wordNumber]) {
         //we have a word
         if (bInPhrase) {
            wordNumber++;
            q++;
            w = phrase[wordNumber];
            continue;
         }
         if (0 == strcmp("or",phrase[0])) {
            mode = MODE_OR;
         }
         else if (0 == strcmp("and",phrase[0])) {
            mode = MODE_AND;
         }
         else {
            DoPhraseSearch(tdv,phrase,1,mode);
            mode = MODE_AND;
         }
         wordNumber = 0;
         w = phrase[0];
      }
      if (*q == NULL) {
         break;//all done!
      }
   q++;
   }
   void * val;
   pthread_join(tdv[0]->Thread,&val);
   pMatches = tdv[0]->pMatches;
   MatchCount = tdv[0]->MatchCount;
   for (int i = 1; i < tdv.size(); i++) {
     pthread_join(tdv[i]->Thread,&val);
     Merge(tdv[i]->Mode,pMatches,MatchCount,tdv[i]->pMatches,tdv[i]->MatchCount);
   }
   for (int i = 0; i < tdv.size(); i++) {
     delete [] tdv[i]->Query;
     for (int j = 0; j < tdv[i]->WordCount; j++) {
       delete [] tdv[i]->Phrase[j];
     }
     delete [] tdv[i]->Phrase;
     delete tdv[i];
   }
}



