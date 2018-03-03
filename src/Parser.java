package com.krgamestudios.toy;

import java.util.ArrayList;
import java.util.List;

import static com.krgamestudios.toy.TokenType.*;

class Parser {
	private static class ParseError extends RuntimeException {}

	private final List<Token> tokens;
	private int current = 0;

	Parser(List<Token> tokens) {
		this.tokens = tokens;
	}

	List<Stmt> parse() {
		List<Stmt> statements = new ArrayList<>();
		while(!isAtEnd()) {
			statements.add(declaration());
		}
		return statements;
	}

	//declarations
	private Stmt declaration() {
		try {
			//TODO: use
			if (match(VAR)) return varDeclaration();

			return statement();
		} catch(ParseError error) {
			syncronize();
			return null;
		}
	}

	private Stmt varDeclaration() {
		Token name = consume(IDENTIFIER, "Expected variable name.");

		Expr initializer = null;
		if (match(EQUAL)) {
			initializer = expression();
		}

		consume(SEMICOLON, "Expected ';' after variable declaration.");
		return new Stmt.Var(name, initializer);
	}

	//statements
	private Stmt statement() {
		if (match(LOG)) return logStatement();
		if (match(LEFT_BRACE)) return new Stmt.Block(block());

		return expressionStatement();
	}

	private List<Stmt> block() {
		List<Stmt> statements = new ArrayList<>();

		while (!check(RIGHT_BRACE) && !isAtEnd()) {
			statements.add(declaration());
		}

		consume(RIGHT_BRACE, "Expected '}' after block.");
		return statements;
	}

	private Stmt logStatement() {
		Expr value = expression();
		consume(SEMICOLON, "Expected ';' after value.");
		return new Stmt.Log(value);
	}

	private Stmt expressionStatement() {
		Expr expr = expression();
		consume(SEMICOLON, "Expected ';' after expression.");
		return new Stmt.Expression(expr);
	}

	//expressions
	private Expr expression() {
		return assignment();
	}

	private Expr assignment() {
		Expr expr = logicalOr();

		if (match(EQUAL)) {
			Token equals = previous();
			Expr value = assignment();

			if (expr instanceof Expr.Variable) {
				Token name = ((Expr.Variable)expr).name;
				return new Expr.Assign(name, value);
			}

			error(equals, "Invalid assignment target.");
		}

		return expr;
	}

	private Expr logicalOr() {
		return logicalAnd();
	}

	private Expr logicalAnd() {
		return equality();
	}

	private Expr equality() {
		Expr expr = comparison();

		while(match(BANG_EQUAL, EQUAL_EQUAL)) {
			Token operator = previous();
			Expr right = comparison();
			expr = new Expr.Binary(expr, operator, right);
		}

		return expr;
	}

	private Expr comparison() {
		Expr expr = addition();

		while(match(GTHAN, GTHAN_EQUAL, LTHAN, LTHAN_EQUAL)) {
			Token operator = previous();
			Expr right = addition();
			expr = new Expr.Binary(expr, operator, right);
		}

		return expr;
	}

	private Expr addition() {
		Expr expr = multiplication();

		while(match(PLUS, MINUS)) {
			Token operator = previous();
			Expr right = multiplication();
			expr = new Expr.Binary(expr, operator, right);
		}

		return expr;
	}

	private Expr multiplication() {
		Expr expr = prefix();

		while(match(SLASH, STAR, MODULUS)) {
			Token operator = previous();
			Expr right = prefix();
			expr = new Expr.Binary(expr, operator, right);
		}

		return expr;
	}

	private Expr prefix() {
		if (match(BANG, MINUS, PLUS_PLUS, MINUS_MINUS)) {
			Token operator = previous();
			Expr right = prefix();
			return new Expr.Prefix(operator, right);
		}

		return postfix();
	}

	private Expr postfix() {
		Expr expr = call();

		if (match(PLUS_PLUS, MINUS_MINUS)) {
			Token operator = previous();
			return new Expr.Postfix(expr, operator);
		}

		return expr;
	}

	private Expr call() {
		return primary();
	}

	private Expr primary() {
		if (match(FALSE)) return new Expr.Literal(false);
		if (match(TRUE)) return new Expr.Literal(true);
		if (match(UNDEFINED)) return new Expr.Literal(null);

		if (match(NUMBER, STRING)) {
			return new Expr.Literal(previous().literal);
		}

		//TODO: functions, classes

		if (match(IDENTIFIER)) {
			return new Expr.Variable(previous());
		}

		if (match(LEFT_PAREN)) {
			Expr expr = expression();
			consume(RIGHT_PAREN, "Expected ')' after expression.");
			return new Expr.Grouping(expr);
		}

		throw error(peek(), "Expression expected.");
	}

	//helpers
	private boolean match(TokenType... types) {
		for (TokenType type : types) {
			if (check(type)) {
				advance();
				return true;
			}
		}
		return false;
	}

	private boolean check(TokenType type) {
		if (isAtEnd()) return false;
		return peek().type == type;
	}

	private Token advance() {
		if (!isAtEnd()) current++;
		return previous();
	}

	private boolean isAtEnd() {
		return peek().type == EOF;
	}

	private Token peek() {
		return tokens.get(current);
	}

	private Token previous() {
		return tokens.get(current -1);
	}

	private Token consume(TokenType type, String errmsg) {
		if (check(type)) return advance();
		throw error(peek(), errmsg);
	}

	private ParseError error(Token token, String errmsg) {
		Toy.error(token, errmsg);
		return new ParseError();
	}

	private void syncronize() {
		advance();
		while(!isAtEnd()) {
			if (previous().type == SEMICOLON) return;
			advance();
		}
	}
}