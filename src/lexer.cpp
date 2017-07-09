#include "lexer.hpp"

#include "error_handler.hpp"

Lexer::Lexer(std::string s): source(s) {
  //
}

std::vector<Token> Lexer::GetTokenList() {
  tokenVector.clear();

  while(!IsAtEnd()) {
    start = current;
    ScanToken();
  }

  tokenVector.push_back(Token(END_OF_FILE, "END_OF_FILE", 0, line));
  return tokenVector;
}

void Lexer::ScanToken() {
  char c = Advance();
  switch(c) {
    //single chars
    case '(': AddToken(LEFT_PAREN); break;
    case ')': AddToken(RIGHT_PAREN); break;
    case '{': AddToken(LEFT_BRACE); break;
    case '}': AddToken(RIGHT_BRACE); break;
    case '[': AddToken(LEFT_BRACKET); break;
    case ']': AddToken(RIGHT_BRACKET); break;
    case '+': AddToken(PLUS); break;
    case '-': AddToken(MINUS); break;
    case '*': AddToken(STAR); break;
    case '.': AddToken(DOT); break;
    case ':': AddToken(COLON); break;
    case ';': AddToken(SEMICOLON); break;
    case '&': AddToken(AMPERSAND); break;

    case '"': ScanString(); break; //unique

    //double chars
    case '!': AddToken(Match('=') ? BANG : BANG_EQUAL); break;
    case '=': AddToken(Match('=') ? EQUAL : EQUAL_EQUAL); break;
    case '<': AddToken(Match('=') ? LESS : LESS_EQUAL); break;
    case '>': AddToken(Match('=') ? GREATER : GREATER_EQUAL); break;

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
  AddToken(type, -1);
}

void Lexer::AddToken(TokenType type, Literal literal) {
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
    AddToken(IDENTIFIER, text);
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
  AddToken(NUMBER, d);
}

void Lexer::ScanString() {
  int ln = line; //for error handling

  //read the string
  while(!IsAtEnd() && source[current] != '"') {
    if (source[current] == '\n') line++; //allow multiline strings
    Advance();
  }

  //error checking
 if (IsAtEnd()) {
    ErrorHandler::Error(ln, "Unterminated string");
  }

  //eat the "
  Advance();

  //finally
  std::string s = source.substr(start+1, current-start-2); //-2 for the quotes
  AddToken(STRING, s);
}

