CC=g++
CFLAGS=-I$(INCDIR) -m32 -Wall -static-libgcc -static-libstdc++
STATIC_FLAGS=#-DGLEW_STATIC #-FREEGLUT_STATIC

INCDIR=./include
BINDIR=./bin
SRCDIR=./src
OBJDIR=obj
LIBDIR=./lib

LIBS=-lm -lopengl32 -lOpenCL -lglfw3dll

_DEPS=ProgramSettings.h Window.h aCL.h aGL.h gl_core_3_3.h
DEPS=$(patsubst %,$(INCDIR)/%,$(_DEPS))

_OBJ=Window.o aCL.o aGL.o gl_core_3_3.o
OBJ=$(patsubst %,$(OBJDIR)/%,$(_OBJ))

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp $(DEPS)
	$(CC) $(STATIC_FLAGS) -c -o $@ $< $(CFLAGS)

AttractorCL: $(OBJ)
	$(CC) -o $(BINDIR)/$@ $^ $(CFLAGS) -L$(LIBDIR) $(LIBS)

.PHONY: clean

clean:
	rm -f $(OBJDIR)/*.o *~ core $(INCDIR)/*~
