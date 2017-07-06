#pragma once

enum TokenType {
  //single character operators
  LEFT_PAREN, RIGHT_PAREN, LEFT_BRACE, RIGHT_BRACE, LEFT_BRACKET, RIGHT_BRACKET,
  PLUS, MINUS, STAR, SLASH,
  BANG, EQUAL, LESS, GREATER,
  DOT, COLON, SEMICOLON, QUOTE,

  //two character operators
  BANG_EQUAL, EQUAL_EQUAL,
  LESS_EQUAL, GREATER_EQUAL,

  //keywords
  AND, BREAK, CASE, CLASS, CONTINUE, DEFAULT,
  ELSE, FALSE, FOR, FUNCTION, GOTO, IF, MODULE,
  OR, RETURN, SWITCH, TRUE, UNDEFINED, USE, VAR,
  WHILE,

  //literal types
  IDENTIFIER, NUMBER, STRING,

  //program terminator
  END_OF_FILE
};
