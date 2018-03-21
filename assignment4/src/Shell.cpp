#include <iostream>
#include <vector>
#include <cstring>
#include <sstream>
#include <iterator>
#include <stack>
#include <boost/regex.hpp>
#include <cstdlib>

#include "header/Base.h"
#include "header/Shell.h"
#include "header/Connector.h"
using namespace std;

bool Shell::run() {
  
  while(true) {
    string input_command;

    cout << "Enter Command: \n" << "$ ";
    getline(cin, input_command);

    unsigned int comment = input_command.find("#");
    input_command = input_command.substr(0, comment);
    
    if(input_command.size() == 0)
    {
      cout << "Error No Command \n";
      continue;
    }
    
    else if ( input_command == "exit")
    {
      exit(0);
    }
    
    Base* command = parsingInput(input_command);
    if (command == NULL)
    {
      continue;
    }
    
    else
    {
    command->shouldPrint = true;
    command->eval();
    }
  }
  return true;
}

Base* checkNull(stack<Base*>& s) {
  Base* pointer;
  
  if(!s.empty()) 
  {
    pointer = s.top();
    s.pop();
  } 
  
  else 
  {
    cout << "Error: invalid" << endl;
    pointer = new NullCommand();
  }
  return pointer;
}


Shell::Shell(vector<string> args) {
  if(args.size() < 1) 
    return;
}


Base* Shell::parsingInput(string line) {
  vector<string> words;
  
  boost::regex testRegex;
  testRegex.assign("\\[(.+?)\\]");
  line = boost::regex_replace(line, testRegex, "test $1");

  // Split by spaces
  
  char *newInput = new char[line.size() + 1];
  strcpy(newInput, line.c_str());
  char *partInput = strtok(newInput, " ");

  while(partInput != NULL) {
    words.push_back(string(partInput));
    partInput = strtok(NULL, " ");
  }

  // Insert semicolons after commands
 auto it = words.begin();
  
  while(it != words.end())
  {
    if(it->size() > 0 && it->at(it->size() - 1) == ';') 
    {
      
      do{
        *it = it->substr(0, it->size() - 1);
      }
      while(it->size() > 0 && it->at(it->size() - 1) == ';');
      
      it = words.insert(it + 1, ";"); 
    }
    it++;
  }

  // Insert parentheses as seperate tokens
  int checkparen = 0;
  it = words.begin();
  
  while(it != words.end())  {
    if(it->size() > 1 && it->at(0) == '(') 
    {
      checkparen++;
      *it = it->substr(1);
      it = words.insert(it, "(");
    } 
    else if(it->size() > 1 && it->at(it->size() - 1) == ')') 
    {
      int parencount = 0;
      //checkparen--;
      
      while(it->size() > 1 && it->at(it->size() -1) == ')')
      {
        *it = it->substr(0, it ->size() -1);
        parencount++; 
        checkparen--;
      }
      
      int i = 0; 
      while(i < parencount)
      {
        it = words.insert(it + 1, ")");
        i++;
        
      }
      
    }
    it++;
    }
  
  if (checkparen != 0)
  {
    cout << "Error: no equal parenthesis" << endl << checkparen << " number of unmatching \n";
    return NULL;
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
    if(!checkSymbol(parsed)) {
      
      while(i < words.size() && !checkSymbol(words.at(i)))
      {
        tempVect.push_back(words.at(i));
        i++;
      }
      --i;
      
      orders.push(new Command(tempVect));
      tempVect.clear();
      continue;
    } 
    
    else if(parsed == "(") 
    {
      operators.push(parsed);
      continue;
    } 
    else if(parsed == ")") 
    {
      if(operators.empty())
      {
      cout << "Error: empty operators" << endl;
      return NULL;
      }
      
      while(!operators.empty() && operators.top() != "(") 
      {
        string op = operators.top();
        operators.pop();
        
        Base *rhs = checkNull(orders);
        Base *lhs = checkNull(orders);
        orders.push(applyOperator(op, lhs, rhs));
      }
      if(!operators.empty() && operators.top() == "(")
      {
        operators.pop();
      }

    } 
    
    else if(isOperator(parsed)) 
    {
      

      while(!operators.empty() && operators.top() != "("
          && !lowerPriority(operators.top(), parsed))
          {
        string op = operators.top();
        operators.pop();
        //if(checkParen(op)) continue;
        
        Base *rhs = checkNull(orders);
        Base *lhs = checkNull(orders);
        orders.push(applyOperator(op, lhs, rhs));
      }
      operators.push(parsed);
    }
  }
  while(!operators.empty()) {
    string op = operators.top();
    
    operators.pop();
    if(checkParen(op)) continue;

    Base *rhs = checkNull(orders);
    Base *lhs = checkNull(orders);
    orders.push(applyOperator(op, lhs, rhs));
  }

  if(!operators.empty()) {
    cout << "Error: invalid command" << endl;
  }
  return orders.top();
}

Base* Shell::applyOperator(const string& op, Base* lhs, Base* rhs) {
  if(!isOperator(op)) {
    cout << "Error: Invalid operator: " << op << endl;
    return NULL;
  }
  
  if(op == "||")
  {
    return new Or(lhs, rhs);
  }
  if(op == ";") 
  {
    return new CmdConnector(lhs, rhs);
  }
  if(op == "&&") 
  {
    return new And(lhs, rhs);
  }
  if(op == ">" || op == ">>") 
  {
    Command *leftCommand = static_cast<Command*>(lhs);
    Command *rightCommand = static_cast<Command*>(rhs);


    leftCommand->setOutputFile(rightCommand->getArgs().front());
    leftCommand->setAppend(op == ">>");
    return leftCommand;
  }

  if(op == "<") 
  {
    Command *leftCommand = static_cast<Command*>(lhs);
    Command *rightCommand = static_cast<Command*>(rhs);

    leftCommand->setInputFile(rightCommand->getArgs().front());
    return leftCommand;
  }

  

  if(op == "|") {

    return new PipeConnector(lhs, rhs);
  }

  return NULL;  // Should never happen
}

bool Shell::isOperator(const string &s) {
  if( s == ";" || s == "&&" || s == "||" || s == ">" ||
  s == ">>" || s == "<" || s == "|")
  return true;
  
  else
  return false;
}

bool Shell::checkParen(const string &s) {
  if(s == "(" || s == ")")
  return true;
  
  else 
  return false;
}

bool Shell::checkSymbol(const string &s) {
  return Shell::isOperator(s) || Shell::checkParen(s);
}

bool Shell::lowerPriority(const std::string &s1, const std::string &s2) {
  return checkPriority(s1) < checkPriority(s2);
}

unsigned Shell::checkPriority(const std::string &s) {
  if(checkParen(s)) 
  {
    return 0;
  }
  else if(s == "||" || s == "&&" || s == ";" || s == "|") 
  {
    return 1;
  }
  else
  {
  return 2;
  }
  
}
