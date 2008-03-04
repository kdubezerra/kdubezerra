OBJS=main.o fake_console.o ClientListenerTest.o ServerListenerTest.o
EXENAME=main

INCLUDE_NL=/usr/local/include
INCLUDE_ZIG=/usr/local/include
INCLUDE_L0=../p2pse0/include

LIBDIR_NL=/usr/local/lib
LIBDIR_ZIG=/usr/local/lib
LIBDIR_L0=../p2pse0/source

ifndef CXX
	CXX=g++
endif

DEBUGMODE=1
ifdef DEBUGMODE
	CXXFLAGS=-Wall -ansi -pedantic -O0 -g3 -fPIC -DPIC
	LIBNAME_ZIG=zigd
	LIBNAME_L0=p2pse0d
else
	ifndef CXXFLAGS
		CXXFLAGS=-O2 -g0 -fPIC -DPIC
	endif
	LIBNAME_ZIG=zig
	LIBNAME_L0=p2pse0
endif

CLINE=${CXX} ${CXXFLAGS} -I${INCLUDE_NL} -I${INCLUDE_ZIG} -I${INCLUDE_L0} -c

all: ${EXENAME}

${EXENAME}: ${OBJS}
	${CXX} -o ${EXENAME} ${OBJS} -L${LIBDIR_NL} -lNL -L${LIBDIR_ZIG} -l${LIBNAME_ZIG} -L${LIBDIR_L0} -l${LIBNAME_L0}

main.o: main.cpp
	${CLINE} main.cpp

fake_console.o: fake_console.cpp fake_console.h
	${CLINE} fake_console.cpp

ClientListenerTest.o: ClientListenerTest.cpp ClientListenerTest.h
	${CLINE} ClientListenerTest.cpp

ServerListenerTest.o: ServerListenerTest.cpp ServerListenerTest.h
	${CLINE} ServerListenerTest.cpp

clean:
	rm -f ${EXENAME} ${OBJS}
