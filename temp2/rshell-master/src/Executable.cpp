#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <cstring>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <cstring>
#include <sstream>
#include <iterator>
#include <stack>
#include <boost/regex.hpp>

using namespace std;

#include "header/Executable.h"
#define BUFFER_SIZE 100

bool AND::evaluate() {
  lhs->checkStatus = true;
  rhs->checkStatus = true;
  
  if(lhs->evaluate() == false)
  {
      return false;
  }
  
  return rhs->evaluate();
}

bool OR::evaluate() {
  lhs->checkStatus = true;
  
  if(lhs->evaluate() == true) 
  {
      return true;
  }
  
  rhs->checkStatus = true;
  
  return rhs->evaluate();
}

bool CMD::evaluate() {
  
  bool s = true;
  lhs->checkStatus = true;
  lhs->evaluate();
  rhs->checkStatus = true;
  
  if(!rhs->checkClear()) 
  {
      s = rhs->evaluate();
  }
  return s;
}

bool Connector::checkClear() {
  return lhs->checkClear() && rhs->checkClear();
}


string fileToString(const string& fileName) {
  ifstream fs{fileName};
  ostringstream str_stream;

  if(fs.fail()) {
    cout << "Error: cannot open " << fileName << endl;
  }

  fs >> str_stream.rdbuf();
  if (fs.fail() && !fs.eof()) {
    cout << "Error: cannot read " << fileName << endl;
  }

  return str_stream.str();
}

bool Command::checkClear() {
  if(this->args.size() == 0) return true;
  return false;
}

bool Command::evaluate() {
  if (this->args.size() < 1) 
  {
    cout << "Error: cannot find the command." << endl;
    return false;
  }
  
  else if ( this->args.at(0) == " ") 
  {
    cout << "Error: cannot find the command." << endl;
    return false;
  }
  
  else if (this->args.at(0) == "exit") {
    exit(0);
  } 
  
  else if (this->args.at(0) == "test") 
  {
    return (new Check(this->args))->evaluate();

  }
  bool s = this->runCommand(this->args);
  return s;
}

bool Command::runCommand(vector<string>& args) 
{
  char ** argv = new char* [args.size() + 1];
   int i_pout[2];
   int commandStatus;
   
  for(unsigned i = 0; i < args.size(); ++i) {
    argv[i] = const_cast<char*>(args.at(i).c_str());
  }
  
  argv[args.size()] = NULL;

  if(!this->f_out.empty()) 
  {
    /* Create the pipe. */
     if(pipe(this->p_output) < 0) 
      cout << "Error: cannot create p_output" << endl;
  }
  
  else 
  {
    if(pipe(i_pout) == -1) 
        cout << "Error: cannot create i_pout" << endl; 
  }
  
  
  pid_t pid = fork();
  
  if(pid > 0) { 
      char buf;
      ssize_t br = 0;
      ofstream ofs;
      
      //needs to check if the file is empty
    if(this->f_out.empty()) 
    {
       close(i_pout[1]);
      stringstream ss;
      
      //loop it until u find it
      while((br = read(i_pout[0], &buf, 1)) > 0) 
      {
        ss << buf;
      }
      
      close(i_pout[0]);
      
      this->outputString = ss.str();
      
      //needs to check if it's valid
      if(this->checkStatus) 
      {
        cout << this->outputString;
      }
    } 
    
    else 
    {
      
       close(this->p_output[1]);
  
      if(this->appendOutput) 
          ofs.open(this->f_out.c_str(), ios::app);
         
      else 
          ofs.open(this->f_out.c_str());
  
      while((br = read(this->p_output[0], &buf, 1)) > 0) 
        {
          ofs << buf;
        }

    }

    waitpid(pid, &commandStatus, 0);
    //just in case
    delete[] argv;

    if(WEXITSTATUS(commandStatus) == 0) 
    {
      return true;
    }

    return false;
  } 
  
  else if(pid == 0) 
  {  
        if(!this->f_in.empty()) 
        {  
            string fileContents = fileToString(this->f_in);
        
             write(this->p_input[1], fileContents.c_str(), fileContents.size());
             close(this->p_input[1]);
        } 
        
        else if(!this->inputString.empty()) 
        {
              int i_pin[2];
              pipe(i_pin);
        
              dup2(i_pin[0], STDIN_FILENO);
              write(i_pin[1], this->inputString.c_str(), this->inputString.size());
              close(i_pin[0]);
              close(i_pin[1]);
        }
    
        if(!this->f_out.empty()) 
        {
              close(this->p_output[0]);
              close(this->p_output[1]);
        } 
        
        else 
        {
          close(i_pout[0]);
          if(dup2(i_pout[1], STDOUT_FILENO) == -1) 
                 cout << "Error: cannot call dup2()" << endl;
          
          close(i_pout[1]);
        }
    
        if(execvp(argv[0], argv) < 0) 
        {
          cout << "Error: cannot find \"" << argv[0] << endl;
          exit(1);
        }
        exit(0);
  } 
  
  else {  
    //just in case
    delete[] argv;
    return false;
  }
}

bool Check::runCommand(vector<string>& args) {
  if(args.size() < 2) {
    cout << "Error: invalid." << endl;
    return false;
  } 
  
  else if(args.size() == 2) {
    args.insert(args.begin() + 1, "-e");
  }
  
  bool status = false;
  struct stat buf;

  if(args.at(1) == "-e") 
  {
    status = (stat(args.at(2).c_str(), &buf) != -1);
  } 
  
  else if(args.at(1) == "-d") 
  {
    status = (stat(args.at(2).c_str(), &buf) != -1) && S_ISDIR(buf.st_mode);
  }
  
  else if(args.at(1) == "-f") 
  {
    status = (stat(args.at(2).c_str(), &buf) != -1) && S_ISREG(buf.st_mode);
  }
  
  else 
  {
    cout << "Error: cannot test command." << endl;
    return false;
  }
  
  cout << (status ? "(True)" : "(False)") << endl;
  return (status ? true : false);
}

bool Check::evaluate() {
  return this->runCommand(this->args);
}
 