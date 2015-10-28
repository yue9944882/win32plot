
#include"TypeDef.h"

#include<string>
#include<stack>
#include<vector>

#include<iterator>
#include<iostream>
#include<cctype>
#include<cstdlib>
#include<stack>


// Module for Polynomial Analysis


//** Function: Syntax Tree Building
//** Input : String type polynomial
//** Output: A binary tree describing the structure of polynomial




char str[100]="f(x)=1+x";

char*sptr=str;


int main(){
	
	while((*sptr)!='='){
		sptr++;
	}sptr++;
	///Until the assignment operation then syntax analysis begin
	
	std::string expr(sptr);
	
	
	
	std::cout<<"Expression After:"+expr<<std::endl;
	
	system("pause");
	
	return 0;
}


int parse_poly(std::string expr){
	
	
}


