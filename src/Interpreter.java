package com.krgamestudios.toy;

class Interpreter implements Expr.Visitor<Object> {
	public void interpret(Expr expression) {
		try {
			Object value = evaluate(expression);
			System.out.println(stringify(value));
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