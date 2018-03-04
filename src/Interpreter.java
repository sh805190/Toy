package com.krgamestudios.toy;

import java.util.List;

class Interpreter implements Expr.Visitor<Object>, Stmt.Visitor<Void> {
	private Environment environment = new Environment();

	public void interpret(List<Stmt> statements) {
		try {
			for (Stmt statement : statements) {
				execute(statement);
			}
		} catch(RuntimeError error) {
			Toy.runtimeError(error);
		}
	}

	private String stringify(Object object) {
		if (object == null) return "undefined";

		//hack, working around java adding ".0"
		if (object instanceof Double) {
			String text = object.toString();
			if (text.endsWith(".0")) {
				text = text.substring(0, text.length() - 2);
			}
			return text;
		}

		return object.toString();
	}

	//statements
	@Override
	public Void visit(Stmt.Block stmt) {
		executeBlock(stmt.statements, new Environment(environment));
		return null;
	}

	public Void visit(Stmt.Break stmt) {
System.out.println("INTERPRET BREAK");
		return null;
	}

	@Override
	public Void visit(Stmt.If stmt) {
		if (isTruthy(evaluate(stmt.condition))) {
			execute(stmt.thenBranch);
		} else if (stmt.elseBranch != null) {
			execute(stmt.elseBranch);
		}
		return null;
	}

	@Override
	public Void visit(Stmt.Log stmt) {
		Object value = evaluate(stmt.expression);
		System.out.println(stringify(value));
		return null;
	}

	@Override
	public Void visit(Stmt.Var stmt) {
		Object value = null;
		if (stmt.initializer != null) {
			value = evaluate(stmt.initializer);
		}

		environment.define(stmt.name, stmt.name.lexeme, value);
		return null;
	}

	@Override
	public Void visit(Stmt.While stmt) {
		while(isTruthy(evaluate(stmt.condition))) {
			execute(stmt.body);
		}
		return null;
	}

	@Override
	public Void visit(Stmt.Expression stmt) {
		evaluate(stmt.expression);
		return null;
	}

	//expressions
	@Override
	public Object visit(Expr.Assign expr) {
		Object value = evaluate(expr.value);

		environment.assign(expr.name, value);
		return value;
	}

	@Override
	public Object visit(Expr.Binary expr) {
		Object left = evaluate(expr.left);
		Object right = evaluate(expr.right);

		switch(expr.operator.type) {
			//arithmetic
			case PLUS:
				if (left instanceof Double && right instanceof Double) {
					return (double)left + (double)right;
				}

				if (left instanceof String && right instanceof String) {
					return (String)left + (String)right;
				}

				throw new RuntimeError(expr.operator, "Operands must be two numbers or two strings.");

			case MINUS:
				checkNumberOperands(expr.operator, left, right);
				return (double)left - (double)right;
			case STAR:
				checkNumberOperands(expr.operator, left, right);
				return (double)left * (double)right;
			case SLASH:
				checkNumberOperands(expr.operator, left, right);

				if ((double)right == 0) {
					throw new RuntimeError(expr.operator, "Cannot divide by zero.");
				}

				return (double)left / (double)right;
			case MODULUS:
				checkNumberOperands(expr.operator, left, right);
				return (double)left % (double)right;

			//comparison
			case GTHAN:
				checkNumberOperands(expr.operator, left, right);
				return (double)left > (double)right;
			case GTHAN_EQUAL:
				checkNumberOperands(expr.operator, left, right);
				return (double)left >= (double)right;
			case LTHAN:
				checkNumberOperands(expr.operator, left, right);
				return (double)left < (double)right;
			case LTHAN_EQUAL:
				checkNumberOperands(expr.operator, left, right);
				return (double)left <= (double)right;

			//equality
			case EQUAL_EQUAL:
				return isEqual(left, right);
			case BANG_EQUAL:
				return !isEqual(left, right);

			default:
				throw new RuntimeError(expr.operator, "Binary expression expected.");
		}
	}

	@Override
	public Object visit(Expr.Grouping expr) {
		return evaluate(expr.expression);
	}

	@Override
	public Object visit(Expr.Literal expr) {
		return expr.value;
	}

	@Override
	public Object visit(Expr.Logical expr) {
		Object left = evaluate(expr.left);

		if (expr.operator.type == TokenType.OR) {
			if (isTruthy(left)) return left;
		} else {
			if (!isTruthy(left)) return left;
		}

		return evaluate(expr.right);
	}

	@Override
	public Object visit(Expr.Postfix expr) {
		Object left = evaluate(expr.left);

		switch(expr.operator.type) {
			case PLUS_PLUS:
				checkNumberOperand(expr.operator, left);
				return (double)left + 1; //TODO: increment this value in the store
			case MINUS_MINUS:
				checkNumberOperand(expr.operator, left);
				return (double)left - 1; //TODO: increment this value in the store

			default:
				throw new RuntimeError(expr.operator, "Postfix expression expected.");
		}
	}

	@Override
	public Object visit(Expr.Prefix expr) {
		Object right = evaluate(expr.right);

		switch(expr.operator.type) {
			case BANG:
				return !isTruthy(right);
			case MINUS:
				checkNumberOperand(expr.operator, right);
				return -(double)right;
			case PLUS_PLUS:
				checkNumberOperand(expr.operator, right);
				return (double)right + 1; //TODO: increment this value in the store
			case MINUS_MINUS:
				checkNumberOperand(expr.operator, right);
				return (double)right - 1; //TODO: increment this value in the store

			default:
				throw new RuntimeError(expr.operator, "Prefix expression expected.");
		}
	}

	@Override
	public Object visit(Expr.Variable expr) {
		return environment.get(expr.name);
	}

	void executeBlock(List<Stmt> statements, Environment environment) {
		Environment previous = this.environment;
		try {
			this.environment = environment;

			for (Stmt statement : statements) {
				execute(statement);
			}
		} finally {
			this.environment = previous;
		}
	}

	private void execute(Stmt stmt) {
		stmt.accept(this);
	}

	private Object evaluate(Expr expr) {
		return expr.accept(this);
	}

	private boolean isTruthy(Object object) {
		if (object == null) return false;
		if (object instanceof Boolean) return (boolean)object;
		if (object instanceof Double) return (double)object != 0;
		return true;
	}

	private boolean isEqual(Object a, Object b) {
		if (a == null || b == null) return false;
		return a.equals(b);
	}

	private void checkNumberOperand(Token operator, Object operand) {
		if (operand instanceof Double) return;
		throw new RuntimeError(operator, "Operand must be a number.");
	}

	private void checkNumberOperands(Token operator, Object left, Object right) {
		if (left instanceof Double && right instanceof Double) return;
		throw new RuntimeError(operator, "Operands must be numbers.");
	}
}