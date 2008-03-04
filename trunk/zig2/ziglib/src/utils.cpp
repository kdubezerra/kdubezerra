/*
		ZIG game-oriented networking engine
		Project Home: http://zige.sourceforge.net

		Copyright (c) 2002-2005, Fábio Reis Cecin.
		All rights reserved.
    
		This free software is licensed under a BSD-style license. Read the 
		LICENSE.TXT file for the full license.
*/
/*

	LEETNET 2

	utils

*/

	//temporary code - for debugging
	//#include <allegro.h>
	//#include <winalleg.h>
	//#include "../include/zig/console.h"
	//#include "../../minigame/conalleg.h"
	//extern conalleg_c *con;


#include "zig/utils.h"
#include <stdio.h>
#include <math.h>

using namespace std;

// socket statistics -- a bunch of NO-OP name wrappers for the HawkNL functions
// -------------------------------------------------------------------------------

//get current average bandwidth usage for both incoming and outgoing traffic, 
// in bytes per second (all sockets)
int zig_avg_total() { 
	nlEnable(NL_SOCKET_STATS);  
	return (int)(nlGetInteger(NL_AVE_BYTES_SENT)+nlGetInteger(NL_AVE_BYTES_RECEIVED));
}

//get current average bandwidth usage for outgoing traffic, 
// in bytes per second (all sockets)
int zig_avg_out() { 
	nlEnable(NL_SOCKET_STATS);  
	return (int)(nlGetInteger(NL_AVE_BYTES_SENT));
}

//get current average bandwidth usage for incoming traffic, 
// in bytes per second (all sockets)
int zig_avg_in() { 
	nlEnable(NL_SOCKET_STATS);  
	return (int)(nlGetInteger(NL_AVE_BYTES_RECEIVED));
}

// from utils.cpp
extern int zig_global_packet_loss_percent;
extern int zig_global_packet_delay_base;
extern int zig_global_packet_delay_delta;

void zig_emulate_network_packet_loss(int drop_chance_percent) 
{
	// CRAP FILTER: any value not between 0 and 100 percent is changed to 0 percent - disables the emulator
	if ((drop_chance_percent < 0) || (drop_chance_percent > 100)) {

		// CRAP: disable
		zig_global_packet_loss_percent = 0;
	}
	else {
		// percent between 0 and 100
		zig_global_packet_loss_percent = drop_chance_percent;
	}
}

void zig_emulate_network_latency(int min_delay_ms, int max_delay_ms) 
{
	// CRAP FILTER - if crap is provided, disable the emulator
	if ( 
				(min_delay_ms < 0) || 
				(max_delay_ms < 0) ||
				(min_delay_ms > max_delay_ms) 
		 )
	{
		// CRAP: disable
		zig_global_packet_delay_base = 0;
		zig_global_packet_delay_delta = 0;
	}
	else {

		//example: min=100, max=300;  base=100  delta=300-100=200
		zig_global_packet_delay_base = min_delay_ms;
		zig_global_packet_delay_delta = (max_delay_ms - min_delay_ms);
	}
}


// global compression gain statistics & compression setup
// -------------------------------------------------------------------------------

int zig_bzip2_compression_level = 1;

// set compression level (accepts values between 1 and 9, inclusive) for the BZip2 
// packet compression. 1 = fastest compression, 9 = best compression. the default 
// compression level for BZip2 is 1.
void zig_set_compression_level(int level) {

	// limit to bzip2 levels
	if (level < 1) level = 1;
	if (level > 9) level = 9;

	zig_bzip2_compression_level = level;
}

//statistics counters for compression
int zig_stat_zipped_bytes = 1;
int zig_stat_normal_bytes = 1;

//get the current compression gain, in percent(%) being obtained with both incoming and 
//outgoing UDP traffic
double zig_avg_compression_gain() { return ((double)(zig_stat_normal_bytes - zig_stat_zipped_bytes) / (double)zig_stat_normal_bytes) * 100.0; }

//reset compression gain statistics
void zig_reset_compression_stats() { zig_stat_zipped_bytes = zig_stat_normal_bytes = 1; }

//-----------------------

