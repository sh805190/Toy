# Version

This is the definition of Toy version 1. More features may be added at a later date.

# Debugging

For debugging and development purposes, I've added an extra statement, "log". This will print the parameter to the console, regardless of any changes.

```
log "Hello world!";
```

# Reserved keywords

This is a list of keywords that are used in Toy:

* break
* class
* continue
* else
* false
* for
* foreach //reserved for future use
* function
* if
* log
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

```
(   (left parenthesis)
)   (right parenthesis)
[   (left bracket)
]   (right bracket)
{   (left brace)
}   (right brace)
=>  (shortcut function)
..  (range function) 'LHS .. RHS' will only take integers are parameters. It creates a function that returns every integer between LHS and RHS inclusive, and undefined afterwards.
=   (assignment)
==  (is equal to)
!=  (is not equal to)
<   (is less than)
>   (is greater than)
<=  (is less than or equal to)
>=  (is greater than or equal to)
&&  (logical AND)
||  (logical OR)
+   (addition)
-   (subtraction)
*   (multiplication)
/   (division)
%   (modulus)
!   (bang - negate a boolean expression)
-   (negate a number)
++  (prefix increment)
--  (prefix decrement)
++  (postfix increment)
--  (postfix decrement)
.   (access member of)
;   (statement termination)
,   (separator)
"   (surrounds strings)
\   (escape characters in strings)
```

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

A statement is something that produces a side effect, without producing a binding. The second mode of forStmt is intended for use by the range operator, but can be used by any function.

```
statement -> (exprStmt | breakStmt | continueStmt | forStmt | ifStmt | moduleStmt | returnStmt | whileStmt | block);

exprStmt     -> expression ";";
breakStmt    -> "break" ";";
continueStmt -> "continue" ";";
forStmt      -> "for" "(" (varDecl | exprStmt)? ";" expression? ";" expression? ")" statement | "for" "(" "var" IDENTIFIER "=" function ")" statement;
ifStmt       -> "if" "(" expression ")" statement ("else" statement)?;
moduleStmt   -> "module" ";";
returnStmt   -> "return" expression? ";";
whileStmt    -> "while" "(" expression ")" statement;
logStmt      -> "log" string ";";
block        -> "{" declaration* "}";
```

An expression evaluates to a value. Due to the number of unary and binary operators with various precedence levels, these rules have been divided up to make them explicit.

```
expression -> assignment;

assignment     -> (call ".")? IDENTIFIER "=" assignment | logical_or;

logical_or     -> logical_and ("||" logical_and)*;
logical_and    -> equality ("&&" equality)*;
equality       -> comparison ( ("==" | "!=") comparison)*;
comparison     -> addition ( ("<" | "<=" | ">" | ">=") addition)*;
addition       -> multiplication ( ("+" | "-") multiplication)*;
multiplication -> unary ( ("*" | "/") unary)*;

unary          -> ("!" | "-") unary | call;
call           -> primary ( "(" arguments? ")" | "." IDENTIFIER)*;
primary        -> "true" | "false" | "undefined" | "this" | NUMBER | STRING | IDENTIFIER | "(" expression ")" | "[" (expression ("," expression)*)? "]" | function | class;
```

These have been separated out for clarity. Please note that "recurse" is only valid within a function, and will otherwise raise an error. Also, shortcut mode 3 and 4 will return the value of the expression.

```
function   -> "function" "(" parameters? ")" block | shortcut | recurse;
shortcut   -> "(" parameters? ")" "=>" block | parameters? "=>" block | "(" parameters? ")" "=>" expression | parameters? "=>" expression;
class      -> "class" "{" varDecl* "}";
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

# Standard Library

The following variables are introdcued into the namespace when calling 'use "standard";'. These act as the standard library.

```
print(str) //print to the console
printf(str, array) //print formatted to the console
scan() //returns input from the console
scanf(str, array) //returns formatted input from the console as an array
random(i) //produces a random number between 0 inclusive and 1 exclusive, and multiplies it by i
copy(i) //returns a deep copy of i, regardless of what it is
floor(i) //returns i rounded to the highest integer below i
ceiling(i) //returns i rounded to the lowest integer above i
round(i) //returns i rounded to the nearest integer
map(array, func) //returns a copy of array with func applied to each element
clock() //number of seconds as a fraction since the program began
```

# Numbers

All numbers are double precision floating point values. They have the following built in functions.

```
x.floor() //rounds to the highest integer below x
x.ceiling() //rounds to the lowest integer above x
x.round() //rounds to the nearest integer
```

# Arrays

Arrays by default use incremental indexes, but any value can act as a key; this makes arrays associative. Arrays have the following built in functions.

```
copy() //return a deep copy of this array
map(func) //apply func to each element, storing the result in its place
reduce(func) //"fold" the elements of an array, returning the result
filter(func) //return an array that contains copies of all elements that makes func return true
size() //return the size of the array
push_front(e) //append e to the front, indexed arrays only
push_back(e) //append e to the back, indexed arrays only
pop_front() //remove the first element and return it, indexed arrays only
pop_back() //remove the last element and return it, indexed arrays only
insert(index, value) //insert value at index, if array is indexed, other indexes are adjusted to make room
delete(index) //delete value at index, if array is indexed, other indexes are adjusted to fill room
```

# Functions

A function is a block of code that can be reused. It is defined using the 'function' keyword or the shortcut syntax ( the '=>' operator), and acts as a first class object. If the last line of a shortcut function is an expression statement, then the result of that expression is returned. An incorrect number of arguments is an error.

```
var a = function(x) {
	;
}

var b = (x) => {
	;
}

var c = x => {
	;
}

x => x;

a(i);
b(i);
c(i);
```

# Classes

A class is a series of variables bundled together, accessible with the '.' operator. All members of a class are avialable to all other members of that class i.e. functions. If a class has "constructor" as a function, it is called on object initialization; the same for "destructor" and cleanup. To instanciate a class as an object, it must be invoked just like a function. The object itself is accessible using the 'this' keyword as a variable. Classes are immutable after creation.

This language explicitly disallows inheritance.

```
var Dog = class {
	var constructor() {
		tail.wag();
	}

	var tail = class {
		var wag = function() {
			;
		}
	}
}

var rover = Dog();
```

# Oddball Language

This works:

```
var f = () => wag();

var Cat = class {
	var constructor = f;
	var wag = () => {};
}

var Dog = class {
	var constructor = f;
	var wag = () => {};
}
```

