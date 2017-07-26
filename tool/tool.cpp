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

std::string unescapeString(std::string str) {
  std::string ret;

  for (int i = 0; i < str.size(); i++) {
    if (str[i] == '\\') {
      if (str[i+1] == '\\') { ret += '\\'; i++; }
      else if (str[i+1] == '"') {ret += '"'; i++;}
      else if (str[i+1] == 'b') {ret += '\b'; i++;}
      else if (str[i+1] == 'n') {ret += '\n'; i++;}
      else if (str[i+1] == 'r') {ret += '\r'; i++;}
      else if (str[i+1] == 't') {ret += '\t'; i++;}
    }
    else {
      ret += str[i];
    }
  }
  return ret;
}

void defineAST(std::ofstream& header, std::ofstream& source, std::string headerName, std::string baseName, std::vector<std::vector<std::string>> data) {
  //print the basics
  header << "#pragma once" << std::endl;
  header << std::endl;

  source << "#include \"" << headerName.substr(headerName.find_first_of('/')+1, std::string::npos) << "\"" << std::endl << std::endl;

  source << "int " << baseName << "::count = 0;" << std::endl << std::endl;

  //forward declare the AST classes
  header << "class " << baseName << ";" << std::endl;
  for (auto& p : data) {
    header << "class " << p[0] << ";" << std::endl;
  }

  header << std::endl;
  source << "#include \"garbage_collector.hpp\"" << std::endl;
  header << "#include \"expr.hpp\"" << std::endl;
  source << "#include \"literal.hpp\"" << std::endl;
  header << "#include \"stmt.hpp\"" << std::endl;
  header << "#include \"token.hpp\"" << std::endl;
  header << std::endl;
  source << std::endl;

  header << "#include <string>" << std::endl;
  header << "#include <vector>" << std::endl;
  header << std::endl;

  //print the visitor class
  header << "class " << baseName << "Visitor {" << std::endl;
  header << "public:" << std::endl;
  header << "  virtual void Visit(" << baseName << "*) = 0;" << std::endl;
  for (auto& p : data) {
    header << "  virtual void Visit(" << p[0] << "*) = 0;" << std::endl;
  }
  header << "};" << std::endl << std::endl;

  //print the base class
  header << "class " << baseName << " {" << std::endl;
  header << "public:" << std::endl;

  header << "  " << baseName << "();" << std::endl;
  source << baseName << "::" << baseName << "() {" << std::endl;
  source << "  count++;" << std::endl;
  source << "  GarbageCollector<" << baseName << ">::Push(this);" << std::endl;
  source << "}" << std::endl << std::endl;

  header << "  " << baseName << "(int);" << std::endl;
  source << baseName << "::" << baseName << "(int ln) {" << std::endl;
  source << "  line = ln;" << std::endl;
  source << "  count++;" << std::endl;
  source << "  GarbageCollector<" << baseName << ">::Push(this);" << std::endl;
  source << "}" << std::endl << std::endl;

  header << "  virtual ~" << baseName << "();" << std::endl;
  source << baseName << "::~" << baseName << "() {" << std::endl;
  source << "  count--;" << std::endl;
  source << "  GarbageCollector<" << baseName << ">::Pop(this);" << std::endl;
  source << "}" << std::endl << std::endl;

  header << "  virtual void Accept(" << baseName << "Visitor* visitor);" << std::endl;
  source << "void " << baseName << "::" << "Accept(" << baseName << "Visitor* visitor) {" << std::endl;
  source << "  visitor->Visit(this);" << std::endl;
  source << "}" << std::endl;

  header << std::endl;
  header << "  int line = -1;" << std::endl;
  header << "  static int count;" << std::endl;
  header << "};" << std::endl;

  //define each AST class
  for(auto& p : data) {
    //print the opening
    header << std::endl;
    header << "class " << p[0] << ": public " << baseName << " {" << std::endl;
    header << "public:" << std::endl;

    header << "  " << p[0] << "(int ln";
    source << p[0] << "::" << p[0] << "(int ln";
    
    if (p[1].size()) { header << ", "; source << ", "; };
    header << p[1] << ");" << std::endl;
    source << p[1] << ") {" << std::endl;

    //set the member values
    std::vector<std::string> members = splitString(p[1], " ,");
    source << "    this->line = ln;" << std::endl;
    for (int i = 0; i < members.size(); i += 2) { //+2, since the type declaration is mixed in
      source << "    this->" << members[i+1] << " = " << members[i+1] << ";" << std::endl;
    }
    source << "  }" << std::endl << std::endl;

    //create the destructor
    header << "  virtual ~" << p[0] << "();" << std::endl;
    source << p[0] << "::~" << p[0] << "() {" << std::endl;
    source << "}" << std::endl << std::endl;

    //print the second part
    header << "  void Accept(" << baseName << "Visitor* visitor) override;" << std::endl;
    source << "void " << p[0] << "::Accept(" << baseName << "Visitor* visitor) {" << std::endl;
    source << "  visitor->Visit(this);" << std::endl;
    source << "}" << std::endl << std::endl;

    //print the members
    members = splitString(p[1], ",");
    if (members.size()) {
      header << std::endl;
      header << " ";
    }
    for (std::string m : members) {
      header << " " << m << ";" << std::endl;
    }

    //close the class
    header << "};" << std::endl;
  }
}

