PREFIX=/usr/local

#DESTDIR := $(shell python -m site --user-site)

DBIMPL_INC = `mysql_config --include`
DBIMPL_LIB = `mysql_config --libs_r`

LIB_DIR += -L/usr/lib/x86_64-linux-gnu

ifdef LIBHDBPP_LIB
	LIB_DIR += -L${LIBHDBPP_LIB}
endif	

ifdef LIBHDBPP_INC
	INC_DIR += -I${LIBHDBPP_INC}
else
	INC_DIR += -I/usr/include/libhdb++
endif

ifdef TANGO_INC
	INC_DIR += -I${TANGO_INC}
else
	INC_DIR += -I/usr/include/tango
endif

ifdef TANGO_LIB
	LIB_DIR	+= -L${TANGO_LIB}
endif

ifdef OMNIORB_INC
	INC_DIR	+= -I${OMNIORB_INC}
endif

ifdef OMNIORB_LIB
	LIB_DIR	+= -L${OMNIORB_LIB}
endif

ifdef ZMQ_INC
	INC_DIR += -I${ZMQ_INC}
endif

ifdef PYTHON_INC
	INC_DIR += -I${PYTHON_INC}
else
	INC_DIR += -I/usr/include/python2.7/
endif

CXXFLAGS += -std=gnu++0x -Wall -DRELEASE='"$HeadURL$ "' $(DBIMPL_INC) $(INC_DIR)
CPPFLAGS = -I eigen
LDFLAGS += $(LIB_DIR) -lhdb++ -ltango -llog4tango -lomniORB4 -lomniDynamic4 -lomnithread

SWIG = swig3.0 -c++ -python

##############################################
# support for shared libray versioning
#
LFLAGS_SONAME = $(DBIMPL_LIB) $(LDFLAGS) -Wl,-soname,
SHLDFLAGS = -shared
BASELIBNAME =  libhdbppinsert
SHLIB_SUFFIX = so

#  release numbers for libraries
#
 LIBVERSION    = 1
 LIBRELEASE    = 2
 LIBSUBRELEASE = 0
#

LIBRARY       = $(BASELIBNAME).a
DT_SONAME     = $(BASELIBNAME).$(SHLIB_SUFFIX).$(LIBVERSION)

.PHONY : install clean

lib/libhdbppinsert: lib obj obj/libhdbppinsert.o obj/libhdbppinsert_wrap.o
	$(CXX) obj/libhdbppinsert.o obj/libhdbppinsert_wrap.o $(SHLDFLAGS) $(LFLAGS_SONAME)$(DT_SONAME) -o lib/_$(BASELIBNAME).so
	
obj/libhdbppinsert_wrap.o: src/libhdbppinsert_wrap.cxx src/libhdbppinsert.h
	$(CXX) $(CXXFLAGS) -fPIC -c src/libhdbppinsert_wrap.cxx -o $@
	
obj/libhdbppinsert.o: src/libhdbppinsert.cpp src/libhdbppinsert.h
	$(CXX) $(CXXFLAGS) -fPIC -c src/libhdbppinsert.cpp -o $@

src/libhdbppinsert_wrap.cxx: src/libhdbppinsert.i
	$(SWIG) src/libhdbppinsert.i

.PHONY : install clean
	
clean:
	rm -rf obj/ lib/ *.o *.so* *.a src/*_wrap.cxx src/*.py*
	
install:	
	install -d ${DESTDIR}/libhdbppinsert
	echo "from ${BASELIBNAME} import *" > ${DESTDIR}/libhdbppinsert/__init__.py
	install -m 755 src/libhdbppinsert.py ${DESTDIR}/libhdbppinsert/libhdbppinsert.py
	install -m 755 lib/_$(BASELIBNAME).so ${DESTDIR}/libhdbppinsert/_$(BASELIBNAME).so
	
all:
	@echo DESTDIR IS $(DESTDIR)
	

lib obj:
	@mkdir $@
