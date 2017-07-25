#include "ast_reader.hpp"

#include <iostream>

void ASTReader::Print(Stmt* stmt) {
//  std::cout << "(";
  stmt->Accept(this);
//  std::cout << ")";
}

void ASTReader::Print(Expr* expr) {
  std::cout << "(";
  expr->Accept(this);
  std::cout << ")";
}

void ASTReader::Visit(Stmt* stmt) {
  std::cout << "STMT";
}

void ASTReader::Visit(Block* stmt) {
  std::cout << "{ ";
  for (Stmt* ptr : stmt->stmtVector) {
    Print(ptr);
  }
  std::cout << " }";
}

void ASTReader::Visit(Break* stmt) {
  std::cout << "break";
}

void ASTReader::Visit(Continue* stmt) {
  std::cout << "continue";
}

void ASTReader::Visit(Expression* stmt) {
  Print(stmt->expr);
}

void ASTReader::Visit(If* stmt) {
  std::cout << "if ";
  Print(stmt->condition);
  std::cout << " then ";
  Print(stmt->thenBranch);
  if (stmt->elseBranch) {
    std::cout << " else ";
    Print(stmt->elseBranch);
  }
  std::cout << " endif";
}

void ASTReader::Visit(Return* stmt) {
  std::cout << "return";
  if (stmt->result) {
    std::cout << " ";
    Print(stmt->result);
  }
}

void ASTReader::Visit(Var* stmt) {
  std::cout << "Declare " << stmt->name.GetLexeme();
  //TODO: check back on this
  if (stmt->initializer) {
    std::cout << " = ";
    Print(stmt->initializer);
  }
}

void ASTReader::Visit(While* stmt) {
  std::cout << "while ";
  Print(stmt->condition);
  std::cout << " do ";
  Print(stmt->branch);
  std::cout << " endwhile";
}

void ASTReader::Visit(Expr* expr) {
  std::cout << "EXPR";
}

void ASTReader::Visit(Array* expr) {
  std::cout << "[";
  for (Expr* ptr : expr->exprVector) {
    Print(ptr);
    std::cout << ",";
  }
  std::cout << "]";
}

void ASTReader::Visit(Assign* expr) {
  std::cout << "= ";
  Print(expr->target);
  std::cout << " ";
  Print(expr->value);
}

void ASTReader::Visit(Binary* expr) {
  std::cout << expr->op.GetLexeme() << " ";
  Print(expr->lhs);
  std::cout << " ";
  Print(expr->rhs);
}

void ASTReader::Visit(Class* expr) {
  std::cout << "class ";
  Print(expr->block);
}

void ASTReader::Visit(Function* expr) {
  std::cout << "function( ";
  for (std::string arg : expr->parameterVector) {
    std::cout << arg << " ";
  }
  std::cout << ") ";
  Print(expr->block);
}

void ASTReader::Visit(Grouping* expr) {
  std::cout << "( ";
  Print(expr->inner);
  std::cout << " )";
}


void ASTReader::Visit(Index* expr) {
  Print(expr->array);
  std::cout << "[";
  Print(expr->index);
  std::cout << "]";
}

void ASTReader::Visit(Invocation* expr) {
  Print(expr->expr);
  std::cout << "(";
  for (Expr* ptr : expr->exprVector) {
    Print(ptr);
    std::cout << ",";
  }
  std::cout << ")";
}

void ASTReader::Visit(Logical* expr) {
  std::cout << expr->op.GetLexeme() << " ";
  Print(expr->lhs);
  std::cout << " ";
  Print(expr->rhs);
}

void ASTReader::Visit(Unary* expr) {
  std::cout << expr->op.GetLexeme();
  Print(expr->rhs);
}

void ASTReader::Visit(Value* expr) {
  std::cout << expr->value->ToString() << " {VAL} ";
}

void ASTReader::Visit(Variable* expr) {
  std::cout << expr->name.GetLexeme() << " {VAR} ";
}
