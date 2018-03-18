//
// Class Main
//
// Runs the program, and implements the class objects for testing
//

#include <unistd.h>//include Unix library
#include <iostream>//include standard input output stream
#include <string.h>//inlcude string class
#include <fstream>//include the file stream header
#include <vector>//include the vector header
#include <sys/wait.h>//include for wait Function
#include <dirent.h>//include to acces the opendir and readdir functions 

#include "Log.h"//include the log file class 
#include "SetupData.h"//include the setupdata class
#include "Message.h"//include the message struct

// Shortcut for the std namespace
using namespace std;

//Define string length plus one for /0
#define STRLEN 128

//Function Headers for the different processes
void loggingProc(Log& log);
void parentProc(fstream& com);
void putProc();
void searchProc();
void countProc();

//Global variables for the processes
int value, pid;
int pipes[5][2];
std::map<string, int> files;

//main()
//used to run main part of program and grab arguments

int main(int argc, char** argv)
{
	//Used to get arguments from getopt()
	char pathname[STRLEN] = "";
	char filename[STRLEN] = "";
	//Used to open the setup file
	fstream f;
	fstream com;//Used to open the commands file
	vector<Message> msgs;//list of messages
	string temp;//to temporarirly hold a string for conversion
	
	

	//start setup file name to default value
	string setupname = "setup";
	strcpy(filename, setupname.c_str());

	char c; //for the return value of getopt()

	//If command arguments are not present error then quit
	if (argc == 2)
	{
		cout << "Usage: hw1 -p <pathname> [-s <setupfilename>]" << endl;

		exit(0);//Quit the program
	}//end if

	//loop through command line arguments
	while((c = getopt(argc, argv, "p:s:")) != -1)
	{
		switch(c)
		{
			case 'p':	strncpy(pathname, optarg, STRLEN-1); //copy the path name
					  	break;
			case 's':	strncpy(filename, optarg, STRLEN-1); //copy the setup file name
					  	break;
			default:  	break; //blank default
		};//end switch
	}//end while

	//Create the setupdata object using the user generated info
	SetupData data = SetupData(pathname, filename);

	//Ensure the setup file can open, if not send error
	if(data.open() == -2)
	{
		cout << data.error(-2);
		exit(0);
	}//end if

	//Read setup file
	data.read();

	//Create the log file using the user generated info
	Log log = Log(data.getLogfilename());

	//Ensure the log file can open, if not send error.
	if(log.open() == 1)
	{
		cout << "Error: Cannot open log file." << endl;
		exit(0);
	}//end if

	//Write log records for the setup file information
	log.writeLogRecord("Log File: " + data.getLogfilename());
	log.writeLogRecord("Command File: " + data.getCommandfilename());
	log.writeLogRecord("Username: " + data.getUsername());


	//Open the command file
	com.open(data.getCommandfilename(), ios::in);

	//Ensure the command file can open, if not send error
	if(com.fail())
	{
		cout << "Error: cannot open command file." << endl;
	}//end if

	//Initiliaze the pipes for communitcation
	for(int i = 0; i < 5; i++)
	{
		pipe(pipes[i]);
	}//end loop

	//fork a process
	value = fork();

	//If an error occurs quit, otherwise create the logging process
	if(value == -1)
	{
		cout << "Error in creating the Logging process!" << endl;
		exit(0);
	}
	else if(value == 0)
	{
		loggingProc(log);
		exit(0);
	}//end if

	//fork a process
	value = fork();

	//If an error occurs quit, otherwise create the put process
	if(value == -1)
	{
		cout << "Error in creating the put process!" << endl;
		exit(0);
	}
	else if(value == 0)
	{
		putProc();
		exit(0);
	}//end if

	//fork a process
	value = fork();

	//If an error occurs quit, otherwise create the search process
	if(value == -1)
	{
		cout << "Error in creating the search process!" << endl;
		exit(0);
	}
	else if(value == 0)
	{
		searchProc();
		exit(0);
	}//end if

	//fork a process
	value = fork();

	//If an error occurs quit, otherwise create the count process and parent process then wait for children
	if(value == -1)
	{
		cout << "Error in creating the count process!" << endl;
		exit(0);
	}
	else if(value == 0)
	{
		countProc();
		exit(0);
	}
	else
	{
		parentProc(com);
		wait(&pid);
	}//end if

	//close setup file
	data.close();

	log.close(); //close log

	return 0; //Requires an int return value, 0 = all things are good.
}//end main()

