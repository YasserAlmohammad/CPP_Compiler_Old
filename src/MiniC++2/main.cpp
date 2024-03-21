
#include <stdio.h>
#include <iostream.h>
#include "symbol_table.h"
#include "prog_tree.h"
#include "codeGeneration.h"
	
extern  FILE* yyin;
extern  int yyparse();

SymbolTable symtab;
Record* cur_par=NULL;
ProgTree progTree;
CodeGeneration generator;

/*
	-optimize  sourcefile  
*/
int main(int argc,char* args[]){
	/* initially there is a file scope, with 0 depth */
	cur_par=symtab.addRecord(Record("__File_Scope_Rec__"));
	symtab.first_scope=cur_par;

	if(args[1]==NULL){
		cout<<"1USAGE: minic++  -optimize  filepath  ";
		return -1;
	}

	if(strcmp(args[1],"-optimize")==0){
		CodeGeneration::optimize=true;
		if(args[2]==NULL){
			cout<<"2USAGE: minic++  -optimize  filepath  ";
			return -1;
		}
		yyin=fopen(args[2],"r");
	}
	else{
		CodeGeneration::optimize=false;
		yyin=fopen(args[1],"r");
	}

	if(yyin==NULL){
		cout<<"source file couldn't be open";
		return -1;
	}
//	yyin=fopen("s1.cpp","r");
	yyparse();
	progTree.checkTree(progTree.head);
	
	cout<<"**************************symbol table content**************************\n";
	symtab.print();
	cout<<"************************************************************************\n";
	generator.generateVMCode("c:\\1.vm");

	int x=0;
	cin>>x;
	/*
	yyin=fopen("s1.cpp","r");

	yyparse();
	progTree.checkTree(progTree.head);
	
	generator.generateVMCode("vm1.vm");

	cout<<"**************************symbol table content**************************\n";
	symtab.print();
	
	cout<<"**************************prog tree content**************************\n";
//	progTree.print();
	int x=0;
	cin>>x;
	*/
	return 0;
}

/*
int main(int argc,char* args[]){

	if(args[1]==NULL){
		cout<<" input the file name as argument\n";
		return 1;
	}


	// initially there is a file scope, with 0 depth 
	cur_par=symtab.addRecord(Record("__File_Scope_Rec__"));
	symtab.first_scope=cur_par;
	yyin=fopen(args[1],"r");
	
	if(yyin==NULL)
		cout<<"file not found\n";
	else{
			yyparse();
			progTree.checkTree(progTree.head);
			cout<<"**************************symbol table content**************************\n";
			symtab.print();	
			generator.generateVMCode("vm1.vm");
		}
	
	
	return 0;
}
*/