#include <fstream>
#include <iostream.h>
#include <string.h>
#include "prog_tree.h"
#include "errorReporting.h"
#include "codeGeneration.h"

extern SymbolTable symtab;
extern ProgTree progTree;

ofstream file("c:\\1.vm");

using namespace std;

int CodeGeneration::shift=0;
int CodeGeneration::class_shift=0;
bool CodeGeneration::firstRound=true;
int CodeGeneration::labelCount=0;
int params_count=0; //current method params count
int params_call_count=0; //current method call params count
bool class_fucn=false;
char* currentEndLabel=NULL; //used by break statement
char* currentStartLabel=NULL; //used by continue statement


bool CodeGeneration::optimize=true;
/*
 now we'll tranverse the correct syntax tree back to generate
 the virtual machine code file
*/
void CodeGeneration::generateVMCode(char* filePath){
	if(ErrorReport::containsError){
		cout<<"can't generate code for incorrect code\n";
		return;
	}
	//we do initialiaztion
	shift=0;
	file<<"alloc 0\n";
	++shift;
	initGlobalVars(progTree.head->node1); //first we init the global variables by reserving space for them	
	generateVarDeclsUpdateCode(progTree.head->node1);

	file<<"start\n";
	
	generateMainCall();

	file<<"stop\n";
	//first we go through declerations to allocate the global variables

	generateProgDeclsCode(progTree.head->node1);
	
	file.close();
			
}

/*
search program declarations and find first main if there is any
*/
bool CodeGeneration::generateMainCall(){
	TreeRecord* node=progTree.head->node1;
	TreeRecord* decl=NULL;	
	while(node->node1!=NULL){
			
		if(ProgTree::name_comp(node->node2->node_name,NodeName::func_def_normal)){
			TreeRecord* common_node=NULL;
			decl=node->node2;
			if(decl->node2==NULL)
				common_node=decl->node1;
			else
				common_node=decl->node2;
			if(ProgTree::name_comp(common_node->node2->node_name,"main")){

				//first we reserver place for the return value
				this->generateTypeCode(new TreeRecord(common_node->node1->node_type));
				generateProcCallParamsCode(common_node->node3);
				file<<"pusha "<<common_node->codeName<<"\n";
				file<<"call\n";
				//now pop generated params
				if(common_node->node1->node_type!=Type::type_void){
					file<<"pushs \"main exit with code:\"\n";
					file<<"writes\n";
					file<<"writei\n";
//					file<<"pop 1\n";
				}

				return true;
			}	
		}
		node=node->node1;
	}
		return false;
		cout<<"Program doesn't contain main method\n";
}

/*
program declerations

prog_decls :
		{$$=new TreeRecord(NodeName::prog_decls); }	
	|	prog_decls declaration	{$$=new TreeRecord(NodeName::prog_decls,$1,$2); }	
	;
*/
void CodeGeneration::generateProgDeclsCode(TreeRecord* node){
	if(node->node1==NULL)
		return; //empty decleration
	generateProgDeclsCode(node->node1);
	generateDeclCode(node->node2);
}

/*
passing prog_decl node
used initially to set global variables with their initial values
*/
void CodeGeneration::generateVarDeclsUpdateCode(TreeRecord* node){
	if(node->node1==NULL)
		return; //empty decleration
	generateVarDeclsUpdateCode(node->node1);

	if(ProgTree::name_comp(node->node2->node_name,NodeName::var_decl)){
		generateVarDeclCode(node->node2);
		return;
	}
}

/*
declaration:
	var_decl ';'	
	|func_decl 		
	|c		
	|class_decl			
	|class_func_def 
	|class_def		
	|class_static_init_var ';'	
	|class_destructor 	
	|class_constructor	
	;
*/

void CodeGeneration::generateDeclCode(TreeRecord* node){
	if(ProgTree::name_comp(node->node_name,NodeName::func_def_inline)||ProgTree::name_comp(node->node_name,NodeName::func_def_normal)){
		generateFunDefCode(node);
		return;
	}
	if(ProgTree::name_comp(node->node_name,NodeName::class_func_def_inline)||ProgTree::name_comp(node->node_name,NodeName::class_func_def_normal)){
		class_fucn=true;
		generateClassFunDefCode(node);
		class_fucn=false;
		return;
	}
	/* when we see a class definition we just calcluate it's size required for
	it's allocation command: the size is equal to member variable sum of sizes puls
	parents sizes
	*/
	if(ProgTree::name_comp(node->node_name,NodeName::class_def)){
		calcClassSize(node);
		return;
	}

	/*
	like normal function code generation for now
	*/
	if(ProgTree::name_comp(node->node_name,NodeName::class_constructor)){
		generateClassConstructorFunDefCode(node);
		return;
	}
	if(ProgTree::name_comp(node->node_name,NodeName::class_destructor)){
		generateClassDestructorFunDefCode(node);
		return;
	}
	
}
/*
class_def:
	 class_head  class_body		{$$=new TreeRecord(@1,NodeName::class_def,$1,$2);} 
	|class_head ':' access_spec       class_base_list    class_body	{$$=new TreeRecord(@1,NodeName::class_def,$1,$3,$4,$5);} 
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
	|func_def   					
	|var_decl ';'					
	|class_constructor_decl
	|class_destructor_decl
	|error ';'				{$$=new TreeRecord(@1,NodeName::class_body_stmt);}  
	|error '}'				{$$=new TreeRecord(@1,NodeName::class_body_stmt);}  
	;	
*/
/*
	calculate class size required for it's allocation command: the size is equal to member variable sum of sizes puls
	parents sizes
	size is stored in size member of TreeRecord, so size is calulated once
*/
void CodeGeneration::calcClassSize(TreeRecord* node){
	TreeRecord* body=node->node2;
	TreeRecord* baseList=node->node3;
	TreeRecord* stmts=NULL;
	if(node->node3!=NULL)
		body=node->node4;
	if(body->node1==NULL)
		return; //empty class
	stmts=body->node1;
	class_shift=0;

	//check base list first:
	if(baseList!=NULL){ //it hase parent, one parent is allowed
		if(baseList->node_type==Type::class_type){
			//find parent record
			Record r(baseList->node_name,symtab.first_scope->depth+1,symtab.first_scope,Type::class_type);
			Record* classRec=symtab.findRecord(r);
			class_shift=classRec->ref_node->size;
		}
		else{
			cout<<"code will not be generated for parent classes\n";
		}
	}


	while(true){
		if(stmts->node2==NULL){ //final statement
			getClassBodyStmtSize(stmts->node1);
			break;
		}
		getClassBodyStmtSize(stmts->node2);
		stmts=stmts->node1;
	}
	
	//(later) calc size of parents and add them into this size
	node->size=class_shift;
}

/*class_body_stmt:
	access_spec ':'			{$$=$1} 		
	|func_decl					
	|func_def   					
	|var_decl ';'					
	|class_constructor_decl
	|class_destructor_decl
	|error ';'				{$$=new TreeRecord(@1,NodeName::class_body_stmt);}  
	|error '}'				{$$=new TreeRecord(@1,NodeName::class_body_stmt);}  
	;

var_decl:
	storage_spec type var_list		{$$=new TreeRecord(@1,NodeName::var_decl,$1,$2,$3);}
	|type var_list					{$$=new TreeRecord(@1,NodeName::var_decl,$1,$2);}
	;
var_list:
	var_list_elem					
	|var_list ',' var_list_elem		{$$=new TreeRecord(@1,NodeName::var_list,$1,$3);}
	|ID_NAME '(' proc_expr_list_not_empty ')'
			 {$$=new TreeRecord(@1,NodeName::var_constructor_call,new TreeRecord(@1,$1),$3);}
	;

we got these body statements, we need to get it's size if it's relevant in storage
	only var_decl does matter
	*/
int CodeGeneration::getClassBodyStmtSize(TreeRecord* node){
	if(ProgTree::name_comp(node->node_name,NodeName::var_decl)){
		TreeRecord* list=node->node3;
		if(node->node3==NULL)
			list=node->node2;
		
		if(ProgTree::name_comp(list->node_name,NodeName::var_constructor_call)){
			list->rec->shift=class_shift;
			list->rec->shift_from=2; //denotes shifting from class scope
			class_shift++;
			return 1; //one for address
		}
		
		//var_list_elem|var_list ',' var_list_elem`
		while(true){
			if(ProgTree::name_comp(list->node_name,NodeName::var_list_elem)){
				list->rec->shift=class_shift; //list rec is the var rec in sym tab
				list->rec->shift_from=2; //denotes shifting from class scope
				class_shift++;
				break;
			}
			list->node2->rec->shift=class_shift;
			list->rec->shift_from=2; //denotes shifting from class scope
			class_shift++;
			list=list->node1;
		}
	}
	
	return class_shift;
}

/*
class_destructor:
	class_type SCOPE_DOTS '~' class_type '(' ')' block_stmt		{$$=new TreeRecord(@1,NodeName::class_destructor,$1,$4,$7);}
	;
*/
void CodeGeneration::generateClassDestructorFunDefCode(TreeRecord* node){
//method body
	file<<node->codeName<<":\n";

	//generate parent desstructor call if any
	generateParentDestructorCall(node);

	this->generateBlockStmtCode(node->node3);
	
	file<<"return\n";
}

void CodeGeneration::generateParentDestructorCall(TreeRecord* node){
	TreeRecord* class_def=node->rec->ref_node;
	TreeRecord* base_list=class_def->node3;
	if(base_list==NULL)
		return;
			//now push the object address
	if(base_list->rec!=NULL){
		file<<"pushl -1\n";	
		file<<"pusha "<<base_list->rec->codeName<<"\n";
		file<<"call\n";
				//now pop generated params
		file<<"pop 1\n";
	}
}

/*
class_constructor:
	class_type SCOPE_DOTS class_type '(' func_decl_args ')' class_constructor_init_list block_stmt	
	{$$=new TreeRecord(NodeName::class_constructor,$1,3,$5,$7,$8);}
	;
*/
void CodeGeneration::generateClassConstructorFunDefCode(TreeRecord* node){
//method body
	file<<node->codeName<<":\n";
	generateFunParamsCode(node->node3,1);
	
	// generate code for initialization list
	generateInitList(node->node4);
		
	//generate parent constructor call if any
	generateParentCall(node->node4);
	generateDefaultParentCall(node); //if necessary call parent deafult constructor

	this->generateBlockStmtCode(node->node5);
	
	file<<"return\n";
}


/*

class_constructor_init_list:			
	|':' c_c_init_list					
	|':' class_type '(' proc_expr_list_not_empty ')' 
	;
	
	like a normal class constructor call we do:

	if parent exists and explict parent call didn't exist then we try calling the default
	constructor
*/
void CodeGeneration::generateParentCall(TreeRecord* node){
	if(node->node2==NULL) //empty
		return;
	
	generateProcCallParamsCode(node->node2);
			//now push the object address
	file<<"pushl -1\n";	
	file<<"pusha "<<node->codeName<<"\n";
	file<<"call\n";
			//now pop generated params
	file<<"pop "<<params_call_count+1<<"\n";
}

/*
class_constructor_init_list:			{$$=new TreeRecord(@1,NodeName::class_constructor_init_list);}
	|':' c_c_init_list					{$$=new TreeRecord(@1,NodeName::class_constructor_init_list,$2);}
	|':' class_type '(' proc_expr_list ')' {$$=new TreeRecord(@1,NodeName::class_constructor_init_list,$2,$4);}


c_c_init_list:
	ID_NAME '(' expr ')'				{$$=new TreeRecord(@1,NodeName::c_c_init_list,new TreeRecord(@1,$1),$3);}
	|c_c_init_list ',' ID_NAME '(' expr ')'	{$$=new TreeRecord(@1,NodeName::c_c_init_list,$1,new TreeRecord(@3,$3),$5);}
	;
*/
void CodeGeneration::generateInitList(TreeRecord* node){
	if((node->node1==NULL) || (node->node2!=NULL))
		return;
	TreeRecord* list=node->node1;
	while(true){
		if(list->node3==NULL){ //final elem
			file<<"pushl -1\n";
			this->generateExprCode(list->node2);
			file<<"store "<<list->node1->rec->shift<<"\n";
			return;
		}
		file<<"pushl -1\n";
		this->generateExprCode(list->node3);
		file<<"store "<<list->node2->rec->shift<<"\n";
		list=list->node1;	
	}	
}

/*
class_constructor:
	class_type SCOPE_DOTS class_type '(' func_decl_args ')' class_constructor_init_list block_stmt	
	{$$=new TreeRecord(NodeName::class_constructor,$1,3,$5,$7,$8);}
	;
*/
void CodeGeneration::generateDefaultParentCall(TreeRecord* node){
	if(node->node4->node1!=NULL)
		return; //no need

	TreeRecord* class_def=node->node1->rec->ref_node;
	TreeRecord* base=class_def->node3;
	if(base==NULL)
		return;
	generateProcCallParamsCode(new TreeRecord(0));
			//now push the object address
	file<<"pushl -1\n";	
	file<<"pusha "<<node->node4->codeName<<"\n";
	file<<"call\n";
			//now pop generated params
	file<<"pop "<<params_call_count+1<<"\n";

}


