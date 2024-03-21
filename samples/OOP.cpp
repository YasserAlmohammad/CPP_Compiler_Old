		class A{
		public:
			A();
			int a;
		};

		A::A(){
			cout<<"A constructor called...\n";
			a=0;
		}

		class B: public A{
		public:
			B();
			B(int);
			int b;
		};	

		B::B(){
			cout<<"B first constructer called...";
			b=0;
		}

		B::B(int x){
			b=x*2;
			cout<<"B second Constructor called...\n";
		}

		class C: public B{
		public:
			C(int x);
			int c;
		};

		C::C(int x):B(x){
			c=x;
			cout<<"C was constructed...";
		}

		int main(){
			C c(2);
			cout<<"  c.c="<<c.c<<"  c.b="<<c.b<<" c.a="<<c.a;


			cout<<"press a number to end...";
			int ttt=0;
			cin>>ttt;
			return 0;
		}
