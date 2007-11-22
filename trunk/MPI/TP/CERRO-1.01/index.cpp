/* File Name: index.cpp
 * Date: Nov. 07, 2005
 * Team: Cerro Porteno
 * Project Name: Speedy, Parallel Text Search Using MPI
 * Author: John Moe
 */
 
/*This file implements indexing and searching.
 *
 *                        INDEXING
 *For each possible hash value, a linked-list [possibly empty] of words
 *with that hash value is maintained.  Each word then contains an array
 *of document ids that contain that word.
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

#include "common.h"
#include "index.h"
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "listOfWordAndDocIds.h"

using namespace std;

#define NUMBER_OF_BITS 21  //the hash table is 2^NUMBER_OF_BITS long
                           //this number should probably be bigger than
                           //the number of distinct words. 
#define SIZE_OF_TABLE (1<<NUMBER_OF_BITS)


unsigned long g_IndexWords = 0;

//FNV-1 modified to return a NUMBER_OF_BITS hash.
//http://www.isthe.com/chongo/tech/comp/fnv/
unsigned long Hash(const char* word,const char* end)
{
  unsigned long hash = 2166136261;
  while (word != end) {
    hash ^= (*word);
    hash *= 16777619;
    word ++;
  } 
  return (hash >> NUMBER_OF_BITS) ^ (hash & (SIZE_OF_TABLE-1));
} 

//One instance of this class will be created for each distinct word indexed.
//We keep track of the word and which documents it is in.
class WordDetails
{
   public:
   //constructor takes the word and thefirst document it appears in
   WordDetails(const char* word,const char* end,unsigned long docId,unsigned long wordNumber)
   {
      int len = end - word + 1;
      m_pWord = new char[len+1];
      m_pWord[0] = len;
      memcpy(m_pWord+1,word,len);
      m_pNext = NULL; 
      AddDocument(docId,wordNumber);  
   }
   ~WordDetails()
   {
      delete m_pNext;
      delete [] m_pWord;
   }
   
   bool Equals(const char* word,const char* end)
   {
     int len = (end - word) + 1;
     
     return ((len == m_pWord[0]) && (0 == memcmp(word,m_pWord+1,len)));
   }
   
   //Adds a new document to the list of documents for a word.
   void AddDocument(long docId,unsigned long wordId)
   {
     m_wordAndDocIdList.Add(docId,wordId);
   }  
   char *m_pWord;
   ListOfWordAndDocIds m_wordAndDocIdList;
   WordDetails* m_pNext;
};


//This class stores a map of words to a WordDetail class instance
class WordToWordDetailsMap 
{
   public:
   WordToWordDetailsMap()
   {  
      m_pTable = new WordDetails*[SIZE_OF_TABLE]; 
      CHECK_ALLOC(m_pTable,"WordToWordDetailsMap constructor");   
      memset(m_pTable,0,sizeof(*m_pTable)*SIZE_OF_TABLE);
   }
   
   ~WordToWordDetailsMap()
   {
      for (int i=0; i < SIZE_OF_TABLE; i++) {
         delete m_pTable[i];
      }
      delete [] m_pTable;
      m_pTable = NULL;
   }

   //Find returns NULL if the word is not found.
   WordDetails* Find(unsigned long hash, const char* word,const char* end)
   {
       WordDetails * ret = m_pTable[hash];
       while (ret) {
          if (ret->Equals(word,end)) {
             break; 
          }
          ret = ret->m_pNext;
       }
       return ret;
   }
   
   //Adds a new WordDetails class instance to the map.
   //Ownership of the newWord is now held by the map.
   //Do not add the same word twice.
   void Add(unsigned long hash, WordDetails* newWord)
   {
      if (NULL == m_pTable[hash]) {
         m_pTable[hash] = newWord;
      }
      else {
         WordDetails * prev = m_pTable[hash];
         WordDetails * next = prev->m_pNext;
         while (next != NULL) {
            prev = next;
            next =  prev->m_pNext;
         }
         prev->m_pNext = newWord;
      }
   } 

   WordDetails ** m_pTable;
};

WordToWordDetailsMap *g_pWordToWordDetailsMap = NULL;

//This function is used from outside of this file to add a word to the index.
//DocumentIDs must not ever decrease.
void AddWord(const char* Word,const char* End, unsigned long DocumentID,unsigned long WordId)
{
   if ((End - Word) > 250) {
     return;
   }
   g_IndexWords ++;
   unsigned long hash = Hash(Word,End);
   WordDetails* wd = g_pWordToWordDetailsMap->Find(hash,Word,End);
   if (NULL == wd) {
      wd = new WordDetails(Word,End,DocumentID,WordId);
      CHECK_ALLOC(wd,"WordDetails in AddWord");
      g_pWordToWordDetailsMap->Add(hash, wd);
   }
   else {
      wd->AddDocument(DocumentID,WordId);
   }
}

//Finds all documents containing a word.  Used by DoQuery.
void FindPhrase2(char** Word,int wordCount, Match* &pMatches,unsigned long& MatchCount)
{
   WordDetails** wd = new WordDetails*[wordCount];
   ListOfWordAndDocIdsIterator** iter = new ListOfWordAndDocIdsIterator*[wordCount];

   int maxMatches = INT_MAX;
   for (int i=0;i < wordCount; i++) {
      int len = strlen(Word[i]);
      char* w = new char[len+1];
      for (int j=0; j<len;j++) {
         w[j] = tolower(Word[i][j]);
      }
      w[len] = 0;

      unsigned long hash = Hash(w,w+strlen(w)-1);
      wd[i] = g_pWordToWordDetailsMap->Find(hash,w,w+strlen(w)-1);
      delete [] w;
      if (NULL == wd[i]) {
         MatchCount = 0;
         pMatches = NULL;
         delete [] wd;
         return;
      }
      iter[i] = new ListOfWordAndDocIdsIterator(&wd[i]->m_wordAndDocIdList); 
      maxMatches = maxMatches > wd[i]->m_wordAndDocIdList.m_count ? wd[i]->m_wordAndDocIdList.m_count : 
      maxMatches;  
   }
   MatchCount = 0;
   pMatches = new Match[maxMatches];//worst case
   int maxDocId = -1;
   int maxWordId = -1;
 
   bool bPastEndOfAnyList = false;
   for (int i = 0; i < wordCount; i++) {
     iter[i]->GetNext(bPastEndOfAnyList);
     if (maxDocId < iter[i]->m_docId) {
        maxDocId = iter[i]->m_docId;
        maxWordId = iter[i]->m_wordId - i;
     }
     if (maxDocId == iter[i]->m_docId && maxWordId < (iter[i]->m_wordId - i)) {
        maxWordId = iter[i]->m_wordId - i;
     }
   }

   while (!bPastEndOfAnyList) {
     int origMaxDocId = maxDocId;
     int origMaxWordId = maxWordId;
     for (int i = 0; i < wordCount; i++) {
        while (((origMaxDocId > iter[i]->m_docId) || 
          (origMaxDocId == iter[i]->m_docId && origMaxWordId > (iter[i]->m_wordId - i)))) {
           iter[i]->GetNext(bPastEndOfAnyList);
           if (maxDocId < iter[i]->m_docId) {
              maxDocId = iter[i]->m_docId;
              maxWordId = iter[i]->m_wordId - i;
           }
           if (maxDocId == iter[i]->m_docId && maxWordId < (iter[i]->m_wordId - i)) {
              maxWordId = iter[i]->m_wordId - i;
           }
        }
     }
     if (origMaxDocId == maxDocId && origMaxWordId == maxWordId) {
       pMatches[MatchCount++].DocumentId = maxDocId;
       maxDocId ++;
       maxWordId = -1;
     }
   }
}

void Initialize2()
{
   g_pWordToWordDetailsMap = new WordToWordDetailsMap(); 
   g_IndexWords = 0;
}


void Cleanup2()
{
   delete g_pWordToWordDetailsMap;
}




/*      Parsing (tokenizing) stuff... 
 *
 *      Words are located and converted to lowercase here.
 *      AddWord is called for each word found.
 *
 *
 *
 */


