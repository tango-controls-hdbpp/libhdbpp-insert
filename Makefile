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
LDFLAGS += $(LIB_DIR) -lhdb++ -ltango -llog4tango -lomniORB4 -lomniDynamic4 -lomnithread

SWIG = swig3.0 -c++ -python

##############################################
# support for shared libray versioning
#
LFLAGS_SONAME = $(DBIMPL_LIB) $(LDFLAGS) -Wl,-soname,
SHLDFLAGS = -shared
BASELIBNAME =  HdbppInsert
SHLIB_SUFFIX = so

#  release numbers for libraries
#
 LIBVERSION    = 1
 LIBRELEASE    = 0
 LIBSUBRELEASE = 0
#

LIBRARY       = $(BASELIBNAME).a
DT_SONAME     = $(BASELIBNAME).$(SHLIB_SUFFIX).$(LIBVERSION)

.PHONY : install clean

lib/HdbppInsert: lib obj obj/HdbppInsert.o obj/HdbppInsert_wrap.o
	$(CXX) obj/HdbppInsert.o obj/HdbppInsert_wrap.o $(SHLDFLAGS) $(LFLAGS_SONAME)$(DT_SONAME) -o lib/_$(BASELIBNAME).so
	
obj/HdbppInsert_wrap.o: src/HdbppInsert_wrap.cxx src/HdbppInsert.h
	$(CXX) $(CXXFLAGS) -fPIC -c src/HdbppInsert_wrap.cxx -o $@
	
obj/HdbppInsert.o: src/HdbppInsert.cpp src/HdbppInsert.h
	$(CXX) $(CXXFLAGS) -fPIC -c src/HdbppInsert.cpp -o $@

src/HdbppInsert_wrap.cxx: src/HdbppInsert.i
	$(SWIG) src/HdbppInsert.i

.PHONY : install clean
	
clean:
	rm -rf obj/ lib/ *.o *.so* *.a src/*_wrap.cxx src/*.py*
	
install:	
	install -d ${DESTDIR}/libhdbppinsert
	touch ${DESTDIR}/libhdbppinsert/__init__.py
	install -m 755 src/HdbppInsert.py ${DESTDIR}/libhdbppinsert/HdbppInsert.py
	install -m 755 lib/_$(BASELIBNAME).so ${DESTDIR}/libhdbppinsert/_$(BASELIBNAME).so
	
all:
	@echo DESTDIR IS $(DESTDIR)
	

lib obj:
	@mkdir $@
