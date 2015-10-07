CXXFLAGS=-I. -IEigen
SHRFLAGS=-shared

PLATFORM=$(shell $(CXX) -v 2>&1|grep Target|cut -f3 -d'-')
ifeq "$(PLATFORM)" "mingw32"
	CXXFLAGS+=-DUNICODE
	DRES=res.o
	DEXT=dll
	ifeq "$(WRC)" ""
		WRC=$(subst g++,windres,$(CXX))
	endif
else
	SHRFLAGS+=-fPIC
	ifeq "$(PLATFORM)" "linux"
		DEXT=so
	else
		DEXT=dylib
	endif
endif

SYSTUNE=-march=native -mtune=native
SYSARCH=$(shell $(CXX) -v 2>&1|grep Target|cut -f1 -d'-'|cut -f2 -d' ')
ifeq "$(SYSARCH)" "x86_64"
	SYSTUNE+=-msse2
	DARCH=64
else
	DARCH=32
endif
ifeq "$(NOARCH)" "1"
	DARCH=
endif

OPTS=-DEIGEN_NO_DEBUG $(SYSTUNE) -O3 -s -fomit-frame-pointer

HDRS=bsets.h core.h dmatrix.h guess.h matrix.h \
	 sampling.h typedefs.h unmix.h utils.h vector.h resource.h
SRCS=FastICA.cpp FastICA.rc $(HDRS) $(DRES)

TARGET=FastICA$(DARCH).$(DEXT)

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) $(OPTS) $(SHRFLAGS) -o $(TARGET) $< $(DRES) $(LIBS)

$(DRES): FastICA.rc resource.h
	$(WRC) -o $(DRES) $<

clean:
	rm -f *.o

distclean: clean
	rm -f FastICA*.so
	rm -f FastICA*.dll
	rm -f FastICA*.dylib

new:
	make clean
	make
