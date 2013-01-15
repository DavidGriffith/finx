#
# This is a 'utility' make, used to simplify building gkrellmd/nxsensor
# by simply running the targets from the root of the package. Files are
# still built in the server directory, as in the original gkrellmd dist-
# ribution.
#

MAKE_CMD=make
OS_NAME=$(shell uname -s)

ifeq ($(OS_NAME),SunOS)
MAKE_CMD=gmake
endif

all:
		cd server && $(MAKE_CMD)

static:
		cd server && $(MAKE_CMD) static

clean:
		cd server && $(MAKE_CMD) clean

distclean:
		cd server && $(MAKE_CMD) distclean


solaris:
		cd server && $(MAKE_CMD) solaris


