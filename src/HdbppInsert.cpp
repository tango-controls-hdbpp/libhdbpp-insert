//+=============================================================================
//
// file :         HdbppInsert.cpp
//
// description :  C++ source for the HdbPeriodicArchiver device
//
// $Author: mbroseta@cells.es $
//
// $Revision: 1.0 $
//
// copyleft :     ALBA Synchrotron - CELLS
//                Ctra. BP. 1413 km 3,308290
//                Cerdanyola del Valles
//                Barcelona (Spain)
//-=============================================================================
#include "HdbppInsert.h"
#include <stdio.h>
#include <string.h>

#include <future>
#include <thread> 
#include <unistd.h>

#define _LIB_DEBUG

#define MSG_DEBUG 0
#define MSG_INFO  10
#define MSG_ERROR 20
#define MSG_NO    50

#define RESULT_OK 0
#define RESULT_NOT_OK -1

//=============================================================================
// Public Functions for the HdbppInsert Class
//=============================================================================

HdbppInsert::HdbppInsert(const char *dbuser, const char *dbpass,
								const char *dbhost, const char *dbnam,
								const char *libname, const char *lightschema,
								const char *port)
{
	vector< string > arg1 ;	
	string tmp;
	
	debug_level = MSG_ERROR;
	
	string msg = "\033[0;37mtrying to connect to host: \\" + string (dbhost);
	msg += " db name: \\" + string (dbnam);
	msg += " user: \\" + string(dbuser); 
	msg += "\033[0m\t\n";
	print_Msg(msg, MSG_INFO);

	tmp = "host=";
	tmp += dbhost;
	arg1.push_back(tmp);
	
	tmp = "user=";
	tmp += dbuser;
	arg1.push_back(tmp);
	
	tmp = "password=";
	tmp += dbpass;
	arg1.push_back(tmp);
	
	tmp = "dbname=";
	tmp += dbnam;
	arg1.push_back(tmp);
	
	tmp = "libname=";
	tmp += libname;
	arg1.push_back(tmp);
	
	tmp = "lightschema=";
	tmp += lightschema;
	arg1.push_back(tmp);
	
	tmp = "port=";
	tmp += port;
	arg1.push_back(tmp);
	
	try
	{
		mHdbppIns = new HdbClient(arg1);
		isConnected = true;
		msg = "Connected Succesfully to " + string (dbhost);
		msg += " dbname: " + string (dbnam) +"\n";
		print_Msg(msg, MSG_INFO);
	}
	catch (...)
	{
		print_Msg("\e[1;31mError connecting to requested HDB!!\e[0m\n", MSG_ERROR);
		isConnected = false;
	}
}

HdbppInsert::~HdbppInsert()
{
	print_Msg("DELETING mHdbppIns\n", MSG_INFO);
	delete mHdbppIns;
}

bool HdbppInsert::is_Connected()
{
	return isConnected;
}

void HdbppInsert::test_Func()
{
	print_Msg("\e[1;32mtest_func OK\e[0m\n", MSG_DEBUG);
}


int HdbppInsert::insert_Attr_Async(string attribute)
{
	bool opspending = false;
	map <string, bool> :: iterator it;
	
	try
	{
		it = pending_ops.find(attribute);
		if (it != pending_ops.end())
			opspending = it->second;
			
		if (opspending == false)
		{
			print_Msg(attribute+" insert_Attr_Async enter\n", MSG_DEBUG);    
			
			std::thread async_thread(&HdbppInsert::insert_Attr, this, attribute);
			pending_ops[attribute] = true;

			/* join() waits until the thread finishes to continue.
			* Better to use detach() to not wait for the thread to finish */
			/* async_thread.join();*/
			
			async_thread.detach();
			
			print_Msg(attribute+" insert_Attr_Async OK\n", MSG_DEBUG);  

			return RESULT_OK;
		}
		else
		{
			print_Msg("\e[1;31m"+attribute+" insert_Attr_Async: Pending operations!!\e[0m\n", MSG_ERROR);
			return RESULT_NOT_OK;		
		}
	}
	catch (...)
	{
		print_Msg("\e[1;31m"+attribute+" insert_Attr_Async: Error Inserting attribute!!\e[0m\n", MSG_ERROR);
		return RESULT_NOT_OK;
	}		
}

