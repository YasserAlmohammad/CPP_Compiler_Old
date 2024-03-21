#include <iostream.h>
#include <string.h>
#include <stdlib.h>
#include "prog_tree.h"
#include <list>
#include "errorReporting.h"
using namespace std;

extern SymbolTable symtab;
Record* tempPar=NULL;
TreeRecord* cur_return_type;
static int	loops=0; //used for break and continue
/* any statement could be a scope parent, so each time we add a scope info we set this variable
 but when real new scope is entered the parent is updated */
extern Record* cur_par;
static int cur_access_spec=Type::access_private;
//initially this record contains the file scope record
//depth value used with name of id to make a unique id in a scope
//current parent


///////////////////////////////////////////////////////////////////////////////////
//************************* NodeName Class **************************************//
//////////////////////////////////////////////////////////////////////////////////

char* NodeName::program="program";
char* NodeName::prog_decls="prog_decls";
char* NodeName::prog_decls_error="prog_decls_error";
char* NodeName::cast_expr="cast_expr";
char* NodeName::delete_stmt="delete_stmt";
char* NodeName::expr_list="expr_list";
char* NodeName::new_expr="new_expr";
char* NodeName::unary_expr="unary_expr";
char* NodeName::binary_expr="binary_expr";
char* NodeName::pointer_val_expr="pointer_val_expr";
char* NodeName::array_expr="array_expr";
char* NodeName::array_expr_dims="array_expr_dims";
char* NodeName::var_expr="var_expr";
char* NodeName::this_arrow_expr="this_arrow_expr";
char* NodeName::id_arrow_expr="id_arrow_expr";
char* NodeName::scope_expr="scope_expr";
char* NodeName::class_var_expr="class_var_expr";
char* NodeName::this_expr="this_expr";
char* NodeName::id_name_expr="id_name_expr";
char* NodeName::true_const_expr="true_const_expr";
char* NodeName::false_const_expr="false_const_expr";
char* NodeName::str_const_expr="str_const_expr";
char* NodeName::char_const_expr="char_const_expr";
char* NodeName::double_const_expr="double_const_expr";
char* NodeName::int_const_expr="int_const_expr";
char* NodeName::bracket_expr="bracket_expr";
char* NodeName::while_stmt="while_stmt";
char* NodeName::for_itr="for_itr";
char* NodeName::for_stmt="for_stmt";
char* NodeName::if_stmt="if_stmt";
char* NodeName::if_else_stmt="if_else_stmt";
char* NodeName::block_stmt="block_stmt";
char* NodeName::cout_stmt="cout_stmt";
char* NodeName::cout_stmt_output_list="cout_stmt_output_list";
char* NodeName::output_elem="output_elem";
char* NodeName::cin_stmt="cin_stmt";
char* NodeName::cin_stmt_input_list="cin_stmt_input_list";
char* NodeName::input_elem="input_elem";
char* NodeName::proc_expr_list_not_empty="proc_expr_list_not_empty";
char* NodeName::proc_expr_list="proc_expr_list";
char* NodeName::proc_call="proc_call";
char* NodeName::assignment="assignment";
char* NodeName::class_destructor="class_destructor";
char* NodeName::class_static_init_var="class_static_init_var";
char* NodeName::array_init_list_lists="array_init_list_lists";
char* NodeName::array_init_list_consts="array_init_list_consts";
char* NodeName::array_init_list="array_init_list";
char* NodeName::var_list="var_list";
char* NodeName::var_list_elem="var_list_elem";
char* NodeName::var_decl="var_decl";
char* NodeName::stmt_error="stmt_error";
char* NodeName::stmts="stmts";
char* NodeName::class_func_def_common="class_func_def_common";
char* NodeName::class_func_def_normal="class_func_def_normal";
char* NodeName::class_func_def_inline="class_func_def_inline";
char* NodeName::func_def_common="func_def_common";
char* NodeName::func_def_normal="func_def_normal";
char* NodeName::func_def_inline="func_def_inline";
char* NodeName::func_decl_arg_elem="func_decl_arg_elem";
char* NodeName::func_decl_arg_list="func_decl_arg_list";
char* NodeName::func_decl_args="func_decl_args";
char* NodeName::func_decl="func_decl";
char* NodeName::class_destructor_decl="class_destructor_decl";
char* NodeName::c_c_init_list="c_c_init_list";
char* NodeName::class_constructor_init_list="class_constructor_init_list";
char* NodeName::class_constructor="class_constructor";
char* NodeName::class_constructor_decl="class_constructor_decl";
char* NodeName::class_body_stmt_error="class_body_stmt_error";
char* NodeName::class_body_stmt="class_body_stmt";
char* NodeName::class_body_stmts="class_body_stmts";
char* NodeName::class_body="class_body";
char* NodeName::class_def="class_def";
char* NodeName::array_dim="array_dim";
char* NodeName::array_dims="array_dims";
char* NodeName::jump_stmt="jump_stmt";
char* NodeName::class_decl="class_decl";

char* NodeName::symtab_rec="symtab_rec";
char* NodeName::simple_type="simple_type";
char* NodeName::class_head="class_head";
char* NodeName::access_spec="access_spec";
char* NodeName::class_base_list="class_base_list";
char* NodeName::storage_spec="storage_spec";
char* NodeName::pointer_type="pointer_type";
char* NodeName::reference_type="reference_type";
char* NodeName::class_type="class_type";
char* NodeName::var_constructor_call="var_constructor_call";
char* NodeName::for_itr_elem_expr="for_itr_elem_expr";
char* NodeName::math_operation_expr="math_operation_expr";
char* NodeName::logical_operation_expr="logical_operation_expr";
char* NodeName::new_array_expr="new_array_expr";
char* NodeName::array_class_expr="array_class_expr";
char* NodeName::array_pointer_expr="array_pointer_expr";
char* NodeName::compound_block_stmt="compound_block_stmt";
char* NodeName::expr_stmt="expr_stmt";
char* NodeName::name_error="name_error";
char* NodeName::null_const_expr="null_const_expr";
///////////////////////////////////////////////////////////////////////////////////
//************************* TreeRecord Class ***********************************//
//////////////////////////////////////////////////////////////////////////////////

int TreeRecord::count=0;
TreeRecord::TreeRecord(YYLTYPE loc,char* name,TreeRecord* n1,TreeRecord* n2,TreeRecord* n3,TreeRecord* n4,TreeRecord* n5,int type, int align,TreeRecord* p){
	node_name=name;
	parent=p;
	node1=n1;
	node2=n2;
	node3=n3;
	node4=n4;
	node5=n5;
	node_alignment=align;
	node_type=type;
	line_num=loc.first_line;
	col_num=loc.first_column;
	isConstData=false; 
	rec=NULL;
	size=0;
	generateCode=true;
//	symtab_rec=NULL;
}
TreeRecord::TreeRecord(YYLTYPE loc,char* node_name,char* value,int type){
	this->node_name=node_name;
	parent=NULL;
	node1=NULL;
	node2=NULL;
	node3=NULL;
	node4=NULL;
	node5=NULL;
	node_alignment=0;
	node_type=type;	
	line_num=loc.first_line;
	col_num=loc.first_column;
	isConstData=false; 
	rec=NULL;
	size=0;
	generateCode=true;
	switch(type){
		case Type::type_int:
			val.i_val=atoi(value);
			break;
		case Type::type_double:
			val.d_val=atof(value);
			break;
		case Type::type_char:
			val.c_val=atoi(value);
			break;
		case Type::type_false:
			val.b_val=0;
			break;
		case Type::type_true:
			val.b_val=1;
			break;
		case Type::type_void:
			//not know yet
			break;
		case Type::type_str:
			val.s_val=value;
			break;
	}
	
}
TreeRecord::TreeRecord(){
	node_name=randName();
	parent=NULL;
	node1=NULL;
	node2=NULL;
	node3=NULL;
	node4=NULL;
	node5=NULL;
	node_alignment=0;
	node_type=0;	
	line_num=0;
	col_num=0;
	isConstData=false; 
	rec=NULL;
	size=0;
	generateCode=true;
}
TreeRecord::TreeRecord(int t){
	node_type=t;
	parent=NULL;
	node1=NULL;
	node2=NULL;
	node3=NULL;
	node4=NULL;
	node5=NULL;
	node_alignment=0;	
	line_num=0;
	col_num=0;
	isConstData=false; 
	rec=NULL;
	size=0;
	generateCode=true;
}

char* TreeRecord::randName(){
	char countStr[10]={'\0'};
	_itoa(count++,countStr,10);
	int len=7+strlen(countStr);
	char* ret=(char*)malloc(len);
	strcpy(ret,"block_");
	strncat(ret,countStr,len);
	ret[len]='\0';
	return ret;
}

/*
TreeRecord::TreeRecord(Record* rec){
	parent=p;
	node1=n1;
	node2=n2;
	node3=n3;
	node4=n4;
	node_alignment=align;
	node_type=type;

	node_name=NodeName::symtab_rec;
	this->symtab_rec=rec;
}
*/
void TreeRecord::print(){
	cout<<node_name<<endl;
	if(node1!=NULL)
		node1->print();
	if(node2!=NULL)
		node2->print();
	if(node3!=NULL)
		node3->print();
	if(node4!=NULL)
		node4->print();
	if(node5!=NULL)
		node5->print();
}

///////////////////////////////////////////////////////////////////////////////////
//************************* ProgTree Class *************************************//
//////////////////////////////////////////////////////////////////////////////////


ProgTree::ProgTree(){
	YYLTYPE first_loc;
	first_loc.first_column=1;
	first_loc.first_line=1;
	first_loc.last_column=1;
	first_loc.last_line=1;

	head=new TreeRecord(first_loc,NodeName::program);
	isValid=false;
	optimize=true;
}

void ProgTree::print(){
	head->print();
}

void printList(list<int> l){
	cout<<"=================\n";
	list<int>::iterator itr;
	for(itr=l.begin();itr!=l.end();itr++){
		cout<<*itr<<" ";
	}
	cout<<"\n================\n";
}

//later this might change into integer compare
bool ProgTree::name_comp(char* n1,char* n2){
	if(strcmp(n1,n2)==0)
		return true;
	else
		return false;
}
///////////////////////////////////////////////////////////////////////////////////////






