#include "sexp.h"
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <list>

using namespace std;

SExp* isSexp(string data);
bool ATOM(SExp& data);
bool INT(SExp& data);
string EQ(SExp& data, SExp& data2);
bool in(SExp& data);
bool in2(SExp& data);
string getVal(SExp& data);
SExp* CAR(SExp& data);
SExp* CDR(SExp& data);
SExp* apply(SExp& data, SExp& dataRemainder);
SExp* CONS(SExp* right, SExp* left);
SExp* evlis(SExp& data);
SExp* evcon(SExp& data);
string Null(SExp& data);
SExp* PLUS(SExp& data);
SExp* MINUS(SExp& data);
SExp* QUOTIENT(SExp& data);
SExp* TIMES(SExp& data);
SExp* REMAINDER(SExp& data);
SExp* LESS(SExp& data);
SExp* GREATER(SExp& data);
void DEFUN(SExp& data);
pair<bool, list<string>> getParams(SExp& data);
SExp* getVal2(SExp& data);

SExp* openSexp(string data, string evaluated, int usedCount);
SExp* integerEval(string data);
SExp* stringEval(string data);
ifstream::pos_type filesize(const char* filename);
SExp* eval(SExp& exp);
list<pair<string, string>> aList;
list<pair<string, string>>::iterator aIter = aList.begin();
list<pair<string, SExp*>> dList;
list<pair<string, SExp*>>::iterator dIter = dList.begin();

SExp* T = new SExp(2, "T");
SExp* NIL = new SExp(2, "NIL");
SExp* car = new SExp(2, "CAR");
SExp* cdr = new SExp(2, "CDR");
SExp* cons = new SExp(2, "CONS");
SExp* atom = new SExp(2, "ATOM");
SExp* null = new SExp(2, "NULL");
SExp* eq = new SExp(2, "EQ");
SExp* quote = new SExp(2, "QUOTE");
SExp* cond = new SExp(2, "COND");
SExp* defun = new SExp(2, "DEFUN");
SExp* Plus = new SExp(2, "PLUS");
SExp* Minus = new SExp(2, "MINUS");
SExp* Times = new SExp(2, "TIMES");
SExp* Quotient = new SExp(2, "QUOTIENT");
SExp* Remainder = new SExp(2, "REMAINDER");
SExp* Less = new SExp(2, "LESS");
SExp* Greater = new SExp(2, "GREATER");

int main(int argc, char** argv){
	if (argc != 2){
		cout<<"Error: Wrong amount of inputs. Need 1 (and only 1) input for file name. "<<endl;
		return 0;
	}
	
	//The following is the loading of initial primitives
	aList.insert(aIter, make_pair("NIL", "NIL"));
	aIter++;
	aList.insert(aIter, make_pair("T","T"));
	aIter++;
	aList.insert(aIter, make_pair("test","42"));
	aIter++;
	aList.insert(aIter, make_pair("CAR","CAR"));
	aIter++;
	aList.insert(aIter, make_pair("CDR","CDR"));
	aIter++;
	aList.insert(aIter, make_pair("CONS","CONS"));
	aIter++;
	aList.insert(aIter, make_pair("ATOM","ATOM"));
	aIter++;
	aList.insert(aIter, make_pair("NULL","NULL"));
	aIter++;
	aList.insert(aIter, make_pair("EQ","EQ"));
	aIter++;
	aList.insert(aIter, make_pair("QUOTE","QUOTE"));
	aIter++;
	aList.insert(aIter, make_pair("COND","COND"));
	aIter++;
	aList.insert(aIter, make_pair("DEFUN","DEFUN"));
	aIter++;
	aList.insert(aIter, make_pair("PLUS","PLUS"));
	aIter++;
	aList.insert(aIter, make_pair("MINUS","MINUS"));
	aIter++;
	aList.insert(aIter, make_pair("TIMES","TIMES"));
	aIter++;
	aList.insert(aIter, make_pair("QUOTIENT","QUOTIENT"));
	aIter++;
	aList.insert(aIter, make_pair("REMAINDER","REMAINDER"));
	aIter++;
	aList.insert(aIter, make_pair("LESS","LESS"));
	aIter++;
	aList.insert(aIter, make_pair("GREATER","GREATER"));
	aIter++;
	
  	ifstream myfile;
	myfile.open(argv[1]);
	bool eofCheck = false;
	while(!eofCheck){
		//while loop for EOF
		char c;
		char check = 'T';
		string testSexp = "";
		char latest = ' ';
		while(myfile.get(c)){
		//read next s_exp
			bool breakcheck = false;
			while(c == '\n'){
				//new line detected- test for new s-exp
				myfile.get(c); 
				if (c == '$'){
					breakcheck = true;
				}
				else if(isalnum(latest)){
					testSexp += '\n';
				}
			}
			if(breakcheck){ //skip adding the $ to string
				break;
			}
			if(c == '\t'){
				continue;
			}
			testSexp = testSexp + c;
			latest = c;
		}
		if(!testSexp.empty()){
			//empty becuase there is nothing left to get with myfile.get
			SExp* s_2_test = isSexp(testSexp);
			if (s_2_test != NULL){
				SExp* eval_exp = eval(*s_2_test);
				if (eval_exp != NULL){
					cout<<eval_exp->print_tree()<<endl;
				}
			}
		}
		else{
			eofCheck = true;
		}
		myfile.get(c);  // if this is the eof there will be another $, else there is a carraige return for next
		//statement which will not be need to be taken care of now for the next iteration
		if (c == '$'){
			eofCheck = true;
		}
	}
	cout<<"end of file\n";
	myfile.close();
	return 0;
}

