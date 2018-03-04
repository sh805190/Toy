package com.krgamestudios.toy;

import java.util.List;

abstract class Stmt {
	interface Visitor<R> {
		R visit(Block stmt);
		R visit(Break stmt);
		R visit(Expression stmt);
		R visit(If stmt);
		R visit(Log stmt);
		R visit(Var stmt);
		R visit(While stmt);
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
	static class Break extends Stmt {

		Break() {
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
	static class If extends Stmt {
		final Expr condition;
		final Stmt thenBranch;
		final Stmt elseBranch;

		If(Expr condition, Stmt thenBranch, Stmt elseBranch) {
			this.condition = condition;
			this.thenBranch = thenBranch;
			this.elseBranch = elseBranch;
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
	static class While extends Stmt {
		final Expr condition;
		final Stmt body;

		While(Expr condition, Stmt body) {
			this.condition = condition;
			this.body = body;
		}

		<R> R accept(Visitor<R> visitor) {
			return visitor.visit(this);
		}
	}
}
