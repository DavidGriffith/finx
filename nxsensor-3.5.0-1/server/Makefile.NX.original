PACKAGE_D ?= nxsensor
PKG_CONFIG ?= pkg-config
BINMODE ?= 755

INSTALLROOT ?= $(DESTDIR)$(PREFIX)
ifeq ($(INSTALLROOT),)
	INSTALLROOT = /usr/local
endif

SINSTALLDIR ?= $(INSTALLROOT)/bin
INSTALLDIRMODE ?= 755

INCLUDEDIR ?= $(INSTALLROOT)/include
INCLUDEMODE ?= 644
INCLUDEDIRMODE ?= 755

SMANDIR ?= $(INSTALLROOT)/share/man/man1
MANMODE ?= 644
MANDIRMODE ?= 755
INSTALL ?= install
LINK_FLAGS ?= -Wl,-E
#LINK_FLAGS = 
CC = gcc
STRIP ?= -s

OS_NAME=$(shell uname -s)
OS_RELEASE=$(shell uname -r)

GKRELLMD_INCLUDES = gkrellmd.h
PKG_INCLUDE = `$(PKG_CONFIG)  --cflags  glib-2.0 gthread-2.0`


ifeq ($(OS_RELEASE),5.8)
PKG_INCLUDE= -I../../glib-1.2.8/ -I../../glib-1.2.8/gmodule/ -I../../glib-1.2.8/gthread/
PKG_INCLUDE= `$(PKG_CONFIG) --cflags glib-2.0` -pthreads
endif
PKG_LIB = `$(PKG_CONFIG) --libs glib-2.0 gmodule-2.0 gthread-2.0`



#GLIB12_INCLUDE = `glib-config --cflags`
#GLIB12_LIB = `glib-config --libs glib gmodule`

GLIB12_INCLUDE = -I../../glib-1.2.8/ -I../../glib-1.2.8/gmodule/ -I../../glib-1.2.8/gthread/
#GLIB12_LIB = -L../../glib-1.2.8/.libs -L../../glib-1.2.8/gmodule/.libs -L../../glib-1.2.8/gthread/.libs -lglib -lgmodule -lgthread -ldl
GLIB12_LIB = ../../glib-1.2.8/.libs/libglib.a ../../glib-1.2.8/gmodule/.libs/libgmodule.a -ldl


FLAGS = -O3 -I.. $(PKG_INCLUDE) $(GTOP_INCLUDE)
ifeq ($(glib12),1)
FLAGS = -O3 -I.. $(GLIB12_INCLUDE) $(GTOP_INCLUDE)
endif
ifeq ($(glib12),yes)
FLAGS = -O3 -I.. $(GLIB12_INCLUDE) $(GTOP_INCLUDE)
endif

FLAGS+= $(PTHREAD_INC)

LIBS = $(PKG_LIB) $(GTOP_LIBS_D) $(SYS_LIBS) 
ifeq ($(glib12),1)
LIBS = $(GLIB12_LIB) $(GTOP_LIBS_D) $(SYS_LIBS)
endif
ifeq ($(glib12),yes)
LIBS = $(GLIB12_LIB) $(GTOP_LIBS_D) $(SYS_LIBS)
endif

ifeq ($(debug),1)
    FLAGS += -g
endif
ifeq ($(debug),yes)
    FLAGS += -g
endif

ifeq ($(profile),1)
    FLAGS += -g -pg
endif
ifeq ($(profile),yes)
    FLAGS += -g -pg
endif

ifeq ($(enable_nls),1)
    FLAGS += -DENABLE_NLS -DLOCALEDIR=\"$(LOCALEDIR)\"
endif
ifeq ($(enable_nls),yes)
    FLAGS += -DENABLE_NLS -DLOCALEDIR=\"$(LOCALEDIR)\"
endif

ifneq ($(PACKAGE_D),nxsensor)
	FLAGS += -DPACKAGE_D=\"$(PACKAGE_D)\"
endif

ifeq ($(HAVE_GETADDRINFO),1)
    FLAGS += -DHAVE_GETADDRINFO
endif

#
# Force debug information.
#

override CC += -g -Wall $(FLAGS)

OS_NAME=$(shell uname -s)
OS_RELEASE=$(shell uname -r)

OBJS =	main.o monitor.o mail.o plugins.o glib.o utils.o sysdeps-unix.o

all:	../nxsensor

../nxsensor: $(OBJS)
	$(CC) $(OBJS) -o ../nxsensor $(LIBS) $(LINK_FLAGS)

static: $(OBJS)
	$(CC) $(OBJS) -o ../nxsensor.static /lib/libdl.so.2 -static \
		$(LIBS) $(LINK_FLAGS)

