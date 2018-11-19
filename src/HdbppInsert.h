#ifndef HDBPPINSERT_H
#define HDBPPINSERT_H

#include <libhdb++/LibHdb++.h>
#include <tango.h>
#include <mutex>

using namespace std;

class HdbppInsert
{
public:
	HdbppInsert(const char *dbuser, const char *dbpass,
					const char *dbhost, const char *dbnam,
					const char *libname, const char *lightschema,
					const char *port);

	virtual ~HdbppInsert();
	
	bool is_Connected();
	
	void test_Func();

	int insert_Attr_Async(string attribute);
	
	int insert_Attr(string attribute);
	
	int get_Attr_Update_Status(string attribute);

private:
	std::map<string, Tango::DeviceProxy*> dsproxies;
	std::map<string, HdbEventDataType> hdbevttypes;
	std::map<string, bool> pending_ops;
	std::map<string, int> att_update;
	
	HdbClient *mHdbppIns = 0;
	
	int debug_level;
	bool isConnected = false;
	std::mutex mtx;
	
	string get_Device_Name(string attribute);
	string get_Attr_Name(string attribute);
	string remove_Tango(string str);
	
	void print_Msg(string msg, int level);	
};


#endif // HDBPPINSERT_H
