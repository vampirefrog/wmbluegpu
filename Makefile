# Makefile

# Installation directory
PREFIX=/usr/local

# Use Xft for the menu
USE_XFT=yes

VERSION=0.2

BINDIR=$(PREFIX)/bin
MANPREFIX=$(PREFIX)/share/man
MANDIR=$(MANPREFIX)/man1
PROG=wmbluegpu
MANUAL=$(PROG).1
OBJS=main.o menu.o gpu_ati.o gpu_nvidia.o
CFLAGS=-O2 -Wall -DVERSION=\"$(VERSION)\" -IlibXNVCtrl
LIBS=-L/usr/X11R6/lib -lX11 -lXext -lXpm -L./libXNVCtrl -lXNVCtrl

ifeq ($(USE_XFT),yes)
CFLAGS += -DUSE_XFT $(shell pkg-config xft --cflags)
LIBS += $(shell pkg-config xft --libs)
endif

CC=gcc
RM=rm -rf
INST=install

all: $(PROG)

$(PROG): $(OBJS)
	$(CC) -o $(PROG) $(OBJS) $(LIBS)
	strip $(PROG)
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@
clean:
	$(RM) *.o $(PROG) *~ *.bak *.BAK .xvpics
install: $(PROG) $(MANUAL)
	$(INST) -m 755 $(PROG) $(BINDIR)
	$(INST) -m 644 $(MANUAL) $(MANDIR)
uninstall:
	$(RM) $(BINDIR)/$(PROG)
	$(RM) $(MANDIR)/$(MANUAL)
