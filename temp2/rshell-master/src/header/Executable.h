#ifndef EXECUTABLE_H
#define EXECUTABLE_H

#include <iostream>
#include <vector>
#include <string>
#include <cstring>
#include <sstream>
#include <iterator>
#include <stack>
#include <boost/regex.hpp>

using namespace std;

class Base {
 public:
  string inputString;
  string outputString;
  bool checkStatus = false;

  virtual bool  evaluate() = 0;
  virtual bool checkClear() = 0;
};

class Command : public Base {
 protected:
  vector<string> args;
  string f_in, f_out; 
  bool appendOutput;

 public:
  int p_input[2]; 
  int p_output[2];

  bool runCommand(vector<string>& args);
  Command() {}
  explicit Command(vector<string> a)
    : args(a), f_in(""), f_out(""), appendOutput(false) 
  {}

  virtual bool evaluate();
  virtual bool checkClear();

  vector<string> getArgs() { return args; }
  void setf_out(string newOut) { f_out = newOut; }
  void setf_in(string newIn) { f_in = newIn; }
  void setAppend(bool newState) { appendOutput = newState; }

 private:
  
};


class NullCommand : public Command {
 public:
  NullCommand() {}
  explicit NullCommand(vector<string> a) : Command(a) {}
  virtual bool evaluate() { return true; }
  virtual bool checkClear() { return true; }
};


class Check : public Command {
 public:
  explicit Check(vector<string> a) : Command(a) { }

  bool runCommand(vector<string>& args);
  virtual bool evaluate();
};

class Connector : public Base {
 protected:
  Base* lhs;
  Base* rhs;
 public:
  Connector(Base* l, Base* r) : lhs(l), rhs(r) {}
  virtual bool evaluate() = 0;
  virtual bool checkClear();
};

class AND : public Connector {
 public:
  AND(Base* l, Base* r) : Connector(l, r) {}
  virtual bool evaluate();
};

class OR : public Connector {
 public:
  OR(Base* l, Base* r) : Connector(l, r) {}
  virtual bool evaluate();
};

class CMD : public Connector {
 public:
  CMD(Base* l, Base* r) : Connector(l, r) {}
  virtual bool evaluate();
};

#endif