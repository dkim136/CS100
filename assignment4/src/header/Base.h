#ifndef _BASE_H
#define _BASE_H

#include <vector>
#include <string>
#include <iostream>

using namespace std;

class Base {
 public:
  string inputString;
  string outputString;
  bool shouldPrint = false;

  virtual bool eval() = 0;
  virtual bool isEmpty() = 0;
};

class Command : public Base {
 protected:
  vector<string> args;
  string inputFile, outputFile;  // Optional input/output files for streams
  bool appendOutput;

 public:
  int inputPipe[2];  // File descriptor for pipe, [0] is read and [1] is write
  int outputPipe[2];

  bool runCommand(vector<string>& args);
  Command() {}
  explicit Command(vector<string> a)
      : args(a), inputFile(""), outputFile(""), appendOutput(false) {
    
  }

  virtual bool eval();
  virtual bool isEmpty();

  vector<string> getArgs() { return args; }
  void setOutputFile(string newOut) { outputFile = newOut; }
  void setInputFile(string newIn) { inputFile = newIn;  }
  void setAppend(bool newState) { appendOutput = newState; }

 private:
  bool hasPipe() { return !(inputFile.empty() && outputFile.empty());  }
};


class NullCommand : public Command {
 public:
  NullCommand() {}
  explicit NullCommand(vector<string> a) : Command(a) {}
  virtual bool eval() { return true; }
  virtual bool isEmpty() { return true; }
};


class TestCommand : public Command {
 public:
  explicit TestCommand(vector<string> a) : Command(a) { }

  bool runCommand(vector<string>& args);
  virtual bool eval();
};

#endif
