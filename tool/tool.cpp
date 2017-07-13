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

  os << "#include <list>" << std::endl;
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
  os << "  virtual void Accept(" << baseName << "Visitor* visitor) {" << std::endl;
  os << "    visitor->Visit(this);" << std::endl;
  os << "  }" << std::endl;
  os << "};" << std::endl;

  //define each AST class
  for(auto& p : data) {
    //print the opening
    os << std::endl;
    os << "class " << p.first << ": public " << baseName << " {" << std::endl;
    os << "public:" << std::endl;
    os << "  " << p.first << "(" << p.second << ") {" << std::endl;

    //set the member values
    std::vector<std::string> members = splitString(p.second, " ,");
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

int main(int argc, char* argv[]) {
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
  if (!strcmp(argv[1], "expr")) {
    defineAST(os, "Expr", {
//      {"Array", "Token lb, std::list<Literal> literalList, Token rb"},
      {"Assign", "Token name, Expr* value"}, 
      {"Binary", "Expr* lhs, Token op, Expr* rhs"},
      {"Grouping", "Expr* inner"},
      {"Logical", "Expr* lhs, Token op, Expr* rhs"},
      {"Unary", "Token op, Expr* rhs"},
      {"Value", "Literal value"},
      {"Variable", "Token name"}
    });
  }
  //Stmt
  else if (!strcmp(argv[1], "stmt")) {
    defineAST(os, "Stmt", {
      {"Block", "std::list<Stmt*> stmtList"},
      {"Break", "int line"},
      {"Continue", "int line"},
      {"Expression", "Expr* expr"},
      {"If", "Expr* condition, Stmt* thenBranch, Stmt* elseBranch"},
      {"Var", "Token name, Expr* initializer"},
      {"While", "Expr* condition, Stmt* branch"}
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

