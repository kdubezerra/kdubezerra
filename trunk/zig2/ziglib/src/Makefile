VERSION=2.0.0
PREFIX=/usr/local
OBJS_BASE=zig.o zigclient.o zigserver.o server.o client.o \
     msocket.o station.o address.o buffer.o bzip2.o \
     console.o log.o serverinfo.o utils.o \
     pfilesys.o
OBJS=${OBJS_BASE} network.o
HEADERS=address.h bzip2.h console.h leetnet.h \
	serverinfo.h zigclient.h zig.h \
	policy.h buffer.h client.h log.h msocket.h \
	server.h station.h utils.h zigdefs.h zigserver.h \
	pfilesys.h network-api.h

INC_DIR=../include
SOURCE=.

ifndef CXX
	CXX=g++
endif

DEBUGMODE=1
ifdef DEBUGMODE
	CXXFLAGS=-Wall -ansi -pedantic -O0 -g3 -fPIC -DPIC
	LIBNAME=libzigd
else
	ifndef CXXFLAGS
		CXXFLAGS=-O2 -g0 -fPIC -DPIC
	endif
	LIBNAME=libzig
endif

define install_header
install -m 644 ${addprefix ${INC_DIR}/zig/, ${header}} ${PREFIX}/include/zig/

endef

CLINE=${CXX} -I${INC_DIR} ${CXXFLAGS} -c

all: ${LIBNAME}.so.${VERSION}

base: ${LIBNAME}_base.so ${LIBNAME}_base.a

${LIBNAME}.so.${VERSION}: ${OBJS}
	${CXX} -o ${LIBNAME}.so.${VERSION} ${OBJS} -shared -Wl -lNL
	ar crs ${LIBNAME}.a ${OBJS}
	ranlib ${LIBNAME}.a

${LIBNAME}_base.so: ${OBJS_BASE}
	${CXX} -o ${LIBNAME}_base.so ${OBJS_BASE} -shared -Wl -lNL

${LIBNAME}_base.a: ${OBJS_BASE}
	ar crs ${LIBNAME}_base.a ${OBJS_BASE}
	ranlib ${LIBNAME}_base.a

zig.o: ${SOURCE}/zig.cpp ${INC_DIR}/zig/zig.h
	${CLINE} ${SOURCE}/zig.cpp

zigclient.o: zigclient.cpp ${INC_DIR}/zig/zigclient.h
	${CLINE} ${SOURCE}/zigclient.cpp

zigserver.o: zigserver.cpp ${INC_DIR}/zig/zigserver.h
	${CLINE} ${SOURCE}/zigserver.cpp

client.o: client.cpp ${INC_DIR}/zig/client.h
	${CLINE} ${SOURCE}/client.cpp

server.o: server.cpp ${INC_DIR}/zig/server.h
	${CLINE} ${SOURCE}/server.cpp

msocket.o: msocket.cpp ${INC_DIR}/zig/msocket.h
	${CLINE} ${SOURCE}/msocket.cpp

station.o: station.cpp ${INC_DIR}/zig/station.h
	${CLINE} ${SOURCE}/station.cpp

address.o: address.cpp ${INC_DIR}/zig/address.h
	${CLINE} ${SOURCE}/address.cpp

buffer.o: buffer.cpp ${INC_DIR}/zig/buffer.h
	${CLINE} ${SOURCE}/buffer.cpp

bzip2.o: bzip2.cpp ${INC_DIR}/zig/bzip2.h
	${CLINE} ${SOURCE}/bzip2.cpp

console.o: console.cpp ${INC_DIR}/zig/console.h
	${CLINE} ${SOURCE}/console.cpp

log.o: log.cpp ${INC_DIR}/zig/log.h
	${CLINE} ${SOURCE}/log.cpp

serverinfo.o: serverinfo.cpp ${INC_DIR}/zig/serverinfo.h
	${CLINE} ${SOURCE}/serverinfo.cpp

utils.o: utils.cpp ${INC_DIR}/zig/utils.h
	${CLINE} ${SOURCE}/utils.cpp

pfilesys.o: pfilesys.cpp ${INC_DIR}/zig/pfilesys.h
	${CLINE} ${SOURCE}/pfilesys.cpp

network.o: network.c ${INC_DIR}/zig/network-api.h
	${CLINE} ${SOURCE}/network.c

clean:
	rm -f ${LIBNAME}.so.${VERSION} ${LIBNAME}.a ${OBJS}

install: all
	install -d ${PREFIX}/include/zig
	${foreach header, ${HEADERS}, ${install_header}}
	install ${LIBNAME}.so.${VERSION} ${PREFIX}/lib
	install ${LIBNAME}.a ${PREFIX}/lib
	ln -sf ${LIBNAME}.so.${VERSION} ${PREFIX}/lib/${LIBNAME}.so
	ldconfig
