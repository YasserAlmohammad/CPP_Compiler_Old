%{
#include <stdio.h>
#include <iostream.h>
#include "malloc.h"
/*************** Includes and Defines *****************************/
#define YYDEBUG 1        /* get the pretty debugging code to compile*/
#define YYERROR_VERBOSE 1
#define YYSTYPE  char *  /* interface with flex: should be in header file */
extern "C" FILE* yyin;
extern "C" int yylex();
extern "C" int yyparse();
extern "C" int yyerror(char*);
%}

/*************** Token Definitions *****************************/                               
%token CHAR           EXTERN			BOOL                 
%token CONST          DEFINE            BREAK
%token UNDEF	      FOR             TYPEDEF_ID          VOID
%token DOUBLE         INT            INCLUDE          RETURN
%token DO             STATIC          WHILE
%token STRING_CONST	  INTEGER_CONST	DOUBLE_CONST	CHARACTER_CONST							


/* object oriented tokens */
%token NEW             DELETE
%token THIS
%token OPERATOR
%token CLASS
%token PUBLIC          PROTECTED       PRIVATE
%token VIRTUAL         FRIEND			INLINE
         
/* const tokens */
%token ID_NAME              STRING_CONST
%token DOUBLE_CONST        INTEGER_CONST        CHARACTER_CONST

/* New Lexical element, whereas ANSI C suggested non-terminal */
%token TYPEDEF_ID


/* Operator precedence */
%left  LE GE EQ NE '=' '<' '>'     /*    <=      >=      ==      !=      */
%left  '+'	'-'	 ANDAND OROR      /*    &&      ||    */
%left	'*'		'/'
%left	'&'		'!'		'('		')'		ICR DECR /* ++   --   */	POS_NEG
%left	LAST_PREC

 
%token  ARROW     SCOPE_DOTS       /*    ->       ::    */
			

%left	IF
%left	ELSE



%%
/* at first trial we got 170 shidt-reduce conflicts and 23 reduce-reduce conflict

*/

program :
	prog_decls   { cout<<"program\n"; }
	;
	
prog_decls :
	/* empty */
	|	prog_decls declaration	{ cout<<"decl\n"; }
	|	prog_decls error	{ cout<<"decl-error\n"; }
	;

declaration:
	preprocessor	{ cout<<"preprecessor\n"; }
	|class_def		{ cout<<"class_def\n"; }
	|func_decl ';'		{ cout<<"func_decl\n"; }
	|func_def		{ cout<<"func_def\n"; }
	|func_def_scoped { cout<<"func_def_scoped\n"; }
	|var_decl ';'	{ cout<<"var_decl\n"; }	
	|var_def ';'
	|class_static_init_var
	;

/*
	preprocessors must be dealt with during lex, how ever we include them
	here just as temporary action
	class_def:	 class A {...} 
	func_decl:   int getX(int,int);
	func_def:	 int getX(int x1,int x2){...}
	var_decl:	class A;	int x;
	var_def	:	int x=0;
	class_static_init_var:		int A::x=0;
*/


preprocessor:
	include_decl	{ cout<<"include_pre\n"; }
	|define_decl	{ cout<<"define_pre\n"; }
	;
include_decl:
	'#' INCLUDE included_file 
	;
included_file:
	 '<' ID_NAME '.' ID_NAME '>'  { cout<<"include_name<>\n"; }
	| STRING_CONST					{ cout<<"include_name\n"; }
	;
define_decl:
	'#' DEFINE ID_NAME expr

class_def:
	 class_head class_body	{ cout<<"class_def_no_base\n"; }
	|class_head ':' access_spec    class_base_list class_body	{ cout<<"class_def_base\n"; }
	;

class_head:
		CLASS ID_NAME	{ cout<<"class-head\n"; }
		;

access_spec:
	PUBLIC			
	|PROTECTED		
	|PRIVATE
	|FRIEND		
	;

class_base_list:
	ID_NAME
	|class_base_list ',' ID_NAME
	;

class_body:
	'{' class_body_stmts '}' ';'
	;


class_body_stmts:
	access_spec ':'
	|func_decl ';'
	|func_def ';'  /* consided as inline by default if INLINE keyword not mentioned*/
	|var_decl ';'
	;	
/*
update func_def to include the case where const data needs initilaiation
in the construtor X::X()f(0){ }   where--> const int f;
*/

func_decl:
	storage_spec type ID_NAME func_decl_args
	;

