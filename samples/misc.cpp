int function(int x, int y){
	int v=01;
	if(x>0){
		int v=2;
		if(y>0){
			int v=3;
		}
	}
	return 0;
}

	class A{
	public:
		A(int val);
		~A();
		int setXVal(int val);
		int* x;
		A* a;
	};

	A::A(int val){
		x=new int(val);
		cout<<*x;
	}


int A::setXVal(int val){
	*x=val;

	int k=0;
	while(k<10){
		++k;
		if(k==5)
			continue;

		for(int i=0;i<10;i++){
			cout<<"internal loop="<<i;
			if(i==3)
				break;
		}		
		cout<<"			external loop="<<k;
	}

	if(val>0){
		cout<<"bigger";
		if(val>30)
			cout<<"bigger than 30";
	}
	else
		cout<<"smaller";
		
	return val;
}

int main()
{
	
	A* a=new A(1);
	a->a=new A(2);
	a->a->a=new A(3);
	*a->a->x=22;
	*a->a->a->x=33;

	cout<<*a->a->x;
	cout<<*a->a->a->x;

	int x=2;
	int y=3;

	if((x==2)&&((y==3)||(x>0))){
		cout<<"ok";
	}
	cout<<"input integers";
	cin>>x>>y>>*a->a->a->x;

	y=3.3*x;
	cout<<"x="<<x<<"\ny="<<y<<" *a->a->a->x="<<*a->a->a->x;
	
	a->setXVal(55);
	
	int localX;
	int* intPtr=new int(22);
	cout<<"input integer";
	cin>>localX;
	if(localX>0){
		cout<<localX<<*intPtr;
	}
	
	cout<<"press a number to end...";
	int ttt=0;
	cin>>ttt;
	return 0;
}