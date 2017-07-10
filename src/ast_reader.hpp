#pragma once

#include "expr.hpp"
#include "stmt.hpp"

class ASTReader: public ExprVisitor, public StmtVisitor {
public:
  virtual void Print(Stmt*) = 0;
  virtual void Print(Expr*) = 0;
};

class ASTReaderPrefix: public ASTReader {
public:
  void Print(Stmt*) override;
  void Print(Expr*) override;

  //visit methods
  void Visit(Stmt*) override;
  void Visit(Expression*) override;
  void Visit(Expr*) override;
  void Visit(Binary*) override;
  void Visit(Grouping*) override;
  void Visit(Logical*) override;
  void Visit(Unary*) override;
  void Visit(Value*) override;
};

class ASTReaderPostfix: public ASTReader {
public:
  void Print(Stmt*) override;
  void Print(Expr*) override;

  //visit methods
  void Visit(Stmt*) override;
  void Visit(Expression*) override;
  void Visit(Expr*) override;
  void Visit(Binary*) override;
  void Visit(Grouping*) override;
  void Visit(Logical*) override;
  void Visit(Unary*) override;
  void Visit(Value*) override;
};
