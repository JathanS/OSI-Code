//
// Class setupdata
//
// Implementation of setupdata class
//

#include <string>//string Library
#include <unistd.h>//include unix commands
#include <iostream>//Standard input outsput stream

#include "SetupData.h"//Include header file 

//using standard namesapce
using namespace std;

//default constructor
SetupData::SetupData() { }

//Overloaded constructor with path and setup file name
SetupData::SetupData( string pname, string sname )
{
	this->values["pathname"] = pname;
	this->values["setupname"] = sname;
}

//Getter and Setter for path name
void SetupData::setPathname(string pname)
{
	this->values["pathname"] = pname;
}

string SetupData::getPathname()
{
	return values["pathname"];
}

//Getter and Setter for setup file name
void SetupData::setSetupfilename(string sname)
{
	this->values["setupname"] = sname;
}

string SetupData::getSetupfilename()
{
	return values["setupname"];
}

//Getter and Setter for log file name
void SetupData::setLogfile(string lname)
{
	this->values["logfile"] = lname;
}

string SetupData::getLogfilename()
{
	return values["logfile"];
}

//Getter and Setter for command file name
void SetupData::setCommandfilename(string cname)
{
	this->values["commandfile"] = cname;
}

string SetupData::getCommandfilename()
{
	return this->values["commandfile"];
}

//Getter and Setter for user name
void SetupData::setUsername(string uname)
{
	this->values["username"] = uname;
}

string SetupData::getUsername()
{
	return values["username"];
}

//Opens the setup file
int SetupData::open()
{
	//Change directory, if bad pathname return error
	if(chdir(values["pathname"].c_str()) == -1)
	{
		return BAD_PATHNAME;
	}//end if

	this->f.open(values["setupname"], ios::in);//Open the setup file

	//Open setup file if bad filename return error
	if(this->f.fail())
	{
		return BAD_FILENAME;
	}//end if

	//return good
	return SUCCESS;
}

//Reads the setup file
void SetupData::read( )
{
	string temp;//temp string to hold the read lines
	int index;//index for finding :

	//while there is data to read
	while(getline(this->f, temp))
	{		
		//find delimiter then seperate string and store in map
		index = temp.find(":");
		this->values[temp.substr(0, index)] = temp.substr(index + 2);
	}//end loop
}

//Prints the data
void SetupData::print( )
{
	//print out the map stored in the object
	cout << "Path Name: " << values["pathname"] << endl;
	cout << "Setup File Name: " << values["setupname"] << endl;
	cout << "Log File Name: " << values["logfile"] << endl;
	cout << "Command File Name: " << values["commandfile"] << endl;
	cout << "User Name: " << values["username"] << endl;
}

//Closes the setup file
void SetupData::close( )
{
	f.close();
}

//Gets error and returns an error as a sentence
string SetupData::error(int e)
{
	//Return the type error code as a string.
	if(e == -1)
	{
		return "Error: Bad Path";
	}
	else if(e == -2)
	{
		return "Error: Bad File Name\n";
	}//end if
}