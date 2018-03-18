//
// Struct Message
//
// Encapsulate the data for a command into a struct
//

#ifndef MESSAGE_H
#define MESSAGE_H

struct Message {
	int id;				//Unique identification number
	char command;		//Which command
	char key[16];		//Key value for the note
	char payload[128]; 	//The short note
};

#endif
