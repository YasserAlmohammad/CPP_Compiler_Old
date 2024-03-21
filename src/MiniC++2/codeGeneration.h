#pragma once

#include "symbol_table.h"
#include <iostream.h>
/*
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
*/
class TreeRecord;
class Record;

class CodeGeneration{
public:
	static int shift; //cur variable shift from cur fp
	static int class_shift; //member variables shift from current class
	static bool firstRound;// first round is for variable initization at after fp
	//second round is for the rest
	static int labelCount; //used to make a unique labeling and readable ones
	void generateVMCode(char* filePath);

	//for each node we create a code
	//main is a special case for specific params, we pass it a record
	void generateProgDeclsCode(TreeRecord* node);
	void generateVarDeclsUpdateCode(TreeRecord* node);
	void generateDeclCode(TreeRecord* node); 
	void generateFunDefCode(TreeRecord* node);
	void generateFuncCode(TreeRecord* rec);
	void generateFunParamsCode(TreeRecord* node,int type); // 1 for class params, 2 for normal func
	void generateTypeCode(TreeRecord* node);
	void generateFuncArgElemCode(TreeRecord* node,int type);
	void generateVarListElemCode(TreeRecord* elem_node,TreeRecord* type_node);


	void generateBlockStmtCode(TreeRecord* node);
	void generateStmtsCode(TreeRecord* node);
	void generateStmtCode(TreeRecord* node);
	void generateVarDeclCode(TreeRecord* node);

	void outputMethodCall(char* methodName);
	void generateExprCode(TreeRecord* node);
	void generateExprLoadHelper(TreeRecord* node);
	void generateUnaryExprCode(TreeRecord* node);
	void generateLogicalExprCode(TreeRecord* node);
	void generateMathExprCode(TreeRecord* node);

	void initVars(TreeRecord* node);
	void initGlobalVars(TreeRecord* node);
	void initVarDeclStmtCode(TreeRecord* node,int scope);
	void initVarListElemCode(TreeRecord* elem_node,TreeRecord* type_node,int scope);

	void generateCoutStmtCode(TreeRecord* node);
	void generateWriteCode(TreeRecord* node);
	void generateCinStmtCode(TreeRecord* node);
	void generateDeleteStmtCode(TreeRecord* node);
	void generateReadCode(TreeRecord* node);
	void generateLoadVarExprAddressCode(TreeRecord* node);
	void generateLoadVarExprHelper(TreeRecord* node);
	void generateStoreInVarExprCode(TreeRecord* node);
	void generateStoreInVarExprHelper(TreeRecord* node);
	void generateAssignStmtCode(TreeRecord* node);
	void generateMathOperationCode(TreeRecord* node1,TreeRecord* node2,int op);
	void generateCastCode(TreeRecord* node1,TreeRecord* node2);
	void generateCastBalanceCode(TreeRecord* node1,TreeRecord* node2);
	void generateJumbStmtCode(TreeRecord* node);
	void generateIfStmtCode(TreeRecord* node);
	void generateIfElseStmtCode(TreeRecord* node);
	void generateForStmtCode(TreeRecord* node);
	void generateForItrCode(TreeRecord* node);
	void generateWhileStmtCode(TreeRecord* node);
	void generateCBlockStmtCode(TreeRecord* node);
	void generateExprStmtCode(TreeRecord* node);
	void generateProcCallStmtCode(TreeRecord* node);
	void generateUnaryOpStmtCode(TreeRecord* node);
	
	char* generateRandLabel(char* mark,int label);

	void generateProcCallParamsCode(TreeRecord* node);
	void generateFunAssignParams(TreeRecord* node,int type); //1 for class func and 0 for normal func
	void generateFunAssignParamElem(TreeRecord* node,int type);
	void generateProcCallParamsHelperCode(TreeRecord* node);
	void generateClassConstructorFunDefCode(TreeRecord* node);
	void generateClassFunParamsCode(TreeRecord* node);
	void generateClassFunDefCode(TreeRecord* node);
	void generateIDNameCode(TreeRecord* node);

	void modifyString(char* str); // to enable \t \n \" in a string to be shown by vm
	void calcClassSize(TreeRecord* node);//size of class to allocate it in memory
	int getClassBodyStmtSize(TreeRecord* node); //class body statment size if it counts
	void generateParentCall(TreeRecord* node);
	void generateDefaultParentCall(TreeRecord* node);

	void generateParentDestructorCall(TreeRecord* node);
	void generateClassDestructorFunDefCode(TreeRecord* node);
	void generateStoreInIDCode(TreeRecord* node);
	bool generateMainCall();
	void generateConstCode(TreeRecord* node);
	void generateMathMono(TreeRecord* node);
	void generateInitList(TreeRecord* node);
	bool unrollFor(TreeRecord* node);
	static bool optimize;

};