//check for private IP
bool check_private_IP(char *address) {
	//192.168.x.x +++ 172.16.x.x ATÉ 172.31.x.x +++ 10.x.x.x : bloquear
	// ZIG 2.0: catch localhost also (127.*)
	char cbuf[512];
	char cbuf2[512];
	strcpy(cbuf, address);
	cbuf[8]=0;
	if (!strcmp(cbuf, "192.168.")) {  // 192.168.
		return true;
	}
	else {
		strcpy(cbuf, address);
		cbuf[3]=0;
		if (!strcmp(cbuf, "10.")) { // 10.
			return true;
		}
		else {

			strcpy(cbuf, address);
			cbuf[4]=0;
			if (!strcmp(cbuf, "127.")) { // 127.
				return true;
			}
			else {
				strcpy(cbuf, address);
				cbuf[7]=0;
				for (int ipn=16;ipn<32;ipn++) {
					sprintf(cbuf2, "172.%i.", ipn); // 172.16. -- 172.32.
					if (!strcmp(cbuf, cbuf2))
						return true;
				}
			}
		}
	}
	//not private
	return false;
}

//test if an IP address is "private-class"
bool check_private_IP(NLaddress &address) {

	char addr[1024];
	addr[0]=0;
	nlAddrToString(&address, addr);
	return check_private_IP(addr);
}

// increment the packet id, wrapping around appropriately
int inc_packet_id(NLushort *id) {

	if (id == 0) return -1;

	//wrap around id: (1..16320)
	if ((*id) == ZIG_LAST_PACKET_ID) (*id) = ZIG_FIRST_PACKET_ID;
	else (*id)++;

	return (*id);
}

// check if new_id <= base_id
bool older_packet_id(NLushort new_id, NLushort base_id) {

	// TEORIA: nenhum id numa sequencia vai ir muito mais longe do que 100, 200... entao se o newid
	//         possuir uma "distancia" de no maximo 8160 (metade do ID MAXIMO), da pra inferir se ele
	//         esta' antes ou depois, usando um range limitado

	// case base_id is in the upper half of the range
	if (base_id > ZIG_HALF_PACKET_ID) {
		if (new_id >= base_id) return false;
		else if ( (base_id - new_id) > ZIG_HALF_PACKET_ID ) return false;
		else return true;
	}
	// case base_id in the down half of the range
	else {
		if (new_id < base_id) return true;
		else if ( (new_id - base_id) > ZIG_HALF_PACKET_ID ) return true;
		else return false;
	}
}

// diff of two message ids
int diff_packet_id(NLushort new_id, NLushort current_id) {

	int diff = new_id - current_id;
	if (diff < 0) diff = -diff;
	if (diff > ZIG_HALF_PACKET_ID) diff = ZIG_LAST_PACKET_ID - diff;
	return diff;
}
// ZIG 2: new message id API

// increment the message id, wrapping around appropriately
int inc_msg_id(NLushort *id) {

	if (id == NULL) return -1;

	// wrap around id (0..32767)
	if ((*id) == ZIG_LAST_MSG_ID) (*id) = ZIG_FIRST_MSG_ID;
	else (*id)++;

	return (*id);
}

// check if new_id <= base_id
bool older_msg_id(NLushort new_id, NLushort base_id) {

	// TEORIA: nenhum id numa sequencia vai ir muito mais longe do que 100, 200... entao se o newid
	//         possuir uma "distancia" de no maximo 8160 (metade do ID MAXIMO), da pra inferir se ele
	//         esta' antes ou depois, usando um range limitado

	// case base_id is in the upper half of the range
	if (base_id > ZIG_HALF_MSG_ID) {
		if (new_id >= base_id) return false;
		else if ( (base_id - new_id) > ZIG_HALF_MSG_ID ) return false;
		else return true;
	}
	// case base_id in the down half of the range
	else {
		if (new_id < base_id) return true;
		else if ( (new_id - base_id) > ZIG_HALF_MSG_ID ) return true;
		else return false;
	}
}

// diff of two message ids
int diff_msg_id(NLushort new_id, NLushort current_id) {

	int diff = new_id - current_id;
	if (diff < 0) diff = -diff;
	if (diff > ZIG_HALF_MSG_ID) diff = ZIG_LAST_MSG_ID - diff;
	return diff;
}

