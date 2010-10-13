SRCDIR =../source
CC=g++
RELEASEFLAGS=-O3
DEBUGFLAGS=-g
CFLAGS=-I/opt/local/include/
LDFLAGS=-L/opt/local/lib
CXXFLAGS=$(CFLAGS)
RELEASEDIR=./release
DEBUGDIR=./debug
RESOURCESDIR=../resources

LIBS=-lSDL -lSDLmain -lSDL_ttf -lSDL_gfx -framework Cocoa

_OBJ = myutils.o Avatar.o Cell.o Region.o KDTree.o Server.o Simulation.o simcosmmus.o
RELEASEOBJ = $(patsubst %,$(RELEASEDIR)/%,$(_OBJ))
DEBUGOBJ = $(patsubst %,$(DEBUGDIR)/%,$(_OBJ))

all: release debug

release: $(RELEASEDIR)/simcosmmus

debug: $(DEBUGDIR)/simcosmmus

$(RELEASEDIR)/%.o: $(SRCDIR)/%.cpp
	$(CC) -c $(CFLAGS) -o $@ $< $(RELEASEFLAGS)

$(RELEASEDIR)/simcosmmus: $(RELEASEOBJ)
	g++ $(LDFLAGS) -o $@ $^ $(RELEASEFLAGS) $(LIBS) && cp $(RESOURCESDIR)/*.bmp $(RELEASEDIR)/ && cp $(RESOURCESDIR)/*.ttf $(RELEASEDIR)/



$(DEBUGDIR)/%.o: $(SRCDIR)/%.cpp
	$(CC) -c $(CFLAGS) -o $@ $< $(DEBUGFLAGS)

$(DEBUGDIR)/simcosmmus: $(DEBUGOBJ)
	g++ $(LDFLAGS) -o $@ $^ $(DEBUGFLAGS) $(LIBS) && cp $(RESOURCESDIR)/*.bmp $(DEBUGDIR)/ && cp $(RESOURCESDIR)/*.ttf $(DEBUGDIR)/

.PHONY: clean

clean:
	rm -f $(RELEASEDIR)/* $(DEBUGDIR)/* *~
