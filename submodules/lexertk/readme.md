#### Description
The C++ Lexer Toolkit Library (LexerTk) is a simple to use, easy to integrate and extremely fast lexicographical generator - lexer. The tokens generated by the lexer can be used as input to a parser such as "ExprTk".


#### Capabilities
+ Operators: +, -, /, *, %, ^, >>, <<, >, <, >=, <=, =, :=
+ Symbols: Format [a..z|A..Z]{a..z|A..Z|0..9|_}
+ Numbers: Integers, Real (scientific notation 1.234e+56)
+ Strings: 'A string 1234 !@#$?'
+ Brackets: (), [], {}
+ Token processors and validators
+ Single header implementation, no building required. No external dependencies.

The LexerTk will decompose an input character stream into a token stream compatible for the following BNF:

```
expression ::= term { +|- term }
term       ::= (symbol | factor) {operator symbol | factor}
factor     ::= symbol | ( '(' {-} expression ')' )
symbol     ::= number | gensymb | string
gensymb    ::= alphabet {alphabet | digit}
string     ::= "'" {alphabet | digit | operator } "'"
operator   ::= * | / | % | ^ | < | > | <= | >= | << | >> | !=
alphabet   ::= a | b | .. | z | A | B | .. | Z
digit      ::= 0 | 1 | 2 | 3 | 4 | 5 | 6 |  7 | 8 | 9
sign       ::= + | -
edef       ::= e | E
decimal    ::= {digit} (digit [.] | [.] digit) {digit}
exponent   ::= edef [sign] digit {digit}
real       ::= [sign] decimal [exponent]
integer    ::= [sign] {digit}
number     ::= real | integer
```

#### Download
http://www.partow.net/programming/lexertk/index.html


#### Compatibility
The C++ Lexer Library implementation is fully compatible with the following C++ compilers:
+ GNU Compiler Collection (4.1+)
+ Intel� C++ Compiler (9.x+)
+ Clang/LLVM (1.1+)
+ PGI C++ (10.x+)
+ Microsoft Visual Studio C++ Compiler (8.1+)
+ IBM XL C/C++ (10.x+)


#### Simple Example 1
The following example will take a string representing an expression,
tokenize it using the generator and then proceed to dump each of the
tokens to stdout, providing information related to the token such as
its type, value and position within the expression.

```c++
int main()
{
   std::string expression = "(1.1 + ( x - y * z / 1.23E+4 ) % w";

   lexertk::generator generator;

   if (!generator.process(expression))
   {
      std::cout << "Failed to lex: " << expression << std::endl;
      return 1;
   }

   lexertk::helper::dump(generator);

   return 0;
}
```

Expected output of the **Simple Example 1** will be:

```
Token[00] @ 000  NUMBER  -->  '1.1'
Token[01] @ 003       +  -->  '+'
Token[02] @ 004       (  -->  '('
Token[03] @ 005  SYMBOL  -->  'x'
Token[04] @ 006       -  -->  '-'
Token[05] @ 007  SYMBOL  -->  'y'
Token[06] @ 008       *  -->  '*'
Token[07] @ 009  SYMBOL  -->  'z'
Token[08] @ 010       /  -->  '/'
Token[09] @ 011  NUMBER  -->  '1.23E+4'
Token[10] @ 018       )  -->  ')'
Token[11] @ 019       %  -->  '%'
Token[12] @ 020  SYMBOL  -->  'w'
```


#### Simple Example 2
The following example will take a string representing an expression, tokenize
it using the generator. The tokens are then analyized in consequtive pairs.
When a pair of tokens are determined to have an implied multiplication between
themselves, a token of type multiplication is inserted between them. For example
the expression '2x+1' becomes '2*x+1'.

```c++
int main()
{
   std::string expression = "1.23 (2.2variable1 / 3.3variable2) 4.56e+12 + variable3";

   lexertk::generator generator;

   if (!generator.process(expression))
   {
      std::cout << "Failed to lex: " << expression << std::endl;
      return 1;
   }

   lexertk::helper::commutative_inserter ci;
   ci.process(generator);

   lexertk::helper::dump(generator);

   return 0;
}
```

Expected output of the **Simple Example 2** will be:

```
Token[00] @ 000  NUMBER  -->  '1.23'
Token[01] @ 005       *  -->  '*'
Token[02] @ 005       (  -->  '('
Token[03] @ 006  NUMBER  -->  '2.2'
Token[04] @ 007       *  -->  '*'
Token[05] @ 007  SYMBOL  -->  'variable1'
Token[06] @ 017       /  -->  '/'
Token[07] @ 019  NUMBER  -->  '3.3'
Token[08] @ 020       *  -->  '*'
Token[09] @ 020  SYMBOL  -->  'variable2'
Token[10] @ 029       )  -->  ')'
Token[11] @ 031       *  -->  '*'
Token[12] @ 031  NUMBER  -->  '4.56e+12'
Token[13] @ 040       +  -->  '+'
Token[14] @ 042  SYMBOL  -->  'variable3'
```

#### Simple Example 3
The following example will take a string representing an expression,
tokenize it using the generator, then attempt to verify that the
ordering of the brackets within the expression is correct.

```c++
int main()
{
   std::string expression = "{aa+(bb-[cc*dd]+ee)-ff}";

   lexertk::generator generator;

   if (!generator.process(expression))
   {
      std::cout << "Failed to lex: " << expression << std::endl;
      return 1;
   }

   lexertk::helper::bracket_checker bc;
   bc.process(generator);

   if (!bc)
   {
      std::cout << "Failed Bracket Checker!" << std::endl;
      return 1;
   }

   lexertk::helper::dump(generator);

   return 0;
}
```

Expected output of the **Simple Example 3** will be:

```
Token[00] @ 000       {  -->  '{'
Token[01] @ 001  SYMBOL  -->  'aa'
Token[02] @ 002       +  -->  '+'
Token[03] @ 003       (  -->  '('
Token[04] @ 004  SYMBOL  -->  'bb'
Token[05] @ 005       -  -->  '-'
Token[06] @ 006       [  -->  '['
Token[07] @ 007  SYMBOL  -->  'cc'
Token[08] @ 008       *  -->  '*'
Token[09] @ 009  SYMBOL  -->  'dd'
Token[10] @ 010       ]  -->  ']'
Token[11] @ 011       +  -->  '+'
Token[12] @ 012  SYMBOL  -->  'ee'
Token[13] @ 013       )  -->  ')'
Token[14] @ 014       -  -->  '-'
Token[15] @ 015  SYMBOL  -->  'ff'
Token[16] @ 016       }  -->  '}'
```
