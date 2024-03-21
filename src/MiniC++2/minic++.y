%{
#include <stdio.h>
#include <iostream.h>
#include <string.h>
#include "malloc.h"
//#include "symbol_table.h"
#include "prog_tree.h"
#include "errorReporting.h"
%}

/*************** Token Definitions *****************************/                               


%union{
	TreeRecord* node;
	char*	ids;
};
	
%{
/*************** Includes and Defines *****************************/
#define YYDEBUG 1        /* get the pretty debugging code to compile*/
//#define YYSTYPE  char*  /* interface with flex: should be in header file */
extern FILE* yyin;
extern int yylex();	
extern int yyparse();
extern int yyerror(char*);
extern SymbolTable symtab;
//extern Record* cur_par; //initially this record contains the file scope record
//depth value used with name of id to make a unique id in a scope
//current parent
extern ProgTree progTree;
//Record* tempPar=NULL;

//Record* add2symtab(char* name);
//void restorePar();
YYSTYPE yylval;

char name_buffer[100]={0};
%}


%token EXTERN		NULL_TYPE	          CIN       
%token           DEFINE            BREAK		  COUT
%token UNDEF	      FOR             TYPEDEF_ID          
%token DOUBLE                     INCLUDE          RETURN
%token DO                       WHILE			CONTINUE					

%token ID_NAME	CLASS_ID	INT		DOUBLE	BOOL_TYPE	CHAR
%token STATIC	EXTERN CONST	INTEGER_CONST   DOUBLE_CONST	STRING_CONST
%token CHARACTER_CONST	FALSE_CONST		TRUE_CONST	VOID
%token PUBLIC          PROTECTED       PRIVATE	FRIEND

%token NEW   DELETE	THIS	OPERATOR	CLASS		INLINE		VIRTUAL       			
         
/* const tokens */
/* New Lexical element, whereas ANSI C suggested non-terminal */
%token TYPEDEF_ID


/* Operator precedence */
%left	INPUT_VAR_SYM		OUTPUT_VAR_SYM		/* <<		>>	 used for cin-cout*/
%left	PLUS_EQ		MULT_EQ		MINUS_EQ		DIV_EQ	
%left  LE GE EQ NE '=' '<' '>'     /*    <=      >=      ==      !=      */
%left  '+'	'-'	 OROR ANDAND      /*    &&      ||    */
%left	'*'		'/'
%left	'&'		'!'		'('		')'	 POS_NEG	ICR DECR /* ++   --   */
%left	LAST_PREC

%token  ARROW     SCOPE_DOTS       /*    ->       ::    */
			

%left	IF
%left	ELSE


%type <node> program  prog_decls  declaration	class_def	unary_expr	pointer_val_expr
%type <node> array_expr	var_expr	const_expr	expr	array_dim	array_dims
%type <node> while_stmt	for_itr_elem	for_itr		array_expr_dims
%type <node> for_stmt	if_stmt	block_stmt	compound_stmt	output_elem	cout_stmt_output_list
%type <node> cout_stmt	input_elem	cin_stmt_input_list	cin_stmt	jump_stmt	proc_expr_list_not_empty
%type <node> proc_expr_list	proc_call	assignment	class_destructor	class_static_init_var	
%type <node> array_init_list_lists	array_init_list_consts	array_init_list
%type <node> var_list	var_list_elem	var_decl	simple_stmt		stmt	stmts	
%type <node> class_func_def_common	class_func_def_normal	class_func_def_inline	class_func_def	
%type <node> func_def_common		func_def_normal	func_def_inline	func_def	func_decl_arg_elem
%type <node> func_decl_arg_list		func_decl_args	func_decl	class_decl
%type <node> class_destructor_decl	c_c_init_list	class_constructor_init_list	class_constructor
%type <node> class_constructor_decl		class_body_stmt	class_body_stmts	class_body	
%type <node> cast_expr	delete_stmt	/*expr_list*/	new_expr	binary_expr	allocate_expr
%type <node> math_operation_expr logical_operation_expr

%type	<node> class_type	reference_type	pointer_type	complex_type	simple_type	type
%type	<node>  storage_spec	access_spec		class_head	class_base_list

/* token type was requested by bison in addition to it's topken definition */
%type	<ids> ID_NAME	CLASS_ID	INT		DOUBLE	BOOL_TYPE	CHAR
%type   <ids> STATIC	EXTERN CONST	INTEGER_CONST   DOUBLE_CONST	STRING_CONST
%type	<ids> CHARACTER_CONST	FALSE_CONST		TRUE_CONST	VOID
%type <ids> PUBLIC          PROTECTED       PRIVATE	FRIEND	NULL_TYPE	
	
%%
/* at first trial we got 170 shift-reduce conflicts and 23 reduce-reduce conflict
	many of them was simply: one leads to another
	at the end of conflict resolving it yielded no conflicts at all
*/

program :
	prog_decls   { progTree.head->node1=$1; }
	;
	
