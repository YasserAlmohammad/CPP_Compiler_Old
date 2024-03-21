#pragma once
class ErrorReport{
public:
	//once set, no execution should be done
	static bool containsError;
	static void printError(int line,int col,char* msg);
	static void printWarning(int line,int col,char* msg);
	static void printHint(int line,int col,char* msg);
};