SExp* eval(SExp& mystring){
	if (ATOM(mystring)){
		if(INT(mystring)){
			if(mystring.getType() == 3){
				return mystring.getLeft();
			}
			else if(mystring.getType() == 2){
				SExp* temp = new SExp(1, getVal(mystring));
				return temp;
			}
			else{
				return &mystring;
			}
		}
		if(in(mystring)){
			SExp* temp = new SExp(2, getVal(mystring));
			return temp;
		}
		else if(!in(mystring)) {
			cout<<"Error in Evaluation: unbound variable: "<<mystring.print_tree()<<endl;
			return NULL;
		}
		if(EQ(mystring,*T) == "true"){
			return T;
		}
		else if(EQ(mystring,*T) == "NULL"){
			return NULL;
		}
		if(EQ(mystring,*NIL) == "true"){
			return NIL;
		}
		else if(EQ(mystring,*NIL) == "NULL"){
			return NULL;
		}
		else{
			cout<<"Error in Evaluation: unbound variable: "<<mystring.print_tree()<<endl;
			return NULL;
		}
	}
	if(ATOM(*CAR(mystring))){
		if(!in2(*CAR(mystring))){
			string check = EQ(*CAR(mystring),*quote);
			if(check == "true"){
				
				SExp* temp = CDR(mystring);
				if(temp->getRight() != NULL){
					return temp;
				}
				return CAR(*temp);  
			}
			else if(check == "NULL"){
				return NULL;
			}
			check = EQ(*CAR(mystring),*cond);
			if(check == "true"){
				return evcon(*CDR(mystring));
			}
			else if(check == "NULL"){
				return NULL;
			}
			check = EQ(*CAR(mystring),*defun);
			if(check == "true"){
				DEFUN(*CDR(mystring));
				return NULL;	//Nothing to evaluate- only adding function to dlist
			}
			else if(check == "NULL"){
				return NULL;
			}
			else{
				
				SExp* temp = evlis(*CDR(mystring));
				if (temp != NULL){
					SExp* out = apply(*CAR(mystring), *temp);
					if(out != NULL){
					}
					return out;
				}
				else{
					return NULL;
				}
			}
		}
		else{
			
			SExp* temp = evlis(*CDR(mystring));
			
			if (temp != NULL){
				SExp* out = apply(*CAR(mystring), *temp);
				if(out != NULL){
				}
				return out;
			}
			else{
				return NULL;
			}
		}
	}
	if(mystring.getRight() == NULL){
		return eval(*CAR(mystring));
	}
	else if(ATOM(*mystring.getRight())){
		if(Null(*(mystring.getRight())) == "true"){
			return CAR(mystring);
		}
	}
	cout<<"Error in Evaluation: Non-atom found in function declaration"<<endl;
	return NULL;
	
}

void DEFUN(SExp& data){
	string name = CAR(data)->getName();
	dList.insert(dIter, make_pair(name, &data));
	dIter++;
	cout<<"Function " + name + " added to dlist"<<endl;
}

pair<bool, list<string>> getParams(SExp& data){
	list<string> mylist;
	list<string>::iterator count = mylist.begin();
	SExp* d2 = &data;
	while(CAR(*d2) != NULL){
		if(CAR(*d2)->getType() == 1){
			cout<<"Error in Evaluation: Number used as parameter in DEFUN"<<endl;
			return make_pair(false,mylist);
		}
		mylist.push_back(CAR(*d2)->getName());
		count++;
		d2 = CDR(*d2);
	}
	if(d2->getType() == 1){
		cout<<"Error in Evaluation: Number used as parameter in DEFUN"<<endl;
		return make_pair(false,mylist);
	}
	mylist.push_back((d2)->getName());
	return make_pair(true,mylist);
}

SExp* evcon(SExp& data){
	if(ATOM(data)){  //This should also catch NIL since that is an atom
		cout<<"Error in Evaluation: Atom found for condition and function in COND evaluation"<<endl;
		return NULL;
	}
	SExp* temp = eval(*CAR(*CAR(data)));
	if(ATOM(*temp)){  //Null will return an error if it is checked right off the bat without confirming it is being evaluated on an atom- make this check first
		string check = Null(*temp);
		if(check == "false"){
			return eval(*CDR(*CAR(data)));
		}
		else if(check == "NULL"){
			return NULL;
		}	
	}
	if(!(ATOM(*temp))){
		return eval(*CDR(*CAR(data)));
	}
	else{
		return evcon(*CDR(data));
	}	
}

