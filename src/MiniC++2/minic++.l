	/* declerations */
%{
//#include "prog_tree.h"
#include "minic++.cpp.h" /* contains token definitions and definitions */
#include <list>
#include <string>
#include <iostream.h>
#include <malloc.h>
#include "errorReporting.h"
extern YYSTYPE yylval;

void addClassID(char*);
int isClassType(char*);
int includeFileProcess();
char* getVal();
void set_location();

using namespace std;
list<char*> class_id_list;
list<char*>::iterator itr	;
int line_num = 1;
int col_num=1;

#define MAX_INCLUDE_DEPTH 10
YY_BUFFER_STATE include_stack[MAX_INCLUDE_DEPTH];
int include_stack_ptr = 0;

char fileNameBuffer[YY_BUF_SIZE]={0};
int fileNameLeng=0;

extern int yyerror (char *s);
extern YYLTYPE yylloc;
%}

identifier [a-zA-Z_][0-9a-zA-Z_]*

exponent_part [eE][-+]?[0-9]+ 
fractional_const ([0-9]*"."[0-9]+)|([0-9]+".")
double_const (({fractional_const}{exponent_part}?)|([0-9]+{exponent_part}))

integer_const [0-9]+
/* integer_const [-+]?[0-9]+  (-+ cause a problem so we ommit it, else spaces between operands is required */
/* due to the longest match rule */

