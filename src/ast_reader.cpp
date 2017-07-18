#include "ast_reader.hpp"

#include <iostream>

//prefix methods
void ASTReaderPrefix::Print(Stmt* stmt) {
//  std::cout << "(";
  stmt->Accept(this);
//  std::cout << ")";
}

void ASTReaderPrefix::Print(Expr* expr) {
  std::cout << "(";
  expr->Accept(this);
  std::cout << ")";
}

void ASTReaderPrefix::Visit(Stmt* stmt) {
  std::cout << "STMT";
}

void ASTReaderPrefix::Visit(Block* stmt) {
  std::cout << "{";
  for (Stmt* ptr : stmt->stmtVector) {
    Print(ptr);
  }
  std::cout << "}";
}

void ASTReaderPrefix::Visit(Break* stmt) {
  std::cout << "break";
}

void ASTReaderPrefix::Visit(Continue* stmt) {
  std::cout << "continue";
}

void ASTReaderPrefix::Visit(Expression* stmt) {
  Print(stmt->expr);
}

void ASTReaderPrefix::Visit(If* stmt) {
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

void ASTReaderPrefix::Visit(Return* stmt) {
  std::cout << "return ";
  Print(stmt->result);
}

void ASTReaderPrefix::Visit(Var* stmt) {
  std::cout << "Declare " << stmt->name.GetLexeme();
  //TODO: check back on this
  if (stmt->initializer != nullptr) {
    std::cout << " = ";
    Print(stmt->initializer);
  }
}

void ASTReaderPrefix::Visit(While* stmt) {
  std::cout << "while ";
  Print(stmt->condition);
  std::cout << " do ";
  Print(stmt->branch);
  std::cout << " endwhile";
}

void ASTReaderPrefix::Visit(Expr* expr) {
  std::cout << "EXPR";
}

void ASTReaderPrefix::Visit(Array* expr) {
  std::cout << "[";
  for (Expr* ptr : expr->exprVector) {
    Print(ptr);
    std::cout << ",";
  }
  std::cout << "]";
}

void ASTReaderPrefix::Visit(Assign* expr) {
  std::cout << "= ";
  Print(expr->target);
  std::cout << " ";
  Print(expr->value);
}

void ASTReaderPrefix::Visit(Binary* expr) {
  std::cout << expr->op.GetLexeme() << " ";
  Print(expr->lhs);
  std::cout << " ";
  Print(expr->rhs);
}

void ASTReaderPrefix::Visit(Class* expr) {
  std::cout << "class ";
  Print(expr->block);
}

void ASTReaderPrefix::Visit(Function* expr) {
  std::cout << "function( ";
  for (std::string arg : expr->parameterVector) {
    std::cout << arg << " ";
  }
  std::cout << ")";
  Print(expr->block);
}

void ASTReaderPrefix::Visit(Grouping* expr) {
  std::cout << "(";
  Print(expr->inner);
  std::cout << ")";
}


void ASTReaderPrefix::Visit(Index* expr) {
  Print(expr->array);
  std::cout << "[";
  Print(expr->index);
  std::cout << "]";
}

void ASTReaderPrefix::Visit(Invocation* expr) {
  Print(expr->expr);
  std::cout << "(";
  for (Expr* ptr : expr->exprVector) {
    Print(ptr);
    std::cout << ",";
  }
  std::cout << ")";
}

void ASTReaderPrefix::Visit(Logical* expr) {
  std::cout << expr->op.GetLexeme() << " ";
  Print(expr->lhs);
  std::cout << " ";
  Print(expr->rhs);
}

void ASTReaderPrefix::Visit(Unary* expr) {
  std::cout << expr->op.GetLexeme();
  Print(expr->rhs);
}

void ASTReaderPrefix::Visit(Value* expr) {
  std::cout << expr->value.ToString();
}

void ASTReaderPrefix::Visit(Variable* expr) {
  std::cout << expr->name.GetLexeme() << "{VARIABLE}";
}

//postfix methods
void ASTReaderPostfix::Print(Stmt* stmt) {
  stmt->Accept(this);
  std::cout << ";";
}

void ASTReaderPostfix::Print(Expr* expr) {
  expr->Accept(this);
}

void ASTReaderPostfix::Visit(Stmt* stmt) {
  std::cout << "STMT";
}

void ASTReaderPostfix::Visit(Block* stmt) {
  std::cout << "{";
  for (Stmt* ptr : stmt->stmtVector) {
    Print(ptr);
  }
  std::cout << "}";
}

void ASTReaderPostfix::Visit(Break* stmt) {
  std::cout << "break";
}

void ASTReaderPostfix::Visit(Continue* stmt) {
  std::cout << "continue";
}

void ASTReaderPostfix::Visit(Expression* stmt) {
  Print(stmt->expr);
}

void ASTReaderPostfix::Visit(If* stmt) {
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

void ASTReaderPostfix::Visit(Return* stmt) {
  Print(stmt->result);
  std::cout << " return";
}

void ASTReaderPostfix::Visit(Var* stmt) {
  std::cout << "Declare " << stmt->name.GetLexeme();
  //TODO: check back on this
  if (stmt->initializer != nullptr) {
    std::cout << " = (";
    Print(stmt->initializer);
    std::cout << ")";
  }
}

void ASTReaderPostfix::Visit(While* stmt) {
  std::cout << "while ";
  Print(stmt->condition);
  std::cout << " do ";
  Print(stmt->branch);
  std::cout << " endwhile";
}

void ASTReaderPostfix::Visit(Expr* expr) {
  std::cout << "EXPR";
}

void ASTReaderPostfix::Visit(Array* expr) {
  std::cout << "[";
  for (Expr* ptr : expr->exprVector) {
    Print(ptr);
    std::cout << ",";
  }
  std::cout << "]";
}

void ASTReaderPostfix::Visit(Assign* expr) {
  Print(expr->target);
  std::cout << " ";
  Print(expr->value);
  std::cout << " =";
}

void ASTReaderPostfix::Visit(Binary* expr) {
  Print(expr->lhs);
  std::cout << " ";
  Print(expr->rhs);
  std::cout << " " << expr->op.GetLexeme();
}

void ASTReaderPostfix::Visit(Class* expr) {
  std::cout << "class ";
  Print(expr->block);
}

void ASTReaderPostfix::Visit(Function* expr) {
  std::cout << "function( ";
  for (std::string arg : expr->parameterVector) {
    std::cout << arg << " ";
  }
  std::cout << ")";
  Print(expr->block);
}

void ASTReaderPostfix::Visit(Grouping* expr) {
  std::cout << "(";
  Print(expr->inner);
  std::cout << ")";
}

void ASTReaderPostfix::Visit(Index* expr) {
  Print(expr->array);
  std::cout << "[";
  Print(expr->index);
  std::cout << "]";
}

void ASTReaderPostfix::Visit(Invocation* expr) {
  Print(expr->expr);
  std::cout << "(";
  for (Expr* ptr : expr->exprVector) {
    Print(ptr);
    std::cout << ",";
  }
  std::cout << ")";
}

void ASTReaderPostfix::Visit(Logical* expr) {
  Print(expr->lhs);
  std::cout << " ";
  Print(expr->rhs);
  std::cout << " " << expr->op.GetLexeme();
}

void ASTReaderPostfix::Visit(Unary* expr) {
  if (expr->op.GetType() == BANG || expr->op.GetType() == MINUS) {
    Print(expr->rhs);
    std::cout << " negate";
  }
  else if (expr->op.GetType() == AMPERSAND) {
    Print(expr->rhs);
    std::cout << " reference";
  }
  else if (expr->op.GetType() == STAR) {
    Print(expr->rhs);
    std::cout << " dereference";
  }
  else {
    Print(expr->rhs);
    std::cout << expr->op.GetLexeme();
  }
}

void ASTReaderPostfix::Visit(Value* expr) {
  std::cout << expr->value.ToString();
}

void ASTReaderPostfix::Visit(Variable* expr) {
  std::cout << expr->name.GetLexeme() << "{VARIABLE}";
}