SExp* evlis(SExp& data){
	if(ATOM(data)){
		string check = Null(data);
		if(check == "true"){
			return NIL;
		}
		else if(check == "NULL"){
			return NULL;
		}
		return &data;
	}
	SExp* temp = evlis(*CDR(data));
	SExp* temp2 = eval(*CAR(data));
	if (temp != NULL && temp2 != NULL){
		if(ATOM(*temp)){
		}
		return CONS(temp2, temp);
	}
	return NULL;
}

SExp* apply(SExp& data, SExp& f){
	if (ATOM(data)){
		if(!in2(data)){
			if(EQ(data,*car) == "true"){
				SExp* temp = CAR(f);
				if(temp->getType() == 3){
					return (temp->getLeft());  //give me the value, not the NIL at the end for type 3 atoms
				}
				return temp;
			}
			if(EQ(data,*cdr) == "true"){
				SExp* temp = CDR(f);
				if(temp->getType() == 3){
					return temp->getLeft();  //give me the value, not the NIL at the end for type 3 atoms
				}
				return temp;
			}
			if(EQ(data,*cons) == "true"){
				SExp* temp = CDR(f);
				if(!ATOM(*temp)){
					if(temp->getType() == 3){
						return CONS(CAR(f),eval(*temp));  //give me the value, not the NIL at the end	for type 3 atoms	
					}
				}
				return CONS(CAR(f),temp);
			}
			if(EQ(data,*atom) =="true"){
				if(ATOM(f)){
					return T;
				} 
				else{
					return NIL;
				}
			}
			if(EQ(data,*null) =="true"){
				if(Null(f) == "true"){
					return T;
				}
				/*else if(Null(f) == "NULL"){
					return NULL;
				}*/
				else{
					return NIL;
				}
			}
			if(EQ(data,*eq) =="true"){
				string check = EQ(*CAR(f), *CDR(f));
				if(check == "true"){
					return T;
				}
				else{
					if(ATOM(*CAR(f)) && ATOM(*CDR(f))){
						return NIL;
					}
					return NULL;
				}
			}
			if(EQ(data,*Plus) =="true"){
				return PLUS(f);
			}
			if(EQ(data,*Minus) =="true"){
				return MINUS(f);
			}
			if(EQ(data,*Times) =="true"){
				return TIMES(f);
			}
			if(EQ(data,*Quotient) =="true"){
				return QUOTIENT(f);
			}
			if(EQ(data,*Remainder) =="true"){
				return REMAINDER(f);
			}
			if(EQ(data,*Less) =="true"){
				return LESS(f);
			}
			if(EQ(data,*Greater) =="true"){
				return GREATER(f);
			}
		}
		else{	//evaluate some function that was submitted into the dlist
				SExp* temp = getVal2(data);
				list<string> params;
				pair<bool, list<string>> returnvals = getParams(*CAR(*CDR(*temp)));
				if(returnvals.first == false){
					return NULL;
				}
				params = returnvals.second;
				list<string>::iterator newIter;
				SExp* d2 = CAR(f);
				for(newIter = params.begin(); newIter!= params.end(); newIter++){
					if(!ATOM(*d2)){
						if(CAR(*d2)->getType() == 1){
							aList.insert(aIter, make_pair(*newIter,CAR(*d2)->getVal()));
							aIter++;
						}
						else if (in(*CAR(*d2))){
							aList.insert(aIter, make_pair(*newIter,getVal(*CAR(*d2))));
							aIter++;
						}
						else{
							cout<<"Error in Evaluation: Number used as parameter in DEFUN"<<endl;
							return NULL;
						}
						d2 = CDR(*d2);
					}
					else{  //for last atom of parameters
						if((d2)->getType() == 1){
							aList.insert(aIter, make_pair(*newIter,d2->getVal()));
							aIter++;
						}
						else if (in(*d2)){
							aList.insert(aIter, make_pair(*newIter,getVal(*d2)));
							aIter++;
						}
						else{
							cout<<"Error in Evaluation: Number used as parameter in DEFUN"<<endl;
							return NULL;
						}
					}
				}
				SExp* result = eval(*CDR(*CDR(*temp)));
				for(newIter = params.begin(); newIter!= params.end(); newIter++){ //remove temporary alist values
					aList.pop_back();
					aIter--;
				}
				return result;
		}
	}
	cout<<"Error in Evaluation: Non-atom found in function declaration"<<endl;	
	return NULL;
}

