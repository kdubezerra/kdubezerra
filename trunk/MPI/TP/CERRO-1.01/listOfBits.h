/* File Name: listOfBits.h
 * Date: Dec 28, 2005
 * Team: Cerro Porteno
 * Project Name: Speedy, Parallel Text Search Using MPI
 * Author: John Moe
 */

/*This file gives us a list of bits.  
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

class ListOfBits
{
  public:
  ListOfBits()
  {
    m_pBits = NULL;
    m_used = 0;
    m_size = 0;
  }
  ~ListOfBits()
  {
    free (m_pBits);
  }
  void Grow()
  {
    unsigned long newSize = m_size * 2;
    if (newSize < 128 ) {
      newSize = 128;
    }
    unsigned long* bits = (unsigned long*)calloc(newSize>>5,4);
    CHECK_ALLOC(bits,"BitList Growing");
    memcpy(bits,m_pBits,(m_size >> 3));
    free (m_pBits);
    m_pBits = bits;
    m_size = newSize;
  }
  void AppendZeroBits(int numBits)
  {
     if(m_used + numBits > m_size) {
        Grow();
     }
     m_used += numBits;
  }
  void AppendInteger(unsigned long num, int numBits)
  {
     if(m_used + numBits > m_size) {
        Grow();
     }
     unsigned long* thisVal = &m_pBits[m_used>>5];
     int bitsLeft = 32 - (m_used&31);
     m_used += numBits;
     if (numBits > bitsLeft) {
       unsigned long partOfNum = num >> (numBits - bitsLeft);
       *thisVal |= partOfNum;
       numBits -= bitsLeft;
       bitsLeft = 32;
       thisVal++;
     }
     *thisVal |= (num << (bitsLeft - numBits));
  }
  unsigned long m_used;
  unsigned long m_size;
  unsigned long* m_pBits;
};

class ListOfBitsIterator
{
  public:
  ListOfBitsIterator(ListOfBits* pList)
  {
    m_pList = pList;
    m_pos = 0;
    bitsLeft = 0;
  }
  bool GetNext()
  {
    if (0 == bitsLeft) {
      bitsLeft = 32;
      val = m_pList->m_pBits[m_pos >> 5];
    }
    m_pos ++;
    bitsLeft --;
    bool r =  (1 == (val >> 31));
    val = val << 1;
    return r;
  }
  bool EOL()
  {
    return m_pos == m_pList->m_used;
  }
  bool BOL()
  {
    return 0 == m_pos;
  }
  ListOfBits* m_pList;
  unsigned long m_pos;
  int bitsLeft;
  unsigned long val;;
};


