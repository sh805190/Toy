/* A brief rundown of the visitor pattern

#include <iostream>

class Foo;
class Bar;

//base class for all visitors
class Visitor {
public:
  virtual void Visit(Foo*) = 0;
  virtual void Visit(Bar*) = 0;
};

//each visited class must accept
class Foo {
public:
  virtual void Accept(Visitor* visitor) {
    return visitor->Visit(this);
  }
};

class Bar: public Foo {
public:
  void Accept(Visitor* visitor) override {
    return visitor->Visit(this);
  }
};

//here is a concrete example of a visitor
class PrintClass: public Visitor {
  void Visit(Foo* foo) override {
    std::cout << "foo" << std::endl;
  }

  void Visit(Bar* bar) override {
    std::cout << "bar" << std::endl;
  }
};

//using the above examples
int main() {
  PrintClass print;
  Foo* foo = new Bar();

  foo->Accept(&print);
  delete foo;

  return 0;
}
*/

#include <algorithm>
#include <cctype>
#include <cstdarg>
#include <cstring>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

//split a string based on a single delimiter
std::vector<std::string> splitStringOneDelimiter(std::string str, char delim) {
  std::vector<std::string> strings;

  while(str.find_first_of(delim) != std::string::npos) {
    strings.push_back( str.substr(0, str.find_first_of(delim)) );
    str.erase(0, str.find_first_of(delim) + 1); //+1 to erase the delimiter
  }
  //end of the string
  strings.push_back(str);

  return strings;
}

//split strings with variable number of delimiters
std::vector<std::string> splitString(std::string s, std::string delim) {
  std::vector<std::string> strings = {s};
  std::vector<std::string> newStrings;

  while (delim.size()) {
    //get the delimiter
    char c = delim[0];
    delim.erase(0, 1);

    for (std::string str : strings) {
      //split this string and add it to newStrings
      std::vector<std::string> ret = splitStringOneDelimiter(str, c);
      newStrings.insert(newStrings.end(), ret.begin(), ret.end());
    }

   strings = newStrings;
   newStrings.clear(); 
  }

  //prune empty strings
  auto it = std::remove_if(strings.begin(), strings.end(), [](std::string s) -> bool { return !s.size(); });
  strings.erase(it, strings.end());

  return strings;
}

std::string toUpper(std::string s) {
  std::string output;
  for (char c : s) output += toupper(c);
  return output;

}
std::string toLower(std::string s) {
  std::string output;
  for (char c : s) output += tolower(c);
  return output;
}

void defineAST(std::ofstream& os, std::string baseName, std::vector<std::pair<std::string, std::string>> data) {
  //print the basics
  os << "#pragma once" << std::endl;
  os << std::endl;

  //forward declare the AST classes
  os << "class " << baseName << ";" << std::endl;
  for (auto& p : data) {
    os << "class " << p.first << ";" << std::endl;
  }

  os << std::endl;
  os << "#include \"expr.hpp\"" << std::endl;
  os << "#include \"stmt.hpp\"" << std::endl;
  os << "#include \"token.hpp\"" << std::endl;
  os << std::endl;

  os << "#include <string>" << std::endl;
  os << "#include <vector>" << std::endl;
  os << std::endl;

  //print the visitor class
  os << "class " << baseName << "Visitor {" << std::endl;
  os << "public:" << std::endl;
  os << "  virtual void Visit(" << baseName << "*) = 0;" << std::endl;
  for (auto& p : data) {
    os << "  virtual void Visit(" << p.first << "*) = 0;" << std::endl;
  }
  os << "};" << std::endl << std::endl;

  //print the base class
  os << "class " << baseName << " {" << std::endl;
  os << "public:" << std::endl;
  os << "  " << baseName << "() = default;" << std::endl;
  os << "  " << baseName << "(int ln) { line = ln; }" << std::endl;
  os << "  virtual void Accept(" << baseName << "Visitor* visitor) {" << std::endl;
  os << "    visitor->Visit(this);" << std::endl;
  os << "  }" << std::endl;
  os << "  int line = -1;" << std::endl;
  os << "};" << std::endl;

  //define each AST class
  for(auto& p : data) {
    //print the opening
    os << std::endl;
    os << "class " << p.first << ": public " << baseName << " {" << std::endl;
    os << "public:" << std::endl;
    os << "  " << p.first << "(int ln";
    if (p.second.size()) os << ", ";
    os << p.second << ") {" << std::endl;

    //set the member values
    std::vector<std::string> members = splitString(p.second, " ,");
    os << "this->line = ln;" << std::endl;
    for (int i = 0; i < members.size(); i += 2) { //+2, since the type declaration is mixed in
      os << "    this->" << members[i+1] << " = " << members[i+1] << ";" << std::endl;
    }

    //print the second part
    os << "  }" << std::endl << std::endl;
    os << "  void Accept(" << baseName << "Visitor* visitor) override {" << std::endl;
    os << "    visitor->Visit(this);" << std::endl;
    os << "  }" << std::endl;

    //print the members
    os << std::endl;
    members = splitString(p.second, ",");
    os << " ";
    for (std::string m : members) {
      os << " " << m << ";" << std::endl;
    }

    //close the class
    os << "};" << std::endl;
  }
}

