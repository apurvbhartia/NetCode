# $Header: /u/lili/cvsroot/OPRCoding/NetCode/Makefile,v 1.7 2008/11/25 01:36:23 yzhang Exp $

PKG = netcoder

INSTALL_DIR = ../bin

CC = gcc
CCOPT = -O3 -Wall -DNDEBUG -fPIC
PROF_OPT = #-pg
INCLS = -I.

CPLUS = g++ $(PROF_OPT)

CFLAGS = $(CCOPT) $(INCLS)
LDFLAGS = 

LIBS = -lm

COMPRESS = gzip

EXEC = $(PKG)
OBJ =	GaloisField.o NetCoder.o \
	main.o util.o version.o

GENSRC = version.c

CLEANFILES = $(EXEC) $(OBJ) $(GENSRC) $(PKG).core $(PKG).log core

all: $(EXEC)

$(PKG): $(OBJ)
	$(CPLUS) -o $(PKG) $(OBJ) $(LDFLAGS) $(LIBS)

debug:
	@$(MAKE) $(MFLAGS) CCOPT="`echo $(CCOPT) | sed -e 's/-O./-O0/g;s/-DNDEBUG//g;s/$$/ -g/'`"

GaloisField.o: GaloisField.h GaloisField.cc 
	$(CPLUS) $(CFLAGS) -o GaloisField.o -c GaloisField.cc

InterCoder.o: InterCoder.h InterCoder.cc 
	$(CPLUS) $(CFLAGS) -o InterCoder.o -c InterCoder.cc

NetCoder.o: NetCoder.h NetCoder.cc 
	$(CPLUS) $(CFLAGS) -o NetCoder.o -c NetCoder.cc

main.o: main.cc 
	$(CPLUS) $(CFLAGS) -o main.o -c main.cc

util.o: util.h util.cc 
	$(CPLUS) $(CFLAGS) -o util.o -c util.cc

version.o: version.c
	$(CPLUS) $(CFLAGS) -o version.o -c version.c
version.c: VERSION
	@rm -f $@
	sed -e 's/.*/char version[] = "&";/' VERSION > $@

install: all
	cp $(EXEC) $(INSTALL_DIR)

tar: clean
	rm -f $(PKG).tar $(PKG).tar.gz /tmp/$(PKG).tar
	(cd .. ; tar cf /tmp/$(PKG).tar `ls -d NetCode/* | egrep -v 'CVS'`)
	mv /tmp/$(PKG).tar .
	gzip $(PKG).tar

full-tar: clean
	rm -f $(PKG).tar $(PKG).tar.gz /tmp/$(PKG).tar
	(cd .. ; tar cf /tmp/$(PKG).tar `ls -d NetCode/* `)
	mv /tmp/$(PKG).tar .
	gzip $(PKG).tar

clean:
	rm -f $(CLEANFILES)