//Method to encapsulate the parent process
void parentProc(fstream& com)
{
	//Temporarily hold a string for conversions
	string temp;
	int counter = 0;

	//Grab all cmomands from the command file
	while(getline(com, temp))
	{	
		//Create the message and insert the id
		Message msg;
		msg.id = counter;
		
		//Assign the command
		msg.command = temp[0];

		//Get the key value and assign it 
		getline(com, temp);
		strcpy(msg.key, temp.c_str());

		//get the payload and assign it
		getline(com, temp);
		strcpy(msg.payload, temp.c_str());

		//Write a messeage to the logger
		write(pipes[1][1], (char*)&msg, sizeof(Message));

		switch(msg.command)
		{
			case 'p':
			case 'P':
				//If command is 'p' then write to the put process and logger process and output debug info
				write(pipes[2][1], (char*)&msg, sizeof(Message));
				read(pipes[0][0], (char*)&msg, sizeof(Message));

				cout << endl << "Parent recieved message #" << msg.id << ", key = " << msg.key << ", payload = " << msg.payload << endl;

				write(pipes[1][1], (char*)&msg, sizeof(Message));
				break;
			case 's':
			case 'S':
				//If command is 's' then write to the search process and logger process and output debug info
				write(pipes[3][1], (char*)&msg, sizeof(Message));
				read(pipes[0][0], (char*)&msg, sizeof(Message));

				cout << endl << "Parent recieved message #" << msg.id << ", key = " << msg.key << ", payload = " << msg.payload << endl;

				write(pipes[1][1], (char*)&msg, sizeof(Message));
				break;
			case 'n':
			case 'N':
				//If command is 'n' then write to the count process and logger process and output debug info
				write(pipes[4][1], (char*)&msg, sizeof(Message));
				read(pipes[0][0], (char*)&msg, sizeof(Message));

				cout << endl << "Parent recieved message #" << msg.id << ", key = " << msg.key << ", payload = " << msg.payload << endl;

				write(pipes[1][1], (char*)&msg, sizeof(Message));
				break;
			case 'q':
			case 'Q':
				//If command is 'q' then write quit to all processes
				write(pipes[2][1], (char*)&msg, sizeof(Message));
				write(pipes[3][1], (char*)&msg, sizeof(Message));
				write(pipes[4][1], (char*)&msg, sizeof(Message));
				break;
			default:
				break;
		}//end switch

		//increment id counter
		counter++;
	}//end loop
}//end parentProc

void loggingProc(Log& log)
{
	//Strings for placeholder values for conversion
	string placehold, placehold2, placehold3;
	Message msg;//Used to encapsulate a message


	do
	{
		//read in information from parent process
		read(pipes[1][0], (char*)&msg, sizeof(Message));

		//Write out debug info to terminal
		cout << endl << "Logger process recieved message #" << msg.id << ", key = " << msg.key << ", payload = " << msg.payload << endl;

		//place key and payload into temp strings
		placehold2 = string(msg.key);
		placehold3 = string(msg.payload);

		//Create log string and append all relevant information from message
		placehold = "Logger recieved message #" + to_string(msg.id);
		placehold.append(", key = ");
		placehold.append(msg.key);
		placehold.append(", payload = ");
		placehold.append(msg.payload);

		//log the message
		log.writeLogRecord(placehold);
	}while(msg.command != 'q');//end loop
}//end loggingProc