prog_decls :
		{$$=new TreeRecord(@1,NodeName::prog_decls); }	
	|	prog_decls declaration	{$$=new TreeRecord(@1,NodeName::prog_decls,$1,$2); }	
	|	prog_decls error	{$$=new TreeRecord(@1,NodeName::prog_decls_error);} 
	;

declaration:
/*	preprocessor	{ cout<<"preprecessor\n"; } | */
	var_decl ';'	{  } 
	|func_decl 		{  }
	|func_def		{ }
	|class_decl			{}/* declare a class so it becomes defined:		class A; */
	|class_func_def {  }
	|class_def		{  }
	|class_static_init_var ';'	{ } /* static initialiation only permitted in global scope */
	|class_destructor 	{  }
	|class_constructor	{ }
	;

/*
	preprocessors must be dealt with during lex, how ever we include them
	here just as temporary action
	class_def:	 class A {...} 
	func_decl:   int getX(int,int);
	func_def:	 int getX(int x1,int x2){...}
	var_decl:	class A;	int x; int x,y,z;  int x=0,y=0,z[][];
	class_static_init_var:		int A::x=0;
*/


class_def:
	 class_head  class_body		{$$=new TreeRecord(@1,NodeName::class_def,$1,$2);} 
	|class_head ':' access_spec       class_base_list    class_body	{$$=new TreeRecord(@1,NodeName::class_def,$1,$3,$4,$5);} 
	|class_head  access_spec       class_base_list    class_body	{$$=new TreeRecord(@1,NodeName::class_def,$1,$2,$3,$4);
	 ErrorReport::printError(@1.first_line,@1.first_column,"missing :\n");	 } 
	;

class_head:
		CLASS class_type {$$=$2;} 	
		;

access_spec:
	PUBLIC		{$$=new TreeRecord(@1,NodeName::access_spec,NULL,NULL,NULL,NULL,NULL,Type::access_public);} 
	|PROTECTED	{$$=new TreeRecord(@1,NodeName::access_spec,NULL,NULL,NULL,NULL,NULL,Type::access_protected);} 
	|PRIVATE	{$$=new TreeRecord(@1,NodeName::access_spec,NULL,NULL,NULL,NULL,NULL,Type::access_private);} 
	|FRIEND		{$$=new TreeRecord(@1,NodeName::access_spec,NULL,NULL,NULL,NULL,NULL,Type::access_friend);} 
	|error		{$$=new TreeRecord(@1,NodeName::access_spec,NULL,NULL,NULL,NULL,NULL,Type::access_public);
				ErrorReport::printError(@1.first_line,@1.first_column,"unknown access specification :\n");	 } 
	;

class_base_list:
	class_type						
	|class_base_list ',' class_type	{$$=new TreeRecord(@1,NodeName::class_base_list,$1,$3);}
	|class_base_list ',' error		{$$=new TreeRecord(@1,NodeName::name_error);}
	;

class_body:
	'{' '}' ';'		{$$=new TreeRecord(@1,NodeName::class_body);} 
	|'{'  class_body_stmts '}' ';'	{$$=new TreeRecord(@1,NodeName::class_body,$2);} 
	;

class_body_stmts:	
	class_body_stmt						{$$=new TreeRecord(@1,NodeName::class_body_stmts,$1);}
	|class_body_stmts class_body_stmt	{$$=new TreeRecord(@1,NodeName::class_body_stmts,$1,$2);} 
	;
	
class_body_stmt:
	access_spec ':'			{$$=$1} 		
	|func_decl					
	|func_def   /* consided as inline by default if INLINE keyword not mentioned*/					
	|var_decl ';'					
	|class_constructor_decl
	|class_destructor_decl
	|error ';'				{$$=new TreeRecord(@1,NodeName::class_body_stmt);}  
	|error '}'				{$$=new TreeRecord(@1,NodeName::class_body_stmt);}  
	;	
/*
update func_def to include the case where const data needs initilaiation
in the construtor X::X()f(0){ }   where--> const int f;
*/
class_constructor_decl:
	class_type '(' func_decl_args ')' ';'
		 {	$$=new TreeRecord(@1,NodeName::class_constructor_decl,$1,$3);}  
	;

class_constructor:
	class_type SCOPE_DOTS class_type '(' func_decl_args ')' class_constructor_init_list block_stmt	
	{$$=new TreeRecord(@1,NodeName::class_constructor,$1,$3,$5,$7,$8);}
	;
	
class_constructor_init_list: /* empty */{$$=new TreeRecord(@1,NodeName::class_constructor_init_list);}
	|':' c_c_init_list					{$$=new TreeRecord(@1,NodeName::class_constructor_init_list,$2);}
	|':' class_type '(' proc_expr_list ')' {$$=new TreeRecord(@1,NodeName::class_constructor_init_list,$2,$4);}

	;
