
#include "symbol_table.h"
#include "errorReporting.h"
#include "memory.h"
///////////////////////////////////////////////////////////////////////////////////
//************************* Record Class definition ************************//
//////////////////////////////////////////////////////////////////////////////////

int Record::count=0;
extern SymbolTable symtab;
/* this constructor is used to look for records of specific name and depth */

Record::Record(char* n,int d,Record* p,int t,int access,TreeRecord* n_ref,int* inf){
	//name can't be null
	strcpy(name,n);
	type=t;
 //   type=t;
	depth=d;
	parent=p;
	info=inf;
	ref_node=n_ref;
	access_type=access;
	rec_id=++count;
	ignoreSignature=false;
//	val=0;
	storage_type=Type::storage_none;
	isConstFunc=false;
	isFindClassFromVar=false;
	isReference=false;
	reference=NULL;
	shift=0; //shift from cur fp
	shift_from=0;
	isUsed=false;	
	codeName=NULL;
}


char* Record::randName(){
	char countStr[10]={'\0'};
	_itoa(count++,countStr,10);
	int len=7+strlen(countStr);
	char* ret=(char*)malloc(len);
	memset(ret,'\0',len);

	strcpy(ret,"block_");
	strncat(ret,countStr,len);
	return ret;
}

//name is created by appending the rec id to it
void Record::createFuncName(){ //normal function

	char countStr[10]={'\0'};
	_itoa(count++,countStr,10);
	int nameLen=strlen(name);

	codeName=(char*)malloc(100);
	memset(codeName,'\0',100);

	strcpy(codeName,"_function_");
	strncat(codeName,name,nameLen);
	strncat(codeName,"_",1);
	strncat(codeName,countStr,strlen(countStr));
}


void Record::createFuncName(char* base){ //member function
	char countStr[10]={'\0'};
	_itoa(count++,countStr,10);
	int nameLen=strlen(name);
	int funLen=strlen(base);

	codeName=(char*)malloc(100);
	memset(codeName,'\0',100);

	strcpy(codeName,"_CLASS_");
	strncat(codeName,base,funLen);
	strncat(codeName,"_",1);
	strncat(codeName,name,nameLen);
	strncat(codeName,"_",1);
	strncat(codeName,countStr,strlen(countStr));
}

void printlist(list<int> l){
	list<int>::iterator itr;
	for(itr=l.begin();itr!=l.end();itr++){
		cout<<*itr<<" ";
	}
}

void Record::printArrayInfo(){
	list<ArrayInfo>::iterator itr;
	for(itr=array_info.begin();itr!=array_info.end();itr++){
		cout<<itr->dim<<"  "<<itr->dim_size<<"  ";
		printlist(itr->sig_list);
		cout<<endl;
	}
}
int Record::b_id=0;
char* Record::createUniqueName(){
	char* str=(char*)malloc(20);
	itoa(b_id,str,10);
	int len=strlen(str);
//	str[len]='\0';
	strcat(str,"_block_");
	++b_id;
	return str;
}

///////////////////////////////////////////////////////////////////////////////////
//************************* HashKey Class definition ************************//
//////////////////////////////////////////////////////////////////////////////////


/* acceptable coast hash function*/
size_t HashKey:: operator( )( const Record& rec) const{
	int hashValue=0;
	int len=strlen(rec.name);
	int i=0;
	while(i<len){
		hashValue+=(int)rec.name[i];
		++i;
	}
	hashValue*=rec.depth;

	return hashValue;
}

/* equality of two records means they have the same name, the same depth value
	the same parent, the same type and optionally using the same type signature, such a signature is stored in the 
	sig_list
	
	it must be updated to enable the decleration of function and varaible of the same
	name
	also to enable over loading of functions
	//return false for equlaity ?????
*/
bool HashKey::operator( )(const Record& rec1,const Record& rec2) const{
/*
	//don't use name but use signature for class search
	if(rec1.isFindClassFromVar || rec2.isFindClassFromVar){
		if((rec1.depth==rec2.depth)&&(rec1.parent==rec2.parent)&&(rec1.type==rec2.type)&&rec1.sig_list==rec2.sig_list){
			return false;
		}
		else
			return true;
	}
*/

	if((rec1.depth==rec2.depth)&&(strcmp(rec1.name,rec2.name)==0)&&(rec1.parent==rec2.parent)&&(rec1.type==rec2.type)){
		if((rec1.ignoreSignature)||(rec2.ignoreSignature))
			return false;
		if(rec1.sig_list==rec2.sig_list)
			return false;
		else
			return true;
	}
	else
		return true;
}




///////////////////////////////////////////////////////////////////////////////////
//************************* SymbolTable Class definition ************************//
//////////////////////////////////////////////////////////////////////////////////

SymbolTable::SymbolTable(){
	
}

Record* SymbolTable::addRecord(Record& rec){
	pr=hashSet.insert(rec);
	if(pr.second==true)
		return &(*pr.first);
	else
        return NULL;
}
//deprecated
Record* SymbolTable::findRecord(char* name, int depth,Record* p,int type){
	//ignore signature
	Record r(name,depth,p,type);
	r.ignoreSignature=true;
	itr=hashSet.find(r);
	if(itr==hashSet.end()){
		return NULL;
	}
	else{
		r.ignoreSignature=false;//restore default
		return &(*itr);
	}
}

//deprecated
Record* SymbolTable::findRecord(char* name,int type){
	Record r(name,1,symtab.first_scope);
	r.ignoreSignature=true;
	itr=hashSet.find(r);
	if(itr==hashSet.end()){
		return NULL;
	}
	else{
		r.ignoreSignature=false; //restore default
		return &(*itr);
	}
}