void defineLiteral(std::ofstream& header, std::ofstream& source, std::string headerName, std::string baseName, std::vector<std::vector<std::string>> data) {
  //print the basics
  header << "#pragma once" << std::endl;
  header << std::endl;

  source << "#include \"" << headerName.substr(headerName.find_first_of('/')+1, std::string::npos) << "\"" << std::endl << std::endl;

source << "int " << baseName << "::count = 0;" << std::endl << std::endl;

  //forward declare the AST classes
  header << "class " << baseName << ";" << std::endl;
  for (auto& p : data) {
    header << "class " << p[0] << ";" << std::endl;
  }

  header << std::endl;
  source << "#include \"garbage_collector.hpp\"" << std::endl;
  source << "#include \"ast_deleter.hpp\"" << std::endl;
  source << "#include \"ast_duplicator.hpp\"" << std::endl;
  header << "#include \"expr.hpp\"" << std::endl;
  source << "#include \"literal.hpp\"" << std::endl;
  header << "#include \"stmt.hpp\"" << std::endl;
  header << "#include \"token.hpp\"" << std::endl;
  header << std::endl;
  source << std::endl;

  header << "#include <map>" << std::endl;
  header << "#include <string>" << std::endl;
  header << "#include <vector>" << std::endl;
  header << std::endl;

  //delcare the base class
  header << "class " << baseName << " {" << std::endl;
  header << "public:" << std::endl;
  header << "  enum class Type {" << std:: endl;
  header << "    ";
  for (auto& p : data) {
    header << toUpper(p[0]) << ", ";
  }
  header << std::endl;
  header << "  };" << std::endl;
  header << std::endl;

  header << "  " << baseName << "();" << std::endl;
  source << baseName << "::" << baseName << "() {" << std::endl;
  source << "  count++;" << std::endl;
  source << "  GarbageCollector<Literal>::Push(this);" << std::endl;
  source << "}" << std::endl << std::endl;

  header << "  virtual ~" << baseName << "();" << std::endl;
  source << baseName << "::~" << baseName << "() {" << std::endl;
  source << "  count--;" << std::endl;
  source << "  GarbageCollector<Literal>::Pop(this);" << std::endl;
  source << "}" << std::endl << std::endl;

  header << "  virtual Literal* Copy();" << std::endl;
  source << "Literal* " << baseName << "::Copy() {" << std::endl;
  source << "  Literal* l = new Literal();" << std::endl;
  source << "  l->type = type;" << std::endl;
  source << "  return l;" << std::endl;
  source << "}" << std::endl << std::endl;

  header << "  virtual std::string ToString();" << std::endl;
  source << "std::string " << baseName << "::ToString() {" << std::endl;
  source << "  return \"LITERAL\";" << std::endl;
  source << "}" << std::endl << std::endl;

  header << std::endl;

  header << "  Type type = Type::LUNDEFINED;" << std::endl;
  header << "  //debugging" << std::endl;
  header << "  static int count;" << std::endl;
  header << "};" << std::endl << std::endl;

  //declare each child class
  for (auto& p : data) {
    //parsing
    std::vector<std::string> memberDeclarations = splitString(p[1], "/");
    std::vector<std::string> memberNames = splitString(p[1], " /");

    header << "class " << p[0] << ": public " << baseName << " {" << std::endl;
    header << "public:" << std::endl;

    //default constructor
    header << "  " << p[0] << "();" << std::endl;
    source << p[0] << "::" << p[0] << "() {" << std::endl;
    source << "  type = Type::" << toUpper(p[0]) << ";" << std::endl;
    source << "}" << std::endl << std::endl;

    if (memberDeclarations.size()) {
      //parameter constructor
      header << "  " << p[0] << "(";
      source << p[0] << "::" << p[0] << "(";

      for (int i = 0; i < memberDeclarations.size(); ) {
        header << memberDeclarations[i];
        source << memberDeclarations[i];
        i++;
        if (i < memberDeclarations.size()) {
          header << ",";
          source << ",";
        }
      }

      header << ");" << std::endl;
      source << ") {" << std::endl;
      source << "  type = Type::" << toUpper(p[0]) << ";" << std::endl;
      for (int i = 0; i < memberNames.size(); i += 2) {
        source << "  this->" << memberNames[i+1] << " = std::move(" << memberNames[i+1] << ");" << std::endl;
      }
      source << "}" << std::endl << std::endl;
    }

    //destructor
    header << "  virtual ~" << p[0] << "();" << std::endl;
    source << p[0] << "::~" << p[0] << "() {" << std::endl;
    source << "  " << p[2] << ";" << std::endl; //external specification
    source << "}" << std::endl << std::endl;

    //define the methods
    header << "  Literal* Copy() override;" << std::endl;
    source << "Literal* " << p[0] << "::Copy() {" << std::endl;

    source << "  " << p[3] << ";" << std::endl; //external specification

    source << "}" << std::endl;
    source << std::endl;

    header << "  std::string ToString() override;" << std::endl;
    source << "std::string " << p[0] << "::ToString() {" << std::endl;
    source << "  " << p[4] << ";" << std::endl;
    source << "}" << std::endl << std::endl;

    //define the members
    if (memberDeclarations.size()) {
      header << std::endl;
    }

    for (std::string m : memberDeclarations) {
      header << "  " << m << ";" << std::endl;
    }

    //finish
    header << "};" << std::endl << std::endl;
  }
}