/* member initialiation list:		A::A(int x,int y):f(x),f(y){...} */
c_c_init_list:
	ID_NAME '(' expr ')'				{$$=new TreeRecord(@1,NodeName::c_c_init_list,new TreeRecord(@1,$1),$3);}
	|c_c_init_list ',' ID_NAME '(' expr ')'	{$$=new TreeRecord(@1,NodeName::c_c_init_list,$1,new TreeRecord(@3,$3),$5);}
	;

class_destructor_decl:
	'~' class_type '(' ')' ';'		 {	$$=new TreeRecord(@1,NodeName::class_destructor_decl,$2);}
	;	

class_decl:
	class_head ';'		{$$=new TreeRecord(@1,NodeName::class_decl,$1);}

func_decl:
	storage_spec type ID_NAME '(' func_decl_args ')' ';'
			{$$=new TreeRecord(@1,NodeName::func_decl,$1,$2,new TreeRecord(@3,$3),$5);}	
	|type ID_NAME '(' func_decl_args ')' ';'
			 {$$=new TreeRecord(@1,NodeName::func_decl,$1,new TreeRecord(@2,$2),$4);	}
	|storage_spec type ID_NAME '(' func_decl_args ')' CONST ';'	
			 {$$=new TreeRecord(@1,NodeName::func_decl,$1,$2,new TreeRecord(@3,$3),$5,new TreeRecord(@7,"CONST"));}	
	|type ID_NAME '(' func_decl_args ')' CONST ';'  
			{$$=new TreeRecord(@1,NodeName::func_decl,$1,new TreeRecord(@2,$2),$4,new TreeRecord(@6,"CONST"));}			
	
	;
	
storage_spec:
	STATIC	{$$=new TreeRecord(@1,NodeName::storage_spec,NULL,NULL,NULL,NULL,NULL,Type::storage_static);}
	|CONST	{$$=new TreeRecord(@1,NodeName::storage_spec,NULL,NULL,NULL,NULL,NULL,Type::storage_const);}
	|EXTERN {$$=new TreeRecord(@1,NodeName::storage_spec,NULL,NULL,NULL,NULL,NULL,Type::storage_extern);}
	|STATIC CONST	/* these two termainls is to enable within class member initialiation */
		{$$=new TreeRecord(@1,NodeName::storage_spec,NULL,NULL,NULL,NULL,NULL,Type::storage_static_const);}
	|CONST STATIC   /* the check is done in semantic analysis */
		{$$=new TreeRecord(@1,NodeName::storage_spec,NULL,NULL,NULL,NULL,NULL,Type::storage_static_const);}
	;

func_decl_args:				{$$=new TreeRecord(@1,NodeName::func_decl_args);}
	| func_decl_arg_list  	{$$=new TreeRecord(@1,NodeName::func_decl_args,$1);}
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
	|func_decl_arg_list ',' func_decl_arg_elem	{$$=new TreeRecord(@1,NodeName::func_decl_arg_list,$1,$3);}
	;
	
func_decl_arg_elem:
	type							{$$=new TreeRecord(@1,NodeName::func_decl_arg_elem,$1);}
	|type ID_NAME					{$$=new TreeRecord(@1,NodeName::func_decl_arg_elem,$1,new TreeRecord(@2,$2));}
	|type ID_NAME '=' const_expr	{$$=new TreeRecord(@1,NodeName::func_decl_arg_elem,$1,new TreeRecord(@2,$2),$4);}
	;
/*  void f(),  void f(int x,int), void f(int x=0,int) */
/* type is defined later: void, int, double, a class name, array... */
/* const_expr is defined later: 0, "ddd", assuming only const values are permitted...*/
/* i'll leave chekcing for constant redefinition in the semantic check */

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
	storage_spec INLINE func_def_common		{$$=new TreeRecord(@1,NodeName::func_def_inline,$1,$3);}
	|INLINE func_def_common					{$$=new TreeRecord(@1,NodeName::func_def_inline,$2);}
	;

func_def_normal:
	storage_spec func_def_common	{$$=new TreeRecord(@1,NodeName::func_def_normal,$1,$2);}
	|func_def_common				{$$=new TreeRecord(@1,NodeName::func_def_normal,$1);}
	;

func_def_common:		
		type ID_NAME '(' func_decl_args ')' 	block_stmt	
			{$$=new TreeRecord(@1,NodeName::func_def_common,$1,new TreeRecord(@2,$2),$4,$6);}
		|type ID_NAME '(' func_decl_args ')' CONST 		block_stmt
			{$$=new TreeRecord(@1,NodeName::func_def_common,$1,new TreeRecord(@2,$2),$4,new TreeRecord(@6,"CONST"),$7);}
		;

/* changing the func_decl_args for a fucntion decleration and definition cause
	reduce reduce conflicts */

/* two main types of fucntions appear, ones with a class scope like A::f()
and ones with no scope
scoped functions appear only outside classes, wheras normal functions appear 
any where
*/