//string tokenizer (see utils.h)
// retorna: -1 se erro de sintaxe, 0 se string vazia, >0 se string ok
// cmd: pointer to pointer to current token reading position in the command line (is modified)
// buf: buffer for function return (the single token extracted)
// aceitaaspas: if the command line must extract tokens with spaces but enclosed in doublequotes
//							like:     "this is a single token"
//
int getcmd(char* &cmd, char *buf, bool doubleQuote)
{
/*	- pula todos espacos e tabs
	- se proximo é um " entao
		varre até um " 
		- se encontrar end-of-string (\0) da' erro de sintaxe
		- se encontrar um \", traduz pra " no buf
		- se depois do " nao tiver um espaco, tab, ou fim de string, da' erro de sintaxe tb
	- senao
		varre tudo ate' end-of-string ou espaco ou tab
*/
	int len = 0;
   char bla;
	bool slash = false, aspa = false;

	// pula todos espacos e tabs
   while ((*cmd==' ') || (*cmd=='\t'))	cmd++;

	// se proximo e' uma aspas, entao eh string cte incluindo separadores ate' outro "
	if (*cmd=='"')
	{
		// se nao aceita aspas como 1o caractere == syntax error
		if (!doubleQuote)	return -1;

		// pula aspa inicial
		cmd++;			

		// enquanto nao acaba a string do comando
		while ( (bla = *(cmd++)) != '\0')
		{
			len++;	// +1 char

			if (bla == '\\')
			{
				if (slash) // adicao de 1 slash normal no buffer apenas
				{
					*(buf++)='\\';
					slash = false;
				}
				else
					slash = true;
			}
			else if (bla=='"') //break;
			{
				if (slash)	// slash: mata o \, adicao da aspa apenas
				{
					*(buf++)=bla;
					slash = false;
				}
				else			// achou a " final
				{
					aspa = true;
					break;
				}
			}
			else // copia normal
			{
				if (slash) // "desempilha" o slash e poe ele antes do caractere
				{
					*(buf++)='\\';
					slash = false;
				}
				*(buf++)=bla;
			}
		}
		if (!aspa) // syntax error: fim de string sem "
			return -1;  

		// syntax error: " não-seguido de fim de string,
		// espaco ou tab (letra grudada em aspa que fecha um parametro)
		if ((*cmd != '\0') && (*cmd != ' ') && (*cmd != '\t'))
			return -1;
	}
	// tudo até o separador, como era antes
	else
	{
		// enquanto nao acaba a string do comando
		while ( ((bla = *cmd) != '\0') )
		{
			cmd++;
			len++;	// +1 char
			if ((bla==32) || (bla=='\t')) break;
			*(buf++)=bla;
		}
	}
	// termina string no buffer
	*buf='\0';
	// sucessful se nao eh cmd vazio
	return len;
}

//=========================================================
// RANDOM NAME routine by Renato Hentschke
//=========================================================

int Random(int velho)
{
  return rand()%velho;
}

string Vog()
{
	int prob = Random(100);
	if (prob>=0 && prob<=30)
	  return "a";
	else
	if (prob>=31 && prob<=35)
	  return "y";
	else
	if (prob>=36 && prob<=55)
	  return "e";
	else
	if (prob>=56 && prob<=66)
	  return "i";
	else
	if (prob>=67 && prob<=90)
	  return "o";
	else
	if (prob>=91 && prob<=99)
	  return "u";
	else
		return "";//never happens
}


string ConsProlSemiVog()
{
	if (Random(100)<20)
	{
		if (Random(100)<30)
		{
			int prob = Random(100);
			if (prob>=0 && prob<=45)
				return "i";
			if (prob>=46 && prob<=85)
				return "u";
			if (prob>=86 && prob<=95)
				return "y";
			if (prob>=96 && prob<=99)
				return "w";
		}
		else
		{
			int prob = Random(63);
			if (prob>=0 && prob<=10)
				return "h";
			if (prob>=11 && prob<=30)
				return "l";
			if (prob>=31 && prob<=50)
				return "r";
			if (prob>=51 && prob<=57)
				return "s";
			if (prob>=59 && prob<=62)
				return "z";
		}
	}

	return "";
}


string PreVog()
{
	string prevog;

	if (Random(101)<87)
	{
		int prob = Random(100);
		if (prob>=0 && prob<=4)
		  prevog = "b";
		if (prob>=5 && prob<=8)
		  prevog = "c";
		if (prob>=9 && prob<=13)
		  prevog = "d";
		if (prob>=14 && prob<=18)
		  prevog = "f";
		if (prob>=19 && prob<=23)
		  prevog = "g";
		if (prob>=24 && prob<=27)
		  prevog = "h";
		if (prob>=28 && prob<=31)
		  prevog = "j";
		if (prob>=32 && prob<=35)
		  prevog = "k";
		if (prob>=36 && prob<=40)
		  prevog = "l";
		if (prob>=41 && prob<=45)
		  prevog = "m";
		if (prob>=46 && prob<=50)
		  prevog = "n";
		if (prob>=51 && prob<=55)
		  prevog = "p";
		if (prob>=56 && prob<=58)
		  prevog = "q";
		if (prob>=59 && prob<=63)
		  prevog = "r";
		if (prob>=64 && prob<=68)
		  prevog = "s";
		if (prob>=69 && prob<=73)
		  prevog = "t";
		if (prob>=74 && prob<=78)
		  prevog = "v";
		if (prob>=79 && prob<=83)
		  prevog = "w";
		if (prob>=84 && prob<=88)
		  prevog = "x";
		if (prob>=89 && prob<=92)
		  prevog = "z";
		prevog = prevog + ConsProlSemiVog();
	}
	else
		prevog = "";

	return prevog;
}