SExp* PLUS(SExp& data){
	if(ATOM(*data.getRight())){
		string check = Null(*data.getRight());
		if(check == "false"){
			SExp* first = eval(*CAR(data));
			SExp* second = eval(*CAR(*CDR(data)));
			if(INT(*first) && INT(*second)){
				int value, value2; 
				istringstream (first->getVal()) >> value;
				istringstream (second->getVal()) >> value2;
				SExp* temp = new SExp(1, to_string(value+value2));
				return temp;
			}
			else{
				cout<<"Error in Evaluation: Non-Integer Attempted for use in PLUS"<<endl;
				return NULL;
			}
		}
		else if(check == "NULL"){
			return NULL;
		}
		else{
			cout<<"Error in Evaluation: Not enough inputs for PLUS"<<endl;
			return NULL;
		}
	}
	else{
		string check = Null(*CAR(*data.getRight()));
		if(check == "false"){
			SExp* first = eval(*CAR(data));
			SExp* second = eval(*CAR(*CDR(data)));
			if(INT(*first) && INT(*second)){
				int value, value2; 
				istringstream (first->getVal()) >> value;
				istringstream (second->getVal()) >> value2;
				SExp* temp = new SExp(1, to_string(value+value2));
				return temp;
			}
			else{
				cout<<"Error in Evaluation: Non-Integer Attempted for use in PLUS"<<endl;
				return NULL;
			}
		}
		else if(check == "NULL"){
			return NULL;
		}
		else{
			cout<<"Error in Evaluation: Not enough inputs for PLUS"<<endl;
			return NULL;
		}
	}
}

SExp* MINUS(SExp& data){
	if(ATOM(*data.getRight())){
		string check = Null(*data.getRight());
		if(check == "false"){
			SExp* first = eval(*CAR(data));
			SExp* second = eval(*CAR(*CDR(data)));
			if(INT(*first) && INT(*second)){
				int value, value2; 
				istringstream (first->getVal()) >> value;
				istringstream (second->getVal()) >> value2;
				SExp* temp = new SExp(1, to_string(value-value2));
				return temp;
			}
			else{
				cout<<"Error in Evaluation: Non-Integer Attempted for use in MINUS"<<endl;
				return NULL;
			}
		}
		else if(check == "NULL"){
			return NULL;
		}
		else{
			cout<<"Error in Evaluation: Not enough inputs for MINUS"<<endl;
			return NULL;
		}
	}
	else{
		string check = Null(*CAR(*data.getRight()));
		if(check == "false"){
			SExp* first = eval(*CAR(data));
			SExp* second = eval(*CAR(*CDR(data)));
			if(INT(*first) && INT(*second)){
				int value, value2; 
				istringstream (first->getVal()) >> value;
				istringstream (second->getVal()) >> value2;
				SExp* temp = new SExp(1, to_string(value-value2));
				return temp;
			}
			else{
				cout<<"Error in Evaluation: Non-Integer Attempted for use in MINUS"<<endl;
				return NULL;
			}
		}
		else if(check == "NULL"){
			return NULL;
		}
		else{
			cout<<"Error in Evaluation: Not enough inputs for MINUS"<<endl;
			return NULL;
		}
	}
}

SExp* TIMES(SExp& data){
	if(ATOM(*data.getRight())){
		string check = Null(*data.getRight());
		if(check == "false"){
			SExp* first = eval(*CAR(data));
			SExp* second = eval(*CAR(*CDR(data)));
			if(INT(*first) && INT(*second)){
				int value, value2; 
				istringstream (first->getVal()) >> value;
				istringstream (second->getVal()) >> value2;
				SExp* temp = new SExp(1, to_string(value*value2));
				return temp;
			}
			else{
				cout<<"Error in Evaluation: Non-Integer Attempted for use in TIMES"<<endl;
				return NULL;
			}
		}
		else if(check == "NULL"){
			return NULL;
		}
		else{
			cout<<"Error in Evaluation: Not enough inputs for TIMES"<<endl;
			return NULL;
		}
	}
	else{
		string check = Null(*CAR(*data.getRight()));
		if(check == "false"){
			SExp* first = eval(*CAR(data));
			SExp* second = eval(*CAR(*CDR(data)));
			if(INT(*first) && INT(*second)){
				int value, value2; 
				istringstream (first->getVal()) >> value;
				istringstream (second->getVal()) >> value2;
				SExp* temp = new SExp(1, to_string(value*value2));
				return temp;
			}
			else{
				cout<<"Error in Evaluation: Non-Integer Attempted for use in TIMES"<<endl;
				return NULL;
			}
		}
		else if(check == "NULL"){
			return NULL;
		}
		else{
			cout<<"Error in Evaluation: Not enough inputs for TIMES"<<endl;
			return NULL;
		}
	}
}

