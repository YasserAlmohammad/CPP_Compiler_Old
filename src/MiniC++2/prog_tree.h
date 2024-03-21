#pragma once

#include "symbol_table.h"
#include <iostream.h>
#include <hash_set>
#ifndef YYLTYPE
typedef
  struct yyltype
    {
      int timestamp;
      int first_line;
      int first_column;
      int last_line;
      int last_column;
      char *text;
   }
  yyltype;

#define YYLTYPE yyltype
#endif

using namespace std;

class TreeRecord;
class Record;

#ifndef MY_VAL_TYPE
typedef struct val_type{
		int i_val;
		double d_val;
		int c_val;
		int b_val;
		char* s_val;

	} val_type;
#define MY_VAL_TYPE
#endif
///////////////////////////////////////////////////////////////////////////////////
//************************* Type Class deleration ************************//
//////////////////////////////////////////////////////////////////////////////////
const int data_types=100;

class Type{
public:
	static const int NOTYPE = 0;
	static const int andand = 1;
	static const int PLUS = 2;
	static const int MINUS = 3;
	static const int DIV = 4 ;
	static const int MULT = 5 ;
	static const int LESS = 6 ;
	static const int GREATER = 7 ;
	static const int LEQ = 8 ; 
	static const int GEQ = 9;
	static const int NEQ =10;
	static const int EQL = 11;
	
	static const int oror = 12 ;
	static const int NOT = 13;
	static const int left_icr = 14;
	static const int right_icr = 15 ;
	static const int left_decr = 16 ;
	static const int right_decr = 17 ;
	static const int and = 18 ;

	static const int break_type = 19 ;
	static const int return_type = 20 ;
	static const int continue_type = 21 ;

	static const int proc_call_normal = 22 ;
	static const int PROC_CLASS_CALL = 23 ;
	static const int PROC_THIS_CALL = 24 ;
	static const int PROC_CLASS_STATIC_CALL = 25 ;

	static const int ASSIGN_NORMAL = 26 ;
	static const int ASSIGN_PLUS = 27 ;
	static const int ASSIGN_MINUS = 28 ;
	static const int ASSIGN_DIV = 29 ;
	static const int ASSIGN_MULT = 30 ;

	static const int access_public = 31 ;
	static const int access_private = 32 ;
	static const int access_protected = 33 ;
	static const int access_friend = 34 ;
	static const int access_not_specified = 35 ;

	static const int storage_static = 36 ;
	static const int storage_const = 37 ;
	static const int storage_extern = 38 ;
	static const int storage_static_const = 39 ;	
//	static const int storage_const_static = 40 ;
	static const int storage_none = 41 ;
	/*
	static const int constructor_decl= 41;
	static const int constructor_def= 41;
	static const int destructor_decl= 41;
	static const int destructor_def= 41;
	static const int class_func_decl= 41;
	static const int class_func_def= 41;
	static const int class_var_decl= 41;
	static const int class_var_def= 41;
	static const int func_decl= 41;
	static const int func_def= 41;
	static const int class_decl= 42;
	*/

	static const int type_int = data_types+1 ;
	static const int type_double = data_types+2 ;
	static const int type_bool = data_types+3 ;
	static const int simple_type = data_types+4;
	static const int class_type= data_types+5;
	static const int reference_type= data_types+6;
	static const int pointer_type= data_types+7;
	static const int type_char = data_types+8 ;
	static const int type_void = data_types+9 ;
	static const int type_constructor = data_types+10 ;

	static const int type_var = data_types+11 ;
	static const int type_destructor=data_types+12; 
	static const int type_func = data_types+13 ;

	static const int return_expr_type=data_types+15;

	static const int type_str=data_types+16;
	static const int for_itr_elem_expr=data_types+17 ;
	static const int type_block=data_types+18 ;
	static const int type_false=data_types+19 ;
	static const int type_true=data_types+20 ;
	static const int PROC_ARROW_CALL=data_types+21 ;

	static const int type_null=data_types+22 ;
};


///////////////////////////////////////////////////////////////////////////////////
//************************* NodeName Class **************************************//
//////////////////////////////////////////////////////////////////////////////////

//store names once and reference them many
class NodeName{
public:
	static char* program;
	static char* prog_decls;
	static char* prog_decls_error;
	static char* var_decl;
	static char* func_decl;
	static char* func_def;
	static char* class_func_def;
	static char* class_decl;
	static char* class_def;
	static char* class_static_init_var;
	static char* class_constructor_def;
	static char* class_destructor_def;
	
	
	static char* cast_expr;
	static char* delete_stmt;
	static char* expr_list;
	static char* new_expr;
	static char* unary_expr;
	static char* binary_expr;
	static char* pointer_val_expr;
	static char* array_expr;
	static char* array_expr_dims;
	static char* var_expr;
	static char* this_arrow_expr;
	static char* id_arrow_expr;
	static char* scope_expr;
	static char* class_var_expr;
	static char* this_expr;
	static char* id_name_expr;
	static char* true_const_expr;
	static char* false_const_expr;
	static char* str_const_expr;
	static char* char_const_expr;
	static char* double_const_expr;
	static char* int_const_expr;
	static char* bracket_expr;
	static char* while_stmt;
	static char* for_itr;
	static char* for_stmt;
	static char* if_stmt;
	static char* if_else_stmt;
	static char* block_stmt;
	static char* cout_stmt;
	static char* cout_stmt_output_list;
	static char* output_elem;
	static char* cin_stmt;
	static char* cin_stmt_input_list;
	static char* input_elem;
	static char* proc_expr_list_not_empty;
	static char* proc_expr_list;
	static char* proc_call;
	static char* assignment;
	static char* class_destructor;
	static char* array_init_list_lists;
	static char* array_init_list_consts;
	static char* array_init_list;
	static char* var_list;
	static char* var_list_elem;
	static char* stmt_error;
	static char* stmts;
	static char* class_func_def_common;
	static char* class_func_def_normal;
	static char* class_func_def_inline;
	static char* func_def_common;
	static char* func_def_normal;
	static char* func_def_inline;
	static char* func_decl_arg_elem;
	static char* func_decl_arg_list;
	static char* func_decl_args;
	static char* class_destructor_decl;
	static char* c_c_init_list;
	static char* class_constructor_init_list;
	static char* class_constructor;
	static char* class_constructor_decl;
	static char* class_body_stmt_error;
	static char* class_body_stmt;
	static char* class_body_stmts;
	static char* class_body;
	static char* array_dims;	
	static char* jump_stmt;

