export INSTPREFIX=d:/usr/mingw
export INSTALL=install
export CP=cp
export RM=rm
export GCC_STYLE=mingw
export OS_STYLE=win
export NODEP=no


all : libMU test

libMU :
	cd src && \
	$(MAKE) -f Makefile  &&\
	cd ..

test : libMU
	cd test && \
	$(CP) ../src/libMU.a libMU.a &&\
	$(MAKE) -f Makefile  && \
	cd ..
	

install :
	cd src && \
	$(MAKE) -f Makefile  install &&\
	cd ..  &&\
	cd test && \
	$(MAKE) -f Makefile  install && \
	cd ..

clean :
	cd src && \
	$(MAKE) -f Makefile  clean &&\
	cd ..  &&\
	cd test && \
	$(MAKE) -f Makefile  clean && \
	cd ..
	