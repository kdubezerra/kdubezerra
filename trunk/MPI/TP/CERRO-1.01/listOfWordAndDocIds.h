/* File Name: listOfPositiveNumbers.h
 * Date: Dec 28, 2005
 * Team: Cerro Porteno
 * Project Name: Speedy, Parallel Text Search Using MPI
 * Author: John Moe
 */

/* This file gives us a list of Doc and Word Id pairs.
 * They are store using a listOfPositiveNumbers.
 *
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

#include "listOfPositiveNumbers.h"

class ListOfWordAndDocIds
{
  public:
  ListOfWordAndDocIds()
  {
    m_maxDocId = -1;
    m_lastWordId = -1;
    m_count = 0;
  }
  void Add(unsigned long docId, unsigned long wordId)
  {
    int distance = docId - m_maxDocId;
    if (0 != distance) { //a new docId
      m_lastWordId = -1;
      if (m_maxDocId > -1) {
        m_numbers.Add(1);//A 1 signals the next number is a document id
      }
    }
    m_maxDocId = docId;
    int wordDistance = wordId - m_lastWordId;
    m_lastWordId = wordId;
    if (distance > 0) {
      m_numbers.Add(distance);
      m_numbers.Add(wordDistance+1);//reserve 1 for a new doc id
    }
    else {
      m_numbers.Add(wordDistance+1);
    }
    m_count ++;    
  }
  int m_maxDocId;
  int m_lastWordId;
  int m_count;
  ListOfPositiveNumbers m_numbers;
};
                      
class ListOfWordAndDocIdsIterator
{
  public:
  ListOfWordAndDocIdsIterator(ListOfWordAndDocIds* pList)
    : m_numbersIterator(&pList->m_numbers)
  {
    m_docId = -1;
  }
  bool EOL()
  {
    return m_numbersIterator.EOL();
  }
  void GetNext(bool & eol)
  {
    if (m_numbersIterator.BOL()) {
      m_docId += m_numbersIterator.GetNext();
      m_wordId = m_numbersIterator.GetNext() - 2;
    }
    else if (m_numbersIterator.EOL()) {
      m_docId = INT_MAX;
      m_wordId = INT_MAX;
      eol = true;
    }
    else {
      long temp = m_numbersIterator.GetNext();
      if (1 == temp) {
        m_docId += m_numbersIterator.GetNext();
        m_wordId = m_numbersIterator.GetNext() - 2;
      }
      else {
        m_wordId += (temp - 1);
      }
    }
  }
  
  ListOfPositiveNumbersIterator m_numbersIterator;
  int m_docId;
  int m_wordId;
};
