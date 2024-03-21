	/**
		stack class and test application
	*/

	class Record{
	public:
		Record(int);
		int val;
		Record* next;
	};

	Record::Record(int val){
		this->val=val;
		next=NULL;
	}

	class Stack{
	private:
		Record* head;
	public:
		Stack();
		Record* push(Record* rec);
		Record* pop();
		void print();
	};

	Stack::Stack(){
		cout<<"stack created";
		head=NULL;

	}

	Record* Stack::push(Record* rec){
		rec->next=head;
		head=rec;
		return head;
	}

	Record* Stack::pop(){
		Record* temp=head;
		if(head!=NULL)
			head=head->next;
		return head;
	}

	void Stack::print(){	
		Record* temp=head;
		while(temp!=NULL){
			cout<<temp->val;
			temp=temp->next;
		}
	}

	int main(){
		
		Stack* stack=new Stack();
		for(int i=0;i<10;++i){
			stack.push(new Record(i));
		}
		cout<<"stack before poping:";
		stack->print();
		cout<<"\nstack after poping;";
		stack.pop();
		stack.pop();
		stack.print();

		int temp=0;
		cin>>temp;
	}