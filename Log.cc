//
// Class Log
//
// Implementation of log class
//

#include <ctime>//include time library
#include <time.h>//include time library
#include <string>//string library

#include "Log.h"//Include header file

using namespace std;//use standard namespace

//Make default constant file name
const string Log::DEFAULT_LOG_FILE_NAME = "log.txt";

//getter for time
string Log::getTimeStamp()
{
	time_t t;//create a time_t variable to get time

	time(&t);//get current time

	//return the formatted time
	return string(ctime(&t));
}

//Default constructor using default file name
Log::Log() 
{
	this->logfilename = this->getDefaultLogfileName(); 
}

//Overloaded char constructor
Log::Log(char* lname)
{
	this->logfilename = string(lname); 
}

//Overloaded string constructor
Log::Log(string lname)
{
	this->logfilename = lname; 
}

//Setter for log file name
void Log::setLogfileName(string cname)
{
	this->logfilename = cname;
}

//Getter for log file name
string Log::getLogfileName()
{
	return logfilename;
}

string Log::getDefaultLogfileName()
{
	return DEFAULT_LOG_FILE_NAME;
}

//Opens log file
int Log::open()
{
	//open the log file
	logF.open(logfilename, ios::app);

	//if it failed to open return error
	if(logF.fail())
	{
		return OPEN_FAILURE;
	}

	//begin the log file
	this->writeLogRecord("BEGIN");

	//return good
	return SUCCESS;
}

//Closes log file
int Log::close()
{
	//write time stamp then end log file
	this->writeLogRecord("END");

	//close log file
	logF.close();

	//return good
	return SUCCESS;
}

//Writes a logfile record
int Log::writeLogRecord(string s)
{
	//write a log record with timestamp
	logF << this->getTimeStamp();
	logF << s << endl << endl;

	//return good
	return SUCCESS;
}