Record* SymbolTable::findRecord(Record& r,bool ignoreSig/* =true */){
	r.ignoreSignature=ignoreSig;
	itr=hashSet.find(r);
	if(itr==hashSet.end()){
		r.ignoreSignature=false; //by default restore it
		return NULL;
	}
	else{
		r.ignoreSignature=false;
		return &(*itr);
	}
}


void SymbolTable::print(){
	int* addr=0;
	cout<<"rec_address  "<<"parent_address  "<<"depth   "<<"type   "<<"IsUsed   "<<"name  "<<endl; 
	for(itr=hashSet.begin();itr!=hashSet.end();itr++){
		addr=reinterpret_cast<int*>(&(*itr));
		cout<<addr<<"    "<<itr->parent<<"      "<<itr->depth<<"       "<<itr->type<<"     "<<itr->isUsed<<"    "<<itr->name<<endl<<endl;
	}
}

int SymbolTable::getClassCode(char* name){
	Record* r=findRecord(name,Type::class_type);
	if(r==NULL)
		return 0;
	else
		return r->rec_id;
}

/*
we need to find it in a recursive way tell first scope, since variables could be defined any where
if this variable was called inside a class function definition then we search the fathers of
this class if it gots any
*/
Record* SymbolTable::findVarRec(char* id,Record* cur_par){
	Record* temp=symtab.findRecord(id,cur_par->depth+1,cur_par,Type::type_var);
	if(temp!=NULL)
		return temp;
	if(cur_par==first_scope) //search until the global scope
		return NULL;
	else{
		//check if this is a class record==> variable is found in class-func-def
		if(cur_par->type==Type::class_type){ //search inside every parent
			TreeRecord* ref=cur_par->ref_node;
			if(ref==NULL) //should never happens
				return NULL;
/*
class_def:
	 class_head  class_body		{$$=new TreeRecord(NodeName::class_def,$1,$2);} 
	|class_head ':' access_spec       class_base_list    class_body	{$$=new TreeRecord(NodeName::class_def,$1,$3,$4,$5);} 
	;
class_base_list:
	class_type						
	|class_base_list ',' class_type	{$$=new TreeRecord(NodeName::class_base_list,$1,$3);}

*/
			TreeRecord* base_list=ref->node3;
			if(base_list==NULL) //no derivation
				return findVarRec(id,cur_par->parent);
			while(true){
				if(base_list->node2==NULL){ //final element
					//get base record, all classes in first scope
					Record* base=this->findRecord(Record(base_list->node_name,first_scope->depth+1,first_scope,Type::class_type));
					Record* var_rec=this->findVarRec(id,base); //find it recurively
					if(var_rec!=NULL){//check access
						if(var_rec->access_type==Type::access_private){ //deny
							ErrorReport::printError(base_list->line_num,base_list->col_num,"can't access private member of base list");
							return NULL;
						}
						return var_rec;
					}
					break;
				}
				Record* base=this->findRecord(Record(base_list->node2->node_name,first_scope->depth+1,first_scope,Type::class_type));
				Record* var_rec=this->findVarRec(id,base); //find it recurively
				if(var_rec!=NULL){//check access
					if(var_rec->access_type==Type::access_private){ //deny
						ErrorReport::printError(base_list->line_num,base_list->col_num,"can't access private member of base list");
						return NULL;
					}
					return var_rec;
				}
				base_list=base_list->node1;
			}


		}
		else //no access check required, 
			return findVarRec(id,cur_par->parent);
	}
}

/*
same as variable, if current parent is a class then search in his preants too
*/
Record* SymbolTable::findFuncRec(Record* rec,Record* cur_par,bool useSig){
	rec->parent=cur_par;
	rec->depth=cur_par->depth+1;
	Record* temp=symtab.findRecord(*rec,useSig);
	if(temp!=NULL)
		return temp;
	if(cur_par==first_scope) //search until the global scope
		return NULL;
	else{
		//check if this is a class record==> variable is found in class-func-def
		if(cur_par->type==Type::class_type){ //search inside every parent
			TreeRecord* ref=cur_par->ref_node;
			if(ref==NULL) //should never happens
				return NULL;

			TreeRecord* base_list=ref->node3;
			if(base_list==NULL) //no derivation
				return findFuncRec(rec,cur_par->parent,useSig);
			while(true){
				if(base_list->node2==NULL){ //final element
					//get base record, all classes in first scope
					Record* base=this->findRecord(Record(base_list->node_name,first_scope->depth+1,first_scope,Type::class_type));
					Record* var_rec=this->findFuncRec(rec,base,useSig); //find it recurively
					if(var_rec!=NULL){//check access
						if(var_rec->access_type==Type::access_private){ //deny
							ErrorReport::printError(base_list->line_num,base_list->col_num,"can't access private member of base list");
							return NULL;
						}
						return var_rec;
					}
					break;
				}
				Record* base=this->findRecord(Record(base_list->node2->node_name,first_scope->depth+1,first_scope,Type::class_type));
				Record* var_rec=this->findFuncRec(rec,base,useSig); //find it recurively
				if(var_rec!=NULL){//check access
					if(var_rec->access_type==Type::access_private){ //deny
						ErrorReport::printError(base_list->line_num,base_list->col_num,"can't access private member of base list");
						return NULL;
					}
					return var_rec;
				}
				base_list=base_list->node1;
			}


		}
		else //no access check required, 
			return findFuncRec(rec,cur_par->parent,useSig);
	}
}

Record* SymbolTable::findClassFromVar(Record* var){
	Record r("",first_scope->depth+1,first_scope,Type::class_type);
	r.sig_list=var->sig_list;
	r.isFindClassFromVar=true;
	Record* temp=findRecord(r);
	return temp;
}

