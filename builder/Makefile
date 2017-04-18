SRCDIR=../src
INCDIR=../include
LIBDIR=-L../lib
OUTDIR=../bin
WINOUTDIR=../bin
WINOBJDIR=wintmp
OBJDIR=lintmp

SRC=main.cpp mymath.cpp lzi.cpp lze.cpp lzo.cpp ent.cpp
OBJ=$(SRC:.cpp=.o)
GLOBALDEF=-DLZ_DEB -DVERT_PATH=\"vert\" -DFRAG_PATH=\"frag\" -DTEX01_PATH=\"tex1.lzi\" -DTEX02_PATH=\"tex1n.lzi\" -DRES_ENT_DIR=\"../res/ent/\" -DRES_SHDR_DIR=\"../res/shdr/\" -DRES_IMG_DIR=\"../res/img/\" -DRES_OBJ_DIR=\"../res/obj/\" 
HEADERS=$(INCDIR)/*.h
LIBS=-lGL -lglut -lGLEW -lGLU
WINLIBS=-lglew32s -lglu32 -lopengl32 -lfreeglut

ifeq ($(OS),Windows_NT)
  runnable=win
else
  runnable=lin
endif

.PHONY: all debug clean win lin

Debug: $(runnable)
	@echo DEBUG \($(runnable)\)

win: $(WINOUTDIR)/run.exe
lin: $(OUTDIR)/run

release:
	echo Not implemented.

$(WINOUTDIR)/run.exe: $(addprefix $(WINOBJDIR)/,$(OBJ))
	@echo Creating executable...
	$(CXX) -g -Wall -Wextra $^ -o $(WINOUTDIR)/run.exe $(WINLIBS) $(LIBDIR)
	@echo Done.

$(OUTDIR)/run: $(addprefix $(OBJDIR)/,$(OBJ))
	@echo Creating executable...
	$(CXX) -g -Wall -Wextra $^ -o $(OUTDIR)/run $(LIBS)
	@echo Done.

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp $(HEADERS)
	@echo Compiling $@...
	$(CXX) -g -c -Wall -Wextra $(DEF$*) $(GLOBALDEF) -I$(INCDIR) -o $@ $<
	@echo Done.

$(WINOBJDIR)/%.o: $(SRCDIR)/%.cpp $(HEADERS)
	@echo Compiling $@...
	$(CXX) -g -c -Wall -Wextra $(DEF$*) $(GLOBALDEF) -I$(INCDIR) -o $@ $<
	@echo Done.

clean%:
	rm -f $(WINOBJDIR)/*
	rm -f $(OBJDIR)/*
	rm -f $(WINOUTDIR)/run.exe
	rm -f $(WINOUTDIR)/run
