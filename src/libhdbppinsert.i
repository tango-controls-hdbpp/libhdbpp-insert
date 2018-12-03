/* File : libhdbppinsert.i */
%module libhdbppinsert

%{
#include "libhdbppinsert.h"
%}

%include "std_vector.i"
%include "std_string.i"

/* Let's just grab the original header file here */
%include "libhdbppinsert.h"