/*
class_func_def_inline:
	storage_spec INLINE class_func_def_common	{$$=new TreeRecord(NodeName::class_func_def_inline,$1,$3);}
	|INLINE class_func_def_common				{$$=new TreeRecord(NodeName::class_func_def_inline,$2);}
	;

class_func_def_normal:
	storage_spec class_func_def_common	{$$=new TreeRecord(NodeName::class_func_def_normal,$1,$2);}
	|class_func_def_common				{$$=new TreeRecord(NodeName::class_func_def_normal,$1);}
	;

class_func_def_common:		
		type class_type SCOPE_DOTS ID_NAME '(' func_decl_args ')'	 block_stmt	
			{$$=new TreeRecord(NodeName::class_func_def_common,$1,$2,new TreeRecord($4),$6,$8);}
		;
*/
void CodeGeneration::generateClassFunDefCode(TreeRecord* node){
	TreeRecord* common_node=NULL;
	if(node->node2==NULL)
		common_node=node->node1;
	else
		common_node=node->node2;

//method body
	file<<common_node->codeName<<":\n";
//for now
	generateFunParamsCode(common_node->node4,1);

	this->generateBlockStmtCode(common_node->node5);	
	
	file<<"return\n";
}

/*
 normal function call code generation
 a function has a body of one or more statements
 for each one of these statements generate the code for it
*/
/*
func_def_inline:
	storage_spec INLINE func_def_common		{$$=new TreeRecord(NodeName::func_def_inline,$1,$3);}
	|INLINE func_def_common					{$$=new TreeRecord(NodeName::func_def_inline,$2);}
	;

func_def_normal:
	storage_spec func_def_common	{$$=new TreeRecord(NodeName::func_def_normal,$1,$2);}
	|func_def_common				{$$=new TreeRecord(NodeName::func_def_normal,$1);}
	;

func_def_common:		
		type ID_NAME '(' func_decl_args ')' 	block_stmt	
			{$$=new TreeRecord(@1,NodeName::func_def_common,$1,new TreeRecord(@2,$2),$4,$6);}
		|type ID_NAME '(' func_decl_args ')' CONST 		block_stmt
			{$$=new TreeRecord(@1,NodeName::func_def_common,$1,new TreeRecord(@2,$2),$4,new TreeRecord(@6,"CONST"),$7);}
		;
*/
void CodeGeneration::generateFunDefCode(TreeRecord* node){
	TreeRecord* common_node=NULL;
	if(node->node2==NULL)
		common_node=node->node1;
	else
		common_node=node->node2;

//method body
	file<<common_node->codeName<<":\n";
//for now
	generateFunParamsCode(common_node->node3,0);

	if(ProgTree::name_comp(common_node->node4->node_name,"CONST")){
		this->generateBlockStmtCode(common_node->node5);
	}
	else{
		this->generateBlockStmtCode(common_node->node4);
	}
	
	file<<"return\n";


}

/*
func_decl_args:				{$$=new TreeRecord(NodeName::func_decl_args);}
	| func_decl_arg_list  	{$$=new TreeRecord(NodeName::func_decl_args,$1);}
	;
func_decl_arg_list: 
	func_decl_arg_elem
	|func_decl_arg_list ',' func_decl_arg_elem	{$$=new TreeRecord(NodeName::func_decl_arg_list,$1,$3);}
	;
func_decl_arg_elem:
	type							{$$=new TreeRecord(NodeName::func_decl_arg_elem,$1);}
	|type ID_NAME					{$$=new TreeRecord(NodeName::func_decl_arg_elem,$1,new TreeRecord($2));}
	|type ID_NAME '=' const_expr	{$$=new TreeRecord(NodeName::func_decl_arg_elem,$1,new TreeRecord($2),$4);}
	;
*/
void CodeGeneration::generateFunParamsCode(TreeRecord* node,int type){
	shift=0; //initialize shift
	params_count=0;
	if(node->node1==NULL)
		return;
	TreeRecord* args=node;
	node=node->node1;
	TreeRecord* temp=NULL;
	while(true){
		//final element
		if(ProgTree::name_comp(node->node_name,NodeName::func_decl_arg_elem)){
			//from type create a push statement, set the shift from fp inside the symbol table
			generateFuncArgElemCode(node,type);
			//update symtab
			++params_count;
			break;
		}
		temp=node->node2;
		generateFuncArgElemCode(temp,type);
		++params_count;
		node=node->node1;
	}

	//now we assign passed values to the params variables
	generateFunAssignParams(args,type);
}

/*
assuming params are passed before the pf we assign such stacked params
to the variables of the params
*/
void CodeGeneration::generateFunAssignParams(TreeRecord* node,int type){
	if(node->node1==NULL)
		return;
	node=node->node1;
	TreeRecord* temp=NULL;
	while(true){
		//final element
		if(ProgTree::name_comp(node->node_name,NodeName::func_decl_arg_elem)){
			//from type create a push statement, set the shift from fp inside the symbol table
			generateFunAssignParamElem(node,type);
			//update symtab
			break;
		}
		temp=node->node2;
		generateFunAssignParamElem(temp,type);
		
		node=node->node1;
	}
}

/*

*/
void CodeGeneration::generateFunAssignParamElem(TreeRecord* node,int type){
	TreeRecord* id=node->node2;
	if(id==NULL)
		return;
	int param_shift=0;
		if(type==1)	
			param_shift=-id->rec->shift - 2; //one for object address
		else
			param_shift=-id->rec->shift - 1;

	file<<"pushl "<<param_shift<<"\n"; //on the opposite side
	file<<"storel "<<id->rec->shift<<"\n";
}

void CodeGeneration::generateTypeCode(TreeRecord* node){
	if(node->node_type==Type::type_void)
		return;

	if(node->node_type==Type::type_int){
		file<<"pushi 0\n";
		return;
	}

	if(node->node_type==Type::type_double){
		file<<"pushf 0.0\n";
		return;
	}

	if(node->node_type==Type::type_bool){
		file<<"pushi 0\n";
		return;
	}

	if(node->node_type==Type::type_char){
		file<<"pushi 0";
		return;
	}

	/*
		use of fixed location pointer variables but different content
		we allocate 0 just to reserve a predefined poisition relative to the fp
	*/
	file<<"alloc 0\n";

}

void CodeGeneration::generateFuncArgElemCode(TreeRecord* node,int type){
	TreeRecord* type_node=node->node1;
	TreeRecord* id_node=node->node2;
	if(type_node->node_type==Type::type_int){
		file<<"pushi 0\n";
		id_node->rec->shift=shift++;
		id_node->rec->shift_from=1;//local
		//update the symbol table record
		return;
	}

	if(type_node->node_type==Type::type_double){
		file<<"pushf 0.0\n";
		id_node->rec->shift=shift++;
		id_node->rec->shift_from=1;//local
		return;
	}

	if(type_node->node_type==Type::type_bool){
		file<<"pushi 0\n";
		id_node->rec->shift=shift++;
		id_node->rec->shift_from=1;//local
		return;
	}

	if(type_node->node_type==Type::type_char){
		file<<"pushi 0";
		id_node->rec->shift=shift++;
		id_node->rec->shift_from=1;//local
		return;
	}

	if(type_node->node_type==Type::pointer_type){
		file<<"alloc 0\n";
		id_node->rec->shift=shift++;
		id_node->rec->shift_from=1;//local
		return;
	}
	
}

/*	
block_stmt:
	'{' '}'		{$$=new TreeRecord(NodeName::block_stmt);}
	|'{'  stmts '}' 
				{ $$=new TreeRecord(NodeName::block_stmt,$2);}	
	;
	this method is only used when func_def is found
*/
void CodeGeneration::generateBlockStmtCode(TreeRecord* node){
	if(node->node1==NULL)
		return;
//	shift=0; //initialize shift
	firstRound=true;
	//init variables
		initVars(node->node1);
	firstRound=false;
	generateStmtsCode(node->node1);
}


/*
stmts:
	stmt		
	|stmts stmt		{$$=new TreeRecord(NodeName::stmts,$1,$2);}
	;
*/
void CodeGeneration::generateStmtsCode(TreeRecord* node){
	if(ProgTree::name_comp(node->node_name,NodeName::stmts)){
		generateStmtsCode(node->node1);
		generateStmtCode(node->node2);
		return;
	}
	generateStmtCode(node);
	return;
}

/*
stmt:
	simple_stmt ';'		
	|compound_stmt		
	;
simple_stmt:
	var_decl		
	|assignment		
	|expr 		
	|jump_stmt      
	|cin_stmt			
	|cout_stmt     
	;
*/
void CodeGeneration::generateStmtCode(TreeRecord* node){
	if(ProgTree::name_comp(node->node_name,NodeName::var_decl)){
		generateVarDeclCode(node);
		return;
	}
	
	if(ProgTree::name_comp(node->node_name,NodeName::cout_stmt)){
		this->generateCoutStmtCode(node->node1); //pass the list

		return;
	}

	if(ProgTree::name_comp(node->node_name,NodeName::cin_stmt)){
		this->generateCinStmtCode(node->node1); //pass the list
		return;
	}

	if(ProgTree::name_comp(node->node_name,NodeName::assignment)){
		this->generateAssignStmtCode(node); 
		return;
	}
	
	if(ProgTree::name_comp(node->node_name,NodeName::jump_stmt)){
		this->generateJumbStmtCode(node); 
		return;
	}

	if(ProgTree::name_comp(node->node_name,NodeName::if_stmt)){
		this->generateIfStmtCode(node); 
		return;
	}

	if(ProgTree::name_comp(node->node_name,NodeName::if_else_stmt)){
		this->generateIfElseStmtCode(node); 
		return;
	}

	if(ProgTree::name_comp(node->node_name,NodeName::while_stmt)){
		this->generateWhileStmtCode(node); 
		return;
	}

	if(ProgTree::name_comp(node->node_name,NodeName::for_stmt)){
		this->generateForStmtCode(node); 
		return;
	}

	if(ProgTree::name_comp(node->node_name,NodeName::compound_block_stmt)){
		TreeRecord* block=node->node1;
		if(block->node1==NULL)
			return;
		this->generateStmtsCode(block->node1); //local block variablea are still?
		return;
	}

	if(ProgTree::name_comp(node->node_name,NodeName::block_stmt)){
		if(node->node1==NULL)
			return;
		this->generateStmtsCode(node->node1); //local block variablea are still?
		return;
	}

	if(ProgTree::name_comp(node->node_name,NodeName::expr_stmt)){
		if(optimize)
			if(node->node1->generateCode)
				this->generateExprStmtCode(node->node1);
			else
				return;
		this->generateExprStmtCode(node->node1);
		
		//else this is a dangling expression
		return;
	}

	if(ProgTree::name_comp(node->node_name,NodeName::delete_stmt)){
		this->generateDeleteStmtCode(node);		
		return;
	}

}

/*
delete_stmt:
	DELETE var_expr		{$$=new TreeRecord(NodeName::delete_stmt,$2);}
	|DELETE '[' ']' var_expr	{$$=new TreeRecord(NodeName::delete_stmt,$4);}
	;
	just push the address and free data (address remains on stack relative to fp
*/
void CodeGeneration::generateDeleteStmtCode(TreeRecord* node){
	if(ProgTree::name_comp(node->node_name,NodeName::delete_stmt)){
		this->generateExprCode(node->node1);
		file<<"free\n";
		return;
	}
}

/*
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
	dangling expressions are not generated
	so only proc_call and part of the unary expr is generated
*/
void CodeGeneration::generateExprStmtCode(TreeRecord* node){
	if(ProgTree::name_comp(node->node_name,NodeName::proc_call)){
		this->generateProcCallStmtCode(node);
		//pop return value, it's dangling one
		if(params_call_count>0)
			file<<"pop 1\n"; //pop return value
	}
	if(ProgTree::name_comp(node->node_name,NodeName::unary_expr))
		this->generateUnaryOpStmtCode(node);
	//else ignored
}

/*
helper function to generate code to load IDName variable without folding it
inside a var_expr
*/
void CodeGeneration::generateIDNameCode(TreeRecord* node){
		if(node->rec->shift_from==1)
			file<<"pushl "<<node->rec->shift<<"\n"; //local var
		else
			file<<"pushg "<<node->rec->shift<<"\n"; //global var
		return;
}