SExp* QUOTIENT(SExp& data){
	if(ATOM(*data.getRight())){
		string check = Null(*data.getRight());
		if(check == "false"){
			SExp* first = eval(*CAR(data));
			SExp* second = eval(*CAR(*CDR(data)));
			if(INT(*first) && INT(*second)){
				int value, value2; 
				istringstream (first->getVal()) >> value;
				istringstream (second->getVal()) >> value2;
				SExp* temp = new SExp(1, to_string(value/value2));
				return temp;
			}
			else{
				cout<<"Error in Evaluation: Non-Integer Attempted for use in QUOTIENT"<<endl;
				return NULL;
			}
		}
		else if(check == "NULL"){
			return NULL;
		}
		else{
			cout<<"Error in Evaluation: Not enough inputs for QUOTIENT"<<endl;
			return NULL;
		}
	}
	else{
		string check = Null(*CAR(*data.getRight()));
		if(check == "false"){
			SExp* first = eval(*CAR(data));
			SExp* second = eval(*CAR(*CDR(data)));
			if(INT(*first) && INT(*second)){
				int value, value2; 
				istringstream (first->getVal()) >> value;
				istringstream (second->getVal()) >> value2;
				SExp* temp = new SExp(1, to_string(value/value2));
				return temp;
			}
			else{
				cout<<"Error in Evaluation: Non-Integer Attempted for use in QUOTIENT"<<endl;
				return NULL;
			}
		}
		else if(check == "NULL"){
			return NULL;
		}
		else{
			cout<<"Error in Evaluation: Not enough inputs for QUOTIENT"<<endl;
			return NULL;
		}
	}
}

SExp* REMAINDER(SExp& data){
	if(ATOM(*data.getRight())){
		string check = Null(*data.getRight());
		if(check == "false"){
			SExp* first = eval(*CAR(data));
			SExp* second = eval(*CAR(*CDR(data)));
			if(INT(*first) && INT(*second)){
				int value, value2; 
				istringstream (first->getVal()) >> value;
				istringstream (second->getVal()) >> value2;
				SExp* temp = new SExp(1, to_string(value%value2));
				return temp;
			}
			else{
				cout<<"Error in Evaluation: Non-Integer Attempted for use in REMAINDER"<<endl;
				return NULL;
			}
		}
		else if(check == "NULL"){
			return NULL;
		}
		else{
			cout<<"Error in Evaluation: Not enough inputs for REMAINDER"<<endl;
			return NULL;
		}
	}
	else{
		string check = Null(*CAR(*data.getRight()));
		if(check == "false"){
			SExp* first = eval(*CAR(data));
			SExp* second = eval(*CAR(*CDR(data)));
			if(INT(*first) && INT(*second)){
				int value, value2; 
				istringstream (first->getVal()) >> value;
				istringstream (second->getVal()) >> value2;
				SExp* temp = new SExp(1, to_string(value%value2));
				return temp;
			}
			else{
				cout<<"Error in Evaluation: Non-Integer Attempted for use in REMAINDER"<<endl;
				return NULL;
			}
		}
		else if(check == "NULL"){
			return NULL;
		}
		else{
			cout<<"Error in Evaluation: Not enough inputs for REMAINDER"<<endl;
			return NULL;
		}
	}
}

SExp* LESS(SExp& data){
	if(ATOM(*data.getRight())){
		string check = Null(*data.getRight());
		if(check == "false"){
			SExp* first = eval(*CAR(data));
			SExp* second = eval(*CAR(*CDR(data)));
			if(INT(*first) && INT(*second)){
				int value, value2; 
				istringstream (first->getVal()) >> value;
				istringstream (second->getVal()) >> value2;
				if(value<value2){
					return T;
				}
				else{
					return NIL;
				}
			}
			else{
				cout<<"Error in Evaluation: Non-Integer Attempted for use in LESS"<<endl;
				return NULL;
			}
		}
		else if(check == "NULL"){
			return NULL;
		}
		else{
			cout<<"Error in Evaluation: Not enough inputs for LESS"<<endl;
			return NULL;
		}
	}
	else{
		string check = Null(*CAR(*data.getRight()));
		if(check == "false"){
			SExp* first = eval(*CAR(data));
			SExp* second = eval(*CAR(*CDR(data)));
			if(INT(*first) && INT(*second)){
				int value, value2; 
				istringstream (first->getVal()) >> value;
				istringstream (second->getVal()) >> value2;
				if(value<value2){
					return T;
				}
				else{
					return NIL;
				}
			}
			else{
				cout<<"Error in Evaluation: Non-Integer Attempted for use in LESS"<<endl;
				return NULL;
			}
		}
		else if(check == "NULL"){
			return NULL;
		}
		else{
			cout<<"Error in Evaluation: Not enough inputs for LESS"<<endl;
			return NULL;
		}
	}
}

SExp* GREATER(SExp& data){
	if(ATOM(*data.getRight())){
		string check = Null(*data.getRight());
		if(check == "false"){
			SExp* first = eval(*CAR(data));
			SExp* second = eval(*CAR(*CDR(data)));
			if(INT(*first) && INT(*second)){
				int value, value2; 
				istringstream (first->getVal()) >> value;
				istringstream (second->getVal()) >> value2;
				if(value>value2){
					return T;
				}
				else{
					return NIL;
				}
			}
			else{
				cout<<"Error in Evaluation: Non-Integer Attempted for use in GREATER"<<endl;
				return NULL;
			}
		}
		else if(check == "NULL"){
			return NULL;
		}
		else{
			cout<<"Error in Evaluation: Not enough inputs for GREATER"<<endl;
			return NULL;
		}
	}
	else{
		string check = Null(*CAR(*data.getRight()));
		if(check == "false"){
			SExp* first = eval(*CAR(data));
			SExp* second = eval(*CAR(*CDR(data)));
			if(INT(*first) && INT(*second)){
				int value, value2; 
				istringstream (first->getVal()) >> value;
				istringstream (second->getVal()) >> value2;
				if(value>value2){
					return T;
				}
				else{
					return NIL;
				}
			}
			else{
				cout<<"Error in Evaluation: Non-Integer Attempted for use in GREATER"<<endl;
				return NULL;
			}
		}
		else if(check == "NULL"){
			return NULL;
		}
		else{
			cout<<"Error in Evaluation: Not enough inputs for GREATER"<<endl;
			return NULL;
		}
	}
}