class_func_def:
	class_func_def_inline			
	|class_func_def_normal			
	;

class_func_def_inline:
	storage_spec INLINE class_func_def_common	{$$=new TreeRecord(@1,NodeName::class_func_def_inline,$1,$3);}
	|INLINE class_func_def_common				{$$=new TreeRecord(@1,NodeName::class_func_def_inline,$2);}
	;

class_func_def_normal:
	storage_spec class_func_def_common	{$$=new TreeRecord(@1,NodeName::class_func_def_normal,$1,$2);}
	|class_func_def_common				{$$=new TreeRecord(@1,NodeName::class_func_def_normal,$1);}
	;

class_func_def_common:		
		type class_type SCOPE_DOTS ID_NAME '(' func_decl_args ')'	 block_stmt	
			{$$=new TreeRecord(@1,NodeName::class_func_def_common,$1,$2,new TreeRecord(@4,$4),$6,$8);}
		;
/* void f(...){ }		void A::f(...){} */		

	
stmts:
	stmt		
	|stmts stmt		{$$=new TreeRecord(@1,NodeName::stmts,$1,$2);}
	;
/* one or more statements */	

	
stmt:
	simple_stmt ';'		
	|compound_stmt		
	|error ';'			{$$=new TreeRecord(@1,NodeName::stmt_error);}
	|error '}'			{$$=new TreeRecord(@1,NodeName::stmt_error);}
	;	
/*simple contains one statement
	compound: may contain multiple statements like loops-if-blocks
*/


simple_stmt:
	var_decl		
	|assignment		
	|expr 				{$$=new TreeRecord(@1,NodeName::expr_stmt,$1);}
	|jump_stmt      
	|cin_stmt			
	|cout_stmt   
	|delete_stmt	  
	;

var_decl:
	storage_spec type var_list		{$$=new TreeRecord(@1,NodeName::var_decl,$1,$2,$3);}
	|type var_list					{$$=new TreeRecord(@1,NodeName::var_decl,$1,$2);}
	;

var_list_elem:
	ID_NAME						 
			 { $$=new TreeRecord(@1,NodeName::var_list_elem,new TreeRecord(@1,$1));}
	|ID_NAME array_dims			
			  {$$=new TreeRecord(@1,NodeName::var_list_elem,new TreeRecord(@1,$1),$2);}
	|ID_NAME '=' expr			
			 {$$=new TreeRecord(@1,NodeName::var_list_elem,new TreeRecord(@1,$1),$3);}
	|ID_NAME array_dims '=' array_init_list	 
			{$$=new TreeRecord(@1,NodeName::var_list_elem,new TreeRecord(@1,$1),$2,$4);}  
	;
/* int x;		A a(2,3);*/
/* array decleration can be added here since it's decleration is different:
	int x [] []; here variable comes between array def and referenced type
	whereas types usually comes before variable
*/
	
/* there is no need to make the array intilaiation list as part of expr because 
	we then have to make a type check on it when it's used in places not of it's own
	how ever here we check it in parse time
	int c[]={1,2,3}	
*/
	
var_list:
	var_list_elem					
	|var_list ',' var_list_elem		{$$=new TreeRecord(@1,NodeName::var_list,$1,$3);}
	|ID_NAME '(' proc_expr_list_not_empty ')'
			 {$$=new TreeRecord(@1,NodeName::var_constructor_call,new TreeRecord(@1,$1),$3);}
	;
/*
	declaring class variables can't be done with empty arguments, it conflicts with the 
	proc empty args and thus makes epsilon reduction
	so the form A a(); is incorrect and must be replaced by A a; where A is class type
	and has a defult constructor with empty params
	the decleration: A a(1,2); how ever is allowed when contructor has params
	we can enable the A a(); form but then we have to separate class type from other types
	and make separate rules.
*/
array_dims: 
	array_dim
	| array_dims array_dim	{$$=new TreeRecord(@1,NodeName::array_dims,$1,$2);}
	;

array_dim:
	'[' ']'					{$$=new TreeRecord(@1,NodeName::array_dim);}
	|'[' INTEGER_CONST ']'	{$$=new TreeRecord(@1,NodeName::array_dim,new TreeRecord(@2,$2));}
	;
/* int[][][22] x;   integer index only*/

array_init_list:
	'{' array_init_list_consts '}'		{$$=new TreeRecord(@1,NodeName::array_init_list,$2);}
	|'{' array_init_list_lists '}'		{$$=new TreeRecord(@1,NodeName::array_init_list,$2);}
	;	

array_init_list_consts:
	expr									{$$=new TreeRecord(@1,NodeName::array_init_list_consts,$1);}
	|array_init_list_consts ',' expr		{$$=new TreeRecord(@1,NodeName::array_init_list_consts,$1,$3);}
	;	

