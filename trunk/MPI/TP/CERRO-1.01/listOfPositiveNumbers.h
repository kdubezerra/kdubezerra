/* File Name: listOfPositiveNumbers.h
 * Date: Dec 28, 2005
 * Team: Cerro Porteno
 * Project Name: Speedy, Parallel Text Search Using MPI
 * Author: John Moe
 */

/* This file gives us a list of positive numbers.
 * They are store using Elias Gamma Coding using a listOfBits.
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

#include "listOfBits.h"

//Inspired by http://graphics.stanford.edu/~seander/bithacks.html
unsigned char SignificantBits(unsigned long num)
{
  static unsigned char table[256] = { 0,1,2,2,3,3,3,3,4,4,4,4,4,4,4,4,
                                      5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,
                                      6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
                                      6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
                                      7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
                                      7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
                                      7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
                                      7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
                                      8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
                                      8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
                                      8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
                                      8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
                                      8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
                                      8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
                                      8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
                                      8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8};
  if (num >> 8) {
    if (num >> 16) {
      if (num >> 24) {
        return 24 + table[num >> 24];
      }
      else {
        return 16 + table[num>>16];
      }
    }
    else {
      return 8 + table[num>>8];
    }
  }
  else {
    return table[num];
  }
}

class ListOfPositiveNumbers
{
  public:
  void Add(unsigned long num)
  {
    unsigned char sigBits = SignificantBits(num);
    m_bits.AppendZeroBits(sigBits-1);
    m_bits.AppendInteger(num,sigBits);
  }
  ListOfBits m_bits;
};

class ListOfPositiveNumbersIterator
{
  public:
  ListOfPositiveNumbersIterator(ListOfPositiveNumbers* pList)
    : bi(&pList->m_bits)
  {
  }
  unsigned long GetNext()
  {
    int b = 0;
    unsigned long val = 1;
    while (0 == bi.GetNext()) {
      b++;
    }
    for (int i = 0; i < b; i ++) {
      val = val << 1;
      val |= bi.GetNext();
    }

    return val;
  }
  bool EOL()
  {
    return bi.EOL();
  }
  bool BOL()
  {
    return bi.BOL();
  }
  ListOfBitsIterator bi;
};

