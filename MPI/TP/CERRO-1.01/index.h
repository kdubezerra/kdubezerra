/* File Name: index2.h
 * Date: Nov. 07, 2005
 * Team: Cerro Porteno
 * Project Name: Speedy, Parallel Text Search Using MPI
 * Author: John Moe
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

#ifndef __INDEX2_H_INCLUDED

#define __INDEX2_H_INCLUDED

void Initialize2();
void Cleanup2();

void Parse2(char*,int,int);

void FindPhrase2(char**,int,Match*&, unsigned long&);

//Adds a word to the index.  The word must be lowercased.  
//The DocumentIDs must be non-decreasing.  
void AddWord2(const char* Word,const char* End, unsigned long DocumentID,unsigned long wordId,unsigned long hash);

extern unsigned long g_IndexWords;

#endif
