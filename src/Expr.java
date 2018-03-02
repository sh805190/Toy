package com.krgamestudios.toy;

import java.util.List;

abstract class Expr {
	interface Visitor<R> {
		R visit(Binary expr);
		R visit(Grouping expr);
		R visit(Literal expr);
		R visit(Postfix expr);
		R visit(Prefix expr);
	}

	abstract <R> R accept(Visitor<R> visitor);
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
}
