#include <iostream>
#include <vector>
#include <cstring>
#include <sstream>
#include <iterator>
#include <stack>
#include <boost/regex.hpp>

using namespace std;

#include "header/Shell.h"
#include "header/Executable.h"


Base* checkNull(stack<Base*>& s) {
  Base* pointer;
  if(s.empty()) 
  {
    return 0; 
  } 
  
  else 
  {
    pointer = s.top();
    s.pop();
  }
  return pointer;
}

Shell::Shell(vector<string> args) {
  if(args.size() < 1) 
  {
    return;
  }
}

bool Shell::run(string line) {
  Base* cmd = parseCommand(line);
  cmd->checkStatus = true;
  cmd->evaluate();
  
  return true;
 }

Base* Shell::parseCommand(string line) 
{
  
  vector<string> words;

  char *newInput = new char[line.size() + 1];
  strcpy(newInput, line.c_str());
  char * partInput = strtok(newInput, " ");

  while(partInput != NULL) {
    words.push_back(string(partInput));
    partInput = strtok(NULL, " ");
  }

  for(auto it = words.begin(); it != words.end(); ++it) {
    if(it->size() > 0 && it->at(it->size() - 1) == ';') {
      while(it->size() > 0 && it->at(it->size() - 1) == ';') {
        *it = it->substr(0, it->size() - 1);
      }
      it = words.insert(it + 1, ";");
    }
  }

   
  // Clean up
  delete[] newInput;
  delete[] partInput;
  newInput = NULL;
  partInput = NULL;

  vector<string> tempVect;
  stack<string> operators;
  stack<Base*> orders;

  // Do actual parsing
  for(unsigned i = 0; i < words.size(); ++i) {
    string parsed = words.at(i);
    if(!isOperator(parsed)) 
    {
      
      for(; i < words.size() && !isOperator(words.at(i)); ++i) 
      {
        tempVect.push_back(words.at(i));
      }
      --i;
      orders.push(new Command(tempVect));
      tempVect.clear();
      continue;
    }
    
    else if(isOperator(parsed))
    {
      operators.push(parsed);
    }
  }
  while(!operators.empty()) 
  {
    string op = operators.top();
   
    operators.pop();

    Base *rhs = checkNull(orders);
    Base *lhs = checkNull(orders);
    orders.push(checkOperator(op, lhs, rhs));
  }

  return orders.top();
}

Base* Shell::checkOperator(const string& op, Base* lhs, Base* rhs) {
  
  if(!isOperator(op)) {
    return NULL;
  }

  if(op == "||") {
    return new OR(lhs, rhs);
  }
  
  if(op == ";") {
    return new CMD(lhs, rhs);
  }
  
  if(op == "&&") {
    return new AND(lhs, rhs);
  }

  return NULL;  
}

bool Shell::isOperator(const string &s) {
  return s == "||" || s == "&&" || s == ";" ;
}