void putProc()
{
	//Used to encapsulate message
	Message msg;
	fstream file;//used to open a file
	string placehold;//used to temporarily hold a string for conversions

	//read in information from parent process
	read(pipes[2][0], (char*)&msg, sizeof(Message));

	//Write out debug info to terminal
	cout << endl << "Put process recieved message #" << msg.id << ", key = " << msg.key << ", payload = " << msg.payload << endl;

	do
	{
		//place key into temp string
		placehold = string(msg.key);

		//if file does not exist in map and command is not quit
		if(files.count(placehold) == 0) 
		{
			//insert new file into map
			files.insert(std::pair<string, int>(placehold, msg.id));

			//open the new file
			file.open(placehold, ios::out);

			//place payload into temp string
			placehold = string(msg.payload);

			//output payload to the file 
			file << placehold << endl;

			//close the file
			file.close();

			//place okay into temp string and copy to message payload
			placehold = "Okay";
			strcpy(msg.payload, placehold.c_str());

			//write message back to parent process
			write(pipes[0][1], (char*)&msg, sizeof(Message));
		}
		else
		{
			placehold = "Duplicate";

			strcpy(msg.payload, placehold.c_str());

			//write message back to parent process
			write(pipes[0][1], (char*)&msg, sizeof(Message));
		}//end if

		//read in information from parent process
		read(pipes[2][0], (char*)&msg, sizeof(Message));

		//Write out debug info to terminal
		cout << endl << "Put process recieved message #" << msg.id << ", key = " << msg.key << ", payload = " << msg.payload << endl;

	}while(msg.command != 'q');//end loop
}

void searchProc()
{
	//Used to encapsulate message
	Message msg;
	fstream file;//used to open a file
	string placehold;//used to temporarily hold a string for conversions

	//read in information from parent process
	read(pipes[3][0], (char*)&msg, sizeof(Message));

	//Write out debug info to terminal
	cout << endl << "Search process recieved message #" << msg.id << ", key = " << msg.key << ", payload = " << msg.payload << endl;

	do
	{
		//Put message key into temp string
		placehold = string(msg.key);

		//open the file
		file.open(placehold, ios::in);

		//if it failed to open return error
		if(file.fail())
		{
			//file not found put into temp string and copied to message payload
			placehold = "Not Found";
			strcpy(msg.payload, placehold.c_str());

			//close file
			file.close();

			//write message back to parent process
			write(pipes[0][1], (char*)&msg, sizeof(Message));
		}
		else
		{
			//Get the payload from the file and copy to message payload
			getline(file, placehold);
			strcpy(msg.payload, placehold.c_str());

			//close the file
			file.close();

			//write message back to parent process
			write(pipes[0][1], (char*)&msg, sizeof(Message));
		}//end if

		//read in information from parent process
		read(pipes[3][0], (char*)&msg, sizeof(Message));

		//Write out debug info to terminal
		cout << endl << "Search process recieved message #" << msg.id << ", key = " << msg.key << ", payload = " << msg.payload << endl;

	}while(msg.command != 'q');//end loop
}

void countProc()
{
	//Used to encapsulate a message
	Message msg;
	string placehold;//used to temporarily hold a string for conversions
	int numfiles = 0;//used to hold number of files
	DIR* dirptr;//used to hold directory ptr
	struct dirent* entry;//used to hold a file entry from directory
	char* pathname;

	//read in information from parent process
	read(pipes[4][0], (char*)&msg, sizeof(Message));
		
	//Write out debug info to terminal
	cout << endl << "Count process recieved message #" << msg.id << ", key = " << msg.key << ", payload = " << msg.payload << endl;

	do
	{
		//if directory does not exist
		if((dirptr = opendir(".")) == NULL)
		{
			//output error message
			cout << "Error openeing directory!" << endl;
		}
		else
		{
			//Otherwise loop through everyfile in directory and count them
			while((entry = readdir(dirptr)) != NULL)
			{
				numfiles++;//increment number of files
			}//end loop
		}//end if

		//minus the files already present from the count to get true total
		numfiles = numfiles - 5;

		//put map size into temp string and copy to message payload
		placehold = to_string(numfiles);
		strcpy(msg.payload, placehold.c_str());

		//write message back to parent process
		write(pipes[0][1], (char*)&msg, sizeof(Message));

		numfiles = 0;//reset number of files

		//read in information from parent process
		read(pipes[4][0], (char*)&msg, sizeof(Message));
		
		//Write out debug info to terminal
		cout << endl << "Count process recieved message #" << msg.id << ", key = " << msg.key << ", payload = " << msg.payload << endl;

	}while(msg.command != 'q');//end loop
}