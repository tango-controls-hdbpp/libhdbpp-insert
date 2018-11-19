PREFIX=/usr/local

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
else
	INC_DIR += -I/usr/include/omniORB4
endif

ifdef OMNIORB_LIB
	LIB_DIR	+= -L${OMNIORB_LIB}
else
	LIB_DIR	+= -L/usr/lib
endif

ifdef ZMQ_INC
	INC_DIR += -I${ZMQ_INC}
else
	INC_DIR	+= -I/usr/include
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
 LIBVERSION    = 6
#

LIBRARY       = $(BASELIBNAME).a
DT_SONAME     = $(BASELIBNAME).$(SHLIB_SUFFIX).$(LIBVERSION)

.PHONY : install clean

HdbppInsert: HdbppInsert.o
	$(CXX) HdbppInsert.o HdbppInsert_wrap.o $(SHLDFLAGS) $(LFLAGS_SONAME)$(DT_SONAME) -o _$(BASELIBNAME).so

HdbppInsert.o: HdbppInsert.cpp HdbppInsert.h HdbppInsert_wrap.cxx
	$(CXX) $(CXXFLAGS) -fPIC -c HdbppInsert.cpp HdbppInsert_wrap.cxx
	
HdbppInsert_wrap.cxx: HdbppInsert.i
	$(SWIG) HdbppInsert.i
	
clean:
	rm -f *.o *.so* *.a *_wrap.cxx

lib obj:
	@mkdir $@