array_init_list_lists:
	array_init_list			{$$=new TreeRecord(@1,NodeName::array_init_list_lists,$1);}
	|array_init_list_lists ',' array_init_list	{$$=new TreeRecord(@1,NodeName::array_init_list_lists,$1,$3);}
	;

/*
the above declerations enables a complex muli-level array initiailation, i.e.
	int x[3][2]={{0,1},{1,2},{0,0}};
	and more.
*/

class_static_init_var:
	type class_type SCOPE_DOTS ID_NAME '=' expr		{$$=new TreeRecord(@1,NodeName::class_static_init_var,$1,$2,new TreeRecord(@4,$4),$6);}
	;	
/*int A::x=0; in static member initialization, it's a unique variable initialiation rule
	only happens in a global scope
	putting the rule in the following form causes Shift-reduce conflicts:
		type class_type SCOPE_DOTS ID_NAME '=' expr	%prec LAST_PREC	
	how ever removing (type) only deletes this conflict
	and i have no idea why it happens
 */	

class_destructor:
	class_type SCOPE_DOTS '~' class_type '(' ')' block_stmt		{$$=new TreeRecord(@1,NodeName::class_destructor,$1,$4,$7);}
	;

assignment: 
	var_expr '=' expr			{$$=new TreeRecord(@1,NodeName::assignment,$1,$3,NULL,NULL,NULL,Type::ASSIGN_NORMAL);}
	|var_expr PLUS_EQ expr		{$$=new TreeRecord(@1,NodeName::assignment,$1,$3,NULL,NULL,NULL,Type::ASSIGN_PLUS);}
	|var_expr MINUS_EQ expr		{$$=new TreeRecord(@1,NodeName::assignment,$1,$3,NULL,NULL,NULL,Type::ASSIGN_MINUS);}
	|var_expr DIV_EQ expr		{$$=new TreeRecord(@1,NodeName::assignment,$1,$3,NULL,NULL,NULL,Type::ASSIGN_DIV);}
	|var_expr MULT_EQ expr		{$$=new TreeRecord(@1,NodeName::assignment,$1,$3,NULL,NULL,NULL,Type::ASSIGN_MULT);}
	;
/*  x=0;		a.x=0;		A::x=0 */


proc_call: 
	ID_NAME '(' proc_expr_list ')'				
		{$$=new TreeRecord(@1,NodeName::proc_call,new TreeRecord(@1,$1),$3,NULL,NULL,NULL,Type::proc_call_normal);}			
	|ID_NAME '.' ID_NAME '(' proc_expr_list ')'				
		{$$=new TreeRecord(@1,NodeName::proc_call,new TreeRecord(@1,$1),new TreeRecord(@3,$3),$5,NULL,NULL,Type::PROC_CLASS_CALL);}		
	|THIS ARROW ID_NAME '(' proc_expr_list ')'							
		{$$=new TreeRecord(@1,NodeName::proc_call,new TreeRecord(@3,$3),$5,NULL,NULL,NULL,Type::PROC_THIS_CALL);}
	|class_type SCOPE_DOTS ID_NAME '(' proc_expr_list ')'
		{$$=new TreeRecord(@1,NodeName::proc_call,$1,new TreeRecord(@3,$3),$5,NULL,NULL,Type::PROC_CLASS_STATIC_CALL);}		
	|ID_NAME ARROW ID_NAME '(' proc_expr_list ')'							
		{$$=new TreeRecord(@1,NodeName::proc_call,new TreeRecord(@1,$1),new TreeRecord(@3,$3),$5,NULL,NULL,Type::PROC_ARROW_CALL);}
	; 
/*	f(4);	a.f(...)  A::f(...)*/

proc_expr_list:	/*empty list */		{$$=new TreeRecord(@1,NodeName::proc_expr_list);}
	|proc_expr_list_not_empty		{$$=new TreeRecord(@1,NodeName::proc_expr_list,$1);}		
	;
proc_expr_list_not_empty:
	expr								{$$=new TreeRecord(@1,NodeName::proc_expr_list_not_empty,$1);}
	|proc_expr_list_not_empty ',' expr	{$$=new TreeRecord(@1,NodeName::proc_expr_list_not_empty,$1,$3);}
	;


jump_stmt:
	BREAK		{$$=new TreeRecord(@1,NodeName::jump_stmt,NULL,NULL,NULL,NULL,NULL,Type::break_type);}
	|RETURN expr	{$$=new TreeRecord(@1,NodeName::jump_stmt,$2,NULL,NULL,NULL,NULL,Type::return_expr_type);}
	|CONTINUE		{$$=new TreeRecord(@1,NodeName::jump_stmt,NULL,NULL,NULL,NULL,NULL,Type::continue_type);}
	|RETURN			{$$=new TreeRecord(@1,NodeName::jump_stmt,NULL,NULL,NULL,NULL,NULL,Type::return_type);}
	;

cin_stmt:
	CIN cin_stmt_input_list	{$$=new TreeRecord(@1,NodeName::cin_stmt,$2);}
	;