void defineLiteral(std::ofstream& os, std::string baseName, std::vector<std::pair<std::string, std::string>> data) {
  //print the basics
  os << "#pragma once" << std::endl;
  os << std::endl;

  //forward declare the literal classes
  os << "class " << baseName << ";" << std::endl;
  for (auto& p : data) {
    os << "class " << p.first << ";" << std::endl;
  }
  os << std::endl;

  //declare the includes
  os << "#include <map>" << std::endl;
  os << "#include <string>" << std::endl;
  os << "#include <vector>" << std::endl;
  os << std::endl;

  //delcare the base class
  os << "class " << baseName << " {" << std::endl;
  os << "public:" << std::endl;
  os << "  enum class Type {" << std:: endl;
  os << "    ";
  for (auto& p : data) {
    os << toUpper(p.first) << ", ";
  }
  os << std::endl;
  os << "  };" << std::endl;
  os << std::endl;

  os << "  " << baseName << "() { count++; }" << std::endl;
  os << "  virtual ~" << baseName << "() { count--; }" << std::endl << std::endl;
  os << "  virtual Literal* Copy() { return new Literal(); }" << std::endl;
  os << "  virtual std::string ToString() { return \"LITERAL\"; }" << std::endl;

  os << "  Type type;" << std::endl;
  os << "  //debugging" << std::endl;
  os << "  static int count;" << std::endl;
  os << "};" << std::endl << std::endl;

  //declare each child class
  for (auto& p : data) {
    //parsing
    std::vector<std::string> memberDeclarations = splitString(p.second, "/");
    std::vector<std::string> memberNames = splitString(p.second, " /");

    os << "class " << p.first << ": public " << baseName << " {" << std::endl;
    os << "public:" << std::endl;

    //defaults
    os << "  " << p.first << "() = default;" << std::endl;
    os << "  virtual ~" << p.first << "() = default;" << std::endl;
    os << std::endl;

    if (memberDeclarations.size()) {
      //parameter constructor
      os << "  " << p.first << "(";
      for (int i = 0; i < memberDeclarations.size(); ) {
        os << memberDeclarations[i];
        i++;
        if (i < memberDeclarations.size()) {
          os << ",";
        }
      }
      os << ") {" << std::endl;
      for (int i = 0; i < memberNames.size(); i += 2) {
        os << "    this->" << memberNames[i+1] << " = std::move(" << memberNames[i+1] << ");" << std::endl;
      }
      os << "  }" << std::endl << std::endl;
    }

    //define the methods
    os << "  Literal* Copy() override {" << std::endl;
    os << "    return new " << p.first << "(";
    for (int i = 0; i < memberNames.size(); ) {
      os << memberNames[i+1];
      i += 2;
      if (i < memberNames.size()) os << ",";
    }
    os << ");" << std::endl;
    os << "  }" << std::endl;
    os << std::endl;

    os << "  std::string ToString() override {" << std::endl;
    os << "    return \""  << toLower(p.first) << "\";" << std::endl;
    os << "  }" << std::endl;

    //define the members
    for (std::string m : memberDeclarations) {
      os << "  " << m << ";" << std::endl;
    }

    //finish
    os << "};" << std::endl << std::endl;
  }
}

int run(int argc, std::vector<std::string> argv) {
  //ensure the correct usage
  if (argc != 3) {
    std::cout << "Usage: " << argv[0] << " OP fname" << std::endl;
    return 0;
  }

  //open the specified file for writing
  std::ofstream os(argv[2]);
  if (!os.is_open()) {
    std::cerr << "Failed to open " << argv[2] << " for writing" << std::endl;
    return 1;
  }

  //Expr
  if (!strcmp(argv[1].c_str(), "expr")) {
    defineAST(os, "Expr", {
      {"Array", "std::vector<Expr*> exprVector"},
      {"Assign", "Expr* target, Expr* value"}, 
      {"Binary", "Expr* lhs, Token op, Expr* rhs"},
      {"Class", "Block* block"},
      {"Function", "std::vector<std::string> parameterVector, Block* block"},
      {"Grouping", "Expr* inner"},
      {"Index", "Expr* array, Expr* index"},
      {"Invocation", "Expr* expr, std::vector<Expr*> exprVector"},
      {"Logical", "Expr* lhs, Token op, Expr* rhs"},
      {"Unary", "Token op, Expr* rhs"},
      {"Value", "Literal* value"},
      {"Variable", "Token name"}
    });
  }
  //Stmt
  else if (!strcmp(argv[1].c_str(), "stmt")) {
    defineAST(os, "Stmt", {
      {"Block", "std::vector<Stmt*> stmtVector"},
      {"Break", ""},
      {"Continue", ""},
      {"Expression", "Expr* expr"},
      {"If", "Expr* condition, Stmt* thenBranch, Stmt* elseBranch"},
      {"Return", "Expr* result"},
      {"Var", "Token name, Expr* initializer"},
      {"While", "Expr* condition, Stmt* branch"}
    });
  }
  //Literal
  else if (!strcmp(argv[1].c_str(), "literal")) {
    defineLiteral(os, "Literal", {
      {"lArray", "std::vector<Literal*> array"},
      {"lBoolean", "bool boolean"},
      {"lClass", "std::map<std::string,Literal*> members"},
      {"lFunction", "std::vector<std::string> parameters/ void* block"},
      {"lNumber", "double number"},
      {"lObject", "std::map<std::string,Literal*> members"},
      {"lReference", "Literal* reference"},
      {"lString", "std::string str"},
      {"lUndefined", ""},
    });
  }

  //no op
  else {
    std::cerr << "NO OP" << std::endl;
    os.close();
    return 2;
  }

  //cleanup
  os.close();
  return 0;
}

//lazy
int main(int argc, char* argv[]) {
  if (argc == 3) {
    return run(argc, {argv[0], argv[1], argv[2]});
  }
  run(3,{argv[0], "expr", "src/expr.hpp"});
  run(3,{argv[0], "stmt", "src/stmt.hpp"});
  run(3,{argv[0], "literal", "src/literal.hpp"});

  return 0;
}
