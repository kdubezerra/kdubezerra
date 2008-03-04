/*
	-------------------------------------------------------------------------
	-- NL THIGH -------------------------------------------------------------
	-------------------------------------------------------------------------
	---	COISA LIXOSA PRA COLOCAR COMPATIBILIDADE DA HAWKNL 1.7 COM O ZIGE ---
	-------------------------------------------------------------------------
*/

#ifndef _NL_THIGH_H_
#define _NL_THIGH_H_

#include <hawkthreads.h>

typedef HTmutex NLmutex;
inline int nlMutexInit(NLmutex* mutex)		{ return(htMutexInit(mutex)); }
inline int nlMutexDestroy(NLmutex* mutex)	{ return(htMutexDestroy(mutex)); }
inline int nlMutexLock(NLmutex* mutex)		{ return(htMutexLock(mutex)); }
inline int nlMutexUnlock(NLmutex* mutex)	{ return(htMutexUnlock(mutex)); }

typedef HThreadID NLthreadID;
typedef HThreadFunc NLthreadFunc;
inline NLthreadID nlThreadCreate(NLthreadFunc func, void* data, int joinable)	{ return(htThreadCreate(func, data, joinable)); }
inline int nlThreadJoin(NLthreadID threadID, void** status)						{ return(htThreadJoin(threadID, status)); }

#endif // _NL_THIGH_H_
