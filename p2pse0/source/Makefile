PREFIX=/usr/local
OBJS=config.o GroupAbstract.o RemoteAbstract.o Client.o GroupActionClient.o GroupLimboClient.o GroupSocialClient.o RemotePeer.o Server.o GroupActionServer.o GroupLimboServer.o GroupSocialServer.o RemoteClient.o
HEADERS=config.h ServerInterface.h ServerEventInterface.h \
ClientInterface.h ClientEventInterface.h
INC_DIR=../include
SOURCE=.

ifndef CXX
	CXX=g++
endif

ifndef CXXFLAGS
	BASE_CXXFLAGS=-Wall -ansi -pedantic -fPIC -DPIC -D__P2PSE0_COMPILATION__
endif

DEBUGMODE=1
ifdef DEBUGMODE
	CXXFLAGS=${BASE_CXXFLAGS} -O0 -g3
	LIBNAME=libp2pse0d
else
	CXXFLAGS=${BASE_CXXFLAGS} -O2 -g0
	LIBNAME=libp2pse0
endif

define install_header
install -m 644 ${addprefix ${INC_DIR}/, ${header}} ${PREFIX}/include/

endef

LDFLAGS=-shared -Wl -fPIC -DPIC
CLINE=${CXX} ${CXXFLAGS} -I${INC_DIR} -c

all: ${LIBNAME}.a ${LIBNAME}.so Makefile

${LIBNAME}.so: ${OBJS}
	${CXX} -o ${LIBNAME}.so ${OBJS} ${LDFLAGS}

${LIBNAME}.a: ${OBJS}
	ar crs ${LIBNAME}.a ${OBJS}

config.o: ${SOURCE}/config.cpp ${INC_DIR}/config.h
	${CLINE} config.cpp

GroupAbstract.o: ${SOURCE}/GroupAbstract.cpp ${INC_DIR}/GroupAbstract.h
	${CLINE} GroupAbstract.cpp

RemoteAbstract.o: ${SOURCE}/RemoteAbstract.cpp ${INC_DIR}/RemoteAbstract.h
	${CLINE} RemoteAbstract.cpp

Client.o: ${SOURCE}/Client.cpp ${INC_DIR}/Client.h
	${CLINE} Client.cpp

GroupActionClient.o: ${SOURCE}/GroupActionClient.cpp ${INC_DIR}/GroupActionClient.h
	${CLINE} GroupActionClient.cpp

GroupLimboClient.o: ${SOURCE}/GroupLimboClient.cpp ${INC_DIR}/GroupLimboClient.h
	${CLINE} GroupLimboClient.cpp

GroupSocialClient.o: ${SOURCE}/GroupSocialClient.cpp ${INC_DIR}/GroupSocialClient.h
	${CLINE} GroupSocialClient.cpp

RemotePeer.o: ${SOURCE}/RemotePeer.cpp ${INC_DIR}/RemotePeer.h
	${CLINE} RemotePeer.cpp

Server.o: ${SOURCE}/Server.cpp ${INC_DIR}/Server.h
	${CLINE} Server.cpp

GroupActionServer.o: ${SOURCE}/GroupActionServer.cpp ${INC_DIR}/GroupActionServer.h
	${CLINE} GroupActionServer.cpp

GroupLimboServer.o: ${SOURCE}/GroupLimboServer.cpp ${INC_DIR}/GroupLimboServer.h
	${CLINE} GroupLimboServer.cpp

GroupSocialServer.o: ${SOURCE}/GroupSocialServer.cpp ${INC_DIR}/GroupSocialServer.h
	${CLINE} GroupSocialServer.cpp

RemoteClient.o: ${SOURCE}/RemoteClient.cpp ${INC_DIR}/RemoteClient.h
	${CLINE} RemoteClient.cpp

clean:
	rm -f ${OBJS} *~ ${LIBNAME}.a ${LIBNAME}.so*

install: all
	install -d ${PREFIX}/include
	${foreach header, ${HEADERS}, ${install_header}}
	rm -f ${PREFIX}/lib/${LIBNAME}.so
	install ${LIBNAME}.so ${PREFIX}/lib
	rm -f ${PREFIX}/lib/${LIBNAME}.a
	install ${LIBNAME}.a ${PREFIX}/lib
	ldconfig