cin_stmt_input_list:
	input_elem
	|cin_stmt_input_list input_elem	{$$=new TreeRecord(@1,NodeName::cin_stmt_input_list,$1,$2);}
	;
input_elem:
	INPUT_VAR_SYM var_expr	{$$=new TreeRecord(@1,NodeName::input_elem,$2);}
	;	
	
cout_stmt:
	COUT cout_stmt_output_list	{$$=new TreeRecord(@1,NodeName::cout_stmt,$2);}
	;
// cout<<x<<x*x<<f(x)<<"ddd";
	
cout_stmt_output_list:
	output_elem
	|cout_stmt_output_list output_elem	{$$=new TreeRecord(@1,NodeName::cout_stmt_output_list,$1,$2);}
	;
output_elem:
	OUTPUT_VAR_SYM expr		{$$=new TreeRecord(@1,NodeName::output_elem,$2);}
	;	
/* <<x  */
		
compound_stmt:
	if_stmt				
	|for_stmt			
	|while_stmt				
	|block_stmt	{$$=new TreeRecord(@1,NodeName::compound_block_stmt,$1);}			
	;
	
/* block stmt like this  {stmts} */
block_stmt:
	'{' '}'		{$$=new TreeRecord(@1,NodeName::block_stmt);}
	|'{'  stmts '}' 
				{ $$=new TreeRecord(@1,NodeName::block_stmt,$2);}	
	;
	
	if_stmt:
		IF '(' expr ')'  stmt	/* %prec IF */		{$$=new TreeRecord(@1,NodeName::if_stmt,$3,$5);}			
		|IF '(' expr ')'  stmt ELSE stmt	/* %prec ELSE */			{$$=new TreeRecord(@1,NodeName::if_else_stmt,$3,$5,$7);}		
		|IF  expr '}'  stmt				{$$=new TreeRecord(@1,NodeName::if_stmt,$2,$4);
			ErrorReport::printError(@1.last_line,@1.last_column,"missing ( :\n");}			
		
	;
/* no worry about SHIFT_REDUCE conflicts here, because bison always shifts first
and that make the right rule choosen, because bison uses look ahead char 
so even without precedence-associativity it's ok*/

for_stmt:
	FOR '(' var_decl ';' expr ';' for_itr')' stmt		{$$=new TreeRecord(@1,NodeName::for_stmt,$3,$5,$7,$9);}
	|FOR  var_decl ';' expr ';' for_itr')' stmt		{$$=new TreeRecord(@1,NodeName::for_stmt,$2,$4,$6,$8);
		ErrorReport::printError(@2.first_line,@2.first_column,"missing ( :\n");}			
	
	;
/* i.e: for(int x=0, y=1, A a=new A(1,2) ; x<10 ; x++, y=y*2 ){ }  */
/* one or more initiliation statements separated by comma */


for_itr:
	for_itr_elem				{$$=new TreeRecord(@1,NodeName::for_itr,$1);}
	|for_itr ',' for_itr_elem	{$$=new TreeRecord(@1,NodeName::for_itr,$1,$3);}
	;

for_itr_elem:
	expr		{$$=new TreeRecord(@1,NodeName::for_itr_elem_expr,$1);}		
	|assignment
	;


while_stmt:
	WHILE '(' expr ')' stmt			{$$=new TreeRecord(@1,NodeName::while_stmt,$3,$5);}
	; 


type:
	simple_type				
	|complex_type				
	; 

simple_type:
	INT			{$$=new TreeRecord(@1,NodeName::simple_type,NULL,NULL,NULL,NULL,NULL,Type::type_int);}
	|DOUBLE		{$$=new TreeRecord(@1,NodeName::simple_type,NULL,NULL,NULL,NULL,NULL,Type::type_double);}	
	|BOOL_TYPE	{$$=new TreeRecord(@1,NodeName::simple_type,NULL,NULL,NULL,NULL,NULL,Type::type_bool);}		
	|CHAR		{$$=new TreeRecord(@1,NodeName::simple_type,NULL,NULL,NULL,NULL,NULL,Type::type_char);}	
	|VOID		{$$=new TreeRecord(@1,NodeName::simple_type,NULL,NULL,NULL,NULL,NULL,Type::type_void);}	
	;


complex_type: 
	pointer_type			
	|reference_type			
	|class_type 
	;
	
/* array type is not added here, since it's definition is different from others
	it's added as part of var_decl
	besides, in type it gave me 1 shift-reduce conflict which we couldn't resolve
	so the preper changes were made and conflict was resolved upon the rules change
	*/	

pointer_type:
	type '*'	{$$=new TreeRecord(@1,NodeName::pointer_type,$1,NULL,NULL,NULL,NULL,Type::pointer_type);}
	;
reference_type:
	type '&'	{$$=new TreeRecord(@1,NodeName::reference_type,$1,NULL,NULL,NULL,NULL,Type::reference_type);}
	;

