SRCDIR =./source
CC=g++
RELEASEFLAGS=-O3
DEBUGFLAGS=-g
ODIR=./linux-build/obj

RELEASEDIR=./linux-build/release
DEBUGDIR=./linux-build/debug


LIBS=-lSDL -lSDL_ttf -lSDL_gfx

_OBJ = myutils.o Avatar.o Cell.o Region.o KDTree.o Server.o Simulation.o simcosmmus.o
RELEASEOBJ = $(patsubst %,$(RELEASEDIR)/%,$(_OBJ))
DEBUGOBJ = $(patsubst %,$(DEBUGDIR)/%,$(_OBJ))

all: $(RELEASEDIR)/simcosmmus $(DEBUGDIR)/simcosmmus

$(RELEASEDIR)/%.o: $(SRCDIR)/%.cpp
	$(CC) -c -o $@ $< $(RELEASEFLAGS)

$(RELEASEDIR)/simcosmmus: $(RELEASEOBJ)
	g++ -o $@ $^ $(RELEASEFLAGS) $(LIBS) && cp resources/*.bmp $(RELEASEDIR)/ && cp resources/*.ttf $(RELEASEDIR)/



$(DEBUGDIR)/%.o: $(SRCDIR)/%.cpp
	$(CC) -c -o $@ $< $(DEBUGFLAGS)

$(DEBUGDIR)/simcosmmus: $(DEBUGOBJ)
	g++ -o $@ $^ $(DEBUGFLAGS) $(LIBS) && cp resources/*.bmp $(DEBUGDIR)/ && cp resources/*.ttf $(DEBUGDIR)/

.PHONY: clean

clean:
	rm -f $(RELEASEDIR)/*.* $(DEBUGDIR)/*.* *~
