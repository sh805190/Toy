#include "lexer.hpp"

#include "error_handler.hpp"

Lexer::Lexer(std::string s): source(s) {
  //
}

std::vector<Token> Lexer::GetTokenVector() {
  tokenVector.clear();

  while(!IsAtEnd()) {
    start = current;
    ScanToken();
  }

  tokenVector.push_back(Token(END_OF_FILE, "END_OF_FILE", new Literal(), line));
  return std::move(tokenVector);
}

void Lexer::ScanToken() {
  char c = Advance(); //work while pointing to the next character
  switch(c) {
    //single chars
    case '(': AddToken(LEFT_PAREN); break;
    case ')': AddToken(RIGHT_PAREN); break;
    case '{': AddToken(LEFT_BRACE); break;
    case '}': AddToken(RIGHT_BRACE); break;
    case '[': AddToken(LEFT_BRACKET); break;
    case ']': AddToken(RIGHT_BRACKET); break;
    case '+': AddToken(PLUS); break;
    case '&': AddToken(AMPERSAND); break;
    case ':': AddToken(COLON); break;
    case ',': AddToken(COMMA); break;
    case '.': AddToken(DOT); break;
    case ';': AddToken(SEMICOLON); break;

    case '*': ScanReference(); break; //unique
    case '"': ScanString(); break; //unique

    //double chars
    case '!': AddToken(Match('=') ? BANG_EQUAL : BANG); break;
    case '=': AddToken(Match('=') ? EQUAL_EQUAL : EQUAL); break;
    case '<': AddToken(Match('=') ? LESS_EQUAL : LESS); break;
    case '>': AddToken(Match('=') ? GREATER_EQUAL : GREATER); break;
    case '-': AddToken(Match('>') ? MINUS_GREATER : MINUS); break;

    //eat comments
    case '/':
      if (Match('/') || Match('*'))
        EatComments();
      else
        AddToken(SLASH);
    break;

    //eat whitespace
    case '\t':
    case '\r':
    case ' ':
      //DO NOTHING
      break;

    case '\n':
      line++;
    break;

    //otherwise
    default:
      //is number
      if (IsDigit(c)) { ScanNumber(); break; }

      //is identifer
      else if (IsAlpha(c)) { ScanIdentifier(); break; }

      //error
      else ErrorHandler::Error(line, std::string() + "Unexpected character: '" + c + "'");
    break;
  }
}

//helpers
void Lexer::AddToken(TokenType type) {
  AddToken(type, new Literal());
}

void Lexer::AddToken(TokenType type, Literal* literal) {
  std::string lexeme = source.substr(start, current-start);
  tokenVector.push_back(Token(type, lexeme, literal, line));
}

char Lexer::Advance() {
  current++;
  return source[current - 1];
}

void Lexer::EatComments() {
  if (source[current - 1] == '/') {
    //eat one line
    while(source[current] != '\n' && !IsAtEnd())
      Advance();
    if (source[current] == '\n') {
      Advance();
      line++;
    }
  }
  else {
    //eat nested multiline comments
    for (;;) {
      if (Match('/') && Match('*')) EatComments();
      else if (Match('*') && Match('/')) return;
      else if (Match('\n')) line++;
      else if (Match('\0')) return;
      else Advance();
    }
  }
}

bool Lexer::IsAlpha(char c) {
  return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
}

bool Lexer::IsAtEnd() {
  return current >= source.length();
}

bool Lexer::IsDigit(char c) {
  return (c >= '0' && c <= '9');
}

bool Lexer::Match(char expected) {
  //consume the match if true
  if (source[current] == expected) {
    Advance();
    return true;
  }
  return false;
}

void Lexer::ScanIdentifier() {
  //find the end of the identifier
  while(IsAlpha(source[current]) || IsDigit(source[current])) Advance();

  //check for reserved words
  std::string text = source.substr(start, current-start);

  //identifier
  if (keywords.find(text) == keywords.end()) {
    AddToken(IDENTIFIER, new lString(text));
  }
  else {
    //keyword
    AddToken(keywords[text]);
  }
}

void Lexer::ScanNumber() {
  while(IsDigit(source[current])) Advance();

  if (source[current] == '.') {
    Advance();
    while(IsDigit(source[current])) Advance();
  }

  double d = atof(source.substr(start, current-start).c_str());
  AddToken(NUMBER, new lNumber(d));
}

void Lexer::ScanReference() {
  double starCount = 1; //number of levels of dereferencing needed
  while(Match('*')) {
    starCount++;
  }
  AddToken(STAR, new lNumber(starCount));
}

void Lexer::ScanString() {
  int ln = line; //error handling

  //read the string
  while(!IsAtEnd() && source[current] != '"') {
    if (source[current] == '\n') {
      //disallow multiline strings
      ErrorHandler::Error(ln, std::string() + "Unterminated string (multiline strings are not allowed)");
      break;
    }
    Advance();
  }

  //error checking
 if (IsAtEnd()) {
    ErrorHandler::Error(ln, "Unterminated string");
  }

  //eat the ", or the \n
  Advance();

  //get the escaped string
  std::string str = source.substr(start+1, current-start-2); //-2 for the quotes

  //unescape the string before adding the token
  AddToken(STRING, new lString(UnescapeString(str)) );
}

std::string Lexer::UnescapeString(std::string str) {
  std::string ret;

  for (int i = 0; i < str.size(); i++) {
    if (str[i] == '\\') {
      if (str[i+1] == '\\') { ret += '\\'; i++; }
      else if (str[i+1] == '"') { ret += '"'; i++; }
      else if (str[i+1] == 'b') { ret += '\b'; i++; }
      else if (str[i+1] == 'n') { ret += '\n'; i++; }
      else if (str[i+1] == 'r') { ret += '\r'; i++; }
      else if (str[i+1] == 't') { ret += '\t'; i++; }
    }
    else {
      ret += str[i];
    }
  }
  return ret;
}
