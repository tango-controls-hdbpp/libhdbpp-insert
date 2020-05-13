#ifndef LIBHDBPPINSERT_H
#define LIBHDBPPINSERT_H

#include <libhdb++/LibHdb++.h>
#include <tango.h>
#include <thread>
#include <vector>
#include <mutex>

using namespace std;

class HdbppInsert
{
public:
	HdbppInsert(const char *dbuser, const char *dbpass,
					const char *dbhost, const char *dbnam,
					const char *libname, const char *lightschema,
					const char *port, int dbg_level);

	virtual ~HdbppInsert();
	
	bool is_Connected();
	void test_Func();
	int insert_Attr_Async(string attribute);
	int insert_Attr(string attribute);
	int get_Attr_Update_Status(string attribute);
	void reset_Attr_Pending_Ops(string attribute);
	int get_Pending_Threads();
	void set_Debug_Level(int level);

private:
	std::map<string, Tango::DeviceProxy*> dsproxies;
	std::map<string, HdbEventDataType> hdbevttypes;
	std::map<string, bool> pending_ops;
	std::map<string, std::thread::id> att_tid;
	std::vector<std::thread::id> tids_list;	
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


#endif // LIBHDBPPINSERT_H
