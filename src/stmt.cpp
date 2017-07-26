#include "stmt.hpp"

int Stmt::count = 0;

#include "garbage_collector.hpp"
#include "literal.hpp"

Stmt::Stmt() {
  count++;
  GarbageCollector<Stmt>::Push(this);
}

Stmt::Stmt(int ln) {
  line = ln;
  count++;
  GarbageCollector<Stmt>::Push(this);
}

Stmt::~Stmt() {
  count--;
  GarbageCollector<Stmt>::Pop(this);
}

void Stmt::Accept(StmtVisitor* visitor) {
  visitor->Visit(this);
}
Block::Block(int ln, std::vector<Stmt*> stmtVector) {
    this->line = ln;
    this->stmtVector = stmtVector;
  }

Block::~Block() {
}

void Block::Accept(StmtVisitor* visitor) {
  visitor->Visit(this);
}

Break::Break(int ln) {
    this->line = ln;
  }

Break::~Break() {
}

void Break::Accept(StmtVisitor* visitor) {
  visitor->Visit(this);
}

Continue::Continue(int ln) {
    this->line = ln;
  }

Continue::~Continue() {
}

void Continue::Accept(StmtVisitor* visitor) {
  visitor->Visit(this);
}

Expression::Expression(int ln, Expr* expr) {
    this->line = ln;
    this->expr = expr;
  }

Expression::~Expression() {
}

void Expression::Accept(StmtVisitor* visitor) {
  visitor->Visit(this);
}

If::If(int ln, Expr* condition, Stmt* thenBranch, Stmt* elseBranch) {
    this->line = ln;
    this->condition = condition;
    this->thenBranch = thenBranch;
    this->elseBranch = elseBranch;
  }

If::~If() {
}

void If::Accept(StmtVisitor* visitor) {
  visitor->Visit(this);
}

Return::Return(int ln, Expr* result) {
    this->line = ln;
    this->result = result;
  }

Return::~Return() {
}

void Return::Accept(StmtVisitor* visitor) {
  visitor->Visit(this);
}

Use::Use(int ln, Token command) {
    this->line = ln;
    this->command = command;
  }

Use::~Use() {
}

void Use::Accept(StmtVisitor* visitor) {
  visitor->Visit(this);
}

Var::Var(int ln, Token name, Expr* initializer) {
    this->line = ln;
    this->name = name;
    this->initializer = initializer;
  }

Var::~Var() {
}

void Var::Accept(StmtVisitor* visitor) {
  visitor->Visit(this);
}

While::While(int ln, Expr* condition, Stmt* branch) {
    this->line = ln;
    this->condition = condition;
    this->branch = branch;
  }

While::~While() {
}

void While::Accept(StmtVisitor* visitor) {
  visitor->Visit(this);
}