/*
proc_call: 
	ID_NAME '(' proc_expr_list ')'				
		{$$=new TreeRecord(NodeName::proc_call,new TreeRecord($1),$3,NULL,NULL,NULL,Type::proc_call_normal);}			
	|ID_NAME '.' ID_NAME '(' proc_expr_list ')'				
		{$$=new TreeRecord(NodeName::proc_call,new TreeRecord($1),new TreeRecord($3),$5,NULL,NULL,Type::PROC_CLASS_CALL);}		
	|THIS ARROW ID_NAME '(' proc_expr_list ')'							
		{$$=new TreeRecord(NodeName::proc_call,new TreeRecord($3),$5,NULL,NULL,NULL,Type::PROC_THIS_CALL);}
	|class_type SCOPE_DOTS ID_NAME '(' proc_expr_list ')'
		{$$=new TreeRecord(NodeName::proc_call,$1,new TreeRecord($3),$5,NULL,NULL,Type::PROC_CLASS_STATIC_CALL);}		
	; 

*/

void CodeGeneration::generateProcCallStmtCode(TreeRecord* node){
	//init params
	Record* class_rec=NULL;
	int code=*(--node->rec->ret_sig_list.end());
	switch(node->node_type){
		case Type::proc_call_normal:
			//first we reserver place for the return value
			this->generateTypeCode(new TreeRecord(code));
			generateProcCallParamsCode(node->node2);
			file<<"pusha "<<node->rec->codeName<<"\n";
			file<<"call\n";
			//now pop generated params
			if(params_call_count>0)
				file<<"pop "<<params_call_count<<"\n";
			break;
		case Type::PROC_CLASS_CALL:
			//first we reserver place for the return value
			this->generateTypeCode(new TreeRecord(code));
			generateProcCallParamsCode(node->node3);
			//now push the object address
			generateIDNameCode(node->node1);
			//get class name
		///	class_rec=reinterpret_cast<Record*>(code));
			class_rec=reinterpret_cast<Record*>(*(node->node1->rec->sig_list.begin()));
			file<<"pusha "<<node->rec->codeName<<"\n";
			file<<"call\n";
			//now pop generated params
			file<<"pop "<<params_call_count+1<<"\n";			
			break;
		case Type::PROC_CLASS_STATIC_CALL:
	
			break;
		case Type::PROC_THIS_CALL:
			//first we reserver place for the return value
			this->generateTypeCode(new TreeRecord(code));
			generateProcCallParamsCode(node->node2);
			//now push the object address
//			generateIDNameCode(node->node1);
			file<<"pushl -1\n";
			//get class name
			class_rec=node->node1->rec;//reinterpret_cast<Record*>(*(node->node1->rec->sig_list.begin()));
			file<<"pusha "<<node->rec->codeName<<"\n";
			file<<"call\n";
			//now pop generated params
			file<<"pop "<<params_call_count+1<<"\n";	
			break;	
		case Type::PROC_ARROW_CALL:
			//first we reserver place for the return value
			this->generateTypeCode(new TreeRecord(code));
			generateProcCallParamsCode(node->node3);
			//now push the object address
			generateIDNameCode(node->node1);
			//get class name
			class_rec=reinterpret_cast<Record*>(*(node->node1->rec->sig_list.begin()));
			file<<"pusha "<<node->rec->codeName<<"\n";
			file<<"call\n";
			//now pop generated params
			file<<"pop "<<params_call_count+1<<"\n";	
			break;
	}
}

/*
proc_expr_list:						{$$=new TreeRecord(@1,NodeName::proc_expr_list);}
	|proc_expr_list_not_empty		{$$=new TreeRecord(@1,NodeName::proc_expr_list,$1);}		
	;
proc_expr_list_not_empty:
	expr								{$$=new TreeRecord(@1,NodeName::proc_expr_list_not_empty,$1);}
	|proc_expr_list_not_empty ',' expr	{$$=new TreeRecord(@1,NodeName::proc_expr_list_not_empty,$1,$3);}
	;


params are pushed on stack before calling the method
after calling it they are duplicated and compied into the params variables.
we do it recursively to invert the order

return params count
*/
void CodeGeneration::generateProcCallParamsCode(TreeRecord* node){
	params_call_count=0;
	if(node->node1==NULL){ //no arguments
		return;
	}
	else{
		if(ProgTree::name_comp(node->node_name,NodeName::proc_expr_list))
			generateProcCallParamsHelperCode(node->node1);
		else
			generateProcCallParamsHelperCode(node);

	}
}
/*
proc_expr_list_not_empty:
	expr								{$$=new TreeRecord(@1,NodeName::proc_expr_list_not_empty,$1);}
	|proc_expr_list_not_empty ',' expr	{$$=new TreeRecord(@1,NodeName::proc_expr_list_not_empty,$1,$3);}
	;

	return params count
	*/
void CodeGeneration::generateProcCallParamsHelperCode(TreeRecord* node){
			//one final elem
			if(node->node2==NULL){
				this->generateExprCode(node->node1);
				++params_call_count;
				return;
			}
			//list
			generateProcCallParamsHelperCode(node->node1);
			this->generateExprCode(node->node2);
			++params_call_count;
}


/*
unary_expr:	
	  ICR expr					{$$=new TreeRecord(NodeName::unary_expr,$2,NULL,NULL,NULL,NULL,Type::left_icr);}
	| expr ICR 				{$$=new TreeRecord(NodeName::unary_expr,$1,NULL,NULL,NULL,NULL,Type::right_icr);}
	| DECR expr				{$$=new TreeRecord(NodeName::unary_expr,$2,NULL,NULL,NULL,NULL,Type::left_decr);}	
	| expr DECR 			{$$=new TreeRecord(NodeName::unary_expr,$1,NULL,NULL,NULL,NULL,Type::right_decr);}	
	
	remaining unary expressions are ignored
	since it's a statement we don't give a fuck about the order
*/

void CodeGeneration::generateUnaryOpStmtCode(TreeRecord* node){
	int type=*(node->node1->sig_list.begin());
	switch(node->node_type){
		case Type::left_decr:
			this->generateExprCode(node->node1);
			if(type==Type::type_int){
				file<<"pushi 1\n";
				file<<"sub\n";
			}
			if(type==Type::type_double){
				file<<"pushf 1.0\n";
				file<<"sub\n";
			}
			this->generateStoreInVarExprCode(node->node1);
			//pointer data also must be handled
			break;
		case Type::right_icr:
			this->generateExprCode(node->node1);
			if(type==Type::type_int){
				file<<"pushi 1\n";
				file<<"add\n";
			}
			if(type==Type::type_double){
				file<<"pushf 1.0\n";
				file<<"add\n";
			}
			this->generateStoreInVarExprCode(node->node1);
			break;
		case Type::left_icr:
			this->generateExprCode(node->node1);
			if(type==Type::type_int){
				file<<"pushi 1\n";
				file<<"add\n";
			}
			if(type==Type::type_double){
				file<<"pushf 1.0\n";
				file<<"add\n";
			}
			this->generateStoreInVarExprCode(node->node1);
			break;
		case Type::right_decr:
			this->generateExprCode(node->node1);
			if(type==Type::type_int){
				file<<"pushi 1\n";
				file<<"sub\n";
			}
			if(type==Type::type_double){
				file<<"pushf 1.0\n";
				file<<"sub\n";
			}
			this->generateStoreInVarExprCode(node->node1);
			break;
	}

}

/*
if_stmt:
	IF '(' expr ')'  stmt			{$$=new TreeRecord(@1,NodeName::if_stmt,$3,$5);}			
	|IF '(' expr ')'  stmt ELSE stmt				{$$=new TreeRecord(@1,NodeName::if_else_stmt,$3,$5,$7);}		
	;
*/

//just as it's name say to be used by named statements
char* CodeGeneration::generateRandLabel(char* mark,int label){
	char* num=new char[15];
	char* str=new char[50];
	itoa(label,num,10);
	strcpy(str,mark);
	strcat(str,num);
	return str;
}

void CodeGeneration::generateIfStmtCode(TreeRecord* node){
	if(optimize){
		if(node->node1->isConstData){ //condition known
			int val=ProgTree::getMaxVal(node->node1);
			if(val==0)
				return; //no need to generate
			else
				this->generateStmtCode(node->node2); //enter anyway
			return;
		}
	}

	//put value of expr on the stack
	int label=++labelCount;
	this->generateExprCode(node->node1);
	char* endif=this->generateRandLabel("endif",label);
	file<<"jz "<<endif<<"\n"; //check for unique labeling
	this->generateStmtCode(node->node2);
	file<<endif<<":\n";
	delete []endif;
	
}

void CodeGeneration::generateIfElseStmtCode(TreeRecord* node){
	if(optimize){
		if(node->node1->isConstData){ //condition known
			int val=ProgTree::getMaxVal(node->node1);
			if(val==0)
				this->generateStmtCode(node->node3); //else
			else
				this->generateStmtCode(node->node2); //enter if
			return;
		}
	}
	int label=++labelCount;
	this->generateExprCode(node->node1);
	char* endif=this->generateRandLabel("endif",label);
	char* elseLabel=this->generateRandLabel("else",label);
	file<<"jz "<<elseLabel<<"\n"; //check for unique labeling
	this->generateStmtCode(node->node2);
	file<<"jump "<<endif<<"\n";
	file<<elseLabel<<":\n";
	this->generateStmtCode(node->node3);
	file<<endif<<":\n";
	delete []endif;
	delete []elseLabel;
	
}

/*
for_stmt:
	FOR '(' var_decl ';' expr ';' for_itr')' block_stmt		{$$=new TreeRecord(NodeName::for_stmt,$3,$5,$7,$9);}
	;
*/
void CodeGeneration::generateForStmtCode(TreeRecord* node){

	if(optimize){
		if(node->node2->isConstData){ //condition known
			int val=ProgTree::getMaxVal(node->node2);
			if(val==0) //false
				return; //else
		}

		//now check the possibility of unrolling the loop
		if(unrollFor(node))
			return;
	}

	int label=++labelCount;
	char* forLabel=this->generateRandLabel("for",label);
	char* endFor=this->generateRandLabel("endFor",label);
	
	
	currentEndLabel=endFor;
	currentStartLabel=forLabel;
	this->generateVarDeclCode(node->node1);
	file<<forLabel<<":\n";
	this->generateExprCode(node->node2);
	file<<"jz "<<endFor<<"\n";
	this->generateForItrCode(node->node3);
	this->generateStmtCode(node->node4);

	//incase they were changed
	currentEndLabel=endFor;
	currentStartLabel=forLabel;

	file<<"jump "<<forLabel<<"\n";
	file<<endFor<<":\n";

	delete []forLabel;
	delete []endFor;
	
}
/*
for_itr:
	for_itr_elem				{$$=new TreeRecord(NodeName::for_itr,$1);}
	|for_itr ',' for_itr_elem	{$$=new TreeRecord(NodeName::for_itr,$1,$3);}
	;

for_itr_elem:
	expr
	|assignment
	;
*/
void CodeGeneration::generateForItrCode(TreeRecord* node){
	if(node->node2==NULL)
		node=node->node1;

		while(true){
			//final elem
			if(ProgTree::name_comp(node->node_name,NodeName::assignment)){
				this->generateAssignStmtCode(node);
				return;
			}
			else
			if(ProgTree::name_comp(node->node_name,NodeName::for_itr_elem_expr)){
				this->generateExprCode(node->node1);
				return;
			}

			if(ProgTree::name_comp(node->node2->node_name,NodeName::assignment)){
				this->generateAssignStmtCode(node->node2);
			}
			else
				if(ProgTree::name_comp(node->node2->node_name,NodeName::for_itr_elem_expr)){
					this->generateExprCode(node->node2);
				}					
			node=node->node1;
			}	
}

