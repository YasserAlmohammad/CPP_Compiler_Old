#include <iostream.h>
class Increment {
public:
   Increment(int c = 0, int i = 1);
   void addIncrement() { count += increment; }
   void print() const;
private:
   int count;
   const int increment;
};
// Constructor for class Increment
Increment::Increment(int c, int i)
{  
   count = c;
}
/* Print the data*/
void Increment::print() const
{
   cout << "count = " << count
        << ", increment = " << increment << endl;
}
main()
{
   Increment value(10, 5);
   cout << "Before incrementing: ";
   value.print();
   int j=1;
   while (j<=3) {
      value.addIncrement();
      cout << "After increment " << j << ": ";
      value.print();
       j++
   }
   return 0;
}

/*

void g(int x){
	x=0;
	x=1;
	x=2;
}

void f(int x=0);
void f(int x,int y);
inline void h(){ 	
	int x=0; 
	if(x)
		if(x)
			x=0;
	else
		while(x){
			int j=1;
		}
}



class X{
public:
	static int x;
	int y;
	void f();
	inline void h(){ int x=0; }
	static void h();
private:
};


A::x=0;

void main(){
	A::x=0;
	int x[][][];
	int* x;
	A::f();
	f(x);
	f();
	x=new int(3);
	int x=new double(44);
	x=new int[4];
	x[2][1][2]=0;
}
*/