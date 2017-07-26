#include "expr.hpp"

int Expr::count = 0;

#include "garbage_collector.hpp"
#include "literal.hpp"

Expr::Expr() {
  count++;
  GarbageCollector<Expr>::Push(this);
}

Expr::Expr(int ln) {
  line = ln;
  count++;
  GarbageCollector<Expr>::Push(this);
}

Expr::~Expr() {
  count--;
  GarbageCollector<Expr>::Pop(this);
}

void Expr::Accept(ExprVisitor* visitor) {
  visitor->Visit(this);
}
Array::Array(int ln, std::vector<Expr*> exprVector) {
    this->line = ln;
    this->exprVector = exprVector;
  }

Array::~Array() {
}

void Array::Accept(ExprVisitor* visitor) {
  visitor->Visit(this);
}

Assign::Assign(int ln, Expr* target, Expr* value) {
    this->line = ln;
    this->target = target;
    this->value = value;
  }

Assign::~Assign() {
}

void Assign::Accept(ExprVisitor* visitor) {
  visitor->Visit(this);
}

Binary::Binary(int ln, Expr* lhs, Token op, Expr* rhs) {
    this->line = ln;
    this->lhs = lhs;
    this->op = op;
    this->rhs = rhs;
  }

Binary::~Binary() {
}

void Binary::Accept(ExprVisitor* visitor) {
  visitor->Visit(this);
}

Class::Class(int ln, Block* block) {
    this->line = ln;
    this->block = block;
  }

Class::~Class() {
}

void Class::Accept(ExprVisitor* visitor) {
  visitor->Visit(this);
}

Function::Function(int ln, std::vector<std::string> parameterVector, Block* block) {
    this->line = ln;
    this->parameterVector = parameterVector;
    this->block = block;
  }

Function::~Function() {
}

void Function::Accept(ExprVisitor* visitor) {
  visitor->Visit(this);
}

Grouping::Grouping(int ln, Expr* inner) {
    this->line = ln;
    this->inner = inner;
  }

Grouping::~Grouping() {
}

void Grouping::Accept(ExprVisitor* visitor) {
  visitor->Visit(this);
}

Index::Index(int ln, Expr* array, Expr* index) {
    this->line = ln;
    this->array = array;
    this->index = index;
  }

Index::~Index() {
}

void Index::Accept(ExprVisitor* visitor) {
  visitor->Visit(this);
}

Invocation::Invocation(int ln, Expr* expr, std::vector<Expr*> exprVector) {
    this->line = ln;
    this->expr = expr;
    this->exprVector = exprVector;
  }

Invocation::~Invocation() {
}

void Invocation::Accept(ExprVisitor* visitor) {
  visitor->Visit(this);
}

Logical::Logical(int ln, Expr* lhs, Token op, Expr* rhs) {
    this->line = ln;
    this->lhs = lhs;
    this->op = op;
    this->rhs = rhs;
  }

Logical::~Logical() {
}

void Logical::Accept(ExprVisitor* visitor) {
  visitor->Visit(this);
}

Unary::Unary(int ln, Token op, Expr* rhs) {
    this->line = ln;
    this->op = op;
    this->rhs = rhs;
  }

Unary::~Unary() {
}

void Unary::Accept(ExprVisitor* visitor) {
  visitor->Visit(this);
}

Value::Value(int ln, Literal* value) {
    this->line = ln;
    this->value = value;
  }

Value::~Value() {
}

void Value::Accept(ExprVisitor* visitor) {
  visitor->Visit(this);
}

Variable::Variable(int ln, Token name) {
    this->line = ln;
    this->name = name;
  }

Variable::~Variable() {
}

void Variable::Accept(ExprVisitor* visitor) {
  visitor->Visit(this);
}