/*
we unroll for loop in a very narrow case which is
	for(int i=0; i<10; i++){...}
	is rolled into 10 iterations

	for(int i=INITIAL; i COND LIMIT; [++i,--i,i++,i--]){ ...}
	i must not change inside the for body and that means: it should not
	be the left side of an assignment statement.

	we assume that the for head will tend to make the loop ends
	so we take this rule [ abs[ INITIAL - LIMIT] ] as the iteration count
	which must be less that 20.

for_stmt:
	FOR '(' var_decl ';' expr ';' for_itr')' block_stmt		{$$=new TreeRecord(NodeName::for_stmt,$3,$5,$7,$9);}
	;

for_itr:
	for_itr_elem				{$$=new TreeRecord(NodeName::for_itr,$1);}
	|for_itr ',' for_itr_elem	{$$=new TreeRecord(NodeName::for_itr,$1,$3);}
	;
for_itr_elem:
	expr
	|assignment
	;

*/
bool CodeGeneration::unrollFor(TreeRecord* node){
	double init=0;
	double limit=0;
	//get initial cond
	TreeRecord* var_decl=node->node1;
	TreeRecord* init_node=var_decl->node2->node2;
	if(init_node==NULL)
		return false;
	if(init_node->isConstData)
		init=ProgTree::getMaxVal(init_node);
	else
		return false;

	TreeRecord* cond=node->node2;
	if(!ProgTree::name_comp(cond->node_name,NodeName::logical_operation_expr))
		return false;
	//what ever the operation we take the limit as the second node
	TreeRecord* limit_node=cond->node2;
	if(!limit_node->isConstData)
		return false;
	
	limit=ProgTree::getMaxVal(limit_node);

	int itrs=abs(init-limit);
	
	//see how to increment limit or decrement it due > < >= <= == !=
	switch(cond->node_type){
		case Type::GREATER:
		case Type::LESS:
			--itrs;
			break;
		case Type::GEQ:
		case Type::LEQ:
			break;
		case Type::EQL:
		case Type::NEQ:
			return false;			
		default:
			return false; //permit for no other
	}

	if(itrs>50)
		return false;
	TreeRecord* itr_node=node->node3;
	if(itr_node->node2!=NULL) //one itr is permitted
		return false;
	TreeRecord* itr_elem=itr_node->node1->node1;

	//tell what kind this loop is
	bool increment=false;
	bool decrement=false;
	bool left=false;
	bool right=false;

	if(ProgTree::name_comp(itr_elem->node_name,NodeName::unary_expr)){
		switch(itr_elem->node_type){
			case Type::left_icr:
				increment=true;
				left=true;
				break;
			case Type::right_icr:
				increment=true;
				right=true;
				break;
			case Type::left_decr:
				decrement=true;
				left=true;	
				break;
			case Type::right_decr:
				decrement=true;
				right=true;
				break;
			default:
				return false;
		}
	}
	else
		return false;
	
	//check that i wasn't internally changed
	this->generateVarDeclCode(node->node1);
	for(int i=0;i<=itrs;i++){
		file<<"\n";
		if(left)
			this->generateExprCode(itr_elem);
		this->generateStmtCode(node->node4);
		if(right)
			this->generateExprCode(itr_elem);
	}

	return true; //unrolled
}

/*
while_stmt:
	WHILE '(' expr ')' stmt			{$$=new TreeRecord(@1,NodeName::while_stmt,$3,$5);}
	; 
*/
void CodeGeneration::generateWhileStmtCode(TreeRecord* node){
	if(optimize){
		if(node->node1->isConstData){ //condition known
			int val=ProgTree::getMaxVal(node->node1);
			if(val==0)
				return; //no need to generate
		}
	}

	int label=++labelCount;
	char* whileLabel=this->generateRandLabel("while",label);
	char* endWhile=this->generateRandLabel("endWhile",label);
	

	currentEndLabel=endWhile;
	currentStartLabel=whileLabel;

	file<<whileLabel<<":\n";
	this->generateExprCode(node->node1);
	file<<"jz "<<endWhile<<"\n";
	this->generateStmtCode(node->node2);

	//incase of chaning
	currentEndLabel=endWhile;
	currentStartLabel=whileLabel;

	file<<"jump "<<whileLabel<<"\n";
	file<<endWhile<<":\n";

	delete []whileLabel;
	delete []endWhile;
	
}

void CodeGeneration::generateCBlockStmtCode(TreeRecord* node){
	this->generateBlockStmtCode(node->node1);
}


/*
jump_stmt:
	BREAK		{$$=new TreeRecord(@1,NodeName::jump_stmt,NULL,NULL,NULL,NULL,NULL,Type::break_type);}
	|RETURN expr	{$$=new TreeRecord(@1,NodeName::jump_stmt,$2,NULL,NULL,NULL,NULL,Type::return_expr_type);}
	|CONTINUE		{$$=new TreeRecord(@1,NodeName::jump_stmt,NULL,NULL,NULL,NULL,NULL,Type::continue_type);}
	|RETURN			{$$=new TreeRecord(@1,NodeName::jump_stmt,NULL,NULL,NULL,NULL,NULL,Type::return_type);}
	;
*/
void CodeGeneration::generateJumbStmtCode(TreeRecord* node){
	switch(node->node_type){
		case Type::return_expr_type:
			//set the return variable value
			this->generateExprCode(node->node1);
			//store return value in it's place
			if(class_fucn)
				file<<"storel "<<(-params_count-2)<<"\n"; 
			else
				file<<"storel "<<(-params_count-1)<<"\n";
			file<<"return\n";
			break;
		case Type::continue_type:
			file<<"jump "<<currentStartLabel<<"\n";
			break;
		case Type::break_type:
			file<<"jump "<<currentEndLabel<<"\n";
			break;
		case Type::return_type:
			file<<"return\n";
			break;
	}
}

/*
assignment: 
	var_expr '=' expr			{$$=new TreeRecord(@1,NodeName::assignment,$1,$3,NULL,NULL,NULL,Type::ASSIGN_NORMAL);}
	|var_expr PLUS_EQ expr		{$$=new TreeRecord(@1,NodeName::assignment,$1,$3,NULL,NULL,NULL,Type::ASSIGN_PLUS);}
	|var_expr MINUS_EQ expr		{$$=new TreeRecord(@1,NodeName::assignment,$1,$3,NULL,NULL,NULL,Type::ASSIGN_MINUS);}
	|var_expr DIV_EQ expr		{$$=new TreeRecord(@1,NodeName::assignment,$1,$3,NULL,NULL,NULL,Type::ASSIGN_DIV);}
	|var_expr MULT_EQ expr		{$$=new TreeRecord(@1,NodeName::assignment,$1,$3,NULL,NULL,NULL,Type::ASSIGN_MULT);}
	;
*/
void CodeGeneration::generateAssignStmtCode(TreeRecord* node){
	//load address for complex structures
	this->generateLoadVarExprAddressCode(node->node1);
	switch(node->node_type){
		case Type::ASSIGN_NORMAL:
			//calculate values
			this->generateExprCode(node->node2);
			generateCastCode(node->node1,node->node2);
			break;
			case Type::ASSIGN_PLUS:
				//load current values, add it to the expression
				this->generateExprCode(node->node1);
				//calculate values
				this->generateExprCode(node->node2);
				//convert type if necessary
				generateCastCode(node->node1,node->node2);
				//generate operation command
				this->generateMathOperationCode(node->node1,node->node2,Type::PLUS);
			break;
		case Type::ASSIGN_MINUS:
				//node1-node2	
				this->generateExprCode(node->node1);
				//calculate values
				this->generateExprCode(node->node2);
				generateCastCode(node->node1,node->node2);
				//generate operation command
				this->generateMathOperationCode(node->node1,node->node2,Type::MINUS);
			break;
		case Type::ASSIGN_DIV:
				this->generateExprCode(node->node1);
				//calculate values
				this->generateExprCode(node->node2);
				generateCastCode(node->node1,node->node2);
				//generate operation command
				this->generateMathOperationCode(node->node1,node->node2,Type::DIV);
			break;
		case Type::ASSIGN_MULT:
				this->generateExprCode(node->node1);
				//calculate values
				this->generateExprCode(node->node2);
				generateCastCode(node->node1,node->node2);
				//generate operation command
				this->generateMathOperationCode(node->node1,node->node2,Type::MULT);
			break;
	}
	
	this->generateStoreInVarExprCode(node->node1);
}

/*
 convert node2 type to node1 type if type mismatches
*/

void CodeGeneration::generateCastCode(TreeRecord* node1,TreeRecord* node2){
	if(node1->sig_list==node2->sig_list)
		return; //no need, types are the same
	int type=*(node1->sig_list.begin());
	int type2=*(node2->sig_list.begin());
	switch(type){
		case Type::type_double:
			if((type2==Type::type_bool)||(type2==Type::type_int)||(type2==Type::type_char))
				file<<"itof\n";
			break;
		case Type::type_int:
			if((type2==Type::type_bool)||(type2==Type::type_char))
				break; //bool and char are int in the vm
			if(type2==Type::type_double)
				file<<"ftoi\n";
	}
}


/*
	assignment or opertion nodes we generate the appropriate math op
*/
void CodeGeneration::generateMathOperationCode(TreeRecord* node1,TreeRecord* node2,int op){
	//get type, balancing is already done.
	int type=*(node1->sig_list.begin());
	switch(op){
		case Type::PLUS:
			if(type==Type::type_double)
				file<<"fadd\n";
			else
				file<<"add\n";

			break;
		case Type::MINUS:
			if(type==Type::type_double)
				file<<"fsub\n";
			else
				file<<"sub\n";
			break;
		case Type::MULT:
			if(type==Type::type_double)
				file<<"fmul\n";
			else
				file<<"mul\n";
			break;
		case Type::DIV:
			if(type==Type::type_double)
				file<<"fdiv\n";
			else
				file<<"div\n";
			break;
	}
}

/*
cout_stmt_output_list:
	output_elem
	|cout_stmt_output_list output_elem	{$$=new TreeRecord(@1,NodeName::cout_stmt_output_list,$1,$2);}
	;
output_elem:
	OUTPUT_VAR_SYM expr		{$$=new TreeRecord(@1,NodeName::output_elem,$2);}
	;
issue write for the list
node here is the list, we do it recursively to flip the order of printing back

we must concatenate input into one string, so we make a convert then concatenate
after each two are concatenated writes is output outside this method call
*/
void CodeGeneration::generateCoutStmtCode(TreeRecord* list){
	/*
		if(list->node2==NULL){
			this->generateExprCode(list->node1);
			generateWriteCode(list->node1); //check type to see which write
		}
		else{
			generateCoutStmtCode(list->node1);	
			this->generateExprCode(list->node2->node1);
			generateWriteCode(list->node2->node1);

			//concatenate them into one
			file<<"concat\n";
		}
		*/
	file<<"pushs \"\"	\n";
	while(true){
		if(list->node2==NULL){
			this->generateExprCode(list->node1);
			generateWriteCode(list->node1); //check type to see which write
			file<<"concat\n";
			break;
		}
		
		this->generateExprCode(list->node2->node1);
		generateWriteCode(list->node2->node1);
		
		list=list->node1;
		file<<"concat\n";
	}

	//issue one writes command for all concatenated strings
	file<<"writes\n";
}

void CodeGeneration::generateWriteCode(TreeRecord* node){
	int code=*(node->sig_list.begin());
	int codeEnd=*(--node->sig_list.end());
	if((code==Type::type_int)||(code==Type::type_bool)){
	//	file<<"writei\n";
		file<<"STRI\n";
		return;
	}

	if((code==Type::type_char)&&(codeEnd==code)){
	//	file<<"writei\n";
		file<<"STRI\n";	
		return;
	}
	if((code==Type::type_char)&&(codeEnd==Type::pointer_type)){
	//	file<<"writes\n";
//		file<<"STRI\n";
		return;
	}

	if(code==Type::type_double){
	//	file<<"writef\n";
		file<<"STRF\n";
		return;
	}
}


/*
cin_stmt_input_list:
	input_elem
	|cin_stmt_input_list input_elem	{$$=new TreeRecord(@1,NodeName::cin_stmt_input_list,$1,$2);}
	;
input_elem:
	INPUT_VAR_SYM var_expr	{$$=new TreeRecord(@1,NodeName::input_elem,$2);}
	;
*/
void CodeGeneration::generateCinStmtCode(TreeRecord* list){	
		if(list->node2==NULL){
		//	this->generateExprCode(list->node1);
			generateReadCode(list->node1); //check type to see which write
		}
		else{
			generateCinStmtCode(list->node1);	
		//	this->generateExprCode(list->node2->node1);
			generateReadCode(list->node2->node1);
		}
}

//passing an var_expr
void CodeGeneration::generateReadCode(TreeRecord* node){
	int code=*(node->sig_list.begin());
	this->generateLoadVarExprAddressCode(node); //for complex types
	file<<"read\n";
	//convert and store back
	if((code==Type::type_int)||(code==Type::type_bool)){
		file<<"atoi\n";
		generateStoreInVarExprCode(node);
		return;
	}

	if(code==Type::type_char){
		file<<"atoi\n";
		generateStoreInVarExprCode(node);	
		return;
	}
	if(code==Type::type_str){
		generateStoreInVarExprCode(node);
		return;
	}

	if(code==Type::type_double){
		file<<"atof\n";
		generateStoreInVarExprCode(node);
		return;
	}
	
}

