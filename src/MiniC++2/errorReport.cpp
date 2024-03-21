#include "errorReporting.h"
#include <iostream.h>

bool ErrorReport::containsError=false;
void ErrorReport::printError(int line,int col,char* msg){
	cout<<"**ERROR**:   ["<<line<<","<<col<<"] "<<msg<<endl;
	containsError=true;
}
	
void ErrorReport::printWarning(int line,int col,char* msg){

	cout<<"**WARNING**: ["<<line<<","<<col<<"] "<<msg<<endl;
}
	
void ErrorReport::printHint(int line,int col,char* msg){

	cout<<"**HINT**:    ["<<line<<","<<col<<"] "<<msg<<endl;
}
