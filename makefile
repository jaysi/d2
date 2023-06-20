LINKLIBS = -lpthread -lm -l13
CFLAGS = -Wall -Winline -pedantic
release: CFLAGS += -O2 -DNDEBUG -L$(HOME)/prj/lib13/release
debug: CFLAGS += -g -L$(HOME)/prj/lib13/debug
INCLUDE_PATH = -I$(HOME)/prj/lib13/include -I$(HOME)/prj/d2/include
LIB_PATH = 
CFLAGS += $(INCLUDE_PATH)
SRC = $(wildcard *.c)
OBJ = $(SRC:%.c=%.o)
MV = @mv
STRIP = @strip
RM = @rm -rf
TAR = tar -cf
ZIP = bzip2
MKDIR = mkdir

all: debug

d2: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LINKLIBS)
	$(MV) *.o obj/

release: d2
	$(STRIP) d2
	$(MV) d2 release/
	$(info *** put executable in release/ ; 'make clean' before 'make debug' ***)

debug: d2
	$(MV) d2 debug/
	$(info *** put executable in debug/ ; 'make clean' before 'make release' ***)

clean:
	$(RM) *.o obj/* *.*~ *~

cleanall: clean
	$(RM) release/* debug/*

backup:
	$(TAR) d2.tar *.c makefile include/
	$(ZIP) d2.tar

init:
	$(MKDIR) release debug obj