/*
var_decl:
	storage_spec type var_list		{$$=new TreeRecord(NodeName::var_decl,$1,$2,$3);}
	|type var_list					{$$=new TreeRecord(NodeName::var_decl,$1,$2);}
	;
var_list_elem:
	ID_NAME						 
			 { $$=new TreeRecord(NodeName::var_list_elem,new TreeRecord($1));}
	|ID_NAME array_dims			
			  {$$=new TreeRecord(NodeName::var_list_elem,new TreeRecord($1),$2);}
	|ID_NAME '=' expr			
			 {$$=new TreeRecord(NodeName::var_list_elem,new TreeRecord($1),$3);}
	|ID_NAME array_dims '=' array_init_list	 
			{$$=new TreeRecord(NodeName::var_list_elem,new TreeRecord($1),$2,$4);}  
	;
var_list:
	var_list_elem					{$$=new TreeRecord(NodeName::var_list,$1);}
	|var_list ',' var_list_elem		{$$=new TreeRecord(NodeName::var_list,$1,$3);}
	|ID_NAME '(' proc_expr_list_not_empty ')'
			 {$$=new TreeRecord(NodeName::var_constructor_call,new TreeRecord($1),$3);}
	;

	since we already initialized so we just update by storing value if assigned expressions exists
*/
void CodeGeneration::generateVarDeclCode(TreeRecord* node){
		TreeRecord* type_node;
		TreeRecord* list_node;
		if(node->node3){
			type_node=node->node2;
			list_node=node->node3;
		}
		else{
			type_node=node->node1;
			list_node=node->node2;
		}

		//now check the list

		/*case when the list is a single constructor call with parameters:	A a(4);
		  assuming initialization was done, here we call the constructor function nothing more
		  this call is enough to do the work
		  we push the object address before the fp of the function call
		  so we call load or store variables in this object
		*/
		if(ProgTree::name_comp(list_node->node_name,NodeName::var_constructor_call)){
			if(optimize && ! list_node->rec->isUsed){
				ErrorReport::printHint(list_node->line_num,list_node->col_num,"variable declared but never used");
				return;
			}
			
			//get class variable size 
			Record r(type_node->node_name,symtab.first_scope->depth+1,symtab.first_scope,Type::class_type);
			Record* class_rec=symtab.findRecord(r); //always succeed
			if(class_rec==NULL){
				//error
				return;
			}
			//now call constructor function
			//first we reserver place for the return value
		//	this->generateTypeCode(new TreeRecord(*(node->rec->sig_list.begin())));
	
			generateProcCallParamsCode(list_node->node2);
			//now push the object address
			this->generateIDNameCode(list_node->node1);
			file<<"pusha "<<list_node->codeName<<"\n";
			file<<"call\n";
			//now pop generated params
			file<<"pop "<<params_call_count+1<<"\n";	

			return;
		}
		
		//var_list_elem|var_list ',' var_list_elem`
		while(true){
			if(ProgTree::name_comp(list_node->node_name,NodeName::var_list_elem)){
				generateVarListElemCode(list_node,type_node);
				break;
			}
			generateVarListElemCode(list_node->node2,type_node);
			list_node=list_node->node1;
		}

		return;	
}




/*
var_list_elem:
	ID_NAME						 
			 { $$=new TreeRecord(NodeName::var_list_elem,new TreeRecord($1));}
	|ID_NAME array_dims			
			  {$$=new TreeRecord(NodeName::var_list_elem,new TreeRecord($1),$2);}
	|ID_NAME '=' expr			
			 {$$=new TreeRecord(NodeName::var_list_elem,new TreeRecord($1),$3);}
	|ID_NAME array_dims '=' array_init_list	 
			{$$=new TreeRecord(NodeName::var_list_elem,new TreeRecord($1),$2,$4);}  
	;

	here we just update by assigning values nothing more

	*/
void CodeGeneration::generateVarListElemCode(TreeRecord* elem_node,TreeRecord* type_node){
	if(optimize && ! elem_node->rec->isUsed){		
		ErrorReport::printHint(elem_node->line_num,elem_node->col_num,"variable declared but never used");
		return;
	}

	if(elem_node->node2==NULL){
		return ;
	}
	
	if(ProgTree::name_comp(elem_node->node2->node_name,NodeName::array_dims)||ProgTree::name_comp(elem_node->node2->node_name,NodeName::array_dim)){
		// we got array variable decleration,
		int size=0;
		size=((ArrayInfo)*elem_node->rec->array_info.begin()).dim_size;
 		if(elem_node->rec->array_info.size()>1)
			size*=((ArrayInfo)*(++elem_node->rec->array_info.begin())).dim_size;
		file<<"alloc "<<size<<"\n";
		generateStoreInIDCode(elem_node->node1);			
	}
	else{
			//now get the shift value
		//	file<<"pushfp\n";
//			this->generateLoadVarExprAddressCode(elem_node->node1);
			generateExprCode(elem_node->node2);
			type_node->sig_list=elem_node->node1->rec->sig_list;
			this->generateCastCode(type_node,elem_node->node2);
	//		this->generateStoreInVarExprCode(elem_node->node1);
			if(elem_node->node1->rec->shift_from==1)
				file<<"storel "<<elem_node->node1->rec->shift<<"\n"; //for now
			else
				file<<"storeg "<<elem_node->node1->rec->shift<<"\n"; //for now		
			return ;
	}
}


/*
 helper function for generateLoadVarExprAddressCode for example:
 a.x->d=0;
 so generateLoadVarExprAddressCode loads the address and this function generates loads
 before d that is the load shift for x
 then the store function will store the value in the d shift
*/
void CodeGeneration::generateLoadVarExprHelper(TreeRecord* node){
	//ID_NAME '.' var_expr    no other check made for now
	if(ProgTree::name_comp(node->node_name,NodeName::class_var_expr)){
		file<<"load "<<node->node1->rec->shift<<"\n";
		generateLoadVarExprHelper(node->node2);
		return;

	}
	//ID_NAME ARROW var_expr
	if(ProgTree::name_comp(node->node_name,NodeName::id_arrow_expr)){
		file<<"load "<<node->node1->rec->shift<<"\n";
		generateLoadVarExprHelper(node->node2);
		return;
	}

//	if(ProgTree::name_comp(node->node_name,NodeName::id_name_expr)){
//		file<<"load "<<node->rec->shift<<"\n";
//		return;
//	}
}
/*
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
	|ID_NAME ARROW var_expr		 
				{$$=new TreeRecord(@1,NodeName::id_arrow_expr,new TreeRecord(@1,$1),$3);}
	|THIS ARROW var_expr	{$$=new TreeRecord(@1,NodeName::this_arrow_expr,$3);}
	
	;	

passing an expr we load it's address( it has to be var_expr)
simple types have no address just a shift from the pf so we load nothing and handle this in the store comman
we load for preparation of a store command so if we got a.x we load a address so 
we can store in x position
*/
void CodeGeneration::generateLoadVarExprAddressCode(TreeRecord* node){

/*
array_expr:
	ID_NAME array_expr_dims	{$$=new TreeRecord(@1,NodeName::array_expr,new TreeRecord(@1,$1),$2);}
	;
array_expr_dims:
	'[' expr ']'	{$$=new TreeRecord(@1,NodeName::array_expr_dims,$2);}
	|array_expr_dims '[' expr ']'	{$$=new TreeRecord(@1,NodeName::array_expr_dims,$1,$3);}
	;
*/
	if(ProgTree::name_comp(node->node_name,NodeName::array_expr)){	
		this->generateIDNameCode(node->node1);
		TreeRecord* dims=node->node2;
		if(dims->node2!=NULL){		
			this->generateExprCode(dims->node1->node1);
			int sec=((ArrayInfo)*(++node->rec->array_info.begin())).dim_size;
			file<<"pushi "<<sec<<"\n";
			file<<"mul\n";
			this->generateExprCode(dims->node2);
			file<<"add\n";
		}
		else
			this->generateExprCode(dims->node1);
		return;
	}
	/*array_expr '.' var_expr
		array_expr must be of class type
	*/
	if(ProgTree::name_comp(node->node_name,NodeName::array_class_expr)){
		this->generateLoadVarExprAddressCode(node->node1);
		generateLoadVarExprHelper(node->node2);		
		return;

	}

	/*array_expr ARROW var_expr
		array_expr must be a pointer to a class type
	*/
	if(ProgTree::name_comp(node->node_name,NodeName::array_pointer_expr)){
		this->generateLoadVarExprAddressCode(node->node1);
		generateLoadVarExprHelper(node->node2);		
		return;
	}


	/*
	pointer_val_expr:
	'*' var_expr	{$$=new TreeRecord(@1,NodeName::pointer_val_expr,$2);}	
	;
	*/
	if(ProgTree::name_comp(node->node_name,NodeName::pointer_val_expr)){
		//push the address first
	/*	if(node->rec->shift_from==1) //local 
			file<<"pushl "<<node->rec->shift<<"\n";
		if(node->rec->shift_from==0) //global
			file<<"pushg "<<node->rec->shift<<"\n";
		if(node->rec->shift_from==2) //class
			file<<"pushl -1\n";
		generateLoadVarExprHelper(node->node1);
	*/	
	//	this->generateLoadVarExprAddressCode(node->node1);
		this->generateExprCode(node->node1); //we need to load everything
		return;
	}

	//THIS
	if(ProgTree::name_comp(node->node_name,NodeName::this_expr)){
		file<<"pushl -1\n";
		return;
	}	

	//ID_NAME '.' var_expr    no other check made for now
	if(ProgTree::name_comp(node->node_name,NodeName::class_var_expr)){
		if(node->rec->shift_from==1) //local 
			file<<"pushl "<<node->rec->shift<<"\n";
		else
			file<<"pushg "<<node->rec->shift<<"\n";
		generateLoadVarExprHelper(node->node2);
		return;

	}

	//class_type SCOPE_DOTS ID_NAME
	if(ProgTree::name_comp(node->node_name,NodeName::scope_expr)){
		return;
	}

	//ID_NAME ARROW var_expr
	if(ProgTree::name_comp(node->node_name,NodeName::id_arrow_expr)){
		if(node->rec->shift_from==1) //local 
			file<<"pushl "<<node->rec->shift<<"\n";
		if(node->rec->shift_from==0)
			file<<"pushg "<<node->rec->shift<<"\n";
		if(node->rec->shift_from==2)
			file<<"pushl -1\n";
		generateLoadVarExprHelper(node->node2);
		return;
	}

	//THIS ARROW var_expr, no other check made
	if(ProgTree::name_comp(node->node_name,NodeName::this_arrow_expr)){
		file<<"pushl -1\n";
		generateLoadVarExprHelper(node->node1);
		return;
	}	

	//for members variables inside a class function code.
	if(ProgTree::name_comp(node->node_name,NodeName::id_name_expr)){
		if(node->rec->shift_from==2)
			file<<"pushl -1"<<"\n"; //we push object address

		return;
	}

}


/*
 just to store in the last shift
*/
void CodeGeneration::generateStoreInVarExprHelper(TreeRecord* node){
	if(ProgTree::name_comp(node->node_name,NodeName::id_name_expr)){
			file<<"store "<<node->rec->shift<<"\n";
		return;
	}
	//ID_NAME '.' var_expr    no other check made for now
	if(ProgTree::name_comp(node->node_name,NodeName::class_var_expr)){
		this->generateStoreInVarExprHelper(node->node2);
		return;
	}
	//ID_NAME ARROW var_expr
	if(ProgTree::name_comp(node->node_name,NodeName::id_arrow_expr)){
//		file<<"store "<<node->node2->rec->shift<<"\n";
		this->generateStoreInVarExprHelper(node->node2);
		return;
	}

	//THIS ARROW var_expr, no other check made
	if(ProgTree::name_comp(node->node_name,NodeName::this_arrow_expr)){
		this->generateStoreInVarExprHelper(node->node1);
		return;
	}	

}

/* store for ids */
void CodeGeneration::generateStoreInIDCode(TreeRecord* node){
		if(node->rec->shift_from==1) //local 
			file<<"storel "<<node->rec->shift<<"\n";
		if(node->rec->shift_from==0)
			file<<"storeg "<<node->rec->shift<<"\n";
		if(node->rec->shift_from==2)
			file<<"store "<<node->rec->shift<<"\n";
}