void Parse2(char *p,int length,int fileid)
{
  int currentWord = 0;
  char lowercaseWord [2000];
  char * lc = NULL;

  char ch; // current character
  char* word = NULL; // the current word is built here
  char* tag = NULL;  // the current tag is built here

  char * pos = p; //we keep track of each character
  char * end = p + length;
  while (pos != end) //to parse all the characters
  {
    ch = *pos; //increment to parse the next character
    if (word) {
      char nch = ValidChar[ch];
      if (nch) {
        *lc++ = nch;
      }
      else {
        AddWord(lowercaseWord,lc-1,docid,currentWord);
        currentWord ++;
        word = NULL;
      }
    }
 
    else if (tag) {
      //"<DOC
      if ((pos - tag) == 3) {
        if (('D' == tag[1]) && ('O' == tag[2]) && ('C' == tag[3])) {
          DocumentList[docid].startbyte = tag - p;
          DocumentList[docid].fileid = fileid;
        }
      }
      if ('>' == ch) {
        //</DOC
        if ((pos - tag) == 5) {
          if (('/' == tag[1]) && ('D' == tag[2]) && ('O' == tag[3]) && ('C' == tag[4])) {
            DocumentList[docid].endbyte = pos - p + 1;//include /n
            docid++;
            currentWord = 0;
          }
        }
        tag = NULL;
      }
    }
    else {
      char nch = ValidChar[ch];
      if (nch) {
        lc = lowercaseWord;
        *lc++ = nch;
        word = pos;
      }
      else if ('<' == ch) {
        tag = pos;
      }
    }
    pos++;
  }
}


/*
This exists as both an example of how to use these classes and also to
enable testing without involving MPI.

#include <sys/time.h>
#include "search.h"
#include "master.h"

void (*findphrase) (char**, int, Match* &, unsigned long &);

int main()
{	
   char* buffer = new char[100000000];
   //FILE* fp = fopen("/scratch1/cs862108/GIGAWORD-ENGLISH/apw/apw200109","r");
   
   FILE* fp = fopen("/scratch1/cs862108/GIGAWORD-ENGLISH/nyt/nyt199605","r");
   unsigned long len = fread(buffer,1,100000000,fp);

   Initialize2();
   findphrase = FindPhrase2;   

   //printf("HELLO\n");
   
   timeval startTime,endTime;
   gettimeofday(&startTime,NULL);
   Parse2(buffer,len, 1);  
   gettimeofday(&endTime,NULL);

   printf("Time to index: %f\n\n",endTime.tv_sec - startTime.tv_sec + (endTime.tv_usec - 
        startTime.tv_usec)/1000000.0);
   //cout << memUsed << endl;
   //cout << memUsed2 << endl;
   cout << g_IndexWords << endl;

   //return 0;
   Match* m;
   unsigned long count;
   DoQuery("\"Washington D C\" \"NEW YORK\"",m,count);
   printf("%d\n\n\n",count);
   for (int i=0; i < count; i++) {
      printf("DocID: %d    FileId: %d   from %d - %d\n",m[i].DocumentId,
                                           DocumentList[m[i].DocumentId].fileid,
                                           DocumentList[m[i].DocumentId].startbyte,
                                           DocumentList[m[i].DocumentId].endbyte);
   }
   return 0;
}



*/
