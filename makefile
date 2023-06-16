LIBS = -lpthread -lm
CFLAGS = -Wall
release: CFLAGS += -O2
debug: CFLAGS += -g
INCLUDE_PATH = /home/jc/0-zdfs.cl/0-comp.cl/0-dev.cl/0-prj.cl/lib13/include;./include
SRC = $(wildcard *.c)
MV = mv
STRIP = strip
RM = rm -rf



d2: $(SRC)
	$(CC) -o $@ $^ $(CFLAGS) -I$(INCLUDE_PATH) $(LIBS)
	$(MV) *.o obj/	

release: d2	
	$(STRIP) d2
	$(MV) d2 release/	

debug: d2
	$(MV) d2 debug/

clean:
	$(RM) obj/*.o release/* debug/* d2