/* issue a store command */
void CodeGeneration::generateStoreInVarExprCode(TreeRecord* node){
	//store relatively to pf
	if(ProgTree::name_comp(node->node_name,NodeName::id_name_expr)){
		if(node->rec->shift_from==1) //local 
			file<<"storel "<<node->rec->shift<<"\n";
		if(node->rec->shift_from==0)
			file<<"storeg "<<node->rec->shift<<"\n";
		if(node->rec->shift_from==2)
			file<<"store "<<node->rec->shift<<"\n";

		return;
	}
/*
array_expr:
	ID_NAME array_expr_dims	{$$=new TreeRecord(@1,NodeName::array_expr,new TreeRecord(@1,$1),$2);}
	;
array_expr_dims:
	'[' expr ']'	{$$=new TreeRecord(@1,NodeName::array_expr_dims,$2);}
	|array_expr_dims '[' expr ']'	{$$=new TreeRecord(@1,NodeName::array_expr_dims,$1,$3);}
	;
*/
	//we store in the loaded address
	if(ProgTree::name_comp(node->node_name,NodeName::array_expr)){
		TreeRecord* dims=node->node2;
		//assuming two dims max
		file<<"storen\n";
		return;
		
	}
	/*array_expr '.' var_expr
		array_expr must be of class type
	*/
	if(ProgTree::name_comp(node->node_name,NodeName::array_class_expr)){
		this->generateStoreInVarExprCode(node->node2);
		return;

	}

	/*array_expr ARROW var_expr
		array_expr must be a pointer to a class type
	*/
	if(ProgTree::name_comp(node->node_name,NodeName::array_pointer_expr)){
		this->generateStoreInVarExprCode(node->node2);
		return;
	}


	/*
	pointer_val_expr:
	'*' var_expr	{$$=new TreeRecord(@1,NodeName::pointer_val_expr,$2);}	
	;
	*/
	if(ProgTree::name_comp(node->node_name,NodeName::pointer_val_expr)){
		//assuming load is done before expr is called
		file<<"store 0\n";
		return;
	}

	//THIS
	if(ProgTree::name_comp(node->node_name,NodeName::this_expr)){
		return;
	}	

	//ID_NAME '.' var_expr    no other check made for now
	if(ProgTree::name_comp(node->node_name,NodeName::class_var_expr)){
	//	file<<"store "<<node->node2->rec->shift<<"\n";
	//	this->generateStoreInVarExprHelper(node->node2);
		this->generateStoreInVarExprCode(node->node2);
		return;

	}

	//class_type SCOPE_DOTS ID_NAME
	if(ProgTree::name_comp(node->node_name,NodeName::scope_expr)){
		return;
	}

	//ID_NAME ARROW var_expr
	if(ProgTree::name_comp(node->node_name,NodeName::id_arrow_expr)){
//		file<<"store "<<node->node2->rec->shift<<"\n";
//		this->generateStoreInVarExprHelper(node->node2);
			this->generateStoreInVarExprCode(node->node2);
		return;
	}

	//THIS ARROW var_expr, no other check made
	if(ProgTree::name_comp(node->node_name,NodeName::this_arrow_expr)){
	//	this->generateStoreInVarExprHelper(node->node1);
		this->generateStoreInVarExprCode(node->node1);
		return;
	}		
}
/*
	we'll search the const string for \n or \t sequences and replace them by one character
	or two appropriate
	here we'll replace \n with 32 followed by 10 as ascii (space followed by new line)
	\t is replaced by 32 followed by 9 ( space then tab)
	we use space just not to make concats
*/
void CodeGeneration::modifyString(char* str){
	int len=strlen(str);
	int i=0;
	while(i<len){
		if(str[i]=='\\'){
			++i;
			if(i<len)
			if(str[i]=='n'){
				str[i]=10;
				str[i-1]=32;
			}
			if(str[i]=='t'){
				str[i]=9;
				str[i-1]=32	;
			}
			if(str[i]=='\"'){
				str[i]='\'';
				str[i-1]=32	;
			}
		}
		++i;
	}
}

/*
passing a constant expr node, we generate the right push
*/
void CodeGeneration::generateConstCode(TreeRecord* node){
	int type=*node->sig_list.begin();
	switch(type){
		case Type::type_int:
			file<<"pushi "<<node->val.i_val<<"\n";
			break;
		case Type::type_double:
			file.setf(file.showpoint);
			file<<"pushf "<<node->val.d_val<<"\n";
			break;
		case Type::type_bool:
			file<<"pushi "<<node->val.b_val<<"\n";
			break;
		case Type::type_char:
			file<<"pushi "<<node->val.c_val<<"\n";
			break;
		case Type::type_str:
			file<<"pushs "<<node->val.s_val<<"\n";
			break;
	}
}

/*
expr:
	const_expr			
	|var_expr			
	|unary_expr			
	|binary_expr			
	|allocate_expr			
	|proc_call			
	|cast_expr			
	|'(' expr ')'			{$$=new TreeRecord(NodeName::bracket_expr,$2);}
	;
	to output expr or to load it's value
*/
void CodeGeneration::generateExprCode(TreeRecord* node){
	if(ProgTree::name_comp(node->node_name,NodeName::bracket_expr)){
		generateExprCode(node->node1);
		return;
	}		
	if(ProgTree::name_comp(node->node_name,NodeName::int_const_expr)){
		file<<"pushi "<<node->val.i_val<<"\n";
		return;
	}

	if(ProgTree::name_comp(node->node_name,NodeName::double_const_expr)){
		//float value must be well formated like 1.0
		file.setf(file.showpoint);
		file<<"pushf "<<node->val.d_val<<"\n";
		return;
	}

	if(ProgTree::name_comp(node->node_name,NodeName::char_const_expr)){
		file<<"pushi "<<node->val.c_val<<"\n";
		return;
	}
	//STRING is considerd char*
	if(ProgTree::name_comp(node->node_name,NodeName::str_const_expr)){
		modifyString(node->val.s_val);
		file<<"pushs "<<node->val.s_val<<"\n";
		return;
	}
	if(ProgTree::name_comp(node->node_name,NodeName::false_const_expr)){
		file<<"pushi "<<node->val.b_val<<"\n";
		return;
	}
	if(ProgTree::name_comp(node->node_name,NodeName::true_const_expr)){
		file<<"pushi "<<node->val.b_val<<"\n";
		return;
	}

	if(ProgTree::name_comp(node->node_name,NodeName::null_const_expr)){
		file<<"pushg 0\n"; //null
		return;
	}

/*
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
	|ID_NAME ARROW var_expr		 
				{$$=new TreeRecord(@1,NodeName::id_arrow_expr,new TreeRecord(@1,$1),$3);}
	|THIS ARROW var_expr	{$$=new TreeRecord(@1,NodeName::this_arrow_expr,$3);}
	
	;	
*/
/*
array_expr:
	ID_NAME array_expr_dims	{$$=new TreeRecord(@1,NodeName::array_expr,new TreeRecord(@1,$1),$2);}
	;
array_expr_dims:
	'[' expr ']'	{$$=new TreeRecord(@1,NodeName::array_expr_dims,$2);}
	|array_expr_dims '[' expr ']'	{$$=new TreeRecord(@1,NodeName::array_expr_dims,$1,$3);}
	;
*/
	if(ProgTree::name_comp(node->node_name,NodeName::array_expr)){
		this->generateIDNameCode(node->node1);
		TreeRecord* dims=node->node2;
		//assuming two dims max
		if(dims->node2!=NULL){		
			this->generateExprCode(dims->node1->node1);
			int sec=((ArrayInfo)*(++node->rec->array_info.begin())).dim_size;
			file<<"pushi "<<sec<<"\n";
			file<<"mul\n";
			this->generateExprCode(dims->node2);
			file<<"add\n";
		}
		else
			this->generateExprCode(dims->node1);
		file<<"loadn\n";
		return;
	}
	/*array_expr '.' var_expr
		array_expr must be of class type
	*/
	if(ProgTree::name_comp(node->node_name,NodeName::array_class_expr)){
		this->generateExprCode(node->node1);
		generateExprLoadHelper(node->node2);
		return;

	}

	/*array_expr ARROW var_expr
		array_expr must be a pointer to a class type
	*/
	if(ProgTree::name_comp(node->node_name,NodeName::array_pointer_expr)){
		this->generateExprCode(node->node1);
		generateExprLoadHelper(node->node2);
		return;
	}


	/*
	pointer_val_expr:
	'*' var_expr	{$$=new TreeRecord(@1,NodeName::pointer_val_expr,$2);}	
	;
	*/
	if(ProgTree::name_comp(node->node_name,NodeName::pointer_val_expr)){
	//	generateExprLoadHelper(node->node1);
		//	this->generateLoadVarExprAddressCode(node);
		//load first element
		generateExprCode(node->node1);
		file<<"load 0\n";
		return;
	}
	//ID_NAME
	if(ProgTree::name_comp(node->node_name,NodeName::id_name_expr)){
		if(node->rec->shift_from==1)
			file<<"pushl "<<node->rec->shift<<"\n"; //local var
		if(node->rec->shift_from==0)
			file<<"pushg "<<node->rec->shift<<"\n"; //global var
		if(node->rec->shift_from==2){
			file<<"pushl -1"<<"\n"; //global var
			file<<"load "<<node->rec->shift<<"\n";
		}	
		return;
	}

	//THIS
	if(ProgTree::name_comp(node->node_name,NodeName::this_expr)){
		file<<"pushl -1\n"; 
		return;
	}	

	/*ID_NAME '.' var_expr
		just load the value located at the right shift from ID_NAME address
	*/
	if(ProgTree::name_comp(node->node_name,NodeName::class_var_expr)){
		if(node->rec->shift_from==1)
			file<<"pushl "<<node->rec->shift<<"\n"; //local var
		if(node->rec->shift_from==0)
			file<<"pushg "<<node->rec->shift<<"\n"; //global var
		if(node->rec->shift_from==2){
			file<<"pushl -1\n"; 
			generateExprLoadHelper(node->node2);
		}
		
	//	file<<"load "<<node->node2->rec->shift<<"\n";
		generateExprLoadHelper(node->node2);
		return;

	}

	//class_type SCOPE_DOTS ID_NAME
	if(ProgTree::name_comp(node->node_name,NodeName::scope_expr)){
		return;
	}

	//ID_NAME ARROW var_expr
	if(ProgTree::name_comp(node->node_name,NodeName::id_arrow_expr)){
		if(node->rec->shift_from==1)
			file<<"pushl "<<node->rec->shift<<"\n"; //local var
		if(node->rec->shift_from==0)
			file<<"pushg "<<node->rec->shift<<"\n"; //global var
		if(node->rec->shift_from==2){
			file<<"pushl -1\n"; 
			generateExprLoadHelper(node->node2);
		
		}
	//	file<<"load "<<node->node2->rec->shift<<"\n";
		generateExprLoadHelper(node->node2);
		return;
	}

	//THIS ARROW var_expr, no other check made
	if(ProgTree::name_comp(node->node_name,NodeName::this_arrow_expr)){
		file<<"pushl -1\n"; 
		generateExprLoadHelper(node->node1);
		return;
	}	
/*
unary_expr:
	 '!' expr		{$$=new TreeRecord(NodeName::unary_expr,$2,NULL,NULL,NULL,NULL,Type::NOT);}		
	| '-' expr	%prec POS_NEG	{$$=new TreeRecord(NodeName::unary_expr,$2,NULL,NULL,NULL,NULL,Type::MINUS);}		
	| ICR expr					{$$=new TreeRecord(NodeName::unary_expr,$2,NULL,NULL,NULL,NULL,Type::left_icr);}
	| expr ICR 				{$$=new TreeRecord(NodeName::unary_expr,$1,NULL,NULL,NULL,NULL,Type::right_icr);}
	| DECR expr				{$$=new TreeRecord(NodeName::unary_expr,$2,NULL,NULL,NULL,NULL,Type::left_decr);}	
	| expr DECR 			{$$=new TreeRecord(NodeName::unary_expr,$1,NULL,NULL,NULL,NULL,Type::right_decr);}	
	| '&' var_expr			{$$=new TreeRecord(NodeName::unary_expr,$2,NULL,NULL,NULL,NULL,Type::and);}	
	| '+' expr		%prec POS_NEG	{$$=new TreeRecord(NodeName::unary_expr,$2,NULL,NULL,NULL,NULL,Type::PLUS);}			
	;
*/
	if(ProgTree::name_comp(node->node_name,NodeName::unary_expr)){
		if(optimize && node->isConstData)
			generateConstCode(node);
		else
			this->generateUnaryExprCode(node);
		return;
	}

/*
binary_expr:
	math_operation_expr
	|logical_operation_expr
	;

math_operation_expr:
	expr '+' expr		{$$=new TreeRecord(NodeName::math_operation_expr,$1,$3,NULL,NULL,NULL,Type::PLUS);}				
	|expr '-' expr		{$$=new TreeRecord(NodeName::math_operation_expr,$1,$3,NULL,NULL,NULL,Type::MINUS);}				
	|expr '/' expr		{$$=new TreeRecord(NodeName::math_operation_expr,$1,$3,NULL,NULL,NULL,Type::DIV);}				
	|expr '*' expr		{$$=new TreeRecord(NodeName::math_operation_expr,$1,$3,NULL,NULL,NULL,Type::MULT);}				
	;
*/
	if(ProgTree::name_comp(node->node_name,NodeName::math_operation_expr)){
		if(optimize && node->isConstData){
			generateConstCode(node);
			return;
		}
		
		//if one operand only is constant then check monadics 
		if((optimize && !node->isConstData)&&(node->node1->isConstData || node->node2->isConstData)){
			//monadic elements
			generateMathMono(node);
			return;
		}
		else
			this->generateMathExprCode(node);
		return;
	}

/*
logical_operation_expr:
	expr '<' expr		{$$=new TreeRecord(NodeName::logical_operation_expr,$1,$3,NULL,NULL,NULL,Type::LESS);}				
	|expr '>' expr		{$$=new TreeRecord(NodeName::logical_operation_expr,$1,$3,NULL,NULL,NULL,Type::GREATER);}				
	|expr LE expr			{$$=new TreeRecord(NodeName::logical_operation_expr,$1,$3,NULL,NULL,NULL,Type::LEQ);}				
	|expr GE expr			{$$=new TreeRecord(NodeName::logical_operation_expr,$1,$3,NULL,NULL,NULL,Type::GEQ);}	
	|expr EQ expr			{$$=new TreeRecord(NodeName::logical_operation_expr,$1,$3,NULL,NULL,NULL,Type::EQL);}				
	|expr NE expr			{$$=new TreeRecord(NodeName::logical_operation_expr,$1,$3,NULL,NULL,NULL,Type::NEQ);}
	|expr OROR expr			{$$=new TreeRecord(NodeName::logical_operation_expr,$1,$3,NULL,NULL,NULL,Type::oror);}			
	|expr ANDAND expr		{$$=new TreeRecord(NodeName::logical_operation_expr,$1,$3,NULL,NULL,NULL,Type::andand);}		
	;
*/
	if(ProgTree::name_comp(node->node_name,NodeName::logical_operation_expr)){
		if(optimize && node->isConstData)
			generateConstCode(node);
		else		
			generateLogicalExprCode(node);
		return;
	}

/*
allocate_expr:
	new_expr							
	;

new_expr:
	NEW type '[' expr ']'						{$$=new TreeRecord(NodeName::new_array_expr,$2,$4);}
	|NEW type '(' proc_expr_list ')'  	{$$=new TreeRecord(NodeName::new_expr,$2,$4);}
	;

*/
	if(ProgTree::name_comp(node->node_name,NodeName::new_array_expr)){
		return;
	}

	/*	NEW type '(' proc_expr_list ')'
	*/
	if(ProgTree::name_comp(node->node_name,NodeName::new_expr)){
		if(ProgTree::name_comp(node->node1->node_name,NodeName::simple_type)){
			//allocate space and set it back to it's location
			file<<"alloc 1\n";
			file<<"dup 1\n";
			this->generateExprCode(node->node2->node1->node1);
			file<<"store 0\n"; //one elem
			return;
		}

		//else a constructor call
		file<<"alloc "<<node->node1->rec->ref_node->size<<"\n";
		//now call constructor
			//first we reserver place for the return value
		//	this->generateTypeCode(new TreeRecord(*(node->rec->sig_list.begin())));
	
			generateProcCallParamsCode(node->node2);
			//now push the object address
			file<<"pushsp "<<"\n"; //push the latest allocated address
			file<<"load "<<(-params_call_count-1)<<"\n";	
			file<<"pusha "<<node->rec->codeName<<"\n";
			file<<"call\n";
			//now pop generated params
			file<<"pop "<<params_call_count+1<<"\n";	
		return;		
	}



///////////////////////////////////////////////////////
/*
proc_call: 
	ID_NAME '(' proc_expr_list ')'				
		{$$=new TreeRecord(NodeName::proc_call,new TreeRecord($1),$3,NULL,NULL,NULL,Type::proc_call_normal);}			
	|ID_NAME '.' ID_NAME '(' proc_expr_list ')'				
		{$$=new TreeRecord(NodeName::proc_call,new TreeRecord($1),new TreeRecord($3),$5,NULL,NULL,Type::PROC_CLASS_CALL);}		
	|THIS ARROW ID_NAME '(' proc_expr_list ')'							
		{$$=new TreeRecord(NodeName::proc_call,new TreeRecord($3),$5,NULL,NULL,NULL,Type::PROC_THIS_CALL);}
	|class_type SCOPE_DOTS ID_NAME '(' proc_expr_list ')'
		{$$=new TreeRecord(NodeName::proc_call,$1,new TreeRecord($3),$5,NULL,NULL,Type::PROC_CLASS_STATIC_CALL);}		
	;  
*/

	if(ProgTree::name_comp(node->node_name,NodeName::proc_call)){
		this->generateProcCallStmtCode(node);
		//don't pop return value, it's needed
		return;
	}

/*
cast_expr:
	'(' type ')' expr	{$$=new TreeRecord(NodeName::cast_expr,$2,$4);}				
	;
*/
	if(ProgTree::name_comp(node->node_name,NodeName::cast_expr)){
		return;
	}
	return;
}

