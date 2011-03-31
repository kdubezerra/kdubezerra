OUT_DIR=.
OUTPUT=$(OUT_DIR)/pxtest

OBJ_DIR=./debug

SRC_DIR=../src

CC=g++
INCLUDE_DIR=../../libcosmmus-msg/include
CFLAGS=-I$(INCLUDE_DIR) -O0 -g3 -Wall -fmessage-length=0

LDFLAGS=-g -L../../libcosmmus-msg/linux_build/debug -lcosmmus-msg -lSDL -lSDL_net

OBJ_FILES=Main.o testobject.o testobjfactory.o testserver.o

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
	make clean && clear && make