int run(int argc, std::string mode, std::string headerName, std::string sourceName) {
  //open the specified files for writing
  std::ofstream hf(headerName);
  std::ofstream sf(sourceName);

  if (!hf.is_open()) {
    std::cerr << "Failed to open " << headerName << " for writing" << std::endl;
    sf.close();
    return 1;
  }

  if (!sf.is_open()) {
    std::cerr << "Failed to open " << sourceName << " for writing" << std::endl;
    hf.close();
    return 1;
  }

  //Expr
  if (mode == std::string("expr")) {
    defineAST(hf, sf, headerName, "Expr", {
      {
        "Array",
        "std::vector<Expr*> exprVector"
      },
      {
        "Assign",
        "Expr* target, Expr* value"
      },
      {
        "Binary",
        "Expr* lhs, Token op, Expr* rhs"
      },
      {
        "Class",
        "Block* block"
      },
      {
        "Function",
        "std::vector<std::string> parameterVector, Block* block"
      },
      {
        "Grouping",
        "Expr* inner"
      },
      {
        "Index",
        "Expr* array, Expr* index"
      },
      {
        "Invocation",
        "Expr* expr, std::vector<Expr*> exprVector"
      },
      {
        "Logical",
        "Expr* lhs, Token op, Expr* rhs"
      },
      {
        "Unary",
        "Token op, Expr* rhs"
      },
      {
        "Value",
        "Literal* value"
      },
      {
        "Variable",
        "Token name"
      }
    });
  }
  //Stmt
  else if (mode == std::string("stmt")) {
    defineAST(hf, sf, headerName, "Stmt", {
      {
        "Block",
        "std::vector<Stmt*> stmtVector"
      },
      {
        "Break",
        ""
      },
      {
        "Continue",
        ""
      },
      {
        "Expression",
        "Expr* expr"
      },
      {
        "If",
        "Expr* condition, Stmt* thenBranch, Stmt* elseBranch"
      },
      {
        "Return",
        "Expr* result"
      },
      {
        "Use",
        "Token command"
      },
      {
        "Var",
        "Token name, Expr* initializer"
      },
      {
        "While",
        "Expr* condition, Stmt* branch"
      }
    });
  }
  //Literal
  else if (mode == std::string("literal")) {
    defineLiteral(hf, sf, headerName, "Literal", {
      {
        "lArray",
        "std::vector<Literal*> array",
        "for (Literal* ptr : array) delete ptr",
        "lArray* ret = new lArray(); for (Literal* ptr : array) ret->array.push_back(ptr->Copy()); return ret",
        "std::string output = \"[\";for (Literal* ptr : array){output += ptr->ToString();output += \",\";}return output + \"]\""
      },
      {
        "lBoolean",
        "bool boolean",
        "",
        "return new lBoolean(boolean)",
        "return boolean ? \"true\" : \"false\""
      },
      {
        "lClass",
        "std::map<std::string,Literal*> members",
        "for (auto p : members) delete p.second",
        "lClass* ret = new lClass(); for (auto p : members) ret->members[p.first] = p.second->Copy(); return ret",
        "std::string output = \"class {\";for (auto& it : members) {output += it.first + \":\";output += it.second->ToString();output += \",\";}return output + \"}\"" },
      {
        "lFunction",
        "std::vector<std::string> parameters/ void* block",
        "ASTDeleter deleter; deleter.DeleteAST(static_cast<Block*>(block))",
        "ASTDuplicator dup; lFunction* ret = new lFunction(); ret->parameters = parameters; ret->block = static_cast<void*>(dup.DuplicateAST(static_cast<Block*>(block))); return ret",
        "std::string output = \"function(\";for (std::string s : parameters) {output += s;output += \",\";}return output + \") {...}\""
      },
      {
        "lNumber",
        "double number",
        "",
        "return new lNumber(number)",
        "std::string s = std::to_string(number); s = s.substr(0, s.find_last_not_of('0') + 1); if (s[s.size()-1] == '.') s = s.substr(0, s.size()-1); return s"
      },
      {
        "lObject",
        "std::map<std::string,Literal*> members",
        "for (auto p : members) delete p.second",
        "lObject* ret = new lObject(); for (auto p : members) ret->members[p.first] = p.second->Copy(); return ret",
        "std::string output = \"object {\"; for (auto& it : members) {output += it.first; output += \",\";}return output + \"}\""
      },
      {
        "lReference",
        "Literal* reference",
        "",
        "return new lReference(reference)",
        "return std::string() + \"&\" + reference->ToString()"
      },
      {
        "lString",
        "std::string str",
        "",
        "return new lString(str)",
        "return str"
      },
      {
        "lUndefined",
        "",
        "",
        "return new lUndefined()",
        "return \"undefined\""
      },
    });
  }

  //no op
  else {
    std::cerr << "NO OP: " << mode << std::endl;
    hf.close();
    sf.close();
    return 2;
  }

  //cleanup
  hf.close();
  sf.close();
  return 0;
}

//lazy
int main(int argc, char* argv[]) {
  run(3, "expr", "src/expr.hpp", "src/expr.cpp");
  run(3, "stmt", "src/stmt.hpp", "src/stmt.cpp");
  run(3, "literal", "src/literal.hpp", "src/literal.cpp");

  return 0;
}