int HdbppInsert::insert_Attr(string attribute)
{
	int res = RESULT_OK;
	Tango::DevErrorList err;
	HdbEventDataType event_datatype;

	/* Block the function to avoid interferences */
	mtx.lock();
	
	string device_name = get_Device_Name(attribute);
	string attr_name = get_Attr_Name(attribute);
	
	string event_name = "ARCHIVE_EVENT";
	/*Tango::EventType event = Tango::ARCHIVE_EVENT;*/

	print_Msg(attribute+" insert_Attr enter\n", MSG_DEBUG);
	
	/* Get DS Proxy or created if not created before */
	map <string, Tango::DeviceProxy*> :: iterator it_dsproxy;
	it_dsproxy = dsproxies.find(device_name);
	if (it_dsproxy == dsproxies.end())
	{
		err.length(0);
		try
		{
			Tango::DeviceProxy *dev_proxy = new Tango::DeviceProxy(const_cast<std::string&>(device_name));
			dsproxies[device_name] = dev_proxy;
			print_Msg("\e[1;32m"+device_name+" Adding proxy!!\e[0m\n", MSG_INFO);
		}
		catch (Tango::DevFailed &e)
		{
			err = e.errors;
			print_Msg("\e[1;31m"+device_name+" Error creating proxy!!\e[0m\n", MSG_ERROR);
			res = RESULT_NOT_OK;			
		}
	}

	/* Get HdbEventData from previously loaded HdbEventData list */
	/* If it is NOT in the list, create a new HdbEventData and add it to the list */
	if ( res == RESULT_OK ) 
	{
		map <string, HdbEventDataType> :: iterator it_2;
		it_2 = hdbevttypes.find(attribute);
		if (it_2 != hdbevttypes.end())
		{
			event_datatype = it_2->second;
		}
		else
		{
			Tango::AttributeInfoEx *info = NULL;
			
			err.length(0);
			try
			{
				info = new Tango::AttributeInfoEx;
				*info = dsproxies[device_name]->get_attribute_config(attr_name.c_str());
				
				event_datatype.data_type = info->data_type;
				event_datatype.data_format = info->data_format;
				event_datatype.write_type = info->writable;
				event_datatype.max_dim_x = info->max_dim_x;
				event_datatype.max_dim_y = info->max_dim_y;

				print_Msg("\e[1;32m"+attribute+" HDB Event data not found creating it and adding to Map. Adding it!!\e[0m\n", MSG_INFO);
				hdbevttypes[attribute] = event_datatype;
				
				delete info;
				info = nullptr;
			}
			catch (Tango::DevFailed &e)
			{
				err = e.errors;
				print_Msg("\e[1;31m"+attribute+" Error creating HDB event data!!\e[0m\n", MSG_ERROR);
				res = RESULT_NOT_OK;
				
				delete info;
				info = nullptr;				
			}
		}
	}

	if ( res == RESULT_OK ) 
	{
		Tango::DeviceAttribute *da = NULL;
		
		err.length(0);
		try
		{
			da = new Tango::DeviceAttribute();
			*da = dsproxies[device_name]->read_attribute(attr_name.c_str());
			if (da->has_failed() == true)
			{
				err = da->get_err_stack();
				err.length(err.length() - 1);
				print_Msg("\e[1;31m"+attribute+" Error not possible to read attribute!!\e[0m\n", MSG_ERROR);
			
				delete da;
				da = nullptr;				
				res = RESULT_NOT_OK;
			}
		}
		catch (Tango::DevFailed &e)
		{
			err = e.errors;
			
			delete da;
			da = nullptr;
			
			print_Msg("\e[1;31m"+attribute+" Error DevFailed reading attribute!!\e[0m\n", MSG_ERROR);
			res = RESULT_NOT_OK;
		}

		if ( res == RESULT_OK ) 
		{
			Tango::EventData *event_data=NULL;
			
			try
			{
				event_data = new Tango::EventData(dsproxies[device_name], attribute, event_name, da, err);
				
				mHdbppIns->insert_Attr((Tango::EventData *) event_data, event_datatype);
				print_Msg("\e[1;32m"+attribute+" insert_Attr OK\e[0m\n", MSG_INFO);
				
				delete event_data;
				event_data = nullptr;
			}
			catch (...)
			{
				delete event_data;
				event_data = nullptr;
				print_Msg("\e[1;31m"+attribute+" Error inserting attribute!!\e[0m\n", MSG_ERROR);
				res = RESULT_NOT_OK;
			}
		}
	}
	
	pending_ops[attribute] = false;
	
	map <string, int> :: iterator it_attup;
	it_attup = att_update.find(attribute);
	if (it_attup != att_update.end())
	{
		it_attup->second = res;
	}
	else
	{
		att_update[attribute] = res;
	}

	/* Release the function block */
	mtx.unlock();

	return res;	
}

int HdbppInsert::get_Attr_Update_Status(string attribute)
{
	int aux = RESULT_OK;
	
	map <string, int> :: iterator it_attup;
	
	it_attup = att_update.find(attribute);
	if (it_attup != att_update.end())
	{
		aux = it_attup->second;
	}
	
	return aux;
}

//=============================================================================
// Private Functions for the HdbppInsert Class
//=============================================================================

string HdbppInsert::get_Device_Name(string attribute)
{
	string device_name;
	string::size_type pos;
	
	if ((pos = attribute.find_last_of("\\/")) != string::npos)
	{
		device_name = attribute.substr(0,pos);
		return device_name;
	}
	else 
	{
		print_Msg("\e[1;31mAttribute Getting Device name from "+attribute+"!!\e[0m\n", MSG_DEBUG);
		return attribute;
	}	
}

string HdbppInsert::get_Attr_Name(string attribute)
{
	string attr_name;
	string::size_type pos;
	
	if ((pos = attribute.find_last_of("\\/")) != string::npos)
	{
		attr_name = attribute.substr(pos+1);
		return attr_name;
	}
	else 
	{
		print_Msg("\e[1;31mAttribute Getting Attribute name from "+attribute+"!!\e[0m\n", MSG_DEBUG);
		return attribute;
	}	
}

string HdbppInsert::remove_Tango(string str)
{
	string tmp;
	string::size_type pos = str.find("tango://");
	
	if (pos == string::npos)
	{
		pos = 0;
	}
	else
	{
		pos = 8;	
	}

	tmp = str.substr(pos);
	return tmp;
}

void HdbppInsert::print_Msg(string msg, int level)
{
#ifdef _LIB_DEBUG
	if (level >= debug_level )
	{
		printf(msg.c_str());
	}
#endif
	return;
}