///////////////////////////////////////////////////////////////////////
/*
this functions helps in loading an expr value by pushing recursive load commands necessary
to load the whole expression relative to each shift:
a.x->d
here after a is pushed by generateExprCode
we generate a load for the x : i.e load 0
then load 5 for d
*/

void CodeGeneration::generateExprLoadHelper(TreeRecord* node){
		//ID_NAME ARROW var_expr
		if(ProgTree::name_comp(node->node_name,NodeName::id_arrow_expr)){
			file<<"load "<<node->node1->rec->shift<<"\n";
			this->generateExprLoadHelper(node->node2);
			return;
		}
	/*ID_NAME '.' var_expr
		just load the value located at the right shift from ID_NAME address
	*/
	if(ProgTree::name_comp(node->node_name,NodeName::class_var_expr)){
		file<<"load "<<node->node1->rec->shift<<"\n";
		this->generateExprLoadHelper(node->node2);
		return;

	}

	//ID_NAME
	if(ProgTree::name_comp(node->node_name,NodeName::id_name_expr)){
		file<<"load "<<node->rec->shift<<"\n";
		return;
	}

/*
array_expr:
	ID_NAME array_expr_dims	{$$=new TreeRecord(@1,NodeName::array_expr,new TreeRecord(@1,$1),$2);}
	;
array_expr_dims:
	'[' expr ']'	{$$=new TreeRecord(@1,NodeName::array_expr_dims,$2);}
	|array_expr_dims '[' expr ']'	{$$=new TreeRecord(@1,NodeName::array_expr_dims,$1,$3);}
	;
*/
	if(ProgTree::name_comp(node->node_name,NodeName::array_expr)){
		return;
		
	}
	/*array_expr '.' var_expr
		array_expr must be of class type
	*/
	if(ProgTree::name_comp(node->node_name,NodeName::array_class_expr)){
		return;

	}

	/*array_expr ARROW var_expr
		array_expr must be a pointer to a class type
	*/
	if(ProgTree::name_comp(node->node_name,NodeName::array_pointer_expr)){
		return;
	}

}

/*
unary_expr:
	 '!' expr		{$$=new TreeRecord(NodeName::unary_expr,$2,NULL,NULL,NULL,NULL,Type::NOT);}		
	| '-' expr	%prec POS_NEG	{$$=new TreeRecord(NodeName::unary_expr,$2,NULL,NULL,NULL,NULL,Type::MINUS);}		
	| ICR expr					{$$=new TreeRecord(NodeName::unary_expr,$2,NULL,NULL,NULL,NULL,Type::left_icr);}
	| expr ICR 				{$$=new TreeRecord(NodeName::unary_expr,$1,NULL,NULL,NULL,NULL,Type::right_icr);}
	| DECR expr				{$$=new TreeRecord(NodeName::unary_expr,$2,NULL,NULL,NULL,NULL,Type::left_decr);}	
	| expr DECR 			{$$=new TreeRecord(NodeName::unary_expr,$1,NULL,NULL,NULL,NULL,Type::right_decr);}	
	| '&' var_expr			{$$=new TreeRecord(NodeName::unary_expr,$2,NULL,NULL,NULL,NULL,Type::and);}	
	| '+' expr		%prec POS_NEG	{$$=new TreeRecord(NodeName::unary_expr,$2,NULL,NULL,NULL,NULL,Type::PLUS);}			
	;
	stacking the value
*/

void CodeGeneration::generateUnaryExprCode(TreeRecord* node){
	int type=*(node->node1->sig_list.begin());
	switch(node->node_type){
		case Type::NOT:
			this->generateExprCode(node->node1);
	/*		if(type==Type::type_int){
				file<<"pushi 1\n";
				file<<"sub\n";
				break;
			}
			if(type==Type::type_double){
				file<<"pushf 1.0\n";
				file<<"sub\n";
				break;
			}		*/
			file<<"not\n";
			break;
		case Type::MINUS: //same as mult by 1
			this->generateExprCode(node->node1);
			if(type==Type::type_int){
				file<<"pushi -1\n";
				file<<"mul\n";
			}
			if(type==Type::type_double){
				file<<"pushf -1.0\n";
				file<<"fmul\n";
			}
			break;
		case Type::PLUS: //do nothing, + changes no sign
			this->generateExprCode(node->node1);
			break;
		case Type::and: //nothing for now

			break;
		case Type::left_decr: //update variable first then push it
			this->generateExprCode(node->node1);
			if(type==Type::type_int){
				file<<"pushi 1\n";
				file<<"sub\n";
			}
			if(type==Type::type_double){
				file<<"pushf 1.0\n";
				file<<"sub\n";
			}
			//pointer data also must be handled
			
			//now store variable back after making a duplicate
			file<<"dup 1\n";
			this->generateStoreInVarExprCode(node->node1);
			break;
		case Type::right_icr:
			this->generateExprCode(node->node1);
			//make a duplicate then update variable and store it back, old variable is available on stack
			file<<"dup 1\n";
			if(type==Type::type_int){
				file<<"pushi 1\n";
				file<<"add\n";
			}
			if(type==Type::type_double){
				file<<"pushf 1.0\n";
				file<<"add\n";
			}
			this->generateStoreInVarExprCode(node->node1);
			break;
		case Type::left_icr:
			this->generateExprCode(node->node1);
			if(type==Type::type_int){
				file<<"pushi 1\n";
				file<<"add\n";
			}
			if(type==Type::type_double){
				file<<"pushf 1.0\n";
				file<<"add\n";
			}			
			//now store variable back after making a duplicate
			file<<"dup 1\n";
			this->generateStoreInVarExprCode(node->node1);
			break;
		case Type::right_decr:
			this->generateExprCode(node->node1);
			//make a duplicate then update variable and store it back, old variable is available on stack
			file<<"dup 1\n";
			if(type==Type::type_int){
				file<<"pushi 1\n";
				file<<"sub\n";
			}
			if(type==Type::type_double){
				file<<"pushf 1.0\n";
				file<<"sub\n";
			}
			this->generateStoreInVarExprCode(node->node1);
			break;
	}
}

/*
binary_expr:
	math_operation_expr
	|logical_operation_expr
	;
/*
math_operation_expr:
	expr '+' expr		{$$=new TreeRecord(NodeName::math_operation_expr,$1,$3,NULL,NULL,NULL,Type::PLUS);}				
	|expr '-' expr		{$$=new TreeRecord(NodeName::math_operation_expr,$1,$3,NULL,NULL,NULL,Type::MINUS);}				
	|expr '/' expr		{$$=new TreeRecord(NodeName::math_operation_expr,$1,$3,NULL,NULL,NULL,Type::DIV);}				
	|expr '*' expr		{$$=new TreeRecord(NodeName::math_operation_expr,$1,$3,NULL,NULL,NULL,Type::MULT);}				
	;
	we need to balance the two nodes and balance the code by using the appropriate casting
*/
void CodeGeneration::generateMathExprCode(TreeRecord* node){
	int type1=*(node->node1->sig_list.begin());
	int type2=*(node->node2->sig_list.begin());
	
	this->generateExprCode(node->node1);
	generateCastBalanceCode(node->node1,node->node2);
	this->generateExprCode(node->node2);
	generateCastBalanceCode(node->node2,node->node1); //do it for node2
			
	switch(node->node_type){
		case Type::PLUS:
			if((type1==Type::type_double)||(type2==Type::type_double))
				file<<"fadd\n";
			else
				file<<"add\n";
			break;
		case Type::MINUS:
			if((type1==Type::type_double)||(type2==Type::type_double))
				file<<"fsub\n";
			else
				file<<"sub\n";
			break;
		case Type::DIV:
			if((type1==Type::type_double)||(type2==Type::type_double))
				file<<"fdiv\n";
			else
				file<<"div\n";
			break;
		case Type::MULT:
			if((type1==Type::type_double)||(type2==Type::type_double))
				file<<"fmul\n";
			else
				file<<"mul\n";
			break;
	}	
}
/*
balance type to the most common type not to lose data precision
generate code to convert node1 code to the most common type 
*/
void CodeGeneration::generateCastBalanceCode(TreeRecord* node1,TreeRecord* node2){
	if(node1->sig_list==node2->sig_list)
		return; //no need, types are the same
	int type=*(node1->sig_list.begin());
	int type2=*(node2->sig_list.begin());
	switch(type){
		case Type::type_double:
			break;
		case Type::type_int:
			if((type2==Type::type_bool)||(type2==Type::type_char))
				break; //bool and char are int in the vm
			if(type2==Type::type_double)
				file<<"itof\n";
	}
}

