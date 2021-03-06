#include <iostream>
#include <fstream>
#include <sstream>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <cstring>


#include "header/Base.h"

#define BUFFER_SIZE 100

using namespace std;

string fileToString(const string& fileName)
{
  ifstream fs{fileName};
  ostringstream str_stream;

  if(fs.fail()) {
    cout << "Error: opening " << fileName << endl;
  }

  fs >> str_stream.rdbuf();
  
  if (fs.fail() && !fs.eof()) {
    cout << "Error: reading " << fileName << endl;
  }

  return str_stream.str();
}

bool Command::isEmpty() {
  if(this->args.size() == 0)
  {
    return true;
  }
  else
  {
  return false;
  }
}

bool Command::eval() {
  if(this->args.size() < 1 || this->args.at(0) == "") 
  {
    cout << "Error: Missing command." << endl;
    return false;
  }
  
  if(this->args.at(0) == "exit") 
  {
    exit(0);
  } 
  
  else if(this->args.at(0) == "test") 
  {
    return (new TestCommand(this->args))->eval();
  }
  bool s = this->runCommand(this->args);
 
  return s;
}

bool Command::runCommand(vector<string>& args) {
  char ** argv = new char* [args.size() + 1];

  for(unsigned i = 0; i < args.size(); ++i) {
    argv[i] = const_cast<char*>(args.at(i).c_str());
  }
  argv[args.size()] = nullptr;
  int internalPipeOut[2];

  if(!this->outputFile.empty()) 
  {
    if(pipe(this->outputPipe) == -1) {
      cout << "Error: cannot create output pipe" << endl;
    }
  } 
  
  
  else 
  {
    if(pipe(internalPipeOut) == -1) {
      cout << "Error: cannot create internalPipeOut" << endl;
    }
  }

  pid_t pid = fork();
  int commandStatus;

  if(pid == 0) {  
    if(!this->inputFile.empty())
    {  
     
      if(pipe(this->inputPipe) == -1) 
      {
        cout << "Error creating input pipe" << endl;
      }
      string fileContents = fileToString(this->inputFile);

      if(dup2(this->inputPipe[0], STDIN_FILENO) == -1) {
        cout << "Error creating input pipe in child" << endl;
      }
      write(this->inputPipe[1], fileContents.c_str(), fileContents.size());
      close(this->inputPipe[1]);
    } 
    
    else if(!this->inputString.empty()) 
    {
      int internalPipeIn[2];
      pipe(internalPipeIn);

      dup2(internalPipeIn[0], STDIN_FILENO);
      write(internalPipeIn[1], this->inputString.c_str(), this->inputString.size());
      close(internalPipeIn[0]);
      close(internalPipeIn[1]);
    }

    if(!this->outputFile.empty()) {
      close(this->outputPipe[0]);
      
      if(dup2(this->outputPipe[1], STDOUT_FILENO) == -1) 
      {
        cout << "Error creating output pipe in child" << endl;
      }
      close(this->outputPipe[1]);
    } 
    else {
      
      close(internalPipeOut[0]);
      if(dup2(internalPipeOut[1], STDOUT_FILENO) == -1) 
      {  
        cout << "Error calling dup2() on internalPipeOut in child" << endl;
      }
      close(internalPipeOut[1]);
    }

    if(execvp(argv[0], argv) < 0) {
      cout << "No command \"" << argv[0] << "\" found." << endl;
      exit(1);
    }
    exit(0);
  } 
  else if(pid > 0) 
  {  
    if(!this->outputFile.empty())
    {
      close(this->outputPipe[1]);

      char buf;
      ofstream ofs;

      if(this->appendOutput) {
        ofs.open(this->outputFile.c_str(), ios::app);
      } else {
        ofs.open(this->outputFile.c_str());
      }
      
      ssize_t bytesRead = 0;
      while((bytesRead = read(this->outputPipe[0], &buf, 1)) > 0) 
      {
        ofs << buf;
      }
      
    } else {
      close(internalPipeOut[1]);

 
      stringstream ss;
      
      char buf;

      ssize_t bytesRead = 0;
      while((bytesRead = read(internalPipeOut[0], &buf, 1)) > 0) 
      {
        ss << buf;
      }
      close(internalPipeOut[0]);
      this->outputString = ss.str();

      if(this->shouldPrint) 
      {
        cout << this->outputString;
      }

    }

    waitpid(pid, &commandStatus, 0);
    delete[] argv;

    if(WEXITSTATUS(commandStatus) == 0) return true;
    
    return false;
  } 
  else {  // fork failed
    delete[] argv;
    
    return false;
  }
}

bool TestCommand::runCommand(vector<string>& args) {
  if(args.size() < 2) 
  {
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
    cout << "Invalid test command." << endl;
    return false;
  }
  cout << (status ? "(True)" : "(False)") << endl;
  return (status ? true : false);
}

bool TestCommand::eval() {
  return this->runCommand(this->args);
}
