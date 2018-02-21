#include <string>
#include <vector>
#include <boost/regex.hpp>
#include "header/Shell.h"
#include "header/Executable.h"
//#include "Shell.cpp"

using namespace std;

int main(int argc, char** argv) {
  // Convert argv to string vector
  vector<string> args(argc);
  
  string input_command;
  
  do
  {
    for(int i = 0; i < argc; ++i) 
    {
      args.at(i) = argv[i];
    }

    cout << "Enter Command: \n" << "$ ";
    getline(cin, input_command);
    
    unsigned int comment = input_command.find('#');
    input_command = input_command.substr(0, comment);
    if (comment == 0)
    {
      continue;
    }
    if(input_command.size() == 0)
    {
      cout << "Error No command \n"; 
    }
    
    else
    {
         Shell s(args);
      s.run(input_command);
    }
 
  } while(input_command != "exit");
  
  return 1;  // Non-zero exit code if exit command not used
}