/*  for string and character definition */
simple_escape [abfnrtv'"?\\]
hex_escape "x"[0-9a-fA-F]+
escape_sequence [\\]({simple_escape}|{hex_escape})
c_char [^'\\\n]|{escape_sequence} 
s_char [^"\\\n]|{escape_sequence} 

%x COMMENT
%x CLASS_NAME
%x INCLUDE_STATE
%x INCL_S0
%x INCL_S00
%x INCL_S1
%x INCL_S11
/*	INCL_S0 incdicates that we saw <
	so we need to get a valid include file name definition before >and then swap the buffers
	
	INCL_S1 incdicates that we saw "
	so we need to get a valid include file name definition before the next " then swap the buffers
	
	so include can be defined only in these two ways:
	#include <filename>
	#include "filename"		
	
	if include file is not well formed it's line is ignored and parsing continues
	for example:	#include <dddd"
	error could be reported
*/

/* rules definition*/
%%

#include	{BEGIN(INCLUDE_STATE); set_location(); }
<INCLUDE_STATE>"<"		{BEGIN(INCL_S0); set_location(); }
<INCLUDE_STATE>[ \t]*    {set_location(); }  
<INCLUDE_STATE>[^<\t \n"]*"\n"	{++line_num;  set_location();col_num=1; yyerror("include decleration not well formed"); BEGIN(INITIAL); }
<INCL_S0>[^>\n]+			{set_location(); 
								strncpy(fileNameBuffer,yytext,yyleng);
								fileNameBuffer[yyleng]='\0';
								BEGIN(INCL_S00);	
							}
								

<INCL_S00>">"		{set_location(); if(!includeFileProcess()){cout<<"include file doesn't exist\n";}BEGIN(INITIAL);}
<INCL_S00>"\n"		{++line_num;	set_location();  col_num=1; BEGIN(INITIAL);
							/* after one line if we didn't got the include file
								name then we continue parsing, without the include statment*/
							yyerror("include file is not well declared, it's ignored\n");
						}
						
						
<INCLUDE_STATE>["]+			{set_location(); BEGIN(INCL_S1);}
<INCL_S1>[^"\n]+			{set_location(); 
								strncpy(fileNameBuffer,yytext,yyleng);
								fileNameBuffer[yyleng]='\0';
								BEGIN(INCL_S11);	
							}

<INCL_S11>["]+		{set_location(); if(!includeFileProcess()){cout<<"include file doesn't exist\n";}BEGIN(INITIAL);}
<INCL_S11>"\n"			{++line_num;set_location();  col_num=1; BEGIN(INITIAL);
							/* after one line if we didn't got the include file
								name then we continue parsing, without the include statment*/
							yyerror("include file is not well declared, it's ignored\n");
						}


<<EOF>> {set_location(); 
        if ( --include_stack_ptr < 0 )
            {
            yyterminate();
            }

        else
            {
            yy_delete_buffer( YY_CURRENT_BUFFER );
            yy_switch_to_buffer(
					[include_stack_ptr] );
            }
        }


"/*"        { BEGIN(COMMENT); set_location(); }

<COMMENT>[^*\n]*        {set_location();} 
<COMMENT>"*"+[^*/\n]*   {set_location();}
<COMMENT>\n             {++line_num;set_location(); col_num=1; }
<COMMENT>"*"+"/"        {BEGIN(INITIAL);set_location(); }


class			{set_location(); BEGIN(CLASS_NAME); return CLASS;}
<CLASS_NAME>{identifier}	{set_location(); BEGIN(INITIAL); addClassID(yytext); yylval.ids=getVal(); return CLASS_ID;}
cin					{set_location();  return CIN;}
cout				{set_location();  return COUT;}
break               {set_location();  return  BREAK;}
char                {set_location();  return  CHAR;}
const               {set_location();  return CONST;}
define				{set_location();  return DEFINE;}
do                  {set_location();  return DO;}
double              {set_location();  return DOUBLE;}
else                {set_location();  return ELSE;}
extern              {set_location();  return EXTERN;}
for                 {set_location();  return FOR;}
if                  {set_location();  return IF;}
int                 {set_location();  return INT;}
return              {set_location();  return RETURN;}
continue			{set_location();  return CONTINUE;}
static              {set_location();  return STATIC;}
typedef             {set_location();  return TYPEDEF_ID;}
undef               {set_location();  return UNDEF;}
void                {set_location();  return VOID;}
while               {set_location();  return WHILE;}

delete              {set_location(); return DELETE;}
friend              {set_location(); return FRIEND;}
inline              {set_location(); return INLINE;}
new                 {set_location(); return NEW;}
operator            {set_location(); return OPERATOR;}
protected           {set_location(); return PROTECTED;}
private             {set_location(); return PRIVATE;}
public              {set_location();  return PUBLIC;}
this                {set_location(); return THIS;}
false				{set_location(); return FALSE_CONST;}
true				{set_location(); return TRUE_CONST;}
bool				{set_location(); return BOOL_TYPE;}
NULL				{set_location(); return NULL_TYPE;}

{identifier}        {set_location(); yylval.ids=getVal(); if(isClassType(yytext)) return CLASS_ID; else return ID_NAME;}

{integer_const}		{set_location(); yylval.ids=getVal(); return INTEGER_CONST;}

{double_const}		{set_location(); yylval.ids=getVal(); return DOUBLE_CONST;}

[']{c_char}+[']     {set_location(); yylval.ids=getVal(); return CHARACTER_CONST;}

["]{s_char}*["]     {set_location(); yylval.ids=getVal(); return STRING_CONST;}




"("                  {set_location(); return '(';}
")"                  {set_location(); return ')';}
","                  {set_location(); return ',';}
"#"                  {set_location(); return '#' ;}

"{"                  {set_location(); return '{';}
"}"                  {set_location(); return '}';}
"["                  {set_location(); return '[';}
"]"                  {set_location(); return ']';}
"."                  {set_location(); return '.';}
"&"                  {set_location(); return '&';}
"*"                  {set_location(); return '*';}
"+"                  {set_location(); return '+';}
"-"                  {set_location(); return '-';}
"~"                  {set_location(); return '~';}
"!"                  {set_location(); return '!';}
"/"                  {set_location(); return '/';}
"<"                  {set_location(); return '<';}
">"                  {set_location(); return '>';}
"^"                  {set_location(); return '^' ;}
"|"                  {set_location(); return '|';}
"?"                  {set_location(); return '?';}
":"                  {set_location(); return ':';}
";"                  {set_location(); return ';';}
"="                  {set_location(); return '=';}

"::"                 {set_location(); return SCOPE_DOTS;}
"->"                 {set_location(); return ARROW;}
"++"                 {set_location(); return ICR;}
"--"                 {set_location(); return DECR;}
"<="                 {set_location(); return LE;}
">="                 {set_location(); return GE;}
"=="                 {set_location(); return EQ;}
"!="                 {set_location(); return NE;}
"+="				 {set_location(); return PLUS_EQ;}
"-="				 {set_location(); return MINUS_EQ;}				
"*="				 {set_location(); return MULT_EQ;}
"/="				 {set_location(); return DIV_EQ;}


"&&"                 {set_location(); return ANDAND;}
"||"                 {set_location(); return OROR;}
"<<"				 {set_location(); return OUTPUT_VAR_SYM;}
">>"				 {set_location(); return INPUT_VAR_SYM;}
"//"[^\n]*			/* ignore single line comment */

		
[ \t]				{set_location(); }
\n				{++line_num; set_location(); col_num=1;}
.				{ErrorReport::printError(line_num,col_num,"unrecogonzied character"); set_location();}

%%
int yywrap(){
	return 1;

}
int isClassType(char* id){
		for(itr=class_id_list.begin(); itr!=class_id_list.end();itr++){
			if(strncmp(*itr,id,yyleng)==0)
				return 1;
		}	
	return 0;
}

void addClassID(char* id){
	class_id_list.push_front(id);
	
}	

int includeFileProcess(){ 
	FILE* incFile=fopen( fileNameBuffer, "r" );

        if ( ! incFile ){
            cout<<"**********couldn't open include file\n*********";
            return 0;
            
        }
        else{
        		 /* got the include file name */
			if ( include_stack_ptr >= MAX_INCLUDE_DEPTH ){
				fprintf( stderr, "Includes nested too deeply\n" );
				exit( 1 );
			}
			yyin=incFile;	
			include_stack[include_stack_ptr++] =YY_CURRENT_BUFFER;
			yy_switch_to_buffer(yy_new_buffer( yyin, YY_BUF_SIZE ) );
		}
        return 1;
}


char* getVal(){
	char* val=(char*)malloc(strlen(yytext)+1);
	strncpy(val,yytext,yyleng);
	val[yyleng]='\0';
	return val;
}


void set_location(){
	yylloc.first_line=line_num;
	yylloc.first_column=col_num;
	
	yylloc.last_line=line_num;
	col_num+=yyleng; //update column
	yylloc.last_column=col_num;
}