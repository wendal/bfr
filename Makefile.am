SUBDIRS      = testit
CLEANFILES   = bfrdebug bfpdebug bfr.1 debian/{files,substvars}
EXTRA_DIST   = bfr.pod bfr.spec debian/*
bin_PROGRAMS = bfr bfp
bfr_SOURCES  = bfr.c getopt.c getopt1.c bfr.h getopt.h
bfp_SOURCES  = bfr.c getopt.c getopt1.c bfr.h getopt.h
man_MANS     = bfr.1
CFLAGS      := -O2 -Wall $(CFLAGS)

clean:
	rm -rf debian/tmp

bfr.1: bfr.pod
	pod2man --center="bfr documentation" --release="bfr documentation" bfr.pod > bfr.1

test: bfrdebug
	(cd testit && make testit)

bfrdebug: bfrdebug.o getopt.o getopt1.o $(bfr_SOURCES)
	$(CCLD) $(LDFLAGS) -o bfrdebug $(bfr_LDFLAGS) bfrdebug.o getopt.o getopt1.o

bfrdebug.o: bfr.h bfr.c
	$(CC) $(DEFS) $(INCLUDES) $(AM_CFLAGS) $(CFLAGS) -DDEBUG -c bfr.c -o bfrdebug.o -g -ggdb

bfp: bfp.o getopt.o getopt1.o $(bfr_SOURCES)
	$(CCLD) $(LDFLAGS) -o bfp $(bfp_LDFLAGS) bfp.o getopt.o getopt1.o

bfp.o: bfr.h bfr.c
	$(CC) $(DEFS) $(INCLUDES) $(AM_CFLAGS) $(CFLAGS) -DBUFPLAY -c bfr.c -o bfp.o -g -ggdb

bfpdebug: bfpdebug.o getopt.o getopt1.o $(bfr_SOURCES)
	$(CCLD) $(LDFLAGS) -o bfrdebug $(bfr_LDFLAGS) bfpdebug.o getopt.o getopt1.o

bfpdebug.o: bfr.h bfr.c
	$(CC) $(DEFS) $(INCLUDES) $(AM_CFLAGS) $(CFLAGS) -DBUFPLAY -DDEBUG -c bfr.c -o bfpdebug.o -g -ggdb
