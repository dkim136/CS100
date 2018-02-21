#ifndef RSHELL_SHELL_H
#define RSHELL_SHELL_H

#include <string>
#include <vector>
#include "Executable.h"

using namespace std;

class Shell {
 private:
  Base* parseCommand(string line);
 public:
  
  explicit Shell(vector<string> args);
  virtual bool run(string input_command);
  static Base* checkOperator(const string& op, Base* lhs, Base* rhs);
  static bool isOperator(const string &s);

};

#endif
