#include "header/Connector.h"
#include <iostream>

using namespace std;

bool Connector::isEmpty() {
  return lhs->isEmpty() && rhs->isEmpty();
}

bool And ::eval() {
  lhs->shouldPrint = true;
  rhs->shouldPrint = true;
  if(lhs->eval() == false) return false;
  return rhs->eval();
}

bool Or::eval() {
  lhs->shouldPrint = true;
  
  if(lhs->eval() == true) 
  {
    return true;
  }
  else
  {
  rhs->shouldPrint = true;
  return rhs->eval();
  }
}

bool CmdConnector::eval() {
  bool s = true;

  lhs->shouldPrint = true;
  lhs->eval();
  rhs->shouldPrint = true;
  if(!rhs->isEmpty()) 
  {
    s = rhs->eval();
  }
  return s;
}


bool PipeConnector::eval() {
  bool s = true;

  lhs->eval();
 
  rhs->inputString = lhs->outputString;
  rhs->eval();
  
  this->outputString = rhs->outputString;

  if(this->shouldPrint) {
    cout << this->outputString;
  }

  return s;
}