string PosSemiVog()
{
	if (Random(101)<13)
	{
		int prob = Random(100);
		if (prob>=0 && prob<=45)
		  return "i";
		if (prob>=46 && prob<=85)
		  return "u";
		if (prob>=86 && prob<=95)
		  return "y";
		if (prob>=96 && prob<=99)
		  return "w";
	}

	return "";
}

string PosConsProl()
{
	if (Random(100)<18)
	{
		int prob = Random(169);
		if (prob>=0 && prob<=2)
		  return "b";
		if (prob>=3 && prob<=5)
		  return "c";
		if (prob>=6 && prob<=8)
		  return "d";
		if (prob>=9 && prob<=11)
		  return "f";
		if (prob>=12 && prob<=14)
		  return "g";
		if (prob>=15 && prob<=30)
		  return "h";
		if (prob>=31 && prob<=32)
		  return "j";
		if (prob>=33 && prob<=36)
		  return "k";
		if (prob>=37 && prob<=55)
		  return "l";
		if (prob>=56 && prob<=74)
		  return "m";
		if (prob>=75 && prob<=92)
		  return "n";
		if (prob>=93 && prob<=95)
		  return "p";
		if (prob>=96 && prob<=98)
		  return "q";
		if (prob>=99 && prob<=116)
		  return "r";
		if (prob>=117 && prob<=136)
		  return "s";
		if (prob>=137 && prob<=140)
		  return "t";
		if (prob>=141 && prob<=143)
		  return "v";
		if (prob>=141 && prob<=143)
		  return "v";
		if (prob>=144 && prob<=154)
		  return "w";
		if (prob>=155 && prob<=156)
		  return "x";
		if (prob>=157 && prob<=168)
		  return "z";
	}

	return "";
}

string Silaba()
{
	string retorno;

	retorno = PreVog() + Vog() + PosSemiVog() + PosConsProl();
	 
	return retorno;
}


string Palavra()
{
	int nsil;

	string palavra;
	palavra = "";

	int prob = Random(100);

	if (prob>=0 && prob<=10)
		nsil = 1;
	else
	if (prob>=11 && prob<=75)
		nsil = 2;
	else
	if (prob>=76 && prob<=99)
		nsil = 3;
	else
		nsil = 4; //never happens

	for (int r=0; r< nsil; r++)
		palavra = palavra + Silaba();

	palavra[0] = palavra[0] - 32;

	return palavra;
}

string RandomName(int npal)
{
  int prob;
	string nome;

	if (npal<1)
	{
		prob = Random(100);

		if (prob<=50)
			npal=1;
		else
		if (prob<=95)
			npal=2;
		else
			if (prob<=99)
			npal=3;
		else
			npal=4; //never happens.
	}

	nome = "";
	for (int r=0; r<npal-1; r++)
		nome = nome + Palavra() + "_";  //fabio: mudei p/ nao acrescentar espacos nos nomes
	nome = nome + Palavra();

	return nome;
}

//return a string object cointaining an int
string itoa(int val) {
	char blah[64];
	sprintf(blah, "%i", val);
	string str = blah;
	return str;
}

//return a string object containing a double
string ftoa(float val) {
	char blah[64];
	sprintf(blah, "%f", val);
	string str = blah;
	return str;
}

//converts string object to int
int atoi(string str) {
	return atoi(str.c_str());
}

//converts string object to double
double atof(string str) {
	return atof(str.c_str());
}

//round double to the nearest integer
int droundi(double v) {

	double f = floor(v);
	double c = ceil(v);
	if (v >= 0) {
		if ((c - v) < (v - f)) // distance to ceil smaller than distance to floor
			return (int)c;
		else
			return (int)f;
	}
	else {
		if ((c - v) < (v - f)) // distance to ceil smaller than distance to floor
			return (int)f; // inverted
		else
			return (int)c; // inverted
	}
}
