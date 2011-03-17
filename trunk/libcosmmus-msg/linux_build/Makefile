OUTPUT=libcommus-msg.a

RELEASE_DIR=./release
DEBUG_DIR=./debug

CORE_SRCDIR=../layer_core/src
CORE_RELEASE_DIR=$(RELEASE_DIR)/layer_core
CORE_DEBUG_DIR=$(DEBUG_DIR)/layer_core

NET_SRCDIR=../layer_network/src
NET_RELEASE_DIR=$(RELEASE_DIR)/layer_network
NET_DEBUG_DIR=$(DEBUG_DIR)/layer_network

CC=g++
INCLUDE_DIR=/opt/local/include
RELEASEFLAGS=-I$(INCLUDE_DIR) -O3
DEBUGFLAGS=-I$(INCLUDE_DIR) -O0 -g3 -Wall -fmessage-length=0

RESOURCESDIR=../resources

LIBS=-lSDL -lSDL_ttf -lSDL_gfx

CORE_OBJ = Client.o Group.o Object.o PaxosInstance.o ServerInterface.o ClientInterface.o NodeInfo.o ObjectFactory.o PaxosLearnerInterface.o Command.o OPMessage.o ObjectInfo.o Server.o
NET_OBJ = Address.o FIFOReliableClient.o GenericNode.o PeerInterface.o ServerInterface.o ClientInterface.o FIFOReliableServer.o Message.o RemoteFRC.o UnreliablePeer.o

RELEASEOBJ = $(patsubst %,$(CORE_RELEASE_DIR)/%,$(CORE_OBJ)) $(patsubst %,$(NET_RELEASE_DIR)/%,$(NET_OBJ))
DEBUGOBJ = $(patsubst %,$(CORE_DEBUG_DIR)/%,$(CORE_OBJ)) $(patsubst %,$(NET_DEBUG_DIR)/%,$(NET_OBJ))

all: release debug

release: $(RELEASE_DIR)/libcosmmus-msg.a

debug: $(DEBUG_DIR)/libcosmmus-msg.a


  

##########################
# RELEASE BUILD OF THE LIB
##########################

release_tree:
	mkdir -p $(RELEASE_DIR) $(CORE_RELEASE_DIR) $(NET_RELEASE_DIR)

$(CORE_RELEASE_DIR)/%.o: $(CORE_SRCDIR)/%.cpp | release_tree
	$(CC) -c $(RELEASEFLAGS) -o $@ $<

$(NET_RELEASE_DIR)/%.o: $(NET_SRCDIR)/%.cpp | release_tree
	$(CC) -c $(RELEASEFLAGS) -o $@ $<

$(RELEASE_DIR)/$(OUTPUT): $(RELEASEOBJ)
	ar -r $@ $^




##########################
#  DEBUG BUILD OF THE LIB
##########################

debug_tree:
	mkdir -p $(DEBUG_DIR) $(CORE_DEBUG_DIR) $(NET_DEBUG_DIR)

$(CORE_DEBUG_DIR)/%.o: $(CORE_SRCDIR)/%.cpp | debug_tree
	$(CC) -c $(DEBUGFLAGS) -o $@ $<

$(NET_DEBUG_DIR)/%.o: $(NET_SRCDIR)/%.cpp | debug_tree
	$(CC) -c $(DEBUGFLAGS) -o $@ $<

$(DEBUG_DIR)/$(OUTPUT): $(DEBUGOBJ)
	ar -r $@ $^

.PHONY: clean

clean:
	rm -rf $(RELEASE_DIR) $(DEBUG_DIR) ./*~