expr:
	const_expr			
	|var_expr			
	|unary_expr			
	|binary_expr			
	|allocate_expr			
	|proc_call			
	|cast_expr			
	|'(' expr ')'			{$$=new TreeRecord(@1,NodeName::bracket_expr,$2);}
	;


/* expressions: const values, unary and binary ops... 
check, some definitions can accept empty expressions as well
*/


const_expr:
	INTEGER_CONST			{$$=new TreeRecord(@1,NodeName::int_const_expr,$1,Type::type_int);}
	|DOUBLE_CONST			{$$=new TreeRecord(@1,NodeName::double_const_expr,$1,Type::type_double);}
	|CHARACTER_CONST		{$$=new TreeRecord(@1,NodeName::char_const_expr,$1,Type::type_char);}
	|STRING_CONST			{$$=new TreeRecord(@1,NodeName::str_const_expr,$1,Type::type_str);}
	|FALSE_CONST			{$$=new TreeRecord(@1,NodeName::false_const_expr,$1,Type::type_false);}
	|TRUE_CONST				{$$=new TreeRecord(@1,NodeName::true_const_expr,$1,Type::type_true);}
	|NULL_TYPE				{$$=new TreeRecord(@1,NodeName::null_const_expr,$1,Type::type_null);}
	
	;

	
var_expr: 
	array_expr
	|array_expr '.' var_expr {$$=new TreeRecord(@1,NodeName::array_class_expr,$1,$3);}	
	|array_expr ARROW var_expr {$$=new TreeRecord(@1,NodeName::array_pointer_expr,$1,$3);}			
	|pointer_val_expr				
	|ID_NAME				{$$=new TreeRecord(@1,NodeName::id_name_expr,new TreeRecord(@1,$1));}
	|THIS					{$$=new TreeRecord(@1,NodeName::this_expr);}
	|ID_NAME '.' var_expr	{$$=new TreeRecord(@1,NodeName::class_var_expr,new TreeRecord(@1,$1),$3);}			
	|class_type SCOPE_DOTS ID_NAME  
				 {$$=new TreeRecord(@1,NodeName::scope_expr,$1,new TreeRecord(@3,$3));}	 
	|ID_NAME ARROW var_expr		 /* x->d */
				{$$=new TreeRecord(@1,NodeName::id_arrow_expr,new TreeRecord(@1,$1),$3);}
	|THIS ARROW var_expr	{$$=new TreeRecord(@1,NodeName::this_arrow_expr,$3);}
	
	;	
/* those that u can store values in */
/* conflicted when putting it like: var_expr ARROW var_expr
this enables forms like: a.c.d.e=0;		a->a->d.c=0;	a[0].a=0; when a[0] is a class type
*/
	
array_expr:
	ID_NAME array_expr_dims	{$$=new TreeRecord(@1,NodeName::array_expr,new TreeRecord(@1,$1),$2);}
	;
array_expr_dims:
	'[' expr ']'	{$$=new TreeRecord(@1,NodeName::array_expr_dims,$2);}
	|array_expr_dims '[' expr ']'	{$$=new TreeRecord(@1,NodeName::array_expr_dims,$1,$3);}
	;

pointer_val_expr:
	'*' var_expr	{$$=new TreeRecord(@1,NodeName::pointer_val_expr,$2);}	
	;
	
	
unary_expr:
	 '!' expr		{$$=new TreeRecord(@1,NodeName::unary_expr,$2,NULL,NULL,NULL,NULL,Type::NOT);}		
	| '-' expr	%prec POS_NEG	{$$=new TreeRecord(@1,NodeName::unary_expr,$2,NULL,NULL,NULL,NULL,Type::MINUS);}		
	| ICR expr					{$$=new TreeRecord(@1,NodeName::unary_expr,$2,NULL,NULL,NULL,NULL,Type::left_icr);}
	| expr ICR 				{$$=new TreeRecord(@1,NodeName::unary_expr,$1,NULL,NULL,NULL,NULL,Type::right_icr);}
	| DECR expr				{$$=new TreeRecord(@1,NodeName::unary_expr,$2,NULL,NULL,NULL,NULL,Type::left_decr);}	
	| expr DECR 			{$$=new TreeRecord(@1,NodeName::unary_expr,$1,NULL,NULL,NULL,NULL,Type::right_decr);}	
	| '&' var_expr			{$$=new TreeRecord(@1,NodeName::unary_expr,$2,NULL,NULL,NULL,NULL,Type::and);}	
	| '+' expr		%prec POS_NEG	{$$=new TreeRecord(@1,NodeName::unary_expr,$2,NULL,NULL,NULL,NULL,Type::PLUS);}			
	;


binary_expr:
	math_operation_expr
	|logical_operation_expr
	;
