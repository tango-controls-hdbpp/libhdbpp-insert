# liphdbppinsert

Library for inserting attributes in the HDB++ database

## Version

The current release version is 1.0.0

## Documentation

This library includes functions to insert attrbiutes in the corresponding HDB++ library selected.

It has a class HdbppInsert that contains the follwoing functions:

- HdbppInsert(const char *dbuser, const char *dbpass,
					const char *dbhost, const char *dbnam,
					const char *libname, const char *lightschema,
					const char *port);

This function initializes the class whit an instance to the HDB++ database selected
					
- virtual ~HdbppInsert();

This function initializes the class whit an instance to the HDB++ database selected

- bool is_Connected();

This function informs if the class has been successfully connected to an HDB++ instance

- void test_Func();

This is just a test function. Just prints a test message

- int insert_Attr_Async(string attribute);

This function is used to insert an attribute in HDB++ asychronous. The attribute name should be with the long url format.
When this function is called it creates an independent c++ thread responsible to insert the attribute in HDB++. It fails, when there is 
still a pending thread running to insert a value of the same attrbute in the HDB++

- int insert_Attr(string attribute);

This function directly inserts an attribute in HDB++. The attribute name should be with the long url format. It fails, if the proxy to the device
cannot be created, or if the configuration of the attribute cannot be read, or the read attribute fails or also if the insert to the HDB++ fails.

- int get_Attr_Update_Status(string attribute);

Returns information about the status of an attribute. If there were problems while inserting it in the HDB++

## Building

### Dependencies

Ensure the development version of the dependencies are installed. These are as follows:

* Python2.7 lib and header files
* HDB++ library libhdbpp
* Tango Controls 9 or higher - either via debian package or source install.
* omniORB release 4 - libomniorb4 and libomnithread.
* libzmq - libzmq3-dev or libzmq5-dev.

### Build Flags

There are a set of library and include variables that can be set to inform the build of various dependencies. The flags are only required if you have installed a dependency in a non-standard location. 

| Flag | Notes |
|------|-------|
| TANGO_INC | Tango include files directory |
| TANGO_LIB | Tango lib files directory |
| OMNIORB_INC | Omniorb include files directory |
| OMNIORB_LIB | Omniorb lib files directory |
| ZMQ_INC | ZMQ include files directory |
| LIBHDBPP_INC | Libhdb++ include files directory |
| LIBHDBPP_LIB | Libhdb++ lib files directory |
| PYTHON_INC | Python2.7 include files |


### Build

To get the source, pull from git:

```bash
git clone https://git.cells.es/controls/libhdbppinsert.git
cd libhdbppinsert
```

Set appropriate flags in the environment (or pass them to make) if required, then:

```bash
make
```

## Installation

Once built simply run `make install`. The install can be passed a PREFIX variable, this is set to /usr/local by default. It is also possible to use DESTDIR. Install path is constructed as DESTDIR/PREFIX.

## License

The code is released under the LGPL3 license and a copy of this license is provided with the code. 
