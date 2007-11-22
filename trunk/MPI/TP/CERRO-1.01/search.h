#ifndef __SEARCH_H_INCLUDED__

#define __SEARCH_H_INCLUDED__

#include "common.h"

//Handles user queries.
//This function will be called recursively to handle queries with parentheses.
//In initially starts with an empty result set and the mode set to OR.
//The mode is reset to AND after each search term.  Results are combined with
//the existing result set using the current mode.  The mode can be changed by
//including AND or OR in the query.
void DoQuery(const char* query, Match* &pMatches,unsigned long& MatchCount);



#endif
