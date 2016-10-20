CC = gcc

SDLCFLAGS = `bash sdl-config  --cflags`
SDLLDFLAGS = `bash sdl-config --libs` 

CFLAGS = -Wall -O0 -g -std=c99 -Iinclude/ $(SDLCFLAGS)
LDFLAGS = $(SDLLDFLAGS) -lSDL_image -lSDL_mixer

BINDIR = bin
BIN = bombhellman

SRCDIR = src
SRC  = $(wildcard $(SRCDIR)/*.c)

OBJDIR = .obj
OBJ  = $(SRC:$(SRCDIR)/%.c=$(OBJDIR)/%.o)

DEPDIR = .dep
DEP = $(SRC:$(SRCDIR)/%.c=$(DEPDIR)/%.d)

.PHONY: all clean mrproper archive

all : $(BIN)

$(BIN) : $(OBJ)
	$(CC) -o $(BINDIR)/$@ $(OBJ) $(LDFLAGS)

$(OBJDIR)/%.o : $(SRCDIR)/%.c $(DEPDIR)/%.d
	$(CC) -c $(CFLAGS) $< -o $@

$(DEPDIR)/%.d : $(SRCDIR)/%.c
	@$(CC) -Iinclude/ -MT"$(<:$(SRCDIR)/%.c=$(OBJDIR)/%.o)" -MM -o $@ $< 

$(OBJ) : $(OBJDIR)

$(DEP) : $(DEPDIR)

$(BIN) : $(BINDIR)

$(DEPDIR) $(OBJDIR) $(BINDIR):
	-mkdir $@

clean :
	-rm -rf $(BINDIR) $(OBJDIR) $(DEPDIR)
	
mrproper : clean
	-rm -rf .project .cproject .settings '*~' #*

archive:
	-zip -9 -r bombeirb-`date +'%y-%m-%d-%Hh%M'`.zip . -x "*/\.*" -x "\.*" -x "bin/*" -x "*.zip" -x "*~"

-include $(DEP)