storage_spec: /*nothing */
	|STATIC
	|CONST
	;

func_decl_args: '(' ')' /*empty or argument*/
	|'(' func_decl_arg_list ')'
	;
/* the '(' ')' is moved here inside	to eliminate 
	a reduce-reduce conflict caused by func_decl_args which has 
	initially the same tokens, and since bison uses one look ahead
	token so we can't permit the ')' to be that token when reduction 
	is done, now the body_stmts is the look a head (non terminal) and ';'
	is the only look ahead token for func_decl after the ')'
	so the right reduction is chosen
*/

	
func_decl_arg_list: /*one or more elements */
	func_decl_arg_elem
	|func_decl_arg_list ',' func_decl_arg_elem
	;
	
func_decl_arg_elem:
	type
	|type ID_NAME
	|type ID_NAME '=' const_expr
	;
/*  void f(), void f(int,int), void f(int x,int), void f(int x=0,int) */
/* type is defined later: void, int, double, a class name, array... */
/* const_expr is defined later: 0, "ddd", assuming only const values are permitted...*/


/*
update func_def to include the case where const data needs initilaiation
in the construtor X::X()f(0){ }   where--> const int f;
*/

/* the only difference between class functions and none class functions
	is that we can't use this definition inside a class:
		int A::f(...){ ... }
*/

func_def:	
	func_def_inline
	|func_def_normal
	;
func_def_inline:
	storage_spec INLINE func_def_common
	;

	func_def_normal:
	storage_spec func_def_common
	;

func_def_common:		
		type ID_NAME '(' func_def_args ')' func_body
		;

/* two main types of fucntions appear, ones with a class scope like A::f()
and ones with no scope
scoped functions appear only outside classes, wheras normal functions appear 
any where
*/

func_def_scoped:
	func_def_scoped_inline
	|func_def_scoped_normal	
	;

func_def_scoped_inline:
	storage_spec INLINE func_def_scoped_common
	;

func_def_scoped_normal:
	storage_spec func_def_scoped_common
	;

func_def_scoped_common:		
		type class_type SCOPE_DOTS ID_NAME '(' func_def_args ')' func_body
		;
/* void f(...){ }		void A::f(...){} */		


func_def_args: '(' ')'
	|'(' func_def_arg_list ')'
	;
	
	
func_def_arg_list: /*one or more elements */
	func_def_arg_elem
	|func_def_arg_list ',' func_def_arg_elem
	;
	
func_def_arg_elem:
	type ID_NAME
	|type ID_NAME '=' var_expr
	;
/* type alone is not permitted */

func_body:
	'{' stmts '}'
	;
	
stmts:
	stmt
	|stmts stmt
	;
/* one or more statements */	

	
stmt:
	simple_stmt ';'
	|compound_stmt
	;	
/*simple contains one statement
	compound: may contain multiple statements like loops-if-blocks
*/


simple_stmt:
	var_decl
	|var_def
	|assignment
	|proc_call
	;

var_decl:
	storage_spec type ID_NAME
	|storage_spec type ID_NAME array_dims
	;
/* int x;*/
/* array decleration can be added here since it's decleration is different:
	int x [] []; here variable comes between array def and referenced type
	whereas types usually comes before variable
*/


array_dims: 
	array_dim
	| array_dims array_dim
	;

array_dim:
	'[' ']'
	|'[' const_expr ']'
	;
/* int[][][22] x; */
	


var_def: 
	var_decl '=' expr
	;
/* int x=0;		int x=y*2;	 */

class_static_init_var:
	class_type SCOPE_DOTS ID_NAME '=' expr
	;	
/*int A::x=0; in static member initialization, it's a unique variable initialiation rule
	only happens in a global scope
	putting the rule in the following form causes Shift-reduce conflict:
		type class_type SCOPE_DOTS ID_NAME '=' expr	%prec LAST_PREC	
	how ever removing (type) only deletes this conflict
	and i have no idea why it happens
 */	


assignment: 
	var_expr '=' expr
	;
/*  x=0;		a.x=0;		A::x=0 */


proc_call: 'n'
	ID_NAME '(' proc_expr_list ')'
	|ID_NAME '.' ID_NAME '(' proc_expr_list ')'
	|class_type SCOPE_DOTS ID_NAME '(' proc_expr_list ')'
	; 
/*	f(4);	a.f(...)  A::f(...)*/

proc_expr_list:	/*empty list */
	|proc_expr_list_not_empty
	;
