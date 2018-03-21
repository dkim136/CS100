#ifndef _CONNECTOR_H
#define _CONNECTOR_H

#include <vector>
#include <string>
#include "Base.h"

using namespace std;

class Connector : public Base {
 protected:
  Base* lhs;
  Base* rhs;
 public:
  Connector(Base* l, Base* r) : lhs(l), rhs(r) {}
  virtual bool eval() = 0;
  virtual bool isEmpty();
};

class And : public Connector {
 public:
  And(Base* l, Base* r) : Connector(l, r) {}
  virtual bool eval();
};

class Or : public Connector {
 public:
  Or(Base* l, Base* r) : Connector(l, r) {}
  virtual bool eval();
};

class CmdConnector : public Connector {
 public:
  CmdConnector(Base* l, Base* r) : Connector(l, r) {}
  virtual bool eval();
};

class PipeConnector : public Connector {
 public:
  PipeConnector(Base* l, Base* r) : Connector(l, r) {}
  virtual bool eval();
};

#endif
