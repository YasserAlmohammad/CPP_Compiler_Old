#pragma once


#include "prog_tree.h"
#include <hash_set>
#include <iostream.h>
#include <string.h>


using namespace std;

///////////////////////////////////////////////////////////////////////////////////
//************************* Record Class deleration ************************//
//////////////////////////////////////////////////////////////////////////////////
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
class TreeRecord;

const int access_not_specified=0;

//single element in array
class ArrayInfo{
public:
	ArrayInfo(int d,int ds,list<int> type,int p){dim=d; dim_size=ds,sig_list=type,data=p;}
	ArrayInfo(int d,int ds,int p){dim=d; dim_size=ds,data=p;}
	ArrayInfo(){dim=0; dim_size=0,data=0;}
	int dim;
	int dim_size;
	list<int> sig_list; //type
	int data; //32bit pointer value
};

class Record{
public:
	static int count; //count, used in random name generation, and in int codes for classes
	Record(char* n,int d=0,Record* p=NULL,int t=0,int access=access_not_specified,TreeRecord* n_ref=NULL,int* info=NULL);
	
	char name[100]; //max name
	int depth;  //scope depth
	int type; //i.e var, func
	int access_type; //public protected...  Type::access_public ...
	int storage_type;
	bool isConstFunc; // void f() const{}
	Record* parent; //depth+parent forms unique scope info
	int rec_id; //see createSignature for func args
	
	bool ignoreSignature; //if this is true then we don't use the signature in the search clause
	//info, can be dynamically casted to a specific class or data record
	//depending on type
	int* info;
//	int val; //changed later to hold a value for a variable...
	
	//simple var case: type+value
	int id_type;
	void* id_val;
	TreeRecord* ref_node;
	bool isReference; 
	Record* reference; //used for the & thing	
	//some types requires this to execute, for example: functions and classes

	static char* randName();
	void createFuncName(); //normal function
	void createFuncName(char* base); //member function
	
	bool isFindClassFromVar;
	/*
		signature list used by functions to match their delcerations args, it can also used by other
		types especially complex types for efficient type checking
	*/
	list<int> sig_list; 
	list<int> ret_sig_list; //used by functions for return type signature 
	list<int>::iterator sig_itr;

	list<ArrayInfo> array_info; //dim_size, dim_type, data pointer (32bit sys)
	void printArrayInfo();
	static char* createUniqueName();
	static int b_id; //block_id
	int shift;
	int shift_from;//0 gp 1 from fp
	bool isUsed; //used by code generation
	char* codeName; //used by code generation

	val_type val; //for const variables

};

///////////////////////////////////////////////////////////////////////////////////
//************************* Record Class deleration ************************//
//////////////////////////////////////////////////////////////////////////////////


class HashKey{
public:
	static const size_t bucket_size = 4;
	static const size_t min_buckets = 8;

	HashKey(){}
	size_t operator( )( const Record& d ) const;
	bool operator( )(const Record& d1,const Record& d2) const;
   	
};

///////////////////////////////////////////////////////////////////////////////////
//************************* Record Class deleration ************************//
//////////////////////////////////////////////////////////////////////////////////


class SymbolTable{
public:
	SymbolTable();
	Record* findRecord(char* name, int depth,Record* p,int type); //0 when not found
	Record* findRecord(char* name,int type);
	Record* findRecord(Record& r,bool ignoreSignature=true);
	Record* findVarRec(char* id,Record* cur_par);
	Record* findFuncRec(Record* rec,Record* cur_par,bool useSig=true);
	Record* findClassFromVar(Record*);
	Record* addRecord(Record& rec);	//0 returned means not added
	void print();
	Record* first_scope;
	int getClassCode(char* name);

private:
	hash_set<Record,HashKey> hashSet;
	hash_set<Record,HashKey>::iterator itr;
	pair< hash_set<Record,HashKey>::iterator, bool > pr;
	
};