math_operation_expr:
	expr '+' expr		{$$=new TreeRecord(@1,NodeName::math_operation_expr,$1,$3,NULL,NULL,NULL,Type::PLUS);}				
	|expr '-' expr		{$$=new TreeRecord(@1,NodeName::math_operation_expr,$1,$3,NULL,NULL,NULL,Type::MINUS);}				
	|expr '/' expr		{$$=new TreeRecord(@1,NodeName::math_operation_expr,$1,$3,NULL,NULL,NULL,Type::DIV);}				
	|expr '*' expr		{$$=new TreeRecord(@1,NodeName::math_operation_expr,$1,$3,NULL,NULL,NULL,Type::MULT);}				
	
	;

logical_operation_expr:
	expr '<' expr		{$$=new TreeRecord(@1,NodeName::logical_operation_expr,$1,$3,NULL,NULL,NULL,Type::LESS);}				
	|expr '>' expr		{$$=new TreeRecord(@1,NodeName::logical_operation_expr,$1,$3,NULL,NULL,NULL,Type::GREATER);}				
	|expr LE expr			{$$=new TreeRecord(@1,NodeName::logical_operation_expr,$1,$3,NULL,NULL,NULL,Type::LEQ);}				
	|expr GE expr			{$$=new TreeRecord(@1,NodeName::logical_operation_expr,$1,$3,NULL,NULL,NULL,Type::GEQ);}	
	|expr EQ expr			{$$=new TreeRecord(@1,NodeName::logical_operation_expr,$1,$3,NULL,NULL,NULL,Type::EQL);}				
	|expr NE expr			{$$=new TreeRecord(@1,NodeName::logical_operation_expr,$1,$3,NULL,NULL,NULL,Type::NEQ);}
	|expr OROR expr			{$$=new TreeRecord(@1,NodeName::logical_operation_expr,$1,$3,NULL,NULL,NULL,Type::oror);}			
	|expr ANDAND expr		{$$=new TreeRecord(@1,NodeName::logical_operation_expr,$1,$3,NULL,NULL,NULL,Type::andand);}		
	;

allocate_expr:
	new_expr			
	;

new_expr:
	NEW type '[' expr ']'	{$$=new TreeRecord(@1,NodeName::new_array_expr,$2,$4);}
	|NEW type '(' proc_expr_list ')'  	{$$=new TreeRecord(@1,NodeName::new_expr,$2,$4);}
	;
/* for constructor initilization 
		
 expr_list:					{$$=new TreeRecord(@1,NodeName::expr_list);}
	|expr					{$$=new TreeRecord(@1,NodeName::expr_list,$1);}
	| expr_list ',' expr	{$$=new TreeRecord(@1,NodeName::expr_list,$1,$3);}
	;
/* thus the expression : new A(1,2)  or   new A() is also enabled */

delete_stmt:
	DELETE var_expr		{$$=new TreeRecord(@1,NodeName::delete_stmt,$2);}
	|DELETE '[' ']' var_expr	{$$=new TreeRecord(@1,NodeName::delete_stmt,$4);}
	;
cast_expr:
	'(' type ')' expr	{$$=new TreeRecord(@1,NodeName::cast_expr,$2,$4);}				
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
		or in grammar, use type checking to see if the ID_NAME is a Class type in the place
			it's supposed to be
		
	friend
	
	inline functions can be defined inside a class, out side it as member function and 
		as a global inline function
		
		
	all static assignments for static members are to be changed after changing the class
		name which is ID_NAME into CLASS_TYPE_NAME
		
	for functions allow no return types, so they are considered void: f(int x);   main(){}
		by definining func_ret_type: |type;
		
	allow function definition to be null definition
	define NULL
	
	the problem that happened when i defined a single storage_spec rule which enable
		empty reduction, such a reduction conflicted with many other rules, and the solution
		was to remove this empty reduction and double the other relating rules to include both
		cases where a storage class is specified and not, not very essential modification 
		how ever it worked.
	
*/

class_type: 
	CLASS_ID	{$$=new TreeRecord(@1,$1,NULL,NULL,NULL,NULL,NULL,Type::class_type);}
	;
		
%%



int yyerror (char *s)
{
  fprintf (stderr, "%s\n", s);
  return 0;
}

/* i have no idea what this function does, but the VC++ gives me a linker error from the
	lexer, so i defined it.
	when i returned 1 i got syntax errors when i execute the generated compier
	and 0 it gives right result at run time????????????????????????????
*/
int isatty(int x){
	return 0;
}

/* initila adding to symbol table 
Record* add2symtab(char* name){
	return symtab.addRecord(Record(name,cur_par->depth+1,cur_par));
}

// restoring parent scopes must be done acurately or things won't work right 
void restorePar(){
	cur_par=cur_par->parent;
}
*/

char* concat(char* first, char* second){
	int flen=strlen(first);
	int slen=strlen(second);
	
	strncpy(name_buffer,first,flen);
	name_buffer[flen]='\0';
	strncat(name_buffer,second,slen);
	name_buffer[flen+slen]='\0';
	
	return name_buffer;
}