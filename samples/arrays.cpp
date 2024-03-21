/* array sample program */

class A{
public:
	A(int);
	int val;
};

A::A(int val){
	this->val=val;
}

int main(){
	
	cout<<"*********testing matrix of object pointers*********";
	A* b[10];
	int idx=0;
	while(true){
		b[idx]=new A(idx);
		++idx;
		if(idx>=10)
			break;
	}

	idx=0;
	while(true){
		cout<<b[idx]->val;
		++idx;
		if(idx>=10)
			break;
	}
	cout<<"***************testing 2d matrix of integers***********";
	int a[15][15];
	a[10][10]=0;
	
	cout<<"2d matrix example\n";

	/* for implementational reasons i starts from -1 
		because iteration happens before loop
	*/
	for(int i=-1;i<9;i++){
		for(int j=-1;j<9;j++){
				a[i][j]=i*j;
		}
	}

	for(int ii=-1;ii<9;ii++){
		cout<<"line"<<ii<<"************";
		for(int jj=-1;jj<9;jj++){
				cout<<a[ii][jj];
		}
	}


	cout<<"press a number to end...";
	int ttt=0;
	cin>>ttt;

	return 0;
}