	static char* symtab_rec;
	static char* simple_type;
	static char* class_head;
	static char* access_spec;
	static char* class_base_list;
	static char* storage_spec;
	static char* array_dim;
	static char* pointer_type;
	static char* reference_type;
	static char* class_type;
	static char* var_constructor_call;
	static char* for_itr_elem_expr;
	static char* math_operation_expr;
	static char* logical_operation_expr;
	static char* new_array_expr;
	static char* array_class_expr;
	static char* array_pointer_expr;
	static char* compound_block_stmt;
	static char* expr_stmt;
	static char* name_error;
	static char* null_const_expr;	
		
};

///////////////////////////////////////////////////////////////////////////////////
//************************* TreeRecord Class ***********************************//
//////////////////////////////////////////////////////////////////////////////////
class TreeRecord{
public:
	//normal tree record
	TreeRecord(YYLTYPE loc,char* node_name,TreeRecord* n1=NULL,TreeRecord* n2=NULL,TreeRecord* n3=NULL,TreeRecord* n4=NULL,TreeRecord* n5=NULL,int type=0, int align=0,TreeRecord* p=NULL);
	TreeRecord(YYLTYPE loc,char* node_name,char* value,int type);
	TreeRecord();
	TreeRecord(int); //type node

	char* node_name;
	TreeRecord* node1;
	TreeRecord* node2;
	TreeRecord* node3;
	TreeRecord* node4;
	TreeRecord* node5;
	TreeRecord* parent;
	// each node can have optional children as it needs
	int node_alignment; //used for textual output format only
	int node_type; //later for type checking

	void print();
	//node_name: when it's an ID_NAME it must be looked up in the symbol table and replaced
	static char* randName();
	static int count;
	val_type val; //casted to the proper type

	list<int> sig_list; //used for type checking, especially for complex types
//	list<int>::iterator sig_itr;
	int line_num;
	int col_num;

	//used for optimiation, and to check that current expr is const data and contains no variables
	bool isConstData;

	Record* rec; //reference to the symbol table
	int size; //used in code generation to allocate memory for classes
	bool generateCode;
	char* codeName; //used by code generation
};

///////////////////////////////////////////////////////////////////////////////////
//************************* ProgTree Class *************************************//
//////////////////////////////////////////////////////////////////////////////////

//will hold syntax tree puls do all necessary operations 
class ProgTree{
public:
	ProgTree();
	TreeRecord* head;
	
	bool isValid; //when true we can execute, any error in the program must set this to false
	//setting it happens only once
	void print();
	void checkTree(TreeRecord* node);
	void exec();
	void createFuncArgSignature(TreeRecord*,Record& par);
	void createFuncCallArgSignature(TreeRecord*,Record& par);
	void createTypeSignature(TreeRecord* type_node,Record& par,bool setReturnType=false);
	bool addArgs2Symtab(TreeRecord* node, Record* par);
	bool checkInitListVals(TreeRecord*,Record* par);//member initialiation list
	//create signature out of TreeRecord( of type func_decl_arrgs) and set it to a symbol table record r
	bool calcExprVal(TreeRecord*);
	bool calcExprType(TreeRecord*);
	bool assignExpr2Var(TreeRecord* node, Record* var);
	bool matchArgs2Decl(TreeRecord* callArgs,Record* func_rec); //pass call args and compare to arg decl in Symtab rec of a func
	bool setVarListElem(TreeRecord* list_node,TreeRecord* type_node,TreeRecord* storage_node);
	bool canAssignType(Record* r,TreeRecord* expr);
	bool canAssignType(TreeRecord* var_expr,TreeRecord* expr);
	TreeRecord* balanceExprType(TreeRecord* expr1,TreeRecord* expr2); //return most common
	bool castExpr2Type(TreeRecord* type_node,TreeRecord* expr);
	Record* getRecordFromVarExpr(TreeRecord* node,Record* par);
	void createTypeSignature(TreeRecord* type_node);
	bool checkArrayInit(Record* array_rec,int cur_dim,int cur_idx,TreeRecord* initList,TreeRecord* def_node);

	static bool name_comp(char*,char*);
	bool optimize; //enable/disable
	void optimizeDanglingExpr(TreeRecord* expr);
	void calcUnaryConstData(TreeRecord* node);
	void calcMathConstData(TreeRecord* node);
	void calcLogicConstData(TreeRecord* node);
	static double getMaxVal(TreeRecord* node);
	bool checkMultZero(TreeRecord* node);
	void setNodeConstVal(TreeRecord* node,double val);
};