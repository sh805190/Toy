#pragma once

class Stmt;
class Block;
class Break;
class Continue;
class Expression;
class If;
class Return;
class Use;
class Var;
class While;

#include "expr.hpp"
#include "stmt.hpp"
#include "token.hpp"

#include <string>
#include <vector>

class StmtVisitor {
public:
  virtual void Visit(Stmt*) = 0;
  virtual void Visit(Block*) = 0;
  virtual void Visit(Break*) = 0;
  virtual void Visit(Continue*) = 0;
  virtual void Visit(Expression*) = 0;
  virtual void Visit(If*) = 0;
  virtual void Visit(Return*) = 0;
  virtual void Visit(Use*) = 0;
  virtual void Visit(Var*) = 0;
  virtual void Visit(While*) = 0;
};

class Stmt {
public:
  Stmt();
  Stmt(int);
  virtual ~Stmt();
  virtual void Accept(StmtVisitor* visitor);

  int line = -1;
  static int count;
};

class Block: public Stmt {
public:
  Block(int ln, std::vector<Stmt*> stmtVector);
  virtual ~Block();
  void Accept(StmtVisitor* visitor) override;

  std::vector<Stmt*> stmtVector;
};

class Break: public Stmt {
public:
  Break(int ln);
  virtual ~Break();
  void Accept(StmtVisitor* visitor) override;
};

class Continue: public Stmt {
public:
  Continue(int ln);
  virtual ~Continue();
  void Accept(StmtVisitor* visitor) override;
};

class Expression: public Stmt {
public:
  Expression(int ln, Expr* expr);
  virtual ~Expression();
  void Accept(StmtVisitor* visitor) override;

  Expr* expr;
};

class If: public Stmt {
public:
  If(int ln, Expr* condition, Stmt* thenBranch, Stmt* elseBranch);
  virtual ~If();
  void Accept(StmtVisitor* visitor) override;

  Expr* condition;
  Stmt* thenBranch;
  Stmt* elseBranch;
};

class Return: public Stmt {
public:
  Return(int ln, Expr* result);
  virtual ~Return();
  void Accept(StmtVisitor* visitor) override;

  Expr* result;
};

class Use: public Stmt {
public:
  Use(int ln, Token command);
  virtual ~Use();
  void Accept(StmtVisitor* visitor) override;

  Token command;
};

class Var: public Stmt {
public:
  Var(int ln, Token name, Expr* initializer);
  virtual ~Var();
  void Accept(StmtVisitor* visitor) override;

  Token name;
  Expr* initializer;
};

class While: public Stmt {
public:
  While(int ln, Expr* condition, Stmt* branch);
  virtual ~While();
  void Accept(StmtVisitor* visitor) override;

  Expr* condition;
  Stmt* branch;
};