freebsd2:
	$(MAKE) GTK_CONFIG=gtk12-config \
		EXTRAOBJS= SYS_LIBS="-lkvm -lmd" ../nxsensor

freebsd3 freebsd:
	$(MAKE) GTK_CONFIG=gtk12-config \
		EXTRAOBJS= SYS_LIBS="-lkvm -ldevstat -lmd" ../nxsensor

darwin: 
	$(MAKE) GTK_CONFIG=gtk-config STRIP= \
		EXTRAOBJS= SYS_LIBS="-lkvm -lmd5" \
		LINK_FLAGS="-flat_namespace -undefined warning" ../nxsensor

netbsd1:
	$(MAKE) EXTRAOBJS= SYS_LIBS="-lkvm" ../nxsensor

netbsd2:
	$(MAKE) EXTRAOBJS= SYS_LIBS="-lkvm -pthread" ../nxsensor

openbsd:
	$(MAKE) SYS_LIBS="-lkvm -pthread" ../nxsensor

solaris:
ifeq ($(OS_RELEASE),5.8)
	$(MAKE) CFLAGS="-Wno-implicit-int" \
		LD_LIBRARY_PATH="../../glib-1.2.8/.libs ../../glib-1.2.8/gmodule/.libs ../../glib-1.2.8/gthread/.libs" \
		SYS_LIBS="-lkstat -lkvm -ldevinfo -lsocket -lnsl -lintl" \
		-lglib -lgmodule -lgthread -ldl \
		LINK_FLAGS="" ../nxsensor
else
	$(MAKE) CFLAGS="-Wno-implicit-int" \
		SYS_LIBS="-lkstat -lkvm -ldevinfo -lsocket -lnsl" \
		LINK_FLAGS="" ../nxsensor
endif

install:
	$(INSTALL) -d -m $(INSTALLDIRMODE) $(SINSTALLDIR)
	$(INSTALL) -c $(STRIP) -m $(BINMODE) ../nxsensor $(SINSTALLDIR)/$(PACKAGE_D)
	$(INSTALL) -d -m $(INCLUDEDIRMODE) $(INCLUDEDIR)/gkrellm2
	$(INSTALL) -c -m $(INCLUDEMODE) $(GKRELLMD_INCLUDES) $(INCLUDEDIR)/gkrellm2
	$(INSTALL) -d -m $(MANDIRMODE) $(SMANDIR)
	$(INSTALL) -c -m $(MANMODE) ../nxsensor.1 $(SMANDIR)/$(PACKAGE_D).1

uninstall:
	rm -f $(SINSTALLDIR)/$(PACKAGE_D)
	rm -f $(SMANDIR)/$(PACKAGE_D).1

install_darwin:
	$(MAKE) install STRIP=

install_freebsd:
	$(MAKE) install
	chgrp kmem $(SINSTALLDIR)/$(PACKAGE_D)
	chmod g+s $(SINSTALLDIR)/$(PACKAGE_D)

install_netbsd:
	$(MAKE) SMANDIR="$(INSTALLROOT)/man/man1" install

install_openbsd:
	$(MAKE) install
	chgrp kmem $(SINSTALLDIR)/$(PACKAGE_D)
	chmod g+sx $(SINSTALLDIR)/$(PACKAGE_D)

install_solaris:
	$(MAKE) install INSTALL=/usr/ucb/install
	chgrp sys $(SINSTALLDIR)/$(PACKAGE_D)
	chmod g+s $(SINSTALLDIR)/$(PACKAGE_D)  

clean:
	rm -f *.o *~ *.bak ../*~ ../*.bak ../nxsensor \
        ../nxsensor.static core core.* ../core ../core.*

distclean: clean

SYSDEPS = ../src/sysdeps/bsd-common.c ../src/sysdeps/bsd-net-open.c \
	../src/sysdeps/freebsd.c ../src/sysdeps/gtop.c \
	../src/sysdeps/linux.c ../src/sysdeps/netbsd.c \
	../src/sysdeps/openbsd.c ../src/sysdeps/solaris.c ../src/sysdeps/darwin.c

main.o:		main.c gkrellmd.h
monitor.o:	monitor.c gkrellmd.h 
mail.o:		mail.c gkrellmd.h
plugins.o:	plugins.c gkrellmd.h 
glib.o:		glib.c gkrellmd.h
utils.o:	utils.c gkrellmd.h
sysdeps-unix.o: sysdeps-unix.c gkrellmd.h ../src/gkrellm-sysdeps.h $(SYSDEPS)
