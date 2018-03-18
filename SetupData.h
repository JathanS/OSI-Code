//
// Class setupData
//
// encapsulates the setupdata functionality
//

#ifndef SETUPDATA_H
#define SETUPDATA_H

#include <string>//include string class 
#include <fstream>//include the fstream class
#include <map>//include map class

using namespace std;//using standard namespace

// class SetupData
// Open, read, and display setup parameters
class SetupData {
public:
   // Public error codes
   static const int SUCCESS = 0;
   static const int BAD_PATHNAME = -1;
   static const int BAD_FILENAME = -2;

  // Constructors
  SetupData( );
  SetupData( string pname, string sname );

  // Getters and setters
  void setPathname(string pname);
  string getPathname();
  void setSetupfilename(string sname);
  string getSetupfilename();
  void setLogfile(string lname);
  string getLogfilename();
  void setCommandfilename(string cname);
  string getCommandfilename();
  void setUsername(string uname);
  string getUsername();

  // Open, read, display, and close
  int open();
  void read( );
  void print( );
  void close( );

  // Error stringifier
  string error(int e);

private:
  map<string, string> values; // Setup values
  fstream f; //stream to read in file
};

#endif
