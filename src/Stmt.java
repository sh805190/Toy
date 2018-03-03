package com.krgamestudios.toy;

import java.util.List;

abstract class Stmt {
	interface Visitor<R> {
		R visit(Block stmt);
		R visit(Expression stmt);
		R visit(Log stmt);
		R visit(Var stmt);
	}

	abstract <R> R accept(Visitor<R> visitor);
	static class Block extends Stmt {
		final List<Stmt> statements;

		Block(List<Stmt> statements) {
			this.statements = statements;
		}

		<R> R accept(Visitor<R> visitor) {
			return visitor.visit(this);
		}
	}
	static class Expression extends Stmt {
		final Expr expression;

		Expression(Expr expression) {
			this.expression = expression;
		}

		<R> R accept(Visitor<R> visitor) {
			return visitor.visit(this);
		}
	}
	static class Log extends Stmt {
		final Expr expression;

		Log(Expr expression) {
			this.expression = expression;
		}

		<R> R accept(Visitor<R> visitor) {
			return visitor.visit(this);
		}
	}
	static class Var extends Stmt {
		final Token name;
		final Expr initializer;

		Var(Token name, Expr initializer) {
			this.name = name;
			this.initializer = initializer;
		}

		<R> R accept(Visitor<R> visitor) {
			return visitor.visit(this);
		}
	}
}
