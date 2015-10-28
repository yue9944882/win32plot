
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







char str[100]="f(x)=123+44*35+1135";
char*sptr=str;



int main(){
	
/*
	std::string s="-4.412";
	std::regex pattern("(-?\\d*|-?\\d*.\\d*)");
	std::match_results<std::string::const_iterator> result;
	bool valid=std::regex_match(s,result,pattern);

	if(valid){
		std::cout<<"Catch:"<<result[1]<<std::endl;
	}
*/

	while((*sptr)!='='){
		sptr++;
	}sptr++;

	///Until the assignment operation then syntax analysis begin
	
	std::string expr(sptr);
	
	
	
	parse_poly(expr);
	
	std::cout<<"Expression After:"+expr<<std::endl;

	system("pause");
	
	return 0;
}






