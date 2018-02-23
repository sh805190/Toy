# Version

This is the definition of Toy version 1. More features may be added at a later date.

# Reserved keywords

This is a list of keywords that are used in Toy:

* break
* class
* continue
* else
* false
* for
* function
* if
* module
* recurse
* return
* this
* true
* undefined
* use
* while

# Operators

This is a list of operators (single or double symbols) that are used in toy:

* ( (left parenthesis)
* ) (right parenthesis)
* [ (left bracket)
* ] (right bracket)
* { (left brace)
* } (right brace)
* = (assignment)
* == (is equal to)
* != (is not equal to)
* < (is less than)
* > (is greater than)
* <= (is less than or equal to)
* >= (is greater than or equal to)
* + (addition)
* - (subtraction)
* * (multiplication)
* / (division)
* % (modulus)
* ! (bang - negate a boolean expression)
* - (negate a number)
* ++ (prefix increment)
* -- (prefix decrement)
* ++ (postfix increment)
* -- (postfix decrement)
* . (access member of)
* ; (statement termination)
* , (separator)
* " (surrounds strings)
* ' (surrounds single characters)

# Syntax

The syntax of Toy borrows heavily from JavaScript. A program is a series of declarations, followed by an end of file indicatior.

```
program -> declaration* EOF;
```

A declaration either defines something (such as a use statement or a new variable), or is a statement.

```
declaration -> (useDecl | varDecl | statement);

useDecl -> "use" (panic | strict | version NUMBER | STRING);
varDecl -> "var" IDENTIFIER ("=" expression)? ";";
```

A statement is something that produces a side effect, without producing a binding.

```
statement -> (exprStmt | breakStmt | continueStmt | forStmt | ifStmt | moduleStmt | returnStmt | whileStmt | block);

exprStmt     -> expression ";";
breakStmt    -> "break" ";";
continueStmt -> "continue" ";";
forStmt      -> "for" "(" (varDecl | exprStmt)? ";" expression? ";" expression? ")" statement;
ifStmt       -> "if" "(" expression ")" statement ("else" statement)?;
moduleStmt   -> "module" ";";
returnStmt   -> "return" expression? ";";
whileStmt    -> "while" "(" expression ")" statement;
block        -> "{" declaration* "}";
```

An expression evaluates to a value. Due to the number of unary and binary operators with various precedence levels, these rules have been divided up to make them explicit.

```
expression -> assignment;

assignment     -> (call ".")? IDENTIFIER "=" assignment | logical_or | function | class;

logical_or     -> logical_and ("or" logical_and)*;
logical_and    -> equality ("and" equality)*;
equality       -> comparison ( ("==" | "!=") comparison)*;
comparison     -> addition ( ("<" | "<=" | ">" | ">=") addition)*;
addition       -> multiplication ( ("+" | "-") multiplication)*;
multiplication -> unary ( ("*" | "/") unary)*;

unary          -> ("!" | "-") unary | call;
call           -> primary ( "(" arguments? ")" | "." IDENTIFIER)*;
primary        -> "true" | "false" | "undefined" | "this" | NUMBER | STRING | IDENTIFIER | "(" expression ")";
```

These have been separated out for clarity. Please note that "recurse" is only valid within a function, and will otherwise raise an error.

```
function   -> "function" "(" parameters? ")" block | recurse;
class      -> "class" "{" varDecl "}";
recurse    -> "recurse" "("  arguments? ")" ("." call)?;
parameters -> IDENTIFIER ("," IDENTIFIER)*;
arguments  -> expression ("," expression)*;
```

Finally, here is the lexical grammar.

```
NUMBER -> DIGIT+ ("." DIGIT+)?;
STRING -> '"' <any character except '"'>* '"';
IDENTIFIER -> ALPHA (ALPHA | DIGIT)*;
ALPHA -> 'a' ... 'z' | 'A' ... 'Z' | '_';
DIGIT -> '0' ... '9';
```

