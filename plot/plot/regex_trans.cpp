#include"stdafx.h"


#include"global_val.h"
#include<regex>
#include<string>
#include<iostream>
#include<cstdlib>
#include<cmath>


float calc(float fl,float fr,char op);

float pop_N(){
	float tmp=N_stack.top();
	N_stack.pop();
	return tmp;
}

void push_N(float n){
	N_stack.push(n);
}

int stack_transform(char*stack,int begin,int end,int&stack_pos){

	
	char tmp[1024]={};

	strncpy_s(tmp,&stack[begin],end-begin+1);

	for(int i=begin;i<=end;i++){
		stack[i]='\0';
	}

	stack_pos=begin;

	std::string s(tmp);

	std::regex pattern("^N(.*)N$");
	std::regex pattern_b("^\\((.*)\\)$");
	std::regex pattern_m("^(-N)$");
	std::match_results<std::string::const_iterator> result;
	bool valid=std::regex_match(s,result,pattern);
	bool valid_b=std::regex_match(s,result,pattern_b);
	bool valid_m=std::regex_match(s,result,pattern_m);

	if(valid){
		std::cout<<"Catch Operator:"<<result[1]<<std::endl;
		float fr=pop_N();
		float fl=pop_N();

		stack[stack_pos]='N';
		stack_pos++;

		char op=result[1].str()[0];
		float f=calc(fl,fr,op);
		push_N(f);
		return 0;
	}

	if(valid_b){
		std::cout<<"Stripe Brancket:"<<result[1]<<std::endl;
		
		stack[stack_pos]='N';
		stack_pos++;

		return 0;
	}

	if(valid_m){
		std::cout<<"Minus Detect:"<<result[1]<<std::endl;
		stack[stack_pos]='N';
		stack_pos++;
		float f=pop_N();
		push_N(-f);
		return 0;
	}

	return -1;
}

int num_transform(char*stack,int begin,int end,int&stack_pos){
	
	char tmp[1024]={};

	strncpy_s(tmp,&stack[begin],end-begin+1);

	for(int i=begin;i<=end;i++){
		stack[i]='\0';
	}

	stack_pos=begin;

	std::string s(tmp);

	std::regex pattern("^(-?\\d*|-?\\d*.\\d*)$");
	std::match_results<std::string::const_iterator> result;
	bool valid=std::regex_match(s,result,pattern);


	if(valid){
		std::cout<<"Catch:"<<result[1]<<std::endl;
	}

	float f=atof(result[1].str().c_str());

	stack[stack_pos]='N';
	stack_pos++;

	push_N(f);

	return 0;
}

float calc(float fl,float fr,char op){
	switch (op)
	{
	case '+':
		return fl+fr;
	case '-':
		return fl-fr;
	case '*':
		return fl*fr;
	case '/':
		if(fr)return fl/fr;
		else return 0;
	case '^':
		return pow(fl,(int)fr);
	case '&':
		if(fl==1)return sin(fr);
		if(fl==2){
			if(fr>0)return log(fr);
			else return 0;
		}
		if(fl==3)return cos(fr);
		if(fl==4)return exp(fr);
		break;
	default:
		break;
	}
}
