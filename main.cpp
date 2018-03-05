#include "sexp.h"
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

bool isSexp(string data);
bool isAtom(char data, bool firstEle);
void openSexp(string data, string evaluated, int usedCount);
bool integerEval(string data);
bool stringEval(string data);

using namespace std;

int main(int argc, char** argv){
	if (argc != 2){
		cout<<"Error: Wrong amount of inputs. Need 1 (and only 1) input for file name. "<<endl;
		return 0;
	}
  	ifstream myfile;
	myfile.open(argv[1]);
	bool eofCheck = false;
	while(!eofCheck){
		//while loop for EOF
		char c;
		char check = 'T';
		string testSexp = "";
		while((myfile.get(c))){
		//read next s_exp
			bool breakcheck = false;
			while(c == '\n'){
				//new line detected- test for new s-exp
				myfile.get(c); 
				if (c == '$'){
					breakcheck = true;
				}
				else if(isalnum(testSexp.back())){
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
		}
		isSexp(testSexp);
		
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

bool isSexp(string data){
	//check if data is single atom, or s-exp inside ()
	if(data[0] == '('){
		string inData = data.erase(0,1);
		openSexp(inData,"",0);
	}
	else if(data[0] == '\n'){
		isSexp(data.erase(0,1));
	}
	else if(isdigit(data[0]) || data[0] == '-'){
		integerEval(data);
	}
	else{
		stringEval(data);
	}
	
}

bool isAtom(char data, bool firstEle){
	//check if first element
	if(!firstEle){
		return isalnum(data);
	}
	else{
	//check if character is "-" in the case of -1
		if(data != '-'){
			return isalnum(data);
		}	
		else{
			return true;
		}
	}	
}

void openSexp(string data, string evaluated, int usedCount){
	int layer = 0;
	SExp* top = new SExp();
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
						return;
					}
				}
				cout<<top->print_tree()<<endl;
				return;
				//}
			}
			else{
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
					return;
				}
			}
			if (outstring.length() == 1 && data[i-1] == '-'){
				//lone negative sign detected: not an int
				cout<<"Error: not an S-Expression: '-' found without any following numbers for an int."<<endl;
				return;
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
					return;
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
						return;
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
				return;
			}
		}
		else if (data[i] == '.'){
			//left tree must be full and right tree empty or else error
			//cout<<data[i+1]<<endl;		
			if(current->getLeft() == NULL){
				cout<<"Error: not an S-Expression: '.' produced before left sub-tree is full"<<endl;
				return;
			}
			else if(current->getRight() != NULL){
				cout<<"Error: not an S-Expression: '.' produced when right sub-tree is already full"<<endl;
				return;
			}
			else if(!isalnum(data[i+1]) && data[i+1] != '(' && data[i+1] != ' '){
				//make sure next token is actually a value of set of values in case of '('
				cout<<"Error: not an S-Expression: '.' not followed by a right subtree "<<data[i+1]<<endl;
				return;
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
			return;
		}
	}
	cout<<"Error: not an S-Expression: Statement never completed with )"<<endl;
}

bool integerEval(string data){
	if(data[0] == '-'){
		//is first digit a "-" and the only digit
		if (data.length() <= 1){
			cout<<"Error: not an S-Expression: '-' sign found without any following numbers"<<endl;
			return false;
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
			return false;
		}		
	}
	cout<<"("<<data<<".NIL)"<<endl;
	return true;
}

bool stringEval(string data){
	//make sure first element is letter
	if(!isalpha(data[0])){
		cout<<"Error: not an S-Expression: Non-alphabetical value "<<data[0]<<" found to lead string"<<endl;
		return false;	
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
			return false;
		}	
	}
	cout<<"("<<data<<".NIL)"<<endl;
	return true;
}
