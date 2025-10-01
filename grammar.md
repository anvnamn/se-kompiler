program → statement+ ;
statement → scope | declaration | assignment | function_call | if_statement | while_statement | return ;
scope → "{" statement* }" ;
declaration → variable_declaration | variable_initialization ;
variable_declaration → datatype identifier ";" ;
variable_initialization → datatype identifier "=" expression ";" ;

assignment → identifier "=" expression ;
return → "returnera" expression ;
function_definition → identifier "(" arguments_definition? ")" scope ;
arguments_definition → datatype expression ("," datatype expression)* ;

function_call → identifier "(" arguments? ")";
arguments → expression ("," expression)* ;

if_statement → "om" "(" expression comparison_operator expression ")" scope ;
while_statement → "medan" "(" expression comparison_operator expression ")" scope ;

expression → literal | binary | IDENTIFIER | function_call;

literal → INTEGER | BOOL_LITERAL | STRING;
BOOL_LITERAL → "sant" | "falskt"
binary → expression binary_operator expression ;
binary_operator → "+" | "-" | "*" | "/" | "%" ;

comparison_operator → "==" | "!=" | ">" | "<" | ">=" | "<=" ;

datatype → "heltal" | "bool" | "sträng" ;