/*
logical_operation_expr:
	expr '<' expr		{$$=new TreeRecord(NodeName::logical_operation_expr,$1,$3,NULL,NULL,NULL,Type::LESS);}				
	|expr '>' expr		{$$=new TreeRecord(NodeName::logical_operation_expr,$1,$3,NULL,NULL,NULL,Type::GREATER);}				
	|expr LE expr			{$$=new TreeRecord(NodeName::logical_operation_expr,$1,$3,NULL,NULL,NULL,Type::LEQ);}				
	|expr GE expr			{$$=new TreeRecord(NodeName::logical_operation_expr,$1,$3,NULL,NULL,NULL,Type::GEQ);}	
	|expr EQ expr			{$$=new TreeRecord(NodeName::logical_operation_expr,$1,$3,NULL,NULL,NULL,Type::EQL);}				
	|expr NE expr			{$$=new TreeRecord(NodeName::logical_operation_expr,$1,$3,NULL,NULL,NULL,Type::NEQ);}
	|expr OROR expr			{$$=new TreeRecord(NodeName::logical_operation_expr,$1,$3,NULL,NULL,NULL,Type::oror);}			
	|expr ANDAND expr		{$$=new TreeRecord(NodeName::logical_operation_expr,$1,$3,NULL,NULL,NULL,Type::andand);}		
	;
*/
void CodeGeneration::generateLogicalExprCode(TreeRecord* node){
	int type1=*(node->node1->sig_list.begin());
	int type2=*(node->node2->sig_list.begin());
	char* orsFail=NULL;
	char* orsSuccess=NULL;
	int label=++labelCount;
	switch(node->node_type){
		case Type::LESS:
			this->generateExprCode(node->node1);
			generateCastBalanceCode(node->node1,node->node2);
			this->generateExprCode(node->node2);
			generateCastBalanceCode(node->node2,node->node1); //do it for node2
					
			if((type1==Type::type_double)||(type2==Type::type_double))
				file<<"fINF\n";
			else
				file<<"INF\n";	
			break;
		case Type::GREATER:
			this->generateExprCode(node->node1);
			generateCastBalanceCode(node->node1,node->node2);
			this->generateExprCode(node->node2);
			generateCastBalanceCode(node->node2,node->node1); //do it for node2
			
			if((type1==Type::type_double)||(type2==Type::type_double))
				file<<"fSUP\n";
			else
				file<<"SUP\n";
			break;
		case Type::LEQ:
			this->generateExprCode(node->node1);
			generateCastBalanceCode(node->node1,node->node2);
			this->generateExprCode(node->node2);
			generateCastBalanceCode(node->node2,node->node1); //do it for node2
			
			if((type1==Type::type_double)||(type2==Type::type_double))
				file<<"fINFEQ\n";
			else
				file<<"INFEQ\n";
			break;
		case Type::GEQ:
			this->generateExprCode(node->node1);
			generateCastBalanceCode(node->node1,node->node2);
			this->generateExprCode(node->node2);
			generateCastBalanceCode(node->node2,node->node1); //do it for node2
			
			if((type1==Type::type_double)||(type2==Type::type_double))
				file<<"fSUPEQ\n";
			else
				file<<"SUPEQ\n";
			break;

		case Type::EQL: 
			this->generateExprCode(node->node1);
			generateCastBalanceCode(node->node1,node->node2);
			this->generateExprCode(node->node2);
			generateCastBalanceCode(node->node2,node->node1); //do it for node2
			
		//	if((type1==Type::type_double)||(type2==Type::type_double))
		//		file<<"fsub\n";
		//	else
			//	file<<"sub\n";
			file<<"equal\n";
			break;
		case Type::NEQ:
			this->generateExprCode(node->node1);
			generateCastBalanceCode(node->node1,node->node2);
			this->generateExprCode(node->node2);
			generateCastBalanceCode(node->node2,node->node1); //do it for node2
			
			file<<"equal\n";
			file<<"not\n";
			break;

		case Type::oror:

			orsFail=this->generateRandLabel("orsFail",label);
			orsSuccess=this->generateRandLabel("orsSuccess",label);
			
			/*
				first we push 1, if we failed we pop it and push 0 back
				else we continue with this 1 on the stack
			*/
			file<<"pushi 1\n"; //when all exprs are true
			this->generateExprCode(node->node1);
		//	generateCastBalanceCode(node->node1,node->node2);
			file<<"not\n"; //negate
			file<<"jz "<<orsSuccess<<"\n"; //one true is enough
			
			this->generateExprCode(node->node2);
		//	generateCastBalanceCode(node->node2,node->node1); //do it for node2
			file<<"jz "<<orsFail<<"\n";
			file<<"jump "<<orsSuccess<<"\n";

			file<<orsFail<<":\n";
			file<<"pop 1\n";
			file<<"pushi 0\n"; //fail

			file<<orsSuccess<<":\n"; //just go on

			delete []orsSuccess;
			delete []orsFail;			
			break;
		/*
		any expression that yields 0 stops any further checking by jumping
		*/
		case Type::andand:
			char* andsFail=this->generateRandLabel("andsFail",label);
			char* andsSuccess=this->generateRandLabel("andsSuccess",label);
			/*
				first we push 1, if we failed we pop it and push 0 back
				else we continue with this 1 on the stack
			*/
			file<<"pushi 1\n"; //when all exprs are true
			this->generateExprCode(node->node1);
		//	generateCastBalanceCode(node->node1,node->node2);
			
			file<<"jz "<<andsFail<<"\n";
			
			this->generateExprCode(node->node2);
		//	generateCastBalanceCode(node->node2,node->node1); //do it for node2
			file<<"jz "<<andsFail<<"\n";
			file<<"jump "<<andsSuccess<<"\n";

			file<<andsFail<<":\n";
			file<<"pop 1\n";
			file<<"pushi 0\n"; //fail

			file<<andsSuccess<<":\n"; //just go on

			delete []andsSuccess;
			delete []andsFail;
			
			break;
	}	
}

/* node is a statements node 
	used to initialize the local variables by pushing them into the stack
*/
void CodeGeneration::initVars(TreeRecord* node){
	if(ProgTree::name_comp(node->node_name,NodeName::stmts)){
		initVars(node->node1);
		initVars(node->node2);
		return;
	}

	if(ProgTree::name_comp(node->node_name,NodeName::var_decl)){
		initVarDeclStmtCode(node,1);
		return;
	}
	
	if(ProgTree::name_comp(node->node_name,NodeName::compound_block_stmt)){
			TreeRecord* block=node->node1;
			if(block->node1==NULL)
				return;
			else
				this->initVars(block->node1);
			return;
	}

	if(ProgTree::name_comp(node->node_name,NodeName::for_stmt)){
		initVarDeclStmtCode(node->node1,1);	
		initVars(node->node4);
		return;
	}

	if(ProgTree::name_comp(node->node_name,NodeName::if_stmt)){
		initVars(node->node2);
		return;
	}

	if(ProgTree::name_comp(node->node_name,NodeName::if_else_stmt)){
		initVars(node->node2);
		initVars(node->node3);
		return;
	}

	if(ProgTree::name_comp(node->node_name,NodeName::while_stmt)){
		initVars(node->node2);
		return;
	}

/*
	if(ProgTree::name_comp(node->node_name,NodeName::stmts)){
		if(ProgTree::name_comp(node->node2->node_name,NodeName::var_decl))
			initVarDeclStmtCode(node->node2,1);
		else
		if(ProgTree::name_comp(node->node2->node_name,NodeName::compound_block_stmt)){
			TreeRecord* block=node->node2->node1;
			if(block->node1==NULL)
				return;
			else
				this->initVars(block->node1);
		}
		else
			if(ProgTree::name_comp(node->node2->node_name,NodeName::for_stmt)){
				initVarDeclStmtCode(node->node2->node1,1);	
			}
		initVars(node->node1);
		return;
	}

	//one stmt
	if(ProgTree::name_comp(node->node_name,NodeName::var_decl))
		initVarDeclStmtCode(node,1);
	else //for now
		if(ProgTree::name_comp(node->node_name,NodeName::compound_block_stmt)){
			TreeRecord* block=node->node1;
			if(block->node1==NULL)
				return;
			else
				this->initVars(block->node1);
		}
		else
			if(ProgTree::name_comp(node->node_name,NodeName::for_stmt)){
				initVarDeclStmtCode(node->node1,1);	
			}
*/
}

/* 

prog_decls :
		{$$=new TreeRecord(NodeName::prog_decls); }	
	|	prog_decls declaration	{$$=new TreeRecord(NodeName::prog_decls,$1,$2); }	
	|	prog_decls error	{$$=new TreeRecord(NodeName::prog_decls_error);} 
	;
	used to initialize the global variables
*/
void CodeGeneration::initGlobalVars(TreeRecord* node){
	
	if(node->node1==NULL)
		return;
	if(ProgTree::name_comp(node->node_name,NodeName::prog_decls)){
		if(ProgTree::name_comp(node->node2->node_name,NodeName::var_decl))
			initVarDeclStmtCode(node->node2,0); //0 for global
		initGlobalVars(node->node1);
		return;
	}

	//one stmt
	if(ProgTree::name_comp(node->node_name,NodeName::var_decl))
		initVarDeclStmtCode(node,0);
}

/* to init a var_decl stmt in th first round:
	what happens here is that we reserve the initial size of the variables and
	so the same we say for objects by calling their constructor 
*/
void CodeGeneration::initVarDeclStmtCode(TreeRecord* node,int scope){
		TreeRecord* type_node;
		TreeRecord* list_node;
		if(node->node3){
			type_node=node->node2;
			list_node=node->node3;
		}
		else{
			type_node=node->node1;
			list_node=node->node2;
		}
		/*case when the list is a single constructor call with parameters:	A a(4);
		 what we do here is: allocate an address for the object variable with size
			enough to hold all member varaibles
			so we calculate the size required to hold these variables
		ID_NAME '(' proc_expr_list_not_empty ')'
			 {$$=new TreeRecord(@1,NodeName::var_constructor_call,new TreeRecord(@1,$1),$3);}
	;
		*/
		if(ProgTree::name_comp(list_node->node_name,NodeName::var_constructor_call)){
			if(optimize && ! list_node->rec->isUsed)
				return;
			//get class variable size 
			Record r(type_node->node_name,symtab.first_scope->depth+1,symtab.first_scope,Type::class_type);
			Record* class_rec=symtab.findRecord(r); //always succeed
			if(class_rec==NULL){
				//error
				return;
			}
			file<<"alloc "<<class_rec->ref_node->size<<"\n";
			list_node->rec->shift=shift++;
			list_node->rec->shift_from=scope;
			return;
		}
		
		//var_list_elem|var_list ',' var_list_elem`
		while(true){
			if(ProgTree::name_comp(list_node->node_name,NodeName::var_list_elem)){
				initVarListElemCode(list_node,type_node,scope);
				break;
			}
			initVarListElemCode(list_node->node2,type_node,scope);
			list_node=list_node->node1;
		}

		return;
}

/* this method will be used to initilaize the variable by pushing it into the stack and 
updating the symbol table var record
*/
void CodeGeneration::initVarListElemCode(TreeRecord* elem_node,TreeRecord* type_node,int scope){
	if(optimize && ! elem_node->rec->isUsed)
		return;
	
	if(elem_node->node2==NULL){
		this->generateTypeCode(type_node);
		elem_node->node1->rec->shift=shift++;
		elem_node->node1->rec->shift_from=scope;
		return ;
	}
		
	if(ProgTree::name_comp(elem_node->node2->node_name,NodeName::array_dims)||ProgTree::name_comp(elem_node->node2->node_name,NodeName::array_dim)){
		// we got array variable decleration, 
		elem_node->node1->rec->shift=shift++;
		elem_node->node1->rec->shift_from=scope;
		elem_node->rec->shift_from=scope;
		file<<"alloc 0\n";
	}
	else{
			this->generateTypeCode(type_node);
			elem_node->node1->rec->shift=shift++;
			elem_node->node1->rec->shift_from=scope;
			return ;
	}
}

/*if one side is const in a math operation then check for monos:
	expr * 1 = expr no matter what expr was, so no need to generate mult

*/
void CodeGeneration::generateMathMono(TreeRecord* node){
	if(node->node1->isConstData){
		double val=ProgTree::getMaxVal(node->node1);
			switch(node->node_type){
				case Type::PLUS:
					if(val==0)
						this->generateExprCode(node->node2);
					return;
				case Type::MULT: 
					if(val==1)
						this->generateExprCode(node->node2);
					return;
			}
			this->generateMathExprCode(node);
			return;
	}
	
	if(node->node2->isConstData){
		double val=ProgTree::getMaxVal(node->node2);
			switch(node->node_type){
				case Type::PLUS:
					if(val==0)
						this->generateExprCode(node->node1);
					return;
				case Type::MINUS:
					if(val==0)
						this->generateExprCode(node->node1);
					return;
				case Type::MULT: 
					if(val==1)
						this->generateExprCode(node->node1);
					return;
			}
			this->generateMathExprCode(node);
			return;
	}
}