SExp* CAR(SExp& data){
	return data.getLeft();
}

SExp* CDR(SExp& data){

	if (data.getType() == 3 && data.getRight() == NULL){
		return NIL;
	}
	return data.getRight();
}

string getVal(SExp& data){
	int type = data.getType();
	list<pair<string, string>>::iterator newIter;
	for(newIter = aList.begin(); newIter!= aList.end(); newIter++){
		if(type == 2 && newIter->first == data.getName()){
			return newIter->second;
		}
		else if(type == 3 && newIter->first == data.getLeft()->getName()){
			return newIter->second;
		}
	}
}

SExp* CONS(SExp* data, SExp* data2){
	SExp* temp = new SExp();
	temp->setLeft(data);
	data->setParent(temp);
	temp->setRight(data2);
	data2->setParent(temp);
	return temp;		
}

string Null(SExp& data){
	string check = EQ(data,*NIL);
	if(check == "true"){
		return "true";
	}
	else if (check == "NULL"){
		return "NULL";
	}
	return "false";
}

bool in(SExp& data){
	list<pair<string, string>>::iterator newIter;
	int type = data.getType();
	for(newIter = aList.begin(); newIter!= aList.end(); newIter++){
		if(type == 2 && newIter->first == data.getName()){
			return true;
		}
		else if(type == 3 && newIter->first == data.getLeft()->getName()){
			return true;
		}
	}
	return false;
}

bool in2(SExp& data){
	list<pair<string, SExp*>>::iterator newIter;
	int type = data.getType();
	for(newIter = dList.begin(); newIter!= dList.end(); newIter++){
		if(type == 2 && newIter->first == data.getName()){
			return true;
		}
	}
	return false;
}

SExp* getVal2(SExp& data){
	int type = data.getType();
	list<pair<string, SExp*>>::iterator newIter;
	for(newIter = dList.begin(); newIter!= dList.end(); newIter++){
		if(type == 2 && newIter->first == data.getName()){
			return newIter->second;
		}
	}
}

bool equal(SExp& data, SExp& data2){
	if(ATOM(data)){
		if(ATOM(data2)){
			//return EQ(data, data2);
			return true;
		}
	}
	return false;
}

string EQ(SExp& data, SExp& data2){
	//only compares atoms
	if(ATOM(data) && ATOM(data2)){
		if(ATOM(data)){
			int type = data.getType();
			int type2 = data2.getType();
			if(type == type2){
				if (type == 1){
					if(data.getVal() == data2.getVal()){
						return "true";
					}
					else{
						return "false";
					}
				}
				else{
					if(in(data) && in(data2)){\
						if(getVal(data) == getVal(data2)){
							return "true";
						}
						else{
							return "false";
						}
					}
					else{
						cout<<"Error in Evaluation1: unbound variable "<<data2.print_tree()<<endl;
						return "NULL";
					}
				}
			}
			else if (type == 3 && type2 != 3){
				if (type2 == 1){
					if(data.getLeft()->getVal() == data2.getVal()){
						return "true";
					}
					else if(data.getLeft()->getType() == 2){
						if(in(*data.getLeft())){
							if(data2.getVal() == getVal(*data.getLeft())){
								return "true";
							}
						}
						else{
							cout<<"Error in Evaluation2: unbound variable "<<endl;
							return "NULL";
						}
					}
					else{
						return "false";
					}
				}
				else if(data.getLeft()->getType() == 1){
					return "false";
				}
				else{	//type2 == 2
					if(in(*data.getLeft()) && in(data2)){\
						if(getVal(*data.getLeft()) == getVal(data2)){
							return "true";
						}
						else{
							return "false";
						}
					}
					else{
						cout<<"Error in Evaluation3: unbound variable "<<endl;
						return "NULL";
					}
				}
			}
			else if (type != 3 && type2 == 3){
				if (type == 1){
					if(data.getVal() == data2.getLeft()->getVal()){
						return "true";
					}
					else if(data2.getLeft()->getType() == 2){
						if(in(*data2.getLeft())){
							if(data.getVal() == getVal(*data2.getLeft())){
								return "true";
							}
						}
						else{
							cout<<"Error in Evaluation4: unbound variable "<<endl;
							return "NULL";
						}
					}
					else{
						return "false";
					}
				}
				else if(data2.getLeft()->getType() == 1){
					return "false";
				}
				else{
					if(in(data) && in(*data2.getLeft())){\
						if(getVal(data) == getVal(*data2.getLeft())){
							return "true";
						}
						else{
							return "false";
						}
					}
					else{
						cout<<"Error in Evaluation5: unbound variable "<<in(data)<<endl;
						return "NULL";
					}
				}
			}
			else if (type == 1 && type2 == 2){
				if(in(data2)){
					if(data.getVal() == getVal(data2)){
						return "true";
					}
					else{
						return "false";
					}
				}
				else{
					cout<<"Error in Evaluation6: unbound variable "<<in(data)<<endl;
					return "NULL";
				}
			}
			else if (type == 2 && type2 == 1){
				if(in(data)){
					if(data2.getVal() == getVal(data)){
						return "true";
					}
					else{
						return "false";
					}
				}
				else{
					cout<<"Error in Evaluation7: unbound variable "<<in(data)<<endl;
					return "NULL";
				}
			}
		}
	}
	cout<<"Error in Evaluation: Non-atom found in comparison function 'EQ'"<<endl;	
	return "NULL";
}

