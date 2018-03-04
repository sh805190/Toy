package com.krgamestudios.toy;

import java.util.List;

abstract class Expr {
	interface Visitor<R> {
		R visit(Assign expr);
		R visit(Binary expr);
		R visit(Grouping expr);
		R visit(Literal expr);
		R visit(Logical expr);
		R visit(Postfix expr);
		R visit(Prefix expr);
		R visit(Variable expr);
	}

	abstract <R> R accept(Visitor<R> visitor);
	static class Assign extends Expr {
		final Token name;
		final Expr value;

		Assign(Token name, Expr value) {
			this.name = name;
			this.value = value;
		}

		<R> R accept(Visitor<R> visitor) {
			return visitor.visit(this);
		}
	}
	static class Binary extends Expr {
		final Expr left;
		final Token operator;
		final Expr right;

		Binary(Expr left, Token operator, Expr right) {
			this.left = left;
			this.operator = operator;
			this.right = right;
		}

		<R> R accept(Visitor<R> visitor) {
			return visitor.visit(this);
		}
	}
	static class Grouping extends Expr {
		final Expr expression;

		Grouping(Expr expression) {
			this.expression = expression;
		}

		<R> R accept(Visitor<R> visitor) {
			return visitor.visit(this);
		}
	}
	static class Literal extends Expr {
		final Object value;

		Literal(Object value) {
			this.value = value;
		}

		<R> R accept(Visitor<R> visitor) {
			return visitor.visit(this);
		}
	}
	static class Logical extends Expr {
		final Expr left;
		final Token operator;
		final Expr right;

		Logical(Expr left, Token operator, Expr right) {
			this.left = left;
			this.operator = operator;
			this.right = right;
		}

		<R> R accept(Visitor<R> visitor) {
			return visitor.visit(this);
		}
	}
	static class Postfix extends Expr {
		final Expr left;
		final Token operator;

		Postfix(Expr left, Token operator) {
			this.left = left;
			this.operator = operator;
		}

		<R> R accept(Visitor<R> visitor) {
			return visitor.visit(this);
		}
	}
	static class Prefix extends Expr {
		final Token operator;
		final Expr right;

		Prefix(Token operator, Expr right) {
			this.operator = operator;
			this.right = right;
		}

		<R> R accept(Visitor<R> visitor) {
			return visitor.visit(this);
		}
	}
	static class Variable extends Expr {
		final Token name;

		Variable(Token name) {
			this.name = name;
		}

		<R> R accept(Visitor<R> visitor) {
			return visitor.visit(this);
		}
	}
}
