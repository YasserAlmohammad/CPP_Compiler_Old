int fact(int x){
	if(x<=1)
		return 1;
	return x*fact(x-1);
}

int main(){
	int choice=0;
	int x=0;
	cout<<"*********************************************";
	cout<<"select choice: 0 to exit, 1 to continue:";
	cin>>choice;
	
	while(choice){
		cout<<"input number to calculate it's factorial\n";
		cin>>x;
		cout<<"fact="<<fact(x);
		
		cout<<"*********************************************";
		cout<<"select choice: 0 to exit, 1 to continue:";
		cin>>choice;
	}

	return 0;
}