bool INT(SExp& data){
	if(data.getType() == 1){
		return true;
	}
	if(data.getType() == 2){
		if(in(data)){
			return true;
		}
	}
	else if (data.getType() == 3){
		if (data.getParent() == NULL && data.getRight() == NULL && data.getLeft()->getType() == 1){
			return true;
		}		
	}
	else{
		cout<<data.getType()<<endl;
		return false;
	}	
}

bool ATOM(SExp& data){
	if(data.getType() != 3){
		return true;
	}
	else if (data.getRight() == NULL && data.getLeft()->getType() != 3){
		return true;
	}	
	else{
		return false;
	}	
}

SExp* isSexp(string data){
	//check if data is single atom, or s-exp inside ()
	if(data[0] == '('){
		string inData = data.erase(0,1);
		return openSexp(inData,"",0);
	}
	else if(data[0] == '\n'){
		isSexp(data.erase(0,1));
	}
	else if(isdigit(data[0]) || data[0] == '-'){
		return integerEval(data);
	}
	else{
		return stringEval(data);
	}
	
}

SExp* openSexp(string data, string evaluated, int usedCount){
	int layer = 0;
	SExp* top = new SExp(true);
	SExp* current = top;
	int dataLength = data.length();
	int evalLength = evaluated.length();
	//string outstring = "(";
	for (int i = evalLength;i<dataLength;i++){
		if(data[i] == '\n'){
			data.erase(i,1);
			i--;
		}
		if(data[i] == ')'){
		//end of s-expr reached
			if(layer == 0){
				/*if(i == dataLength-1){
				//check if final element
					cout<<top->print_tree()<<endl;
					return;
				}*/
			//	else{
				for(int j = i+1;j<dataLength;j++){
					if(data[j] == '\n' || data[j] == '\t' || data[j] == ' '){
					//check if only remaining "elements" are just carriage returns
						data.erase(j,1);
						dataLength -= 1;
						continue;
						}
					else{
						cout<<"Error: not an S-Expression: Value "<<data[j]<<" found after final )"<<endl;
						return NULL;
					}
				}
				return top;
				//}
			}
			else{	
				if(current->getRight() == NULL){
					SExp* temp = current->getParent();
					SExp* temp2 = current->getLeft();
					temp2->setParent(temp);
					if(temp->getRight() == current){
						temp->setRight(temp2);	
					}
					else{
						temp->setLeft(temp2);	
					}
				}
				current = current->getParent();
				layer --;
				if(current->getLayer() != layer){
					i--;
					layer++;
				}
			}
		}
		else if(isdigit(data[i]) || data[i] == '-'){
			string outstring, extrastring;
			outstring += data[i];
			i += 1;		
			while(data[i] != '.' && data[i] != ')' && data[i] != ' '){
				if(isdigit(data[i])){
					outstring += data[i];
					i++;
				}
				else if(data[i] == '\n'){
					data.at(i) = ' ';
				}
				else{
					cout<<"Error: not an S-Expression: Non-digit "<<data<<" found."<<endl;
					return NULL;
				}
			}
			if (outstring.length() == 1 && data[i-1] == '-'){
				//lone negative sign detected: not an int
				cout<<"Error: not an S-Expression: '-' found without any following numbers for an int."<<endl;
				return NULL;
			}
			SExp* temp = new SExp(1, outstring, current, layer);
			if(current->getLeft() != NULL){
				current->setRight(temp);
				//current = current->getParent();
			}
			else{
				current->setLeft(temp);
			}
			i--;
		}
		else if (isalpha(data[i])){
			string outstring, extrastring;
			outstring += data[i];
			i += 1;		
			while(data[i] != '.' && data[i] != ')' && data[i] != ' '){
				if(isalnum(data[i])){
					outstring += data[i];
					i++;
				}
				else if(data[i] == '\n'){
					data.at(i) = ' ';
				}
				else{
					cout<<"Error: not an S-Expression: Non alphanumeric value found in string"<<endl;
					return NULL;
				}
			}
			SExp* temp = new SExp(2, outstring, current, layer);
			if(current->getLeft() != NULL){
				current->setRight(temp);
				//current = current->getParent();
				
			}
			else{
				current->setLeft(temp);
			}
			i--;
		}
		else if (data[i] == ' '){
			// this implementation assumes spaces are only for lists and not just for user aesthetics
			if(current->getLeft() == NULL){
				//just an extra space so do nothing
				continue;
			}
			else if(data[i+1] == ' ' || data[i+1] == '.'){
				data.erase(i,1);
				dataLength -= 1;
			}
			else if (current->getRight() != NULL){
				do{
					if(current == top){
						cout<<"Error: not an S-Expression: Trying to add to list after tree is already full"<<endl;
						return NULL;
					}
					//the only case where there isnt a parent is current = top 
					current = current->getParent();
					if (current->getRight() == NULL){
						
						
						SExp* temp = new SExp(current, layer);
						current->setRight(temp);
						current = temp;
						break;
					}
				}while(true);
			}
			else{
				
				SExp* temp = new SExp(current, layer);
				current->setRight(temp);
				current = temp;
			}
		}
		else if (data[i] == '('){
			if(current->getLeft() == NULL){
				//left side of ()
				layer ++;
				SExp* temp = new SExp(current, layer);
				current->setLeft(temp);
				current = temp;
			}
			else if(current->getRight() == NULL){
				//after a .
				layer ++;
				SExp* temp = new SExp(current, layer);
				current->setRight(temp);
				current = temp;
			}
			else{
				//after a ' ' and in a list
				cout<<"Error: not an S-Expression: '(' found out of place"<<endl;
				return NULL;
			}
		}
		else if (data[i] == '.'){
			//left tree must be full and right tree empty or else error	
			if(current->getLeft() == NULL){
				cout<<"Error: not an S-Expression: '.' produced before left sub-tree is full"<<endl;
				return NULL;
			}
			else if(current->getRight() != NULL){
				cout<<"Error: not an S-Expression: '.' produced when right sub-tree is already full"<<endl;
				return NULL;
			}
			else if(!isalnum(data[i+1]) && data[i+1] != '(' && data[i+1] != ' '){
				//make sure next token is actually a value of set of values in case of '('
				cout<<"Error: not an S-Expression: '.' not followed by a right subtree "<<data[i+1]<<endl;
				return NULL;
			}
			else if(data[i+1] == ' '){
				while(data[i+1] == ' '){
					// remove any spaces after a .
					data.erase(i+1,1);
					dataLength -= 1;
				}
			}
		}
		else{
			cout<<"Error: not an S-Expression: Bad token received inside of s-expression"<<endl;
			return NULL;
		}
	}
	cout<<"Error: not an S-Expression: Statement never completed with )"<<endl;
	return NULL;
}

