#include "ast_deleter.hpp"

#include <iostream>

#define DEBUGGING(x) ;

void ASTDeleter::DeleteAST(Expr* expr) {
  if (expr) {
    expr->Accept(this);
  }
}

void ASTDeleter::DeleteAST(Stmt* stmt) {
  if (stmt) {
    stmt->Accept(this);
  }
}

void ASTDeleter::Visit(Stmt* stmt) {
DEBUGGING(std::cout << "Deleting stmt: " << stmt << std::endl);
  //NO OP
  delete stmt;
}

void ASTDeleter::Visit(Block* stmt) {
DEBUGGING(std::cout << "Deleting Block.stmtVector: " << std::endl);
  for(Stmt* ptr : stmt->stmtVector) {
    DeleteAST(ptr);
  }
DEBUGGING(std::cout << "Deleting Block: " << stmt << std::endl);
  delete stmt;
}

void ASTDeleter::Visit(Break* stmt) {
DEBUGGING(std::cout << "Deleting Break: " << stmt << std::endl);
  delete stmt;
}

void ASTDeleter::Visit(Continue* stmt) {
DEBUGGING(std::cout << "Deleting Continue: " << stmt << std::endl);
  delete stmt;
}

void ASTDeleter::Visit(Expression* stmt) {
DEBUGGING(std::cout << "Deleting Expression.expr: " << stmt->expr << std::endl);
  DeleteAST(stmt->expr);
DEBUGGING(std::cout << "Deleting Expression: " << stmt << std::endl);
  delete stmt;
}

void ASTDeleter::Visit(If* stmt) {
DEBUGGING(std::cout << "Deleting If.condition: " << stmt->condition << std::endl);
  DeleteAST(stmt->condition);
DEBUGGING(std::cout << "Deleting If.themBranch: " << stmt->thenBranch << std::endl);
  DeleteAST(stmt->thenBranch);
DEBUGGING(std::cout << "Deleting If.elseBranch: " << stmt->elseBranch << std::endl);
  DeleteAST(stmt->elseBranch);
DEBUGGING(std::cout << "Deleting If: " << stmt << std::endl);
  delete stmt;
}

void ASTDeleter::Visit(Return* stmt) {
DEBUGGING(std::cout << "Deleting Return.result: " << stmt->result << std::endl);
  DeleteAST(stmt->result);
DEBUGGING(std::cout << "Deleting Return: " << stmt << std::endl);
  delete stmt;
}

void ASTDeleter::Visit(Var* stmt) {
DEBUGGING(std::cout << "Deleting Var.initializer: " << stmt->initializer << std::endl);
  DeleteAST(stmt->initializer);
DEBUGGING(std::cout << "Deleting Return: " << stmt << std::endl);
  delete stmt;
}

void ASTDeleter::Visit(While* stmt) {
DEBUGGING(std::cout << "Deleting While.condition: " << stmt->condition << std::endl);
  DeleteAST(stmt->condition);
DEBUGGING(std::cout << "Deleting While.branch: " << stmt->branch << std::endl);
  DeleteAST(stmt->branch);
DEBUGGING(std::cout << "Deleting While: " << stmt << std::endl);
  delete stmt;
}

void ASTDeleter::Visit(Expr* expr) {
  //NO OP
DEBUGGING(std::cout << "Deleting Expr: " << expr << std::endl);
  delete expr;
}

void ASTDeleter::Visit(Array* expr) {
DEBUGGING(std::cout << "Deleting Array.exprVector: " << std::endl);
  for (Expr* ptr : expr->exprVector) {
    DeleteAST(ptr);
  }
DEBUGGING(std::cout << "Deleting Array: " << expr << std::endl);
  delete expr;
}

void ASTDeleter::Visit(Assign* expr) {
DEBUGGING(std::cout << "Deleting Assign.target: " << expr->target << std::endl);
  DeleteAST(expr->target);
DEBUGGING(std::cout << "Deleting Assign.value: " << expr->value << std::endl);
  DeleteAST(expr->value);
DEBUGGING(std::cout << "Deleting Assign: " << expr << std::endl);
  delete expr;
}

void ASTDeleter::Visit(Binary* expr) {
DEBUGGING(std::cout << "Deleting Binary.lhs: " << expr->lhs << std::endl);
  DeleteAST(expr->lhs);
DEBUGGING(std::cout << "Deleting Binary.rhs: " << expr->rhs << std::endl);
  DeleteAST(expr->rhs);
DEBUGGING(std::cout << "Deleting Binary: " << expr << std::endl);
  delete expr;
}

void ASTDeleter::Visit(Class* expr) {
DEBUGGING(std::cout << "Deleting Class.block: " << expr->block << std::endl);
  DeleteAST(expr->block);
DEBUGGING(std::cout << "Deleting Class: " << expr << std::endl);
  delete expr;
}

void ASTDeleter::Visit(Function* expr) {
DEBUGGING(std::cout << "Deleting Function.block: " << expr->block << std::endl);
  DeleteAST(expr->block);
DEBUGGING(std::cout << "Deleting Function: "  << expr << std::endl);
  delete expr;
}

void ASTDeleter::Visit(Grouping* expr) {
DEBUGGING(std::cout << "Deleting Grouping.inner: "  << expr->inner << std::endl);
  DeleteAST(expr->inner);
DEBUGGING(std::cout << "Deleting Grouping: "  << expr << std::endl);
  delete expr;
}

void ASTDeleter::Visit(Index* expr) {
DEBUGGING(std::cout << "Deleting Index.array: "  << expr->array << std::endl);
  DeleteAST(expr->array);
DEBUGGING(std::cout << "Deleting Index.index: "  << expr->index << std::endl);
  DeleteAST(expr->index);
DEBUGGING(std::cout << "Deleting Index: "  << expr << std::endl);
  delete expr;
}

void ASTDeleter::Visit(Invocation* expr) {
DEBUGGING(std::cout << "Deleting Invocation.expr: "  << expr->expr << std::endl);
  DeleteAST(expr->expr);
DEBUGGING(std::cout << "Deleting Invocation.exprVector: " << std::endl);
  for (Expr* ptr : expr->exprVector) {
    DeleteAST(ptr);
  }
DEBUGGING(std::cout << "Deleting Invocation: "  << expr << std::endl);
  delete expr;
}

void ASTDeleter::Visit(Logical* expr) {
DEBUGGING(std::cout << "Deleting Logical.lhs: "  << expr << std::endl);
  DeleteAST(expr->lhs);
DEBUGGING(std::cout << "Deleting Logical.rhs: "  << expr << std::endl);
  DeleteAST(expr->rhs);
DEBUGGING(std::cout << "Deleting Logical: "  << expr << std::endl);
  delete expr;
}

void ASTDeleter::Visit(Unary* expr) {
DEBUGGING(std::cout << "Deleting Unary.rhs: "  << expr->rhs << std::endl);
  DeleteAST(expr->rhs);
DEBUGGING(std::cout << "Deleting Unary: "  << expr << std::endl);
  delete expr;
}

void ASTDeleter::Visit(Value* expr) {
DEBUGGING(std::cout << "Deleting Value: "  << expr << std::endl);
  delete expr;
}

void ASTDeleter::Visit(Variable* expr) {
DEBUGGING(std::cout << "Deleting Variable: "  << expr << std::endl);
  delete expr;
}