proc_expr_list_not_empty:
	expr
	|proc_expr_list_not_empty ',' expr
	;
	
compound_stmt:
	if_stmt
	|while_stmt
	|block_stmt
	;
/* block stmt like this  {stmts} */
block_stmt:
	'{' stmts '}'
	;
	
if_stmt:
	IF '(' expr ')'  stmt	/* %prec IF */
	|IF '(' expr ')'  stmt ELSE stmt	/* %prec ELSE */
	;
/* no worry about SHIFT_REDUCE conflicts here, because bison always shifts first
and that make the right rule choosen, because bison uses look ahead char 
so even without precedence-associativity it's ok*/

while_stmt:
	WHILE '(' expr ')' stmt
	; 


type:
	simple_type
	|complex_type
	; 

simple_type:
	INT
	|DOUBLE
	|BOOL
	|CHAR
	|VOID
	;


complex_type: 
	pointer_type
	|reference_type
/*	|class_type */
	;
	
/* array type is not added here, since it's definition is different from others
	it's added as part of var_decl
	besides, in type it gave me 1 shift-reduce conflict which we couldn't resolve
	so the preper changes were made and conflict was resolved upon the rules change
	*/	

pointer_type:
	type '*'
	;
reference_type:
	type '&'
	;

expr:
	const_expr
	|var_expr
	|unary_expr
	|binary_expr
	|allocate_expr
	|proc_call
	|cast_expr
	|'(' expr ')'
	;


/* expressions: const values, unary and binary ops... 
check, some definitions can accept empty expressions as well
*/


const_expr:
	INTEGER_CONST
	|DOUBLE_CONST
	|CHARACTER_CONST
	|STRING_CONST	
	;

	
var_expr: 
	array_expr
	|pointer_val_expr
	|ID_NAME
	|ID_NAME '.' ID_NAME
	|class_type SCOPE_DOTS ID_NAME   /* is to become a CLASS_TYPE_NAME */
	
/* those that u can store values in */

	
array_expr:
	ID_NAME array_expr_dims
	;
array_expr_dims:
	'[' expr ']'
	|array_expr_dims '[' expr ']'
	;

pointer_val_expr:
	'*' var_expr	
	;
	
	
unary_expr:
	 '!' expr
	| '-' expr		%prec POS_NEG
	| ICR expr
	| DECR expr	
	| '&' var_expr
	| '+' expr		%prec POS_NEG
	;


binary_expr:
	expr '+' expr
	|expr '-' expr
	|expr '/' expr
	|expr '*' expr
	|expr '<' expr
	|expr '>' expr
	|expr LE expr
	|expr GE expr
	|expr EQ expr
	|expr NE expr
	|expr OROR expr
	|expr ANDAND expr
	;
allocate_expr:
	new_expr
	|delete_expr
	;

new_expr:
	NEW type '(' expr ')'
	|NEW type '[' expr ']'
	;

delete_expr:
	DELETE var_expr
	|DELETE '[' ']' var_expr
	;
cast_expr:
	'(' type ')' var_expr
	;
	
/***********************	NOTES	********************
	to make it robuts change the lex to return an include file name

	preprocessors must be dealt with during lex, how ever we include them
	

	update func_def to include the case where const data needs initilaiation
	in the construtor X::X()f(0){ }   where--> const int f;
		
	update assigment rules to accomidate discrimination between the following cases
	x=0;		a.x=0;		A::x=0 

	multi dimensional array, assigining values in { , , ,... } for an array variable
	array definition is like this (int x[2][][] ) not like (int [][][] x;) this changes it's 
	rules
	
	class type: each defined class is considerd a type it self, this is why any Identifier 
		is considered as a possible type
		so discrimination between class types and identifiers can be checked and done in
		lex
		
	friend
	
	inline functions can be defined inside a class, out side it as member function and 
		as a global inline function
		
		
	all static assignments for static members are to be changed after changing the class
		name which is ID_NAME into CLASS_TYPE_NAME
	
*/



class_type:ID_NAME;
		
%%

int main(){
//	yylloc.first_line = yylloc.last_line = 1;
//	yylloc.first_column = yylloc.last_column = 0;
	
	yyin=fopen("E:\\FINAL\\CompilerProject\\ProjectFiles\\MiniC++2\\s1.cpp","r");
	yyparse();

	int x=0;
	cin>>x;
	return 0;
}

int yyerror (char *s)
{
  fprintf (stderr, "%s\n", s);
  return 0;
}