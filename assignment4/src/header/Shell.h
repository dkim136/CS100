#ifndef RSHELL_SHELL_H
#define RSHELL_SHELL_H

#include <string>
#include <vector>
#include "Base.h"

using namespace std;

class Shell {
 private:
  Base* parsingInput(string line);
 public:
  explicit Shell(vector<string> args);
  virtual bool run();
  static Base* applyOperator(const string& op, Base* lhs, Base* rhs);
  static bool isOperator(const string &s);
  static bool checkParen(const string &s);
  static bool checkSymbol(const string &s);
  static bool lowerPriority(const string &s1, const string &s2);
  static unsigned checkPriority(const string &s);
};

#endif