/*these checks are mapped to the grammar decleration syntax
  this tree traversal adds the symbol table info, updates it
  also does check of all kinds to the program before it's considerd ok
  every thing was moved to the program tree so we can focus on one place for processing
  rather distributing it among the yacc and here 
*/
void ProgTree::checkTree(TreeRecord* node){
	if(node==NULL) //never happens, how ever
		return;
/////////////////////////////////////////////////////
/*
program :
	prog_decls   { progTree.head->node1=$1; }
	;
*/
	if(name_comp(node->node_name,NodeName::program)){
		checkTree(node->node1);
		return;
	}


/////////////////////////////////////////////////////
/*
prog_decls :
		{$$=new TreeRecord(NodeName::prog_decls); }	
	|	prog_decls declaration	{$$=new TreeRecord(NodeName::prog_decls,$1,$2); }	
	|	prog_decls error	{$$=new TreeRecord(NodeName::prog_decls_error);} 
	;
*/
	if(name_comp(node->node_name,NodeName::prog_decls)){
		checkTree(node->node1);
		checkTree(node->node2);
			return;
	}

	if(name_comp(node->node_name,NodeName::prog_decls_error)){
		//check error
		ErrorReport::printError(node->line_num,node->col_num,"program main decleration error");
		isValid=false;
		return;
	}
/////////////////////////////////////////////////////
/*
class_def:
	 class_head  class_body		{$$=new TreeRecord(NodeName::class_def,$1,$2);} 
	|class_head ':' access_spec       class_base_list    class_body	{$$=new TreeRecord(NodeName::class_def,$1,$3,$4,$5);} 
	;
*/
	if(name_comp(node->node_name,NodeName::class_def)){
		/*add class type to the symbol table
		  if it's already there due previous decleration (only) then we update the node_ref pointer
		  only(i.e. the case where it was declared as class A;
		  */
		//check if previous decleration exists
		char* class_name=node->node1->node_name;
		Record temp(class_name,cur_par->depth+1,cur_par,Type::class_type);
		tempPar=symtab.findRecord(temp);
		if(tempPar==NULL){ //new
		//	tempPar=symtab.addRecord(Record(class_name,cur_par->depth+1,cur_par,node,Type::type_class_def));		
		//	createTypeSignature(node->node1->node1->node_name,temp);
			tempPar=symtab.addRecord(temp);
			createTypeSignature(node->node1,*tempPar); //used to match against later
			tempPar->ref_node=node;
		}
		else{ //exists
			if(tempPar->ref_node!=NULL){ //redeclaration error
			//	cout<<"class "<<class_name<<" is already declared\n";
				ErrorReport::printError(node->line_num,node->col_num,"class is already declared");
				isValid=false;
			}
			else{ //update it
				tempPar->ref_node=node;
			}
		}
		//check base_list not to contain names of the same derived name 
		/*	class_base_list:
				class_type						
				|class_base_list ',' class_type	{$$=new TreeRecord(NodeName::class_base_list,$1,$3);}
				|class_base_list ',' error		{$$=new TreeRecord(@1,NodeName::name_error,$1);}

		*/
		if(node->node3!=NULL)
		{
			TreeRecord* base=node->node3;
			if(base->node2!=NULL){
				ErrorReport::printError(node->line_num,node->col_num,"multiple inheritance not allowed");
			}
			else{
				Record r(base->node_name,symtab.first_scope->depth+1,symtab.first_scope,Type::class_type);
				Record* class_rec=symtab.findRecord(r);
				if(class_rec==NULL){
					ErrorReport::printError(node->line_num,node->col_num,"can't find base class in global scope");	
				}
				else{
					Record c(base->node_name,class_rec->depth+1,class_rec,Type::type_destructor);
					Record* c_rec=symtab.findRecord(c);
					base->rec=c_rec;

				}
			}
			
			//assuming one

			/*
			TreeRecord* temp=node->node3;
			if(!name_comp(temp->node_name,NodeName::name_error)){ //==null in error case
				while(temp->node2!=NULL){
					if(name_comp(class_name,temp->node2->node_name)){
						//error
						isValid=false;
					//	cout<<"u can't derive from ur self\n";
						ErrorReport::printError(node->line_num,node->col_num,"u can't derive from ur self");
						break;
					}
					temp=temp->node1;
				}
				//final element of class_type name
				if(name_comp(class_name,temp->node_name)){
					//error
					isValid=false;
				//	cout<<"u can't derive from ur self\n";
					ErrorReport::printError(node->line_num,node->col_num,"u can't derive from ur self");
				}
			}
			else{
				ErrorReport::printError(temp->line_num,temp->col_num,"class base list error\n");
			}
			*/
			
		}
		//class body check
		if(node->node4==NULL){
				checkTree(node->node2);

		}
		else{
				checkTree(node->node4);
		}

		return;
	}
/////////////////////////////////////////////////////
/*
class_body:
	'{' '}' ';'		{$$=new TreeRecord(NodeName::class_body);} 
	|'{'  class_body_stmts '}' ';'	{$$=new TreeRecord(NodeName::class_body,$2);} 
	;
*/
	//make scope rules and check internal statements
	if(name_comp(node->node_name,NodeName::class_body)){
		cur_par=tempPar;
		cur_access_spec=Type::access_private; //reset access type to private by default
		checkTree(node->node1);
		cur_par=cur_par->parent; //restore scope
		cur_access_spec=Type::access_private; 
		//reset access type again(not neccessary)
		return;
	}

///////////////////////////////////////////////////////
/*
class_body_stmts:
	class_body_stmt						{$$=new TreeRecord(@1,NodeName::class_body_stmts,$1);}
	|class_body_stmts class_body_stmt	{$$=new TreeRecord(NodeName::class_body_stmts,$1,$2);} 
	;
*/
	/*here we rely on each node to check it's own self, how ever, we need to set
	the access_spec to all following statements until next access_spec is mentioned
	now access info is stored in the symbol table for fast access
	we do it this way: just like we did for scope info, define global variable
	change it only when access_spec is changed
	*/
	if(name_comp(node->node_name,NodeName::class_body_stmts)){
		if(node->node2==NULL)
			checkTree(node->node1);
		else{
			checkTree(node->node1);
			checkTree(node->node2);
		}
			return;
	}

///////////////////////////////////////////////////////
/*
class_body_stmt:
	access_spec ':'			{$$=$1} 		
	|func_decl					
	|func_def   				
	|var_decl ';'					
	|class_constructor_decl
	|class_destructor_decl
	|error ';'				{$$=new TreeRecord(NodeName::class_body_stmt);}  
	|error '}'				{$$=new TreeRecord(NodeName::class_body_stmt);}  
	;	
*/

	if(name_comp(node->node_name,NodeName::class_body_stmt)){
			isValid=false;
		//	cout<<"class definition error\n";
			ErrorReport::printError(node->line_num,node->col_num,"class definition error");

			//the only two nodes with such label is the errors, other labels are of their own
			return;
	}

///////////////////////////////////////////////////////
/*
access_spec:
	PUBLIC		{$$=new TreeRecord(NodeName::access_spec,NULL,NULL,NULL,NULL,NULL,Type::access_public);} 
	|PROTECTED	{$$=new TreeRecord(NodeName::access_spec,NULL,NULL,NULL,NULL,NULL,Type::access_protected);} 
	|PRIVATE	{$$=new TreeRecord(NodeName::access_spec,NULL,NULL,NULL,NULL,NULL,Type::access_private);} 
	|FRIEND		{$$=new TreeRecord(NodeName::access_spec,NULL,NULL,NULL,NULL,NULL,Type::access_friend);} 
	;
*/
	if(name_comp(node->node_name,NodeName::access_spec)){
		cur_access_spec=node->node_type; //update global
		return;
	}
///////////////////////////////////////////////////////
/*
class_constructor_decl:
	class_type '(' func_decl_args ')' ';'
			{	$$=new TreeRecord(NodeName::class_constructor_decl,$1,$3);}  
	;
*/
	/*add the decleration to the symbol table, set it's scope and access spec, code it's signature
	  , check if it was already delared before with the same signature, allow multiple declerations
	  of constructor but with different signature (function overloading)
	 but first of all check if this constructor is the current class constructor	 
	 */
	if(name_comp(node->node_name,NodeName::class_constructor_decl)){
		char* class_name=node->node1->node_name;
		if(name_comp(class_name,cur_par->name)){
			Record r(class_name,cur_par->depth+1,cur_par,Type::type_constructor,cur_access_spec);
	//		Record* temp=symtab.findRecord(r);
			createFuncArgSignature(node->node2,r);
			if(!symtab.addRecord(r)){ //error
				isValid=false;
			//	cout<<class_name<<" constructor with the same parameters already declared\n";
				ErrorReport::printError(node->line_num,node->col_num,"constructor with the same parameters already declared");
			}
		}
		else{ //error
			isValid=false;
		//	cout<<"this costructor doesn't belong to this class\n";
			ErrorReport::printError(node->line_num,node->col_num,"this costructor doesn't belong to this class");
		}
	
		return;
	}
	/*
	  - ambiguty in function decleration resulting from default value declerations must be checked
	  - function signature depends only on it's parameter delerations
	  - f(int,int)	conflicts with  f(int x,int y)  or f(int s,int v)
			we use this signature to enable function overloading
	  - a function that contains default values have more than one signature( signature list)
	  - function call uses the same method to match it's call signatur to the decleration 
	    signature
	  - signature info is int list for fast matching signatures, each element refers to a type
  */

///////////////////////////////////////////////////////
/*
class_constructor:
	class_type SCOPE_DOTS class_type '(' func_decl_args ')' class_constructor_init_list block_stmt	
	{$$=new TreeRecord(NodeName::class_constructor,$1,3,$5,$7,$8);}
	;
*/
/* 
	- a constructor like: A::A(args){ stmts }
	- a constructor must match a declerated constructor in a defined class( match name and signature)
		- variables inside the declerations arguments must be stored in the symbol table with the relevant function
		scope, and the nodes they come from must reference them back
*/
	if(name_comp(node->node_name,NodeName::class_constructor)){
		//check the the constructor belongs to the class
		char* class_name=node->node1->node_name;
		if(!name_comp(class_name,node->node2->node_name)){ //error
			isValid=false;
		//	cout<<"this constructor is not member of the refred class\n";
			ErrorReport::printError(node->line_num,node->col_num,"this constructor is not member of the refred class");
			return;
		}
		//find parent first
		Record* parent=symtab.findRecord(class_name,cur_par->depth+1,cur_par,Type::class_type);

		Record temp(class_name,parent->depth+1,parent,Type::type_constructor);
		createFuncArgSignature(node->node3,temp);
//		temp.ignoreSignature=false;
		//find one with the same signature
		Record* decl=symtab.findRecord(temp,false);
		if(decl==NULL){ //no previous decleration, error
			isValid=false;
		//	cout<<"no previous decleration for this constructor\n";
			ErrorReport::printError(node->line_num,node->col_num,"no previous decleration for this constructor");
			return;
		}
		
		decl->createFuncName(class_name);
		
		node->codeName=decl->codeName;
		
		node->node1->rec=parent; //used later
		decl->ref_node=node; //set reference node and denote as definition
		//add argument variables to the symbol table with this function scope
		addArgs2Symtab(node->node3,decl);

		//change scope
		cur_par=decl;
		//check member init list
		checkInitListVals(node->node4,parent); //these variables are from the class scope not the function's
		//check body
		checkTree(node->node5);

		//restore scope into parent of parent( because we wen two levels down CLASS->func
		cur_par=cur_par->parent->parent;

		return;
	}

///////////////////////////////////////////////////////
/*class_destructor_decl:
	'~' class_type '(' ')' ';'		 {	$$=new TreeRecord(NodeName::class_destructor_decl,$2);}
	;

	destrucotrs are stored in the same name as class but with type type_destructor
*/
	if(name_comp(node->node_name,NodeName::class_destructor_decl)){
		char* class_name=node->node1->node_name;
		if(name_comp(class_name,cur_par->name)){
			Record r(class_name,cur_par->depth+1,cur_par,Type::type_destructor,cur_access_spec);
	//		Record* temp=symtab.findRecord(r);
			if(!symtab.addRecord(r)){ //error
				isValid=false;
			//	cout<<class_name<<" destructor with the same parameters already declared\n";
				ErrorReport::printError(node->line_num,node->col_num,"destructor with the same parameters already declared");

			}
		}
		else{ //error
			isValid=false;
		//	cout<<"this destructor doesn't belong to this class\n";
			ErrorReport::printError(node->line_num,node->col_num,"this destructor doesn't belong to this class");
	
		}

		return;
	}

///////////////////////////////////////////////////////
/*class_decl:
	class_head ';'		{$$=new TreeRecord(NodeName::class_decl,$1);}

	a decleration of a class makes it available in the symbol table so it can be referenced
	when a matching definition is faced we preserve the same recortd but update the node reference only
	only check of previous decleration exists
	*/
	if(name_comp(node->node_name,NodeName::class_decl)){
		Record r(node->node1->node_name,cur_par->depth+1,cur_par,Type::class_type);
		Record* temp=symtab.findRecord(r);
		if(temp==NULL){ //new
			symtab.addRecord(r);
			return;
		}
		else{ //error
			isValid=false;
		//	cout<<"class already declared\n";
			ErrorReport::printError(node->line_num,node->col_num,"class already declared");
		}

		return;
	}
///////////////////////////////////////////////////////
/*
func_decl:
	storage_spec type ID_NAME '(' func_decl_args ')' ';'
			{$$=new TreeRecord(NodeName::func_decl,$1,$2,new TreeRecord($3),$5);}	
	|type ID_NAME '(' func_decl_args ')' ';'
			 {$$=new TreeRecord(NodeName::func_decl,$1,new TreeRecord($2),$4);	}
	|storage_spec type ID_NAME '(' func_decl_args ')' CONST ';'	
			 {$$=new TreeRecord(NodeName::func_decl,$1,$2,new TreeRecord($3),$5	);}	
	|type ID_NAME '(' func_decl_args ')' CONST ';'  
			{$$=new TreeRecord(NodeName::func_decl,$1,new TreeRecord($2),$4,new TreeRecord("CONST"));}			
	
	;	
*/
	/* check if it was already delared before with the same signature, allow multiple declerations
	  but with different signature (function overloading)
	  add the decleration to the symbol table if it's not there, set it's scope and access spec,
	 */
	if(name_comp(node->node_name,NodeName::func_decl)){
		TreeRecord* name_node;
		TreeRecord* arg_node;
		TreeRecord* storage_node=NULL;
		TreeRecord* type_node;
		bool const_node=false;
		if(name_comp(node->node1->node_name,NodeName::storage_spec)){
			name_node=node->node3;
			arg_node=node->node4;
			storage_node=node->node1;
			type_node=node->node2;
			if(node->node5)
				const_node=true;
		}
		else{ //without storage spec
			name_node=node->node2;
			arg_node=node->node3;
			type_node=node->node1;
			if(node->node4)
				const_node=true;
		}

		Record r(name_node->node_name,cur_par->depth+1,cur_par,Type::type_func,cur_access_spec);
		createFuncArgSignature(arg_node,r);
		Record* temp=symtab.addRecord(r); //use signature by default
		if(!temp){ //error NULL
			isValid=false;
		//	cout<<name_node->node_name<<" function with the same parameters already declared\n";
			ErrorReport::printError(node->line_num,node->col_num,"function with the same parameters already declared");			
		}
		else{ //set other info
			temp->access_type=cur_access_spec; //if it was a class func decl
			if(storage_node)
				temp->storage_type=storage_node->node_type;
			temp->isConstFunc=const_node; //set suffix type
			createTypeSignature(type_node,*temp,true); //set return type siganture
		}

		return;
	}
///////////////////////////////////////////////////////
/*
func_def:	
	func_def_inline		
	|func_def_normal		
	;
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
			{$$=new TreeRecord(NodeName::func_def_common,$1,new TreeRecord($2),$4);}
		|type ID_NAME '(' func_decl_args ')' CONST 		block_stmt
			{$$=new TreeRecord(NodeName::func_def_common,$1,new TreeRecord($2),new TreeRecord("CONST"),$4);}
		;

match the definition to previous decleration if possoible to update it's info
if new insert the entry in the symbol table
use signature
add argument variables to the symbol table as part of the function scope
*/
	if(name_comp(node->node_name,NodeName::func_def_inline)||name_comp(node->node_name,NodeName::func_def_normal)){
		TreeRecord* storage_node=NULL;
		TreeRecord* common_node;
		if(node->node2==NULL){
			common_node=node->node1;
		}
		else{
			common_node=node->node2;
			storage_node=node->node1;
		}
		
		cur_return_type=common_node->node1; //global


		Record r(common_node->node2->node_name,cur_par->depth+1,cur_par,Type::type_func,cur_access_spec);
		this->createFuncArgSignature(common_node->node3,r);
		this->createTypeSignature(common_node->node1,r,true);
		Record* temp=symtab.findRecord(r,false);
		cur_return_type->sig_list=r.ret_sig_list;


		if(temp){ //decl at least exists
			if(temp->ref_node==NULL){ //decleration exists, just update it, after matching the return type
				if(temp->ret_sig_list==r.ret_sig_list){
					//update reference node, add arguments to the symbol table
					temp->ref_node=node;
					this->addArgs2Symtab(common_node->node3,temp);
					if(common_node->node5!=NULL){ //const specifier
						temp->isConstFunc=true;
						//check the body
						cur_par=temp;
						checkTree(common_node->node5);
						cur_par=cur_par->parent;
					}
					else{
						cur_par=temp;
						checkTree(common_node->node4);
						cur_par=cur_par->parent;
					}
				}
				else{ //error
					cur_par=temp;
					isValid=false;
				//	cout<<"can't define the function with different return type as that of it's decleration\n";
					ErrorReport::printError(node->line_num,node->col_num,"can't define the function with different return type as that of it's decleration");	
				}
			}
			else{ //error
				isValid=false;
			//	cout<<"function definition already exists\n";
				ErrorReport::printError(node->line_num,node->col_num,"function definition already exists");	
			}
		}
		else{ //new
			temp=symtab.addRecord(r);
			temp->ref_node=node;
			this->addArgs2Symtab(common_node->node3,temp);
			if(common_node->node5!=NULL){ //const specifier
				temp->isConstFunc=true;
				//check the body
				cur_par=temp;
				checkTree(common_node->node5);
				cur_par=cur_par->parent;
			}
			else{
				cur_par=temp;
				checkTree(common_node->node4);
				cur_par=cur_par->parent;
			}
		}
		temp->createFuncName();
		common_node->codeName=temp->codeName;

		return;
	}

///////////////////////////////////////////////////////
/*
class_func_def:
	class_func_def_inline			
	|class_func_def_normal			
	;

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
void A::f(...){ }

class function definition differs in that it's parent record in the symbol table is the referencing class
and thus it's decleration has a class function scope
so we need to find the matching decleration in the same class that defines the function and do the rest as 
the normal function decleration

later implement enabling the redefinition of base class functions
	*/
	if(name_comp(node->node_name,NodeName::class_func_def_inline)||name_comp(node->node_name,NodeName::class_func_def_normal)){
		TreeRecord* storage_node=NULL;
		TreeRecord* common_node;
		if(node->node2==NULL){
			common_node=node->node1;
		}
		else{
			common_node=node->node2;
			storage_node=node->node1;
		}

		cur_return_type=common_node->node1; //global
	
		//find parent class record
		Record par_rec(common_node->node2->node_name,cur_par->depth+1,cur_par,Type::class_type);
		tempPar=symtab.findRecord(par_rec);
		if(tempPar==NULL){ //error
		//	cout<<"this function is not member of the defining class\n";
			ErrorReport::printError(node->line_num,node->col_num,"this function is not member of the defining class");	
			isValid=false;
			return;
		}
	//	cur_par=tempPar; //set the parent
		//do it like normal functions now
		
		Record r(common_node->node3->node_name,tempPar->depth+1,tempPar,Type::type_func);
		this->createFuncArgSignature(common_node->node4,r);
		this->createTypeSignature(common_node->node1,r,true); //set return type
		Record* temp=symtab.findRecord(r,false);
		
		cur_return_type->sig_list=r.ret_sig_list;


		if(temp){ //decl at least exists
			if(temp->ref_node==NULL){ //decleration exists, just update it, after matching the return type
				if(temp->ret_sig_list==r.ret_sig_list){
					//update reference node, add arguments to the symbol table
					temp->ref_node=node;
					this->addArgs2Symtab(common_node->node4,temp);
					if(common_node->node5!=NULL){ //const specifier
						temp->isConstFunc=true;
						//check the body
						cur_par=temp;
						checkTree(common_node->node5);
						cur_par=cur_par->parent->parent; //restore scope
					}
					else{
						cur_par=temp;
						checkTree(common_node->node5);
						cur_par=cur_par->parent->parent; //restore scope
					}
				
				}
				else{ //error
					isValid=false;
				//	cout<<"can't define the function with different return type as that of it's decleration\n";
					ErrorReport::printError(node->line_num,node->col_num,"can't define the function with different return type as that of it's decleration");
				}
			}
			else{ //error
				isValid=false;
			//	cout<<"function definition already exists\n";
				ErrorReport::printError(node->line_num,node->col_num,"function definition already exists");
			}
		}
		else{ //error
			isValid=false;
		//	cout<<"no decleration of such method\n";
			ErrorReport::printError(node->line_num,node->col_num,"no decleration of such method");
		}
		temp->createFuncName(common_node->node2->node_name);
		common_node->codeName=temp->codeName;

		return;
	}
///////////////////////////////////////////////////////
/*
stmts:
	stmt		
	|stmts stmt		{$$=new TreeRecord(NodeName::stmts,$1,$2);}
	;
*/
	if(name_comp(node->node_name,NodeName::stmts)){
		checkTree(node->node1);
		checkTree(node->node2);
		return;
	}

///////////////////////////////////////////////////////
/*
stmt:
	simple_stmt ';'		
	|compound_stmt		
	|error ';'			{$$=new TreeRecord(NodeName::stmt_error);}
	|error '}'			{$$=new TreeRecord(NodeName::stmt_error);}
	;
simple_stmt:
	var_decl		
	|assignment		
	|expr 		
	|jump_stmt      
	|cin_stmt			
	|cout_stmt
	|delete_stmt
	;
*/
	if(name_comp(node->node_name,NodeName::stmt_error)){
		isValid=false;
	//	cout<<"statement error\n";
		ErrorReport::printError(node->line_num,node->col_num,"statement error");
		return;
	}

	///////////////////////////////////////////////////////////////////////
	if(name_comp(node->node_name,NodeName::expr_stmt)){
		this->calcExprType(node->node1);
		/*
			now optimize, dangling expression case
		*/
		if(optimize){
			optimizeDanglingExpr(node->node1);
		}
		return;
	}

	////////////////////////////////////////////////////////////////////////
/*
delete_stmt:
	DELETE var_expr		{$$=new TreeRecord(NodeName::delete_stmt,$2);}
	|DELETE '[' ']' var_expr	{$$=new TreeRecord(NodeName::delete_stmt,$4);}
	;

	for now, var_expr must be a pointer to any type, pointer signature part exists at last
*/
	if(name_comp(node->node_name,NodeName::delete_stmt)){
		calcExprType(node->node1);
		int code=*(--node->node1->sig_list.end());
		if(code!=Type::pointer_type)
			ErrorReport::printError(node->line_num,node->col_num,"can't delete non pointer data");
		
		return;
	}
	///////////////////////////////////////////////////////////////////////
/*
cin_stmt:
	CIN			{$$=new TreeRecord(@1,NodeName::cin_stmt,$2);}
	;

cin_stmt_input_list:
	input_elem
	|cin_stmt_input_list input_elem	{$$=new TreeRecord(@1,NodeName::cin_stmt_input_list,$1,$2);}
	;
input_elem:
	INPUT_VAR_SYM var_expr	{$$=new TreeRecord(@1,NodeName::input_elem,$2);}
	;	
*/
	if(name_comp(node->node_name,NodeName::cin_stmt)){
		TreeRecord* list=node->node1;
		while(true){
			//final element
			if(name_comp(list->node_name,NodeName::input_elem)){
				this->calcExprType(list->node1);
				break;
			}
			this->calcExprType(list->node2->node1);
			list=list->node1;
		}
		return;
	}

//////////////////////////////////////////////////////////////////////////
/*
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
*/
	if(name_comp(node->node_name,NodeName::cout_stmt)){
		TreeRecord* list=node->node1;
		while(true){
			//final element
			if(name_comp(list->node_name,NodeName::output_elem)){
				this->calcExprType(list->node1);
				break;
			}
			this->calcExprType(list->node2->node1);
			list=list->node1;
		}
		return;
	}


///////////////////////////////////////////////////////
/*
var_decl:
	storage_spec type var_list		{$$=new TreeRecord(NodeName::var_decl,$1,$2,$3);}
	|type var_list					{$$=new TreeRecord(NodeName::var_decl,$1,$2);}
	;
			:
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

- store variable ids in the symbol table, set their common type signature, if it's not there already
*/
	if(name_comp(node->node_name,NodeName::var_decl)){
		TreeRecord* storage_node=NULL;
		TreeRecord* type_node;
		TreeRecord* list_node;
		if(node->node3){
			storage_node=node->node1;
			type_node=node->node2;
			list_node=node->node3;
		}
		else{
			type_node=node->node1;
			list_node=node->node2;
		}

		//now check the list

		//case when the list is a single constructor call with parameters:	A a(4);
		if(name_comp(list_node->node_name,NodeName::var_constructor_call)){
			//type then must be a class_type, we look for such a one
			//classes are declared in global scope
			Record r(type_node->node_name,symtab.first_scope->depth+1,symtab.first_scope,Type::class_type);
			Record* class_rec=symtab.findRecord(r);
			if(!class_rec){ //error, usually should never happen
				isValid=false;
			//	cout<<"no class exists in global scope\n";
				ErrorReport::printError(node->line_num,node->col_num,"no class exists in global scope");
				return;
			}
			//constructor record
			Record c_r(type_node->node_name,class_rec->depth+1,class_rec,Type::type_constructor);
		//	this->createFuncArgSignature(list_node->node2,c_r);
			this->createFuncCallArgSignature(list_node->node2,c_r);
			Record* func_rec=symtab.findRecord(c_r);
			if(!func_rec){ //error 
				isValid=false;
			//	cout<<"no matching constructor\n";
				ErrorReport::printError(node->line_num,node->col_num,"no matching constructor");
				return;
			}
			//match procedure(constructor) call args type signature to one in the class's (type)
			if(!matchArgs2Decl(list_node->node2,func_rec)){ //error
				isValid=false;
				//cout<<"constructor call for the variable decleration matches no one\n"; 
				ErrorReport::printError(node->line_num,node->col_num,"constructor call for the variable decleration matches no one");
				return;
			}
			
			//now add the variable to the symbol table if it's not there already
			Record temp(list_node->node1->node_name,cur_par->depth+1,cur_par,Type::type_var,cur_access_spec);
			Record* var_rec=symtab.findRecord(temp);
			if(var_rec!=NULL){ //error
				isValid=false;
				//cout<<"variable already declared\n";
				ErrorReport::printError(node->line_num,node->col_num,"variable already declared");
				return;
			}
			list_node->codeName=func_rec->codeName;
			var_rec=symtab.addRecord(temp); //must succeed
			this->createTypeSignature(type_node,*var_rec); //set type signature
			list_node->rec=var_rec; //reference back
			list_node->node1->rec=var_rec;

			return;
		}
			
		
		
		//var_list_elem|var_list ',' var_list_elem`
		while(true){
			if(name_comp(list_node->node_name,NodeName::var_list_elem)){
				if(!setVarListElem(list_node,type_node,storage_node))
					return;
				break;
			}
			if(!setVarListElem(list_node->node2,type_node,storage_node))
				return;
			list_node=list_node->node1;
		}

		return;
	}

///////////////////////////////////////////////////////
/*
class_static_init_var:
	type class_type SCOPE_DOTS ID_NAME '=' expr		{$$=new TreeRecord(NodeName::class_static_init_var,$1,$2,new TreeRecord($4),$6);}
	;

	check that ID_NAME is already declared, with the specified type in the class class_type
	update the intiial value and check expr type to atch that of ID_NAME
*/

	if(name_comp(node->node_name,NodeName::class_static_init_var)){
		//class definitions exists in first level global scope
		Record class_rec(node->node2->node_name,symtab.first_scope->depth+1,symtab.first_scope,Type::class_type);
		Record* temp=symtab.findRecord(class_rec);
		if(temp==NULL){ //should not happend
			isValid=false;
			return;
		}
		Record r(node->node3->node_name,temp->depth+1,temp,Type::type_var);
		this->createTypeSignature(node->node1,r);
		Record* var_rec=symtab.findRecord(r,false);
		if(var_rec==NULL){ //error
			isValid=false;
			//cout<<"no static member variable declared with such name in class "<<node->node2->node_name<<endl;
			ErrorReport::printError(node->line_num,node->col_num,"no static member variable declared with such name in class");
			return;
		}
		//check expr type
		this->calcExprType(node->node4);
		if(canAssignType(var_rec,node->node4))
			return;
		else{
			isValid=false;
		//	cout<<"assigned expr doesn't match variable type\n";
			ErrorReport::printError(node->line_num,node->col_num,"assigned expr doesn't match variable type");
			return;
		}
		return;
	}
///////////////////////////////////////////////////////
/*
class_destructor:
	class_type SCOPE_DOTS '~' class_type '(' ')' block_stmt		{$$=new TreeRecord(NodeName::class_destructor,$1,$7);}
	;
	like constructor but no arguments and init list

	- a destructor like: A::~A(){ stmts }
	- a destructor must match a declerated destructor in a defined class( match name and signature)
*/
	if(name_comp(node->node_name,NodeName::class_destructor)){
		//check the the constructor belongs to the class
		char* class_name=node->node1->node_name;
		if(!name_comp(class_name,node->node2->node_name)){ //error
			isValid=false;
		//	cout<<"this destructor is not member of the refered class\n";
			ErrorReport::printError(node->line_num,node->col_num,"this destructor is not member of the refered class");
			return;
		}
		//find parent first
		Record* parent=symtab.findRecord(class_name,cur_par->depth+1,cur_par,Type::class_type);

		Record temp(class_name,parent->depth+1,parent,Type::type_destructor);
		//find one with the same signature
		Record* decl=symtab.findRecord(temp,false);
		if(decl==NULL){ //no previous decleration, error
			isValid=false;
		//	cout<<"no previous decleration for this destructor\n";
			ErrorReport::printError(node->line_num,node->col_num,"no previous decleration for this destructor");
			return;
		}
		decl->ref_node=node; //set reference node and denote as definition
		//add argument variables to the symbol table with this function scope
		decl->createFuncName(class_name);
		node->rec=decl;
		//change scope
		cur_par=decl;
		//check body
		checkTree(node->node3);
		//restore scope into parent of parent( because we wen two levels down CLASS->func
		cur_par=cur_par->parent->parent;
		return;

	}


////////////////////////////////////////////////////////////////
	//just create a unique block name and scope
	if(name_comp(node->node_name,NodeName::compound_block_stmt)){
		Record r(Record::createUniqueName(),cur_par->depth+1,cur_par,Type::type_block);
		Record* temp=symtab.addRecord(r);
		if(temp==NULL){
			ErrorReport::printError(node->line_num,node->col_num,"couldn't create block statement\n");
			return;
		}	
		cur_par=temp;
		checkTree(node->node1);
		cur_par=temp->parent;
	}


///////////////////////////////////////////////////////
/*

jump_stmt:
	BREAK		{$$=new TreeRecord(@1,NodeName::jump_stmt,NULL,NULL,NULL,NULL,NULL,Type::break_type);}
	|RETURN expr	{$$=new TreeRecord(@1,NodeName::jump_stmt,$2,NULL,NULL,NULL,NULL,Type::return_expr_type);}
	|CONTINUE		{$$=new TreeRecord(@1,NodeName::jump_stmt,NULL,NULL,NULL,NULL,NULL,Type::continue_type);}
	|RETURN			{$$=new TreeRecord(@1,NodeName::jump_stmt,NULL,NULL,NULL,NULL,NULL,Type::return_type);}
	;

	assuming the use of these statements is valid within any block of info, the only check
	is done against the Return expr; statement
	returned expr must match that of the parent block if available.
	so we define a global variable to hold current possibe return type and we match against it
	thus variable is changed when a new function block is entered only.		
*/
	if(name_comp(node->node_name,NodeName::jump_stmt)){
		if(node->node_type==Type::return_type){ //current return type must be void
			if(cur_return_type->node_type==Type::type_void)
				return;
			else
				ErrorReport::printError(node->line_num,node->col_num,"you must return a value");
			return;
		}
		if(node->node1!=NULL){
			this->calcExprType(node->node1);
			if(this->canAssignType(cur_return_type,node->node1))
				return;
			else{ //error
				isValid=false;
			//	cout<<"return type mis matches definition\n";
				ErrorReport::printError(node->line_num,node->col_num,"return type mismatches definition");
			}

		}

		if(loops==0){
			ErrorReport::printError(node->line_num,node->col_num,"can't issue jump statement out side loops");
		}


		return;
	}
///////////////////////////////////////////////////////
/*

for_stmt:
	FOR '(' var_decl ';' expr ';' for_itr')' stmt			{$$=new TreeRecord(NodeName::for_stmt,$3,$5,$7,$9);}
	;

	check that the variable decleration is right
*/

	if(name_comp(node->node_name,NodeName::for_stmt)){
		checkTree(node->node1);
		this->calcExprType(node->node2);
		checkTree(node->node3);
		++loops;
		checkTree(node->node4);
		--loops;
		return;
	}
/////////////////////////////////////////////////////////
/*
if_stmt:
	IF '(' expr ')'  stmt			{$$=new TreeRecord(@1,NodeName::if_stmt,$3,$5);}			
	|IF '(' expr ')'  stmt ELSE stmt				{$$=new TreeRecord(@1,NodeName::if_else_stmt,$3,$5,$7);}		
	;
*/

	if(name_comp(node->node_name,NodeName::if_stmt)){
		calcExprType(node->node1);
		checkTree(node->node2);
		return;
	}

/////////////////////////////////////////////////////////
	if(name_comp(node->node_name,NodeName::if_else_stmt)){
		calcExprType(node->node1);
		checkTree(node->node2);
		checkTree(node->node3);
		return;
	}	
/////////////////////////////////////////////////////////
/*
	:
	WHILE '(' expr ')' stmt			{$$=new TreeRecord(@1,NodeName::while_stmt,$3,$5);}
	; 
*/
	if(name_comp(node->node_name,NodeName::while_stmt)){
		calcExprType(node->node1);
		++loops;
		checkTree(node->node2);
		--loops;
		return;
	}



///////////////////////////////////////////////////////
/*
for_itr:
	for_itr_elem				{$$=new TreeRecord(NodeName::for_itr,$1);}
	|for_itr ',' for_itr_elem	{$$=new TreeRecord(NodeName::for_itr,$1,$3);}
	;

for_itr_elem:
	expr		{$$=new TreeRecord(@1,NodeName::for_itr_elem_expr,$1);}
	|assignment
	;
	then check for_itr assignment components if they are rightly typed
*/
//check again, not well formed
	if(name_comp(node->node_name,NodeName::for_itr)){
		TreeRecord* list=node;
		if(node->node2==NULL)
			list=node->node1;
		while(true){
			//final elem
			if(name_comp(list->node_name,NodeName::assignment)||name_comp(list->node_name,NodeName::for_itr_elem_expr)){
				calcExprType(list->node1);
				return;
			}
			if(name_comp(list->node2->node_name,NodeName::assignment))
				checkTree(list->node2); 
			if(name_comp(list->node2->node_name,NodeName::for_itr_elem_expr))
				calcExprType(list->node2->node1);
			//no check for expr
			list=list->node1;

		}	
		return;
	}
///////////////////////////////////////////////////////
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

///////////////////////////////////////////////////////
/*
block_stmt:
	'{' '}'		{$$=new TreeRecord(NodeName::block_stmt);}
	|'{'  stmts '}' 
				{ $$=new TreeRecord(NodeName::block_stmt,$2);}	
	;
*/
	if(name_comp(node->node_name,NodeName::block_stmt)){
		if(node->node1==NULL)
			return;
		checkTree(node->node1);
	}
///////////////////////////////////////////////////////
/*
assignment: 
	var_expr '=' expr			{$$=new TreeRecord(NodeName::assignment,$1,$3,NULL,NULL,NULL,Type::ASSIGN_NORMAL);}
	|var_expr PLUS_EQ expr		{$$=new TreeRecord(NodeName::assignment,$1,$3,NULL,NULL,NULL,Type::ASSIGN_PLUS);}
	|var_expr MINUS_EQ expr		{$$=new TreeRecord(NodeName::assignment,$1,$3,NULL,NULL,NULL,Type::ASSIGN_MINUS);}
	|var_expr DIV_EQ expr		{$$=new TreeRecord(NodeName::assignment,$1,$3,NULL,NULL,NULL,Type::ASSIGN_DIV);}
	|var_expr MULT_EQ expr		{$$=new TreeRecord(NodeName::assignment,$1,$3,NULL,NULL,NULL,Type::ASSIGN_MULT);}
	;

	a statement
	match expr to var_expr
*/
	
	if(name_comp(node->node_name,NodeName::assignment)){
		//first we balance the operands

		this->calcExprType(node->node1);
		this->calcExprType(node->node2);

		
		if(node->node1->isConstData){
			ErrorReport::printError(node->line_num,node->col_num,"can't assign to constant data");
		}

		if(canAssignType(node->node1,node->node2))
			return;
		isValid=false;
		ErrorReport::printError(node->line_num,node->col_num,"can't assign, type mismatch");
		return;
	}


///////////////////////////////////////////////////////
/*
cast_expr:
	'(' type ')' expr	{$$=new TreeRecord(NodeName::cast_expr,$2,$4);}				
	;
*/

	if(name_comp(node->node_name,NodeName::cast_expr)){

	}

///////////////////////////////////////////////////////
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
/* return false if variable names dupicate in th node
	dealing with default values is dealt with later
*/
bool ProgTree::addArgs2Symtab(TreeRecord* node, Record* par){
	if(node->node1==NULL)
		return true;  //empty args
	TreeRecord* list=node->node1;
	Record* temp=NULL;
	while(true){
		//last element
		if(name_comp(NodeName::func_decl_arg_elem,list->node_name)){
			Record r(list->node2->node_name,par->depth+1,par,Type::type_var,cur_access_spec);
			createTypeSignature(list->node1,r);
			temp=symtab.addRecord(r);
			if(!temp){ //means it's been declared before in the same argument list
			//	cout<<"argument variabl redecleration\n";
				ErrorReport::printError(node->line_num,node->col_num,"argument variabl redecleration");
				return false;
			}
			//reference the ID back
			list->node2->rec=temp;
			break;
		}
		
		Record r(list->node2->node2->node_name,par->depth+1,par,Type::type_var,cur_access_spec);
		createTypeSignature(list->node2->node1,r);
		temp=symtab.addRecord(r);
		if(!temp){ //means it's been declared before in the same argument list
			ErrorReport::printError(node->line_num,node->col_num,"argument variabl redecleration");
			return false;
		}
		//reference id back
		list->node2->node2->rec=temp;
		list=list->node1;
	}
}

/* passing a type tree record we create a unique signature and set it
	for the relevant symbol table record
	for pointer type better to do it recursively for multiple pointing levels
	like int*** x;
	we use the cast the class record pointer into int to use it as signature and for
	later use(to access relative class from signature)
	*/
void ProgTree::createTypeSignature(TreeRecord* type_node,Record& r,bool setReturnType){
				list<int>* list;
				if(setReturnType)
					list=&r.ret_sig_list;
				else
					list=&r.sig_list;
				if(type_node->node_type==Type::pointer_type){
					list->push_front(Type::pointer_type);
					createTypeSignature(type_node->node1,r,setReturnType);
				}
				else{
					//check class
					if(type_node->node_type==Type::class_type){
						Record r(type_node->node_name,symtab.first_scope->depth+1,symtab.first_scope,Type::class_type);
						Record* temp=symtab.findRecord(r);
						if(temp!=NULL){
							int x=reinterpret_cast<int>(temp);
							list->push_front(x);
						}
					}
					else
						if(type_node->node_type==Type::reference_type)
							createTypeSignature(type_node->node1,r,setReturnType);
						else
							list->push_front(type_node->node_type);
				}
}

//set it to the type node
void ProgTree::createTypeSignature(TreeRecord* type_node){
				list<int>* list=&type_node->sig_list;
				if(type_node->node_type==Type::pointer_type){
					list->push_front(Type::pointer_type);
					createTypeSignature(type_node->node1);
				}
				else{
					//check class
					if(type_node->node_type==Type::class_type){
					//cast it's symbol table record to int
						Record r(type_node->node_name,symtab.first_scope->depth+1,symtab.first_scope,Type::class_type);
						Record* temp=symtab.findRecord(r);
						if(temp!=NULL){
							int x=reinterpret_cast<int>(temp);
							list->push_front(x);
						}
					}
					else
						if(type_node->node_type==Type::reference_type)
							createTypeSignature(type_node->node1);
						else
							list->push_front(type_node->node_type);
				}
}

/* function arguments signature unique generation
	case1: no defualt arguments, no complex types, one list is created
	case2: complex types included	
	case3: list of lists is to be created
	
	using sequence of integer values we call tell the signature of any argument
	as for class case: we set a static variable inside the symbol table which increments 
	on each insertion and sets an internal variabe in the symbol table record that identifies each
	record and thus gives a unique integer code for each class
	we start for such a code at high value differs from those in the noromal types
	for single types that are complex in such a way: int** x; we use three codes to identify 
	this one type: one for int anf two for pointer type
*/

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

	one definition multiple comparisions
	because we push the single types, each call to createTypeSignature stacks the signature elemet

*/
void ProgTree::createFuncArgSignature(TreeRecord* node ,Record& r){
	if(node->node1==NULL){ //no arguments
		return;
	}
	else{
		TreeRecord* temp=node->node1;
		while(true){
			//one final elem
			if(name_comp(temp->node_name,NodeName::func_decl_arg_elem)){
				//for now work on one level for pointers and references
				createTypeSignature(temp->node1,r);
				break;
			}
			//list
 			TreeRecord* elem_type=temp->node2->node1;
			createTypeSignature(elem_type,r);
			temp=temp->node1;
		}
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
*/
void ProgTree::createFuncCallArgSignature(TreeRecord* node ,Record& r){
	TreeRecord* temp=NULL;
	if(node->node1==NULL){ //no arguments
		return;
	}
	else{
		if(name_comp(node->node_name,NodeName::proc_expr_list))
			temp=node->node1;
		else
			temp=node;
		while(true){
			//one final elem
			if(temp->node2==NULL){
				//for now work on one level for pointers and references
				this->calcExprType(temp->node1);
				//now push expr type signature to the r sinature list
				r.sig_list.insert(r.sig_list.begin(),temp->node1->sig_list.begin(),temp->node1->sig_list.end());
				break;
			}
			//list
			this->calcExprType(temp->node2);		
			r.sig_list.insert(r.sig_list.begin(),temp->node2->sig_list.begin(),temp->node2->sig_list.end());
			temp=temp->node1;
		}
	}

}

/*
class_constructor_init_list:			{$$=new TreeRecord(NodeName::class_constructor_init_list);}
	|':' c_c_init_list					{$$=new TreeRecord(NodeName::class_constructor_init_list,$2);}
	|':' class_type '(' proc_expr_list_not_empty ')' {$$=new TreeRecord(@1,NodeName::class_constructor_init_list,$2,$4);}
	;
// member initialiation list:		A::A(int x,int y):f(x),f(y){...} 
c_c_init_list:
	ID_NAME '(' expr ')'				{$$=new TreeRecord(NodeName::c_c_init_list,new TreeRecord($1),$3);}
	|c_c_init_list ',' ID_NAME '(' expr ')'	{$$=new TreeRecord(NodeName::c_c_init_list,$1,new TreeRecord($3),$5);}
	;
*/
/*member initialiation list ( first case is handled outside this function
	basicly we need to check the expr type to match that of the member
	also to check that list elems are actually a member of the pretending class
	for now we just do type checking
*/
bool ProgTree::checkInitListVals(TreeRecord* node,Record* par){ //par is a class record
	/*one parent constructer allowed
	 - first check it's in the parent base list
	 - find constructer in class with such a signature
	
	class_base_list:
		class_type						
		|class_base_list ',' class_type	{$$=new TreeRecord(@1,NodeName::class_base_list,$1,$3);}
		|class_base_list ',' error		{$$=new TreeRecord(@1,NodeName::name_error);}
		;
	 */
	TreeRecord* baseList=par->ref_node->node3;
	TreeRecord* classType=node->node1;
	TreeRecord* args=node->node2;

	if(node->node2!=NULL){

		
		if(baseList!=NULL){
			if(baseList->node2!=NULL){
				ErrorReport::printError(node->line_num,node->col_num,"one aprent allowed\n");
				return false;
			}
			if(!name_comp(baseList->node_name,classType->node_name)){
				ErrorReport::printError(node->line_num,node->col_num,"base list constructor must be a parent one\n");
				return false;		
			}
		}
		
		//now find base list record
		Record r(classType->node_name,symtab.first_scope->depth+1,symtab.first_scope,Type::class_type);
		Record* base=symtab.findRecord(r);
		if(base==NULL){ //should not happen
			cout<<"class not found in global scope\n";
			ErrorReport::printError(node->line_num,node->col_num,"class not found in global scope");
			return false;
		}

		//find constructor with the same signature
		Record c(classType->node_name,base->depth+1,base,Type::type_constructor);
		//create signature from arg value lit
		this->createFuncCallArgSignature(args,c);

		Record* cRec=symtab.findRecord(c,false);
		if(cRec==NULL){
			ErrorReport::printError(node->line_num,node->col_num,"base list constructor not found\n");
			return false;			
		}

		node->codeName=cRec->codeName;
		return true;	
	}

	/* if base list does exist then base class must have a default constructor when
	explicit constructor is not called */
	
	if(node->node1==NULL && baseList!=NULL){
		if(baseList->node2!=NULL){
			ErrorReport::printError(node->line_num,node->col_num,"one parent allowed\n");
			return false;
		}
		
		Record r(baseList->node_name,symtab.first_scope->depth+1,symtab.first_scope,Type::class_type);
		Record* base=symtab.findRecord(r);
		if(base==NULL){ //should not happen
			cout<<"class not found in global scope\n";
			ErrorReport::printError(node->line_num,node->col_num,"class not found in global scope");
			return false;
		}
		
		args=new TreeRecord(0); //like empty args

	//find constructor with the same signature
		Record c(baseList->node_name,base->depth+1,base,Type::type_constructor);
	
		this->createFuncCallArgSignature(args,c);

		Record* cRec=symtab.findRecord(c,false);
		if(cRec==NULL){
			ErrorReport::printError(node->line_num,node->col_num,"no default constructor\n");
			return false;			
		}
		node->codeName=cRec->codeName;
		return true;	
	}


	if(node->node1==NULL)
		return true; //no member init list

	TreeRecord* list=node->node1;

	while(true){
		if(list->node3==NULL){ //final element in the list
			Record r(list->node1->node_name,par->depth+1,par,Type::type_var);
			Record* temp=symtab.findRecord(r);
			if(temp==NULL){
			//	cout<<"class initialiation list element "<<list->node1->node_name<<" is not member of relevant class\n";
				ErrorReport::printError(node->line_num,node->col_num,"class initialiation list element is not member of relevant class");

				return false;
			}
			calcExprType(list->node2);
			list->node1->rec=temp;
			if(list->node2->sig_list == temp->sig_list) //type match through signature
				return true;
			else
				return false;
		}
		
		Record r(list->node2->node_name,par->depth+1,par,Type::type_var);
		Record* temp=symtab.findRecord(r);
		if(temp==NULL){
		//	cout<<"class initialiation list element "<<list->node1->node_name<<" is not member of relevant class\n";
			ErrorReport::printError(node->line_num,node->col_num,"class initialiation list element is not member of relevant class");

			return false;
		}
		calcExprType(list->node3);
		list->node2->rec=temp;
		if(list->node3->sig_list != temp->sig_list) //type match through signature
			return false;


		list=list->node1;

	}
}

bool ProgTree::assignExpr2Var(TreeRecord* node, Record* var){ //var is the symbol table record to change

	return true;
}

bool ProgTree::calcExprVal(TreeRecord* node){
	
	return true;
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

	u can check an expr node not to check it's type again

	now all kind of rules that forms an expr some way or the other must be checked
	this method also do type checking, this is why it can be part of the checkTree too
	how ever it's the same.

*/
bool ProgTree::calcExprType(TreeRecord* node){
	if(name_comp(node->node_name,NodeName::bracket_expr	)){
		calcExprType(node->node1);
		node->sig_list=node->node1->sig_list;
	
		node->isConstData=node->node1->isConstData;
		if(node->isConstData)
			node->val=node->node1->val;
		return true;
	}		
/*
const_expr:
	INTEGER_CONST			{$$=new TreeRecord(NodeName::int_const_expr);}
	|DOUBLE_CONST			{$$=new TreeRecord(NodeName::double_const_expr);}
	|CHARACTER_CONST		{$$=new TreeRecord(NodeName::char_const_expr);}
	|STRING_CONST			{$$=new TreeRecord(NodeName::str_const_expr);}
	|FALSE_CONST			{$$=new TreeRecord(NodeName::false_const_expr);}
	|TRUE_CONST				{$$=new TreeRecord(NodeName::true_const_expr);}
	;
*/
	if(name_comp(node->node_name,NodeName::int_const_expr)){
		node->sig_list.push_front(Type::type_int);
		node->isConstData=true;
		return true;
	}

	if(name_comp(node->node_name,NodeName::double_const_expr)){
		node->sig_list.push_front(Type::type_double);
		node->isConstData=true;
		return true;
	}

	if(name_comp(node->node_name,NodeName::char_const_expr)){
		node->sig_list.push_front(Type::type_char);
		node->isConstData=true;
		return true;
	}
	//STRING is considerd char*
	if(name_comp(node->node_name,NodeName::str_const_expr)){
	//	node->sig_list.push_front(Type::type_str);
		node->sig_list.push_front(Type::pointer_type);
		node->sig_list.push_front(Type::type_char);
		node->isConstData=true;
		return true;
	}
	if(name_comp(node->node_name,NodeName::false_const_expr)){
		node->sig_list.push_front(Type::type_bool);
		node->isConstData=true;
		return true;
	}
	if(name_comp(node->node_name,NodeName::true_const_expr)){
		node->sig_list.push_front(Type::type_bool);
		node->isConstData=true;
		return true;
	}

	if(name_comp(node->node_name,NodeName::null_const_expr)){
		node->sig_list.push_front(Type::type_null);
		node->isConstData=true;
		return true;
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
	just dereference the array types the count that [] is menstioned, also the count of [] 
	must be less or equal to the array dim 
*/
	if(name_comp(node->node_name,NodeName::array_expr)){
		Record* var_rec=this->getRecordFromVarExpr(node,cur_par);
		if(var_rec==NULL){
			ErrorReport::printError(node->line_num,node->col_num,"no such array defined");
			return false;
		}
		if(var_rec->array_info.size()==0){
			ErrorReport::printError(node->line_num,node->col_num,"this pointer elems can't be accessed in this way");
			return false;
		}
		int count=0;		
		TreeRecord* dims=node->node2;
		while(true){
			if(dims->node2==NULL){ //final elem
				this->calcExprType(dims->node1);
				++count;
				break;
			}
			++count;			
			this->calcExprType(dims->node2);
			dims=dims->node1;
		}
		if(count>var_rec->array_info.size()){
			ErrorReport::printError(node->line_num,node->col_num,"the dimension of this array is less than u call");
			return false;
		}
		node->sig_list=var_rec->sig_list;
		for(int i=0;i<count;i++)
			node->sig_list.pop_back();
		node->rec=var_rec;
		return true;
		
	}
	/*array_expr '.' var_expr
		array_expr must be of class type
	*/
	if(name_comp(node->node_name,NodeName::array_class_expr)){
		this->calcExprType(node->node1);
		Record* var_rec=this->getRecordFromVarExpr(node,cur_par);
		if(var_rec==NULL)
			return false;
		node->sig_list=var_rec->sig_list;

		if(var_rec->access_type==Type::access_private){
			ErrorReport::printError(node->line_num,node->col_num,"can't access private members of a class");
			return false;

		}

		if(var_rec->access_type==Type::access_protected){
			ErrorReport::printError(node->line_num,node->col_num,"can't access protected members of class");
			return false;
		}
		if(node->node2->isConstData){
			node->isConstData=true;
			node->val=node->node2->val;
		}
		return true;

	}

	/*array_expr ARROW var_expr
		array_expr must be a pointer to a class type
	*/
	if(name_comp(node->node_name,NodeName::array_pointer_expr)){
		this->calcExprType(node->node1);
		Record* var_rec=this->getRecordFromVarExpr(node,cur_par);
		if(var_rec==NULL)
			return false;
		node->sig_list=var_rec->sig_list;

		if(var_rec->access_type==Type::access_private){
			ErrorReport::printError(node->line_num,node->col_num,"can't access private members of a class");
			return false;

		}

		if(var_rec->access_type==Type::access_protected){
			ErrorReport::printError(node->line_num,node->col_num,"can't access protected members of class");
			return false;
		}
		if(node->node2->isConstData){
			node->isConstData=true;
			node->val=node->node2->val;
		}
		return true;
	}


	/*
	pointer_val_expr:
	'*' var_expr	{$$=new TreeRecord(@1,NodeName::pointer_val_expr,$2);}	
	;
	it's dereferencing the var_expr so the type is the type of var_expr without the pointer type
	so we just pop the last number that indicates the pointer type
	*/
	if(name_comp(node->node_name,NodeName::pointer_val_expr)){
		this->calcExprType(node->node1);
		Record* rec=getRecordFromVarExpr(node,cur_par);
		if(rec==NULL)
			return false;
		node->sig_list=node->node1->sig_list;
		node->sig_list.pop_back();
		node->rec=rec;
		return true;
	}
	//ID_NAME
	if(name_comp(node->node_name,NodeName::id_name_expr)){
		//we got a variable, get it from symbol table and get it's type(signature)
		Record* temp=this->getRecordFromVarExpr(node,cur_par); //get variable starting from current
		//scope
		if(temp==NULL)
			return false;

		node->sig_list=temp->sig_list; //set type
		//check if it was const
		if((temp->storage_type==Type::storage_const)||(temp->storage_type==Type::storage_static_const)){
			node->isConstData=true;
		}
		node->val=temp->val;
		node->rec=temp;
		return true;
	}

	//THIS
	if(name_comp(node->node_name,NodeName::this_expr)){
		/*the type is a popinter to parent of parent: this is used inside class function def*/
		Record* rec=this->getRecordFromVarExpr(node,cur_par);
		if(rec==NULL)
			return false;
		
		node->sig_list=rec->sig_list;
		node->sig_list.push_back(Type::pointer_type);
		return true;
	}	

	//ID_NAME '.' var_expr    no other check made for now
	if(name_comp(node->node_name,NodeName::class_var_expr)){
		Record* temp=this->getRecordFromVarExpr(node,cur_par);
		if(temp==NULL)
			return false;
		node->node2->sig_list=temp->sig_list;
		node->sig_list=temp->sig_list;
		//check if it was const
		if((temp->storage_type==Type::storage_const)||(temp->storage_type==Type::storage_static_const)){
			node->isConstData=true;
		}
		if(temp->access_type==Type::access_private){
			ErrorReport::printError(node->line_num,node->col_num,"can't access private members of a class");
			return false;

		}

		if(temp->access_type==Type::access_protected){
			ErrorReport::printError(node->line_num,node->col_num,"can't access protected members of class");
			return false;
		}

		if(node->node2->isConstData){
			node->isConstData=true;

			node->val=node->node2->val;
		}
		return true;

	}

	//class_type SCOPE_DOTS ID_NAME
	if(name_comp(node->node_name,NodeName::scope_expr)){
		Record* var_rec=getRecordFromVarExpr(node,cur_par);
		if(var_rec==NULL)
			return false;	
		//check if it was static
		if((var_rec->storage_type!=Type::storage_static)&&(var_rec->storage_type!=Type::storage_static_const)){
		//	cout<<"non static variable can't be accessed via class name\n";
			ErrorReport::printError(node->line_num,node->col_num,"non static variable can't be accessed via class name");

			return false;
		}
		//now set type
		node->sig_list=var_rec->sig_list;
		//check if it was const
		if((var_rec->storage_type==Type::storage_const)||(var_rec->storage_type==Type::storage_static_const)){
			node->isConstData=true;
		}
				//check access
		if(var_rec->access_type==Type::access_protected){
			ErrorReport::printError(node->line_num,node->col_num,"Can't access protected data from outside class scope");
			return false;
		}
		return true;
	}

	//ID_NAME ARROW var_expr
	if(name_comp(node->node_name,NodeName::id_arrow_expr)){
		Record* temp=this->getRecordFromVarExpr(node,cur_par);
		if(temp==NULL)
			return false;
		
		node->sig_list=temp->sig_list;
		node->node2->sig_list=node->sig_list;
		//check if it was const
		if((temp->storage_type==Type::storage_const)||(temp->storage_type==Type::storage_static_const)){
			node->isConstData=true;
		}
		//check access
		if(temp->access_type==Type::access_protected){
			ErrorReport::printError(node->line_num,node->col_num,"can't access protected data  outside class scope");
			return false;
		}

		if(temp->access_type==Type::access_private){
			ErrorReport::printError(node->line_num,node->col_num,"can't access private members of a class");
			return false;

		}
		if(node->node2->isConstData){
			node->isConstData=true;
			node->val=node->node2->val;
		}
		return true;
	}

	//THIS ARROW var_expr, no other check made
	if(name_comp(node->node_name,NodeName::this_arrow_expr)){
		calcExprType(node->node1);
		Record* temp=this->getRecordFromVarExpr(node,cur_par->parent);
		node->sig_list=node->node1->sig_list;
		if(temp==NULL){
			ErrorReport::printError(node->line_num,node->col_num,"expression is not member of this variable");
			return false;
		}
		//check if it was const
		if((temp->storage_type==Type::storage_const)||(temp->storage_type==Type::storage_static_const)){
			node->isConstData=true;
		}
		if(node->node1->isConstData){
			node->isConstData=true;
			node->val=node->node1->val;
		}
		return true;
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

	type doesn't change only for the referencing unary var_expr
	the type of the reference in a pointer to such type  which expr must be a var_expr

	int x=0;
	int* y=&x; //ok
	int& v=y; //error
	int& v=x; //ok
	when we define a reference to a variable we define another name for the same variable nothing
	more
	a reference must be done against volatile variables only not against consts
*/
	if(name_comp(node->node_name,NodeName::unary_expr)){
		calcExprType(node->node1);
		if(node->node_type==Type::and){
			if(!this->getRecordFromVarExpr(node->node1,cur_par)){
				ErrorReport::printError(node->line_num,node->col_num,"can only reference a variable experssion\n");
				return false;	
			}
		}
		
		node->sig_list=node->node1->sig_list;
		if(node->node_type==Type::and){
			node->sig_list.push_back(Type::pointer_type);
		}
		node->isConstData=node->node1->isConstData;
		if(!node->isConstData)
			return true;
		node->val=node->node1->val;
		calcUnaryConstData(node);
		
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
		if implicit conversion is applicable, then we convert to the most common part
		no extra nodes are added, direct change is applied to the tree
*/
	if(name_comp(node->node_name,NodeName::math_operation_expr)){
		calcExprType(node->node1);
		calcExprType(node->node2);
		TreeRecord* mostCommon=balanceExprType(node->node1,node->node2);
		if(!mostCommon){
		//	cout<<"can't balance";
			ErrorReport::printError(node->line_num,node->col_num,"can't balance types for binary operation");
			return false;
		}
		//expr type is the most common side
		node->sig_list=mostCommon->sig_list;
		node->isConstData=node->node1->isConstData&&node->node2->isConstData;
		
		/*if one expr is const and the other is not then check for 0 operand
			expr * 0=0		is const
			0/expr=0		const
			0 * expr =0		const
		*/
		if(checkMultZero(node))
			return true;
		//both expressions must be constants for the whole to be
		if(!node->isConstData)
			return true;
		calcMathConstData(node);
		return true;
	}

/*
logical_operation_expr
	:
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
	if(name_comp(node->node_name,NodeName::logical_operation_expr)){
		calcExprType(node->node1);
		calcExprType(node->node2);
		if(!balanceExprType(node->node1,node->node2)){
			ErrorReport::printError(node->line_num,node->col_num,"can't balance types for binary operation");
			return false;
		}
		node->sig_list.push_front(Type::type_bool); //boolean
		node->isConstData=node->node1->isConstData&&node->node2->isConstData;

		//both expressions must be constants for the whole to be
		if(!node->isConstData)
			return true;
		calcLogicConstData(node);

		return true;
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
	if(name_comp(node->node_name,NodeName::new_array_expr)){
		calcExprType(node->node2);
		//type of expr must be integeral const
		if(*(node->node2->sig_list.begin())==Type::type_int){
			//type of the returned expr is a pointer to type
			Record r("");
			createTypeSignature(node->node1,r);
			node->sig_list=r.sig_list;
			node->sig_list.push_back(Type::pointer_type);
			return true;
		}
	//	cout<<"array count must be integer\n";
		ErrorReport::printError(node->line_num,node->col_num,"array count must be integer");
		return false;
	}

	/*	NEW type '(' proc_expr_list ')'
		must match a constructor decleration and signature, returned type is pointer to class
		type	
		type can be class type the proc_expr_list is the constructor signature
		if it was a simple type then proc_expr_list must be one single simple value
		

	*/
	if(name_comp(node->node_name,NodeName::new_expr)){
		if(name_comp(node->node1->node_name,NodeName::simple_type)){
			createTypeSignature(node->node1);
			//then only one expr should be inside the brackets and of the same type of type or convertable
			TreeRecord* expr_list=node->node2;
			if(expr_list->node1==NULL){ //empty case
				ErrorReport::printError(expr_list->line_num,expr_list->col_num,"empty initialiation not allowed");
				return false;
			}
			TreeRecord* expr=expr_list->node1;
			if(expr->node2!=NULL){ //more than one element case
				ErrorReport::printError(expr_list->line_num,expr_list->col_num,"one initialiation element allowed");
				return false;
			}
			expr=expr->node1;
			bool i=calcExprType(expr);
			if(this->castExpr2Type(node->node1,expr)){
				node->sig_list=expr->sig_list;
				node->sig_list.push_back(Type::pointer_type);
				return true;
			}
			return false;
		}
		Record r(node->node1->node_name,symtab.first_scope->depth+1,symtab.first_scope,Type::class_type);
		Record* par=symtab.findRecord(r);
		if(par==NULL){ //should not happen
			cout<<"class not found in global scope\n";
			ErrorReport::printError(node->line_num,node->col_num,"class not found in global scope");
			return false;
		}
		Record c(node->node1->node_name,par->depth+1,par,Type::type_constructor);
		//create signature from arg value lit
		this->createFuncCallArgSignature(node->node2,c);
		Record* temp=symtab.findRecord(c,false); //use signature

		if(temp==NULL){ //try the global scope
		//	cout<<"no matching constructor\n";
			ErrorReport::printError(node->line_num,node->col_num,"no matching constructor");

			return false;
		}
	
		if(temp->access_type==Type::access_protected){
        	ErrorReport::printError(node->line_num,node->col_num,"can't access protected constructor");
			return false;
		}
		if(temp->access_type==Type::access_private){
        	ErrorReport::printError(node->line_num,node->col_num,"can't access private constructor");
			return false;
		}
		//set node type as pointer to class type
		node->sig_list.push_front(Type::pointer_type);
		node->sig_list.push_front(*(par->sig_list.begin()));
		node->rec=temp; //for constructor
		node->node1->rec=par; //for class
		return true;		
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
	|ID_NAME ARROW ID_NAME '(' proc_expr_list ')'							
		{$$=new TreeRecord(@1,NodeName::proc_call,new TreeRecord(@1,$1),new TreeRecord(@3,$3),$5,NULL,NULL,Type::PROC_ARROW_CALL);}
	;  
	the right thing is to define the proc call as
	var_expr '(' proc_expr_list ')'
	but now we use this for ease of the job

	a call must be done against an existing decleration
	the argument list signature must match that of decleration

	for the case: ID_NAME '.' ID_NAME  
		find a record of class_type and with the same type signature and check against (unknown class name)
		take into consideration that the variable decleration could happen from higer scopes so we 
		take the right one through the parent chain(recursive chaining).

		when declaring a variable we only search inside the current scope, because we allow decleration
		through multiple scopes
		how ever when using a variable we should check it's being from the current scope and tell the
		global scope, through the parent relation ships only.

	first of all we find the parent of the function

	check the access_spec if access is allowed

	new method to get the class record: after changin type signature to include the integer value
	of class record, we get the parent directly


*/

	if(name_comp(node->node_name,NodeName::proc_call)){
		//find prev decl
		TreeRecord* args_node=NULL;
		TreeRecord* func_name=NULL;
		Record* func_par=NULL;
		bool isStatic=false;

		//ID_NAME '(' proc_expr_list ')'	
		if(node->node_type==Type::proc_call_normal){
			args_node=node->node2;
			func_name=node->node1;
			//normal functions is declared in global scope or in the current parent scope(class function def case)
			//class func def scope
			func_par=cur_par->parent;
			Record r(func_name->node_name,func_par->depth+1,func_par,Type::type_func);
			//create signature from arg value lit
			this->createFuncCallArgSignature(args_node,r);
			Record* temp=symtab.findFuncRec(&r,func_par,false);

			if(temp==NULL){
				isValid=false;
				ErrorReport::printError(node->line_num,node->col_num,"call to undefined procedure");

				return false;
			}
			node->rec=temp;
			node->sig_list=temp->ret_sig_list;
			return true;
		}

		//ID_NAME '.' ID_NAME '(' proc_expr_list ')'
		if(node->node_type==Type::PROC_CLASS_CALL){
			args_node=node->node3;
			func_name=node->node2;
			//now find parent
			Record* r=symtab.findVarRec(node->node1->node_name,cur_par);
			node->node1->rec=r;
			if(r==NULL){//error
				isValid=false;
			//	cout<<"variable not declared\n";
				ErrorReport::printError(node->line_num,node->col_num,"variable not declared");
				return false;
			}
			r->isUsed=true;
			/*
			//find the class_type of the signature that matches that of the variable
			Record* func_par=symtab.findClassFromVar(r);
			*/
			func_par=reinterpret_cast<Record*>(*(r->sig_list.begin()));
			if(func_par==NULL){ //error   should not happen 
                isValid=false;
			//	cout<<"couldn't find the class\n";
				ErrorReport::printError(node->line_num,node->col_num,"couldn't find the class");
				return false;
			}
			Record f(node->node2->node_name,func_par->depth+1,func_par,Type::type_func); //record will be updated later
			//create signature from arg value lit
			this->createFuncCallArgSignature(args_node,f);
			Record* def=symtab.findFuncRec(&f,func_par,false);
			if(def==NULL){
                isValid=false;
				ErrorReport::printError(node->line_num,node->col_num,"couldn't find the function");
				return false;
			}
			if(def->access_type==Type::access_protected){
				ErrorReport::printError(node->line_num,node->col_num,"can't access protected members");
				return false;
			}

			if(def->access_type==Type::access_private){
				ErrorReport::printError(node->line_num,node->col_num,"can't access private function");
				return false;
			}

			node->rec=def;
			node->sig_list=def->ret_sig_list;
			return true;
		}

		//class_type SCOPE_DOTS ID_NAME '(' proc_expr_list ')'
		if(node->node_type==Type::PROC_CLASS_STATIC_CALL){
			//find parent, a class in the global scope with name class_type			
			Record r(node->node1->node_name,symtab.first_scope->depth+1,symtab.first_scope,Type::class_type);
			func_par=symtab.findRecord(r);
			if(func_par==NULL){ //error
				isValid=false;
			//	cout<<"function is nor member of class\n";
				ErrorReport::printError(node->line_num,node->col_num,"class not defined in global scope");
				return false;
			}
			Record f(node->node2->node_name,func_par->depth+1,func_par,Type::type_func);
			//create signature from arg value lit
			this->createFuncCallArgSignature(args_node,f);

			Record* temp=symtab.findFuncRec(&f,func_par,false);
		
			if((temp->storage_type!=Type::storage_static)&&(temp->storage_type!=Type::storage_static_const)){	
				//error
				isValid=false;
			//	cout<<"call to unstatic function is not permitted through class it self\n";
				ErrorReport::printError(node->line_num,node->col_num,"call to unstatic function is not permitted through class it self");
				return false;
			}
			if(temp->access_type==Type::access_protected){
				ErrorReport::printError(node->line_num,node->col_num,"can't access protected function");
				return false;
			}

			if(temp->access_type==Type::access_private){
				ErrorReport::printError(node->line_num,node->col_num,"can't access private function");
				return false;
			}


			node->rec=temp;
			node->sig_list=temp->ret_sig_list;
			return true;

		}
		
		//THIS ARROW ID_NAME '(' proc_expr_list ')'
		if(node->node_type==Type::PROC_THIS_CALL){
			func_par=cur_par->parent;
			
			Record f(node->node1->node_name,func_par->depth+1,func_par,Type::type_func);
			//create signature from arg value lit
			this->createFuncCallArgSignature(args_node,f);

			Record* temp=symtab.findFuncRec(&f,func_par,false);
			if(temp==NULL){
				ErrorReport::printError(node->line_num,node->col_num,"call to none member function");
				return false;
			}

			if(temp->access_type==Type::access_private){
				ErrorReport::printError(node->line_num,node->col_num,"can't access private function");
				return false;
			}
			node->rec=temp;
			node->sig_list=temp->ret_sig_list;
			return true;
		}


		//ID_NAME ARROW ID_NAME '(' proc_expr_list ')'
		if(node->node_type==Type::PROC_ARROW_CALL){
			args_node=node->node3;
			func_name=node->node2;
			//now find parent
			Record* r=symtab.findVarRec(node->node1->node_name,cur_par);
			node->node1->rec=r;
			if(r==NULL){//error
				isValid=false;
			//	cout<<"variable not declared\n";
				ErrorReport::printError(node->line_num,node->col_num,"variable not declared");
				return false;
			}
			r->isUsed=true;
			/*
			//find the class_type of the signature that matches that of the variable
			Record* func_par=symtab.findClassFromVar(r);
			*/
			//first ID_NAME must be a pointer to a class type
			int t=0;
			t=*(--(r->sig_list.end()));
			if(t!=Type::pointer_type){
				ErrorReport::printError(node->line_num,node->col_num,"left side must be pointer to a cass type");
				return false;
			}

			func_par=reinterpret_cast<Record*>(*(r->sig_list.begin()));
			if(func_par==NULL){ //error   should not happen 
                isValid=false;
			//	cout<<"couldn't find the class\n";
				ErrorReport::printError(node->line_num,node->col_num,"couldn't find the class");
				return false;
			}
			Record f(node->node2->node_name,func_par->depth+1,func_par,Type::type_func); //record will be updated later
			//create signature from arg value lit
			this->createFuncCallArgSignature(args_node,f);

			Record* def=symtab.findFuncRec(&f,func_par,false);
			if(def==NULL){
                isValid=false;
				ErrorReport::printError(node->line_num,node->col_num,"couldn't find the function");
				return false;
			}
			if(def->access_type==Type::access_protected){
                isValid=false;
				ErrorReport::printError(node->line_num,node->col_num,"can't access protected function");
				return false;
			}

			if(def->access_type==Type::access_private){
            	ErrorReport::printError(node->line_num,node->col_num,"can't access private function");
				return false;
			}

			node->rec=def;
			node->sig_list=def->ret_sig_list;
			return true;
		}


		return true;
	}

/*
cast_expr:
	'(' type ')' expr	{$$=new TreeRecord(NodeName::cast_expr,$2,$4);}				
	;
*/
	if(name_comp(node->node_name,NodeName::cast_expr)){
		calcExprType(node->node2);
		if(!castExpr2Type(node->node1,node->node2)){
		//	cout<<"can't case\n";
			ErrorReport::printError(node->line_num,node->col_num,"can't case");

			return false;
		}
		node->sig_list=node->node2->sig_list; //now the casted expr holds the casted type
		return true;
	}
	return true;
}

/*
proc_expr_list:						{$$=new TreeRecord(NodeName::proc_expr_list);}
	|proc_expr_list_not_empty		{$$=new TreeRecord(NodeName::proc_expr_list,$1);}		
	;
proc_expr_list_not_empty:
	expr
	|proc_expr_list_not_empty ',' expr	{$$=new TreeRecord(NodeName::proc_expr_list_not_empty,$1,$3);}
	;
*/
bool ProgTree::matchArgs2Decl(TreeRecord* callArgs,Record* func_rec){

	return true;
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
when have to discuss all 4 cases, check if similar variabl exists with the same name only
if not add it to the symbol table with accompanying info

we the variable declared is a reference type then we must insure that it's intitialized
besides set the reference field of it's symbol table proberly.

	*/
bool ProgTree::setVarListElem(TreeRecord* elem_node,TreeRecord* type_node,TreeRecord* storage_node){
	//check previous decl
	Record r(elem_node->node1->node_name,cur_par->depth+1,cur_par,Type::type_var,cur_access_spec);
	Record* var=symtab.findRecord(r);

	if(var!=NULL){ //already declared
		isValid=false;
	//	cout<<"variable already declared\n";
		ErrorReport::printError(elem_node->line_num,elem_node->col_num,"variable already declared");
		return false;
	}
	this->createTypeSignature(type_node,r); //initially
	if(storage_node!=NULL){ //set storage info
		r.storage_type=storage_node->node_type;
	}
	Record* array_rec=symtab.addRecord(r);
	elem_node->rec=array_rec;
	elem_node->node1->rec=array_rec;
	if(array_rec==NULL){ //never happens, however
		ErrorReport::printError(elem_node->line_num,elem_node->col_num,"couldn't add array to symtab");
		return false;
	}
	if(elem_node->node2==NULL){
		//check reference
		if(name_comp(type_node->node_name,NodeName::reference_type)){
			ErrorReport::printError(elem_node->line_num,elem_node->col_num,"Reference variables must be initialized");
			return false;
		}
		//if it was cost then initialization is required
		if((r.storage_type==Type::storage_const)||(r.storage_type==Type::storage_static_const)){
			if(cur_par->type!=Type::class_type){
				ErrorReport::printError(elem_node->line_num,elem_node->col_num,"u must initialize const varaibles");
				return false;
			}
		}
		return true; //just simple variable decleration
	}
	//now set array dim info
	if(name_comp(elem_node->node2->node_name,NodeName::array_dims)||name_comp(elem_node->node2->node_name,NodeName::array_dim)){
		// we got array variable decleration, 
/*
array_dims: 
	array_dim
	| array_dims array_dim	{$$=new TreeRecord(NodeName::array_dims,$1,$2);}
	;

array_dim:
	'[' ']'					{$$=new TreeRecord(NodeName::array_dim);}
	|'[' INTEGER_CONST ']'	{$$=new TreeRecord(NodeName::array_dim,new TreeRecord($2));}
	;
	we are gonna check the array_dims node, check type, add array info to the var rec
	for each dimension we got: <dim number, dim size, type signature, data pointer> so we store it in a list
	we begin from 0 for dims

	a separate field in the symbol table record to hold array info:
	dim  dim_size  sig_list( the type of current element) and data pointer
	we define the final element as the basic type as for consecutive dims we define them as pointer to the previous dim into:

	for example:		int a[3][2][2];
			a[1][1][1] is of type int
			a[1][1]    is of type int*
			a[1]	     is of type int**;
			a 	     is of type a***;
	thus array or pointer to same type are the same.
	As for data access it’s not safe and done by shifting from current pointer. 

	INTEGER_CONST must be a positive number greater than 0
*/
		TreeRecord* dim_node=elem_node->node2;
		int dim_size=0;
		int dim=0;
		list<int> prev_type=array_rec->sig_list; //basic type for last element
		while(true){ //check the dimensions
			dim_size=0;
			if(name_comp(dim_node->node_name,NodeName::array_dim)){ //final dim decl
				//just check for positive const integeral dim
				if(dim_node->node1!=NULL){
					//now check positive value of dim
					dim_size=atoi(dim_node->node1->node_name);
					if(dim_size<=0){
						ErrorReport::printError(dim_node->node1->line_num,dim_node->node1->col_num,
							"dimension of array must positive integer value greater than 0");
					}
				}
				ArrayInfo dim_info(dim,dim_size,prev_type,0);
				array_rec->array_info.push_front(dim_info); //0 means not inited
				prev_type.push_back(Type::pointer_type); //update dim type by adding one pointer level
				break;
			}
			//just check for positive const integeral dim and set dim type
			TreeRecord* temp=dim_node->node2;
			if(temp->node1!=NULL){
				dim_size=atoi(temp->node1->node_name);
				if(dim_size<=0){
					ErrorReport::printError(dim_node->node1->line_num,dim_node->node1->col_num,
							"dimension of array must positive integer value greater than 0");
				}
				ArrayInfo dim_info(dim,dim_size,prev_type,0);
				array_rec->array_info.push_front(dim_info);
				prev_type.push_back(Type::pointer_type); //update dim type by adding one pointer level

			}
			dim_node=dim_node->node1;
			++dim;
		}
		//now update the array_rec with the right type (multi pointer to a type)
		array_rec->sig_list=prev_type;
		//since we start from the end to the beginging we'll reverse the dims
		list<ArrayInfo>::iterator itr;
		int d=0;
		for(itr=array_rec->array_info.begin();itr!=array_rec->array_info.end();itr++){
			itr->dim=d;
			++d;
		}

		if(elem_node->node3==NULL){ //just decleration
			return true;
		}
		else{ //delceration puls initialiation of array
			return checkArrayInit(array_rec,0,0,elem_node->node3,elem_node);
		}
	}
	else{ //ordinary varaible decleration with assigned expr, check expr type
		if(cur_par->type==Type::class_type){
			ErrorReport::printError(elem_node->line_num,elem_node->col_num,"can't assign values inside class definition");
			return false;
		}
		this->calcExprType(elem_node->node2);
			if(canAssignType(&r,elem_node->node2)){

			if((r.storage_type==Type::storage_const)||(r.storage_type==Type::storage_static_const)){
				//then the assigned expr must be a constant value
				if(!elem_node->node2->isConstData){
					ErrorReport::printError(elem_node->line_num,elem_node->col_num,"u must initialize const varaibles with const data only");
					return false;
				}
				elem_node->rec->val=elem_node->node2->val;
			}

			//check reference, if yes then set the reference record
			if(name_comp(type_node->node_name,NodeName::reference_type)){
				r.isReference=true;
				r.reference=getRecordFromVarExpr(elem_node->node2,cur_par);
				if(r.reference==NULL){
					ErrorReport::printError(elem_node->line_num,elem_node->col_num,"can't reference non variables");
					return false;
				}
			}
			
			return true;
		}
		ErrorReport::printError(elem_node->line_num,elem_node->col_num,"expr type doesn't match variable decleration");
		return false;
	}

	//check the initialiation to have the same type
//	if(name_comp(elem_node->node_name,NodeName::expr
//	this->calcExprType(elem_node->node2);
}


/*
	we use this method to check if we can assign an expr of specific type to varaibe
	of the same or different type
	used in type checkin phase
	for now we just allow the same types to be assignable
	we assume the type of expr has lready been calculated
*/
bool ProgTree::canAssignType(Record* r,TreeRecord* expr){
	if(r->sig_list==expr->sig_list){
		return true;
	}

	int rightBegin=*(expr->sig_list.begin());
	int rightEnd=*(--r->sig_list.end());
	if((rightEnd==Type::pointer_type) && (rightBegin==Type::type_null )){
		return true;
	}
	if(*(r->sig_list.begin())==Type::type_int){
		if(*(expr->sig_list.begin())==Type::type_double){
			//cast and warn
			//cout<<"warning lose of precision when cast from double to int\n";
			ErrorReport::printWarning(expr->line_num,expr->col_num,"warning lose of precision when cast from double to int");

			//change the signature and value only on const data
	//		expr->sig_list=var_expr->sig_list;
			//make the cast
	//		expr->val.i_val=(int)expr->val.d_val;
			return true;
		}
		else
		if(*(expr->sig_list.begin())==Type::type_bool){
		//	expr->sig_list=var_expr->sig_list;
		//	expr->val.i_val=(int)expr->val.b_val;
			return true;
		}
		else
			if(*(expr->sig_list.begin())==Type::type_char){
		//		expr->sig_list=var_expr->sig_list;
		//		expr->val.i_val=(int)expr->val.c_val;
				return true;
			}
			else{
				ErrorReport::printError(expr->line_num,expr->col_num,"type mismatch");
				return false; //no others permited
			}
	}
	else
	if(*(r->sig_list.begin())==Type::type_double){
		if(*(expr->sig_list.begin())==Type::type_int){
		//	expr->sig_list=var_expr->sig_list;
		//	expr->val.d_val=(double)expr->val.i_val;
			return true;
		}
		else
		if(*(expr->sig_list.begin())==Type::type_char){
		//	expr->sig_list=var_expr->sig_list;
		//	expr->val.d_val=(double)expr->val.c_val;
			return true;
		}
		else
			if((*(expr->sig_list.begin())==Type::type_bool)){
		//		expr->sig_list=var_expr->sig_list;
		//		expr->val.d_val=(double)expr->val.b_val;
				return true;
			}
			else{
				ErrorReport::printError(expr->line_num,expr->col_num,"type mismatch");
				return false; //no others permited
			}	
	} 
	return false;
}

//also does an implicit type conversion
bool ProgTree::canAssignType(TreeRecord* var_expr,TreeRecord* expr){
	if(var_expr->sig_list==expr->sig_list){
		return true;
	}

	//if right side is null then left side can be any pointer
	
	int rightBegin=*(expr->sig_list.begin());
	int rightEnd=*(--var_expr->sig_list.end());
	if((rightEnd==Type::pointer_type) && (rightBegin==Type::type_null )){
		return true;
	}
	

	//implicitly cast
   	if(*(var_expr->sig_list.begin())==Type::type_int){
		if(*(expr->sig_list.begin())==Type::type_double){
			//cast and warn
			//cout<<"warning lose of precision when cast from double to int\n";
			ErrorReport::printWarning(expr->line_num,expr->col_num,"warning lose of precision when cast from double to int");

			//change the signature and value only on const data
	//		expr->sig_list=var_expr->sig_list;
			//make the cast
	//		expr->val.i_val=(int)expr->val.d_val;
			return true;
		}
		else
		if(*(expr->sig_list.begin())==Type::type_bool){
		//	expr->sig_list=var_expr->sig_list;
		//	expr->val.i_val=(int)expr->val.b_val;
			return true;
		}
		else
			if(*(expr->sig_list.begin())==Type::type_char){
		//		expr->sig_list=var_expr->sig_list;
		//		expr->val.i_val=(int)expr->val.c_val;
				return true;
			}
			else{
				ErrorReport::printError(var_expr->line_num,var_expr->col_num,"type mismatch");
				return false; //no others permited
			}
	}
	else
	if(*(var_expr->sig_list.begin())==Type::type_double){
		if(*(expr->sig_list.begin())==Type::type_int){
		//	expr->sig_list=var_expr->sig_list;
		//	expr->val.d_val=(double)expr->val.i_val;
			return true;
		}
		else
		if(*(expr->sig_list.begin())==Type::type_char){
		//	expr->sig_list=var_expr->sig_list;
		//	expr->val.d_val=(double)expr->val.c_val;
			return true;
		}
		else
			if((*(expr->sig_list.begin())==Type::type_bool)){
		//		expr->sig_list=var_expr->sig_list;
		//		expr->val.d_val=(double)expr->val.b_val;
				return true;
			}
			else{
				ErrorReport::printError(var_expr->line_num,var_expr->col_num,"type mismatch");
				return false; //no others permited
			}	
	} 
	return false;
}

/* cast the one expr type to the other that is most common if type mismatches 
	if both have the same type just return
	conversion only done between simple types only: int,double,bool,char
	can convert to int from double-char-bool
				to double from int-char-bool
	double is most common

	
*/
TreeRecord* ProgTree::balanceExprType(TreeRecord* expr1,TreeRecord* expr2){

	//applicabale for pointers too
	if(expr1->sig_list==expr2->sig_list)
		return expr1;

	int rightBegin=*(expr2->sig_list.begin());
	int rightEnd=*(--expr1->sig_list.end());
	if((rightEnd==Type::pointer_type) && (rightBegin==Type::type_null )){
		return expr1;
	}
		
	if(*(expr1->sig_list.begin())==Type::type_int){
		if(*(expr2->sig_list.begin())==Type::type_double){
		//	expr1->sig_list=expr2->sig_list;
		//	expr1->val.d_val=(double)expr1->val.i_val;
			return expr2;
		}
		else
		if(*(expr2->sig_list.begin())==Type::type_bool){
		//	expr2->sig_list=expr1->sig_list;
		//	expr2->val.i_val=(int)expr1->val.b_val;
			return expr1;
		}
		else
		if(*(expr2->sig_list.begin())==Type::type_char){
		//	expr2->sig_list=expr1->sig_list;
		//	expr2->val.i_val=(int)expr1->val.c_val;
			return expr1;		
		}
		else{
			ErrorReport::printError(expr1->line_num,expr1->col_num,"type mismatch");
			return NULL; //no others permited
		}
	}
	else
	if(*(expr1->sig_list.begin())==Type::type_double){
		if(*(expr2->sig_list.begin())==Type::type_int){
		//	expr2->sig_list=expr1->sig_list;
		//	expr2->val.d_val=(double)expr2->val.i_val;
			return expr1;			
		}
		else
		if(*(expr2->sig_list.begin())==Type::type_bool){
		//	expr2->sig_list=expr1->sig_list;
		//	expr2->val.d_val=(double)expr2->val.b_val;
			return expr1;	
		}
		else
		if(*(expr2->sig_list.begin())==Type::type_char){
		//	expr2->sig_list=expr1->sig_list;
		//	expr2->val.d_val=(double)expr2->val.c_val;
			return expr1;	
		}
		else{
			ErrorReport::printError(expr1->line_num,expr1->col_num,"type mismatch");
			return NULL; //no others permited
		}
	}
	return expr1;
}

/*
	set the expr to type if cast is permitted
	allow casting into int or double from basic types

*/
bool ProgTree::castExpr2Type(TreeRecord* type_node,TreeRecord* expr){
	Record r(""); //dull
	createTypeSignature(type_node,r);
	if((*(r.sig_list.begin())==Type::type_int)||(*(r.sig_list.begin())==Type::type_double)){
		if((*(expr->sig_list.begin())==Type::type_bool)||(*(expr->sig_list.begin())==Type::type_char)){
			expr->sig_list=r.sig_list;
			return true;
		}
		if((*(expr->sig_list.begin())==Type::type_int)||(*(expr->sig_list.begin())==Type::type_double)){
			expr->sig_list=r.sig_list;
			return true;			
		}

		ErrorReport::printError(type_node->line_num,type_node->col_num,"cast not permitted on such types");
		return false; //no other casting is permitted
	}
}

/*
node must be a var_exr node, and from it we must be able to get a reference to a symbol table record
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
pointer_val_expr:
	'*' var_expr	{$$=new TreeRecord(@1,NodeName::pointer_val_expr,$2);}	
	;

*/
Record* ProgTree::getRecordFromVarExpr(TreeRecord* node,Record* par){
	//simpel variable		ID_NAME
	if(name_comp(node->node_name,NodeName::id_name_expr)){
		Record* temp=symtab.findVarRec(node->node1->node_name,par);
		if(temp==NULL){
			ErrorReport::printError(node->line_num,node->col_num,"variable not found");
			return NULL;
		}	
		node->rec=temp; //reference back
		temp->isUsed=true;
		return temp;
	}

	//THIS
	if(name_comp(node->node_name,NodeName::this_expr)){
		Record* temp=cur_par->parent;
		if(temp==NULL){
			ErrorReport::printError(node->line_num,node->col_num,"this can't be used out side class scope");
			return NULL;
		}
		if(temp->type!=Type::class_type){
			ErrorReport::printError(node->line_num,node->col_num,"this can't be used out side class scope");
			return NULL;
		}
		node->rec=temp; //reference back
		return temp;
	}

	/*ID_NAME '.' var_expr			a.a.b.c
		type of ID_NAME must be a class type, so we check that and search in the class as 
		the next parent
	*/
	if(name_comp(node->node_name,NodeName::class_var_expr)){
		Record* var_rec=symtab.findVarRec(node->node1->node_name,par);
		
		if(var_rec==NULL){
			ErrorReport::printError(node->line_num,node->col_num,"left side not found as variable");
			return NULL;
		}
		//var_rec must not be a point to a class
		if((*(--var_rec->sig_list.end()))==Type::pointer_type){
			ErrorReport::printError(node->line_num,node->col_num,"left side is a class poiner not object");
			return NULL;
		}

		node->node1->rec=var_rec; //reference back
		node->rec=var_rec;
		Record* class_rec=reinterpret_cast<Record*>(*var_rec->sig_list.begin()); //the parent
		Record* temp=getRecordFromVarExpr(node->node2,class_rec);
		var_rec->isUsed=true;
		class_rec->isUsed=true;
		temp->isUsed=true;
		return temp;
	}
	//THIS ARROW var_expr
	if(name_comp(node->node_name,NodeName::this_arrow_expr)){
		Record* temp=getRecordFromVarExpr(node->node1,par);
		if(temp==NULL)
			ErrorReport::printError(node->line_num,node->col_num,"can't match left side to right side");
		temp->isUsed=true;
		return temp;
	}

	//class_type SCOPE_DOTS ID_NAME  
	if(name_comp(node->node_name,NodeName::scope_expr)){
		//find the record for ID_NAME
		Record r(node->node1->node_name,symtab.first_scope->depth+1,symtab.first_scope,Type::class_type);
		Record* var_par=symtab.findRecord(r);
		if(var_par==NULL){
		//	cout<<"non static variable can't be accessed via class\n";
			ErrorReport::printError(node->line_num,node->col_num,"non static variable can't be accessed via class");
			return NULL;
		}
		Record* var_rec=symtab.findVarRec(node->node2->node_name,var_par);
		if(var_rec==NULL){
		//	cout<<"variable no member in class\n";
			ErrorReport::printError(node->line_num,node->col_num,"variable no member in class");
			return NULL;	
		}
		node->node2->rec=var_rec; //reference back
		var_rec->isUsed=true;
		return var_rec;
	}

	//ID_NAME ARROW var_expr
	if(name_comp(node->node_name,NodeName::id_arrow_expr)){
		Record* var_rec=symtab.findVarRec(node->node1->node_name,par);
		if(var_rec==NULL){
			ErrorReport::printError(node->line_num,node->col_num,"left side variable not found");
			return NULL;
		}

		//var_rec must point to a class
		if((*(--var_rec->sig_list.end()))!=Type::pointer_type){
			ErrorReport::printError(node->line_num,node->col_num,"left side of -> must be a pointer to a class");
			return NULL;
		}
		node->node1->rec=var_rec; //reference back
		node->rec=var_rec;

		Record* class_rec=reinterpret_cast<Record*>(*var_rec->sig_list.begin()); //the parent
		Record* temp=getRecordFromVarExpr(node->node2,class_rec);
		var_rec->isUsed=true;
		if(temp!=NULL)
			temp->isUsed=true;
		class_rec->isUsed=true;
		return temp;
	}

	//pointer_val_expr
	if(name_comp(node->node_name,NodeName::pointer_val_expr)){
		Record* rec=getRecordFromVarExpr(node->node1,par);
		if(rec==NULL)
			return NULL;
		if((*(--rec->sig_list.end()))==Type::pointer_type){
			rec->isUsed=true;
			return rec;
		}
		else{
			ErrorReport::printError(node->line_num,node->col_num,"can't dereference non pointers");
			return NULL;
		}
	}
/*
array_expr:
	ID_NAME array_expr_dims	{$$=new TreeRecord(@1,NodeName::array_expr,new TreeRecord(@1,$1),$2);}
	;
*/
	if(name_comp(node->node_name,NodeName::array_expr)){
		Record* var_rec=symtab.findVarRec(node->node1->node_name,par);
		node->node1->rec=var_rec; //reference back
		var_rec->isUsed=true;
		return var_rec;
	}

	/*array_expr '.' var_expr
		array_expr must be of class type
	*/
	if(name_comp(node->node_name,NodeName::array_class_expr)){
		Record* class_rec=reinterpret_cast<Record*> (*node->node1->sig_list.begin());
		if(class_rec==NULL)
			return NULL;
		if(class_rec->type!=Type::class_type){
			ErrorReport::printError(node->line_num,node->col_num,"left side must be pointer to class");
			return NULL;
		}
		node->node1->rec=class_rec;
		class_rec->isUsed=true;
		return this->getRecordFromVarExpr(node->node2,class_rec);

	}

	/*array_expr ARROW var_expr
		array_expr must be a pointer to a class type
	*/
	if(name_comp(node->node_name,NodeName::array_pointer_expr)){
		Record* class_rec=reinterpret_cast<Record*> (*node->node1->sig_list.begin());
		if(class_rec==NULL)
			return NULL;
		if(class_rec->type!=Type::class_type){
			ErrorReport::printError(node->line_num,node->col_num,"left side must be pointer to class");
			return NULL;
		}
		node->node1->rec=class_rec;
		class_rec->isUsed=true;
		if((*(--node->node1->sig_list.end()))==Type::pointer_type)
			return this->getRecordFromVarExpr(node->node2,class_rec);
		else{
			ErrorReport::printError(node->line_num,node->col_num,"left side of -> must be pointer to class");
			return NULL;
		}
	}
	return NULL;
}

/*
array_init_list:
	'{' array_init_list_consts '}'		{$$=new TreeRecord(@1,NodeName::array_init_list,$2);}
	|'{' array_init_list_lists '}'		{$$=new TreeRecord(@1,NodeName::array_init_list,$2);}
	;	

array_init_list_consts:
	const_expr									{$$=new TreeRecord(@1,NodeName::array_init_list_consts,$1);}
	|array_init_list_consts ',' const_expr		{$$=new TreeRecord(@1,NodeName::array_init_list_consts,$1,$3);}
	;	

array_init_list_lists:
	array_init_list			{$$=new TreeRecord(@1,NodeName::array_init_list_lists,$1);}
	|array_init_list_lists ',' array_init_list	{$$=new TreeRecord(@1,NodeName::array_init_list_lists,$1,$3);}
	;

	int x[3][2]={{0,1},{1,2},{0,0}};
	int x[3][3]={1,2,3,4,5,6};
	int x[]={1,2,3};


		Array initialization rules:
				int a[][][2]={1,1,1}; //error, one empty is allowed
	 			int a[][2]={1,1,1}; //ok, set dim size to 3
				int a[1][][2]={1,1,1}; //error  empty should be to the left
				int a[][]={{1,1,1},{1,1,1}}; //truly just one empty
				int a[3][3]={{1,1,1},{1,1,1}}; //3 by 3
				int a[3][3]={1,1,1,1,1,1}; //all fill
				int a[3][3]={1,1,1,1}}; //left to right fill
				int a[2][3][2]={{{1},{1},{1}},{{1},{1,2},{1,1}}}; //three list levels ok

		one empty to the left, left to right fill, list contains count less or equal to dim_size 

	list values must match the basic type(the last dim type) and no type conversion allowed

	basiclly this is how it works to match a list against dims:
		recursively: from current dimension if cureent list is consts then it's count
			must be less or equal to the remaining elements in the array
			if list is list of lists then the count of list must be less or equal
			to the current dimension size, then we check each partial list against the 
			next dimension.

			cur_idx is used later to set value

*/

bool ProgTree::checkArrayInit(Record* array_rec,int cur_dim,int cur_idx,TreeRecord* initList,TreeRecord* def_node){

	int dim_size=0;
	int list_count=0;
	ArrayInfo* cur_info;
	list<ArrayInfo>::iterator itr;
	TreeRecord* cur_list=initList;
	//the basic type of the array elems exists in the last dimension
	list<int> basic_type=(*(--array_rec->array_info.end())).sig_list;

	for(itr=array_rec->array_info.begin(); itr!=array_rec->array_info.end();itr++){
		if(itr->dim==cur_dim)
			break;
	}
	cur_info=&(*itr);

	/*consts, then count of such list is <= multiplication of cur and remaining dim_sizes
		check type
		get consts count
		get remainng array elements count

	array_init_list_consts:
		expr									{$$=new TreeRecord(@1,NodeName::array_init_list_consts,$1);}
		|array_init_list_consts ',' expr		{$$=new TreeRecord(@1,NodeName::array_init_list_consts,$1,$3);}
		;	
	*/
	if(name_comp(initList->node_name,NodeName::array_init_list)) //remove { }
		cur_list=initList->node1;

	if(name_comp(cur_list->node_name,NodeName::array_init_list_consts)){
		int elem_count=1; //remaining elems of array

		bool isFinal=false;
		while(true){
			++list_count;
			TreeRecord* expr=NULL;
			if(cur_list->node2==NULL){ //final element
				expr=cur_list->node1;
				isFinal=true;
			}
			else
				expr=cur_list->node2;
			//check type
			this->calcExprType(expr);
			if(expr->sig_list!=basic_type){ //type mismatch
				ErrorReport::printError(expr->line_num,expr->col_num,"type mismatch");
				return false;
			}
			if(isFinal)
				break;
			cur_list=cur_list->node1;
		}
		if(cur_info->dim_size==0){
			if(cur_info->dim==0){ //set dimension 
				cur_info->dim_size=list_count;
			}
			else{ //check empty place
				ErrorReport::printError(cur_list->line_num,cur_list->col_num,"empty dimension not allowed here");
				return false;
			}
		}

		//get remaing count
		
		for(;itr!=array_rec->array_info.end();itr++){
			elem_count*=itr->dim_size;
		}
		if(list_count>elem_count){ //to many initializers
			ErrorReport::printError(cur_list->line_num,cur_list->col_num,"too many initializers");
			return false;			
		}
	}




	/*
	array_init_list_lists:
		array_init_list			{$$=new TreeRecord(@1,NodeName::array_init_list_lists,$1);}
		|array_init_list_lists ',' array_init_list	{$$=new TreeRecord(@1,NodeName::array_init_list_lists,$1,$3);}
		;
	*/	
	//list of lists
	if(name_comp(cur_list->node_name,NodeName::array_init_list_lists)){
		TreeRecord* temp=cur_list;
		while(true){
			++list_count;
			if(temp->node2==NULL) //final element
				break;
			temp=temp->node1;
		}
		//if this is the first dimension and not specified
		if(cur_info->dim_size==0){
			if(cur_info->dim==0){ //set dimension 
				cur_info->dim_size=list_count;
			}
			else{ //check empty place
				ErrorReport::printError(cur_list->line_num,cur_list->col_num,"empty dimension not allowed here");
				return false;
			}
		}
		//match list count dim size	
		if(list_count<=cur_info->dim_size){
			//now match each list against the following dimension
			TreeRecord* temp=cur_list;
			int idx=0;
			while(true){
				if(temp->node2==NULL){ //final element
					if(!checkArrayInit(array_rec,cur_dim+1,idx,temp->node1,def_node))
						return false;
					break;
				}
				if(!checkArrayInit(array_rec,cur_dim+1,idx,temp->node2,def_node))
					return false;
				temp=temp->node1;
				++idx;
			}
		}
		else{
			ErrorReport::printError(cur_list->line_num,cur_list->col_num,"too many initializers");
			return false;
		}
	//	}
	
	}

	return true;
}





///////////////////////////////////////////////////////////////////
/////////////////// optimization related code ////////////////////
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

out of those we only need the proc_call and unary_expr
the action we make is: if we need to generate code for it then just return 
else
set generateCode to false;
*/


void ProgTree::optimizeDanglingExpr(TreeRecord* expr){
	if(name_comp(expr->node_name,NodeName::bracket_expr	)){
		optimizeDanglingExpr(expr->node1);
		expr->generateCode=expr->node1->generateCode;
		if(expr->generateCode==false)
			ErrorReport::printHint(expr->line_num,expr->col_num,"dangling expressions have no used, skiped by optimization");
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
	if(name_comp(expr->node_name,NodeName::unary_expr)){
		switch(expr->node_type){
			case Type::left_decr : case Type::right_decr : case Type::left_icr: case Type::right_icr:
				return;
		}
		expr->generateCode=false;
		ErrorReport::printHint(expr->line_num,expr->col_num,"dangling expressions have no used, skiped by optimization");
		return;
	}

	if(name_comp(expr->node_name,NodeName::proc_call)){
		return; 
	}
	expr->generateCode=false;
	ErrorReport::printHint(expr->line_num,expr->col_num,"dangling expressions have no used, skiped by optimization");
}

//passing unary operation over a const data
void ProgTree::calcUnaryConstData(TreeRecord* node){
	int type=*node->sig_list.begin();
		switch(node->node_type){
			case Type::NOT:
				if(type==Type::type_int)
					node->val.i_val=!node->val.i_val;
				if(type==Type::type_double)
					node->val.d_val=!node->val.d_val;
				if(type==Type::type_bool)
					node->val.b_val=!node->val.b_val;
				if(type==Type::type_char)
					node->val.c_val=!node->val.c_val;
				if(type==Type::type_str)
					node->val.s_val=node->val.s_val;

				break;
			case Type::MINUS:
				if(type==Type::type_int)
					node->val.i_val=-node->val.i_val;
				if(type==Type::type_double)
					node->val.d_val=-node->val.d_val;
				if(type==Type::type_bool)
					node->val.b_val=-node->val.b_val;
				if(type==Type::type_char)
					node->val.c_val=-node->val.c_val;
				if(type==Type::type_str)
					node->val.s_val=node->val.s_val;
				break;
			case Type::left_decr:
				if(type==Type::type_int)
					node->val.i_val=--node->val.i_val;
				if(type==Type::type_double)
					node->val.d_val=--node->val.d_val;
				if(type==Type::type_bool)
					node->val.b_val=--node->val.b_val;
				if(type==Type::type_char)
					node->val.c_val=--node->val.c_val;
				if(type==Type::type_str)
					node->val.s_val=--node->val.s_val;
				break;
			case Type::left_icr:

				if(type==Type::type_int)
					node->val.i_val=++node->val.i_val;
				if(type==Type::type_double)
					node->val.d_val=++node->val.d_val;
				if(type==Type::type_bool)
					node->val.b_val=++node->val.b_val;
				if(type==Type::type_char)
					node->val.c_val=++node->val.c_val;
				if(type==Type::type_str)
					node->val.s_val=++node->val.s_val;
				break;
			case Type::right_decr:

				if(type==Type::type_int)
					node->val.i_val=--node->val.i_val;
				if(type==Type::type_double)
					node->val.d_val=--node->val.d_val;
				if(type==Type::type_bool)
					node->val.b_val=--node->val.b_val;
				if(type==Type::type_char)
					node->val.c_val=--node->val.c_val;
				if(type==Type::type_str)
					node->val.s_val=--node->val.s_val;
				break;
			case Type::right_icr:

				if(type==Type::type_int)
					node->val.i_val=++node->val.i_val;
				if(type==Type::type_double)
					node->val.d_val=++node->val.d_val;
				if(type==Type::type_bool)
					node->val.b_val=++node->val.b_val;
				if(type==Type::type_char)
					node->val.c_val=++node->val.c_val;
				if(type==Type::type_str)
					node->val.s_val=++node->val.s_val;
				break;		
		}
}

/*
math_operation_expr:
	expr '+' expr		{$$=new TreeRecord(NodeName::math_operation_expr,$1,$3,NULL,NULL,NULL,Type::PLUS);}				
	|expr '-' expr		{$$=new TreeRecord(NodeName::math_operation_expr,$1,$3,NULL,NULL,NULL,Type::MINUS);}				
	|expr '/' expr		{$$=new TreeRecord(NodeName::math_operation_expr,$1,$3,NULL,NULL,NULL,Type::DIV);}				
	|expr '*' expr		{$$=new TreeRecord(NodeName::math_operation_expr,$1,$3,NULL,NULL,NULL,Type::MULT);}				
	;
*/

//binary operation over a const data
void ProgTree::calcMathConstData(TreeRecord* node){
	int type=*node->sig_list.begin();
		switch(node->node_type){
			case Type::PLUS:
				if(type==Type::type_int)
					node->val.i_val=getMaxVal(node->node1)+getMaxVal(node->node2);
				if(type==Type::type_double)
					node->val.d_val=getMaxVal(node->node1)+getMaxVal(node->node2);
				if(type==Type::type_bool)
					node->val.b_val=getMaxVal(node->node1)+getMaxVal(node->node2);
				if(type==Type::type_char)
					node->val.c_val=getMaxVal(node->node1)+getMaxVal(node->node2);
				if(type==Type::type_str)
					node->val.s_val=node->node1->val.s_val;

				break;
			case Type::MINUS:
				if(type==Type::type_int)
					node->val.i_val=getMaxVal(node->node1)-getMaxVal(node->node2);
				if(type==Type::type_double)
					node->val.d_val=getMaxVal(node->node1)-getMaxVal(node->node2);
				if(type==Type::type_bool)
					node->val.b_val=getMaxVal(node->node1)-getMaxVal(node->node2);
				if(type==Type::type_char)
					node->val.c_val=getMaxVal(node->node1)-getMaxVal(node->node2);
				if(type==Type::type_str)
					node->val.s_val=node->node1->val.s_val;
				break;
			case Type::DIV:
				if(type==Type::type_int)
					node->val.i_val=getMaxVal(node->node1)/getMaxVal(node->node2);;
				if(type==Type::type_double)
					node->val.d_val=getMaxVal(node->node1)/getMaxVal(node->node2);
				if(type==Type::type_bool)
					node->val.b_val=getMaxVal(node->node1)/getMaxVal(node->node2);
				if(type==Type::type_char)
					node->val.c_val=getMaxVal(node->node1)/getMaxVal(node->node2);
				if(type==Type::type_str)
					node->val.s_val=node->node1->val.s_val;
				break;
			case Type::MULT:
				if(type==Type::type_int)
					node->val.i_val=getMaxVal(node->node1)*getMaxVal(node->node2);
				if(type==Type::type_double)
					node->val.d_val=getMaxVal(node->node1)*getMaxVal(node->node2);
				if(type==Type::type_bool)
					node->val.b_val=getMaxVal(node->node1)*getMaxVal(node->node2);
				if(type==Type::type_char)
					node->val.c_val=getMaxVal(node->node1)*getMaxVal(node->node2);
				if(type==Type::type_str)
					node->val.s_val=node->val.s_val;
				break;
		}
}

/*
	:
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
//binary operation over a const data
void ProgTree::calcLogicConstData(TreeRecord* node){
		switch(node->node_type){
			case Type::LESS:
					node->val.b_val=getMaxVal(node->node1)<getMaxVal(node->node2);
				break;
			case Type::GREATER:
					node->val.b_val=getMaxVal(node->node1)>getMaxVal(node->node2);
				break;
			case Type::LEQ:
					node->val.b_val=getMaxVal(node->node1)<=getMaxVal(node->node2);
				break;
			case Type::GEQ:
					node->val.b_val=getMaxVal(node->node1)>=getMaxVal(node->node2);
				break;
			case Type::EQL:
					node->val.b_val=getMaxVal(node->node1)==getMaxVal(node->node2);
				break;
			case Type::NEQ:
					node->val.b_val=getMaxVal(node->node1)!=getMaxVal(node->node2);
				break;
			case Type::oror:
					node->val.b_val=getMaxVal(node->node1)||getMaxVal(node->node2);
				break;
			case Type::andand:
					node->val.b_val=getMaxVal(node->node1)&&getMaxVal(node->node2);
				break;
		}
}

/*
get the numeric value of this const expression node
*/
double ProgTree::getMaxVal(TreeRecord* node){
	int type=*node->sig_list.begin();
	switch(type){
		case Type::type_int:
			return node->val.i_val;
		case Type::type_double:
			return node->val.d_val;
		case Type::type_char:
			return node->val.c_val;
		case Type::type_bool:
			return node->val.b_val;
	}
}

/* set const value to the passed value */
void ProgTree::setNodeConstVal(TreeRecord* node,double val){
	int type=*node->sig_list.begin();
	switch(type){
		case Type::type_int:
			node->val.i_val=val;
			break;
		case Type::type_double:
			node->val.d_val=val;
			break;
		case Type::type_bool:
			node->val.b_val=val;
			break;
		case Type::type_char:
			node->val.c_val=val;
			break;
		case Type::type_str:
			node->val.s_val="";
			break;
	}
}
		/*
			expr op expr
		if one expr is const and the other is not then check for 0 operand
			expr * 0=0		is const
			0/expr=0		const
			0 * expr =0		const
		*/
bool ProgTree::checkMultZero(TreeRecord* node){
	if(node->node_type==Type::MULT || node->node_type==Type::DIV){
		double val=0;
		if(node->node1->isConstData){
			val=getMaxVal(node->node1);
			if(val==0){
				node->isConstData=true;
				setNodeConstVal(node,val);
				return true;
			}
		}
		else
			if(node->node2->isConstData){
				val=getMaxVal(node->node2);
				if(val==0){
					node->isConstData=true;
					setNodeConstVal(node,val);
					if(node->node_type==Type::DIV){
						ErrorReport::printError(node->node2->line_num,node->node2->col_num,"Direct Division By Zero is not permitted");
					}
					return true;
				}
			}
	}
	return false;
}