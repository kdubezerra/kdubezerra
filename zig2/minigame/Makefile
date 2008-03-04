OBJS=conalleg.o gameserver.o gameclient.o main.o
HEADERS=aconalleg.h gameserver.h gameclient.h main.h

EXENAME=minigame

ifndef CXX
	CXX=g++
endif

DEBUGMODE=1
ifdef DEBUGMODE
	CXXFLAGS=-O0 -g3 -fPIC -DPIC
	LIBNAME=zigd
else
	ifndef CXXFLAGS
		CXXFLAGS=-O2 -g0 -fPIC -DPIC
	endif
	LIBNAME=zig
endif

all: ${EXENAME}

${EXENAME}: ${OBJS}
	${CXX} -o ${EXENAME} ${OBJS} -lNL `allegro-config --libs` -l${LIBNAME}

conalleg.o: conalleg.cpp conalleg.h
	${CXX} -o conalleg.o -c conalleg.cpp ${CXXFLAGS}

gameserver.o: gameserver.cpp gameserver.h
	${CXX} -o gameserver.o -c gameserver.cpp ${CXXFLAGS}

gameclient.o: gameclient.cpp gameclient.h
	${CXX} -o gameclient.o -c gameclient.cpp ${CXXFLAGS}

main.o: main.cpp main.h
	${CXX} -o main.o -c main.cpp ${CXXFLAGS}

clean:
	rm -f ${EXENAME} ${OBJS}
