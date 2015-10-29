#include"parse_poly\parse_poly\inter_change.h"
#include"parse_poly\parse_poly\parse.h"

#include<string>
#include<stack>
#include<vector>
#include<iterator>
#include<iostream>
#include<cctype>
#include<cstring>
#include<cstdlib>
#include<stack>
#include<map>
#include<regex>


// Module for Polynomial Analysis

char str[100]="f(x)=sin(sin(x))";
char*sptr=str;



int main(){

	while((*sptr)!='='){
		sptr++;
	}sptr++;

	///Until the assignment operation then syntax analysis begin
	
	std::string s(sptr);

	change_fx(s);

	change_x(s,1);

	float ret=parse_poly(s);


	std::cout<<"Expression After:"<<ret<<std::endl;
	

	system("pause");
	
	return 0;
}






