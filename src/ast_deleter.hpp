#pragma once

#include "expr.hpp"
#include "stmt.hpp"

class ASTDeleter: public ExprVisitor, public StmtVisitor {
public:
  void DeleteAST(Expr*);
  void DeleteAST(Stmt*);

  //visit methods
  void Visit(Stmt*) override;
  void Visit(Expression*) override;
  void Visit(Expr*) override;
  void Visit(Binary*) override;
  void Visit(Grouping*) override;
  void Visit(Unary*) override;
  void Visit(Value*) override;
};
