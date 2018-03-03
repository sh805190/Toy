package com.krgamestudios.toy;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import static com.krgamestudios.toy.TokenType.*;

class Scanner {
	private static final Map<String, TokenType> keywords;

	static {
		keywords = new HashMap<>();
		keywords.put("break", BREAK);
		keywords.put("class", CLASS);
		keywords.put("continue", CONTINUE);
		keywords.put("else", ELSE);
		keywords.put("false", FALSE);
		keywords.put("for", FOR);
//		keywords.put("foreach", FOREACH);
		keywords.put("function", FUNCTION);
		keywords.put("if", IF);
		keywords.put("log", LOG);
		keywords.put("module", MODULE);
		keywords.put("recurse", RECURSE);
		keywords.put("return", RETURN);
		keywords.put("this", THIS);
		keywords.put("true", TRUE);
		keywords.put("undefined", UNDEFINED);
		keywords.put("use", USE);
		keywords.put("var", VAR);
		keywords.put("while", WHILE);
	}

	private final String source;
	private final List<Token> tokens = new ArrayList<>();
	private int start = 0;
	private int current = 0;
	private int line = 1;

	Scanner(String source) {
		this.source = source;
	}

	List<Token> scanTokens() {
		while(!isAtEnd()) {
			start = current;
			scanToken();
		}

		tokens.add(new Token(EOF, "", null, line));
		return tokens;
	}

	private void scanToken() {
		char c = advance();
		switch(c) {
			//single chars
			case '(': addToken(LEFT_PAREN); break;
			case ')': addToken(RIGHT_PAREN); break;
			case '[': addToken(LEFT_BRACKET); break;
			case ']': addToken(RIGHT_BRACKET); break;
			case '{': addToken(LEFT_BRACE); break;
			case '}': addToken(RIGHT_BRACE); break;
			case '*': addToken(STAR); break;
			case '%': addToken(MODULUS); break;
			case ';': addToken(SEMICOLON); break;
			case ',': addToken(COMMA); break;

			//single/double chars
			case '=': addToken(match('=') ? EQUAL_EQUAL : match('>') ? EQUAL_GTHAN : EQUAL); break;
			case '<': addToken(match('=') ? LTHAN_EQUAL : LTHAN); break;
			case '>': addToken(match('=') ? GTHAN_EQUAL : GTHAN); break;
			case '!': addToken(match('=') ? BANG_EQUAL : BANG); break;
			case '&': addToken(match('&') ? AND_AND : AND); break;
			case '|': addToken(match('|') ? OR_OR : OR); break;
			case '+': addToken(match('+') ? PLUS_PLUS : PLUS); break;
			case '-': addToken(match('-') ? MINUS_MINUS : MINUS); break;
			case '.': addToken(match('.') ? DOT_DOT : DOT); break;

			//handle comments
			case '/':
				if (match('/')) {
					//single line comment
					while(peek() != '\n' && !isAtEnd()) advance();
				} else if (match('*')) {
					//multi-line comments
					while(!isAtEnd()) {
						if (match('*') && match('/')) break;
						if (peek() == '\n') line++;
						advance();
					}
				} else {
					addToken(SLASH);
				}
			break;

			//handle string literals
			case '"': handleString(); break;

			//handle whitespace
			case ' ':
			case '\r':
			case '\t':
				//ignore whitespace
			break;

			case '\n':
				line++;
			break;

			//handle errors
			default:
				if (isDigit(c)) {
					handleNumber();
				} else if (isAlpha(c)) {
					handleIdentifier();
				} else {
					Toy.error(line, "Unexpected character.");
				}
			break;
		}
	}

	private void handleString() {
		while(peek() != '"' && !isAtEnd()) {
			if (peek() == '\n') line++;
			advance();
		}

		//unterminated string
		if (isAtEnd()) {
			Toy.error(line, "Unterminated string.");
			return;
		}

		//the closing '"'
		advance();
		String value = source.substring(start +1, current -1);

		//TODO: unescape characters here

		addToken(STRING, value);
	}

	private void handleNumber() {
		while(isDigit(peek())) advance();

		if (peek() == '.' && isDigit(peekNext())) {
			//consume the '.'
			advance();
			while(isDigit(peek())) advance();
		}

		addToken(NUMBER, Double.parseDouble(source.substring(start, current)));
	}

	private void handleIdentifier() {
		while(isAlphaNumeric(peek())) advance();

		String text = source.substring(start, current);

		//check the identifier for reserved words
		TokenType type = keywords.get(text);
		if (type == null) type = IDENTIFIER;

		addToken(type);
	}

	private boolean isAtEnd() {
		return current >= source.length();
	}

	private char advance() {
		current++;
		return source.charAt(current-1);
	}

	private boolean match(char expected) {
		if (isAtEnd()) return false;
		if (source.charAt(current) != expected) return false;

		current++;
		return true;
	}

	private char peek() {
		if (isAtEnd()) return '\0';
		return source.charAt(current);
	}

	private char peekNext() {
		if (current +1 >= source.length()) return '\0';
		return source.charAt(current+1);
	}

	private boolean isDigit(char c) {
		return c >= '0' && c <= '9';
	}

	private boolean isAlpha(char c) {
		return
			(c >= 'a' && c <= 'z') ||
			(c >= 'A' && c <= 'Z') ||
			c == '_';
	}

	private boolean isAlphaNumeric(char c) {
		return isAlpha(c) || isDigit(c);
	}

	private void addToken(TokenType type) {
		addToken(type, null);
	}

	private void addToken(TokenType type, Object literal) {
		String text = source.substring(start, current);
		tokens.add(new Token(type, text, literal, line));
	}
}