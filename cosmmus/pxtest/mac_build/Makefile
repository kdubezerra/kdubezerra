OUT_DIR=.
OUTPUT=$(OUT_DIR)/pxtest

OBJ_DIR=./debug

SRC_DIR=../src

CC=g++
INCLUDEFLAGS=-I../../libcosmmus-msg/include -I/opt/local/include
CFLAGS=$(INCLUDEFLAGS) -O0 -g3 -Wall -fmessage-length=0

LDFLAGS=-g3 -L../../libcosmmus-msg/linux_build/debug -L/opt/local/lib -lcosmmus-msg -lSDL -lSDL_net -lSDLMain -framework Cocoa

OBJ_FILES=Main.o testobject.o testobjfactory.o testserver.o testclient.o

OBJS=$(patsubst %,$(OBJ_DIR)/%,$(OBJ_FILES))

all: $(OUTPUT)  

directories:
	mkdir -p $(OUT_DIR) $(OBJ_DIR)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | directories
	$(CC) -c $(CFLAGS) $< -o $@ 

$(OUTPUT): $(OBJS)
	$(CC) -g $^ -o $@ $(LDFLAGS)

.PHONY: clean

clean:
	rm -rf $(OUTPUT) $(OBJ_DIR) ./*~

up:
	make clean && make
