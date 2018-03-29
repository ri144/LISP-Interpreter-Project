#include <string>
#include <iostream>

#pragma once

#ifndef _SEXP_H_
#define _SEXP_H_

using namespace std;



class SExp{
	private:
		int type;
		string val;
		string name;
		SExp* left;
		SExp* right;
		SExp* parent;
		int layer;
		bool isTop = false;
	public:
		SExp(int, string, SExp*&, int);
		SExp(SExp& , SExp&, SExp);		
		SExp(SExp*&, int);
		SExp();
		SExp(bool);
		SExp(int, string);
		void setRight(SExp*& inRight);
		void setLeft(SExp*& inLeft);
		void setParent(SExp*& inParent);
		void setType(int intype);
		int getLayer();
		SExp* getParent();
		SExp* getLeft();
		SExp* getRight();
		int getType();
		string getVal();
		string getName();
		string print_tree();
	};

SExp::SExp(int initType, string value, SExp*& initParent, int initLayer){
	if(initType == 1){
		type = 1;
		val = value;
		parent = initParent;
		layer = initLayer;
	}
	else if(initType == 2){
		type = 2;
		name = value;
		parent = initParent;
		layer = initLayer;
	}
	else{
		cout<<"Error: wrong initType associated with the input value"<<endl;
	}
}

SExp::SExp(SExp& inLeft, SExp& inRight, SExp initParent){
	type = 3;
	*left = inLeft;
	*right = inRight;
	*parent = initParent;
}	

SExp::SExp(SExp*& initParent, int initLayer){
	type = 3;
	parent = initParent;
	left = NULL;
	right = NULL;
	layer = initLayer;
}

SExp::SExp(){
	type = 3;
	parent = NULL;
	left = NULL;
	right = NULL;
	layer = 0;
}

SExp::SExp(bool flag){
	//used for top layer/ atom
	isTop = flag;
	type = 3;
	parent = NULL;
	left = NULL;
	right = NULL;
	layer = 0;
}

SExp::SExp(int initType, string value){
	//used for top layer
	type = initType;
	val = value;
	name = value;
	parent = NULL;
	left = NULL;
	right = NULL;
	layer = 0;
}

void SExp::setRight(SExp*& inRight){
	right = inRight;
}

void SExp::setLeft(SExp*& inLeft){
	left = inLeft;
}

void SExp::setParent(SExp*& inParent){
	parent = inParent;
}

void SExp::setType(int intype){
	type = intype;
}


SExp* SExp::getLeft(){
	return left;
}

SExp* SExp::getRight(){
	return right;
}

SExp* SExp::getParent(){
	return parent;
}

int SExp::getType(){
	return type;
}

string SExp::getVal(){
	return val;
}

string SExp::getName(){
	return name;
}

string SExp::print_tree(){
	if (type == 1){
		return val;
	}
	else if (type == 2){
		return name;
	}
	else{
		if(left == NULL && right == NULL)
			return "(NIL)";	
		else if(left == NULL){
			//should never get here
			return "Error: not an S-Expression: Left side of tree empty but right side has data";
		}
		else if(right == NULL) // must be just an atom
			return left->print_tree();
		else
			return "("+left->print_tree() + "." + right->print_tree() + ")";
	}
}

int SExp::getLayer(){
	return layer;
}

#endif