SExp* integerEval(string data){
	if(data[0] == '-'){
		//is first digit a "-" and the only digit
		if (data.length() <= 1){
			cout<<"Error: not an S-Expression: '-' sign found without any following numbers"<<endl;
			return NULL;
		}
	}
	for(int i = 1;i<data.length();i++){
		if(isdigit(data[i])){
			continue;
		}
		else if(data[i] == '\n'){
		//erase carriage returns
			data.erase(i,1);
		}
		else if(data[i] == ' ' && !isalnum(data[i+1])){
			//detect if space there to separate a list (which shouldnt happen in an atom) or space just there for aesthetics
			data.erase(i,1);
		}
		else{
			cout<<"Error: not an S-Expression: Non-numeric value: "<<data[i]<<" found in middle of number"<<endl;
			return NULL;
		}		
	}
	//cout<<"("<<data<<".NIL)"<<endl;  //part 1 output
	SExp* output = new SExp(1, data);
	return output;
	//return "("+ data + ".NIL)";
}

ifstream::pos_type filesize(const char* filename){
    		ifstream in(filename, ifstream::ate | ifstream::binary);
   		return in.tellg(); 
}

SExp* stringEval(string data){
	//make sure first element is letter
	if(!isalpha(data[0])){
		cout<<"Error: not an S-Expression: Non-alphabetical value "<<data[0]<<" found to lead string"<<endl;
		return NULL;	
	}
	for(int i = 1;i<data.length();i++){
		if(isalnum(data[i])){
			continue;
		}
		else if(data[i] == '\n'){
		//erase carriage returns
			data.erase(i,1);
		}
		else if(data[i] == ' ' && !isalnum(data[i+1])){
			//detect if space there to separate a list (which shouldnt happen in an atom) or space just there for aesthetics
			data.erase(i,1);
		}
		else{
			cout<<"Error: not an S-Expression: Symbol/Space found in atom"<<endl;
			return NULL;
		}	
	}
	//cout<<"("<<data<<".NIL)"<<endl; //part 1 output
	SExp* output = new SExp(2, data);
	return output;
}
